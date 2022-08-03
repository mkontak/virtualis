// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File:  DcmClient.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

// includes
#include "Stdafx.h"
#include "DcmLog.h"
#include "DcmExceptions.h"
#include "DcmClient.h"
#include "DcmUtility.h"
#include "DcmImage.h"
#include "Logger.h"
#include "DcmQueryRetrieveParameters.h"
#include "DcmMoveParameters.h"
#include "DcmFindParameters.h"
#include "DcmHost.h"


// using
using namespace dcm;

/**
 * Constructor
 *
 *  @param sAETitle
 */
DcmClient::DcmClient(const std::string & sAETitle) :
DcmDevice(sAETitle),
m_sLastResponseFile(""),
m_bAbort(false),
m_presentationContextId(-1),
m_pNetwork(__nullptr),
m_pAssociation(__nullptr),
m_blockingMode(DCM_NONBLOCKING),
m_server(DcmHost::Create())
{
  Initialize();
} // DcmClient()



/**
 * Destructor
 */
DcmClient::~DcmClient()
{
 
  /// Release any association currently established
  ReleaseAssociation();

  // Disconnect any connection
  Disconnect();

} // ~DcmClient()


/**
 * Initializes the clients state
 */
void DcmClient::Initialize()
{

  // Default transfer Syntaxes
  m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
  m_transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
  m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);

  // Default abstract syntax is verification
  m_abstractSyntaxes.push_back(UID_VerificationSOPClass);


  // add the (short list of) known storage sop classes to the list
  // the array of Storage SOP Class UIDs comes from dcuid.h
  //for (int i=0; i<numberOfDcmShortSCUStorageSOPClassUIDs; i++) 
  //{
  //  m_abstractSyntaxes.push_back(dcmShortSCUStorageSOPClassUIDs[i]);
  //}

} // Initialize()


/**
 * Sets the connection information for this client, used for the COnnection Information struct in the callback
 *
 * @param info      Connection information 
 */
void DcmClient::SetConnectionInformation(DcmConnectionInformation & info)
{
  info.CallingAETitle = m_sAETitle.c_str();
  info.CalledAETitle = m_server->GetAETitle().c_str();
  info.Port = m_server->GetPort();
  info.ServerName = m_server->GetServer().c_str();
} // SetConnectionInformation(DcmConnectionInformation & info)


/**
 * Sets the server for the client and extracts the usable attributes from the entry
 *
 * @param server    Host server to use
 */
void DcmClient::SetServer(std::shared_ptr<DcmHost> & server)
{

  m_server = server;

  std::shared_ptr<DcmHostAttribute> pAttribute;

  AttributeCollection attributes = server->GetAttributes();
  
  AttributeCollection::iterator iter;

  if ((iter = attributes.find("blocking_mode")) != attributes.end())
    m_blockingMode = DcmUtility::Convert<DcmBlockingMode>(iter->second, m_blockingMode);

  if ((iter = attributes.find("network_timeout")) != attributes.end())
    m_iNetworkTimeout = DcmUtility::Convert<unsigned int>(iter->second, m_iNetworkTimeout);

} // SetServer(std::shared_ptr<DcmHost> server)


/** 
 * Creates a dicom client (SCU) from the xml node
 * 
 * @param mpNode        Xml node
 *
 * @return  DcmClient object
 */
std::shared_ptr<DcmClient>  DcmClient::Create(const std::string & sXml)
{
 
  try
  {
    /// Create the new DcmClient
    std::shared_ptr<DcmClient> pDcmClient(std::shared_ptr<DcmClient>(new DcmClient()));

    // Make sure the memory was allocated
    if ( pDcmClient== __nullptr )
      throw DcmOutOfMemoryException(LogRec("Could not allocate client", "DcmClient", "Client"));

    pDcmClient->Initialize(sXml);

    return pDcmClient;

  }
  catch ( DcmException & ex )
  {
    DcmLog::Error(LogRec(util::Strings::Format("Failed to create client : %s",ex.GetTextMessage().c_str()), "DcmClient", "Create"));
    throw; 
  }
  catch ( ... )
  {
    DcmLog::Error(LogRec("Failed to create client, check configuration", "DcmClient", "Create"));
    throw DcmConfigurationException(LogRec("Failed to create client, check configuration", "DcmClient", "Create")); 
  }

} // Create(const std::string & sXml)


/** 
 * Initializes the client with the xml supplied
 * 
 * @param mpNode        Xml node
 * @param pDcmClient    DCM client to v=be initialize
 *
 */
void DcmClient::Initialize(const std::string & sXml)
{  
  try
  {

    xml::XmlDocument document;
    document.LoadXml(sXml);

     /// Gets the document element
    xml::XmlNode docNode(document.GetDocumentElement());

   
    /// AE Title (required)
    if ( ! docNode.TryGetAttributeValue<std::string>("aetitle", m_sAETitle ) )
      throw DcmConfigurationException(LogRec("No AETitle specified", "DcmClient","Initialize"));

    /// Log source (optional) defaults to the AE title
    docNode.TryGetAttributeValue<std::string>("logSource", m_sLogSource );

    xml::XmlNode server = docNode.SelectSingleNode("server");

    // ================================================
    // If the server definition was found then create
    // ================================================
    if ( server.IsValid() )
      m_server = DcmHost::Create(server);
       

    /// Last response file (optional)
    docNode.TryGetAttributeValue<std::string>("lastResponseFile", m_sLastResponseFile);

    /// Network timeout (optional)
    docNode.TryGetAttributeValue<unsigned int>("networkTimeout", m_iNetworkTimeout);

    /// Blocking mode
    std::string sBlockingMode;
    if ( docNode.TryGetAttributeValue<std::string>("blockingMode", sBlockingMode) )
      m_blockingMode = DcmUtility::Convert<DcmBlockingMode>(sBlockingMode, m_blockingMode);   

    /// Max PDU (optional)
    docNode.TryGetAttributeValue<unsigned int>("maxPDU", m_iMaxPDU);

    /// Load the abstractSyntaxes defined in the config file ( required so the throwException flag is true )
    DcmUtility::AddSection(m_abstractSyntaxes, docNode.SelectSingleNode( "abstractSyntaxes" ));

    /// Load the transferSyntaxes from the config file ( Not required )
    DcmUtility::AddSection(m_transferSyntaxes, docNode.SelectSingleNode( "transferSyntaxes" ));

  }
  catch ( DcmException & ex )
  {
    
    DcmLog::Error(m_sLogSource, ex.GetLogRecord());

    throw;
  }
  catch ( ... )
  {

    throw DcmException(LogRec("Failed to create client, check xml", "DcmClient","Create"), m_sLogSource); 
  }

} // Initialize(const std::string & sXml)


/** 
 * Connects to the DICOM SCP
 */
void DcmClient::Connect()
{
  
  if ( m_pNetwork == __nullptr )
  {

    DcmLog::Information(m_sLogSource,util::Strings::Format("Attempting connection to %s",m_server->GetServer().c_str()), "DcmClient", "Connect" );

    //=====================================
    // make sure data dictionary is loaded 
    //=====================================
    if ( ! dcmDataDict.isDictionaryLoaded() )
       throw DcmException(LogRec("Dicom Dictionary is not loaded","DcmClient","Connect"), m_sLogSource);



    DcmAssociationConfiguration associationCfg;

    DcmLog::Debug(m_sLogSource,"Initializing network", "DcmClient", "Connect");

    const size_t transferSyntaxCount(m_transferSyntaxes.size());
    const size_t abstractSyntaxCount(m_abstractSyntaxes.size());

    const char ** transferSyntaxes = __nullptr;
    const char ** abstractSyntaxes = __nullptr;

    try
    {

      /// Make sure the memory was allocated
      if ( (transferSyntaxes = new const char * [transferSyntaxCount]) == __nullptr )
        throw DcmOutOfMemoryException(LogRec("Failed to allocation const char **", "DcmServer", "AcceptAssociation"));

      /// Make sure the memory was allocated
      if ( (abstractSyntaxes = new const char * [abstractSyntaxCount]) == __nullptr )
        throw DcmOutOfMemoryException(LogRec("Failed to allocation const char **", "DcmServer", "AcceptAssociation"));

      /// Copy the vector to the array
      DcmUtility::Copy(m_transferSyntaxes, transferSyntaxes);
      DcmUtility::Copy(m_abstractSyntaxes, abstractSyntaxes);

      //================================================================
      // initialize network, i.e. create an instance of T_ASC_Network.
      //=================================================================
      OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, m_server->GetPort(), m_iNetworkTimeout, &m_pNetwork);
      if (cond.bad())
         throw DcmException(LogRec(util::Strings::Format("Network initialization failed : %s",cond.text()), "DcmClient", "Connect"), m_sLogSource);

 
      T_ASC_Parameters * pAssocitaionParameters;

      //=====================================================================================
      // initialize association parameters, i.e. create an instance of T_ASC_Parameters
      //=====================================================================================
      cond = ASC_createAssociationParameters( &pAssocitaionParameters, m_iMaxPDU );
      if ( cond.bad() )
        throw DcmException(LogRec( util::Strings::Format("Failed creating association parameters : %s",cond.text()), "DcmClient", "Connect"), m_sLogSource);


      //===================================================================================
      // sets this application's title and the called application's title in the params
      // structure. 
      //===================================================================================
      ASC_setAPTitles(pAssocitaionParameters, m_sAETitle.c_str(), m_server->GetAETitle().c_str(), __nullptr);

      //================================================================================
      // Set the transport layer type (type of network connection) in the params 
      // structure. The default is an insecure connection; where OpenSSL is  
      // available the user is able to request an encrypted,secure connection. 
      //================================================================================
      cond = ASC_setTransportLayerType( pAssocitaionParameters, OFFalse);
      if ( cond.bad() ) 
        throw DcmException(LogRec(util::Strings::Format("Failed setting transport layer : %s",cond.text()), "DcmClient","Connect"), m_sLogSource);
 
      char peerHost[32];
      sprintf(peerHost, "%s:%d", m_server->GetServer().c_str(), m_server->GetPort());

      char hostname[64];
      int rc = gethostname(hostname,64);

      //===========================================================================================================
      // Figure out the presentation addresses and copy the corresponding values into the association parameters.
      //==========================================================================================================
      ASC_setPresentationAddresses(pAssocitaionParameters, hostname, peerHost);


      int presentationContextId = 1;

      for ( int i(0); i < abstractSyntaxCount; i++ )
      {

        cond = ASC_addPresentationContext(pAssocitaionParameters, presentationContextId,
                            abstractSyntaxes[i],  transferSyntaxes, transferSyntaxCount, ASC_SC_ROLE_DEFAULT);

        if ( cond.bad() )
          throw DcmException(LogRec(util::Strings::Format("Failed adding presentation context [%s] : %s", abstractSyntaxes[i], cond.text()), "DcmClient", "Connect"), m_sLogSource);

        presentationContextId += 2;

      } // END ... For each abstract syntax

      std::ostringstream os;
      os << "Association Request Parameters After: ";
      ASC_dumpParameters(pAssocitaionParameters, os);
      DcmLog::Debug(m_sLogSource,os.str(), "DcmClient", "Connect");


      /// Request the association with the server
      cond = ASC_requestAssociation(m_pNetwork, pAssocitaionParameters, &m_pAssociation);

      // ====================================
      // Check the response from the request
      // ====================================
      if ( cond.bad() )
      {
        if (cond == DUL_ASSOCIATIONREJECTED) 
        {
          T_ASC_RejectParameters rejection;

          std::ostringstream os;

          os << "Association rejected : ";

          ASC_getRejectParameters(pAssocitaionParameters, &rejection);
          ASC_printRejectParameters(os, &rejection);

          throw DcmRejectionException(LogRec(os.str(), "DcmClient", "Connect"), m_sLogSource);
       } 
       else 
       {

         throw DcmConnectionException(LogRec(util::Strings::Format("Association request failed : %s",cond.text()), "DcmClient", "Connect"), m_sLogSource);

         //DimseCondition::dump(cond);     
        }

      } // END ... If the association was rejected or failed

      os.clear();
      os << "Association Request Parameters After: ";
      ASC_dumpParameters(pAssocitaionParameters, os);
      DcmLog::Debug(m_sLogSource, os.str(), "DcmClient", "Connect");

      //===========================================================================
      // count the presentation contexts which have been accepted by the SCP 
      // If there are none, finish the execution 
      //==========================================================================
      if (ASC_countAcceptedPresentationContexts(pAssocitaionParameters) == 0) 
        throw DcmException(LogRec("No acceptable presentation contexts found", "DcmClient", "Connect"), m_sLogSource);

      if ( transferSyntaxes != __nullptr )
        delete [] transferSyntaxes;

      if ( abstractSyntaxes != __nullptr )
        delete [] abstractSyntaxes;

    }
    catch ( DcmException & ex )
    {

      if ( transferSyntaxes != __nullptr )
        delete [] transferSyntaxes;

      if ( abstractSyntaxes != __nullptr )
        delete [] abstractSyntaxes;
 
      Disconnect();

      throw DcmConnectionException(ex.GetLogRecord(), m_sLogSource);

    }
    catch ( ... )
    {
      if ( transferSyntaxes != __nullptr )
        delete [] transferSyntaxes;

      if ( abstractSyntaxes != __nullptr )
        delete [] abstractSyntaxes;

      throw DcmConnectionException(LogRec("Client connection failed with unknown error", "DcmClient", "Connect"), m_sLogSource);

    }

  } // END ... If we are not already connected

} // Connect()


/** 
 * Disconnects from the DICOM SCP
 */
void DcmClient::Disconnect()
{

  /// Condition
  OFCondition cond;

  //================================================
  // We only drop the association if it was created
  //================================================
  if (m_pAssociation != __nullptr )
  {

    //=======================================================================================
    // destroy the association, i.e. free memory of T_ASC_Association* structure. This 
    // call is the counterpart of ASC_requestAssociation(...) which was called in Connect()
    //=======================================================================================
    cond = ASC_destroyAssociation(&m_pAssociation);

    if ( cond.bad() )
      DcmLog::Debug(m_sLogSource, util::Strings::Format("Failed to destroy association : %s", cond.text()), "DcmClient", "Disconnect");

    m_pAssociation = __nullptr;
   
  }

  //================================================
  // We only destroy the network if it was created
  //================================================
  if ( m_pNetwork != __nullptr )
  {
    //================================================================================
    // Drop the network, i.e. free memory of T_ASC_Network* structure. This call 
    // is the counterpart of ASC_initializeNetwork(...) which was called above. 
    //================================================================================
    cond = ASC_dropNetwork(&m_pNetwork);

    if ( cond.bad() ) 
      DcmLog::Debug(m_sLogSource, util::Strings::Format("Failed to drop network : %s", cond.text()), "DcmClient", "Disconnect");
    
    m_pNetwork = __nullptr;

  } // END ... If the network has been created

} // Disconnect()



/** 
 * Releases the association and destroys it (cleanup resources) then sets it to __nullptr. Since this call is done 
 * at the end of an operation no exception will be thrown on warnings will be logged. 
 */
void DcmClient::ReleaseAssociation( )
{

  if ( m_pAssociation != __nullptr )
  {
    //=========================
    // Release the association
    //===========================
    OFCondition cond = ASC_releaseAssociation(m_pAssociation);

    if (cond.bad())
      DcmLog::Warning(m_sLogSource, util::Strings::Format("Failed to release the association : %s", cond.text()), "DcmClient", "ReleaseAssociation");

    //=============================================================================================================
    // destroy the association, i.e. free memory of T_ASC_Association* structure. This call is the counterpart 
    // of ASC_requestAssociation(...) which was called above.
    //============================================================================================================
    cond = ASC_destroyAssociation(&m_pAssociation);

    if ( cond.bad() ) 
      DcmLog::Warning(m_sLogSource, util::Strings::Format("Failed to destroy the association : %s",cond.text()), "DcmClient", "ReleaseAssociation");

    m_pAssociation = __nullptr;
  } // if

} // ReleaseAssociation()


/** 
 * Aborts the association and destroys it (cleanup resources) then sets it to __nullptr. Since this call is done 
 * at the end of an operation no exception will be thrown only warnings will be logged. 
 */
void DcmClient::AbortAssociation( )
{

  if ( m_pAssociation != __nullptr )
  {
    //=========================
    // Release the association
    //===========================
    OFCondition cond = ASC_abortAssociation(m_pAssociation);

    if (cond.bad())
      DcmLog::Warning(m_sLogSource, util::Strings::Format("Failed to abort the association : %s",cond.text()), "DcmClient", "AbortAssociation");

    //==============================
    // Destroy the association
    //==============================
    cond = ASC_destroyAssociation(&m_pAssociation);

    if ( cond.bad() ) 
      DcmLog::Warning(m_sLogSource, util::Strings::Format("Failed to destroy the association : %s",cond.text()), "DcmClient", "AbortAssociation");

    m_pAssociation = __nullptr;
  } // if

} // AbortAssociation(T_ASC_Association ** ppAssociation )


/**
 * Writes the last repsonse
 *
 *  @param response
 */
void DcmClient::WriteLastResponse(T_DIMSE_C_StoreRSP response)
{

  if ( ! m_sLastResponseFile.empty() )
  {

   FILE * fp(__nullptr);
   
   if ( ( fp = fopen(GetLastResponseFile().c_str(), "rw") ) != __nullptr )
   {
         
      try
      {
        DIMSE_printCStoreRSP(fp, &response);
      }
      catch ( ... )
      {
        DcmLog::Warning(util::Strings::Format("Failed to write last response [%d]", GetLastError()),"DcmClient","WriteLastResponse", GetLogSource() );  
      }

      // close the file
      fclose(fp);
   }
   else
     DcmLog::Warning(util::Strings::Format("Failed to open last response file %s [%d]", m_sLastResponseFile.c_str(), GetLastError()),"DcmClient","WriteLastResponse", GetLogSource() );

  } // END ... If the last response file was specified

} // WriteLastResponse(T_DIMSE_C_StoreRSP response)


#pragma region Command Methods

/**
 * Stores supplied image to the SCP
 *
 * @param pDcmImage       Image to be stored
 */
void DcmClient::Store(const std::shared_ptr<DcmImage> & pDcmImage)
{
  if ( m_pAssociation != __nullptr )
  {
   
    DcmDataset * pStatusDetail( __nullptr);
 
    // =========================================================
    // Make sure the status detail is deleted 
    // when we leave this method
    // =========================================================
    SCOPE_EXIT
    {
      if ( pStatusDetail != __nullptr )
        delete pStatusDetail;

    };

    try
    {

      T_DIMSE_C_StoreRQ request;
      T_DIMSE_C_StoreRSP response;
      DIC_UI sopClass;
      DIC_UI sopInstance;
     DIC_US messageId = m_pAssociation->nextMsgID++;

      request.Priority = DIMSE_PRIORITY_MEDIUM;

      /// Extract the unmanaged DcmDataset from the dicom image
      DcmDataset * pDcmDataset(pDcmImage->GetDcmDataset());


      std::shared_ptr<DcmHostAttribute> pAttribute;
     
      //==========================================================================
      // figure out which SOP class and SOP instance is encapsulated in the file 
      //==========================================================================
      if ( !DU_findSOPClassAndInstanceInDataSet(pDcmDataset, sopClass, sopInstance, OFFalse) ) 
        throw DcmInvalidDataException(LogRec("No SOP Class and Instance UIDs in dataset", "DcmClient","Store"), GetLogSource());
  
      //=========================================================================
      // figure out which of the accepted presentation contexts should be used 
      //=========================================================================
      DcmXfer dcmXfer(pDcmDataset->getOriginalXfer());

      m_presentationContextId = ( dcmXfer.getXfer() != EXS_Unknown ? ASC_findAcceptedPresentationContextID(m_pAssociation, sopClass, dcmXfer.getXferID()) : 
                                            ASC_findAcceptedPresentationContextID(m_pAssociation, sopClass) );

      //========================================================================
      // If no presentation context was found to send on then throw exception
      //========================================================================
      if ( m_presentationContextId == 0 ) 
        throw DcmException(LogRec(util::Strings::Format("No presentation context for : %s %s", DcmUtility::GetModalityName(sopClass).c_str(), sopClass), "DcmClient", "Store"), GetLogSource());


      DcmXfer fileXfer(pDcmDataset->getOriginalXfer());
      T_ASC_PresentationContext presentationContext;
      ASC_findAcceptedPresentationContext(m_pAssociation->params, m_presentationContextId, &presentationContext);
      DcmXfer netXfer(presentationContext.acceptedTransferSyntax);

      DcmLog::Debug(m_sLogSource, util::Strings::Format("Transfer: %s -> %s", dcmFindNameOfUID(fileXfer.getXferID()), dcmFindNameOfUID(netXfer.getXferID())), "DcmClient", "Store" );
      

      //==================================
      // prepare the transmission of data 
      //==================================
      bzero((char*)&request, sizeof(request));
      request.MessageID = messageId;
      strcpy_s(request.AffectedSOPClassUID, sizeof(sopClass), sopClass );
      strcpy_s(request.AffectedSOPInstanceUID, sizeof(sopInstance), sopInstance);
      request.DataSetType = DIMSE_DATASET_PRESENT;
      request.Priority = DIMSE_PRIORITY_LOW;

      DcmLog::Debug(m_sLogSource, util::Strings::Format("Store SCU RQ: MsgID %d, (%s)", messageId, dcmSOPClassUIDToModality(sopClass)), "DcmClient", "Store") ;
          

      // Transmit data
      OFCondition cond = DIMSE_storeUser( m_pAssociation, 
                                          m_presentationContextId, 
                                          &request, 
                                          __nullptr, 
                                          pDcmDataset, 
                                          &dcm::DcmClient::StoreCallback, 
                                          //__nullptr,
                                          this,
                                          (T_DIMSE_BlockingMode)m_blockingMode, 
                                          m_iNetworkTimeout, 
                                          &response, 
                                          &pStatusDetail );
      
      //===========================================================================================================
      // If store command completed normally, with a status of success or some warning then the image was accepted.
      //===========================================================================================================
      if ( cond == EC_Normal && (response.DimseStatus == STATUS_Success || DICOM_WARNING_STATUS(response.DimseStatus)))
        WriteLastResponse(response);
      else
        throw DcmException(LogRec(util::Strings::Format( "Store of image failed [0x%04x] : %s", (int)response.DimseStatus, cond.text()), "DcmClient","Store"), GetLogSource() );

    }
    catch ( DcmException & )
    {
      throw;
    }
    catch ( ... )
    {
      throw DcmException(LogRec("Store of image failed due to unspecified error", "DcmClient", "Store"), GetLogSource() );
    }

  } // END ... If there is an association

} // Store()


/**
 * Moves the specified UID to the destination AE Title
 *
 * @param pParameters       Move operation parameters
 */
void DcmClient::Move(const std::shared_ptr<DcmMoveParameters> & parameters)
{

  // Move Request
  T_DIMSE_C_MoveRQ request;
  RtlZeroMemory( &request, sizeof(T_DIMSE_C_MoveRQ));

  // Move Response
  T_DIMSE_C_MoveRSP response;
  RtlZeroMemory( &response, sizeof(T_DIMSE_C_MoveRSP));

  DcmDataset * pStatusDetail(__nullptr);
  DcmDataset * pResponseIds(__nullptr);

  // =========================================================
  // Make sure the status detail and reponse ids are deleted 
  // when we leave this method
  // =========================================================
  SCOPE_EXIT
  {
    if ( pStatusDetail != __nullptr )
      delete pStatusDetail;

    if ( pResponseIds != __nullptr )
      delete pResponseIds;
  };

  DIC_US messageId = m_pAssociation->nextMsgID++;

  /// Get the move parameters DcmImage representation
  std::shared_ptr<DcmImage> pDcmImage(parameters->GetDcmImage());

  std::string affectedSopClassUid(parameters->GetAffectedSopInstanceUid());
  DIC_UI sopClass;
  RtlZeroMemory(sopClass, sizeof(sopClass));

  strncpy(sopClass, affectedSopClassUid.c_str(), sizeof(DIC_UI));

  m_presentationContextId = ASC_findAcceptedPresentationContextID(m_pAssociation, sopClass); 

  //========================================================================
  // If no presentation context was found to send on then throw exception
  //========================================================================
  if ( m_presentationContextId == 0 ) 
    throw DcmException(LogRec(util::Strings::Format("No presentation context for : %s %s", DcmUtility::GetModalityName(sopClass).c_str(), sopClass), "DcmClient", "Move"), GetLogSource());

 
  std::string sDestination(parameters->GetDestinationAEtitle());

  // Setup the 
  request.MessageID   = messageId;
  request.Priority    = DIMSE_PRIORITY_MEDIUM;
  request.DataSetType = DIMSE_DATASET_PRESENT;
  strncpy(  request.AffectedSOPClassUID, sopClass, sizeof(DIC_UI) );
  strncpy(  request.MoveDestination,     sDestination.c_str(), sDestination.size() );


  OFCondition cond = DIMSE_moveUser(  m_pAssociation, 
                                      m_presentationContextId,
                                      &request,
                                      pDcmImage->m_pDcmDataset,
                                      &dcm::DcmClient::MoveCallback, 
                                      this, 
                                      (T_DIMSE_BlockingMode)m_blockingMode, 
                                      m_iNetworkTimeout,
                                      m_pNetwork, 
                                      &dcm::DcmClient::SubOpCallback,
                                      this,
                                      &response, 
                                      &pStatusDetail, 
                                      &pResponseIds,
                                      OFTrue );

  /// Dump the move command to a separate file
  DcmUtility::DumpMoveCommand( parameters->GetLogFile(), pDcmImage->m_pDcmDataset, &request, &response, pResponseIds, pStatusDetail);

  if (cond == EC_Normal) 
  {


    // ===================================================
    // Throw the exception appropriate to the status code
    // ====================================================
    switch ( response.DimseStatus) 
    {
    case STATUS_Success:
      break;

    case  STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches:
      throw DcmMoveException(LogRec("Out of resources (number of matches)", "DcmClient", "Move"), m_sLogSource);
      break;

    case  STATUS_MOVE_Refused_OutOfResourcesSubOperations:
      throw DcmMoveException(LogRec("Out of resources (sub-operations)", "DcmClient", "Move"), m_sLogSource);
      break;

    case  STATUS_MOVE_Failed_SOPClassNotSupported:
      throw DcmMoveException(LogRec("SOP class is not supported", "DcmClient", "Move"), m_sLogSource);
      break;

    case  STATUS_MOVE_Failed_MoveDestinationUnknown:
      throw DcmMoveException(LogRec("Move destination is unknown", "DcmClient", "Move"), m_sLogSource);
      break;

    case  STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass:
      throw DcmMoveException(LogRec("Identifier does not match SOP class", "DcmClient", "Move"), m_sLogSource);
      break;

    case  STATUS_MOVE_Failed_UnableToProcess:
      throw DcmMoveException(LogRec("Unable to process", "DcmClient", "Move"), m_sLogSource);
      break;

    case  STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication:
      throw DcmMoveException(LogRec("Sub-operation terminated due to cancel", "DcmClient", "Move"), m_sLogSource);
      break;

    case  STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures:
      DcmLog::Warning(m_sLogSource, "Sub-operation completed with one or more failures", "DcmClient","Move");
      break;

    default:
      if ( ( STATUS_MOVE_Failed_UnableToProcess & response.DimseStatus ) == STATUS_MOVE_Failed_UnableToProcess )
        throw DcmMoveException(LogRec("Unable to process", "DcmClient", "Move"), m_sLogSource);
      else
        throw DcmMoveException(LogRec("Unknown error", "DcmClient", "Move"), m_sLogSource);
      break;

    } // END ... Switch of status        
        
  } 
  else 
  {

    throw DcmMoveException(LogRec(util::Strings::Format("C-MOVE failed : %s",cond.text()), "DcmClient", "Move"), m_sLogSource);
  }

} // Move(const std::string & sUid)


/**
 * Performs the C-FIND commands
 *
 *  @param parameters
 */
void DcmClient::Find(const std::shared_ptr<DcmFindParameters> & parameters)
{

  /// Find request
  T_DIMSE_C_FindRQ request;
  RtlZeroMemory(&request,sizeof(T_DIMSE_C_FindRQ));

  /// Find response
  T_DIMSE_C_FindRSP response;
  RtlZeroMemory(&response,sizeof(T_DIMSE_C_FindRSP));

  // Status detail
  DcmDataset * pStatusDetail(__nullptr);

  // =========================================================
  // Make sure the status detail is deleted 
  // when we leave this method
  // =========================================================
  SCOPE_EXIT
  {
    if ( pStatusDetail != __nullptr )
      delete pStatusDetail;

  };

  // Gets the DcmImage for the find
  std::shared_ptr<DcmImage> pDcmImage(parameters->GetDcmImage());

  // Gets the message id
  DIC_US messageId(m_pAssociation->nextMsgID++);

  // Affected SOP class UID
  std::string affectedSopClassUid(parameters->GetAffectedSopInstanceUid());

  // Setup the request
  request.Priority    = DIMSE_PRIORITY_MEDIUM;
  request.MessageID   = messageId;
  request.Priority    = DIMSE_PRIORITY_MEDIUM;
  request.DataSetType = DIMSE_DATASET_PRESENT;
  strncpy(request.AffectedSOPClassUID, affectedSopClassUid.c_str(), affectedSopClassUid.size());

 
  m_presentationContextId = ASC_findAcceptedPresentationContextID(m_pAssociation, affectedSopClassUid.c_str()); 

  //========================================================================
  // If no presentation context was found to send on then throw exception
  //========================================================================
  if ( m_presentationContextId == 0 ) 
    throw DcmException(LogRec(util::Strings::Format("No presentation context for : %s %s", DcmUtility::GetModalityName(affectedSopClassUid.c_str()).c_str(), affectedSopClassUid.c_str()), "DcmClient", "Find"), GetLogSource());
  
 
  /// Perform query
  OFCondition cond = DIMSE_findUser( m_pAssociation, 
                                     m_presentationContextId, 
                                     &request, 
                                     pDcmImage->m_pDcmDataset,
                                     &dcm::DcmClient::FindCallback, 
                                     this,
                                     (T_DIMSE_BlockingMode)m_blockingMode, 
                                     m_iNetworkTimeout,
                                     &response, 
                                     &pStatusDetail);

  /// Dump the find command to a separate file
  DcmUtility::DumpFindCommand( parameters->GetLogFile(), pDcmImage->m_pDcmDataset, &request, &response, pStatusDetail);

  if (cond == EC_Normal) 
  {

    // ===================================================
    // Throw the exception appropriate to the status code
    // ====================================================
    switch ( response.DimseStatus) 
    {
    case STATUS_Success:
      break;

    case  STATUS_FIND_Refused_OutOfResources:
      throw DcmFindException(LogRec("Out of resources (number of matches)", "DcmClient", "Find"), m_sLogSource);
      break;


    case  STATUS_FIND_Refused_SOPClassNotSupported:
      throw DcmFindException(LogRec("SOP class is not supported", "DcmClient", "Find"), m_sLogSource);
      break;


    case  STATUS_FIND_Failed_IdentifierDoesNotMatchSOPClass:
      throw DcmFindException(LogRec("Identifier does not match SOP class", "DcmClient", "Find"), m_sLogSource);
      break;

    case  STATUS_FIND_Failed_UnableToProcess:
      throw DcmFindException(LogRec("Unable to process", "DcmClient", "Find"), m_sLogSource);
      break;

    case  STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest:
      throw DcmFindException(LogRec("Terminated due to cancel", "DcmClient", "Find"), m_sLogSource);
      break;

    case  STATUS_FIND_Pending_WarningUnsupportedOptionalKeys:
      DcmLog::Warning(m_sLogSource, "Unsupported optional keys", "DcmClient","Find");
      break;

    default:
      if ( ( STATUS_FIND_Failed_UnableToProcess & response.DimseStatus ) == STATUS_FIND_Failed_UnableToProcess )
        throw DcmFindException(LogRec("Unable to process", "DcmClient", "Find"), m_sLogSource);
      else
        throw DcmFindException(LogRec("Unknown error", "DcmClient", "Find"), m_sLogSource);
      break;

    } // END ... Switch of status        
        
  } 
  else 
  {
     throw DcmFindException(LogRec(util::Strings::Format("C-FIND failed : %s",cond.text()), "DcmClient", "Find"), m_sLogSource);
  }

} // Find()

#pragma endregion Command Methods


#pragma region Callback

/*
 * Callback method used to record status of the storage operation launched using the CStore() method
 *
 * @param   pCallbackData     [in]      data for this callback function
 * @param   pProgress         [in]      The state of progress. (identifies if this is the initial or final call
 *                                      to this function, or a call in between these two calls.
 * @param   pStoreRequest     [in]      The original store request message.
 */
void  DcmClient::StoreCallback( void * pCallbackData, T_DIMSE_StoreProgress * pProgress, T_DIMSE_C_StoreRQ * pRequest )
{

  
  DcmClient * pDcmClient = reinterpret_cast<DcmClient *>(pCallbackData);
  
  // =========================================
  // Only check on an END
  //  DIMSE_StoreBegin
  //  DIMSE_StoreEnd
  //  DIMSE_StoreProgressing
  // =========================================
  if (pProgress->state == DIMSE_StoreEnd && pDcmClient->m_bAbort == true)
  {
    DcmLog::Debug(pDcmClient->GetLogSource(), util::Strings::Format("Sending Cancel RQ, MsgId: %d, PresId: %d", pRequest->MessageID, pDcmClient->m_presentationContextId), "DcmClient", "StoreCallback");

    OFCondition  cond = DIMSE_sendCancelRequest(pDcmClient->m_pAssociation, pDcmClient->m_presentationContextId, pRequest->MessageID);

    if (cond != EC_Normal)
      DcmLog::Error(pDcmClient->GetLogSource(), util::Strings::Format("C-STORE cancel failed : %s", cond.text()), "DcmClient", "StoreCallback");

  } // END ... switch
  
} // StoreCallback(   void * pCallbackData, T_DIMSE_StoreProgress * pProgress, T_DIMSE_C_StoreRQ * pStoreRequest ) 


/**
 * HAndles the callback from the move operation.
 * 
 * @param   pCallbackData     [in]      data for this callback function
 * @param   pRequest          [in]      The initial move request
 * @param   iResponseCount    [in]      Number of responses received from the server (Used as an image count)
 * @param   pResponse         [out]     Response
 *
 */
void DcmClient::MoveCallback(void * pCallbackData, T_DIMSE_C_MoveRQ * pRequest, int iResponseCount, T_DIMSE_C_MoveRSP * pResponse)
{

   DcmClient * pDcmClient = reinterpret_cast<DcmClient *>(pCallbackData);

   DcmLog::Debug( pDcmClient->GetLogSource(), util::Strings::Format("C-MOVE received response %d", iResponseCount), "DcmClient", "MoveCallback" );

  // ====================================
  // Abort/Cancel operation
  // ====================================
  if ( pDcmClient->m_bAbort ) 
  {

    DcmLog::Debug( pDcmClient->GetLogSource(), util::Strings::Format("Sending Cancel RQ, MsgId: %d, PresId: %d", pRequest->MessageID, pDcmClient->m_presentationContextId), "DcmClient", "MoveCallback");

    OFCondition  cond = DIMSE_sendCancelRequest(pDcmClient->m_pAssociation, pDcmClient->m_presentationContextId, pRequest->MessageID);

    if (cond != EC_Normal) 
      DcmLog::Error( pDcmClient->GetLogSource(), util::Strings::Format("C-MOVE cancel failed : %s", cond.text()),"DcmClient", "MoveCallback");

  } // 

  ///
  pDcmClient->CallbackRelay(pResponse, __nullptr);

} // MoveCallback(void * pCallbackData, T_DIMSE_C_MoveRQ * pRequest, int iResponseCount, T_DIMSE_C_MoveRSP * pResponse)


/**
 * HAndles the callback from the move operation.
 * 
 * @param   pCallbackData     [in]      data for this callback function
 * @param   pRequest          [in]      The initial move request
 * @param   iResponseCount    [in]      Number of responses received from the server (Used as an image count)
 * @param   pResponse         [out]     Response
 * @param   pResponseIds      [out]
 *
 */
void DcmClient::FindCallback(void * pCallbackData, T_DIMSE_C_FindRQ * pRequest, int iResponseCount, T_DIMSE_C_FindRSP * pResponse, DcmDataset * pResponseIds )
{

   DcmClient * pDcmClient = reinterpret_cast<DcmClient *>(pCallbackData);

   DcmLog::Debug( pDcmClient->GetLogSource(), util::Strings::Format("C-FIND received data %d [Status: %s]", iResponseCount, DU_cfindStatusString(pResponse->DimseStatus)), "DcmClient", "FindCallback" );
   
  // ====================================
  // Abort/Cancel operation
  // ====================================
  if ( pDcmClient->m_bAbort ) 
  {

    DcmLog::Debug( pDcmClient->GetLogSource(), util::Strings::Format("Sending Cancel RQ, MsgId: %d, PresId: %d", pRequest->MessageID, pDcmClient->m_presentationContextId), "DcmClient", "FindCallback");

    OFCondition  cond = DIMSE_sendCancelRequest(pDcmClient->m_pAssociation, pDcmClient->m_presentationContextId, pRequest->MessageID);

    if (cond != EC_Normal) 
      DcmLog::Error( pDcmClient->GetLogSource(), util::Strings::Format("C-FIND cancel failed : %s", cond.text()), "DcmClient", "FindCallback");

  } // 
  else
  {
    /// Relay the callback
    pDcmClient->CallbackRelay(pResponse, pResponseIds);
  }

} // FindCallback(void * pCallbackData, T_DIMSE_C_FindRQ * pRequest, int iResponseCount, T_DIMSE_C_FindRSP * pResponse, DcmDataset * pResponseIds )


/**
 * Sub-OPeration callback 
 *
 * @param pCallbackData       Callback data
 * @param pNetwork            Network
 * @param ppSubAssociation    Association
 */
void DcmClient::SubOpCallback(void * pCallbackData, T_ASC_Network * pNetwork, T_ASC_Association ** ppSubAssociation)
{

    if (pNetwork == NULL) return;   

    if (*ppSubAssociation == NULL) 
    {
      // Negotiate an association
    } else 
    {
      // SCP
    }
} // SubOpCallback(void * pCallbackData, T_ASC_Network * pNetwork, T_ASC_Association ** ppSubAssociation)

#pragma endregion Callbacks



