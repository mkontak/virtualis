// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#include "stdafx.h"
#include "CvnPacsConverter.h"
#include "CvnExceptions.h"
#include "CvnEnums.h"
#include "CvnLog.h"
#include "CvnSubVolumeInfo.h"


using namespace cvn;

CvnPacsConverter::CvnPacsConverter(int iSiteId, const std::string & sAEtitle) :
CvnThread(),
CvnConverter(iSiteId),
m_get(__nullptr),
m_sAEtitle(sAEtitle)
{

 
}

/**
 * Destructor
 */
CvnPacsConverter::~CvnPacsConverter()
{
  // Abort the get
  if ( m_get != __nullptr ) 
    m_get.reset();

} // ~CvnFileConverter

/**
 * Sets the retrieval parameters
 */
void CvnPacsConverter::SetRetrievalParameters(std::shared_ptr<dcm::DcmHost> & pServer,  std::shared_ptr<dcm::DcmGetParameters> & pParameters)
{

  m_retrievalParameters.first   = pServer;
  m_retrievalParameters.second  = pParameters;

} // SetRetievalParameters(std::shared_ptr<dcm::DcmHost> & pServer, const dcm::DcmGetParameters & pParameters)


/**
 * Creates the volume 
 *
 * @param sVolumeFilePath       Volume file path to be written to
 * @param callbackData          Data to be returned in callback
 * @param callbackFunction      Function to callback
 *
 * @return  List of volume files
 */
const std::map<uint4,std::shared_ptr<CvnSubVolumeInfo>> & CvnPacsConverter::Create(  std::function< void ( PCVN_CALLBACK_PARAMETERS, LPVOID ) > callbackFunction,  LPVOID callbackData, const std::string & sVolumeFilePath )
{


  m_callbackFunction = callbackFunction;
  m_callbackData     = callbackData;
  m_sVolumeFilePath  = sVolumeFilePath;

  RunSynchronously();

  return m_pSubVolumes;

} // CvnPacsConverter::Create( LPVOID callbackData, std::function< CVN_CALLBACK_FUNCTION > callbackFunction )

/**
 * Runs the thread
 */
void CvnPacsConverter::RunSynchronously()
{
  m_bAbort = false;


  /// Make sure there is a series uid
  if ( m_retrievalParameters.second->GetSeriesUids().Size() == 0 )
    throw CvnInvalidDataException(LogRec("Missing series uid", "CvnPacsConverter", "Run"));


  /// Series being retreived
  m_sSeriesUid = m_retrievalParameters.second->GetSeriesUids()[0];

  m_callbackParameters.SeriesUid = m_sSeriesUid.c_str();
  m_callbackParameters.Completed = 0;
  m_callbackParameters.Remaining = 0;
  m_callbackParameters.Total = 0;

  Callback(cvn::CvnEvents::E_STARTED);

  std::shared_ptr<dcm::DcmImageCollection> images;

  try
  {


    /// Retrieve the data first
    Retrieve();

    /// Create the volume
		/// It will be implemented later for load dicom from memory
    //CvnConverter::Create();


    /// Complete
    Callback(E_COMPLETED);


  }
  catch ( CvnAbortException & )
  {
    CvnLog::Error("Conversion was aborted", "CvnPacsConverter", "Run");

    Callback(E_ABORTED);

    throw;
  }
  catch ( CvnException & ex )
  {

    std::string sMessage(util::Strings::Format("Conversion failed : %s", ex.GetTextMessage().c_str()));

    CvnLog::Error(sMessage, "CvnPacsConverter", "Run");

    Callback(E_FAILED, sMessage);

    throw CvnException(LogRec(sMessage, "CvnPacsConverter", "Run"));

  }
  catch ( dcm::DcmException & ex )
  {
    std::string sMessage(util::Strings::Format("Conversion failed : %s", ex.GetTextMessage().c_str()));

    CvnLog::Error(sMessage, "CvnPacsConverter", "Run");

    Callback(E_FAILED, sMessage);

    throw CvnException(LogRec(sMessage, "CvnPacsConverter", "Run"));

  }
  catch ( ... )
  {
 
    CvnLog::Error("Conversion failed : unspecified error", "CvnFileConverter", "Run");

    Callback(E_FAILED, "Conversion failed : unspecified error");

    throw CvnException(LogRec("Conversion failed : unspecified error", "CvnPacsConverter", "Run"));

  }
} // Run()


/**
 * Aborts the move operation
 */
void CvnPacsConverter::Abort()
{
  CvnLog::Debug("Aborting File Conversion", "CvnPacsConverter", "Abort" );

  // Set the abort thread
  m_bAbort = true;

  // Abort the get
  if ( m_get != __nullptr ) 
    m_get->Abort();


  // Wait for the threads process to pick up the abort and shut down
  Sleep(500);

  // Terminates the thread
  CvnThread::Abort();

} // Abort()




/**
 * Handles the server callback and relays the information back
 */
void CvnPacsConverter::GetCallbackHandler(dcm::PDCM_CALLBACK_PARAMETERS callbackParameters, LPVOID callbackData)
{
  CvnPacsConverter * pConverter = reinterpret_cast<CvnPacsConverter *>(callbackData);


  if ( callbackParameters->EventType == dcm::DcmEvents::E_PROGRESS )
  {
    /// Insert the image to the list after we clone it since the underlying find owns the memory
    pConverter->m_images->Add(callbackParameters->Command.Get.Image->Clone());

    pConverter->m_callbackParameters.SeriesUid = pConverter->m_images->GetSeriesUid().c_str();
    pConverter->m_callbackParameters.Completed = callbackParameters->Command.Get.Completed;
    pConverter->m_callbackParameters.Remaining = callbackParameters->Command.Get.Remaining;
 
    /// callback the get information
    pConverter->Callback(E_UPDATED);

  }

} // GetCallback(PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)

/**
 * Retrieves the series for conversion
 *
 */
void CvnPacsConverter::Retrieve()
{

 
  try
  {

    /// Timer
    vxLogTimer timer;

    m_callbackParameters.StageType = S_RETRIEVAL;
    
    m_get = dcm::DcmGet::Create(m_sAEtitle, m_retrievalParameters.first);
 
    /// Add the get parameters
    m_get->AddParameters(m_retrievalParameters.second);

    /// Starts the wait
    m_get->Start( this, cvn::CvnPacsConverter::GetCallbackHandler );
    
    /// Wait for the GET to finish
    m_get->Wait();

    timer.Trace(util::Strings::Format("Retrieval of series %s", m_sSeriesUid.c_str()));

  }
  catch ( dcm::DcmAbortException )
  {
    throw CvnAbortException(LogRec("Aborting conversion","CvnPacsConverter","Retrieve"));
  }
  catch ( dcm::DcmException & ex )
  {
    CvnLog::Error(util::Strings::Format("Failed to retrieve dicom : %s", ex.GetTextMessage()), "CvnPacsConverter","Retrieve");

    throw CvnException(ex.GetLogRecord());

  } 
  
} // Retrieve()

/**
 * CallbackRelay agent
 *
 * @param parameters        Parameters to pass back
 */
void CvnPacsConverter::CallbackRelay(const CVN_CALLBACK_PARAMETERS & parameters)
{

  // Only relay if progress event
  if ( parameters.EventType == E_PROGRESS )
    Callback(&const_cast<CVN_CALLBACK_PARAMETERS &>(parameters));

} // CallbackRelay
