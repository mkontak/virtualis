// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmGet.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

// include
#include "DcmClient.h"
#include "DcmThread.h"
#include "DcmEnums.h"

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)


namespace dcm
{



class DCM_DLL DcmImage;
class DCM_DLL DcmGetParameters;
class DCM_DLL DcmServer;
class DCM_DLL DcmMove;
class DCM_DLL DcmHost;

/**
 * Implements a class that wraps the STORE
 */
class DCM_DLL DcmGet : public DcmClient, public DcmThread
{
protected:

  /// Default constructor
  DcmGet(const std::string & sAETitle = "");

 
public:
  
 
  /// Destructor
  virtual ~DcmGet();

  /// Aborts the store command
  virtual void Abort();


  /// Creates the get client object
  static std::shared_ptr<DcmGet> Create(const std::string & sAETitleOrXml = "");

  /// Creates the get client object 
  static std::shared_ptr<DcmGet> Create(const std::string & sAETitle, std::shared_ptr<DcmHost> & pServer);


  /// Adds the parameters needed to perform the move
  void AddParameters(std::shared_ptr<DcmGetParameters> & parameters);

  /// Thread process
  virtual void RunSynchronously();

private:


  /// Relays the callback from the client
  virtual void CallbackRelay(void * pResponse, DcmDataset * pResponseIds);

  /// Creates the server object
  void CreateServer(const std::string & sAETitle, int iPort);

  /// Handles the server callback
  static void ServerCallback(PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData);

  /// Handles the move callback
  static void MoveCallback(PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData);

  //// Starts the server
  void StartServer(const std::string & sAETitle, int iPort);



protected:

  /// Mover list
  std::vector<std::shared_ptr<DcmMove>> m_movers;

  /// Server (Needs to be only one server)
  std::shared_ptr<DcmServer> m_pDcmServer;

  /// Move parameters
  std::vector<std::shared_ptr<DcmGetParameters>> m_parameters;

  /// Server state based on the event received from the server
  DcmEvents m_serverState;

 
};  // class DcmGet
  
};  // namespace dcm

#pragma warning(pop)