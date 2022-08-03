// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLoggerServer.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

#include "vxLoggerClassic.h"

#pragma warning(push)
#pragma warning(disable:4251)

class VX_LOGGER_DLL TaskGroup;

/**
 * Logger Class declaration.
 */
class VX_LOGGER_DLL vxLoggerServer : public vxLoggerClassic
{

// member functions
protected:
  ///
  vxLoggerServer(const vxLoggerInfo & loggerInfo );

public:

  ///Default Destructor
  virtual ~vxLoggerServer();

  /// Creates the logger
  static std::shared_ptr<vxLogger>  Create( const vxLoggerInfo & loggerInfo );

 
  ///Static Message method  which logs the Record
  virtual void Trace( const vxLogRecord & record );

  /// Gets the port for the server
  const unsigned int GetPort() const
  { return m_iPort; }

  /// Gets the server
  const std::string & GetServer() const
  { return m_sServer; }
  
protected:

  /// Releases the resources
  virtual void Release();

  /// Initialize
  virtual void Initialize();

private:

  /// Attempts to connect to the server and return the SOCKET
  SOCKET Connect();

  /// Disconnects the specified socket
  void Disconnect(SOCKET & socketHandle);

  /// Traces the fie the classic way
  void TraceClassic(const vxLogRecord & record );

// member variables
private:

  /// Socket data
  WSADATA m_wsaData;


 ///Last back up day
  mutable SYSTEMTIME m_lastBackupDay;


  /// Winsock initialize flag
  bool m_bInitialized;

    /// Task group
  std::shared_ptr<TaskGroup> m_tasks;

};  // class vxLoggerServer



#pragma warning(pop)