// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmAssociation.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once





#include "DcmEnums.h"
#include "DcmThread.h"


#pragma warning(push)
#pragma warning(disable:4251)


namespace dcm
{

/// Foreard declarations
class DCM_DLL DcmStore;

/**
 * Implements an abstract dicom association that will handle the communications and the command operation requested from the SCU.
 * Implemented as a thread object. 
 *
 * Command Supported:
 *
 *    C-STORE     Receives images from another dicom device.
 *    C-FIND      Receives query from another dicom device and sned the informtaion through the callback to get 
 *                information from a top layer. Since no data/informtaion is maintained in this library the call
 *                is passed up to be handled.
 *    C-MOVE      Allows the data that is stored from a upper layer to be sent to another dicom device specified in 
 *                command.
 *    C-GET       Not implemented
 *    C-ECHO      Sends a response back indicating we are up (works like a network ping)
 *              
 */
class DCM_DLL DcmAssociation :  public DcmThread
{

#pragma region contsrutors/destructors

private:

  // Default Constructor
  DcmAssociation( );


public:

  /// Destructor
  ~DcmAssociation();


#pragma endregion



#pragma region methods

public:

  /// Aborts the thread and the association
  virtual void Abort();

  /// Creates the association
  static std::shared_ptr<DcmAssociation> Create( T_ASC_Association * pAssociation,  int iNetworkTimeout, const std::string & sCallingAETitle, 
                                                 const std::string & sCalledAETitle, const std::string & sLogSource = vxLogFactory::GetDefaultLogger() );


private:

  /// Executes the association command processing
  void RunSynchronously();

  /// Release the association
  void Release();

  /// Aborts the association
  void AbortAssociation();

#pragma region Command Processing Methods

  /// Process the store command
  OFCondition ProcessStoreCommand(  T_DIMSE_Message * pMessage, T_ASC_PresentationContextID presentationContextId );
  
  /// Process the echo command
  OFCondition ProcessEchoCommand(   T_DIMSE_Message * pMessage, T_ASC_PresentationContextID presentationContextId );

  /// Process the Move command
  OFCondition ProcessMoveCommand(   T_DIMSE_Message * pMessage, T_ASC_PresentationContextID presentationContextId );

  /// Process the Find command
  OFCondition ProcessFindCommand(   T_DIMSE_Message * pMessage, T_ASC_PresentationContextID presentationContextId );

  /// Process the Get command
  OFCondition ProcessGetCommand(    T_DIMSE_Message * pMessage, T_ASC_PresentationContextID presentationContextId );

#pragma endregion


#pragma region Command Callbacks

  /// Store command callback used when calling the store command method in the library
  static void  StoreCommandCallback(void * pCallbackData, T_DIMSE_StoreProgress * pProgress, T_DIMSE_C_StoreRQ * pStoreRequest, char * notUsed,  DcmDataset ** ppDataSet, T_DIMSE_C_StoreRSP * pStoreResponse, DcmDataset ** ppStatusDetail );

  /// Move command callback used when calling the move command method in the library
  static void  MoveCommandCallback(void * pCallbackData, OFBool bCancelled, T_DIMSE_C_MoveRQ * pMoveRequest, DcmDataset * pRequestIdentifiers, int responseCount,  T_DIMSE_C_MoveRSP * pMoveResponse, DcmDataset ** ppStatusDetail, DcmDataset ** pResponseIdentifiers);

  /// Find command callback used when calling the move command method in the library
  static void  FindCommandCallback(void * pCallbackData, OFBool bCancelled, T_DIMSE_C_FindRQ * pFindRequest, DcmDataset * pRequestIdentifiers, int responseCount, T_DIMSE_C_FindRSP * pFindResponse, DcmDataset ** pResponseIdentifiers, DcmDataset ** ppStatusDetail);

#pragma endregion



#pragma endregion




#pragma region properties

public:

  /// Gets the handle to be used for waiting
  const HANDLE GetHandle() const 
  { return m_hThread; }

  /// Gets the log source
  const std::string & GetLogSource() const
  { return m_sLogSource; }

  /// Gets the called AE title from the association
  const std::string & GetCalledAETitle() const
  { return m_sCalledAETitle; }
  
  /// Gets the calling AE Title from the association
  const std::string & GetCallingAETitle() const 
  { return m_sCallingAETitle; }
  
  /// Gets the state
  const DcmStates GetState() const
  { return m_eState; }
   
  /// Gets the current command
  const DcmCommands GetCommandType() const
  { return m_eCommandType; }

#pragma endregion

#pragma region fields

private:
 
  /// Log source
  std::string m_sLogSource;

  /// Association
  T_ASC_Association * m_pAssociation;

  /// This is our AE Title
  std::string  m_sCallingAETitle;

  /// This is the AE title of the client
  std::string  m_sCalledAETitle;

  ///
  int m_iNetworkTimeout;


  /// Thread handle
  HANDLE m_hThread;

  /// State
  DcmStates m_eState;

  /// Command this association is working on
  DcmCommands m_eCommandType;

  /// List of images to be sent back
  std::vector<std::shared_ptr<DcmImage>> m_queryResponse;

  /// Store used in Move Command porcessing
  std::shared_ptr<DcmStore> m_store;

#pragma endregion

};  // class DcmAssociation


}; // namespace dcm


#pragma warning(pop)