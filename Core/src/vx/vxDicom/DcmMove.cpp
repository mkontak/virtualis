// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DcmMove.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


// includes
#include "Stdafx.h"
#include "DcmMove.h"
#include "DcmClient.h"
#include "DcmImage.h"
#include "DcmEnums.h"
#include "DcmLog.h"
#include "DcmExceptions.h"
#include "DcmUtility.h"
#include "DcmMoveParameters.h"
#include "DcmHost.h"


// using
using namespace dcm;

/**
 * Constructor
 *
 * @param sAETitle      AE Title of the move client
 */
DcmMove::DcmMove(const std::string & sAETitle) :
DcmClient(sAETitle),
DcmThread("Starting C-MOVE command")
{
  m_callbackParameters.CommandType = C_MOVE;

  m_abstractSyntaxes.push_back(UID_MOVEPatientRootQueryRetrieveInformationModel);
  m_abstractSyntaxes.push_back(UID_MOVEStudyRootQueryRetrieveInformationModel);

} // DcmMove



/**
 * Destructor
 */
DcmMove::~DcmMove()
{
  /// Clear 
  m_parameters.clear();

} // ~DcmStore()


#pragma region create methods


/**
 * Creates the find client object
 *
 * @param sAETitleOrXml     AE Title of the client or an xml initializer
 *
 * @return DcmMove object
 */
std::shared_ptr<DcmMove> DcmMove::Create(const std::string & sAETitleOrXml)
{
  
  try
  {

    std::string sAETitle( ( sAETitleOrXml.empty() ? "" : ( sAETitleOrXml[0] == '<' ? "" : sAETitleOrXml ) ) );
    std::string sXml( ( sAETitleOrXml.empty() ? "" : ( sAETitleOrXml[0] == '<' ? sAETitleOrXml : "" ) ) );
         
    /// Allocate memory
    std::shared_ptr<DcmMove> pDcmMove(new DcmMove(sAETitle));

    /// Make sure no memory error occurred
    if ( pDcmMove== __nullptr )
      throw DcmOutOfMemoryException(LogRec("Could not allocate DcmMove", "DcmMove", "Create"));

 
    // ======================================================================================================
    // Initializes the DcmClient inherited class. Cannot override a static method so I do it this way 
    // so that the code is only written once.
    // ====================================================================================================
    if ( ! sXml.empty() ) 
      pDcmMove->DcmClient::Initialize(sXml);

    // Returns the DcmFinc object
    return pDcmMove;

  }
  catch ( DcmException & ex )
  {

    DcmLog::Error(util::Strings::Format("Failed to create find : %s",ex.GetTextMessage().c_str()), "DcmClient", "Create");

    /// This is most likely a configuration issue
    throw;

  }
  catch ( ... )
  {

    DcmLog::Error("Failed to create find, check configuration", "DcmMove", "Create");

     /// This is most likely a configuration issue
    throw DcmConfigurationException(LogRec("Failed to create find, check configuration", "DcmMove", "Create")); 

  } // END ... catch ( ... )

} // Create(const std::string & sXml)


/**
 * Creates the DcmStore object using the xml std::string
 *
 * @param sAETitle      AE Title
 * @param pHost         Server host
 *
 * @return DcmMove object
 */
std::shared_ptr<DcmMove> DcmMove::Create(const std::string & sAETitle, std::shared_ptr<DcmHost> & pServer )
{
 
  // Validates the host 
  pServer->Validate(dcm::DcmServiceTypes::ST_RETRIEVE);

  /// Allocate memory
  std::shared_ptr<DcmMove> pDcmMove(Create(sAETitle));

  // Sets the host
  pDcmMove->SetServer(pServer);

  // Returns the find object
  return pDcmMove; 
  
} // Create(const std::string & sAETitle, std::shared_ptr<DcmHost> & pHost )

#pragma endregion



/**
 * Adds move parameters to the list
 *
 * @param parameters      Move parameters
 */
void DcmMove::AddParameters(std::shared_ptr<DcmMoveParameters> & parameters)
{ 
  /// Validates the parameters
  parameters->Validate();

  /// Add parameters the list
  m_parameters.push_back(parameters); 

} // AddParameters(const DcmMoveParameters & parameters)




/**
 * Stores the specified files and images to the server specified
 */
void DcmMove::RunSynchronously()
{
  m_bAbort = false;
 
  try
  {

    // ==================================================================
    // Set up parameters that are returned during the callback phase
    // =================================================================
    SetConnectionInformation(m_callbackParameters.Connection);
    
    // ===============================================================
    // If there are images to store then we need to connect and store
    // ===============================================================
    if (  m_parameters.size() > 0 )
    {
      
      // Connect to the SCP
      Connect();

      // Log informative message that eth store is being performed
      DcmLog::Information(m_sLogSource, "Performing a C-MOVE operation", "DcmMove", "Run");

      /// Indicate that we have started
      Callback(E_STARTED);

      //===========================
      // Loop through all the files
      //==========================

      std::for_each( m_parameters.begin(), m_parameters.end(), [ this ] ( std::shared_ptr<DcmMoveParameters> parameters ) 
      {
        // Check the abort flag each iteration
        if ( m_bAbortRequested )
          throw DcmAbortException(LogRec("Aborting","DcmMove","Run"));

        /// Set the current parameters we are working on
        m_callbackParameters.Command.Move.Parameters = parameters.get();
 
        /// This call is not threaded so it returns when the move has completed or has an error
        this->DcmClient::Move(parameters);

      });

  
      // Releases the association
      ReleaseAssociation();
 
      // Disconnect from the server
      Disconnect();
      
    }

    // Indicate that we have completed
    m_callbackParameters.Command.Store.Completed += m_callbackParameters.Command.Store.Remaining;
    m_callbackParameters.Command.Store.Remaining = 0;
    Callback(E_COMPLETED);
 
  }
  catch ( DcmAbortException &  )
  {

    /// Log Error Message
    DcmLog::Error(m_sLogSource, "C-MOVE was aborted" , "DcmMove", "Run" );

    /// Abort the association
    AbortAssociation();

    // Disconnect from the server
    Disconnect();

    /// Abort event
    Callback(E_ABORTED);

    /// Rethrow
    throw;

  }
  catch ( DcmException & ex )
  {
    /// Abort the association
    AbortAssociation();

    // Disconnect from the server
    Disconnect();

    std::string sMessage(util::Strings::Format("C-MOVE failed : %s", ex.GetTextMessage().c_str()));

    /// Set message
    m_callbackParameters.Message = sMessage.c_str();

    /// Failed event
    Callback(E_FAILED);

    /// Rethrow
    throw;

  }
  catch ( ...  )
  {

    /// Abort the association
    AbortAssociation();

    // Disconnect from the server
    Disconnect();

    std::string sMessage(util::Strings::Format("C-MOVE failed : : unknown error [%d]", ::GetLastError()));

    /// Set message
    m_callbackParameters.Message = sMessage.c_str();

    /// Failed event
    Callback(E_FAILED);

    /// Throw exception but get the last error code from the system
    throw DcmException(LogRec(m_callbackParameters.Message, "DcmMove", "Run"), m_sLogSource);

  }

} // Run()



/**
 * Callback relay will overrode the DcmClient's virtual method and is called from  within the 
 * static callback method used for the libraries callback. 
 *
 * @param   pResponse       Response from the C-MOVE
 * @param   pResponseIds
 *
 */
void DcmMove::CallbackRelay( void * pResponse, DcmDataset * pResponseIds)
{

  /// Move response
  T_DIMSE_C_MoveRSP * pMoveResponse = reinterpret_cast<T_DIMSE_C_MoveRSP *>(pResponse);

  /// Setup callback parameters
  m_callbackParameters.Status                 = pMoveResponse->DimseStatus;
  m_callbackParameters.Command.Move.Completed = pMoveResponse->NumberOfCompletedSubOperations;
  m_callbackParameters.Command.Move.Remaining = pMoveResponse->NumberOfRemainingSubOperations;
  m_callbackParameters.Command.Move.Warning   = pMoveResponse->NumberOfWarningSubOperations;
  m_callbackParameters.Command.Move.Failed    = pMoveResponse->NumberOfFailedSubOperations;
 

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
void DcmMove::Abort()
{
  DcmLog::Debug(m_sLogSource, "Aborting C-MOVE", "DcmStore", "Abort" );

  // Set the abort thread
  m_bAbort = true;

  // Wait for the threads process to pick up the abort and shut down
  Sleep(500);

  DcmThread::Abort();
} // Abort()