#include "Stdafx.h"
#include "ImageCollectionEnumerator.h"
#include "Imagecollection.h"




USING_DICOM_NS

/**
 * Constructor
 *
 * @param mpCollection      Element collection to enunerator over
 */
ImageCollectionEnumerator::ImageCollectionEnumerator(ImageCollection ^ mpCollection) :
m_mpCollection(mpCollection)
{
  Reset();
} // ImageCollectionEnumerator(ElementCollection ^ mpCollection)


/**
 * Moves the enumerator to the next
 *
 * @return True if we are not at the end false if we are
 */
bool ImageCollectionEnumerator::MoveNext()
{
  return m_mpCollection->MoveNext();
} // MoveNExt()



/**
 * Resets the enumerator to the next
 *
 */
void ImageCollectionEnumerator::Reset()
{
  m_mpCollection->Reset();
} // Reset()


/**
 * Gets the current object
 */
IImage ^ ImageCollectionEnumerator::Current::get()
{
  return m_mpCollection->Current;
} // Current