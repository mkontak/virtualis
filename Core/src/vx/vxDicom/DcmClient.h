// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File:  DcmClient.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

// includes
#include "DcmDevice.h"
#include "DcmCallback.h"
#include "DcmEnums.h"

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)

namespace dcm
{

/// Forward declarations
class DCM_DLL DcmImage;
class DCM_DLL DcmMoveParameters;
class DCM_DLL DcmFindParameters;
class DCM_DLL DcmHost;

/**
 * Implements a DICOM SCU (client) to connect to and SCP (server).
 */
class DCM_DLL DcmClient : public DcmDevice
{

protected:

  DcmClient(const std::string & sAETitle = ""); 

public:

  /// Destructor
  virtual ~DcmClient();

  /// Creates the client from the xml
  static std::shared_ptr<DcmClient> Create(const std::string & sXml);

protected:

  /// Initializes the client using the xml supplied
  virtual void Initialize(const std::string & sXml);

  /// Sets the connection information
  void SetConnectionInformation(DcmConnectionInformation & info);

  /// Connects to the network
  void Connect();

  /// Disconnects from the SCP
  virtual void Disconnect();

  /// Aborts the association
  virtual void AbortAssociation();

  /// Releases the association
  virtual void ReleaseAssociation();

  /// Store a single image to the SCP we are connected to
  virtual void Store(const std::shared_ptr<DcmImage> & pDcmImage);

  /// Perform Move operation
  virtual void Move(const std::shared_ptr<DcmMoveParameters> & parameters);

  /// Perform Find operation
  virtual void Find(const std::shared_ptr<DcmFindParameters> & parameters);


private:

  
  /// Initializes the objects state
  void Initialize();

  /// Callback used in the C-STORE 
  static void StoreCallback( void * pCallbackData, T_DIMSE_StoreProgress * pProgress, T_DIMSE_C_StoreRQ * pRequest );


  /// Callback used in the C-MOVE
  static void MoveCallback( void * pCallbackData, T_DIMSE_C_MoveRQ * pRequest, int iResponseCount, T_DIMSE_C_MoveRSP * pResponse);


  /// Callback used in the C-FIND
  static void FindCallback( void * pCallbackData, T_DIMSE_C_FindRQ * pRequest, int iResponseCount, T_DIMSE_C_FindRSP * pResponse, DcmDataset * pResponseIds );

  /// Sub association callback
  static void SubOpCallback(void * pCallbackData, T_ASC_Network * pNetwork, T_ASC_Association ** ppSubAssociation);

  
  /// Main callback is called within the others
  virtual void CallbackRelay(void * pResponse, DcmDataset * pResponseIds) { }

  /// Write the last response
  void WriteLastResponse(T_DIMSE_C_StoreRSP response);

#pragma region properties

public:

  /// Gets the blocking mode
  const DcmBlockingMode & GetBlockingMode() const
  { return m_blockingMode; }

  /// Sets the blocking mode
  void SetBlockingMode( DcmBlockingMode mode )
  { m_blockingMode = mode; }


  /// Last response file used for debug purposes
  virtual const std::string & GetLastResponseFile() const
  {return m_sLastResponseFile; }


  /// Gets the server information
  std::shared_ptr<DcmHost> & GetServer()
  { return m_server; }

protected:

   /// Sets the server
  void SetServer(std::shared_ptr<DcmHost> & server);

#pragma endregion


#pragma region fields

protected:

  /// Last response file used for debugging
  std::string m_sLastResponseFile;

  /// Abort flag used in callback
  bool m_bAbort;


  /// Blocking Mode
  DcmBlockingMode   m_blockingMode;

private:

   /// Presentation context id being used
  T_ASC_PresentationContextID m_presentationContextId;

  /// Association
  T_ASC_Association * m_pAssociation;

  /// Network
  T_ASC_Network * m_pNetwork;

  /// Server information
  std::shared_ptr<DcmHost> m_server;


#pragma endregion

};  // class DcmClient

};  // namespace dcm

#pragma warning(pop)