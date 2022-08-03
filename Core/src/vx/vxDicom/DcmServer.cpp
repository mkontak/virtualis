// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmServer.h
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "stdafx.h"

#include "DcmLog.h"
#include "DcmExceptions.h"
#include "DcmServer.h"
#include "DcmAssociation.h"
#include "DcmUtility.h"
#include "Logger.h"



using namespace dcm;



/** 
 * Default constructor
 */
DcmServer::DcmServer() :
DcmDevice(),
DcmThread("Starting dicom server"),
m_pNetwork(__nullptr),
m_iPort(0),
m_eState(S_IDLE),
m_sName(DcmUtility::GetHostname()),
m_bAborting(false)
{  
} // DcmServer()


/**
 * Constructor
 *
 *
 *  @param sAETitle     AE Title of this server
 *  @param iPort        Port this server will listen on
 */
DcmServer::DcmServer(const std::string & sAETitle, int iPort) : 
DcmDevice(sAETitle),  
DcmThread("Starting dicom server"),
m_bAborting(false),
m_pNetwork(__nullptr),
m_iPort(iPort),
m_eState(S_IDLE),
m_sName(DcmUtility::GetHostname())
{

  // Default transfer Syntaxes
  m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
  m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);

  // Default command syntaxes
  m_abstractSyntaxes.push_back(UID_VerificationSOPClass);
  m_abstractSyntaxes.push_back(UID_CTImageStorage);
 
} // DcmServer(std::string msAETitle, int port)


/**
 * Destructor
 */
DcmServer::~DcmServer()
{
  
  // Stops the server
  this->Abort();

  /// Clear all associations
  m_associations.clear();

  /// Cleanup the network
  WSACleanup(); 

} // ~DcmServer()


/** 
 * Creates a dicom server from the xml node
 * 
 * @param mpNode        Xml node
 *
 * @return  DcmServer object
 */
std::shared_ptr<DcmServer> DcmServer::Create(const std::string & sXml)
{

  /// New DcmServer
  std::shared_ptr<DcmServer> pDcmServer(new DcmServer());
    
  try
  {

    /// Check allocation
    if ( pDcmServer == __nullptr )
      throw DcmOutOfMemoryException(LogRec("Failed to allocation DcmServer","DcmServer","Create"));


    // Parse the xml
    xml::XmlDocument document;
    document.LoadXml(sXml);
      
  
    /// Gets the document element
    xml::XmlNode docNode(document.GetDocumentElement());

    /// Defaults
    int iPort(104);
    int iNetworkTimeout(0);
    int iMaxPDU(ASC_DEFAULTMAXPDU);
    bool bValidateCalledAETitle(true);
    
   
    if ( ! docNode.TryGetAttributeValue<std::string>("aetitle", pDcmServer->m_sAETitle ) )
      throw DcmConfigurationException(LogRec("No AETitle specified", "DcmServer","Create"));

    if ( ! docNode.TryGetAttributeValue<int>("port", pDcmServer->m_iPort) )
      throw DcmConfigurationException(LogRec("No port specified", "DcmServer","Create"));

    docNode.TryGetAttributeValue<std::string>("logSource", pDcmServer->m_sLogSource);
    docNode.TryGetAttributeValue<unsigned int>("networkTimeout", pDcmServer->m_iNetworkTimeout);
    docNode.TryGetAttributeValue<unsigned int>("maxPDU", pDcmServer->m_iMaxPDU);
    docNode.TryGetAttributeValue<bool>("validateCalledAETitle", pDcmServer->m_bValidateCalledAETitle);

    DcmUtility::AddSection(pDcmServer->m_acceptedAETitles, docNode.SelectSingleNode( "acceptedAETitles" ));
    DcmUtility::AddSection(pDcmServer->m_abstractSyntaxes, docNode.SelectSingleNode( "abstractSyntaxes" ));
    DcmUtility::AddSection(pDcmServer->m_transferSyntaxes, docNode.SelectSingleNode( "transferSyntaxes" ));

    return pDcmServer;

  }
  catch ( DcmException & ex )
  {
    
    DcmLog::Error(pDcmServer->m_sLogSource, ex.GetLogRecord());

    throw;
  }
  catch ( ... )
  {

    DcmLog::Error(pDcmServer->m_sLogSource, "Failed to create server, check xml", "DcmServer","Create");

    throw DcmException(LogRec("Failed to create server, check xml", "DcmServer","Create")); 
  }

} // Create(XmlNode ^ mpNode)


/** 
 * Creates a dicom server from the xml node
 * 
 * @param sAETitle    Servers AE Title
 * @param iPort       Port the server will listen on
 *
 * @return  DcmServer object
 */
std::shared_ptr<DcmServer> DcmServer::Create(const std::string & sAETitle, int iPort)
{
    /// New DcmServer
  std::shared_ptr<DcmServer> pDcmServer(std::make_shared<DcmServer>(sAETitle, iPort));

  /// Check allocation
  if ( pDcmServer == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocation DcmServer","DcmServer","Create"));

  return pDcmServer;
} // Create(const std::string & sAETitle, int iPort)


/** 
 * Starts the servers thread
 */
HANDLE DcmServer::Start(LPVOID callbackData , std::function< void( PDCM_CALLBACK_PARAMETERS, LPVOID) > callbackFunction  )
{

  DcmLog::Information(m_sLogSource, util::Strings::Format("Starting the dicom server (%s:%d)", m_sAETitle.c_str(), m_iPort), "DcmServer","Start");
 
  try
  {
    // Start the thread and return the handle
    return DcmThread::Start(callbackData, callbackFunction);
  }
  catch ( DcmException & ex )
  {
    DcmLog::Error(m_sLogSource, const_cast<vxLogRecord &>(ex.GetLogRecord()).GetTextMessage(), "DcmServer", "Start");

    throw;
  }

} // Start(DCM_CALLBACK_FUNCTION pCallBack, LPVOID lpCallbackData)


/**
 * Drops the network if it was allocated
 */
void DcmServer::DropNetwork()
{

  // ===================================================
  // Only need to drop if a network has been allocated
  // ===================================================
  if ( m_pNetwork != __nullptr )
  {

    DcmLog::Information(m_sLogSource, "Dropping the DISME network", "DcmServer", "DropNetwork");

    // drop the network association
    OFCondition cond = ASC_dropNetwork(&m_pNetwork);

    // If the drop failed log the message as an error but dont throw exception
    if ( cond.bad() )
      DcmLog::Error(m_sLogSource, util::Strings::Format("Failed to drop the network : %s",cond.text()), "DcmServer", "DropNetwork");

    m_pNetwork = __nullptr;

  } // END .. If the Network has been allocated

} // DropNetwork()


/**
 * Runs the dicom server synchronously
 */
void DcmServer::RunSynchronously()
{

  /// Makes sure that the networks not up
  DropNetwork();


  DcmLog::Information(m_sLogSource, "DcmServer starting", "DcmServer", "Run" );


  //=====================================
  // make sure data dictionary is loaded 
  //=====================================
  if ( ! dcmDataDict.isDictionaryLoaded() )
    throw  DcmException(LogRec("Dicom dictionary is not loaded", "DcmServer", "Run"));

  DcmLog::Debug(m_sLogSource, "Initializing network", "DcmServer", "Run");

  // ===========================================================
  // Initialize network ... create an instance of T_ASC_Network
  // ===========================================================
  OFCondition cond = ASC_initializeNetwork(NET_ACCEPTOR, m_iPort, m_iNetworkTimeout, &m_pNetwork);

  // ===================
  // Check for a failure
  // ===================
  if (cond.bad())
  {
    throw DcmException(LogRec(util::Strings::Format("Network initialization failed : %s",cond.text()), "DcmServer", "Run"));
    //DimseCondition::dump(cond);
  } // END ... If the network initialization failed

  // set the acllbask aetitle and prot
  m_callbackParameters.Connection.CalledAETitle = m_sAETitle.c_str();
  m_callbackParameters.Connection.Port          = m_iPort;

  // Callback that we started
  Callback(E_STARTED);

  try
  {

    //================================
    // While the conditions are good
    //===============================
    while (  cond.good() && ! m_bAborting )
    {

      //========================================================================================
      // Receive an association and acknowledge or reject it. If the association was 
      // acknowledged, offer corresponding services and invoke one or more if required.
      //========================================================================================
      AcceptAssociation();

    } // END ... while condition is good

    /// Drops the network
    DropNetwork();

    // Callback that we have completed
    Callback(E_COMPLETED);

  }
  catch ( DcmAbortException )
  {
    /// Drops the network
    DropNetwork();

    // Clear the message
    m_callbackParameters.Message = __nullptr;

    // Log message indicating that we aborted
    DcmLog::Error(m_sLogSource,"Server was aborted","DcmServer", "Run");
 
    // Callback that we aborted
    Callback(E_ABORTED);

    throw;

  }
  catch ( DcmException & ex )
  {

    /// Drops the network
    DropNetwork();

    // Set the error message
    m_callbackParameters.Message = ex.GetTextMessage().c_str();

    // Log the error message
    DcmLog::Error(m_sLogSource, util::Strings::Format("Server failed : %s", m_callbackParameters.Message),"DcmServer", "Run");
 
    // Callback that we failed
    Callback(E_FAILED);

    throw;

  }
  catch ( ... )
  {

    /// Drops the network
    DropNetwork();
    
    // Set the message to undefined
    m_callbackParameters.Message = g_undefinedError.c_str();

    // Callback that we failed
    Callback(E_FAILED);

    // Throw a DcmException
    throw DcmException(LogRec("DcmServer failed","DcmServer","Run"), m_sLogSource);

  } // END ... catch

} // Run()


/**
 * Stops the Dicom DcmServer thread.
 *
 * @param bWait       Flag to indicate that you want to wait for the abort to finish.
 *
 * @return void.
 */
void DcmServer::Abort()
{

  // Log abort message
  DcmLog::Information(m_sLogSource, util::Strings::Format("Aborting dicom server [%s]", m_sAETitle.c_str()), "DcmServer", "Abort");

  m_bAborting = true;

  // ====================================================================
  // Make sure that the server is running before attempting to abort
  // ===================================================================
  if ( IsRunning() )
  {    

    // ====================================================================================
    // Since the server is blocking we need toi unblock it by attempting a dummy connection
    // on a seperate thread. We will ait for the thread to complete.
    // =====================================================================================
    try
    {

      DcmLog::Debug(m_sLogSource, "Creating thread to unblock the server so that it may shutdown", "DcmServer", "Abort");

      LPSECURITY_ATTRIBUTES pSecurityAttributes = __nullptr;
      SIZE_T dwStackSize = 0;
      DWORD dwThreadId = 0;

      // Log we are starting an unblock thread
      DcmLog::Debug(m_sLogSource, "Starting the Unblock Connection thread", "DcmServer", "Abort");

      // Create the thared to unblock the connection
      HANDLE hThread = CreateThread(pSecurityAttributes, dwStackSize, &dcm::DcmServer::UnblockConnection, this, 0, &dwThreadId);

      // Log we are waiting
      DcmLog::Debug(m_sLogSource, "Waiting for the servers connection to be unblocked", "DcmServer", "Abort");

      /// Wait for the unblocking to occur
      WaitForSingleObject(hThread, INFINITE);

      /// Close the thread handle
      CloseHandle(hThread);

      /// Abort each association
      std::for_each( m_associations.begin(), m_associations.end(), [] ( ASSOC item )
      {
        // abort association
        item.second->Abort();
      } );

      /// Clear the list
      m_associations.clear();

    }
    catch ( ... )
    {
      // Any exception we will catch and return the last error
      throw DcmException(LogRec(util::Strings::Format("Failed to stop server [%d]", GetLastError()), "DcmServer", "Abort"), m_sLogSource );
    }

    /// Stops the thread
    DcmThread::Abort();

  } // END ... If not finished
 
} // Stop()


/**
 * Unblocks the servers connection called when the server needs to stop. Called from a seperate 
 * thread.
 *
 *  @param lpParam        Actual DcmServer we are connecting to.
 *
 *  @return error code
 */
DWORD WINAPI DcmServer::UnblockConnection(LPVOID lpParam)
{

  bool bWindowsSocketStarted(true);
  WSAData wsaData;
  WORD wVersionRequested = MAKEWORD(2,2);    
  SOCKET _socket = 0;

  // Server to unblock
  DcmServer * pServer = reinterpret_cast<DcmServer *>(lpParam);

  // Log message we are attempting to unblock
  DcmLog::Debug(pServer->GetLogSource(), "Attempting to unblock the server", "DcmServer", "UnblockConnection");


  // ====================================================
  // Block of code to execute upon exit of the method
  // ===================================================
  SCOPE_EXIT
  {
    // If the sock was open close it
    if ( _socket != 0 )
      closesocket(_socket);

    // If the windows socket's was initialized then clean it up
    if ( bWindowsSocketStarted )
      WSACleanup();

  };


  try
  {

    DcmLog::Debug(pServer->GetLogSource(), "Starting windows socket", "DcmServer", "UnblockConnection");

    // =========================
    // Startup windows socket
    // =========================
    if ( WSAStartup(wVersionRequested,&wsaData) < 0 )
    {
      bWindowsSocketStarted = false;

      throw DcmConnectionException(LogRec( util::Strings::Format("Could not startup windows socket [%d]", WSAGetLastError()), "DcmServer", "UnblockConnection"), pServer->GetLogSource());
    }


    DcmLog::Debug(pServer->GetLogSource(), "Creating socket", "DcmServer", "UnblockConnection");

    //===============================
    // Create a socket to listen on
    //===============================
    _socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //=============================================
    // Make sure the socket was created properly
    //=============================================
    if ( _socket == INVALID_SOCKET )
      throw DcmConnectionException(LogRec( util::Strings::Format("Could not create socket [%d]", WSAGetLastError()), "DcmServer", "UnblockConnection"), pServer->GetLogSource());
    

    struct sockaddr_in socketAddr;

    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(pServer->GetPort());    
    socketAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    DcmLog::Debug(pServer->GetLogSource(), util::Strings::Format( "Forcing a connection to the server on port %d", pServer->GetPort()), "DcmServer", "UnblockConnection");

    /////////////////////////////
    // bind to the local socket
    /////////////////////////////
    if ( connect( _socket, (SOCKADDR *)&socketAddr, sizeof(socketAddr)) == SOCKET_ERROR )
      throw DcmConnectionException(LogRec("Could not connect to socket", "DcmServer", "UnblockConnection"), pServer->GetLogSource());


    return 0;

  }
  catch ( DcmException & ex )
  {

    DcmLog::Error(pServer->GetLogSource(), util::Strings::Format( "Unblock failed [%d] : %s", WSAGetLastError(), ex.GetTextMessage().c_str()),  "DcmServer", "UnblockConnection");
    
    return 1;
  }

} // UnblockConnection()


/**
 * Accepts and association
  */
void DcmServer::AcceptAssociation()
{

  T_ASC_Association * pAssociation;
  OFCondition cond;

  const size_t transferSyntaxCount(m_transferSyntaxes.size());
  const size_t abstractSyntaxCount(m_abstractSyntaxes.size());

  const char ** transferSyntaxes = __nullptr;
  const char ** abstractSyntaxes = __nullptr;

  // ========================================================================
  // Make sure the memory of the transfersyntaxes and abstraxsyntaxes are 
  // always cleaned up when leaving the method's scope
  // =======================================================================
  SCOPE_EXIT
  {

    // Only delete if not null
    if ( transferSyntaxes != __nullptr )
      delete [] transferSyntaxes;

    // Only delete if not null
    if ( abstractSyntaxes != __nullptr )
      delete [] abstractSyntaxes;

  };

  try
  { 

    // Make sure the transferSyntax array is allocated
    if ( (transferSyntaxes = new const char * [transferSyntaxCount]) == __nullptr )
      throw DcmOutOfMemoryException(LogRec("Failed to allocation const char **", "DcmServer", "AcceptAssociation"));

    // Make sure the abstractSyntax array is allocated 
    if ( (abstractSyntaxes = new const char * [abstractSyntaxCount]) == __nullptr )
      throw DcmOutOfMemoryException(LogRec("Failed to allocation const char **", "DcmServer", "AcceptAssociation"));

    // Copy the vector to the allocated array
    DcmUtility::Copy(m_transferSyntaxes, transferSyntaxes);
    DcmUtility::Copy(m_abstractSyntaxes, abstractSyntaxes);

    DcmLog::Information(m_sLogSource,"Accepting association", "DcmServer", "AcceptAssociation" );

    /// Attempt to receive the association request
    cond = ASC_receiveAssociation(m_pNetwork, &pAssociation, m_iMaxPDU, (void**)(__nullptr), (unsigned long *)(__nullptr), OFFalse );

    // Check for an error
    if ( cond.bad() )
      throw DcmAssociationException(LogRec( "Failed to receiving association : " + std::string(cond.text()), "DcmServer", "AcceptAssociation" ));
      

    // Get the called/calling AE Title from the association
    char callingAETitle[32]; 
    RtlZeroMemory((void *)callingAETitle,sizeof(callingAETitle));
    char calledAETitle[32];
    RtlZeroMemory((void *)calledAETitle,sizeof(calledAETitle));

    ASC_getAPTitles(pAssociation->params, callingAETitle, calledAETitle, __nullptr);

    DcmLog::Debug(m_sLogSource, util::Strings::Format("Association received (CalledAETitle = %s) (CallingAETitle = %s)", calledAETitle, callingAETitle) ,"DcmServer", "AcceptAssociation" );

    const int presentationContextCount(ASC_countPresentationContexts(pAssociation->params));

    DcmLog::Debug(m_sLogSource, util::Strings::Format("Presentation Context Count %d", presentationContextCount), "DcmServer", "AcceptAssociation");

    //==============================================================================
    // Check if an association context is accepted by getting the accepted count
    // if 0 then we are not accepting the association.
    //===============================================================================
    if ( presentationContextCount == 0  )
      throw DcmAssociationException ( LogRec("SOP not accepted", "DcmServer", "AcceptAssociation") );
 
    /// Validate the called AE Title
    ValidateCalledAETitle(pAssociation, calledAETitle);
   
    /// Validate the calling AE Title
    ValidateCallingAETitle(pAssociation, callingAETitle);

    //======================================
    // Build the transfers syntaxes array
    // =====================================
    for ( register int i(0); i < m_transferSyntaxes.size(); i++ )
    {

      DcmLog::Debug(m_sLogSource, "Transfer Syntax " + m_transferSyntaxes[i], "DcmServer", "AcceptAssociation");

      const char * transferSyntax = m_transferSyntaxes[i].c_str();

      transferSyntaxes[i] = transferSyntax;

    } // END ... For each transfer syntax
 

    //====================================
    // Build the command syntaxes list
    //=====================================
    for ( register int i(0); i < m_abstractSyntaxes.size();  i++ )
    {

      DcmLog::Debug(m_sLogSource, "Abstract Syntax " + m_abstractSyntaxes[i], "DcmServer", "AcceptAssociation");

      const char * abstractSyntax = m_abstractSyntaxes[i].c_str();

      abstractSyntaxes[i] = abstractSyntax;

    } // END ... For each abstract syntax


    DcmLog::Debug(m_sLogSource,"Attempting to accept contexts with the preferred transfer syntaxes", "DcmServer", "AcceptAssociation");

    //========================================================================
    // Attempt to accept the contexts using the preferred transfer syntaxes
    //=========================================================================
    cond = ASC_acceptContextsWithPreferredTransferSyntaxes( pAssociation->params, abstractSyntaxes, (int)abstractSyntaxCount, transferSyntaxes, (int)transferSyntaxCount);


    // Check for an error
    if ( cond.bad() )
      throw DcmAssociationException(LogRec( "Failed to receiving association : " + std::string(cond.text()), "DcmServer", "AcceptAssociation" ));

    //===================
    // set our AE title 
    //===================
    ASC_setAPTitles(pAssociation->params, __nullptr, __nullptr, m_sAETitle.c_str() );


    // Context name
    char contextName[1024];

    //================================================================
    // acknowledge or reject this association 
    //================================================================
    cond = ASC_getApplicationContextName(pAssociation->params, contextName);

    //======================================================================================
    // Make sure we can get the context name and that it is a standard application context
    //=======================================================================================
    if ( (cond.bad()) || strcmp(contextName, UID_StandardApplicationContext) != 0 )
    {
      //==========================================================
      // reject: the application context name is not supported 
      //==========================================================
      T_ASC_RejectParameters rejectionParameters =
      {
        ASC_RESULT_REJECTEDPERMANENT,
        ASC_SOURCE_SERVICEUSER,
        ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED
      };

      // TODO Association Rejected: bad application context name: %s\n
      cond = ASC_rejectAssociation(pAssociation, &rejectionParameters);

      throw DcmRejectionException(LogRec("Association rejected, context name problem", "DcmServer", "AcceptAssociation" ) );

    } // END ... If the context name cannot be retrieved or is invalid

    else if ( strlen(pAssociation->params->theirImplementationClassUID) == 0)
    {
      //===================================================
      // reject: the no implementation Class UID provided
      //=====================================================
      T_ASC_RejectParameters rejectionParameters =
      {
        ASC_RESULT_REJECTEDPERMANENT,
        ASC_SOURCE_SERVICEUSER,
        ASC_REASON_SU_NOREASON
      };

      cond = ASC_rejectAssociation(pAssociation, &rejectionParameters);

      throw DcmAssociationException(LogRec("Association rejected, missing implementation class uid", "DcmServer", "AcceptAssociation"));

    }   // END ... Else if not implementation class UID is provided

    else
    {

      // Acknowledge the association
      cond = ASC_acknowledgeAssociation(pAssociation);    

      // Check if the acknowledge was successful
      if (cond.bad())
        throw DcmAssociationException ( LogRec("Failed to acknowledge association : " + std::string(cond.text()), "DcmServer", "AcceptAssociation") );
     
      //// Get the association parameters and dump them to the log
      std::ostringstream os;
      ASC_dumpParameters(pAssociation->params,  os);

      DcmLog::Information(m_sLogSource, "Association Parameters: " + os.str(), "DcmServer", "AcceptAssociation");
 
    } // END ... else

  
    // Create the association that will do the actual processing of the request
    std::shared_ptr<DcmAssociation> pDcmAssociation = std::shared_ptr<DcmAssociation>(DcmAssociation::Create(pAssociation, m_iNetworkTimeout, callingAETitle, calledAETitle, m_sLogSource));
    
    // Start the association thread
    pDcmAssociation->Start(this, &dcm::DcmServer::AssociationCallback );

    // Add association to the list
    m_associations.insert( std::pair<std::string, std::shared_ptr<DcmAssociation> >(pDcmAssociation->GetThreadId(), pDcmAssociation) );
 

  } // END ... try
  catch ( DcmAssociationException & ex )
  {
    // Log the Error
    DcmLog::Error(m_sLogSource, util::Strings::Format("Failed to accept association : %s", ex.GetTextMessage().c_str()), "DcmServer", "AcceptAssociation" );

    // Drop the association if allocated
    if ( pAssociation != __nullptr )
      ASC_dropAssociation(pAssociation);

  }
  catch( DcmException & ex )
  {

    // Log the Error
    DcmLog::Error(m_sLogSource, util::Strings::Format("Failed to start receiver : %s", ex.GetTextMessage().c_str()), "DcmServer", "AcceptAssociation" );
    
    // Drop the association if allocated
    if ( pAssociation != __nullptr )
      ASC_dropAssociation(pAssociation);

    throw;
   
  } // END ... Catch any managed exception
  
} // AcceptAssociation()


/** 
 * Validates the called AE Title by checking against the servers AE Title. Only validate if the flag is set.
 *
 * @param asscotaionPtr     Unmanaged association pointer
 * @param msCalledAETitle   Called AE Title
 *
 */
void DcmServer::ValidateCalledAETitle(T_ASC_Association * pAssociation, const std::string & sCalledAETitle)
{

  //=================================================================================================
  // If we are validating the called AE title then extract it from the association and make sure
  // it matches the server AE Title. This is case sensitive as there are many UNIX based dicom 
  // systems.
  //=================================================================================================
  if ( m_bValidateCalledAETitle && sCalledAETitle != m_sAETitle )
  {

    //=========================================================
    // reject the connection dues to invalid Called AE Title
    //==========================================================
    T_ASC_RejectParameters rejectionParameters =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED
    };

    // Reject the association
    OFCondition cond = ASC_rejectAssociation(pAssociation, &rejectionParameters);

    // Check the rejection return code
    if ( cond.bad() )
      DcmLog::Error(m_sLogSource, "Rejection of association failed : " + std::string(cond.text()), "DcmServer", "ValidateCalledAETitle");

    // Throw association rejection exception
    throw dcm::DcmAssociationException(LogRec("Association rejected due to invalid called AE Title, does not match server;s AE Title", "Server", "ValidateCallingAETitle"));
   
  } // END .... If we are to validate the called AE TItle against the servers AE Title

} // ValidateCalledAETitle(System::IntPtr associationPtr, std::string & sCalledAETitle)


/** 
 * Validates the calling AE Title by checking against the list of accepted AE Titles. Only validate if a list was supplied.
 *
 * @param asscotaionPtr       Unmanaged association pointer
 * @param msCallingAETitle    Calling AE Title
 *
 */
void DcmServer::ValidateCallingAETitle(T_ASC_Association * pAssociation, const std::string & sCallingAETitle)
{

  //==============================================================================================
  // If there is a list of accepted AE Titles then check the calling AE Title against the list
  //==============================================================================================
  if ( m_acceptedAETitles.size() > 0 )
  {

    if ( ! util::Strings::Contains(m_acceptedAETitles,sCallingAETitle) )
    {
      //=========================================================
      // reject the connection dues to invalid calling AE Title
      //==========================================================
      T_ASC_RejectParameters rejectionParameters =
      {
        ASC_RESULT_REJECTEDPERMANENT,
        ASC_SOURCE_SERVICEUSER,
        ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED
      };

      // Reject the association
      OFCondition cond = ASC_rejectAssociation(pAssociation, &rejectionParameters);

      // Check teh reject association return code
      if ( cond.bad() )
        DcmLog::Error(m_sLogSource, util::Strings::Format("Rejection of association failed : %s",cond.text()), "DcmServer", "ValidateCallingAETitle");

      // throw an association rejected exception
      throw dcm::DcmRejectionException(LogRec("Association rejected due to invalid calling AE Title : [" + sCallingAETitle + "] is not in accepted list", "Server", "ValidateCallingAETitle"));


    } // END ... If the calling AE Title is not in the acpted list

  } // END ... If there is an accepted AE Title list


} // ValidateCallingAETitle(System::IntPtr associationPtr, std::string & sCallingAETitle)


/**
 * Cleans up finished associations
 */
void DcmServer::CleanupAssociations()
{
 
   DcmLog::Debug(m_sLogSource, "Cleaning up finished associations from list", "DcmServer", "CleanupAssociations");
 
   std::vector<std::string> list;

   std::for_each( m_associations.begin(), m_associations.end(), [&list] ( ASSOC item ) 
   {
     if ( ! item.second->IsRunning() )
       list.push_back(item.first);
   });

  // ==============================================================
  // For each removal item erase from the associations by their id
  // ===============================================================
  for ( std::vector<std::string>::iterator it = list.begin(); it != list.end(); it++ )
  {
    DcmLog::Debug(m_sLogSource, util::Strings::Format("Removing association %s from list", (*it).c_str()) , "DcmServer", "CleanupAssociations");

    /// Erase the association from the list
    m_associations.erase((*it));

  } // END ... For each association key

} // CleanupAssociations()
 

/**
 * Handles the association callbacks
 *
 * @param pParameters       Parameters from the association
 * @param lpCallbackData    Callback data 
 */
void DcmServer::AssociationCallback(PDCM_CALLBACK_PARAMETERS pParameters, LPVOID lpCallbackData )
{

  // Callback data is pointer to the server that started the association
  DcmServer * pServer(reinterpret_cast<DcmServer *>(lpCallbackData));

  // Set server specific values in the associations callback structure
  pParameters->Connection.CalledAETitle  = pServer->m_sAETitle.c_str();
  pParameters->Connection.Port           = pServer->m_iPort;
  pParameters->Connection.ServerName     = pServer->m_sName.c_str();
  
  try
  {
    /// Callback from server 
    pServer->Callback(pParameters);
  }
  catch ( DcmAbortException & )
  {
    ; // Ignore Aborts and pass back to the association
  }
  catch ( ... ) 
  {
    throw DcmException(LogRec("Unspecified exception detected","DcmServer","AsssociationCallback"));
  }
} // AssociationCallback
