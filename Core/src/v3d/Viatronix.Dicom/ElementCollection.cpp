#include "Stdafx.h"
#include "Utility.h"
#include "ElementCollection.h"
#include "Element.h"
#include "Sequence.h"
#include "Buffer.h"
#include "ElementCollectionEnumerator.h"
#include "Group.h"

using namespace System;
using namespace System::IO;
using namespace System::Xml;
using namespace System::Text;
using namespace System::Drawing;
using namespace System::ComponentModel;
using namespace System::Drawing::Imaging;
using namespace System::Collections;

USING_DICOM_NS

#pragma region constructors/destructors

/**
 * Constructor
 */
ElementCollection::ElementCollection() :
m_bIsReadOnly(false),
m_pElementCollection(scoped_ptr<std::shared_ptr<dcm::DcmElementCollection>>(new std::shared_ptr<dcm::DcmElementCollection>(dcm::DcmElementCollection::Create())))
{
  
} // ElementCollection()


/**
 * Destructor
 */
ElementCollection::~ElementCollection()
{
  this->!ElementCollection();
} // ~ElementCollection()
  
/**
 * Finalizer
 */
ElementCollection::!ElementCollection()
{
  m_pElementCollection.reset(__nullptr);
} // !ElementCollection


#pragma endregion

#pragma region properties

/**
 * Gets the study date and time.
 */
DateTime ElementCollection::StudyDateTime::get()
{
  return Viatronix::Dicom::Utility::ToDateTime(StudyDate, StudyTime);
} // StudyDateTime::get()


/**
 * Gets the series date and time.
 */
DateTime ElementCollection::SeriesDateTime::get()
{
  return Viatronix::Dicom::Utility::ToDateTime(SeriesDate, SeriesTime);
} // SeriesDateTime::get()


/**
 * Gets the serailzie header
 */
String ^ ElementCollection::SerializedHeader::get()
{
  return gcnew String((*m_pElementCollection)->GetSerializedHeader().c_str());
} 

/**
 * Gets the current element
 *
 * @return Current element
 */
Element ^ ElementCollection::Current::get()
{
  std::shared_ptr<dcm::DcmElement> * pElement(new std::shared_ptr<dcm::DcmElement>((*m_pElementCollection)->Current()));

  if ( pElement->get() == __nullptr )
    return nullptr;
  else if ( (*pElement)->IsSequence() )
    return Sequence::Create(System::IntPtr((void *)(pElement)));
  else
    return Element::Create(System::IntPtr((void *)(pElement)));
} // Current

/**
 * Gets the count of elements
 */
int ElementCollection::Count::get()
{
  return (*m_pElementCollection)->Count();
} // COunt


/**
 * Gets the synchronized flag
 */
bool ElementCollection::IsSynchronized::get()
{
  return true;
} // Synchronized



/**
 * Gets the synchronized flag
 */
Object ^ ElementCollection::SyncRoot::get()
{
  return nullptr;
} // SyncRoot


 
/// Gets/Sets the name
unsigned short ElementCollection::Level::get()
{
   return(*m_pElementCollection)->GetLevel();
} // Name

#pragma endregion

#pragma region methods


#pragma region create methods

/**
 * Creates an empty element list
 */
ElementCollection ^ ElementCollection::Create()
{


  ElementCollection ^ mpElementCollection(gcnew ElementCollection());

  if ( mpElementCollection == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocated ElementCollection object");

  return mpElementCollection;

} // Create(String ^ msHeader)



/**
 * Creates the element list from the header
 *
 * @param msHeader
 */
ElementCollection ^ ElementCollection::Create(String ^ msParameer, ParameterType type)
{


  ElementCollection ^ mpElementCollection(Create());

  mpElementCollection->m_pElementCollection.reset( new std::shared_ptr<dcm::DcmElementCollection>(dcm::DcmElementCollection::Create(ss(msParameer), (dcm::DcmParameterType)type)));
 

  return mpElementCollection;
} // Create(String ^ msHeader)



/**
 * Creates the element list from the header
 *
 * @param msHeader
 */
ElementCollection ^ ElementCollection::Create(XmlNode ^ mpNode)
{


  ElementCollection ^ mpElementCollection(Create());

  

  mpElementCollection->m_pElementCollection.reset( new std::shared_ptr<dcm::DcmElementCollection>(dcm::DcmElementCollection::Create(ss(mpNode->OuterXml), dcm::DcmParameterType::PT_HEADER)));
 

  return mpElementCollection;
} // Create(String ^ msHeader)



/**
 * Creates the element list using the unmanaged DcmElementPtr
 *
 * @param pDcmElementCollection unmanaged shared_ptr<>
 */
ElementCollection ^ ElementCollection::Create(System::IntPtr dcmElemntListPtr)
{
  ElementCollection ^ mpElementCollection(Create());

  mpElementCollection->m_pElementCollection.reset( (std::shared_ptr<dcm::DcmElementCollection> *)(dcmElemntListPtr.ToPointer()));

  return mpElementCollection;

} // Create(System::IntPtr pDcmElemntListPtr)

#pragma endregion


#pragma region enumberator implementation


/**
 * Continues the iteration
 *
 * @return next element or null if none
 */
void ElementCollection::Reset()
{
  (*m_pElementCollection)->Reset();
} // Reset()


/**
 * Clears the collection
 */
void ElementCollection::Clear()
{
  (*m_pElementCollection)->Clear();

} // Clear()

/**
 * Moves to the next item in the list
 */
bool ElementCollection::MoveNext()
{
  return ( (*m_pElementCollection)->MoveNext() );
 } // MoveNext()


/**
 * Gets the enumerator
 */
Generic::IEnumerator<Element ^> ^ ElementCollection::GetEnumerator()
{
  return gcnew ElementCollectionEnumerator(this);
} // GetEnumerator()


/**
 * Copies collection to array
 */
void ElementCollection::CopyTo(array<Element ^> ^ mpArray, int index)
{

  Reset();

  int i(-1);

  while ( MoveNext() )
  {

    if ( ++i >= index )
      mpArray[i] = this->Current;
  }

  
} // CopyTo()


/** 
 * Adds the specified element to the list if it does not exist. 
 *
 * @param mpElement     Element to add
 */
void ElementCollection::Add(Element ^ mpElement)
{
 
 if ( ! Contains(mpElement) )
 {
   (*m_pElementCollection)->Add(  *((std::shared_ptr<dcm::DcmElement> *)(mpElement->GetDcmElementPtr().ToPointer())) );
 }
} // Insert(Element ^ mpElement)

/** 
 * Removes the specified element to the list if it does not exist. 
 *
 * @param mpElement     Element to remove
 */
bool ElementCollection::Remove(Element ^ mpElement)
{
 return (*m_pElementCollection)->Remove(  *((std::shared_ptr<dcm::DcmElement> *)(mpElement->GetDcmElementPtr().ToPointer())) );
} // Insert(Element ^ mpElement)




/** 
 * Determines in the collection contains the element
 *
 * @param mpElement     Element to check for
 *
 * @return Returns the true if the element is in the list, flase othersie
 */
bool ElementCollection::Contains(unsigned short groupId, unsigned short elementId)
{

  return (*m_pElementCollection)->Contains(DcmTag(groupId, elementId));

} // Contains(Element ^ mpElement)

/** 
 * Determines in the collection contains the element
 *
 * @param mpElement     Element to check for
 *
 * @return Returns the true if the element is in the list, flase othersie
 */
inline bool ElementCollection::Contains(Element ^ mpElement)
{

  return Contains(mpElement->GroupId, mpElement->ElementId);

} // Contains(Element ^ mpElement)


#pragma endregion


/**
 * Gets the unmanaged object
 */
System::IntPtr ElementCollection::GetDcmElementCollectionPtr()
{
  return System::IntPtr((void *)(m_pElementCollection.get()));
} // GetDcmElementCollectionPtr()

/**
 * Writes the element list as xml to the files specified
 *
 * @param msFilePath      File to write to
 */
void ElementCollection::WriteXml(String ^ msFilePath)
{
   return(*m_pElementCollection)->WriteXml(ss(msFilePath));

} // WriteXml(String ^ msFilePath)

/**
 * Gets the list of groups this element list is associated with
 *
 * @return List of groups
 */
Generic::List<Group ^> ^ ElementCollection::GetGroups()
{
  Generic::List<Group ^> ^ mpGroups = gcnew Generic::List<Group ^>();

  dcm::DcmGroupMap groups = (*m_pElementCollection)->GetGroups();

  for ( dcm::DcmGroupMap::iterator it(groups.begin()); it != groups.end(); it++ )
  {
    mpGroups->Add(gcnew Group((*it).second.GetId(), gcnew String((*it).second.GetDescription().c_str())));
  }

  return mpGroups;
} // GetGroups()



/** 
 * tray and get the specified element
 *
 * @param iGroup    [in]    Group id
 * @param iElement  [in]    Field id
 * @param mpElement [out]   Element to return
 *
 * @return true if found, false otherwise
 */
bool ElementCollection::TryGetElement(unsigned short iGroup, unsigned short iElement, Element ^% mpElement)
{

  bool bFound = false;

  std::shared_ptr<dcm::DcmElement> * pElement = new std::shared_ptr<dcm::DcmElement>(__nullptr);
  
  if ( ( bFound = (*m_pElementCollection)->TryGetElement(DcmTag(iGroup,iElement), *pElement, true) ) == true )
  {
    if ( (*pElement)->IsSequence() )
      mpElement = Viatronix::Dicom::Sequence::Create(System::IntPtr((void *)(pElement)));
    else
      mpElement = Element::Create(System::IntPtr((void *)pElement));
  }

  return bFound;

} // TryGetElement(unsigned short iGroup, unsigned short iElement, Element ^% mpElement)


#pragma endregion