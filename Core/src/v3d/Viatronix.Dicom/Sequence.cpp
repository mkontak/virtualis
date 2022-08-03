#include "Stdafx.h"
#include "Element.h"
#include "Sequence.h"
#include "ElementCollection.h"
#include "SequenceEnumerator.h"


using namespace System::Text;
USING_DICOM_NS

#pragma region constuctors/destructors

/**
 * Constructor
 */
Sequence::Sequence() :
Element()
{
 
} // Sequence()

/**
 * Constructor
 */
Sequence::Sequence(unsigned short iGroup, unsigned short iElement) :
Element(iGroup, iElement, String::Empty),
m_bIsReadOnly(false)
{
  m_pElement.reset(new std::shared_ptr<dcm::DcmElement>(dcm::DcmSequence::Create(DcmTag(iGroup,iElement))));
} // Sequence()


/**
 * Destructor
 */
Sequence::~Sequence()
{
  this->!Sequence();
} // ~Sequence()
  
/**
 * Finalizer
 */
Sequence::!Sequence()
{
   
} // !Sequence


#pragma endregion


#pragma region method



#pragma region create methods

 /**
  *  Create the sequence
  *
  * @param iGroup      Group tag
  * @param iElement     Element tag
  *
  * @return Sequence
  */
Sequence ^ Sequence::Create(unsigned short iGroup, unsigned short iElement)
{

  Sequence ^ mpSequence(gcnew Sequence(iGroup, iElement));

  if ( mpSequence == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Sequence object");

  return mpSequence;
} // Create(unsigned short iGroup, unsigned short iElement)

/**
 * Create the sequence
 *
 * @param dcmSequencePtr      Unmanaged object pointer
 *
 * @return Sequence
 */
Sequence ^ Sequence::Create(System::IntPtr dcmSequencePtr)
{

  Sequence ^ mpSequence(gcnew Sequence());

  if ( mpSequence == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Sequence object");

  mpSequence->m_pElement.reset((std::shared_ptr<dcm::DcmElement> *)(dcmSequencePtr.ToPointer()));

  return mpSequence;

} // Create(System::IntPtr dcmSequencePtr)


#pragma endregion


#pragma region enumeration/iteration methods


/**
 * Continues the iteration
 *
 * @return next element or null if none
 */
void Sequence::Reset()
{
  ((std::dynamic_pointer_cast<dcm::DcmSequence> )(*m_pElement))->Reset();
} // Reset()


/**
 * Copies collection to array
 */
void Sequence::CopyTo(array<ElementCollection ^>  ^ mpArray, int index)
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
 * @param mpSequenceItem     Element to add
 */
void Sequence::Add(ElementCollection ^ mpCollection)
{
 
 ((std::dynamic_pointer_cast<dcm::DcmSequence> )(*m_pElement))->Add(  *((std::shared_ptr<dcm::DcmElementCollection> *)(mpCollection->GetDcmElementCollectionPtr().ToPointer())) );

} // Insert(Element ^ mpElement)

/** 
 * Removes the specified element to the list if it does not exist. 
 *
 * @param mpSequenceItem     Element to remove
 */
bool Sequence::Remove(ElementCollection ^ mpCollection)
{
 return  false;
} // Remove(Element ^ mpElement)



/** 
 * Determines in the collection contains the element
 *
 * @param mpSequenceItem     Element to check for
 *
 * @return Returns the true if the element is in the list, flase othersie
 */
bool Sequence::Contains(ElementCollection ^ mpCollection)
{
  return false;
} // Contains(Element ^ mpElement)
/**
 * Clears the collection
 */
void Sequence::Clear()
{
  ((std::dynamic_pointer_cast<dcm::DcmSequence> )(*m_pElement))->Clear();
} // Clear()

/**
 * Moves to the next item in the list
 */
bool Sequence::MoveNext()
{
  return(((std::dynamic_pointer_cast<dcm::DcmSequence> )(*m_pElement))->MoveNext());
} // MoveNext()


/**
 * Gets the enumerator
 */
Generic::IEnumerator<ElementCollection ^> ^ Sequence::GetEnumerator()
{
  return gcnew SequenceEnumerator(this);
} // GetEnumerator()

#pragma endregion


#pragma endregion



#pragma region properties

/**
 * Gets the current element
 *
 * @return Current element
 */
ElementCollection ^ Sequence::Current::get()
{
  std::shared_ptr<dcm::DcmElementCollection> * pCollection(new std::shared_ptr<dcm::DcmElementCollection>(((std::dynamic_pointer_cast<dcm::DcmSequence> )(*m_pElement))->Current()));

  if ( pCollection->get() == __nullptr )
    return nullptr;
  else 
    return ElementCollection::Create(System::IntPtr((void *)(pCollection)));
} // Current

/**
 * Gets the count of elements
 */
int Sequence::Count::get()
{
  return ((std::dynamic_pointer_cast<dcm::DcmSequence> )(*m_pElement))->Count();
} // COunt


/**
 * Gets the synchronized flag
 */
bool Sequence::IsSynchronized::get()
{
  return true;
} // Synchronized



/**
 * Gets the synchronized flag
 */
Object ^ Sequence::SyncRoot::get()
{
  return nullptr;
} // SyncRoot


#pragma endregion
