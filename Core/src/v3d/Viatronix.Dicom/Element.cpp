#include "Stdafx.h"
#include "Element.h"
#include "Utility.h"
//#include "Tag.h"

using namespace System;
using namespace System::Text;
using namespace System::Xml;
using namespace System::Windows;

USING_DICOM_NS

#pragma region constructors/destructors

  /**
 * Constructors
 */
Element::Element() :
m_pElement(scoped_ptr<std::shared_ptr<dcm::DcmElement>>(__nullptr)),
m_foreColor(System::Windows::Media::Brushes::Black)
{

} // Element()

/**
 * Constructors
 */
Element::Element(unsigned short iGroup, unsigned short iElement, String ^ msValue) :
m_pElement(scoped_ptr<std::shared_ptr<dcm::DcmElement>>(new std::shared_ptr<dcm::DcmElement>(dcm::DcmElement::Create(DcmTag(iGroup,iElement),ss(msValue))))),
m_foreColor(System::Windows::Media::Brushes::Black)
{

} // Element()


/**
 * Destructor
 */
Element::~Element()
{
  this->!Element();
} // ~Element()
  
/**
 * Finalizer
 */
Element::!Element()
{
  m_pElement.reset(__nullptr);
} // !Element


#pragma endregion


/**
 * Returns the string representation of the element
 */
String ^ Element::ToString()
{
  return gcnew String((*m_pElement)->ToString().c_str());
} // ToString()

/**
 * Create the element
 *
 * @param iGroup      Group id
 * @param iElement    Element id
 * @param msValue     Value
 *
 * @return Element 
 */
Element ^ Element::Create(unsigned short iGroup, unsigned short iElement, String ^ msValue)
{

  Element ^ mpElement(gcnew Element(iGroup, iElement, msValue));

  if ( mpElement == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Element object");

  return mpElement;

} // Create(unsigned short iGroup, unsigned short iElement, String ^ sValue)



/**
 * Create the element
 *
 * @param dcmElementPtr    
 *
 * @return Element 
 */
Element ^ Element::Create(System::IntPtr dcmElementPtr)
{

  Element ^ mpElement(gcnew Element());

  if ( mpElement == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Element object");

  mpElement->m_pElement.reset((std::shared_ptr<dcm::DcmElement> *)(dcmElementPtr.ToPointer()));

  return mpElement;

} // Create(System::IntPtr dcmElementPtr)


/**
 * Returns the std::shared_ptr 
 */
System::IntPtr Element::GetDcmElementPtr()
{
  return System::IntPtr((void *)(m_pElement.get()));
} // GetDcmElementPtr()

#pragma region IElement

/**
 * Gets the tag
 */
String ^ Element::Tag::get()
{
  return gcnew String((*m_pElement)->GetTag().c_str());
}

/**
 * Gets the ID for this element
 */
int Element::Id::get()
{
  return (*m_pElement)->GetId();
} // Id

/**
 * Gets the group id
 */
unsigned short Element::GroupId::get()
{
  return (*m_pElement)->GetGroup();
}

/**
 * Gets the element id
 */
unsigned short Element::ElementId::get()
{
  return (*m_pElement)->GetElement();
}


// ===============================
// Gets the Value as a string
// ==============================
String ^ Element::Value::get()
{
   return gcnew String((*m_pElement)->GetValue().c_str());
} // Value

// ===============================
// Gets the Value as a string
// ==============================
void Element::Value::set(String ^ msValue)
{
   return(*m_pElement)->SetValue(ss(msValue));
   RaisePropertyChanged("Value");

} // Value

 
/// Gets/Sets the name
String ^ Element::Name::get()
{
   return gcnew String((*m_pElement)->GetName().c_str());
} // Name

/// Gets/Sets the VR
String ^ Element::VR::get()
{
  return gcnew String((*m_pElement)->GetVR().c_str());  
} // VR

/// Gets/Sets the Length
unsigned long Element::Length::get()
{
  return (*m_pElement)->GetLength();
} // Length

/// Gets/Sets the VM
unsigned short Element::VM::get()
{
  return (*m_pElement)->GetVM();
} // VM

/// Gets/Sets the private creator for this element
String ^ Element::PrivateCreator::get()
{
  return gcnew String((*m_pElement)->GetPrivateCreator().c_str());
} // PrivateCreator

/**
 * Gets the sequence flag
 */
bool Element::IsSequence::get()
{
  return (*m_pElement)->IsSequence();
} // IsSequence


/**
 * Gets the sub-elements
 */
bool Element::HasSubElements::get()
{
  return (*m_pElement)->HasSubElements();
} // HasSubElements


/**
 * Gets the private flag
 */
bool Element::IsPrivate::get()
{
  return (*m_pElement)->IsPrivate();
} // IsPrivate


/**
 * Gets the sub-element flag
 */
bool Element::IsSubElement::get()
{
  return (*m_pElement)->IsSubElement();
} // IsSubElement

 
/// Gets/Sets the name
unsigned short Element::Level::get()
{
   return (*m_pElement)->GetLevel();
} // Name


/// Gets/Sets the name
bool Element::Expanded::get()
{
   return (*m_pElement)->GetExpanded();
} // Name


/// Gets/Sets the name
void Element::Expanded::set(bool bExpanded)
{
   (*m_pElement)->SetExpanded(bExpanded);
   RaisePropertyChanged("Expanded");
} // Name
#pragma region endregion