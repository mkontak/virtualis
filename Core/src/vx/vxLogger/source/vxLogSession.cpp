// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogSession.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "vxLogProvider.h"
#include "vxLogSession.h"
#include "vxLogProvider.h"
#include "vxLogRecord.h"
#include "vxLogUtility.h"
#include "vxLogExceptions.h"

// ======================================================================
// Exposes the Session object if we are building for Windows 7 and up.
//=======================================================================
#ifndef WINXP

size_t vxLogSession::m_maxString = 1024;


/**
 * Constructor
 *
 * @param loggerInfo      Logger information needed for the session
 */
vxLogSession::vxLogSession(const vxLoggerInfo & loggerInfo) :
vxLogger(loggerInfo),
m_properties(__nullptr),
m_hTraceHandle(0),
m_iFlushTimer(0),
m_bRestarting(false)
{

  CoCreateGuid(&m_guid); 

  m_sGuid = util::Strings::ToString(m_guid);
 
  Initialize();

} // vxLogSession() 

/**
 * Destructor
 *
  * Release the session
 */
vxLogSession::~vxLogSession()
{
  Release();
} // ~vxLogSession()

/**
 * Releases the session
 */
void vxLogSession::Release()
{

  // ========================================
  // If the trace handle has been allocated
  // =======================================
  if ( m_hTraceHandle != 0 )
  {
    // Disable the logging
    Disable();

    // Stop the logging
    Stop();
  } // END ... If the trace handle was created

  /// Release the properties
  ReleaseProperties();

} // Release()

/**
 * Restarts the session
 */
void vxLogSession::Restart()
{
  // ===================================
  // If we are not already restarting
  // ==================================
  if ( ! m_bRestarting )
  {
    LogDbg(util::Strings::Format(std::string(""),"Restarting tracing session %s", m_sHandle.c_str()), "vxLogSession", "Restart");
    m_bRestarting = true;
    Stop();
    Start();
  }
  else
  {
    LogDbg(util::Strings::Format(std::string(""),"Session %s is already restarting", m_sHandle.c_str()), "vxLogSession", "Restart");
    m_bRestarting = false;
  }

}  // Restart()

/**
 * Releases the session properties
 */
void vxLogSession::ReleaseProperties()
{
  if ( m_properties != __nullptr )
  {
    delete m_properties;
    m_properties = __nullptr;
  }
} // ReleaseProperties()

/**
 * Starts the trace session
 */
void vxLogSession::Start()
{

  if ( ! IsStarted() )
  {

    /// INitialize first
    Initialize();

    LogDbg(util::Strings::Format(std::string(""),"Starting tracing session %s", m_sHandle.c_str()), "vxLogSession", "Start");

    ULONG status = StartTrace(&m_hTraceHandle, m_sHandle.c_str(), m_properties );

    // ==========================================================
    // If the session is already up attempt to stop it an restart
    // ==========================================================
    if ( status == ERROR_ALREADY_EXISTS )
    {
      LogDbg(util::Strings::Format(std::string(""),"Session %s is already running attempting a restart", m_sHandle.c_str()), "vxLogSession", "Start");

      Restart();

    } // 
    else if (status != ERROR_SUCCESS) 
    {
      LogErr(util::Strings::Format(std::string(""),"Session %s failed to start [%ld]", m_sHandle.c_str(), status), "vxLogSession", "Start");
    }

  } // END ... If the 
      
} // Start()



/**
 * Stops the trace logger
 */
void vxLogSession::Stop()
{

  LogDbg(util::Strings::Format(std::string(""),"Stopping tracing session %s", m_sHandle.c_str()), "vxLogSession", "Stop");

  /// Stop the tracing
  ULONG status = ControlTrace(m_hTraceHandle, m_sHandle.c_str(), m_properties, EVENT_TRACE_CONTROL_STOP);


  if (status != ERROR_SUCCESS) 
     LogErr(util::Strings::Format(std::string(""),"Stopping of session %s failed [%ld]", m_sHandle.c_str(), status), "vxLogSession", "Stop");


} // Stop()



 
/**
 * Enables the tracing session
 */
void vxLogSession::Disable()
{

  if  ( IsStarted()  )
  {

    LogDbg(util::Strings::Format(std::string(""),"Disabling all providers in session %s", m_sHandle.c_str()), "vxLogSession", "Disable");

    for ( std::map<std::string, vxLogProvider *>::iterator it = m_providers.begin(); it != m_providers.end(); it++ )
    {
      Disable(it->second);

    } // END ... For each provider/sesion map
  }
  
} // Disable()

 
/**
 * Enables the tracing session
 */
void vxLogSession::Enable()
{

  if ( IsStarted()  )
  {
    LogDbg(util::Strings::Format(std::string(""),"Enabling all providers in session %s", m_sHandle.c_str()), "vxLogSession", "Disable");

    for ( std::map<std::string, vxLogProvider *>::iterator it = m_providers.begin(); it != m_providers.end(); it++ )
    {
      Enable(it->second);

    } // END ... For each provider/sesion map
  }
  
} // Enable()


/**
 * Enables the tracing session for the provider
 *
 * @param pProviderInfo     Provider/Session info
 */
void vxLogSession::Enable(vxLogProvider * provider)
{
  // Insert into the list
  Insert(provider);

  if ( IsStarted()  )
  {

    LogDbg(util::Strings::Format(std::string(""),"Enabling provider %s with session %s", provider->GetName().c_str(), m_sHandle.c_str()), "vxLogSession", "Enable");

    GUID providerGuid = provider->GetGuid();
 
    /// Disable the tracing
    ULONG status = EnableTraceEx(&providerGuid, __nullptr, m_hTraceHandle, TRUE, (UCHAR)m_eLogLevel, 0, 0, 0, __nullptr);

    /// New Call
    // ULONG status = EnableTraceEx2(m_sHandle, &providerGuid, __nullptr, (UCHAR)m_eLogLevel, 0, 0, 0, __nullptr );

    if (status != ERROR_SUCCESS) 
      LogDbg(util::Strings::Format(std::string(""),"Failed enabling provider %s with session %s [%d]", provider->GetName().c_str(), m_sHandle.c_str(), status),  "vxLogSession", "Enable");

  } // END ... If the sHandle indicates that it is already enabled


} // Enable(vxLogProviderInfo * pProviderInfo)


 /**
 * Disables the tracing session
 */
void vxLogSession::Disable(vxLogProvider * provider)
{
 
  if ( IsStarted()  )
  {
    LogDbg(util::Strings::Format(std::string(""),"Disabling provider %s with session %s", provider->GetName().c_str(), m_sHandle.c_str()), "vxLogSession", "Enable");

    /// Disable the tracing
    ULONG status = EnableTraceEx(&(provider->GetGuid()), __nullptr, m_hTraceHandle, FALSE, (UCHAR)m_eLogLevel, 0, 0, 0, __nullptr);

    if (status != ERROR_SUCCESS) 
       LogDbg(util::Strings::Format(std::string(""),"Failed disabling provider %s with session %s [%d]", provider->GetName().c_str(), m_sHandle.c_str(), status),  "vxLogSession", "Enable");

  } // END .. If the logger is already started


} // Disable()

/**
 * Flushes the session buffers
 */
void vxLogSession::Flush()
{
  // ======================
  // Only flush if started
  // ======================
  if ( IsStarted() )
  {
    LogDbg(util::Strings::Format(std::string(""),"Flushing session %s", m_sHandle.c_str()), "vxLogSession", "Flush");


    ULONG status = ControlTrace(m_hTraceHandle, m_sHandle.c_str(), m_properties, EVENT_TRACE_CONTROL_FLUSH);

    if (status != ERROR_SUCCESS) 
       LogDbg(util::Strings::Format(std::string(""),"Failed flushing session %s [%d]", m_sHandle.c_str(), status),  "vxLogSession", "Flush");

 } // END ... If the session is started
} // Flush()

/**
 * Initializes the logger.
 *
 * @param sName     Logger name
 * @param sFilename Filename associated with the logger
 * @param eLOgLevel Highest supported logging level
 */
std::shared_ptr<vxLogSession> vxLogSession::Create(const vxLoggerInfo & loggerInfo)
{

  std::shared_ptr<vxLogSession> pSession = std::shared_ptr<vxLogSession>(new vxLogSession(loggerInfo));

  if ( pSession == __nullptr )
    throw vxLogOutOfMemoryException(LogRec("Failed to allocation session", "vxLogSession", "Create"));
 
  /// Initializes the session
  pSession->Initialize();

  return pSession;
}


/**
 * Starts the session for the provider specified
 *
 * @param sName     Logger name
 * @param sFilename Filename associated with the logger
 * @param eLOgLevel Highest supported logging level
 */
void vxLogSession::Initialize()
{

 
  if ( ! IsInitialized() )
  {

    ///
    size_t maxSize = m_maxString * sizeof(TCHAR);

    size_t sizeNeeded = sizeof(EVENT_TRACE_PROPERTIES) + ( 2 * maxSize );

    m_properties = (PEVENT_TRACE_PROPERTIES)malloc(sizeNeeded);
 
    if (m_properties == __nullptr) 
      throw vxLogOutOfMemoryException(LogRec("Failed to allocation event trace properties", "vxLogSession", "Initialize"));
  
    RtlZeroMemory(m_properties, sizeNeeded);

    m_properties->Wnode.BufferSize        = (ULONG)sizeNeeded;
    m_properties->Wnode.ClientContext     = 2;
    m_properties->Wnode.Flags             = WNODE_FLAG_TRACED_GUID;

    // ========================================================================================================================================================
    // Adding the folowing mask to the line causes error code 32 and 1450 
    // EVENT_TRACE_PRIVATE_LOGGER_MODE 
    //
    // Adding the floowinging mask to the line casues the error code 87 (invalid parameter)
    // EVENT_TRACE_PRIVATE_IN_PROC
    //
    // Adding the folowing masks together causes an error code of 87 to be generated indicating an invalid parameter
    // EVENT_TRACE_FILE_MODE_SEQUENTIAL | EVENT_TRACE_FILE_MODE_NEWFILE  
    //
    // | EVENT_TRACE_NO_PER_PROCESSOR_BUFFERING 
    //
    // =======================================================================================================================================================
    m_properties->LogFileMode             = EVENT_TRACE_FILE_MODE_SEQUENTIAL | EVENT_TRACE_REAL_TIME_MODE | EVENT_TRACE_NO_PER_PROCESSOR_BUFFERING  | EVENT_TRACE_USE_PAGED_MEMORY;
    //m_properties->Wnode.Guid              = m_guid;
    m_properties->LoggerNameOffset        = sizeof(EVENT_TRACE_PROPERTIES);
    m_properties->LogFileNameOffset       = m_properties->LoggerNameOffset + (ULONG)maxSize;
    m_properties->FlushTimer              = m_iFlushTimer;

    LPTSTR loggerName  = (LPTSTR)((PCHAR)m_properties + m_properties->LoggerNameOffset);
    LPTSTR fileName    = (LPTSTR)((PCHAR)m_properties + m_properties->LogFileNameOffset);

    strcpy_s(loggerName, m_maxString, m_sHandle.c_str());
    strcpy_s(fileName, m_maxString, m_sFilename.c_str());

    LogDbg(util::Strings::Format(std::string(""),"Initializing session [%s] [%s]\n",m_sHandle.c_str(), m_sFilename.c_str()), "vxLogSession", "Initialize");

  }

} // Initialize();

/**
 * Starts the session for the provider specified
 *
 * @param sName     Logger name
 * @param sFilename Filename associated with the logger
 * @param eLOgLevel Highest supported logging level
 */
void vxLogSession::Insert(vxLogProvider * provider)
{

  if ( m_providers.find(provider->GetGuidAsString()) == m_providers.end() )
  {

    /// Make sure the provider is registered
    provider->Register();

    m_providers.insert(std::pair<std::string, vxLogProvider *>(provider->GetGuidAsString(), provider));
  }
  else
    LogDbg(util::Strings::Format(std::string(""),"Provider %s already added to session", provider->GetGuidAsString().c_str()), "vxLogSession", "Insert");
 
} // Create(const std::string & sName, const std::string & sFilename, SeverityEnum eLogLevel)


/**
 * Sets the file name for the logger. Allows for the changing of the log name during logging operations.
 *
 * @param sFilename     New log file name
 */
void vxLogSession::SetFilename(const std::string & sFilename)
{

  vxLoggerInfo::SetFilename(sFilename);

  if ( m_properties != __nullptr )
  {
    LPTSTR fileName    = (LPTSTR)((PCHAR)m_properties + m_properties->LogFileNameOffset);
    strcpy_s(fileName, m_maxString, m_sFilename.c_str());

    if ( IsStarted()  )
    {
      ULONG status = UpdateTrace(m_hTraceHandle, m_sHandle.c_str(), m_properties );

      if (status != ERROR_SUCCESS) 
        LogErr(util::Strings::Format(std::string(""),"Update session %s failed  [%ld]", m_sHandle.c_str(), status), "vxLogSession", "SetFilename");
   
    }
  }


} // SetFilename(const std::string & sFilename)



/**
 * Sets the log level for the logger
 *
 * @param eLogLevel     New log level
 */
void vxLogSession::SetLogLevel(vxLogRecord::SeverityEnum eLogLevel)
{
  if ( m_eLogLevel != eLogLevel )
  {

    m_eLogLevel = eLogLevel;

    // Renable the logger with the new level
    Enable();

  } // END ... If the log level is different
} // SetLogLevel(SeverityEnum eLogLevel)

#endif