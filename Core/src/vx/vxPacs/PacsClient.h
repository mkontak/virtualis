// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: PacsClient.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)


namespace pacs
{

#pragma region typedefs

// =================================================
// Type definitions used within the Pacs Client
// =================================================
typedef std::vector<std::shared_ptr<Gdiplus::Bitmap>> BITMAPLIST;        // Input to StoreImages
typedef std::map<HANDLE,std::shared_ptr<dcm::DcmClient>> ACTIONLIST;
typedef std::pair<HANDLE,std::shared_ptr<dcm::DcmClient>> ACTIONITEM;

#pragma endregion



#pragma region event handler types


/// Event Handler structure hold the function pointer and the callback data
class PACS_DLL PacsEventHandler
{
private:

  /// Constructor
  PacsEventHandler(const std::string & sName, LPVOID data, std::function< void ( dcm::PDCM_CALLBACK_PARAMETERS, LPVOID ) > function) :
  m_sName(sName),
  m_callbackData(data),
  m_callbackFunction(function)
  { }

public:

  /// Performs the callback
  void Callback(dcm::PDCM_CALLBACK_PARAMETERS parameters )
  {
    if ( m_callbackFunction != __nullptr )
      (m_callbackFunction)( parameters, m_callbackData );
  } // Callback()

  /// Creates the handler
  static std::shared_ptr<PacsEventHandler> Create( const std::string & sName, LPVOID data, std::function< void ( dcm::PDCM_CALLBACK_PARAMETERS, LPVOID ) > function )
  {
    std::shared_ptr<PacsEventHandler> handler(new PacsEventHandler(sName, data, function));

    if ( handler == __nullptr )
        throw dcm::DcmOutOfMemoryException(LogRec("Failed to allocate memory for PacsEventHandler", "PacsEventHandler", "Create"));

    return handler;

  } // Create( const std::string & sName, LPVOID data, std::function< void ( dcm::PDCM_CALLBACK_PARAMETERS, LPVOID ) > function )

  /// = operator
  inline bool operator==(const std::shared_ptr<PacsEventHandler> & handler) const
  { return ( handler->GetName() == m_sName ); }


public:

  /// Gets the event handler name
  const std::string & GetName() 
  { return m_sName; }


private:

  /// Name
  std::string m_sName;                                                              

  /// Callback function
  std::function< void ( dcm::PDCM_CALLBACK_PARAMETERS, LPVOID ) > m_callbackFunction;      

  /// Callback data
  LPVOID m_callbackData;    

 
};  // class PacsEventHandler

// Type definition for EVENT handler list
typedef std::vector<std::shared_ptr<PacsEventHandler>> EVENT_HANDLER_LIST;

#pragma endregion



#pragma region PacsClient

/**
 * Implements a PACS interface so that a more automated process can be used 
 */
class PACS_DLL PacsClient 
{

protected:

  /// Constructor 
  PacsClient(const std::string & sAETitle = "" );


public:

  /// Add event handler
  void AddEventHandler(std::shared_ptr<PacsEventHandler> & handler );

  /// Removes the handler
  void RemoveEventHandler(std::shared_ptr<PacsEventHandler> & handler)
  { RemoveEventHandler(handler->GetName()); }

  /// Remove event handler
  void RemoveEventHandler(const std::string & sName);

  /// Creates the pacs client using the aetitle and the host name (Performs a host table lookup)
  static std::shared_ptr<PacsClient> Create(const std::string & sAETitle, const std::string & sHostname );

  /// Creates the pacs client using the aetitle and server host entry
  static std::shared_ptr<PacsClient> Create(const std::string & sAETitle, std::shared_ptr<dcm::DcmHost> & pServer);

  /// Waits
  DWORD Wait(HANDLE handle, DWORD waitTime = INFINITE);

  /// Waits
  DWORD Wait(DWORD waitTime = INFINITE);

  /// Stores all files in the directory
  HANDLE StoreDirectory(const std::string & sDirectory, const std::string & sPattern = "*.dcm", bool wait = false );

  /// Stores all files in the directory
  HANDLE StoreDirectoryRecursive(const std::string & sDirectory, const std::string & sPattern = "*.dcm", bool wait = false );


  /// Stores a collection of pdf pages to the PACS
  HANDLE StorePdf(const std::string & sHeader, const BITMAPLIST & images, bool wait = false)
  { unsigned int iPage(0); return StoreImages(sHeader, images, iPage, wait); }

  /// Stores a collection of snapshots to the PACS
  HANDLE StoreSnapshots(const std::string & sHeader, const BITMAPLIST & images, bool wait = false)
  { return StoreImages(sHeader, images, m_iSnapshotCounter, wait); }

  /// Stores a single snapshot to the PACS
  void StoreSnapshot(const std::string & sHeader, std::shared_ptr<Gdiplus::Bitmap> & image)
  { StoreImage(sHeader, image, m_sSnapshotSeriesInstanceUid, m_iSnapshotCounter); }
  
  /// Stores the supplied PDF
  void StorePdf(const std::string & sHeader, std::shared_ptr<pdf::Pdf> & pPdf);

  /// Adds the supplied PDF
  void AddPdf(const std::string & sHeader, std::shared_ptr<pdf::Pdf> & pPdf);


  /// Adds snapShot to internal list
  void AddSnapshot(const std::string & sHeader, std::shared_ptr<Gdiplus::Bitmap> & snapshot);

  /// Adds the snapshots
  void AddSnapshots(const std::string & sHeader, BITMAPLIST snapshots);

  /// Stores the items that have been added to the internal lists
  HANDLE Store(bool bWait, bool bSingleSeries = false);



private:

  /// Action callback handler
  static void ActionCallback(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData);

  /// Performs callback
  void Callback(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters);

    /// Stores the supplied images to the PACS 
  HANDLE StoreImages( const std::string & sHeader, const IMAGELIST & images, unsigned int & iCounter, bool wait = false );

  /// Stores the single image the PACS 
  void StoreImage( const std::string & sHeader, std::shared_ptr<Gdiplus::Bitmap> & image, const std::string & sSeriesInstanceUid, unsigned int & iCounter );

  /// Clears the PDF images
  void ClearPdfImages();

  /// Gets the core header elements
  std::shared_ptr<dcm::DcmElementCollection> GetElements(const std::string & sHeader);

#pragma region properties

public:


  /// Gets the AE title of this client
  const std::string & GetAEtitle() const
  { return m_sAETItle; }

  /// Gets the snapshot counter
  const unsigned int GetSnapshotCounter() const
  { return m_iSnapshotCounter; }

  /// Gets the snapshot series instance uid 
  const std::string & GetSnapshotSeriesInstanceUid() const
  { return m_sSnapshotSeriesInstanceUid; }

  /// Gets the network timeout
  const unsigned int GetNetworkTimeout() const
  { return m_iNetworkTimeout; }
  
  /// Sets the network timeout
  void SetNetworkTimeout(unsigned int iNetworkTimeout) 
  { m_iNetworkTimeout = iNetworkTimeout; }

  // Gets the blocking mode
  const dcm::DcmBlockingMode & GetBlockingMode() const
  { return m_blockingMode; }

  /// Sets the blocking mode
  void SetBlockingMode( dcm::DcmBlockingMode & mode )
  { m_blockingMode = mode; }


#pragma endregion

#pragma region fields

private:

  /// Blocking mode
  dcm::DcmBlockingMode m_blockingMode;

  /// PDF Page counter used for when the pages are bitmaps
  unsigned int m_iPdfPageCounter;

  /// Snapshot image counter
  unsigned int m_iSnapshotCounter;

  /// Snapshot series instance UID
  std::string m_sSnapshotSeriesInstanceUid;

  /// Pdf Series Uid
  std::string m_sPdfSeriesInstanceUid;

  /// Acquisition counter
  unsigned __int64 m_iAcquisitionCounter;

  /// Actions being performed 
  ACTIONLIST m_actions;

  /// AE Title of this object
  std::string m_sAETItle;

  /// Event handlers
  EVENT_HANDLER_LIST m_eventHandlers;

  /// Snapshot images used to delay the store
  std::vector<std::shared_ptr<dcm::DcmImage>> m_snapshotImages;

  /// Pdf Images
  std::vector<std::shared_ptr<dcm::DcmImage>> m_pdfImages;

  /// Dicom header
  std::string m_sHeader;


  // Server
  std::shared_ptr<dcm::DcmHost> m_server;

 /// Network timeout
  unsigned int m_iNetworkTimeout;


#pragma endregion

};    // class PacsClient

#pragma endregion


};  // namespace pacs

#pragma warning(pop)

