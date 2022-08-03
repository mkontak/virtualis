// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmImage.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmFrame.h"
#include "DcmImage.h"
#include "DcmExceptions.h"
#include "DcmUtility.h"
#include "DcmCosines.h"
#include "DcmTriple.h"
#include "DcmDouble.h"
#include "DcmDate.h"
#include "DcmTime.h"
#include "DcmBuffer.h"
#include "DcmLog.h"
#include "DcmName.h"
#include "DcmElement.h"
#include "DcmDictionary.h"
#include "DcmElementCollection.h"
#include "DcmSequence.h"
#include "DcmImageCollection.h"
#include "DcmImageFactory.h"
#include "DcmIndex.h"
#include "DcmIndexList.h"
#include "DcmGdiplusEngine.h"

#include "dcmimgle/dcmimage.h"
#include "dcmdata/dctk.h"
#include "dcmdata/dcpxitem.h"


#include <regex>

using namespace dcm;


// Sort List ( UID,  array ( GROUP_TAG, ELEMENT_TAG ) )
typedef  std::pair<const DcmTag,const DcmTag> SORTITEM;
typedef  std::vector<SORTITEM> SORTITEM_LIST;
typedef  std::map<std::string, SORTITEM_LIST> SORTMAP;

#pragma region constructors/destructors/clones

/**
 * Constructor
 *
 *  @param bEmpty
 */
DcmImage::DcmImage(bool bEmpty) :
IDcmImage(false),
m_fileFormat(DcmFileFormat()),
m_pDcmDataset(m_fileFormat.getDataset()),
m_elements(__nullptr),
 m_frames(DcmImageCollection::Create()),
m_sFilename("")
{

  // Generate a UID for this instance
  if ( ! bEmpty )
  {
    SetValue<std::string>(DCM_SOPInstanceUID,  DcmUtility::GenerateUid() );

    // Set the current date and time for this instance
    SetValue<dcm::DcmDate>(DCM_InstanceCreationDate, dcm::DcmDate::Now());
    SetValue<dcm::DcmTime>(DCM_InstanceCreationTime, dcm::DcmTime::Now());
  }

} // DcmImage()


/**
 * Constructor
 *
 *  @param pDcmDataset
 */
DcmImage::DcmImage( DcmDataset * pDcmDataset) :
IDcmImage(false),
m_fileFormat(DcmFileFormat(pDcmDataset)),
m_pDcmDataset(m_fileFormat.getDataset()),
m_elements(__nullptr),
 m_frames(DcmImageCollection::Create()),
m_sFilename("")
{
} // DcmImage(DcmDataset * pDcmDataset)


/**
 * Destructor
 */
DcmImage::~DcmImage()
{
  // Nothing to do
} // ~DcmImage()


/**
 * Clones the image
 *
 *  @return the pointer of clone
 */
std::shared_ptr<IDcmImage> DcmImage::Clone()
{
  
  // Create using the internal dataset
  std::shared_ptr<DcmImage> pDcmImage(new DcmImage(m_pDcmDataset));

  // Make sure the image was allocated
  if ( pDcmImage == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocation memory for DcmImage","DcmImage","Clone"));

  return std::dynamic_pointer_cast<IDcmImage>(pDcmImage);
  
} // Clone()


#pragma endregion


/**
 * Creates a new frame for this image
 *
 * @return frame
 */
std::shared_ptr<DcmFrame> DcmImage::CreateFrame(unsigned int iFrameNumber, std::shared_ptr<DcmElementCollection> & frameElements, std::shared_ptr<DcmBuffer> & framePixelData)
{

  std::shared_ptr<DcmFrame> frame = std::shared_ptr<DcmFrame>(new DcmFrame(iFrameNumber, this, frameElements));

  if ( frame == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmFrame object","DcmImage","Create"));

  frame->m_pixelData = framePixelData;

  return frame;
} // CreateFrame()

/**
 * Creates a new frame for this image
 *
 * @return frame
 */
std::shared_ptr<DcmFrame> DcmImage::CreateFrame(std::shared_ptr<DcmElementCollection> & frameElements, std::shared_ptr<DcmBuffer> & framePixelData)
{
  unsigned int iFrameNumber(GetNumberOfFrames() + 1);

  std::shared_ptr<DcmFrame> frame = CreateFrame(iFrameNumber, frameElements, framePixelData);

  /// Set the frame number
  SetNumberOfFrames(iFrameNumber);

  std::shared_ptr<DcmBuffer> pixelData(GetPixelData());

  pixelData->Append(framePixelData);

  return frame;
} // CreateFrame()


/**
 * Extracts the Multi-Frame image collections from with-in enhanced image
 */
const std::shared_ptr<DcmImageCollection> & DcmImage::GetFrames()
{


  if ( m_frames->Count() == 0 && GetNumberOfFrames() > 0 )
  {

    /// Set the frame collections series uid
    m_frames->m_sSeriesUid = GetSeriesInstanceUid();

    /// Get the elements for this image
    std::shared_ptr<DcmElementCollection> pDcmElementCollection = GetElements();

    /// Get the multi-frame element list so we know how many datasets exists
    DcmElementCollectionList frameElementCollections = pDcmElementCollection->GetFrames();

    /// Get the pixel data for this image which needs to be devided into frames
    std::shared_ptr<DcmBuffer> pixelData = GetPixelData();

    // =======================================================
    // Only need to process if we have multiple element lists
    // =======================================================
    if ( frameElementCollections.size() > 0 )
    {


      size_t iOffset(0);    // Pixel data offset
      int iFrameNumber(1);  // frame counter (indexer)


      // =======================================================================
      // For each element list collection create the multilist using the sorting 
      // ========================================================================
      std::for_each(frameElementCollections.begin(), frameElementCollections.end(), 
                    [ & ] 
                    ( std::shared_ptr<DcmElementCollection> frameElements )
      { 


        
        /// Create a frame for this image
        std::shared_ptr<DcmFrame> frame = CreateFrame(iFrameNumber++, frameElements); 

        // Get the frmae's size 
        const unsigned int iFrameSize(frame->GetImageSize<unsigned char>());

        // Allocate a new frame buffer for the pixel data
        std::shared_ptr<DcmBuffer> frameBuffer(DcmBuffer::Create(iFrameSize));

        // Copy the frame out of the image's pixel data into the new frame buffer
        pixelData->CopyTo(frameBuffer,iFrameSize, iOffset);

        // Set the frames pixel data to the new buffer
        frame->SetPixelData(frameBuffer);

        // Move the pixel data's offset to the next frame
        iOffset += iFrameSize;
       
        /// Insert the frame into the collection
        m_frames->Add(std::dynamic_pointer_cast<IDcmImage>(frame));

        

      } );

      // =======================================================================================================================================
      // Setup the frame collections indexes. If the current images contains the DimensionIndexSequence with no defined indexes then I will 
      // check for thr DimenisionIndexValue in the first frame. If the frame does not have this multi-valued field then use the first frames
      // FrameContentSequence which more than likly contains the timing of the frame acquisition.
      // =========================================================================================================================================
      std::shared_ptr<DcmElement> element;
      std::shared_ptr<DcmIndexList> indexes(GetIndexes());

      std::shared_ptr<IDcmImage> firstFrame(m_frames->GetFirstImage());
      if ( indexes->Count() == 0 && 
           ! firstFrame->TryGetElement(indexes->GetMultiValuedIndex()->GetFunctionalGroup(), indexes->GetMultiValuedIndex()->GetIndexElement(), element, true ) ) 
      {
        if ( firstFrame->TryGetElement(DCM_FrameContentSequence, element) )
        {
          indexes = DcmIndexList::Create("FrameContent");

          std::shared_ptr<DcmSequence> sequence = std::dynamic_pointer_cast<DcmSequence>(element);

          sequence->Reset();

          while ( sequence->MoveNext() )
          {
            indexes->Add(sequence->Current());
          }

        }
      }

      // Set the frames indexes
      m_frames->m_indexes = indexes;

    }

  } // END ... If there are no frames and the number of frames is > 0

  return m_frames;

} // GetFrames()



/**
 * Get the Dimensional
 *
 * @return vector of indexes
 */
std::shared_ptr<DcmIndexList> DcmImage::GetIndexes()
{

  std::shared_ptr<DcmIndexList> indexes(DcmIndexList::Create());

  std::shared_ptr<DcmElementCollection> elementCollection(GetElements());


  std::shared_ptr<DcmElement> element;

  // ===========================================================================================================
  // (0020,9222) DimensionIndexSequence  
  // element was found we can determine the indexing of the frames. This indexing sequence defines the ordering
  // sequence of the frames and can contain multiple elements. These wlements will be in the the (0020,9157) 
  // DemensionIndexValues element which is in mlti-valued element of the form SORT-1/SORT-2/SORT-3/... I grab 
  // the individuaL elements tags here just in case the (0020,9157) element is missing.
  // ===========================================================================================================
  if ( elementCollection->TryGetElement(DCM_DimensionIndexSequence, element) )
  {

    indexes = DcmIndexList::Create(DCM_DimensionIndexSequence);

    /// cast the element to a sequence
    std::shared_ptr<dcm::DcmSequence> sequence(std::static_pointer_cast<dcm::DcmSequence>(element));

    sequence->Reset();

      
    while ( sequence->MoveNext() )
    {
 
      std::shared_ptr<dcm::DcmElementCollection> collection(sequence->Current());

      std::string  sUid( collection->TryGetElement(DCM_DimensionOrganizationUID, element ) ? 
                            element->GetValue() : 
                            ( collection->TryGetElement(DCM_ConcatenationUID, element ) ? element->GetValue() : "" ) );

      if ( ! sUid.empty() )
      {
          
        const DcmTag index( collection->TryGetElement(DCM_DimensionIndexPointer, element ) ? DcmUtility::CreateDcmTag(element->GetValue()) : DcmTag(0,0) );
        const DcmTag group( collection->TryGetElement(DCM_FunctionalGroupPointer, element ) ? DcmUtility::CreateDcmTag(element->GetValue()) : DcmTag(0,0) );

        indexes->Add(DcmIndex::Create(sUid, group, index));
      }
        
 
    } // END ... For each sequence item


  }

  return indexes;

} // GetIndexes()


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
bool DcmImage::TryGetElement(const DcmTag & sequenceTag, const DcmTag & elementTag, std::shared_ptr<dcm::DcmElement> & element, bool searchSubElement )
{

  return GetElements()->TryGetElement(sequenceTag, elementTag, element, searchSubElement) ;
} // TryGetElement(const DcmTag & sequenceTag, const DcmTag & elementTag, std::shared_ptr<DcmElement> & element, bool searchSubElement = false )


/**
 *
 */
bool DcmImage::TryGetBuffer(const DcmTag & tag, std::shared_ptr<DcmBuffer> & dcmBuffer, bool searchSubElements )
{

  bool found = false;
  unsigned long size(0);
  ::DcmElement * element = __nullptr;
  Uint8 * imageBuffer = __nullptr;

  OFCondition cond = m_pDcmDataset->findAndGetElement(tag, element, searchSubElements);

  // ======================================
  // Make sure the element tag was found
  // ======================================
  if ( cond.good() )
  {

    // =============================================================================================================
    // Set the flag indicating that the pixel data was found. At this point no errors will be logged only warnings.
    // =============================================================================================================
    found = true;


    // =============================================================================================================
    // If the data representation is OW (Word) then extract the pixel data directly from the value. If the
    // representation is OB then check if it is encapsulated and extract from the encapsaulated items otherwise
    // extract from the value.
    // ==============================================================================================================
    if ( element->getTag().getEVR() == DcmEVR::EVR_OW )
    {

      Uint16 * imageBuffer16 = __nullptr;
      cond = element->getUint16Array(imageBuffer16);
      size = element->getLength();

      if ( cond.bad() )
         DcmLog::Warning(util::Strings::Format("Failed to extract image data for element (%04x,%04x): %s", 
                          element->getTag().getGroup(), element->getTag().getElement(), cond.text() ), "DcmImage", "TryGetBuffer");
      else
        imageBuffer = (Uint8 *)imageBuffer16;
      

    } // END ... If the value representation is OW
    else
    {
      DcmPixelData * pixelData = OFstatic_cast(DcmPixelData*, element);
      DcmPixelSequence * sequence = __nullptr;
      E_TransferSyntax xferSyntax = EXS_Unknown;
      const DcmRepresentationParameter * parameter = __nullptr;

      // =========================================================================================
      // Find the key that is needed to access the right representation of the data within DCMTK
      // =========================================================================================
      pixelData->getOriginalRepresentationKey(xferSyntax, parameter);

      // ==================================================================================================================
      // attempt to get the encapsulated representation of the pixel data. If found then we will extract the pixel data
      // from the items otherwise we will extract the pixel data directly from the value.
      // ==================================================================================================================
      if ( pixelData->getEncapsulatedRepresentation(xferSyntax, parameter, sequence) == EC_Normal)
      {

        DcmPixelItem* pixelItem = NULL;

        // Access first frame (skipping offset table)
        sequence->getItem(pixelItem, 1);

        // =======================================================
        // Make sure that the pixel data sequence item was found
        // =======================================================
        if (pixelItem != __nullptr)
        {

          // Set to NULL
          imageBuffer = __nullptr;

          // ==================================================================================================
          // Get the length of this pixel item (i.e. fragment, i.e. most of the time, the lenght of the frame).
          // if the lengtrh returned is 0 then issue a warning.
          // ==================================================================================================
          if ( ( size = pixelItem->getLength() ) > 0 )
            cond = pixelItem->getUint8Array(imageBuffer);
          else
            DcmLog::Warning(util::Strings::Format("Image data length is 0 (%04x,%04x): %s", 
                element->getTag().getGroup(), element->getTag().getElement(), cond.text() ), "DcmImage", "TryGetBuffer");

        } // END ... If the pixel item was found


      } // END ... If there is emcapsulated pixel data
      else
      {
        cond = element->getUint8Array(imageBuffer);
        size = element->getLength();
      } // END ... Else is the pixel data is not encapsulated


    } // END ... Else the data representation is OB



    // ===============================================================================
    // If the binary data was extracted from the element the copy to the DcmBuffer
    // ===============================================================================
    if ( cond.good() && size > 0 )
      dcmBuffer->Assign((unsigned char *)imageBuffer, size);
    else
      DcmLog::Warning(util::Strings::Format("Failed to extract image data for element (%04x,%04x): %s", 
                      element->getTag().getGroup(), element->getTag().getElement(), cond.text() ), "DcmImage", "TryGetBuffer");

  } // END ... If the dicom element was found

  return found;

} // TryGetValue(const DcmTag & tag, std::shared_ptr<DcmBuffer> dcmBuffer, bool searchSubElements )


/**
 * Try and get the string value
 *
 * @patam tag                 Tag to get value
 * @param value               String value to assign
 * @paran searchSubElements   F;ag to indicate that we are to search sub-selements
 *
 * @return True is found, false otherwsie
 */
bool DcmImage::TryGetString(const DcmTag & tag, std::string & value , bool searchSubElements )
{
  bool found(false);

  OFString str;
  OFCondition cond;
  ::DcmElement * element = __nullptr;
  if ( searchSubElements )
  {
    
    cond = m_pDcmDataset->findAndGetElement( tag,  element, false ); 

    
   // cond = m_pDcmDataset->findAndGetOFStringArray  ( tag, str, false);  

    if ( ! cond.good()  )
       cond = m_pDcmDataset->findAndGetElement( tag,  element, true ); 

      //cond = m_pDcmDataset->findAndGetOFStringArray  ( tag, str, true);  
  }
  else
    cond = m_pDcmDataset->findAndGetElement( tag,  element, true ); 


  // =======================================================
  // If the element was found then try to extract the data
  // =======================================================
  if ( (found = cond.good()) && element != __nullptr)
  {
    // ================================================================================================
    // If the VR == UN (Unknown) then the library just stores the binary so I need to read the data
    // out into a byte array and convert it,
    // ================================================================================================
    if ( element->getVR() == EVR_UN )
    {
      Uint8 * buffer;
      cond = element->getUint8Array(buffer);

      // ==========================================================
      // Only load the data if the value are extracted correctly
      // =========================================================
      if ( ( found = cond.good()) )
        value.assign((char *)buffer, element->getLength());
      else
         DcmLog::Warning(util::Strings::Format("Failed to extract data for element (%04x,%04x): %s", 
                          element->getTag().getGroup(), element->getTag().getElement(), cond.text() ), "DcmImage", "GetValue");

    }
    else
    {

      cond = element->getOFStringArray( str, false);

      // ==========================================================
      // Only load the data if the value are extracted correctly
      // =========================================================
      if ( ( found = cond.good()) )
        value.assign(str.c_str());
      else
        DcmLog::Warning(util::Strings::Format("Failed to extract data for element (%04x,%04x): %s", 
                          element->getTag().getGroup(), element->getTag().getElement(), cond.text() ), "DcmImage", "GetValue");
    }
   


  }

  return found;
   

} // TryGetValue(unsigned short iGroup, unsigned short iElement, std::string & sValue)

/**
 * Saves the image to the specified file
 *
 * @param sFilname      File to save the dicom to
 */
void DcmImage::Save(const std::string & sFilename)
{

  // ===============================================================================
  // Saves the dicom to the specified file
  //
  //virtual OFCondition saveFile(const char *fileName,
  //                           const E_TransferSyntax writeXfer = EXS_Unknown,
  //                           const E_EncodingType encodingType = EET_UndefinedLength,
  //                           const E_GrpLenEncoding groupLength = EGL_recalcGL,
  //                           const E_PaddingEncoding padEncoding = EPD_noChange,
  //                           const Uint32 padLength = 0,
  //                           const Uint32 subPadLength = 0,
  //                           const OFBool isDataset = OFFalse);
  // ==================================================================================
  //E_TransferSyntax
  //E_EncodingType
  OFCondition cond  = m_fileFormat.saveFile(sFilename.c_str(), EXS_LittleEndianExplicit, EET_ExplicitLength );

  if ( cond.bad() )
    throw DcmIOException(LogRec( util::Strings::Format("Failed to save image : %s", cond.text()), "DcmImage", "Save"));

} // Save(const std::string & sFilename)


/**
 * Saves the header to the specified file
 *
 * @param sFilname      File to save the dicom header to
 */
void DcmImage::SaveHeader(const std::string & sFilename)
{

  m_fileFormat.loadAllDataIntoMemory();

  std::ofstream of(sFilename.c_str());

  OFCondition cond  = m_fileFormat.writeXML(of);

  of.close();

  if ( cond.bad() )
    throw DcmIOException(LogRec( util::Strings::Format("Failed to save header : %s", cond.text()), "DcmImage", "SaveHeader"));

} // SaveHeader(const std::string & sFilename)



#pragma region File Loading Methods



/**
 * Loads the file into the DcmImage
 *
 * @param sFilename     File to be loaded
 * @return true if successful
 */
bool DcmImage::Load(const std::string & sFilename, bool bThrowException )
{

  bool bLoaded(true);

  if ( _access(sFilename.c_str(), 4) == -1 )
  {
    if ( bThrowException )
      throw DcmIOException(LogRec(util::Strings::Format("File specified was not found or unreadable %s", sFilename.c_str()), "DcmImage", "Create"));
    else
      DcmLog::Warning(util::Strings::Format("File specified was not found or unreadable %s", sFilename.c_str()), "DcmImage", "Load");
  }
  else
  {
    //
    // OFCondition loadFile(const char *fileName,
    //                               const E_TransferSyntax readXfer = EXS_Unknown,
    //                               const E_GrpLenEncoding groupLength = EGL_noChange,
    //                               const Uint32 maxReadLength = DCM_MaxReadLength,
    //                               const E_FileReadMode readMode = ERM_autoDetect);

    OFCondition cond = m_fileFormat.loadFile(sFilename.c_str());

    // =================================================================================
    // Check the return condition. If it failed then we will examine the file for a PDF
    // =================================================================================
    if ( cond.bad() )
    {
      bLoaded = false;
      if ( bThrowException )
        throw DcmInvalidDicomFile(LogRec(util::Strings::Format("Failed to load file %s : %s", sFilename.c_str(), cond.text()), "DcmImage", "Load"));
      else
        DcmLog::Warning(util::Strings::Format("Failed to load file %s : %s", sFilename.c_str(), cond.text()), "DcmImage", "Load");
    }

    m_sFilename = sFilename;

  }

  return bLoaded;

} // Load(const std::string & sFilename)



#pragma endregion



#pragma region management methods





/**
 * Removes the specified Element if it exists
 *
 * @param tag       DcmTag 
 */
void DcmImage::Remove(const DcmTag & tag)
{
  
  ClearCachedElements();

  DcmLog::Debug(util::Strings::Format("Removing (%04x,%04x) from list", tag.getGroup(), tag.getElement()), "DcmImage", "Remove");

  /// Do nothing with the 
  m_pDcmDataset->remove(tag);

} // Remove(const DcmTag & tag)

/**
 * Removes the specified Element if it exists
 *
 * @param iGroup      Group id
 */
void DcmImage::Remove(unsigned short iGroup)
{
  ClearCachedElements();

  DcmLog::Debug(util::Strings::Format("Removing Group (%04x) from list", iGroup), "DcmImage", "Remove");

} // Remove(unsigned short iGroup)



/**
 * Clears all elements
 */
void DcmImage::Clear()
{
  ClearCachedElements();

  DcmLog::Debug("Clearing all element from the list", "DcmImage", "Clear");

  /// Do nothing with the 
  OFCondition cond = m_pDcmDataset->clear();

  if ( cond.bad() )
    throw DcmException(LogRec(util::Strings::Format("Failed to clear the element list : %s", cond.text()), "DcmImage", "Clear"));

} // Clear()



/**
 * Inserts the specified element list
 *
 * @param pDcmElementCollection       Element list to insert
 *
 */
void DcmImage::Load(const std::shared_ptr<dcm::DcmElementCollection> & elementList)
{
  elementList->CopyTo((DcmItem *)m_pDcmDataset);
} // Insert(std::shared_ptr<dcm::DcmElementCollection> pDcmElementCollection)


/**
 * Adds or Updates the element in the image
 *
 * @param element       Element to be updated
 */
void DcmImage::Insert(std::shared_ptr<dcm::DcmElement> & element)
{
  
  if ( element->IsSequence() )
  {
    std::shared_ptr<DcmSequence> sequence = std::dynamic_pointer_cast<DcmSequence>(element);
    sequence->CopyTo((DcmItem *)m_pDcmDataset);
  }    
  else
  {
    std::string value(element->GetValue());

    if ( value == "*DELETE*" )
      Remove(element->GetDcmTag());  
    else
      SetValue<std::string>(element->GetDcmTag(), element->GetValue());
  }
  

} // Insert(std::shread_ptr<DcmElement> & element)

#pragma endregion


#pragma region properties

/**
 * Determines if this image is an concatenated image
 */
bool DcmImage::IsConcatenated()
{
  std::shared_ptr<dcm::DcmElement> element;
  return IDcmImage::TryGetElement(DCM_InConcatenationNumber,element, true);
} // IsConcatenated()


/**
 * Sets the retrieval ae title
 *
 * @param sAETitle      AE Title
 */
void DcmImage::SetRetrievalAEtitle(const std::string & sAETitle)
{
  SetValue<std::string>(DcmTag(0x0008,0x0054),sAETitle);
} // SetRetrievalAEtitle(const std::string & sAETitle)


/**
 * Gets the Retrieval ae title
 *
 *  @return string
 */
std::string DcmImage::GetRetrievalAETitle()
{
  std::string sAETitle("");
  TryGetValue<std::string>(DcmTag(0x0008,0x0054),sAETitle);
  return sAETitle;
} // GetRetrievalAETitle()


/**
 * Sets the image type
 *
 * @param sImageType  Image type
 */
void DcmImage::SetImageType(const std::string & sImageTYype)
{
  SetValue<std::string>(DcmTag(0x0008,0x0008),sImageTYype);
} // SetImageType(const std::string & sImageTYype)


/**
 * Sets the photometric interpretation
 *
 * @param sPhotometricInterpretation  Photometric interpretation
 */
void DcmImage::SetPhotometricInterpretation(const std::string & sPhotometricInterpretation)
{
  SetValue<std::string>(DcmTag(0x0028,0x0004),sPhotometricInterpretation);
} // SetPhotometricInterpretation(const std::string & sPhotometricInterpretation)

/**
 * Gets the photometric interpretation
 */
std::string DcmImage::GetPhotometricInterpretation()
{
  std::string sPhotometricInterpretation("");
  TryGetValue<std::string>(DcmTag(0x0028,0x0004),sPhotometricInterpretation);
  return sPhotometricInterpretation;
} // SetPhotometricInterpretation(const std::string & sPhotometricInterpretation)



/**
 * Sets the high bit value
 *
 * @param iHighBit    New high bit value
 */
void DcmImage::SetHighBit(unsigned short iHighBit)
{
  SetValue<unsigned short>(DcmTag(0x0028, 0x0102), iHighBit);
} // SetHighBit(unsigned short iHighBit)


/**
 * Sets the bits stored
 *
 * @param iBitsStored    New bits stored value
 */
void DcmImage::SetBitsStored(unsigned short iBitsStored)
{
  SetValue<unsigned short>(DcmTag(0x0028, 0x0101), iBitsStored);
} // SetBtsStored(unsigned short iBitsStored)



/**
 * Sets the bits allocated
 *
 * @param iBitsAllocated    New bits allocated value
 */
void DcmImage::SetBitsAllocated(unsigned short iBitsAllocated)
{
  SetValue<unsigned short>(DcmTag(0x0028, 0x0100), iBitsAllocated);
} // SetBitsAllocated(unsigned short iBitsStored)



/**
 * Returns the dicom header in XML
 *
 * @param bStripControlCharacters       
 *
 */
std::string DcmImage::GetHeader(bool bStripControlChracters)
{
  std::ostringstream os;

  m_pDcmDataset->writeXML(os);


  std::string sXml(os.str());

  if ( bStripControlChracters )
  {
    std::regex rx("[\\x00-\\x1f\\x7f]");
    std::string sReplace("");
    
    sXml = std::regex_replace( sXml , rx , sReplace);    
  }
  

  return sXml;

} // GetHeader()




/**
 * Sets the pixel spacing
 *
 * @param pixelSpacing    Double contains the pixel spacing 
 */
void DcmImage::SetPixelSpacing(DcmDouble<double> pixelSpacing)
{

  SetValue<std::string>(DcmTag(0x0028,0x0030),pixelSpacing.ToString());

} // SetPixelSpacing(DcmDouble<double> pixelSpacing)



/**
 * Sets the series instance uid
 *
 *  @param sUid
 */
void  DcmImage::SetInstanceUid(const std::string & sUid)
{  
  SetValue<std::string>(DCM_SOPInstanceUID, sUid);  
} // SetInstanceUid(const std::string & sUid)



/**
 * Sets the series instance uid
 *  
 *  @param sUid
 */
void  DcmImage::SetAffectedSopClassUid(const std::string & sUid)
{  
  SetValue<std::string>(DCM_SOPClassUID, sUid);  
} // SetAffectedSopClassUid(const std::string & sUid)



/**
 * Sets the series instance uid
 *
 *  @param sUid
 */
void  DcmImage::SetStudyInstanceUid(const std::string & sUid)
{  
  SetValue<std::string>(DCM_StudyInstanceUID, sUid);  
} // SetStudyInstanceUid(const std::string & sUid)




/**
 * Sets the series instance uid
 *
 *  @param sUid
 */
void  DcmImage::SetSeriesInstanceUid(const std::string & sUid)
{  
  SetValue<std::string>(DCM_SeriesInstanceUID, sUid);  
} // SetSeriesInstanceUid(const std::string & sUid)



/**
 * Sets the institution
 *  
 *  @param sInstitution
 */
void  DcmImage::SetInstitutionName(const std::string & sInstitution)
{  
  SetValue<std::string>(DCM_InstitutionName, sInstitution); 
} // SetInstitutionName(const std::string & sInstitution)


/**
 * Sets the manufacturer 
 *
 *  @param sManufacturer
 */
void  DcmImage::SetManufacturer(const std::string & sManufacturer)
{  
  SetValue<std::string>(DCM_Manufacturer, sManufacturer); 
} // SetManufacturer(const std::string & sModel)



/**
 * Sets the manufacturer model
 *
 *  @param sModel
 */
void  DcmImage::SetManufacturerModel(const std::string & sModel)
{  
  SetValue<std::string>(DCM_ManufacturerModelName, sModel); 
} // SetManufacturerModel(const std::string & sModel)


/**
 * Gets the instance number
 *
 *  @return the instance number
 */
unsigned int DcmImage::GetInstanceNumber()
{
  unsigned int iNumber(0);
  TryGetValue<unsigned int>(DCM_InstanceNumber, iNumber);
  return iNumber;
} // GetInstanceNumber()


/**
 * Sets the instance number
 *
 *  @param iInstanceNumber
 */
void  DcmImage::SetInstanceNumber(unsigned int iInstanceNumber)
{  
  SetValue<unsigned int>(DCM_InstanceNumber, iInstanceNumber); 
} // SetInstanceNumber(unsigned int iInstanceNumber)


/**
 * Sets the rescale intercept
 *
 *  @param fRescaleIntecept
 */
void  DcmImage::SetRescaleIntercept(float fRecaleIntecept)
{  
  SetValue<float>(DCM_RescaleIntercept, fRecaleIntecept); 
} // SetRescaleIntercept(float fRecaleIntecept)



/**
 * Sets the rescale slope
 *
 *  @param fRescaleSlope
 */
void  DcmImage::SetRescaleSlope(float fRecaleSlope)
{  
  SetValue<float>(DCM_RescaleSlope, fRecaleSlope); 
}  // SetRescaleSlope(float fRecaleSlope)



/*
 * Sets the row
 *
 *  @param iRows
 */
void  DcmImage::SetRows(unsigned int iRows)
{  
  SetValue<unsigned int>(DCM_Rows, iRows); 
} // SetRows(unsigned int iRows)



/*
 * Sets the columns
 *
 * @param iColums
 */
void  DcmImage::SetColumns(unsigned int iColumns)
{  
  SetValue<unsigned int>(DCM_Columns, iColumns); 
} // SetColumns(unsigned int iColumns)


/**
 * Gets the additional patient history
 *
 *  @return the patient history string
 */
std::string DcmImage::GetAdditionalPatientHistory()
{
  std::string sHistory("");
  TryGetValue<std::string>(DCM_AdditionalPatientHistory, sHistory);
  return sHistory;
} // GetAdditionalPatientHistory()


/**
 * Sets the additional history
 *
 *  @param sHistory
 */
void DcmImage::SetAdditionalPatientHistory(const std::string & sHistory)
{  
  SetValue<std::string>(DCM_AdditionalPatientHistory, sHistory); 
} // SetAdditionalPatientHistory(const std::string & sHistory)



/**
 * Gets the operator name
 *
 *  @return the operator name string
 */
std::string DcmImage::GetOperatorName()
{
  std::string sName("");
  TryGetValue<std::string>(DCM_OperatorsName, sName);
  return sName;
} // GetOperatorName()


/**
 * Sets the operator
 *
 *  @param sName
 */
void DcmImage::SetOperatorName(const std::string & sName)
{  
  SetValue<std::string>(DCM_OperatorsName, sName); 
} // SetOperatorName(const std::string & sName)



/**
 * Sets the referring physician
 *
 *  @param sName
 */
void DcmImage::SetReferringPhysician(const std::string & sName)
{  
  SetValue<std::string>(DCM_ReferringPhysicianName, sName); 
} // SetReferringPhysician(const std::string & sNumber)


/**
 * Sets the accession number
 *
 *  @param sNumber
 */
void DcmImage::SetAccessionNumber(const std::string & sNumber)
{  
  SetValue<std::string>(DCM_AccessionNumber, sNumber); 
} // SetAccessionNumber(const std::string & sNumber)


/**
 * Sets the study description
 *
 *  @param sDescription
 */
void DcmImage::SetStudyDescription(const std::string & sDescription)
{  
  SetValue<std::string>(DCM_StudyDescription, sDescription); 
} // SetStudyDescription(const std::string & sDescription)


/**
 * Sets the series description
 *
 *  @param sDescription
 */
void DcmImage::SetSeriesDescription(const std::string & sDescription)
{  
  SetValue<std::string>(DCM_SeriesDescription, sDescription); 
} // SetSeriesDescription(const std::string & sDescription)




/**
 * Sets the patient position
 *
 *  @param sPosition
 */
void DcmImage::SetPatientPosition(const std::string & sPosition)
{  
  SetValue<std::string>(DCM_PatientPosition, sPosition); 
} // SetPatientPosition(const std::string & sPosition)


/**
 * Gets the patient sex
 *
 *  @return sex
 */
std::string DcmImage::GetPatientSex()
{
  std::string sSex("");
  TryGetValue<std::string>(DCM_PatientSex, sSex);
  return sSex;
} // GetPatientSex()


/**
 * Sets the patient sex
 *
 *  @param sSex
 */
void DcmImage::SetPatientSex(const std::string & sSex)
{  
  SetValue<std::string>(DCM_PatientSex, sSex); 
} // SetPatientSex(const std::string & sSex)


/**
 * Sets the patients dob
 *
 *  @param dob
 */
void DcmImage::SetPatientDob(const dcm::DcmDate & dob)
{
  /// Validates the date first
  const_cast<dcm::DcmDate &>(dob).Validate();

  SetValue<std::string>(DCM_PatientBirthDate, const_cast<dcm::DcmDate &>(dob).ToString());
} // SetPatientDob(const dcm::DcmDate & dob)


/**
 * Gets the patient DOB
 *
 *  @return the DOB
 */
dcm::DcmDate DcmImage::GetPatientDob()
{
  std::string sDate("");
  TryGetValue<std::string>(DCM_PatientBirthDate, sDate);
  return dcm::DcmDate(sDate);
} // GetPatientDob()


/**
 * Sets the patients tob
 *
 *  @param tob
 */
void DcmImage::SetPatientTob(const dcm::DcmTime & tob)
{
  /// Validates the date first
  const_cast<dcm::DcmTime &>(tob).Validate();

  SetValue<std::string>(DCM_PatientBirthTime, const_cast<dcm::DcmTime &>(tob).ToString());
} // SetPatientTob(const dcm::DcmTime & tob)


/**
 * Gets the patient TOB
 *
 *  @return the patient tob
 */
dcm::DcmTime DcmImage::GetPatientTob()
{
  std::string sTime("");
  TryGetValue<std::string>(DCM_PatientBirthTime, sTime);
  return dcm::DcmTime(sTime);
} // GetPatientTob()



/**
 * Gets the KVP
 *
 *  @return the kvp
 */
float DcmImage::GetKvp()
{
  float fKvp(0.0F);
  TryGetValue<float>(DCM_KVP, fKvp);
  return fKvp;
} // GetKvp()


/**
 * Sets the KVP
 *
 *  @param fKvp
 */
void DcmImage::SetKvp(float fKvp)
{ 
  SetValue<float>(DCM_KVP,fKvp); 
} // SetKvp(float fKvp)


/**
 * Gets the xray tube current
 *
 *  @return tube current
 */
float DcmImage::GetXrayTubeCurrent()
{
  float fCurrent(0.0F);
  TryGetValue<float>(DCM_XRayTubeCurrent, fCurrent);
  return fCurrent;
} // GetXrayTubeCurrent()


/**
 *  set tube current
 *
 *  @param fCurrent
 */
void DcmImage::SetXrayTubeCurrent(float fCurrent)
{ 
  SetValue<float>(DCM_XRayTubeCurrent,fCurrent); 
} // SetXrayTubeCurrent(float fCurrent)



/**
 * Gets the slice thickness
 *  
 *  @return slice thickness
 */
float DcmImage::GetSliceThickness()
{
  float fThickness(0.0F);
  TryGetValue<float>(DCM_SliceThickness, fThickness);
  return fThickness;
} // GetSliceThickness()


/**
 * Sets the slice thickness
 *
 *  @param fThickness
 */
void DcmImage::SetSliceThickness(float fThickness)
{ 
  SetValue<float>(DCM_SliceThickness,fThickness); 
} // SetSliceThickness(float fThickness)


/**
 * Gets the patient weight
 *
 *  @return weight
 */
float DcmImage::GetPatientWeight()
{
  float fWeight(0);
  TryGetValue<float>(DCM_PatientWeight, fWeight);
  return fWeight;
} // GetPatientWeight()


/**
 * Sets the patient weight
 *
 *  @param iWeight
 */
void DcmImage::SetPatientWeight(int iWeight)
{ 
  SetValue<int>(DCM_PatientWeight,iWeight); 
} // SetPatientWeight(int iWieght)


/**
 * Gets the patient age
 *
 *  @return age
 */
std::string DcmImage::GetPatientAge()
{
  std::string sAge("");
  TryGetValue<std::string>(DCM_PatientAge, sAge);
  return sAge;
} // GetPatientAge()


/**
 * Sets the patient age
 *
 *  @param sAge
 */
void DcmImage::SetPatientAge(const std::string & sAge)
{ 
  SetValue<std::string>(DCM_PatientAge,sAge); 
} // SetPatientAge(const std::string & sAge)


/*
 * Gets the reconstruction diameter
 *
 *  @return diameter of FOV
 */
float DcmImage::GetReconstructionDiameter()
{
  float fValue = 0.0F;
  TryGetValue<float>(DCM_ReconstructionDiameter, fValue);
  return fValue;
} // GetReconstructionDiameter()


/**
 * Sets the reconstruction diameter
 *
 *  @param fDiameter
 */
void DcmImage::SetReconstructionDiameter(float fDiameter)
{ 
  SetValue<float>(DCM_ReconstructionDiameter,fDiameter); 
} // SetReconstructionDiameter(float fDiameter)


/*
 * Gets the patient id
 *
 *  @return patient id
 */
std::string DcmImage::GetPatientId()
{
  std::string msValue("");
  TryGetValue<std::string>(DCM_PatientID, msValue);
  return msValue;
} // GetPatientId()


/**
 * Sets the patient id
 *
 *  @param sPatientId
 */
void DcmImage::SetPatientId(const std::string & sPatientId)
{ 
  SetValue<std::string>(DCM_PatientID, sPatientId); 
} // SetPatientId(const std::string & sPatientId)) 


/** 
 * Sets the samples per pixel
 *
 *  @param iSamples
 */
void DcmImage::SetSamplePerPixel(unsigned short iSamples)
{ 
  SetValue<unsigned short>(DCM_SamplesPerPixel, iSamples); 
} // SetSamplePerPixel(unsigned short iSamples)


/**
 * Sets the modality. Makes sure the value is upper case
 * 
 * @param sModality
 */
void DcmImage::SetModality(const std::string & sModality)
{ 
  std::string sUpper(toupper(sModality));
  //std::string sUpper(sModality.size(), ' ');
  //std::transform(sModality.begin(), sModality.end(), sUpper.begin(), ::toupper); 
  SetValue<std::string>(DCM_Modality, sUpper); 
} // SetModality(const std::string & sModality)

/**
 * Sets the study id
 * 
 * @param sId study id
 */
void DcmImage::SetStudyId(const std::string & sId)
{ 
  SetValue<std::string>(DCM_StudyID, sId); 
} // SetStudyId(const std::string & sId)


/*
 * Gets the study id
 *
 *  @return study id
 */
std::string DcmImage::GetStudyId()
{
  std::string sId("");
  TryGetValue<std::string>(DCM_StudyID, sId);
  return sId;
} // GetStudyId()


/**
 * Sets the series number
 * 
 * @param sId
 */
void DcmImage::SetSeriesNumber(const std::string & sNumber)
{ 
  SetValue<std::string>(DCM_SeriesNumber, sNumber); 
} // SetSeriesNumber(const std::string & sNumber)


/*
 * Gets the series number 
 *
 *  @return series number
 */
std::string DcmImage::GetSeriesNumber()
{
  std::string sNumber("");
  TryGetValue<std::string>(DCM_SeriesNumber, sNumber);
  return sNumber;
} // GetSeriesNumber()


/**
 * Sets the patient name
 * 
 * @param sName   Patient name
 */
void DcmImage::SetPatientName(const std::string & sName)
{ 
  SetValue<std::string>(DCM_PatientName, sName); 
} // SetSeriesNumber(const std::string & sNumber)


/**
 *  Sets the gantry detector tilt
 *
 * @param fTilit
 */
 void DcmImage::SetGantryDetectorTilt(float fTilt)
 { 
   SetValue<float>(DCM_GantryDetectorTilt, fTilt); 
 }  // SetGantryDetectorTilt(float fTilt)


/**
 * Gets the gantry detector tilt
 *
 *  @return tilt angle
 */
float DcmImage::GetGantryDetectorTilt()
{
  float fTilt; 
  TryGetValue<float >(DCM_GantryDetectorTilt, fTilt);
  return fTilt;
} // GetGantryDetectorTilt()



/**
 * Sets teh study date
 *
 *  @param date
 */
void DcmImage::SetStudyDate(const dcm::DcmDate & date)
{
  // Valdate first
  const_cast<dcm::DcmDate &>(date).Validate();

  SetValue<std::string>(DCM_StudyDate, const_cast<dcm::DcmDate &>(date).ToString() );

} // SetStudyDate(const dcm::DcmDate & date)



/**
 * Sets the series  date
 *
 *  @param time
 */
void DcmImage::SetSeriesTime(const dcm::DcmTime & time)
{
  // Validate first
  const_cast<dcm::DcmTime &>(time).Validate();

  SetValue<std::string>(DCM_SeriesTime, const_cast<dcm::DcmTime &>(time).ToString() );

} // GetSeriesTime(const dcm::DcmTime & time)


/**
 * Gets the acquisition date
 *
 *  @return acquisition date
 */
dcm::DcmDate DcmImage::GetAcquisitionDate()
{
  std::string sDate;
  TryGetValue<std::string>(DCM_AcquisitionDate, sDate);
  return dcm::DcmDate(sDate);
} // GetAcquisitionDate()


/**
 * Sets the acquisition date
 *
 *  @param date
 */
void DcmImage::SetAcquisitionDate(const dcm::DcmDate & date)
{
  // Validate first
  const_cast<dcm::DcmDate &>(date).Validate();

  SetValue<std::string>(DCM_AcquisitionDate, const_cast<dcm::DcmDate &>(date).ToString() );

} // SetAcquisitionDate(const dcm::DcmDate & date)


/**
 * Gets the acquisition time
 *
 *  @return acquisition time
 */
dcm::DcmTime DcmImage::GetAcquisitionTime()
{
  std::string sTime;
  TryGetValue<std::string>(DCM_AcquisitionTime, sTime);
  return dcm::DcmTime(sTime);
} // GetSeriesTime()


/**
 * Sets the acquisition  date
 *
 *  @param time
 */
void DcmImage::SetAcquisitionTime(const dcm::DcmTime & time)
{
  // Validate first
  const_cast<dcm::DcmTime &>(time).Validate();

  SetValue<std::string>(DCM_AcquisitionTime, const_cast<dcm::DcmTime &>(time).ToString() );

} // SetAcquisitionTime(const dcm::DcmTime & time)



/**
 * Gets the image date
 * 
 *  @return image date
 */
dcm::DcmDate DcmImage::GetImageDate()
{
  std::string sDate;
  TryGetValue<std::string>(DCM_ContentDate, sDate);
  return dcm::DcmDate(sDate);
} // GetImageDate()


/**
 * Sets the Image date
 *
 *  @param date
 */
void DcmImage::SetImageDate(const dcm::DcmDate & date)
{
  // Validate first
  const_cast<dcm::DcmDate &>(date).Validate();

  SetValue<std::string>(DCM_ContentDate, const_cast<dcm::DcmDate &>(date).ToString() );

} // SetImageDate(const dcm::DcmDate & date)


/**
 * Gets the Image time
 *
 *  @return image time
 */
dcm::DcmTime DcmImage::GetImageTime()
{
  std::string sTime;
  TryGetValue<std::string>(DCM_ContentTime, sTime);
  return dcm::DcmTime(sTime);
} // GetSeriesTime()


/**
 * Sets the Image  date
 *
 *  @param time
 */
void DcmImage::SetImageTime(const dcm::DcmTime & time)
{
  // Validate first
  const_cast<dcm::DcmTime &>(time).Validate();

  SetValue<std::string>(DCM_ContentTime, const_cast<dcm::DcmTime &>(time).ToString() );

} // SetImageTime(const dcm::DcmTime & time)



/**
 * Gets the acquisition number from the image
 *
 *  @return number
 */
__int64 DcmImage::GetAcquisitionNumber()
{
  __int64 iNumber(1);
  TryGetValue<__int64>(DCM_AcquisitionNumber, iNumber);
  return iNumber;

} // GetAcquisitionNumber()


/**
 * Sets the pixel padding value
 *
 *  @param iNumber
 */
void DcmImage::SetAcquisitionNumber(__int64 iNumber)
{ 
  SetValue<__int64>(DCM_AcquisitionNumber, iNumber); 
} // SetAcquisitionNumber(__int64 iNumber)


/**
 * Sets the pixel padding value
 *
 *  @param iPaddingValue
 */
void DcmImage::SetPixelPaddingValue(unsigned short iPaddingValue)
{ 
  SetValue<unsigned short>(DCM_PixelPaddingValue, iPaddingValue); 
} // SetPixelPaddingValue(unsigned short iPaddingValue)



/**
 * Sets the samples per pixel
 *
 *  @iSamples
 */
void DcmImage::SetSamplesPerPixel(unsigned short iSamples)
{  
  SetValue<unsigned short>(DCM_SamplesPerPixel, iSamples); 
} // SetSamplesPerPixel(unsigned short iSamples)




/**
 * Sets the number of frames
 *
 *  @param iFrames
 */
void DcmImage::SetNumberOfFrames(unsigned short iFrames)
{  
  SetValue<unsigned short>(DCM_NumberOfFrames, iFrames); 
} // SetNumberOfFrames(unsigned int iFrames)



/**
 * Gets the query Retrieval level
 *
 *  @return retrieve level
 */
std::string DcmImage::GetQueryRetrievalLevel()
{
  std::string sLevel(""); 
  TryGetValue<std::string >(DCM_QueryRetrieveLevel, sLevel);
  return sLevel;
} // GetQueryRetrievalLevel()

/**
 * Sets the query retrieval level
 */
void  DcmImage::SetQueryRetrievalLevel(const std::string & sLevel)
{  
  SetValue<std::string>(DCM_QueryRetrieveLevel, sLevel); 
} // SetQueryRetrievalLevel(const std::string & sLevel)



/**
 * Gets the position of this image (Relative to the plane)
 */
float DcmImage::GetPosition()
{

  /// Get the coordinates
  DcmTriple<float> coordinates(GetImagePosition());

  /// Gets the unit cosines
  DcmCosines cosines(GetImageOrientation());

  /// Get the plane of the image
  DcmPlanes plane(cosines.GetPlane());

  // ==========================================
  // Return the coordinate based on the plane
  // ==========================================
  switch ( plane )
  {
  case P_AXIAL:
    return coordinates.GetZ();
  case P_CORONAL:
    return coordinates.GetY();
  case P_SAGITTAL:
    return coordinates.GetX();
  default:
    return coordinates.GetZ();
  };  // END ... Switch on the plane
 
} // GetPosition()

/**
 * Returns the pixel data. Since the pixel data is stored as a OB (Other Byte) we store the value as an
 * unsigned char.
 *
 *  @return pixel data
 */
std::shared_ptr<DcmBuffer> DcmImage::GetPixelData()
{

  std::shared_ptr<DcmBuffer> pixelBuffer(new DcmBuffer());

  if ( ! TryGetValue<std::shared_ptr<DcmBuffer>>(DCM_PixelData, pixelBuffer ) )
  {
    LogWrn("No pixel data found", "DcmImage", "GetPixelData");
    //throw DcmInvalidDataException(LogRec("No pixel data found", "DcmImage", "GetPixelData"));
  }

  return pixelBuffer;

} // GetPixelData()


/**
 * Returns the pixel data. Since the pixel data is stored as a OB (Other Byte) we store the value as an
 * unsigned char.
 *
 *  @param pixelData
 */
void DcmImage::SetPixelData(const std::shared_ptr<DcmBuffer> & pixelData)
{
  SetValue<std::shared_ptr<DcmBuffer>>(DCM_PixelData, pixelData);
} // SetPixelData(const DcmBuffer & dcmBuffer)


/**
 * Sets the pixel data
 *
 *  @param pImage
 *  @param pPayload
 */
void DcmImage::SetPixelData(const std::shared_ptr<Gdiplus::Bitmap> & pImage,  const std::shared_ptr<DcmBuffer> & pPayload )
{

  DcmGdiplusEngine engine;

  // Make sure the image buffer is not null
  if ( pImage == __nullptr )
    throw DcmInvalidDataException(LogRec("Invalid image specified, value is NULL", "DcmImage", "SetPixelData"));

  Gdiplus::Rect rect(0,0,pImage->GetWidth(),pImage->GetHeight());

  const int iWidth(pImage->GetWidth());
  const int iHeight(pImage->GetHeight());

   
  unsigned short iBytesPerPixel(2);
  unsigned short iAlphaMask(0);
  unsigned short iRedMask(0);
  unsigned short iGreenMask(0);
  unsigned short iBlueMask(0);
  unsigned short iRedShift(0);
  unsigned short iGreenShift(0);
  unsigned short iBlueShift(0);
  short iRedOffset(0);
  short iGreenOffset(0);
  short iBlueOffset(0);


  switch ( pImage->GetPixelFormat() )
  {
  case PF_24bppRgb:
    {
      iBytesPerPixel    = 3;
      iRedOffset        = 2; 
      iGreenOffset      = 1; 
      iBlueOffset       = 0; 
    }
    break;
  case PF_32bppArgb:
  case PF_32bppPArgb:
    {
      iBytesPerPixel    = 4;
      iRedOffset        = 2; 
      iGreenOffset      = 1; 
      iBlueOffset       = 0; 
    }
    break;

  case PF_32bppRgb:
    {
      iBytesPerPixel    = 4;
      iRedOffset        = 2; 
      iGreenOffset      = 1; 
      iBlueOffset       = 0; 
    }
    break;
  case PF_16bppArgb1555:
    {

      // ======================
      // 1000 0000 0000 0000
      // 0111 1100 0000 0000  
      // 0000 0011 1110 0000  
      // 0000 0000 0001 1111  
      // ======================

      iBytesPerPixel = 2;
      iAlphaMask  = 0x8000;
      iRedMask    = 0x7C00;
      iGreenMask  = 0x03D0;
      iBlueMask   = 0x001F;
      iRedShift   = 7;
      iGreenShift = 2;
      iBlueShift  = 3;
    }
    break;

  case PF_16bppRgb565:
    {
      //===========================
      // 1111 1000 0000 0000     
      // 0000 0111 1110 0000  
      // 0000 0000 0001 1111  
      //===========================

      iBytesPerPixel = 2;
      iAlphaMask  = 0x0000;
      iRedMask    = 0xF800;
      iGreenMask  = 0x07D0;
      iBlueMask   = 0x001F;
      iRedShift   = 8;
      iGreenShift = 3;
      iBlueShift  = 3;

    }
    break;

  case PF_16bppRgb555:
    {

      // =============================
      // 0111 1100 0000 0000     
      // 0000 0011 1110 0000  
      // 0000 0000 0001 1111  
      // =============================

      iBytesPerPixel = 2;
      iAlphaMask  = 0x0000;
      iRedMask    = 0x7C00;
      iGreenMask  = 0x03D0;
      iBlueMask   = 0x001F;
      iRedShift   = 7;
      iGreenShift = 2;
      iBlueShift  = 3;
    }
    break;

  default:
    throw DcmUnsupportedException(LogRec("Invalid image format type, unsupported", "DcmImage", "SetPixelData"));

  };  // END ... Switch

  
  Gdiplus::BitmapData bitmapData;
  Gdiplus::Status status;

  status = pImage->LockBits(&rect, 0, pImage->GetPixelFormat(), &bitmapData);

  // Make sure that the lock bits was successful
  if ( status != Gdiplus::Ok )
    throw DcmException(LogRec(util::Strings::Format("Failed to lock bitmap %d",status),"DcmImage","SetPixelData"));

  const int iStride(bitmapData.Stride);
  unsigned int iImageSize(iWidth * iHeight * iBytesPerPixel);
  if ( iImageSize % 2 == 1 ) iImageSize++;
 
 
  std::shared_ptr<DcmBuffer> pixelData(DcmBuffer::Create(iImageSize + ( pPayload == __nullptr ? 0  : pPayload->GetSize() ) ));

 
  unsigned char * pPixelDataPtr(pixelData->GetData<unsigned char>());

  // ================================================
  // If the bytes per pixel > 2 then we RGB or ARGB
  // ================================================
  if ( iBytesPerPixel > 2 )
  {

    unsigned char * pBitmapPtr = (unsigned char *)bitmapData.Scan0;

    // =========================
    // Loop through all the rows
    // ==========================
    for ( register int r(0); r < iHeight; ++r )
    {

      
      unsigned char * pRowPtr = pBitmapPtr + (r * iStride);
      
      // =============================
      // Loop through all the columns
      // =============================
      for ( register int c(0); c < iWidth; c++ )
      {

        *pPixelDataPtr++ = pRowPtr[ iRedOffset  ];
        *pPixelDataPtr++ = pRowPtr[ iGreenOffset ];
        *pPixelDataPtr++ = pRowPtr[ iBlueOffset ];

        pRowPtr += iBytesPerPixel;
      } // END ... For each column


    } // END ... For each row

  }
  else
  {

    unsigned short * pBitmapPtr = (unsigned short *)bitmapData.Scan0;

    for ( register unsigned int i(0); i < iImageSize; i++ )
    {
      const unsigned short pixelValue(*pBitmapPtr++);
      *pPixelDataPtr++ = (pixelValue & iRedMask)   >> iRedShift;
      *pPixelDataPtr++ = (pixelValue & iGreenMask) >> iGreenShift;
      *pPixelDataPtr++ = (pixelValue & iBlueMask)  << iBlueShift;
    }   
  }

  
  pImage->UnlockBits(&bitmapData);

  // ======================================
  // Append the payload behind the image
  // =======================================
  if ( pPayload != __nullptr )
    memcpy((pixelData->GetData<unsigned char>() + iImageSize), pPayload->GetData<unsigned char>() , pPayload->GetSize());

  /// Sets the pixel data
  SetPixelData(pixelData);
   
  // Set the values 
  SetPhotometricInterpretation("RGB");
  SetImageType("DERIVED/SECONDARY");
  SetAffectedSopClassUid(UID_SecondaryCaptureImageStorage);
  SetRows(iHeight);
  SetColumns(iWidth);
  SetSamplesPerPixel(3);
  SetBitsAllocated(8);
  SetHighBit(7);
  SetBitsStored(8);
  SetValue<std::string>(DCM_PlanarConfiguration, "0");
  SetValue<std::string>(DCM_PixelRepresentation, "0");
  SetValue<std::string>(DCM_ConversionType, "WSD");

} // SetPixelData(unsigned int iWidth, unsigned int iHeight, unsigned int iBitsPerPixel, void * pImage, unsigned int iExtraDataSize, void * pExtraData)




/**
 * Gets the element list for the DcmImage
 *
 *  @return element list
 */
std::shared_ptr<DcmElementCollection> & DcmImage::GetElements()
{
  if ( m_elements == __nullptr )
    m_elements = std::dynamic_pointer_cast<DcmElementCollection>(DcmElementCollection::Create(GetHeader(), PT_HEADER));

  return m_elements;

} // GetElementCollection()

/**
 * Gets the element list for the DcmImage
 *
 *  @return element list
 */
DcmElementCollectionList & DcmImage::GetFrameElements()
{
  if ( m_frameElements.size() == 0 )
    m_frameElements = GetElements()->GetFrames();

  return m_frameElements;

} // GetElementCollection()


/**
 * Sets the pixel representation
 * 
 * @param iPixelRepresentation    Pixel representation
 */
void DcmImage::SetPixelRepresentation(unsigned short iPixelRepresentation)
{ 
  SetValue<unsigned short>(DcmTag(0x0028,0x0103),iPixelRepresentation); 
} // SetPixelRepresenation(unsigned short iPixelRepresentation)


/**
 * Sets the window width
 * 
 * @param dWidth    Window width
 */
void DcmImage::SetWindowWidth(double dWidth)
{ 
  SetValue<double>(DcmTag(0x0028,0x1051),dWidth); 
} // SetWindowWidth(double dWidth)


/**
 * Sets the window center
 * 
 * @param dWidth    Window center
 */
void DcmImage::SetWindowCenter(double dCenter)
{ 
  SetValue<double>(DcmTag(0x0028,0x1050),dCenter); 
} // SSetWindowsCenter(double dCenter)



/**
 * Determines if the tag supplied is to included in the modification process.
 *
 * @param tag       Tag to be checked
 *
 * @return true if the tag is included, false otherwise
 */
bool DcmImage::IsIncluded(const DcmTag & tag)
{

  // If there are no inclusion and exclusions then assume all is to be included.
  bool bInclude( m_inclusion.size() == 0 && m_exclusion.size() == 0 ? true : false);

  // ========================================================================
  // Always the following groups. Use of the exclusions list will allow for 
  // finer granularity.
  //
  // 0008   8   General Information
  // 0010   16  Patient Information
  // 0032   50  Study Information
  // 0038   56  Admission Information
  // ==========================================================================
  if ( tag.getGroup() == 8 || tag.getGroup() == 16 || tag.getGroup() == 50 || tag.getGroup() == 56 )
  {

    // Assume inclusion
    bInclude = true;

    // =========================
    // Check for any exclusions 
    // =========================
    for ( std::vector<DcmTag>::const_iterator it = m_exclusion.cbegin(); it != m_exclusion.cend(); it++ )
    {
      if ( (*it) == tag )
      {
        bInclude = false;
        break;
      }
    } // END ... For each exclusion
    
  }
  else
  {
    // =========================
    // Check for any inclusions
    // ========================
    for ( std::vector<DcmTag>::const_iterator it = m_inclusion.cbegin(); it != m_inclusion.cend(); it++ )
    {
      if ( (*it) == tag )
      {
        bInclude = true;
        break;
      }
    } // END ... For each inclusion
  } // else

  return bInclude;
} //IsInclude


#pragma endregion

