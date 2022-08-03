// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmGet.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

// includes
#include "Stdafx.h"
#include "DcmGet.h"
#include "DcmMove.h"
#include "DcmClient.h"
#include "DcmServer.h"
#include "DcmImage.h"
#include "DcmEnums.h"
#include "DcmLog.h"
#include "DcmExceptions.h"
#include "DcmUtility.h"
#include "DcmGetParameters.h"
#include "DcmMoveParameters.h"
#include "DcmHost.h"
#include "DcmHostTable.h"

// usings
using namespace dcm;

/**
 * Constructor
 *
 *  @param sAETitle
 */
DcmGet::DcmGet(const std::string & sAETitle) :
DcmClient(sAETitle),
DcmThread("Starting C-GET command"),
m_serverState(E_NONE),
m_pDcmServer(__nullptr)
{
  m_callbackParameters.CommandType = C_GET;

  m_abstractSyntaxes.push_back(UID_GETPatientRootQueryRetrieveInformationModel);
  m_abstractSyntaxes.push_back(UID_GETStudyRootQueryRetrieveInformationModel);

} // DcmGet


/**
 * Destructor
 */
DcmGet::~DcmGet()
{
 
  m_pDcmServer.reset();

  /// Clear the movers list
  m_movers.clear();
 
  /// Clear 
  m_parameters.clear();

} // ~DcmStore()


#pragma region create methods


/**
 * Creates the find client object
 *
 * @param sAETitleOrXml     AE Title of the client or an xml initializer
 *
 * @return DcmGet object
 */
std::shared_ptr<DcmGet> DcmGet::Create(const std::string & sAETitleOrXml)
{
  
  try
  {
    std::string sAETitle( ( sAETitleOrXml.empty() ? "" : ( sAETitleOrXml[0] == '<' ? "" : sAETitleOrXml ) ) );
    std::string sXml( ( sAETitleOrXml.empty() ? "" : ( sAETitleOrXml[0] == '<' ? sAETitleOrXml : "" ) ) );
         
    /// Allocate memory
    std::shared_ptr<DcmGet> pDcmGet(new DcmGet(sAETitle));

    /// Make sure no memory error occurred
    if ( pDcmGet== __nullptr )
      throw DcmOutOfMemoryException(LogRec("Could not allocate DcmGet", "DcmGet", "Create"));

 
    // ======================================================================================================
    // Initializes the DcmClient inherited class. Cannot override a static method so I do it this way 
    // so that the code is only written once.
    // ====================================================================================================
    if ( ! sXml.empty() ) 
      pDcmGet->DcmClient::Initialize(sXml);

    // Returns the DcmFinc object
    return pDcmGet;

  }
  catch ( DcmException & ex )
  {

    DcmLog::Error(util::Strings::Format("Failed to create find : %s",ex.GetTextMessage().c_str()), "DcmClient", "Create");

    /// This is most likely a configuration issue
    throw;

  }
  catch ( ... )
  {

    DcmLog::Error("Failed to create find, check configuration", "DcmClient", "Create");

     /// This is most likely a configuration issue
    throw DcmConfigurationException(LogRec("Failed to create find, check configuration", "DcmGet", "Create")); 

  } // END ... catch ( ... )

} // Create(const std::string & sXml)


/**
 * Creates the DcmStore object using the xml std::string
 *
 * @param sAETitle      AE Title
 * @param pServer         Server host
 *
 * @return DcmMove object
 */
std::shared_ptr<DcmGet> DcmGet::Create(const std::string & sAETitle, std::shared_ptr<DcmHost> & pServer )
{
 
  // Validates the host 
  pServer->Validate(dcm::DcmServiceTypes::ST_RETRIEVE);

  /// Allocate memory
  std::shared_ptr<DcmGet> pDcmGet(Create(sAETitle));

  // Sets the host
  pDcmGet->SetServer(pServer);

  // Returns the find object
  return pDcmGet; 
  
} // Create(const std::string & sAETitle, std::shared_ptr<DcmHost> & pHost )

#pragma endregion



/**
 * Stores the specified files and images to the server specified
 */
void DcmGet::RunSynchronously()
{

  m_bAbort = false;
 
  try
  {
    /// Clear the list
    m_movers.clear();

    // Log informative message that eth store is being performed
    DcmLog::Information(m_sLogSource, "Performing a C-GET operation", "DcmGet", "Run");

    // ==================================================================
    // Set up parameters that are returned during the callback phase
    // =================================================================
    SetConnectionInformation(m_callbackParameters.Connection);
     

    /// Start Get
    Callback(E_STARTED);

    DWORD handleCount(0);
    std::unique_ptr<HANDLE> handles = std::unique_ptr<HANDLE>(new HANDLE[m_parameters.size()]);

    // =====================================================================
    // Loop through all parameters in the list. There should only be one.
    // ====================================================================
    while ( ! m_parameters.empty() )
    {

      // Check the abort flag each iteration
      if ( m_bAbort )
          throw DcmAbortException(LogRec("Aborting","DcmMove","Run"));

      std::shared_ptr<DcmGetParameters> parameters(m_parameters.back());

      /// Starts the dicom server form receiving
      StartServer(parameters->GetDestinationAEtitle(), parameters->GetDestinationPort());

      DcmLog::Debug("Creating Move", "DcmGet", "Run");

      // Create the DcmMove object
      std::shared_ptr<DcmMove> pDcmMove = DcmMove::Create(m_sAETitle, GetServer());
      
      DcmLog::Debug("Adding Move parameters", "DcmGet", "Run");
      pDcmMove->AddParameters(std::shared_ptr<DcmMoveParameters>(parameters));

      m_parameters.pop_back();

      m_movers.push_back(pDcmMove);

      DcmLog::Information("Starting C-MOVE client", "DcmGet", "Run");

      /// Start the move
      pDcmMove->Start(this, &dcm::DcmGet::MoveCallback );

      handles.get()[handleCount++] = pDcmMove->GetHandle();

    }

    /// Wait for all the movers to complete
    DWORD wait = WaitForMultipleObjects(handleCount, handles.get(), TRUE, INFINITE );

    if ( wait == WAIT_FAILED )
      throw DcmException (LogRec(util::Strings::Format("Wait failed (%d)", GetLastError()), "DcmGet", "Run"));

     // Indicate that we have completed
    m_callbackParameters.Command.Store.Completed += m_callbackParameters.Command.Store.Remaining;
    m_callbackParameters.Command.Store.Remaining = 0;
    Callback(E_COMPLETED);
 
  }
  catch ( DcmAbortException &  )
  {

    /// Log Error Message
    DcmLog::Error(m_sLogSource, "C-GET was aborted" , "DcmGet", "Run" );

    /// Abort the association
    AbortAssociation();

    // Disconnect from the server
    Disconnect();

    /// Abort event
    Callback(E_ABORTED);

    /// Re-throw
    throw;

  }
  catch ( DcmException & ex )
  {
    /// Abort the association
    AbortAssociation();

    // Disconnect from the server
    Disconnect();

    std::string sMessage(util::Strings::Format("C-GET failed : %s", ex.GetTextMessage().c_str()));

    /// Set message
    m_callbackParameters.Message = sMessage.c_str();

    /// Failed event
    Callback(E_FAILED);

    /// Re-throw
    throw;

  }
  catch ( ...  )
  {

    /// Abort the association
    AbortAssociation();

    // Disconnect from the server
    Disconnect();


    std::string sMessage(util::Strings::Format("C-GET failed : : unknown error [%d]", ::GetLastError()));

    /// Set message
    m_callbackParameters.Message = sMessage.c_str();

    /// Failed event
    Callback(E_FAILED);

    /// Throw exception but get the last error code from the system
    throw DcmException(LogRec(m_callbackParameters.Message, "DcmGet", "Run"), m_sLogSource);

  }

} // Run()



/**
 * Callback relay will overrode the DcmClient's virtual method and is called from  within the 
 * static callback method used for the libraries callback. 
 *
 * @param   pResponse       Response from the C-GET
 * @param   pResponseIDs
 *
 */
void DcmGet::CallbackRelay( void * pResponse, DcmDataset * pResponseIds)
{

  /// Move response
  T_DIMSE_C_GetRSP * pGetResponse = reinterpret_cast<T_DIMSE_C_GetRSP *>(pResponse);

  /// Setup callback parameters
  m_callbackParameters.Status                 = pGetResponse->DimseStatus;
  m_callbackParameters.Command.Get.Completed  = pGetResponse->NumberOfCompletedSubOperations;
  m_callbackParameters.Command.Get.Remaining  = pGetResponse->NumberOfRemainingSubOperations;
  m_callbackParameters.Command.Get.Warning    = pGetResponse->NumberOfWarningSubOperations;
  m_callbackParameters.Command.Get.Failed     = pGetResponse->NumberOfFailedSubOperations;

  try
  {
    /// Callback progress
    Callback(E_PROGRESS);
  }
  catch ( ... )
  {
    ; /// noop
  }

} // CallbackRelay(void * pResponse)


/**
 * Aborts the move operation
 */
void DcmGet::Abort()
{
  DcmLog::Debug(m_sLogSource, "Aborting C-GET", "DcmStore", "Abort" );

  // Set the abort thread
  m_bAbort = true;

  // Wait for the threads process to pick up the abort and shut down
  Sleep(500);

  DcmThread::Abort();
} // Abort()


/**
 * Add the parameters to the list making sure that 
 *
 * @param parameters    Get parameters
 */
void DcmGet::AddParameters( std::shared_ptr<DcmGetParameters> & parameters )
{
 
  /// Validate the parameters before adding
  parameters->Validate();

  if ( m_parameters.size() == 0 )
    m_parameters.push_back(parameters);
  else
  {
    if ( (*m_parameters.begin())->SameDestination(parameters) )
    {
      throw DcmInvalidDataException(LogRec(util::Strings::Format("Destinations do not match (%s/%d) (%s/%d)",
                                                              m_parameters[0]->GetDestinationAEtitle().c_str(),
                                                              m_parameters[0]->GetDestinationAEtitle(),
                                                              parameters->GetDestinationAEtitle().c_str(),
                                                              parameters->GetDestinationPort()), "DcmGet", "AddParameters"));
    }
  }
}   // AddParameters( DcmGetParameters * pParameters )


/**
 * Starts the dicom server
 *
 *  @param sAETitle
 *  @param iPort
 */
void DcmGet::StartServer(const std::string & sAETitle, int iPort)
{

  // ==================================================
  // Check to see if the server parameter have changed
  // ===================================================
  if ( m_pDcmServer == __nullptr )
  {
    // Log informative message that eth store is being performed
    DcmLog::Information(m_sLogSource, util::Strings::Format("Starting server (%s:%d) for C-GET", sAETitle.c_str(), iPort), "DcmGet", "StartServer");

    // Allocate new server
    m_pDcmServer =  DcmServer::Create(sAETitle, iPort);

    m_pDcmServer->AddAbstractSyntax(UID_CTImageStorage);
    m_pDcmServer->AddAbstractSyntax(UID_MRImageStorage);
    m_pDcmServer->AddAbstractSyntax(UID_SecondaryCaptureImageStorage);


    // Set the log source
    m_pDcmServer->SetLogSource(m_sLogSource);

    // Start the server for receiving
    m_pDcmServer->Start(this, &dcm::DcmGet::ServerCallback);

    // Wait until the server's state indicates that it is started
    while ( m_serverState != E_STARTED && m_serverState != E_FAILED &&  m_serverState != E_COMPLETED ) Sleep(200);

   }  // END ... If the server is not already started

} // StartServer(const std::string & sAETitle, int iPort)


/**
 * Handles the server callback and relays the information back
 *
 *  @param pCallbackParameters
 *  @param lpCallbackData
 */
void DcmGet::ServerCallback(PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)
{

  DcmGet * pDcmGet(reinterpret_cast<DcmGet *>(lpCallbackData));

  pDcmGet->m_serverState = pCallbackParameters->EventType;

  std::ostringstream os;

  os << "(" << pCallbackParameters->Connection.CalledAETitle << ") (" << pCallbackParameters->Connection.Port << ")";

  // =================================
  // Log messages based on the event
  // ==================================
  switch ( pCallbackParameters->EventType )
  {

  case E_STARTED:
    DcmLog::Information(util::Strings::Format("Server started %s", os.str().c_str()), "DcmGet", "ServerCallback");
    break;
  case E_ABORTED:
    DcmLog::Debug(util::Strings::Format("Server aborted %s", os.str().c_str()), "DcmGet", "ServerCallback");
    break;
  case E_FAILED:
    DcmLog::Error(util::Strings::Format("Server failed %s : %s", os.str().c_str(), pCallbackParameters->Message), "DcmGet", "ServerCallback");
    break;
  case E_PROGRESS:
    {
      pDcmGet->m_callbackParameters.Command.Get.Completed++;
      pDcmGet->m_callbackParameters.Command.Get.Image = pCallbackParameters->Command.Store.Image;

      /// Callback the get information
      pDcmGet->Callback(E_PROGRESS);

     }
    break;
  case E_COMPLETED:
    DcmLog::Information(util::Strings::Format("Server completed %s", os.str().c_str()), "DcmGet", "ServerCallback");
    break;

  };  // END ... switch on server event
 
} //ServerCallback(PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)


/**
 * Handles the move callbask
 *
 *  @param pCallbackParameters
 *  @param lpCallbackData
 */
void DcmGet::MoveCallback(PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)
{
  DcmGet * pDcmGet = reinterpret_cast<DcmGet *>(lpCallbackData);

  std::ostringstream os;

  os << "(" << pCallbackParameters->Connection.CallingAETitle << ") (" << pCallbackParameters->Connection.CalledAETitle << ") (" << pCallbackParameters->Connection.ServerName << ") (" << pCallbackParameters->Connection.Port << ")";

   // =================================
  // Log messages based on the event
  // ==================================
  switch ( pCallbackParameters->EventType )
  {

  case E_STARTED:
    DcmLog::Debug(util::Strings::Format("C-MOVE started %s", os.str().c_str()), "DcmGet", "MoveCallback");
    break;
  case E_ABORTED:
    DcmLog::Debug(util::Strings::Format("C-MOVE aborted %s", os.str().c_str()), "DcmGet", "MoveCallback");
    break;
  case E_FAILED:
    DcmLog::Debug(util::Strings::Format("C-MOVE failed %s : %s", os.str().c_str(), pCallbackParameters->Message), "DcmGet", "MoveCallback");

    break;
  case E_PROGRESS:
    {
       pDcmGet->m_callbackParameters.Command.Get.Remaining = pCallbackParameters->Command.Move.Remaining;
    }
    break;
  case E_COMPLETED:
    DcmLog::Debug(util::Strings::Format("C-MOVE completed %s", os.str().c_str()), "DcmGet", "MoveCallback");
    break;

  };  // END ... switch on server event

 
} // MoveCallback(PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData)
