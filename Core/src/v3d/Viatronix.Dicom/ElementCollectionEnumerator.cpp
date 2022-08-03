#include "Stdafx.h"
#include "ElementCollectionEnumerator.h"
#include "IElementCollection.h"




USING_DICOM_NS

/**
 * Constructor
 *
 * @param mpCollection      Element collection to enunerator over
 */
ElementCollectionEnumerator::ElementCollectionEnumerator(IElementCollection ^ mpCollection) :
m_mpCollection(mpCollection)
{
  Reset();
} // ElementCollectionEnumerator(ElementCollection ^ mpCollection)


/**
 * Moves the enumerator to the next
 *
 * @return True if we are not at the end false if we are
 */
bool ElementCollectionEnumerator::MoveNext()
{
  return m_mpCollection->MoveNext();
} // MoveNExt()



/**
 * Resets the enumerator to the next
 *
 */
void ElementCollectionEnumerator::Reset()
{
  m_mpCollection->Reset();
} // Reset()


/**
 * Gets the current object
 */
Element ^ ElementCollectionEnumerator::Current::get()
{
  return m_mpCollection->Current;
} // Current