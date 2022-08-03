#include "Stdafx.h"
#include "SequenceEnumerator.h"
#include "Sequence.h"




USING_DICOM_NS

/**
 * Constructor
 *
 * @param mpCollection      Element collection to enunerator over
 */
SequenceEnumerator::SequenceEnumerator(Sequence ^ mpSequence) :
m_mpSequence(mpSequence)
{
  Reset();
} // ElementCollectionEnumerator(ElementCollection ^ mpCollection)


/**
 * Moves the enumerator to the next
 *
 * @return True if we are not at the end false if we are
 */
bool SequenceEnumerator::MoveNext()
{
  return m_mpSequence->MoveNext();
} // MoveNExt()



/**
 * Resets the enumerator to the next
 *
 */
void SequenceEnumerator::Reset()
{
  m_mpSequence->Reset();
} // Reset()


/**
 * Gets the current object
 */
ElementCollection ^ SequenceEnumerator::Current::get()
{
  return m_mpSequence->Current;
} // Current