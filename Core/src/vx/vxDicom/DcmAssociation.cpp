// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmAssociation.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"

#include "DcmLog.h"
#include "DcmAssociation.h"
#include "DcmUtility.h"
#include "DcmExceptions.h"
#include "DcmQueryRetrieveParameters.h"
#include "DcmFindParameters.h"
#include "DcmMoveParameters.h"
#include "DcmStore.h"
#include "DcmHost.h"
#include "DcmLog.h"
#include "DcmImageFactory.h"
#include "DcmHostTable.h"

using namespace dcm;

/**
 * Constructor
 *
 * @param
 * @param
 * @param
 * @param
 *
 */
DcmAssociation::DcmAssociation(  )  : 
DcmThread("Starting association to handle the dicom connection"),
m_sLogSource(vxLogFactory::GetDefaultLogger()),
m_eCommandType(C_NONE),
m_sCallingAETitle(""),
m_sCalledAETitle(""),
m_hThread(__nullptr),
m_pAssociation(__nullptr),
m_store(__nullptr),
m_iNetworkTimeout(20)
{   
} // DcmAssociation( const std::string & sLogSource, T_ASC_Association * pAssociation,  int iNetworkTimeout )  : 


/**
 * Finalizer for the receiver, drops and closes the association
 */
DcmAssociation::~DcmAssociation()
{
  DcmLog::Debug(m_sLogSource, "Destroying association", "DcmAssociation", "~DcmAssociation");

  if ( m_store != __nullptr )
    m_store.reset();

  /// If we are still running abort
  if ( this->IsRunning() )
    Abort();
  
} // !DcmAssociation


/**
 *  Creates an association object that handles the rest of the cuumuncation between our server and the SCU making the request. 
 *  Will  call the appropriate processing method based on the command received. 
 *
 *  @param pAssociation             Association we are wrapping
 *  @param iNetworkTimeout          Network timeout for communications
 *  @param sCallingAETitle          Calling AETitle (SCU)
 *  @param sCalledAETitle           Called AE TItle (SCP) 
 *  @param sLogSource               Log source
 *
 *  @return the pointer to the association
 */
std::shared_ptr<DcmAssociation> DcmAssociation::Create( T_ASC_Association * pAssociation,  int iNetworkTimeout, const std::string & sCallingAETitle, const std::string & sCalledAETitle, const std::string & sLogSource ) 
{

  /// Create the object
  std::shared_ptr<DcmAssociation> pDcmAssociation = std::shared_ptr<DcmAssociation>(new DcmAssociation());

  /// Make sure we did not run out of memory
  if ( pDcmAssociation == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to create the DcmAssociation", "DcmAssociation", "Create"));
 

  //  // Get the called/calling AE Title from the association
  //char callingAETitle[32]; 
  //RtlZeroMemory((void *)callingAETitle,sizeof(callingAETitle));
  //char calledAETitle[32];
  //RtlZeroMemory((void *)calledAETitle,sizeof(calledAETitle));

  //ASC_getAPTitles(pAssociation->params, callingAETitle, calledAETitle, __nullptr);
  // int count(ASC_countPresentationContexts(pAssociation->params));

  /// Set internal fields
  pDcmAssociation->m_sLogSource       = sLogSource;
  pDcmAssociation->m_pAssociation     = pAssociation;
  pDcmAssociation->m_iNetworkTimeout  = iNetworkTimeout;
  pDcmAssociation->m_sCalledAETitle   = sCalledAETitle;
  pDcmAssociation->m_sCallingAETitle  = sCallingAETitle;

  return pDcmAssociation;
 
} // DcmAssociation( const std::string & sLogSource, T_ASC_Association * pAssociation,  int iNetworkTimeout )  : 


/**
 * Aborts the association
 *
 */
void DcmAssociation::Abort()
{

  // Log aborting message
   DcmLog::Information(m_sLogSource, util::Strings::Format("Aborting association %s", GetThreadId().c_str()), "DcmAssociation", "Abort");
 
   // Abort the association
   AbortAssociation();

   // abort the thread
   DcmThread::Abort();

} // Abort()


/** 
 * Runs thread for the association synchrounously
 */
void DcmAssociation::RunSynchronously()
{

  /// Set the state of the association as running
  m_eState = S_RUNNING;

  OFCondition cond = EC_Normal;
  T_DIMSE_Message message;
  T_ASC_PresentationContextID presentationContextId = 0;
  DcmDataset * pStatusDetail = __nullptr;
  
  try
  {

    // Log information abort the 
    DcmLog::Information(m_sLogSource, util::Strings::Format("Starting association - Called AE Title = %s ,  Calling AE Title = %s", m_sCalledAETitle.c_str(), m_sCallingAETitle.c_str()), "DcmAssociation", "Run");

    /// Association started
    Callback(E_STARTED);


    //==================================================================
    // start a loop to be able to receive more than one DIMSE command
    //==================================================================
    while( cond == EC_Normal || cond == DIMSE_NODATAAVAILABLE || cond == DIMSE_OUTOFRESOURCES )
    {

      // Receive the command
      cond = DIMSE_receiveCommand(m_pAssociation, DIMSE_BLOCKING, 0, &presentationContextId, &message, &pStatusDetail);
 
      // Prints the status detail to the log
      DcmUtility::PrintStatusDetail(pStatusDetail);


      //===================================================================
      // Process based on the command received
      //====================================================================
      if (cond == EC_Normal)
      {

        //==========================================
        // Process the command if valid
        //========================================
        switch (message.CommandField)
        {

          case DIMSE_C_ECHO_RQ:                       // C-ECHO
            // process C-ECHO-Request
            cond = ProcessEchoCommand(&message, presentationContextId);
            break;

          case DIMSE_C_STORE_RQ:                      // C-STORE
            // process C-STORE-Request
            cond = ProcessStoreCommand(&message, presentationContextId);
            break;

          case DIMSE_C_MOVE_RQ:                       // C-MOVE
            // process C-MOVE-Request
            cond = ProcessMoveCommand(&message, presentationContextId);
            break;

          case DIMSE_C_GET_RQ:                        // C-GET
            // process C-GET-Request
            cond = ProcessGetCommand(&message, presentationContextId);
            break;

          case DIMSE_C_FIND_RQ:                       // C-FIND
            // process C-FIND-Request
            cond = ProcessFindCommand(&message, presentationContextId);
            break;

          default:
            {
              // we cannot handle this kind of message
              cond = DIMSE_BADCOMMANDTYPE;

              throw DcmUnsupportedException(LogRec(util::Strings::Format("Cannot handle command: %4x", (int)message.CommandField), 
                                            "DcmAssociation", "Run"), m_sLogSource);
            }
            break;

        } // END ... switch command field

      } // END ... If the condition returned from receive command was normal
      else
      {
        DcmLog::Warning(m_sLogSource, util::Strings::Format("Non-Normal condition while receiving command : %s",cond.text()), 
                        "DcmAssociation", "Run");
      }
     
    } // END ... while

    //===================================================================
    // The condition indicates that a association release was requested
    //===================================================================
    if (cond == DUL_PEERREQUESTEDRELEASE)
    {

      // Log message indicatin we recieved a release
      DcmLog::Information(m_sLogSource, "Acknowledging the release of the association by the SCU", "DcmAssociation", "Run");

      // Acknowledge the release
      cond = ASC_acknowledgeRelease(m_pAssociation);

      // Callback completed
      Callback(E_COMPLETED);

    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
      throw DcmConnectionException(LogRec(  "Peer aborted association", "DcmAssociation", "Run"), m_sLogSource );
    }
    else
    {    
      throw DcmConnectionException(LogRec( util::Strings::Format("DIMSE failure : %s",cond.text()),"DcmAssociation", "Run"), m_sLogSource );
    } // END ... Else

  }
  catch ( DcmAbortException &   )
  {

    // Log Information
    DcmLog::Information(m_sLogSource, "Association aborted", "DcmAssociation", "Run");

    // Abort the association
    AbortAssociation();

    // Callback aborted
    Callback(E_ABORTED);

    throw;

  }
  catch ( DcmException & ex )
  {

    // Set the error message
    m_callbackParameters.Message = ex.GetTextMessage().c_str();

    // Log Error
    DcmLog::Error(m_sLogSource, util::Strings::Format("Association failed : %s", ex.GetTextMessage().c_str()), "DcmAssociation", "Run");

    // Cllbak failed
    Callback(E_FAILED);

    // Abort the association
    AbortAssociation();

    throw;
  }
 
}   // Run()


/** 
 * Aborts the association
 */
void DcmAssociation::AbortAssociation()
{

  //===========================================
  // Only abort if the there is an association
  //===========================================
  if ( m_pAssociation != __nullptr )
  {

    DcmLog::Information(m_sLogSource, "Aborting association", "DcmAssociation", "Abort");
  
    // Abort the association
    OFCondition cond = ASC_abortAssociation(m_pAssociation);

    if (cond.bad())
      DcmLog::Error(m_sLogSource, "Failed to drop association : " + std::string(cond.text()),"DcmAssociation", "Abort");
    
    T_ASC_Association * pAssociation = m_pAssociation;

    DcmLog::Information(m_sLogSource, "Destroying the association", "DcmAssociation", "Abort");

    cond = ASC_destroyAssociation(&pAssociation);
    if (cond.bad())
      DcmLog::Error(m_sLogSource, "Failed to destroy association : " + std::string(cond.text()), "DcmAssociation", "Abort");

    m_pAssociation = __nullptr;

  } // END ... If there is an association


} // AbortAssociation()


/** 
 * Releases the association
 */
void DcmAssociation::Release()
{
  //==============================================
  // Only release if the association is present
  //==============================================
  if ( m_pAssociation != __nullptr )
  {

    DcmLog::Information(m_sLogSource, "Releasing the association", "DcmAssociation", "ReleaseAssociation");

    OFCondition cond = ASC_releaseAssociation(m_pAssociation);
    if (cond.bad())
      DcmLog::Error(m_sLogSource, "Failed to release association : " + std::string(cond.text()), "DcmAssociation", "ReleaseAsociation");
 
  } // END ... If the association is present

} // ReleaseAssociation()



/**
 * Process the C-ECHO command
 *
 * @param pMessage                [in]  Pointer to the command message
 * @param presentationContextId   [in]  Presentation context id to send response on
 *
 * @return  condition of response.
 */
OFCondition DcmAssociation::ProcessEchoCommand( T_DIMSE_Message * pMessage , T_ASC_PresentationContextID presentationContextId)
{
  /// Set the command type to Echo
  m_eCommandType = C_ECHO;

  DcmLog::Debug(m_sLogSource, "Processing echo command", "DcmAssociation", "ProcessEchoCommand");

  //============================
  // Send an ECHO response 
  //============================
  OFCondition cond = DIMSE_sendEchoResponse(m_pAssociation, presentationContextId, &pMessage->msg.CEchoRQ, STATUS_Success, __nullptr);

  if ( cond.bad() )
    DcmLog::Warning(m_sLogSource, "Echo response failed : " + std::string(cond.text()), "DcmAssociation", "ProcessEchoCommand");

  // Return the response condition
  return cond;

} // ProcessEchoCommand( System::IntPtr messagePtr, T_ASC_PresentationContextID presentationContextId)


/*
 * This function processes a DIMSE C-FIND-RQ command that was received over the network connection.
 *
 * @param  pMessage                [in]  The DIMSE C-STORE-RQ message that was received.
 * @param  presentationContextId   [in]  The ID of the presentation context which was specified in the PDV which contained
 *                                       the DIMSE command.
 *
 * @return condition of the response
 */
OFCondition DcmAssociation::ProcessFindCommand( T_DIMSE_Message * pMessage , T_ASC_PresentationContextID presentationContextId )
{

  /// Set the command type to Find
  m_eCommandType = C_FIND;

  DcmLog::Debug(m_sLogSource, "Processing find command", "DcmAssociation", "ProcessFindCommand");

  // Set the condition to Normal (No error )
  OFCondition cond = EC_Normal;

  // Assign the actual information of the C-STORE-RQ command to a local variable
  T_DIMSE_C_FindRQ * pFindRequest( &pMessage->msg.CFindRQ );


  m_callbackParameters.Connection.CallingAETitle = m_sCallingAETitle.c_str();
  m_callbackParameters.Connection.CalledAETitle  = m_sCalledAETitle.c_str();
  m_callbackParameters.CommandType = C_FIND;
  m_callbackParameters.Command.Find.Completed = 0;
  m_callbackParameters.Command.Find.Image = __nullptr;  // Used in the client (SCU)


  DcmLog::Debug(m_sLogSource, "Invoking the find provider to receive the dicom data", "DcmAssociation", "ProcessStoreCommand");

  // Invoke the store operation to receive the dicom images
  cond = DIMSE_findProvider(m_pAssociation, presentationContextId, pFindRequest, &dcm::DcmAssociation::FindCommandCallback, this, DIMSE_BLOCKING, m_iNetworkTimeout);

  if ( cond.bad() )
    DcmLog::Warning(m_sLogSource, "Find response failed : " + std::string(cond.text()), "DcmAssociation", "ProcessEchoCommand");

  // Return the response condition
  return cond;

} // ProcessFindCommand( System::IntPtr messagePtr, T_ASC_PresentationContextID presentationContextId )


/*
 * This function processes a DIMSE C-GET-RQ command that was received over the network connection.
 *
 * @param  pMessage                [in]  The DIMSE C-STORE-RQ message that was received.
 * @param  presentationContextId   [in]  The ID of the presentation context which was specified in the PDV which contained
 *                                       the DIMSE command.
 *
 * @return condition of the response
 */
OFCondition DcmAssociation::ProcessGetCommand( T_DIMSE_Message * pMessage , T_ASC_PresentationContextID presentationContextId )
{

  /// Set the command type to GET
  m_eCommandType = C_GET;

  DcmLog::Debug(m_sLogSource, "Processing get command", "DcmAssociation", "ProcessMoveCommand");


  throw DcmUnsupportedException(LogRec( "C-GET is unsupported", "DcmAssociation", "ProcessGetCommand"));

  // Set the condition to Normal (No error )
  OFCondition cond = EC_Normal;

  if ( cond.bad() )
    DcmLog::Warning(m_sLogSource, "Echo response failed : " + std::string(cond.text()), "DcmAssociation", "ProcessEchoCommand");

  // Return the response condition
  return cond;

} // ProcessGetCommand( System::IntPtr messagePtr, T_ASC_PresentationContextID presentationContextId )


/*
 * This function processes a DIMSE C-STORE-RQ command that was received over the network connection.
 *
 * @param  pMessage                [in]  The DIMSE C-STORE-RQ message that was received.
 * @param  presentationContextId   [in]  The ID of the presentation context which was specified in the PDV which contained
 *                                       the DIMSE command.
 *
 * @return condition of the response
 */
OFCondition DcmAssociation::ProcessMoveCommand( T_DIMSE_Message * pMessage , T_ASC_PresentationContextID presentationContextId )
{

  // Set the command type to a move
  m_eCommandType = C_MOVE;

  DcmLog::Debug(m_sLogSource, "Processing move command", "DcmAssociation", "ProcessMoveCommand");


   // Set the condition to Normal (No error )
  OFCondition cond = EC_Normal;

  // Store Rq
  T_DIMSE_C_MoveRQ * pMoveRequest;

  // Assign the actual information of the C-MOVE-RQ command to a local variable
  pMoveRequest = &pMessage->msg.CMoveRQ;


  m_callbackParameters.Connection.CallingAETitle = m_sCallingAETitle.c_str();
  m_callbackParameters.Connection.CalledAETitle  = m_sCalledAETitle.c_str();
  m_callbackParameters.CommandType = C_MOVE;
  m_callbackParameters.Command.Move.Completed = 0;
  m_callbackParameters.Command.Move.Remaining = 0;
  m_callbackParameters.Command.Move.Warning = 0;
  m_callbackParameters.Command.Move.Failed = 0;
  m_callbackParameters.Command.Move.Directories = __nullptr;


  DcmLog::Debug(m_sLogSource, "Invoking the move provider to receive the dicom data", "DcmAssociation", "ProcessMoveCommand");


  // Invoke the move operation to receive the dicom images
  cond = DIMSE_moveProvider(m_pAssociation, presentationContextId, pMoveRequest, &dcm::DcmAssociation::MoveCommandCallback, this, DIMSE_BLOCKING, m_iNetworkTimeout);
 
  // return return value
  return cond;

} // ProcessMoveCommand( System::IntPtr messagePtr, T_ASC_PresentationContextID presentationContextId )


/**
 * This function processes a DIMSE C-STORE-RQ command that was received over the network connection.
 *
 * @param  pMessage                [in]  The DIMSE C-STORE-RQ message that was received.
 * @param  presentationContextId   [in]  The ID of the presentation context which was specified in the PDV which contained
 *                                       the DIMSE command.
 *
 * @return condition of the response
 */
OFCondition DcmAssociation::ProcessStoreCommand( T_DIMSE_Message * pMessage , T_ASC_PresentationContextID presentationContextId )
{

  // Set the command type to a store
  m_eCommandType = C_STORE;

  DcmLog::Debug(m_sLogSource, "Processing store command", "DcmAssociation", "ProcessStoreCommand");

  // Set the condition to Normal (No error )
  OFCondition cond = EC_Normal;

  // Assign the actual information of the C-STORE-RQ command to a local variable
  T_DIMSE_C_StoreRQ * pStoreRequest( &pMessage->msg.CStoreRQ );

  /// DcmImage used for data receiving (create the image as empty)
  DcmImage dcmImage = DcmImage(true);


  m_callbackParameters.Connection.CallingAETitle  = m_sCallingAETitle.c_str();
  m_callbackParameters.Connection.CalledAETitle   = m_sCalledAETitle.c_str();
  m_callbackParameters.CommandType                = C_STORE;
  m_callbackParameters.Command.Store.Image        = &dcmImage;
  m_callbackParameters.Command.Store.Completed    = 0;

 
  // define an address where the information which will be received over the network will be stored
  DcmDataset * pDataset =  dcmImage.GetDcmDataset();

  // True if writing to file, False if writing to dataset
  bool useMetaHeader = OFTrue;

  DcmLog::Debug(m_sLogSource, "Invoking the store provider to receive the dicom data", "DcmAssociation", "ProcessStoreCommand");

  // Invoke the store operation to receive the dicom images
  cond = DIMSE_storeProvider(m_pAssociation, presentationContextId, pStoreRequest, __nullptr, useMetaHeader, &pDataset,
                             &dcm::DcmAssociation::StoreCommandCallback, this, DIMSE_BLOCKING, m_iNetworkTimeout);

  /// Clear the image
  m_callbackParameters.Command.Store.Image = __nullptr;
 
  // return return value
  return cond;

} // ProcessStoreCommand( System::IntPtr messagePtr, T_ASC_PresentationContextID presentationContextId )


#pragma region Command Callback Methods


/*
 * This function.is used to indicate progress when store receives instance data over the
 * network. On the final call to this function (identified by progress->state == DIMSE_StoreEnd)
 * this function will store the data set which was received over the network to a file.
 *
 * @param   pCallbackData     [in]      data for this callback function
 * @param   pProgress         [in]      The state of progress. (identifies if this is the initial or final call
 *                                      to this function, or a call in between these two calls.
 * @param   pRequest          [in]      The original store request message.
 * @param   pImageFileName    [in]      The path to and name of the file the information shall be written to. (Not Used)
 * @param   ppDataSet         [in]      The data set which shall be stored in the image file
 * @param   pResponse         [inout]   the C-STORE-RSP message (will be sent after the call to this function)
 * @param   ppStatusDetail    [inout]   This variable can be used to capture detailed information with regard to
 *                                      the status information which is captured in the status element (0000,0900). Note
 *                                      that this function does specify any such information, the pointer will be set to __nullptr.
 */
void  DcmAssociation::StoreCommandCallback(   void * pCallbackData, T_DIMSE_StoreProgress * pProgress, T_DIMSE_C_StoreRQ * pRequest,  char * /*imageFileName*/, 
                                              DcmDataset ** ppDataset,  T_DIMSE_C_StoreRSP * pResponse,  DcmDataset ** ppStatusDetail )
 {
   
  OFCondition cond;
  DIC_UI sopClass;
  DIC_UI sopInstance;

   // remember callback data
  DcmAssociation * pAssociation = reinterpret_cast<DcmAssociation *>(pCallbackData);
 
  // ======================================================================================================
  // Log information to determine data begin received
  //
  // Uncomment out the block below if you need to see if you are receiving data. We only care about the 
  // StoreEnd since that tells us that we have received a complete image. Un-commenting will only increase 
  // your log file so I will leave these commented out.
  // =======================================================================================================
  //switch (pProgress->state)
  //{
  //  case DIMSE_StoreBegin:
  //    DcmLog::Debug(pAssociation->GetLogSource(), "Receiving storage begin", "DcmAssociation", "StoreCommandCallback");
  //    break;
  //  case DIMSE_StoreEnd:
  //    DcmLog::Debug(pAssociation->GetLogSource(), "Receiving storage end", "DcmAssociation", "StoreCommandCallback");
  //    break;
  //  default:
  //    DcmLog::Debug(pAssociation->GetLogSource(), "Receiving storage data", "DcmAssociation", "StoreCommandCallback");
  //    break;
  //} // END ... Switch
  
  //==============================================================================================
  // if this is the final call of this function, save the data which was received to a file
  // (note that we could also save the image somewhere else, put it in database, etc.)
  //=============================================================================================
  if (pProgress->state == DIMSE_StoreEnd)
  {


    // ======================================
    // Check if the dataset contains data
    // =====================================
    if ( ppDataset != __nullptr && *ppDataset != __nullptr )
    {
      //=========================================================================================================
      // check the image to make sure it is consistent, i.e. that its sopClass and sopInstance correspond
      // to those mentioned in the request. If not, set the status in the response message variable.
      //========================================================================================================
      if ( pResponse->DimseStatus == STATUS_Success )
      {

        // which SOP class and SOP instance ?
        if (! DU_findSOPClassAndInstanceInDataSet(*ppDataset, sopClass, sopInstance, OFTrue) )
        {
           DcmLog::Error(pAssociation->GetLogSource(), "Bad dicom image received", "DcmAssociation", "StoreCommandCallback");
           pResponse->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
        }
        else if (strcmp(sopClass, pRequest->AffectedSOPClassUID) != 0)
        {
          DcmLog::Error(pAssociation->GetLogSource(), "Dataset does not match SOP class", "DcmAssociation", "StoreCommandCallback");
          pResponse->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
        }
        else if (strcmp(sopInstance, pRequest->AffectedSOPInstanceUID) != 0)
        {
          DcmLog::Error(pAssociation->GetLogSource(), "Dataset does not match SOP class", "DcmAssociation", "StoreCommandCallback");
          pResponse->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
        }
      
      } // END ... If the status indicates success check for additional errors 

    } // END ... If a dataset was received
  
    /// Increment the completed number for the store
    pAssociation->m_callbackParameters.Command.Store.Completed++;
    pAssociation->m_callbackParameters.Status = pResponse->DimseStatus;

    if ( pResponse->DimseStatus == STATUS_Success )
      pAssociation->Callback(E_PROGRESS);
    else
      pAssociation->Callback(E_FAILED);

  } // END ... If we are at the end of the storage cycle of an imaged

  // return
  return;

} // StoreCommandCallback(   void * pCallbackData, T_DIMSE_StoreProgress * pProgress, T_DIMSE_C_StoreRQ * pStoreRequest,  char * /*imageFileName*/, DcmDataset ** ppDataSet,  T_DIMSE_C_StoreRSP * pStoreResponse,  DcmDataset ** ppStatusDetail )


/*
 * This function.is used to indicate C-FIND progress.
 *
 * @param   pCallbackData         [in]      data for this callback function
 * @param   bCancelled            [in]      Cancelled flag
 * @param   pFindRequest          [in]      The original find request message.
 * @param   pRequestIdentifiers   [in]      Request identifiers
 * @param   iResponseCount        [in]      Response cunt
 * @param   pFindResponse         [out]     Find response
 * @param   pResponseIdentifiers  [out]     Response identifiers
 * @param   ppStatusDetail        [out]     This variable can be used to capture detailed information with regard to
 *                                          the status information which is captured in the status element (0000,0900). Note
 *                                          that this function does specify any such information, the pointer will be set to __nullptr.
 */
void  DcmAssociation::FindCommandCallback(  void * pCallbackData, OFBool bCancelled, T_DIMSE_C_FindRQ * pFindRequest, DcmDataset * pRequestIdentifiers, 
                                            int iResponseCount, T_DIMSE_C_FindRSP * pFindResponse, DcmDataset ** pResponseIdentifiers, DcmDataset ** ppStatusDetail)
 {
   
  // Xml Document Ptr
  xmlDocPtr pDocument(__nullptr);

  // Condition
  OFCondition cond(EC_Normal);

  // remember callback data
  DcmAssociation * pAssociation(reinterpret_cast<DcmAssociation *>(pCallbackData));
 
  // Find requsets
  std::shared_ptr<DcmImage> pRequest(DcmImageFactory::Create(pRequestIdentifiers));
 
  // Create the find paramaters from the find request
  std::shared_ptr<DcmFindParameters> pDcmFindParameters(DcmFindParameters::Create(pRequest));

  /// Set up the callback parameters for C-FIND
  pAssociation->m_callbackParameters.Command.Find.Completed++;                                // Completed (Increment)
  pAssociation->m_callbackParameters.Status                   = 0;                            // Status (Default 0)
  pAssociation->m_callbackParameters.Command.Find.Image       = pRequest.get();               // Pointer to DcmImage (Request)
  pAssociation->m_callbackParameters.Command.Find.Parameters  = pDcmFindParameters.get();     // Find Parameters
  pAssociation->m_callbackParameters.Command.Find.Results     = __nullptr;                    // Set the result to NULL


  try
  {

    // ========================================================
    // Do intial setup for the first callback
    // ========================================================
    if ( iResponseCount == 1 )
    {

      /// Get the DcmImage fromt the request
      DcmImage image(pRequestIdentifiers);

      // =======================================================
      // Cannot process a PATIENT level with a STUDY level UID
      // ======================================================
      if ( image.GetQueryRetrievalLevel() == "PATIENT" && strncmp(UID_FINDStudyRootQueryRetrieveInformationModel,pFindRequest->AffectedSOPClassUID,sizeof(DIC_UI)) == 0 )
      {
        
        DcmLog::Error(pAssociation->GetLogSource(), 
              util::Strings::Format("Cannot process C-FIND with PATIENT/UID_FINDStudyRootQueryRetrieveInformationModel pair (SOURCE:%s)",pAssociation->GetCallingAETitle().c_str()), 
                  "DcmAssociation", "FindCommandCallback" );

        pFindResponse->DimseStatus = STATUS_FIND_Failed_UnableToProcess;
        pFindResponse->DataSetType = DIMSE_DATASET_NULL;
        return;

      } // END ... If attempt is made to QUERY at the PATIENT level and a study level uid

      /// Clear list
      pAssociation->m_queryResponse.clear();

      // Perform the progress callback
      pAssociation->Callback(E_PROGRESS);

      // ================================================
      // If the results are not NULL them parse the xml
      // ================================================
      if ( pAssociation->m_callbackParameters.Command.Find.Results != __nullptr )
      {
  
        xml::XmlDocument document;
        document.LoadXml(pAssociation->m_callbackParameters.Command.Find.Results);
   
        /// Gets the document element
        xml::XmlNode docNode(document.GetDocumentElement());


        std::string nodeName(toupper(pDcmFindParameters->GetLevel()));
        //std::string nodeName = pDcmFindParameters->GetLevel();
        //std::transform(nodeName.begin(), nodeName.end(), nodeName.begin(), ::tolower );

        std::vector<xml::XmlNode> nodes = docNode.SelectNodes(nodeName);
      
        // ===========================
        // If there are response nodes
        // ===========================
        if ( ! nodes.empty() )
        {

          // =====================================================
          // For each node/record returned build a response list
          // =====================================================
          for ( std::vector<xml::XmlNode>::iterator it = nodes.begin(); it != nodes.end(); it++ ) 
          {
            std::shared_ptr<DcmImage> pDcmImage(DcmImageFactory::Create(*it));  
            pDcmImage->SetRetrievalAEtitle(pAssociation->GetCalledAETitle());
            pDcmImage->SetQueryRetrievalLevel(pDcmFindParameters->GetLevel());
            pAssociation->m_queryResponse.push_back(pDcmImage);
          } // END ... For each node returned

        } // END ... If there are nodes

        // Delete the responses results which were allocated in the callback
        delete pAssociation->m_callbackParameters.Command.Find.Results;
        pAssociation->m_callbackParameters.Command.Find.Results = __nullptr;


      } // END ... If we do not already have query responses
    }

    if ( pAssociation->m_queryResponse.empty() )
    {
      pFindResponse->DimseStatus = STATUS_Success;
      pFindResponse->DataSetType = DIMSE_DATASET_NULL;
      strncpy(pFindResponse->AffectedSOPClassUID, pFindRequest->AffectedSOPClassUID, sizeof(DIC_UI));
      pFindResponse->MessageIDBeingRespondedTo = pFindRequest->MessageID;
      *pResponseIdentifiers = __nullptr;
    }
    else
    {
      std::shared_ptr<DcmImage> pDcmImage(pAssociation->m_queryResponse.back());
      pAssociation->m_queryResponse.pop_back();

      pFindResponse->DimseStatus = STATUS_Pending;
      pFindResponse->DataSetType = DIMSE_DATASET_PRESENT;
      *pResponseIdentifiers = (DcmDataset *)pDcmImage->m_pDcmDataset->clone();
 
    } // END ... If the query response is empty

  } // try
  catch ( DcmException & ex )
  {

    DcmLog::Error(pAssociation->m_sLogSource, ex.GetLogRecord());

    pFindResponse->DimseStatus = STATUS_FIND_Failed_UnableToProcess;
    pFindResponse->DataSetType = DIMSE_DATASET_NULL;
    strncpy(pFindResponse->AffectedSOPClassUID, pFindRequest->AffectedSOPClassUID, sizeof(DIC_UI));
    pFindResponse->MessageIDBeingRespondedTo = pFindRequest->MessageID;

  } // catch ( DcmException & ex )
  catch ( ... )
  {

     DcmLog::Error(pAssociation->m_sLogSource, "Failed to extract query results", "DcmAssociation","FindCommandCallback");
     
     pFindResponse->DimseStatus = STATUS_FIND_Failed_UnableToProcess;
     pFindResponse->DataSetType = DIMSE_DATASET_NULL;
     strncpy(pFindResponse->AffectedSOPClassUID, pFindRequest->AffectedSOPClassUID, sizeof(DIC_UI));
     pFindResponse->MessageIDBeingRespondedTo = pFindRequest->MessageID;

  } // catch ( .... )

  // ====================================================================
  // Delete the responses results which were allocated in the callback
  // ====================================================================
  if ( pAssociation->m_callbackParameters.Command.Find.Results != __nullptr )
  {
    delete pAssociation->m_callbackParameters.Command.Find.Results;
    pAssociation->m_callbackParameters.Command.Find.Results = __nullptr;
  } // END ... If the results have not been cleaned up yet

  // Free xml document if it is not NULL
  if ( pDocument != __nullptr )
    xmlFreeDoc(pDocument);

  // return
  return;

} // FindCommandCallback( )



/**
 * This function.is used to indicate progress when storescp receives instance data over the
 * network. On the final call to this function (identified by progress->state == DIMSE_StoreEnd)
 * this function will store the data set which was received over the network to a file.
 * Earlier calls to this function will simply cause some information to be dumped to stdout.
 *
 * @param   pCallbackData         [in]      data for this callback function
 * @param   bCancelled            [in]      The state of progress. (identifies if this is the initial or final call
 *                                          to this function, or a call in between these two calls.
 * @param   pMoveRequest          [in]      The original move request message.
 * @param   pRequestIdentifiers   [in]      The request identifiers.
 * @param   iResponseCount        [in]      Response count
 * @param   pMoveResponse         [out]     The C-MOVE-RSP response
 * @param   ppStatusDetail        [inout]   This variable can be used to capture detailed information with regard to
 *                                          the status information which is captured in the status element (0000,0900). Note
 *                                          that this function does specify any such information, the pointer will be set to __nullptr.
 * @param   pResponseIdentifiers  [inout]  
 *
 */
void  DcmAssociation::MoveCommandCallback( void * pCallbackData, OFBool bCancelled, T_DIMSE_C_MoveRQ *pMoveRequest, DcmDataset * pRequestIdentifiers, 
                                            int iResponseCount, T_DIMSE_C_MoveRSP * pMoveResponse, DcmDataset **ppStatusDetail, DcmDataset ** pResponseIdentifiers)
 {

  OFCondition cond(EC_Normal);

  // remember callback data
  DcmAssociation * pAssociation(reinterpret_cast<DcmAssociation *>(pCallbackData));

  // Move requsets
  std::shared_ptr<DcmImage> pRequest(DcmImageFactory::Create(pRequestIdentifiers));
 
  // Create the find paramaters from the find request
  std::shared_ptr<DcmMoveParameters> pDcmMoveParameters(DcmMoveParameters::Create(pRequest));

  /// Set up the parameters for FIND
  pAssociation->m_callbackParameters.Status                   = 0;                            // Status (Default 0)
  pAssociation->m_callbackParameters.Command.Move.Parameters  = pDcmMoveParameters.get();     // Find Parameters

  // No reponse dataset
  *pResponseIdentifiers                         = __nullptr;

  // Set the state to pending and the dataset type to NULL
  pMoveResponse->DimseStatus                    = STATUS_Pending;
  pMoveResponse->DataSetType                    = DIMSE_DATASET_NULL;
  pMoveResponse->MessageIDBeingRespondedTo      = pMoveRequest->MessageID;
  strncpy(pMoveResponse->AffectedSOPClassUID, pMoveRequest->AffectedSOPClassUID, sizeof(DIC_UI));

  try
  {

    // =======================================================================
    // If this is the first time then we need to initialize the store and all
    // parameters settings.
    // =======================================================================
    if ( iResponseCount == 1 )
    {

      /// Initialize response counters
      pMoveResponse->NumberOfRemainingSubOperations = 0;
      pMoveResponse->NumberOfFailedSubOperations    = 0;
      pMoveResponse->NumberOfWarningSubOperations   = 0;
      pMoveResponse->NumberOfCompletedSubOperations = 0;

      /// Initialize callback parameters
      pAssociation->m_callbackParameters.Command.Move.Completed = 0;
      pAssociation->m_callbackParameters.Command.Move.Remaining = 0;

      // Host (needed for the TRyGetHostByAETitle)
      std::shared_ptr<DcmHost> pHost(__nullptr);
  
      // ==============================================================================================================
      // Attempt to get the host from the table. If the host was not found then return destination unknown error
      // back to the client. If the host does not support STORE commands then also send destination unknown error
      // ==============================================================================================================
      if ( ! DcmHostTable::GetCurrentHostTable()->TryGetHostByAETitle(pMoveRequest->MoveDestination, pHost) )
      {
        DcmLog::Error(pAssociation->GetLogSource(), 
              util::Strings::Format("Cannot process C-MOVE cannot find destination %s pair (SOURCE:%s)",pMoveRequest->MoveDestination, pAssociation->GetCallingAETitle().c_str()), 
                  "DcmAssociation", "MoveCommandCallback" );

        pMoveResponse->DimseStatus = STATUS_MOVE_Failed_MoveDestinationUnknown;
        pMoveResponse->DataSetType = DIMSE_DATASET_NULL;
        return;
      } // END ... If the host was not found
      else if (! pHost->Supports(ST_STORE) )
      {
        DcmLog::Error(pAssociation->GetLogSource(), 
              util::Strings::Format("Cannot process C-MOVE destination %s does not support STORE (SOURCE:%s)",pMoveRequest->MoveDestination, pAssociation->GetCallingAETitle().c_str()), 
                  "DcmAssociation", "MoveCommandCallback" );

        pMoveResponse->DimseStatus = STATUS_MOVE_Failed_MoveDestinationUnknown;
        pMoveResponse->DataSetType = DIMSE_DATASET_NULL;
        return;
      } // END ... Else if the host does not support the C-STORE

      // Perform the progress callback
      pAssociation->Callback(E_PROGRESS);


      // ======================================================================
      // If there are directories to be pushed then parse them out of the XML
      // ======================================================================
      if ( pAssociation->m_callbackParameters.Command.Move.Directories != __nullptr )
      {

        // Create the store (Only necessary of there were directories foudn for the query)
        pAssociation->m_store = DcmStore::Create(pAssociation->m_sCalledAETitle, pHost );

        xml::XmlDocument doc;

        doc.LoadXml(pAssociation->m_callbackParameters.Command.Move.Directories);
    
 
        // Get all the 'dir' nodes
        std::vector<xml::XmlNode> dirs = doc.GetDocumentElement().SelectNodes( "dir");
      
        // ================================================================
        // Loop through all the directories and add then to the store list
        // ================================================================
        for ( std::vector<xml::XmlNode>::iterator it = dirs.begin(); it != dirs.end(); it++ )
        {

          std::string dir("");

          // ==============================================================
          // Try and get the value attribute which contains the directory
          // ==============================================================
          if ( (*it).TryGetAttributeValue<std::string>( "value", dir) )
            pAssociation->m_store->AddDirectory(dir);
          
        } // END ... For each dir node

        /// Since the directories was allocated in the callback we delete it here, but only of it was not NULL 
        delete pAssociation->m_callbackParameters.Command.Move.Directories;
        pAssociation->m_callbackParameters.Command.Move.Directories = __nullptr;
       
      } // END ... If there are directories
      else
      {
        pMoveResponse->DimseStatus = STATUS_Success;
        pMoveResponse->DataSetType = DIMSE_DATASET_NULL;
        return;
      }

    } // END ... If this is the first time


    // ================================================
    // If there are still files to send then send on
    // ================================================
    if ( pAssociation->m_store->GetImageCount() > 0 )
    {

      /// Sets the completed/remaining callback poroperties
      pAssociation->m_callbackParameters.Command.Move.Completed++;                               
      pAssociation->m_callbackParameters.Command.Move.Remaining--;

      
      /// Send the next image on the list
      pAssociation->m_store->Send();
 
      // Perform the progress callback
      pAssociation->Callback(E_PROGRESS);

      // Set the response counters
      pMoveResponse->NumberOfRemainingSubOperations = pAssociation->m_store->GetImageCount();
      pMoveResponse->NumberOfCompletedSubOperations = pAssociation->m_callbackParameters.Command.Move.Completed;

      pMoveResponse->DimseStatus = STATUS_Pending;
      pMoveResponse->DataSetType = DIMSE_DATASET_NULL;

    } // END ... If there are still files to send
    else
    {
      // Clears the store
      pAssociation->m_store.reset();
      pAssociation->m_store = __nullptr;
      pMoveResponse->DimseStatus = STATUS_Success;
      pMoveResponse->DataSetType = DIMSE_DATASET_NULL;
    } // END ... No more files then we are done

  }
  catch ( DcmException & ex )
  {

    // ===========================================================================================================
    // Make sure the repsonse to the MOVE is cleaned up since the memory was allocated in the calback and returned
    // ===========================================================================================================
    if (  pAssociation->m_callbackParameters.Command.Move.Directories != __nullptr )
    {
      delete pAssociation->m_callbackParameters.Command.Move.Directories;
      pAssociation->m_callbackParameters.Command.Move.Directories = __nullptr;
    } // END .. If the memory has not been cleaned up yet

    // Resets the store
    pAssociation->m_store.reset();

    /// Deletes the internal memory
    DcmLog::Error(util::Strings::Format("Failed to store image for C-MOVE : %s", ex.GetTextMessage().c_str()), "DcmAssociation", "MoveCommandCallback");
    pMoveResponse->DimseStatus = STATUS_MOVE_Failed_UnableToProcess;
    pMoveResponse->DataSetType = DIMSE_DATASET_NULL;

    return;

  }
  
} // MoveCommandCallback()


#pragma endregion