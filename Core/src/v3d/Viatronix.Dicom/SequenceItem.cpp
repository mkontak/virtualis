#include "Stdafx.h"
#include "SequenceItem.h"
#include "Sequence.h"
#include "Element.h"
#include "ElementCollectionEnumerator.h"


using namespace System::Text;
USING_DICOM_NS


#pragma region constructrs/destructors

/**
 * Constructor
 */
SequenceItem::SequenceItem() :
ElementCollection()
{
  m_pElementCollection.reset( __nullptr );
    
}  // Sequence()


/**
 * Constructor
 */
SequenceItem::SequenceItem(unsigned int iCard) :
ElementCollection(),
m_iCard(iCard)
{
  m_pElementCollection = scoped_ptr<std::shared_ptr<dcm::DcmSequenceItem>>(new std::shared_ptr<dcm::DcmSequenceItem>(dcm::DcmSequenceItem::Create(iCard)));
} // Sequence()


/**
 * Destructor
 */
SequenceItem::~SequenceItem()
{
  this->!SequenceItem();
} // ~SequenceItem()
  
/**
 * Finalizer
 */
SequenceItem::!SequenceItem()
{
  
} // !SequenceItem


#pragma endregion


#pragma region methods


#pragma region methods

#pragma region create methods


/**
 * Creates a sequence item
 * 
 * @param iCard   Sequence item card number
 *
 * @return SequenceItem
 */
SequenceItem ^ SequenceItem::Create(unsigned int iCard)
{

  SequenceItem ^ mpSequenceItem(gcnew SequenceItem(iCard));

  if ( mpSequenceItem == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate SequenceItem object");

  return mpSequenceItem;

} //  Create(unsigned int iCard)

/**
 * Creates a sequence item
 * 
 * @param dcmSequenceItemPtr   Unmanaged eequence item
 *
 * @return SequenceItem
 */
SequenceItem ^ SequenceItem::Create(System::IntPtr dcmSequenceItemPtr)
{

  SequenceItem ^ mpSequenceItem(gcnew SequenceItem());

  if ( mpSequenceItem == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate SequenceItem object");

  // =================================================================================================================================
  // double cast to make sure nothing craps out. The unmanaged IntPtr is suppose to be std::shared_ptr<dcm::DcmSequenceItem> but
  // the SequenceItem inheris ElementCollection so we recast to the std::shared_ptr<dcm::DcmElementCollection> which the scoped_ptr 
  // is expecting.
  // ===================================================================================================================================
  mpSequenceItem->m_pElementCollection.reset( (std::shared_ptr<dcm::DcmElementCollection> *)((std::shared_ptr<dcm::DcmSequenceItem> *)(dcmSequenceItemPtr.ToPointer())) );

  return mpSequenceItem;

} //  Create(unsigned int iCard)



#pragma endregion

/**
 * Gets the interanl unmanaged std::shared_ptr<dcm::DcmSequenceIte> *
 */
System::IntPtr SequenceItem::GetDcmSequenceItemPtr()
{
  return System::IntPtr( (std::shared_ptr<dcm::DcmSequenceItem> *)m_pElementCollection.get() );
} // GetDcmSequenceItemPtr()




#pragma endregion