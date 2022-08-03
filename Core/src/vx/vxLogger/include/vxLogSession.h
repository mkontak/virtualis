// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogSession.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once

#ifndef WINXP

// includes
#include "vxLogger.h"
#include "vxLogProvider.h"
#include "vxLogRecord.h"

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)

/**
 * Implements a session that defines how logging is done. You enable the provider to the session.
 */
class VX_LOGGER_DLL vxLogSession : public vxLogger
{

protected:

  /// Constructor
  vxLogSession(const vxLoggerInfo & loggerInfo );

public:

 
  /// Destructor
  virtual ~vxLogSession();

  /// Creates the session 
  std::shared_ptr<vxLogSession> Create(const vxLoggerInfo & loggerInfo);

  /// Overloaded == operator
  inline bool operator==(const vxLogSession & rhs)
  { return Equals(rhs); }

  /// Overloaded != operator
  inline bool operator!=(const vxLogSession & rhs)
  { return ! Equals(rhs); }

  /// Equals
  virtual inline bool Equals(const vxLogSession & session)
  { return ( const_cast<vxLogSession &>(session).m_sHandle == m_sHandle) ; }

  // Starts the session
  void Start();

  // Stops the session
  void Stop();

  /// Enables a single provider 
  void Enable(vxLogProvider * pProvider);

  /// Disables a single provider
  void Disable(vxLogProvider * pProvider);

  /// Flush the session buffers
  void Flush();

  // Restarts the session
  void Restart();



 
protected:

  /// Releases the session
  virtual void Release();

private:

  /// Does nothing
  virtual void Trace(const vxLogRecord & record)
  { }



  void Initialize();

  /// Disables all providers
  void Disable();

  /// Enables all providers
  void Enable();

  /// Inserts the provider into the list. This is called by the Enable so a list of providers is maintained.
  void Insert(vxLogProvider * pProvider);

  /// Releases Properties
  void ReleaseProperties();

public:

  /// Gets the GUID for this provider
  const GUID & GetGuid() 
  { return m_guid; }

  /// Gets the GUID as a string
  const std::string & GetGuidAsString()
  { return m_sGuid; }

  /// Gets the session name
  const std::string & GetName() 
  { return m_sHandle; }

  /// Sets the file name for the logging session (Allows changing during the execution)
  virtual void SetFilename(const std::string & sFilename);


  /// Sets the log level (Allows changing during execution)
  virtual void SetLogLevel(vxLogRecord::SeverityEnum eLogLevel);


  /// Gets the flush timer
  int GetFlushTimer()
  { return m_iFlushTimer; }

  /// Sets the flush timer (Will only take affect after the a restart)
  void SetFlushTimer(int iFlushTimer)
  { m_iFlushTimer; ReleaseProperties(); }
 
  /// True if the session is started
  bool IsStarted()
  { return ( m_hTraceHandle > 0 ); }

  /// True if the session is started
  bool IsInitialized()
  { return ( m_properties != __nullptr ); }

private:


  // Provider 
  vxLogProvider m_provider;

  /// Max string size (1024)
  static size_t m_maxString;

  /// Provider list
  std::map<std::string, vxLogProvider *> m_providers;

  /// Event tracing properties
  PEVENT_TRACE_PROPERTIES m_properties;

  /// Trace sHandle
  TRACEHANDLE m_hTraceHandle;

  /// Restarting flag
  bool m_bRestarting;

  /// Started flag
  bool m_bStarted;

  /// Enable flag
  bool m_bEnabled;
 
  /// Guid as a string
  std::string m_sGuid;

  /// GUID for Session
  GUID m_guid;

  /// Flush time (time to wait in seconds before flushing buffers)
  int m_iFlushTimer;

    
}; // class vxLogSession


#pragma warning(pop)

#endif