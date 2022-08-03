// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmFind.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

// includes
#include "Stdafx.h"
#include "DcmFind.h"
#include "DcmClient.h"
#include "DcmImage.h"
#include "DcmEnums.h"
#include "DcmLog.h"
#include "DcmExceptions.h"
#include "DcmUtility.h"
#include "DcmFindParameters.h"
#include "DcmHost.h"

// using
using namespace dcm;


#pragma region constructors/destructors

/**
 * Constructor
 *
 * @param sAETitle    AE Title of the find client
 */
DcmFind::DcmFind(const std::string & sAETitle) :
DcmClient(sAETitle),
DcmThread("Starting C-FIND command")
{

  m_callbackParameters.CommandType  = C_FIND;

  m_abstractSyntaxes.push_back(UID_FINDPatientRootQueryRetrieveInformationModel);
  m_abstractSyntaxes.push_back(UID_FINDStudyRootQueryRetrieveInformationModel);
  m_abstractSyntaxes.push_back(UID_FINDModalityWorklistInformationModel);
  m_abstractSyntaxes.push_back(UID_FINDGeneralPurposeWorklistInformationModel);

} // DcmFind


/**
 * Destructor
 */
DcmFind::~DcmFind()
{
  /// Clear 
  m_parameters.clear();

} // ~DcmFind()

#pragma endregion



#pragma region create methods


/**
 * Creates the find client object
 *
 * @param sAETitleOrXml     AE Title of the client or an xml initializer
 *
 * @return DcmFind object
 */
std::shared_ptr<DcmFind> DcmFind::Create(const std::string & sAETitleOrXml)
{  

  try
  {

    std::string sAETitle( ( sAETitleOrXml.empty() ? "" : ( sAETitleOrXml[0] == '<' ? "" : sAETitleOrXml ) ) );
    std::string sXml( ( sAETitleOrXml.empty() ? "" : ( sAETitleOrXml[0] == '<' ? sAETitleOrXml : "" ) ) );
         
    /// Allocate memory
    std::shared_ptr<DcmFind> pDcmFind(new DcmFind(sAETitle));

    /// Make sure no memory error occurred
    if ( pDcmFind== __nullptr )
      throw DcmOutOfMemoryException(LogRec("Could not allocate DcmFind", "DcmFind", "Create"));

 
    // ======================================================================================================
    // Initializes the DcmClient inherited class. Cannot override a static method so I do it this way 
    // so that the code is only written once.
    // ====================================================================================================
    if ( ! sXml.empty() ) 
      pDcmFind->DcmClient::Initialize(sXml);

    // Returns the DcmFinc object
    return pDcmFind;

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
    throw DcmConfigurationException(LogRec("Failed to create find, check configuration", "DcmFind", "Create")); 

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
std::shared_ptr<DcmFind> DcmFind::Create(const std::string & sAETitle, std::shared_ptr<DcmHost> & pServer )
{
 
  // Validates the host 
  pServer->Validate(dcm::DcmServiceTypes::ST_QUERY);

  /// Allocate memory
  std::shared_ptr<DcmFind> pDcmFind(Create(sAETitle));

  // Sets the host
  pDcmFind->SetServer(pServer);

  // Returns the find object
  return pDcmFind; 
  
} // Create(const std::string & sAETitle, std::shared_ptr<DcmHost> & pHost )


#pragma endregion

/**
 * Runs the CFIND operation
 */
void DcmFind::RunSynchronously()
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
      DcmLog::Information(m_sLogSource, "Performing a C-FIND operation", "DcmFind", "Run");

      /// Indicate that we have started
      Callback(E_STARTED);

 
      //===========================
      // Loop through all the files
      //==========================
      std::for_each ( m_parameters.begin(), m_parameters.end(), [this] ( std::shared_ptr<DcmFindParameters> parameters )
      { 
        // Check the abort flag each iteration
        if ( m_bAbort )
          throw DcmAbortException(LogRec("Aborting","DcmFind","Run"));

        /// Set the callback parameters
        m_callbackParameters.Command.Find.Parameters = parameters.get();

        this->DcmClient::Find(parameters);

      } ); // END ... For each file


      // Releases the association
      ReleaseAssociation();
 
      // Disconnect from the server
      Disconnect();

    }

    // Indicate that we have completed
    Callback(E_COMPLETED);
 
  }
  catch ( DcmAbortException &  )
  {

    /// Log Error Message
    DcmLog::Error(m_sLogSource, "C-FIND was aborted" , "DcmFind", "Run" );

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

    std::string sMessage(util::Strings::Format("C-FIND failed : %s", ex.GetTextMessage().c_str()));

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


    std::string sMessage(util::Strings::Format("C-FIND failed : : unknown error [%d]", ::GetLastError()));

    /// Set message
    m_callbackParameters.Message = sMessage.c_str();

    /// Failed event
    Callback(E_FAILED);

    /// Throw exception but get the last error code from the system
    throw DcmException(LogRec(m_callbackParameters.Message, "DcmFind", "Run"), m_sLogSource);

  }

} // Run()



/**
 * Callback relay will overrode the DcmClient's virtual method and is called from  within the 
 * static callback method used for the libraries callback. 
 *
 * @param   pResponse       Response from the C-MOVE
 * @param   pResponseIDs
 */
void DcmFind::CallbackRelay(void * pResponse, DcmDataset * pResponseIds)
{

  /// Find response
  T_DIMSE_C_FindRSP * pFindResponse = reinterpret_cast<T_DIMSE_C_FindRSP *>(pResponse);

  m_callbackParameters.Command.Find.Completed++;
  m_callbackParameters.Command.Find.Image = new DcmImage(pResponseIds);
  m_callbackParameters.Status = pFindResponse->DimseStatus;
  
  try
  {
    /// Callback progress
    Callback(E_PROGRESS);
  }
  catch ( ... )
  {
    ; /// noop
  }

  // Delete the image only if it is not null (this allows the function taking the image to take ownership)
  if ( m_callbackParameters.Command.Find.Image != __nullptr )
    delete m_callbackParameters.Command.Find.Image;

  m_callbackParameters.Command.Find.Image = __nullptr;

} // CallbackRelay(void * pResponse)


/**
 * Aborts the move operation
 */
void DcmFind::Abort()
{
  DcmLog::Debug(m_sLogSource, "Aborting C-MOVE", "DcmStore", "Abort" );

  // Set the abort thread
  m_bAbort = true;

  // Wait for the threads process to pick up the abort and shut down
  Sleep(500);

  // Terminates the thread
  DcmThread::Abort();

} // Abort()


/**
 * Adds move parameters to the list
 *
 * @param parameters      Move parameters
 */
void DcmFind::AddParameters(std::shared_ptr<DcmFindParameters> & parameters)
{ 
  /// Validates the parameters
  parameters->Validate();

  /// Add parameters the list
  m_parameters.push_back(parameters); 

} // AddParameters(std::shared_ptr<DcmFindParameters> & parameters)

