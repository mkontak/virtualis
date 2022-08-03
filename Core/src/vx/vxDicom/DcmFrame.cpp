#include "Stdafx.h"
#include "DcmTriple.h"
#include "DcmFrame.h"
#include "DcmExceptions.h"
#include "DcmElementCollection.h"
#include "DcmImage.h"
#include "DcmSequence.h"
#include "DcmIndex.h"
#include "DcmIndexList.h"


using namespace dcm;

/**
 * Constructor
 *
 * Default
 */
 DcmFrame::DcmFrame() : 
 IDcmImage(true), 
 m_pDcmImage(__nullptr),
 m_iFrameNumber(0)
 { 
 }


 /**
  * Constructor 
  *
  * Used for wheh the frame is part of an image
  *
  * @param iFrameNumber       Associated frame number
  * @param pDcmImage          Image this fram is from
  * @param frameElements      Frame elements collection
  *
  */
 DcmFrame::DcmFrame(unsigned int iFrameNumber, DcmImage * pDcmImage, std::shared_ptr<DcmElementCollection> & frameElements) : 
 IDcmImage(true), 
 m_pDcmImage(pDcmImage),
 m_iFrameNumber(iFrameNumber),
 m_iFrameIndex(iFrameNumber-1)
 { 
 }

 /**
  * Gets the elements for the main image
  */
 std::shared_ptr<DcmElementCollection> & DcmFrame::GetElements()
 { 
   return m_pDcmImage->GetElements(); 
 }


/**
 * Clones the image
 *
 *  @return the pointer of clone
 */
std::shared_ptr<IDcmImage> DcmFrame::Clone()
{
  
  // Create using the internal dataset
  std::shared_ptr<DcmFrame> frame(new DcmFrame());

  // Make sure the image was allocated
  if ( frame == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocation memory for DcmFrame","DcmImage","Clone"));

  frame->m_pDcmImage = m_pDcmImage;
  frame->m_pixelData = m_pixelData;
  frame->m_iFrameNumber = m_iFrameNumber;

  return std::dynamic_pointer_cast<IDcmImage>(frame);
  
} // Clone()


/**
 * Trys to get the specified element under the specified sequence 
 *
 * @param sequenceTag       Sequence to search for first if 0 then ignore
 * @param elementTag        Element to search for
 * @param element           Return parameter
 * @param searchSubElements Search for sub elements
 *
 * @return    True if found, false if not
 */
bool DcmFrame::TryGetElement(const DcmTag & sequenceTag, const DcmTag & elementTag, std::shared_ptr<dcm::DcmElement> & element, bool searchSubElement )
{

  std::vector<std::shared_ptr<DcmElementCollection>> frameElements = m_pDcmImage->GetFrameElements();

  bool bFound( frameElements.size() > 0 &&  frameElements[m_iFrameIndex]->TryGetElement(sequenceTag, elementTag, element, searchSubElement) );

  if ( ! bFound )
  {
    bFound = GetElements()->TryGetElement(sequenceTag, elementTag, element, searchSubElement) ;
  }
 
  return bFound;

} // TryGteElement(const DcmTag & sequenceTag, const DcmTag & elementTag, std::shared_ptr<DcmElement> & element, bool searchSubElement = false )


/**
 * Tries to get the value of the tag specfied
 *
 * @param tag                 Tag to lookup
 * @param str                 Return string value
 * @param searchSubelements   Flag to indicat that we are to search the subselements
 *
 * @return True if found, false if not
 */
bool DcmFrame::TryGetString(const  DcmTag & elementTag, std::string & str, bool searchSubElement)
{

  std::shared_ptr<DcmElement> element;

  std::vector<std::shared_ptr<DcmElementCollection>> frameElements = m_pDcmImage->GetFrameElements();

  bool bFound( frameElements.size() > 0 &&  frameElements[m_iFrameIndex]->TryGetElement(elementTag, element, searchSubElement) );

  if ( ! bFound )
  {
    bFound = GetElements()->TryGetElement(elementTag, element, searchSubElement) ;
  }
 
  if ( bFound )
     str = element->GetValue<std::string>();

  return bFound;
} // TryGetValue(const  DcmTag & tag, std::string & str, bool searchSubelements)



/**
 *
 */
bool DcmFrame::TryGetBuffer(const DcmTag & tag, std::shared_ptr<DcmBuffer> & dcmBuffer, bool searchSubElements )
{

  bool found = false;


  if ( tag == DCM_PixelData )
  {
    found = true;
    dcmBuffer = m_pixelData;
  }
  else
  {
    found = this->m_pDcmImage->TryGetValue<std::shared_ptr<DcmBuffer>>(tag, dcmBuffer, searchSubElements);
  }


  return found;

} // TryGetValue(const DcmTag & tag, std::shared_ptr<DcmBuffer> dcmBuffer, bool searchSubElements )


/**
 * Gets the filename associated with the image
 */
const std::string & DcmFrame::GetFilename()
{
  return m_pDcmImage->GetFilename();
} // 



/**
 * Get the Dimensional
 *
 * @return vector of indexes
 */
std::shared_ptr<DcmIndexList> DcmFrame::GetIndexes()
{

  std::shared_ptr<DcmIndexList> indexes(DcmIndexList::Create());

  std::shared_ptr<DcmElementCollection> elementCollection(GetElements());

  std::vector<std::shared_ptr<DcmElementCollection>> frames = elementCollection->GetFrames();


  std::shared_ptr<DcmElement> element;

  // ===========================================================================================================
  // (0020,9111) FrameContentSequence  
  //
  // Identifies general characteristics of this frame
  //
  // ===========================================================================================================
  if ( elementCollection->TryGetElement(DCM_FrameContentSequence, element) )
  {

    indexes = DcmIndexList::Create(DCM_FrameContentSequence);

    /// cast the element to a sequence
    std::shared_ptr<dcm::DcmSequence> sequence(std::static_pointer_cast<dcm::DcmSequence>(element));

    sequence->Reset();

      
    while ( sequence->MoveNext() )
    {
 
      std::shared_ptr<dcm::DcmElementCollection> collection(sequence->Current());

      collection->Reset();

      while ( collection->MoveNext() )
      {
        indexes->Add(DcmIndex::Create(collection->Current()->GetDcmTag()));
      }
        
 
    } // END ... For each sequence item


  }

  return indexes;

} // GetIndexes()

