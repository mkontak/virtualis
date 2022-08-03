// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmImageFactory.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmImageFactory.h"
#include "DcmDate.h"
#include "DcmTime.h"
#include "DcmExceptions.h"
#include "DcmLog.h"
#include "DcmDictionary.h"
#include "DcmElementCollection.h"
#include "DcmUtility.h"


using namespace dcm;


#pragma region Create Static Methdod

/**
 * Creates the DcmImage object
 *  
 *  @param bEmpty
 *  @return pointer of the image
 */
std::shared_ptr<DcmImage> DcmImageFactory::Create(bool bEmpty)
{
  std::shared_ptr<DcmImage> pDcmImage(new DcmImage(bEmpty));

  if ( pDcmImage== __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocated DcmImage","DcmImageFactory","Create"));

  return pDcmImage;

} // DcmImage::Create()


/**
 * Creates the DcmImage object
 *
 *  @param pDcmDataset
 *  @return the pointer of the image
 */
std::shared_ptr<DcmImage> DcmImageFactory::Create(DcmDataset * pDcmDataset)
{

  std::shared_ptr<DcmImage> pDcmImage(new DcmImage(pDcmDataset));

  if ( pDcmImage== __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocated DcmImage","DcmImageFactory","Create"));

  return pDcmImage;

} // DcmImage::Create()


/**
 * Creates the DcmImage from the file specified
 *
 * @param sValue   String value
 *
 * @return DcmImage
 */
std::shared_ptr<DcmImage> DcmImageFactory::Create(const std::string & sParameter, const DcmParameterType & type)
{
   
  std::shared_ptr<DcmImage> pDcmImage(Create());

  try
  {
    if ( type == PT_HEADER )
      pDcmImage->Load(DcmElementCollection::Create(sParameter,type));
    else
      pDcmImage->Load(sParameter);
    
  }
  catch ( DcmException & ex) 
  {

    DcmLog::Error(ex.GetTextMessage(), "DcmImageFactory", "Create");

    throw;
  }
  catch ( ... )
  {
    throw DcmException(LogRec("Failed to create the DcmImage : unknown exception", "DcmImageFactory", "Create"));
  }

  return pDcmImage;

} // Create(const std::string & sFile)


/**
 * Creates the DcmImage from the xml node
 *
 * @param pNode   Xml node
 *
 * @return DcmImage
 */
std::shared_ptr<DcmImage> DcmImageFactory::Create(const xml::XmlNode & node)
{
  // Make sure the node was not NULL
  if ( ! node.IsValid() )
    throw DcmInvalidDataException(LogRec("Invalid xml node", "DcmImageFactory", "Create"));

  // Create an empty DcmImage
  std::shared_ptr<DcmImage> pDcmImage(Create());

  /// Loads the xml
  pDcmImage->Load(DcmElementCollection::Create(node));

  return pDcmImage;

} // (xmlNodePtr pNode)


/**
 * Creates the DcmImage from header and image file specified
 *
 * @param sHeader       Header (Xml or Serialized)
 * @param sFilename     Image file to use (Pdf, Bitmap, ... )
 * @param pPayload      Image payload (Sits behind the pixel data)
 * @param iPayloadSize  Size for the payload in bytes
 *
 * @return DcmImage
 */
std::shared_ptr<DcmImage> DcmImageFactory::Create(const std::string & sHeader, const std::string & sFilename, const std::shared_ptr<DcmBuffer> & pPayload )
{   
  try
  {

    std::vector<std::string> vFileSections = util::IO::SplitPath(sFilename);

    // =============================================================================================================
    // If the file extension is one that is supported by the Gdiplus Bitmap then create the bitmap and create the 
    // DcmImge from it. 
    // =============================================================================================================
    if (  vFileSections[PS_EXT] == ".bmp"   || vFileSections[PS_EXT] == ".png"  ||
          vFileSections[PS_EXT] == ".jpeg"  || vFileSections[PS_EXT] == ".jpg")
    {
      std::wstring wide;
      for ( std::string::const_iterator it = sFilename.cbegin(); it != sFilename.cend(); wide.push_back(static_cast<wchar_t>(*it)), it++ );

      std::shared_ptr<Gdiplus::Bitmap> pImage = std::shared_ptr<Gdiplus::Bitmap>(Gdiplus::Bitmap::FromFile(wide.c_str()));

      return Create(sHeader, pImage, pPayload);

    }
    else 
    {

      std::shared_ptr<DcmImage> pDcmImage(Create());

      if ( vFileSections[PS_EXT] == ".pdf" )
      {

        std::ifstream stream;
        struct _stati64 fileStat;

        try
        {
          // ======================================================
          // Get the file state to determine the size of the file
          // ======================================================
          if ( _stati64(sFilename.c_str(), &fileStat) != 0 )
            throw DcmNotFoundException(LogRec(util::Strings::Format("File %s not found",sFilename.c_str()),"DcmImageFactory","Create"));

          // ===============================
          // Make sure the file has data
          // ==============================
          if ( fileStat.st_size == 0 )
           throw DcmInvalidDataException(LogRec(util::Strings::Format("PDF file %s is empty",sFilename.c_str()),"DcmImageFactory","Create"));
 
          stream.open(sFilename.c_str(),std::ios::binary);

          if ( ! stream.is_open() )
            throw DcmIOException(LogRec(util::Strings::Format("Unable to read file %s",sFilename.c_str()),"DcmImageFactory","Create"));

          LoadPdf(pDcmImage, DcmBuffer::Create(stream), pPayload);

          stream.close();
        }
        catch ( dcm::DcmException & ex )
        {
          if ( stream.is_open() )
            stream.close();

          LogErr(util::Strings::Format("Failed to load file %s : %s",sFilename.c_str(),ex.GetTextMessage().c_str()), "DcmImageFactory","Create");

          throw;
        }
        catch ( ... )
        {
          if ( stream.is_open() )
            stream.close();

          throw dcm::DcmException(LogRec(util::Strings::Format("Failed to read PDF file %s : unspecified error", sFilename.c_str()), "DcmImageFactory", "Create"));

        }

      }         
      else
      {
        // Load Dicom File
        pDcmImage->Load(sFilename, true);
      }

      pDcmImage->Load(DcmElementCollection::Create( sHeader, PT_HEADER ) );

      return pDcmImage;

    } // else
  
  }
  catch ( DcmException & ex) 
  {

    DcmLog::Error(ex.GetTextMessage(), "DcmImageFactory", "Create");

    throw;

  }
  catch ( ... )
  {
    throw DcmException(LogRec("Failed to create the DcmImage : unknown exception", "DcmImageFactory", "Create"));
  }

} // Create(const std::string & sFilename, const std::string & sHeader, std::shared_ptr<DcmBuffer> & pPayload )


/**
 * Creates the DcmImage using the image data supplied
 *
 * @param sHeader         Header to initialize the image with
 * @param pDcmBuffer      Image buffer (If knows BMP, PNG, JPEG it will be loaded through a Bitmap, otherwise just the raw data is loaded )
 * @param eType           External image type ( If not ANY then validation will occur )
 *
 * @return DcmImage
 * 
 */
std::shared_ptr<DcmImage> DcmImageFactory::Create(const std::string & sHeader, const std::shared_ptr<DcmBuffer> & pImage,  const std::shared_ptr<DcmBuffer> & pPayload )
{

  std::shared_ptr<DcmImage> pDcmImage(Create());

  try
  {
    /// Loads an image into the pixel data
    LoadImage(pDcmImage, pImage, pPayload );
    
    /// Modifies the header
    pDcmImage->Load(DcmElementCollection::Create(sHeader, PT_HEADER));

  }
  catch ( DcmException & ex) 
  {

    DcmLog::Error(ex.GetTextMessage(), "DcmImageFactory", "Create");

    throw;

  }
  catch ( ... )
  {

    throw DcmException(LogRec("Failed to create the DcmImage : unknown exception", "DcmImageFactory", "Create"));
  }

  return pDcmImage;

} // Create(const std::string & sHeader, void * pImage, unsigned int iWidth, unsigned int iHeight, void * pPayload = __nullptr, unsigned int iPayloadSize = 0)


/**
 * Creates the DcmImage using the image data supplied
 *
 * @param sHeader         Header to initialize the image with
 * @param pImage      Image buffer
 * @param eType           External image type ( If not ANY then validation will occur )
 *
 * @return DcmImage
 */
std::shared_ptr<DcmImage> DcmImageFactory::Create(const std::string & sHeader, const std::shared_ptr<Gdiplus::Bitmap> & pImage,  const std::shared_ptr<DcmBuffer> & pPayload )
{

  std::shared_ptr<DcmImage> pDcmImage(Create());

  try
  {
    /// Initialize the image
    InitializeImage(pDcmImage);
     
    pDcmImage->SetPixelData(pImage, pPayload);

    /// Modifies the header
    pDcmImage->Load(DcmElementCollection::Create(sHeader, PT_HEADER));

  }
  catch ( DcmException & ex) 
  {

    DcmLog::Error(ex.GetTextMessage(), "DcmImageFactory", "Create");

    throw;

  }
  catch ( ... )
  {

    throw DcmException(LogRec("Failed to create the DcmImage : unknown exception", "DcmImageFactory", "Create"));
  }

  return pDcmImage;

} // Create(const std::string & sHeader, void * pImage, unsigned int iWidth, unsigned int iHeight, void * pPayload = __nullptr, unsigned int iPayloadSize = 0)


/**
 * Creates the DcmImage using the image data supplied
 *
 * @param pDcmElementCollection Contains the header
 * @param pDcmBuffer      Image buffer (If knows BMP, PNG, JPEG it will be loaded through a Bitmap, otherwise just the raw data is loaded )
 * @param eType           External image type ( If not ANY then validation will occur )
 *
 * @return DcmImage
 * 
 */
std::shared_ptr<DcmImage> DcmImageFactory::Create(const std::shared_ptr<DcmElementCollection> & pDcmElementCollection, const std::shared_ptr<DcmBuffer> & pImage,  const std::shared_ptr<DcmBuffer> & pPayload )
{

  std::shared_ptr<DcmImage> pDcmImage(Create());

  try
  {
    /// Loads an image into the pixel data
    LoadImage(pDcmImage, pImage, pPayload );
    
    /// Modifies the header
    pDcmImage->Load(pDcmElementCollection);

  }
  catch ( DcmException & ex) 
  {

    DcmLog::Error(ex.GetTextMessage(), "DcmImageFactory", "Create");

    throw;

  }
  catch ( ... )
  {

    throw DcmException(LogRec("Failed to create the DcmImage : unknown exception", "DcmImageFactory", "Create"));
  }

  return pDcmImage;

} // Create(const std::shared_ptr<DcmElementCollection> & pDcmElementCollection, void * pImage, unsigned int iWidth, unsigned int iHeight, void * pPayload = __nullptr, unsigned int iPayloadSize = 0)



#pragma endregion


#pragma region File Loading Methods


/**
 * Load the bitmap
 *
 * @param pDcmImage       Image to load
 * @param pImage          Bitmap image
 * @param pPayload        Payload to sit behind image
 * 
 * @return  DcmImage
 */
void DcmImageFactory::LoadBitmap(const std::shared_ptr<DcmImage> & pDcmImage, const std::shared_ptr<DcmBuffer> & pImage, const std::shared_ptr<DcmBuffer> & pPayload)
{

  HGLOBAL	hMemory = ::GlobalAlloc(GPTR,pImage->GetSize());
  if (!hMemory)
      throw DcmOutOfMemoryException(LogRec("Failed to allocate stream memory", "DcmImageFactory","LoadBitmap"));

      CoInitialize(NULL);
  try
  {

    memcpy((LPVOID)hMemory, pImage->GetData<char>(), pImage->GetSize());

    CComPtr<IStream> spStream;
    HRESULT hr = ::CreateStreamOnHGlobal(hMemory,FALSE,&spStream);

    /// Creates the bitmap from the stream
    std::shared_ptr<Gdiplus::Bitmap> pBitmap(Gdiplus::Bitmap::FromStream(spStream));

    spStream.Release();


    /// Initialize the image
    InitializeImage(pDcmImage);

    /// Sets the pixel data using the bitmap
    pDcmImage->SetPixelData(pBitmap, pPayload);

    /// Free the stream memory
    ::GlobalFree(hMemory);

       CoUninitialize();
  }
  catch ( DcmException &  )
  {

    /// Free the stream memory
    ::GlobalFree(hMemory);

    throw;
  }
  catch ( ... )
  {
    
    /// Free the stream memory
    ::GlobalFree(hMemory);

    throw DcmException(LogRec("Failed to load bitmap, unspecified error", "DcmImageFactory","LoadBitmap"));

  }

} // LoadBitmap(const std::shared_ptr<DcmImage> & pDcmImage, const std::string & sFilename, void * pPayload, unsigned int iPayloadSize)


/**
 * Reads the PDF file and loads into binary data element
 *
 * @param sFilename      Pdf file
 */
void DcmImageFactory::LoadPdf(const std::shared_ptr<DcmImage> & pDcmImage, const std::shared_ptr<DcmBuffer> & pImage, const std::shared_ptr<DcmBuffer> & pPayload )
{
  try
  {

    /// Get the PDF version
    std::string sVersion(GetPdfVersion(pImage));            

    /// Initialize DcmImage as a encapsulated PDF
    InitializePdf(pDcmImage);

    // Sets the encapsulated PDF binary element
    pDcmImage->SetValue<std::shared_ptr<DcmBuffer>>(DCM_EncapsulatedDocument,pImage);

  }
  catch(DcmException ex)
  {
    throw;    
  }
  catch ( ... )
  {

    throw DcmException(LogRec("PDF read failed, unspecified error","DcmImageFactory","LoadPdf"));
  }

} // LoadPdf(const std::shared_ptr<DcmImage> & pDcmImage, const std::string & sFilename, void * pPayload, unsigned int iPayloadSize)


#pragma endregion



/**
 * Loads the image
 *
 * @param pDcmImage       Dicom image to load
 * @param pDcmBuffer      Image 
 * @param eType           External data type ( If ANY then try to interpret otherwise just load into the pixel data area )
 *
 */
void DcmImageFactory::LoadImage(const std::shared_ptr<DcmImage> & pDcmImage, const std::shared_ptr<DcmBuffer> & pImage,  const std::shared_ptr<DcmBuffer> & pPayload  )
{
  try
  {

    // ==================================================
    // Switch on the type to load the proper image type
    // =================================================
    switch ( pImage->GetDataType() )
    {
    case DcmBuffer::DATA_PDF:
      LoadPdf(pDcmImage, pImage, pPayload);
      break;

    case DcmBuffer::DATA_PNG:
      LoadBitmap(pDcmImage, pImage, pPayload);
      break;

    case DcmBuffer::DATA_JPEG:
      LoadBitmap(pDcmImage, pImage, pPayload);
      break;

    case DcmBuffer::DATA_BITMAP:
      LoadBitmap(pDcmImage, pImage, pPayload);
      break;

    default:
      {
        // Initialize a regular image
        InitializeImage(pDcmImage);

        
        if ( pPayload != __nullptr )
        {
          std::shared_ptr<dcm::DcmBuffer> pBuffer = dcm::DcmBuffer::Create(pImage->GetSize() + pPayload->GetSize());


          unsigned char * pData(pBuffer->GetData<unsigned char>());

          // Load Image
          memcpy(pData, pImage->GetData<unsigned char>(), pImage->GetSize());

          // Load Payload
          memcpy((pData + pImage->GetSize()), pPayload->GetData<unsigned char>(), pPayload->GetSize());


          pDcmImage->SetPixelData(pBuffer);


        }
        else
          pDcmImage->SetPixelData(pImage);
        
      } // END ... Else we have just a regular image

    } // END ... Switch
 
  }
  catch(DcmException ex)
  {
    // Log error 
    LogErr(util::Strings::Format("Failed to load image : %s", ex.GetTextMessage().c_str()),"DcmImageFactory","LoadImage");

    // re-throw
    throw;
    
  }
  catch ( ... )
  {

    throw DcmException(LogRec("Loading of the image failed, unspecified error","DcmImageFactory","LoadImage"));
  }

} // LoadImage



/**
 * Gets the PDF version from the image buffer, also validates that it is PDF
 *
 * @param pImage          image buffer
 * @param iImageSize      Buffer size
 *
 * @return Version
 */
std::string DcmImageFactory::GetPdfVersion(const std::shared_ptr<DcmBuffer> & pDcmBuffer)
{
  // PDF version
  std::string sVersion;

  // Make sure the file is a PDF
  if (  pDcmBuffer->GetDataType() != DcmBuffer::DATA_PDF )
    throw DcmInvalidDataException(LogRec("Image is not a PDF","DcmImageFactory","GetPdfVersion"));

  bool bFound(false);

  // ========================================================
  // Make sure there is enough data to read for the version
  // ========================================================
  if ( pDcmBuffer->GetSize() >= 10 )
  {

    // Offset past the PDF tag so we can get the version
    char * ptr(pDcmBuffer->GetData<char>() + 5);

    // ============================================================
    // Loop through the next 5 characters looking for the version
    // ============================================================
    for (unsigned short i(0); i < 5; ++i)
    {
      // =======================================================================================
      // If the version is a BackSpace or a Vertical Tab then we are at the end of the version
      // =======================================================================================
      if ( ptr[i] == 10 || ptr[i] == 13)
      {
        sVersion.push_back('\0');
        bFound = true;
        break;
      }

      sVersion.push_back(ptr[i]);

    } // END ... For each on the next 5 characters

  } // END ... If the image buffer is big enough to have the version

  // If the version was not found
  if (! bFound)
    throw DcmInvalidDataException(LogRec("Unable to decode PDF version","DcmImageFactory","GetPdfVersion"));

  return sVersion;

} // GetPdfVersion(std::shared_ptr<DcmBuffer> & dcmBuffer)



#pragma region Initialization Methods

/**
 * Initializes the DcmImage for a Bitmap
 *
 * @param pDcmImage     Image to be initialized
 */
void DcmImageFactory::InitializeImage(const std::shared_ptr<DcmImage> & pDcmImage)
{

  dcm::DcmDate currentDate(dcm::DcmDate::Now());
  dcm::DcmTime currentTime(dcm::DcmTime::Now());

  // =======================================================================================
  // Setup the inclusions and exclusion list used when loading a header to seed this header
  // ========================================================================================

  // =======================================================================================================
  // Exclude the modality and conversion type from the 0008 group since we need to have a special setting
  // can be overridden by hand.
  // =======================================================================================================
  pDcmImage->m_exclusion.push_back(DCM_Modality);
  pDcmImage->m_exclusion.push_back(DCM_ConversionType);
  pDcmImage->m_exclusion.push_back(DCM_SOPClassUID);
  pDcmImage->m_exclusion.push_back(DCM_Manufacturer);
  pDcmImage->m_exclusion.push_back(DCM_ContentDate);
  pDcmImage->m_exclusion.push_back(DCM_ContentTime);
  pDcmImage->m_exclusion.push_back(DCM_ImageType);
  pDcmImage->m_exclusion.push_back(DCM_SOPInstanceUID);
  pDcmImage->m_exclusion.push_back(DCM_ImageOrientationPatient);

  pDcmImage->m_inclusion.push_back(DCM_StudyInstanceUID);
  pDcmImage->m_inclusion.push_back(DCM_SeriesInstanceUID);
  pDcmImage->m_inclusion.push_back(DCM_SeriesNumber);
  pDcmImage->m_inclusion.push_back(DCM_StudyID);

  // These three values should not be changed
  pDcmImage->SetAffectedSopClassUid(UID_SecondaryCaptureImageStorage);
  pDcmImage->SetPhotometricInterpretation("RGB");
  pDcmImage->SetImageType("DERIVED/SECONDARY");

  // ====================
  // Initialize values 
  // ====================

  // Study Date and Time
  pDcmImage->SetValue<dcm::DcmDate>(DCM_StudyDate, currentDate);
  pDcmImage->SetValue<dcm::DcmTime>(DCM_StudyTime, currentTime);

  // Series Date and Time
  pDcmImage->SetValue<dcm::DcmDate>(DCM_SeriesDate, currentDate);
  pDcmImage->SetValue<dcm::DcmTime>(DCM_SeriesTime, currentTime);

  pDcmImage->SetEmptyValue(DCM_AccessionNumber);
  pDcmImage->SetValue<std::string>(DCM_Manufacturer,"Viatronix Inc.");
  pDcmImage->SetEmptyValue(DCM_ReferringPhysicianName);
  pDcmImage->SetEmptyValue(DCM_StudyID);
  pDcmImage->SetValue<dcm::DcmDate>(DCM_ContentDate, currentDate);
  pDcmImage->SetValue<dcm::DcmTime>(DCM_ContentTime, currentTime);

  // insert const value attributes
  pDcmImage->SetValue<std::string>(DCM_SpecificCharacterSet, "ISO_IR 100");
 
  pDcmImage->SetValue<std::string>(DCM_SeriesNumber, "1");

  pDcmImage->SetEmptyValue(DCM_PatientName);
  pDcmImage->SetEmptyValue(DCM_PatientID);
  pDcmImage->SetEmptyValue(DCM_PatientBirthDate);
  pDcmImage->SetEmptyValue(DCM_PatientSex);


  pDcmImage->SetValue<std::string>(DCM_PlanarConfiguration, "0");
  pDcmImage->SetValue<std::string>(DCM_PixelRepresentation, "0");
  pDcmImage->SetValue<std::string>(DCM_Modality, "OT");
  pDcmImage->SetValue<std::string>(DCM_ConversionType, "WSD");

  pDcmImage->SetEmptyValue(DCM_InstanceNumber);

} // InitializeBitmap(const std::shared_ptr<DcmImage> & pDcmImage)


/**
 * Initializes the DcmImage for a Pdf
 *
 * @param pDcmImage     Image to be initialized
 */
void DcmImageFactory::InitializePdf(const std::shared_ptr<DcmImage> & pDcmImage)
{
  dcm::DcmDate currentDate(dcm::DcmDate::Now());
  dcm::DcmTime currentTime(dcm::DcmTime::Now());

  // =======================================================================================
  // Setup the inclusions and exclusion list used when loading a header to seed this header
  // ========================================================================================

  // =======================================================================================================
  // Exclude the modality and conversion type from the 0008 group since we need to have a special setting
  // can be overridden by hand.
  // =======================================================================================================
  pDcmImage->m_exclusion.push_back(DCM_Modality);
  pDcmImage->m_exclusion.push_back(DCM_ConversionType);
  pDcmImage->m_exclusion.push_back(DCM_SOPClassUID);
  pDcmImage->m_exclusion.push_back(DCM_Manufacturer);
  pDcmImage->m_exclusion.push_back(DCM_ContentDate);
  pDcmImage->m_exclusion.push_back(DCM_ContentTime);
  pDcmImage->m_exclusion.push_back(DCM_SOPInstanceUID);

  // =========================================================================================
  // Set up the list of inclusion tags these are tags that are not 0008, 0010, 0032 and 0038
  // =========================================================================================
  pDcmImage->m_inclusion.push_back(DCM_ConceptNameCodeSequence);
  pDcmImage->m_inclusion.push_back(DCM_StudyInstanceUID);
  pDcmImage->m_inclusion.push_back(DCM_SeriesInstanceUID);
  pDcmImage->m_inclusion.push_back(DCM_SeriesNumber);
  pDcmImage->m_inclusion.push_back(DCM_StudyID);

  // These two value should not be changed
  pDcmImage->SetValue<std::string>(DCM_SOPClassUID, UID_EncapsulatedPDFStorage);               // Added to exclusion list
  pDcmImage->SetValue<std::string>(DCM_MIMETypeOfEncapsulatedDocument, "application/pdf");     // By not adding to the inclusion list it will not be set

  // ==========================
  // Initialize base values 
  // ==========================

  // Study Date and Time
  pDcmImage->SetValue<dcm::DcmDate>(DCM_StudyDate, currentDate);
  pDcmImage->SetValue<dcm::DcmTime>(DCM_StudyTime, currentTime);

  // Series Date and Time
  pDcmImage->SetValue<dcm::DcmDate>(DCM_SeriesDate, currentDate);
  pDcmImage->SetValue<dcm::DcmTime>(DCM_SeriesTime, currentTime);


  pDcmImage->SetValue<std::string>(DCM_Manufacturer,"Viatronix Inc.");
  pDcmImage->SetValue<dcm::DcmDate>(DCM_ContentDate, currentDate);
  pDcmImage->SetValue<dcm::DcmTime>(DCM_ContentTime, currentTime);

  pDcmImage->SetEmptyValue(DCM_AccessionNumber);
  pDcmImage->SetEmptyValue(DCM_ReferringPhysicianName);
  pDcmImage->SetEmptyValue(DCM_StudyID);
 
  pDcmImage->SetEmptyValue(DCM_ConceptNameCodeSequence);

  // insert const value attributes
  pDcmImage->SetValue<std::string>(DCM_SpecificCharacterSet, "ISO_IR 100");
  pDcmImage->SetValue<std::string>(DCM_Modality, "OT");
  pDcmImage->SetValue<std::string>(DCM_ConversionType, "WSD");
 
  pDcmImage->SetValue<std::string>(DCM_SeriesNumber, "1");

  pDcmImage->SetValue<std::string>(DCM_DocumentTitle, "PDF Report");

  pDcmImage->SetEmptyValue(DCM_PatientName);
  pDcmImage->SetEmptyValue(DCM_PatientID);
  pDcmImage->SetEmptyValue(DCM_PatientBirthDate);
  pDcmImage->SetEmptyValue(DCM_PatientSex);

  // Either YES or NO
  pDcmImage->SetValue<std::string>(DCM_BurnedInAnnotation,  "NO");

  pDcmImage->SetEmptyValue(DCM_InstanceNumber);

} //InitializePdf(const std::shared_ptr<DcmImage> & pDcmImage)

#pragma endregion

