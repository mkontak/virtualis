// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: PacsClient.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


// includes
#include "stdafx.h"
#include "DcmEnums.h"
#include "PacsClient.h"

// using
using namespace pacs;


/**
 * Creates the PACS object linking it to the host name specified. The host information 
 * is looked up in the host table
 *
 * @param sHostName     Host name
 */
PacsClient::PacsClient(const std::string & sAETitle) :
m_sAETItle(sAETitle),
m_iPdfPageCounter(0),
m_sPdfSeriesInstanceUid(dcm::DcmUtility::GenerateUid()),
m_iSnapshotCounter(0),
m_iNetworkTimeout(DCM_DefaultNetworkTimeout),
m_sSnapshotSeriesInstanceUid(dcm::DcmUtility::GenerateUid()),
m_blockingMode(dcm::DCM_BLOCKING),
m_iAcquisitionCounter(0)
{ 
} //PacsClient


/**
 * Creates the PACS client and returns the shared pointer
 *
 * @param sAETitle      AE title of this client
 * @param pServer       Server connection information
 *
 * @return Shared pointer to PacsClient
 */
std::shared_ptr<PacsClient> PacsClient::Create(const std::string & sAETitle, std::shared_ptr<dcm::DcmHost> & pServer ) 
{

  std::shared_ptr<PacsClient> pPacsClient( new PacsClient(sAETitle) );

  if ( pPacsClient == __nullptr )
    throw dcm::DcmOutOfMemoryException(LogRec("Failed to create pacs client","PacsClient","Create"));
  
  pPacsClient->m_server = pServer;

  return pPacsClient;

} // Create(const std::string & sAETitle, const std::string & sHostname) 


/**
 * Creates the PACS client and returns the shared pointer
 *
 * @param sAETitle      AE title of this client
 * @param sHostname     Host name to connection to (Lookup in HoatsTable)
 *
 * @return Shared pointer to PacsClient
 */
std::shared_ptr<PacsClient> PacsClient::Create(const std::string & sAETitle, const std::string & sHostname ) 
{
  return Create(sAETitle, dcm::DcmHostTable::GetCurrentHostTable()->GetHostByName(sHostname));
} // Create(const std::string & sAETitle, const std::string & sHostname) 


/**
 *  wait
 *
 *  @param handle
 *  @param waitTime
 *  @return
 */
DWORD PacsClient::Wait(HANDLE handle, DWORD waitTime)
{
  return WaitForSingleObject(handle, waitTime);
} //  PacsClient(HANDLE handle, , DWORD waitTime)


/**
 *  wait
 *
 *  @param waitTime
 *  @return 
 */
DWORD PacsClient::Wait(DWORD waitTime)
{
  DWORD dwReturn(0);

  if ( m_actions.size() > 0 )
  {

    int i(0);

    HANDLE * handles = new HANDLE [m_actions.size()];

    // Loads the action item handles into the list
    std::for_each( m_actions.begin(), m_actions.end(), [&handles, &i]( ACTIONITEM actionItem )
    {
      handles[i++] = actionItem.first;
    });
 
    dwReturn = WaitForMultipleObjects(i,handles, TRUE, waitTime);

  }

  return dwReturn;

} //  PacsClient(HANDLE handle, , DWORD waitTime)


/**
 * Adds the specified event handler to the list
 *
 * @param evenetHandler     Function pointer to the handler
 * @param lpData            Data pointer to be returned during the callback
 */
void PacsClient::AddEventHandler(std::shared_ptr<PacsEventHandler> & eventHandler)
{

  bool found(false);
  
  // =======================
  // Search for the handler
  // =======================
  std::for_each( m_eventHandlers.begin(), m_eventHandlers.end(), [&eventHandler, &found ] ( std::shared_ptr<PacsEventHandler> handler )
  {
    if ( handler == eventHandler )
    {
      found = true;
      return;
    } //if
 
  });

  // =========================================
  // If the handler was not found then add it
  // =========================================
  if ( ! found )
   m_eventHandlers.push_back(eventHandler);
  
} // AddEventHandler(dcm::DCM_CALLBACK_FUNCTION eventHandler, LPVOID lpData)


/**
 * Removes the specified event handler from the list
 *
 * @param sName     Handler name
 */
void PacsClient::RemoveEventHandler(const std::string & sName)
{  
  for ( EVENT_HANDLER_LIST::const_iterator it = m_eventHandlers.cbegin(); it != m_eventHandlers.cend(); it++ )
  {
    if ( (*it)->GetName() == sName )
      m_eventHandlers.erase(it);
  }

} // AddEventHandler(dcm::DCM_CALLBACK_FUNCTION eventHandler, LPVOID lpData)


/**
 * Performs the callback to all event handlers.
 *
 * @param pParameters     Parameters to send to the callback
 */
void PacsClient::Callback(dcm::PDCM_CALLBACK_PARAMETERS parameters)
{

  // ==========================
  // Call all event handlers
  // ==========================
  std::for_each( m_eventHandlers.begin(), m_eventHandlers.end(), [parameters] ( std::shared_ptr<PacsEventHandler> handler )
  {
    handler->Callback(parameters);
  });

}  // Callback(dcm::PDCM_CALLBACK_PARAMETERS pParameters)



/**
 * Handles the association callbacks
 *
 * @param pParameters       Parameters from the association
 * @param lpCallbackData    Callback data 
 */
void PacsClient::ActionCallback(dcm::PDCM_CALLBACK_PARAMETERS pParameters, LPVOID lpCallbackData )
{

  // Callback data is pointer to the server that started the association
  PacsClient * pPacsClient(reinterpret_cast<PacsClient *>(lpCallbackData));

  pPacsClient->Callback(pParameters);

} // ActionCallback(dcm::PDCM_CALLBACK_PARAMETERS pParameters, LPVOID lpCallbackData )



/**
 * Stores the supplied PDF
 *
 * @param pPdf      Pdf
 */
void PacsClient::StorePdf(const std::string & sHeader, std::shared_ptr<pdf::Pdf> & pPdf)
{
  
  DcmTag tag(0x0020,0x000e);

  std::shared_ptr<dcm::DcmElementCollection> list(GetElements(sHeader));

  // Create the DICOM store object that will do all the work
  std::shared_ptr<dcm::DcmStore> pDcmStore(dcm::DcmStore::Create(m_sAETItle, m_server));

  // Sets the client network timeout
  pDcmStore->SetNetworkTimeout(m_iNetworkTimeout);

  // Sets the blocking mode
  pDcmStore->SetBlockingMode(m_blockingMode);

  // ==============================================================
  // If the attribute exists and is set to true then encapsulate
  // ==============================================================
  if ( m_server->IsEncapsulatedPdfSupported()  )
  {
    std::shared_ptr<pdf::PdfBuffer> pBuffer(pPdf->ToPdfBuffer());

    std::shared_ptr<dcm::DcmImage> image(dcm::DcmImageFactory::Create());
    image->SetSeriesInstanceUid(dcm::DcmUtility::GenerateUid());

    std::shared_ptr<dcm::DcmElement> element(__nullptr);
    list->Reset();
    while ( list->MoveNext() )
    {
      image->Insert(list->Current());
    }

    image->SetInstanceNumber(1);
    image->SetPixelData(dcm::DcmBuffer::Create(pBuffer->TakeOwnership<unsigned char>(), pBuffer->GetSize(), dcm::DcmBuffer::DATA_PDF));

    // Creates the image from the PDF file stream
    pDcmStore->AddImage(image);
  }
  else
  {
    BITMAPLIST images = pPdf->GetPages();



    // =====================================
    // Loop through all the pdf page images
    // =====================================
    for ( register int i(0); i < images.size(); i++)
    {
      std::shared_ptr<dcm::DcmImage> image(dcm::DcmImageFactory::Create());

      std::shared_ptr<dcm::DcmElement> element(__nullptr);
      list->Reset();
      while ( list->MoveNext() )
      {
        image->Insert(list->Current());
      }

      image->SetInstanceNumber(i+1);
      image->SetPixelData(images[i]);
      pDcmStore->AddImage(image);
    } // END ... For each image

  }

  // Store
  pDcmStore->RunSynchronously();

} // StorePdf(std::shared_ptr<pdf::Pdf> & pPdf)

/**
 * Stores the the snapshots to the pacs pointing to.
 *
 * @param sHeader       Dicom header xml
 * @param images        Image collection
 * @param wait          Flag to allow waiting or to have the method start the action thread and return
 *
 * @return HANDLE allowing waiting outside this object. INVALID_HANDLE is returned if no images are specified
 */
HANDLE PacsClient::StoreImages( const std::string & sHeader, const BITMAPLIST & images,  unsigned int & iCounter, bool wait )
{
  // ==================================================================
  // Make sure there are images, if not log a warning and just return.
  // ==================================================================
  if ( images.empty() )
  {
    LogWrn("No images received","PacsClient", "StoreSnapshots");
    return INVALID_HANDLE_VALUE;
  } // END ... No images 


  std::shared_ptr<dcm::DcmElementCollection> list(GetElements(sHeader));

  // Create the DICOM store object that will do all the work
  std::shared_ptr<dcm::DcmStore> pDcmStore(dcm::DcmStore::Create(m_sAETItle, m_server));

  // Sets the client network timeout
  pDcmStore->SetNetworkTimeout(m_iNetworkTimeout);

  // Sets the blocking mode
  pDcmStore->SetBlockingMode(m_blockingMode);

  // ===============================================================================================================================
  // Create a DcmImage for each bitmap image. If this uses too much resources another model could be used were a single image is 
  // created sent then destroyed.
  // ===============================================================================================================================
  std::for_each( images.begin(), images.end(), [this, &list, &iCounter, &pDcmStore ] ( std::shared_ptr<Gdiplus::Bitmap> image )
  {
 
    std::shared_ptr<dcm::DcmImage> pDcmImage(dcm::DcmImageFactory::Create());

     // Set the image number
    pDcmImage->SetInstanceNumber(++iCounter);

    // Set the series instance UID
    pDcmImage->SetSeriesInstanceUid(dcm::DcmUtility::GenerateUid());

    std::shared_ptr<dcm::DcmElement> element(__nullptr);
    list->Reset();
    while ( list->MoveNext() )
    {
      pDcmImage->Insert(list->Current());
    }

    pDcmImage->SetPixelData(image);

 
 
    pDcmStore->AddImage(pDcmImage);

  } ); // END ... For each image in the list

  HANDLE handle(pDcmStore->GetHandle());

  /// Add the action item to the list
  m_actions.insert(ACTIONITEM(handle,std::dynamic_pointer_cast<dcm::DcmClient>(pDcmStore)));

  if ( wait )  
    pDcmStore->RunSynchronously();
  else
    pDcmStore->Start(this, ActionCallback);

  return handle;

} // StoreImages( const std::string & sHeader, const IMAGELIST & images, bool wait )


/**
 * Stores the supplied image to the PACS
 *
 * @param sHeader             Dicom header xml
 * @param image               Image to be stored
 * @param sSeriesInstanceUid  Snapshot series uid 
 * @param iCounter            Snapshot image number
 */
void PacsClient::StoreImage( const std::string & sHeader, std::shared_ptr<Gdiplus::Bitmap> & image, const std::string & sSeriesInstanceUid, unsigned int & iCounter )
{

 std::shared_ptr<dcm::DcmElementCollection> list(GetElements(sHeader));

  // Create the DICOM store object that will do all the work
  std::shared_ptr<dcm::DcmStore> pDcmStore(dcm::DcmStore::Create(m_sAETItle, m_server));

  // Sets the client network timeout
  pDcmStore->SetNetworkTimeout(m_iNetworkTimeout);

    // Sets the blocking mode
  pDcmStore->SetBlockingMode(m_blockingMode);

  std::shared_ptr<dcm::DcmImage> pDcmImage(dcm::DcmImageFactory::Create());

    // Set the image number
  pDcmImage->SetInstanceNumber(++iCounter);

  // Set the series instance UID
  pDcmImage->SetSeriesInstanceUid(dcm::DcmUtility::GenerateUid());

  std::shared_ptr<dcm::DcmElement> element(__nullptr);
  list->Reset();
  while ( list->MoveNext() )
  {
    pDcmImage->Insert(list->Current());
  }

  pDcmImage->SetPixelData(image);

  /// Add image
  pDcmStore->AddImage(pDcmImage);

  /// Store
  pDcmStore->RunSynchronously();
  
} // StoreImage( const std::string & sHeader, const std::shared_ptr<Gdiplus::Bitmap> image, unsigned int & iCounter )


#pragma region add methods

/**
 * Adds image to the list
 *
 * @param sHeader Dicom header
 * @param pPdf    PDF to be added
 *
 */
void PacsClient::AddPdf(const std::string & sHeader,std::shared_ptr<pdf::Pdf> & pPdf)
{

  std::shared_ptr<dcm::DcmElementCollection> list(GetElements(sHeader));


  if ( m_server->IsEncapsulatedPdfSupported() )
  {
    std::shared_ptr<pdf::PdfBuffer> pBuffer(pPdf->ToPdfBuffer());

    std::shared_ptr<dcm::DcmImage> pDcmImage(dcm::DcmImageFactory::Create());

    std::shared_ptr<dcm::DcmElement> element(__nullptr);
    list->Reset();
    while ( list->MoveNext() )
    {
      pDcmImage->Insert(list->Current());
    }

    pDcmImage->SetSeriesInstanceUid(m_sPdfSeriesInstanceUid);
    pDcmImage->SetInstanceNumber(1);
    pDcmImage->SetPixelData(dcm::DcmBuffer::Create(pBuffer->TakeOwnership<unsigned char>(), pBuffer->GetSize(), dcm::DcmBuffer::DATA_PDF));

    // Creates the image from the PDF file stream
    m_pdfImages.push_back(pDcmImage);
  }
  else
  {
    BITMAPLIST images = pPdf->GetPages();

    int iCounter(0);

    // =====================================================================
    // Loop through all the bitmap images and add to the store collection
    // ====================================================================
    std::for_each( images.begin(), images.end(), [ this, &list, &iCounter ] ( std::shared_ptr<Gdiplus::Bitmap> image ) 
    {
       std::shared_ptr<dcm::DcmImage> pDcmImage(dcm::DcmImageFactory::Create());

      std::shared_ptr<dcm::DcmElement> element(__nullptr);
      list->Reset();
      while ( list->MoveNext() )
      {
        pDcmImage->Insert(list->Current());
      }

      pDcmImage->SetInstanceNumber(++iCounter);
      pDcmImage->SetSeriesInstanceUid(m_sPdfSeriesInstanceUid);

      pDcmImage->SetPixelData(image);

      m_pdfImages.push_back(pDcmImage);

    });
  } // else

} // AddPdf(const std::string & sHeader,std::shared_ptr<pdf::Pdf> & pPdf)



/**
 * Adds image to the list
 *
 * @param sHeader Dicom header
 * @param image   Bitmap image
 *
 */
void PacsClient::AddSnapshot(const std::string & sHeader, std::shared_ptr<Gdiplus::Bitmap> & snapshot)
{
  std::shared_ptr<dcm::DcmElementCollection> list(GetElements(sHeader));

  std::shared_ptr<dcm::DcmImage> pDcmImage(dcm::DcmImageFactory::Create());

  std::shared_ptr<dcm::DcmElement> element(__nullptr);
  list->Reset();
  while ( list->MoveNext() )
  {
    pDcmImage->Insert(list->Current());
  }

  pDcmImage->SetInstanceNumber(++m_iSnapshotCounter);
  pDcmImage->SetSeriesInstanceUid(m_sPdfSeriesInstanceUid);

  pDcmImage->SetPixelData(snapshot);

  m_snapshotImages.push_back(pDcmImage);

} // AddImage(const std::string & sHeader,std::shared_ptr<Gdiplus::Bitmap> & snapshot)


/**
 * Adds image to the list
 *
 * @param sHeader Dicom header
 * @param snapshots   LIst of snapshot images
 *
 */
void PacsClient::AddSnapshots(const std::string & sHeader, BITMAPLIST snapshots)
{

  std::for_each( snapshots.begin(), snapshots.end(), [this, &sHeader ] ( std::shared_ptr<Gdiplus::Bitmap> snapshot )
  {
     AddSnapshot(sHeader, snapshot);
  });

} // AddSnapshots(const std::string & sHeader, BITMAPLIST snapshots)



#pragma endregion

/**
 * Stores all dicom files ijn the directory specified
 *
 * @param sDirectory        Directory
 * @param sPattern          Pattern
 * @param wait              Wait flag
 * @return handle
 */
HANDLE PacsClient::StoreDirectory(const std::string & sDirectory, const std::string & sPattern, bool wait)
{
  HANDLE handle(__nullptr);

  // Create the DICOM store object that will do all the work
  std::shared_ptr<dcm::DcmStore> pDcmStore(dcm::DcmStore::Create(m_sAETItle, m_server));

  // Sets the client network timeout
  pDcmStore->SetNetworkTimeout(m_iNetworkTimeout);

  // Sets the blocking mode
  pDcmStore->SetBlockingMode(m_blockingMode);


  // Adds directory
  pDcmStore->AddDirectory(sDirectory, sPattern);

  if ( wait ) 
    pDcmStore->RunSynchronously();
  else
    handle = pDcmStore->Start(this, ActionCallback);

  return handle;

} // StoreDirectory(const std::string & sDirectory, const std::string & sPattern, bool wait)


/**
 * Stores all dicom files in the directory specified recursivly
 *
 * @param sDirectory        Directory
 * @param sPattern          Pattern
 * @param wait              Wait flag
 * @return handle
 */
HANDLE PacsClient::StoreDirectoryRecursive(const std::string & sDirectory, const std::string & sPattern, bool wait)
{
  HANDLE handle(__nullptr);

  // Create the DICOM store object that will do all the work
  std::shared_ptr<dcm::DcmStore> pDcmStore(dcm::DcmStore::Create(m_sAETItle, m_server));

  // Sets the client network timeout
  pDcmStore->SetNetworkTimeout(m_iNetworkTimeout);

  // Sets the blocking mode
  pDcmStore->SetBlockingMode(m_blockingMode);

  // Adds directory
  pDcmStore->AddDirectory(sDirectory, sPattern, true);

  if ( wait ) 
    pDcmStore->RunSynchronously();
  else
    handle = pDcmStore->Start(this, ActionCallback);

  return handle;

} // StoreDirectoryRecursive(const std::string & sDirectory, const std::string & sPattern, bool wait)


/**
 * Stores the list of images in the PACS client
 *
 * @param   wait          True if we do not want threading and we are going to wait, false if we want a threaded store.
 * @param   singleSeries  Flag to indicate that all images are store in the same series that means the report and snapshots
 *
 * @return HANDLE for thread, if there were not images to store then __nullptr is returned
 */
HANDLE PacsClient::Store(bool wait, bool singleSeries)
{

  HANDLE handle(__nullptr);

  if ( m_pdfImages.size() > 0  || m_snapshotImages.size() > 0  )
  {
    // Create the DICOM store object that will do all the work
    std::shared_ptr<dcm::DcmStore> pDcmStore(dcm::DcmStore::Create(m_sAETItle, m_server));

    // Sets the client network timeout
    pDcmStore->SetNetworkTimeout(m_iNetworkTimeout);

    // Sets the blocking mode
    pDcmStore->SetBlockingMode(m_blockingMode);

    int iCounter(0);

    // =======================================================================================================================
    // If we are a single series and not encapsulated then use the snapshots uid. Otherwise we need to separate the series
    // into snapshot and reports series.
    // ======================================================================================================================
    std::string sUid( ( singleSeries && !m_server->IsEncapsulatedPdfSupported() ) ? m_sSnapshotSeriesInstanceUid : m_sPdfSeriesInstanceUid );

    // Add all the pdf page images
    std::for_each(m_pdfImages.begin(), m_pdfImages.end(), [&pDcmStore, &iCounter, &sUid ] ( std::shared_ptr<dcm::DcmImage> pDcmImage )
    {
      pDcmImage->SetSeriesInstanceUid(sUid);
      pDcmImage->SetInstanceNumber(++iCounter);
      pDcmStore->AddImage(pDcmImage);
    });

    // ======================================================================================================
    // If we are single series and we do not support encapsulated pdf then continue the counter otherwise 
    // reset it back to 0. If encapsulated pdf is supported then we need to separate the two series.
    // ======================================================================================================
    iCounter  =  ( ( singleSeries && !m_server->IsEncapsulatedPdfSupported() ) ? iCounter : 0 );    
    sUid      = m_sSnapshotSeriesInstanceUid;

    // Add all the snapshot images
    std::for_each(m_snapshotImages.begin(), m_snapshotImages.end(), [&pDcmStore, &iCounter, &sUid ] ( std::shared_ptr<dcm::DcmImage> pDcmImage )
    {
      pDcmImage->SetSeriesInstanceUid(sUid);
      pDcmImage->SetInstanceNumber(++iCounter);
      pDcmStore->AddImage(pDcmImage);
    });
    
    /// Get the store handle
    handle = pDcmStore->GetHandle();

    /// Add the action item to the list
    m_actions.insert(ACTIONITEM(handle,std::dynamic_pointer_cast<dcm::DcmClient>(pDcmStore)));


    /// If the wait flag is set run the store without threading
    if ( wait )
      pDcmStore->RunSynchronously();
    else
      handle = pDcmStore->Start(this, ActionCallback);    
  } // if

  return handle;

} // Store(bool wait)


/**
 * Clears the PDF images
 */
void PacsClient::ClearPdfImages()
{

   // Reset all dicom images (releases the resource )
   std::for_each( m_pdfImages.begin(), m_pdfImages.end(), [] ( std::shared_ptr<dcm::DcmImage> pDcmImage ) { pDcmImage.reset(); } );


   m_pdfImages.clear();

} // ClearPdfImages

/**
 * Gets the seeding elements from the header
 *
 * @param sHeader       Header
 *
 * @return DcmElementList
 */
std::shared_ptr<dcm::DcmElementCollection> PacsClient::GetElements(const std::string & sHeader)
{

  std::shared_ptr<dcm::DcmElementCollection> elements(dcm::DcmElementCollection::Create());
  //std::vector<std::shared_ptr<dcm::DcmElement>> elements;

  std::shared_ptr<dcm::DcmElement> element(__nullptr);

  std::shared_ptr<dcm::DcmElement> patientDob(__nullptr);
  std::shared_ptr<dcm::DcmElement> seriesUid(__nullptr);

  std::shared_ptr<dcm::DcmElementCollection> elementList(dcm::DcmElementCollection::Create(sHeader, dcm::DcmParameterType::PT_HEADER) );

  elementList->TryGetElement(DcmTag(0x0020,0x000e), seriesUid);        // Study Date

  // Study Date
  if ( elementList->TryGetElement(DcmTag(0x0008,0x0020), element) )
    elements->Add(element);

  // Study Time
  if ( elementList->TryGetElement(DcmTag(0x0008,0x0030), element) )
    elements->Add(element);

  // Modality
  if ( elementList->TryGetElement(DcmTag(0x0008,0x0060), element) )
    elements->Add(element);

  // Institution
  if ( elementList->TryGetElement(DcmTag(0x0008,0x0080), element) )
    elements->Add(element);

  // Accession
  if ( elementList->TryGetElement(DcmTag(0x0008,0x0050), element) )
    elements->Add(element);

  // Patient name
  if ( elementList->TryGetElement(DcmTag(0x0010,0x0010), element) )
    elements->Add(element);

  // Patient Id
  if ( elementList->TryGetElement(DcmTag(0x0010,0x0020), element) )
    elements->Add(element);
  
  // Patient Sex
  if ( elementList->TryGetElement(DcmTag(0x0010,0x0040), element) )
    elements->Add(element);

  // Patient Age
  if ( elementList->TryGetElement(DcmTag(0x0010,0x1010), element) )
    elements->Add(element);

  // Patient Dob
  if ( elementList->TryGetElement(DcmTag(0x0010,0x0030), element) )
    elements->Add(element);

  // Study Description
  if ( elementList->TryGetElement(DcmTag(0x0008,0x1030), element) )
    elements->Add(element);

  // Study Uid
  if ( elementList->TryGetElement(DcmTag(0x0020,0x000d), element) )
    elements->Add(element);

  // Study Id
  if ( elementList->TryGetElement(DcmTag(0x0020,0x0010), element) )
    elements->Add(element);

  std::shared_ptr<dcm::DcmSequence> sequence(dcm::DcmSequence::Create(DcmTag(0x0008, 0x1115)));
  std::shared_ptr<dcm::DcmElementCollection> list(dcm::DcmElementCollection::Create());
  list->Add(seriesUid);
  sequence->Add(list);
  elements->Add(std::dynamic_pointer_cast<dcm::DcmElement>(sequence));

  element = dcm::DcmElement::Create(DcmTag(0x0008,0x0021),dcm::DcmDate::Now());
  elements->Add(element);

  element = dcm::DcmElement::Create(DcmTag(0x0008,0x0031),dcm::DcmTime::Now());
  elements->Add(element);
    
  element = dcm::DcmElement::Create(DcmTag(0x0008,0x0070),"Viatronix Inc.");
  elements->Add(element);
 
  return elements;

}