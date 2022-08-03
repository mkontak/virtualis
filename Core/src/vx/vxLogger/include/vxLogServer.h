// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogServer.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once 


// includes
#include "vxLogThread.h"



// pragmas
#pragma warning(push)
#pragma warning(disable:4251)


// forward declarations 
class VX_LOGGER_DLL TaskGroup;
class VX_LOGGER_DLL Task;


/**
 * class implements a log server. This server will accept log messages a route them to appropriate local logger. Works the same a 
 * the local loggers, but remotely.
 */
class VX_LOGGER_DLL vxLogServer 
{


public:

  // Constructor
  vxLogServer(const unsigned int iPort = 0);

  // Destructor
  virtual ~vxLogServer();

  static std::shared_ptr<vxLogServer> Create(const unsigned int iPort = 0);

  /// Stops the server
  void Stop()
  { Abort(); }

  /// Starts the server
  void Start();

  /// Abort the server
  void Abort();
  
private:

  // Runs the server
  void RunSynchrounously();

   /// Message handler
  void MessageHandler(SOCKET socket);

public:

  /// Get the port the server is listening on
  const unsigned int GetPort() const
  { return m_uPort; }


#pragma region fields

private:

  /// Port to listen on
  unsigned int m_uPort;   

  /// Socket Handle
  SOCKET m_socketHandle;

  /// Thread sHandle
  HANDLE m_hThread;

  /// Task group
  std::shared_ptr<TaskGroup> m_tasks;

  /// Task
  std::shared_ptr<Task> m_task;


  /// Started flag
  bool m_bStarted;

  /// Abort requested flag
  bool m_bAbortRequested;

  

#pragma endregion

};  // class vxLogServer

#pragma warning(pop)