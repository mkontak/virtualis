// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogFactory.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "StdAfx.h"
#include "vxLogFactory.h"
#include "vxLogProvider.h"
#include "vxLogUtility.h"
#include "vxLogExceptions.h"
#include "vxLogMutex.h"
#include "vxLoggerClassic.h"
#include "vxLoggerServer.h"

// statics
LOGMAP vxLogFactory::m_loggers;
bool vxLogFactory::m_bLoggingEnabled(false);
int vxLogFactory::m_iProcessId(GetCurrentProcessId());
std::string vxLogFactory::m_sMutexName("");
std::shared_ptr<vxLogger> vxLogFactory::m_pDefaultLogger(__nullptr);


/**
 * Destructor
 */
vxLogFactory::~vxLogFactory(void)
{
 
  ReleaseLoggers();

} // ~vxLogFactory()

/**
 * Gets the mutex name
 */
const std::string vxLogFactory::GetMutexName()
{
  // ==================================================
  // If the mutex name has not already been determined
  // =================================================
  if ( m_sMutexName.empty() )
  {
    std::ostringstream os;
    os << m_iProcessId;
    m_sMutexName = os.str();
  }

  return m_sMutexName;
}   // GetMutexName()

/**
 * Creates the logger specified for the name. If a logger with that name already exists then that logger will be returned.
 *
 * @param parameters  Logger information
 *
 */
std::shared_ptr<vxLogger> vxLogFactory::CreateLogger( const vxLoggerInfo & loggerInfo, bool bDefault )
{

  // Check if the logger with the same name already exists
  std::shared_ptr<vxLogger> logger(GetLogger(loggerInfo));

  if ( logger == __nullptr )
  {

    logger =  ( loggerInfo.IsServerBased()  ? vxLoggerServer::Create(loggerInfo) : vxLoggerClassic::Create(loggerInfo) );



    m_loggers.push_back(logger);

    // ====================================================================
    // If this logger is the default then either add it or overwrite it
    // ====================================================================
    if ( bDefault )
      m_pDefaultLogger = logger;

    /// Enable logging since a logger was created
    m_bLoggingEnabled = true;

    LogFyi(util::Strings::Format("Adding logger to list (%s, %s, %d, %s)",
                            logger->GetHandle().c_str(), 
                            logger->GetSystem().c_str(), 
                            (int)logger->GetLogLevel(), 
                            logger->GetFilename().c_str()), 
                            "vxLogFactory",
                            "CreateLogger");

  }
#ifdef DEBUG
  else
    vxLogUtility::LogMessage(util::Strings::Format("Logger already exists (%s, %s)",loggerInfo.GetHandle().c_str(),loggerInfo.GetSystem().c_str()) );
#endif
  

  return logger;

} // CreateLogger( const std::string & sName, const std::string & sFilename, vxLogRecord::SeverityEnum  eSeverity )






/**
 * Inserts the logger to the list
 */
void vxLogFactory::Insert(std::shared_ptr<vxLogger> logger)
{

 
  //vxLogMutex mutex(GetMutexName());

  //mutex.Wait();


  //m_loggers.insert( std::make_pair( sHandle, logger ) );


  m_loggers.push_back(logger);

}   // Inserts

/**
 * Gets the default logger file name
 */
std::string vxLogFactory::GetDefaultLoggerFilename()
{
  std::shared_ptr<vxLogger> logger = GetLogger(GetDefaultLogger());

  if ( logger == __nullptr )
    return std::string("");
  else
    return logger->GetFilename();
} // GetDefaultLoggerFilename()

/**
 * Gets the default logger logging level
 */
vxLogRecord::SeverityEnum vxLogFactory::GetDefaultLoggerLogLevel()
{
  std::shared_ptr<vxLogger> logger = GetLogger(GetDefaultLogger());

  if ( logger == __nullptr )
    return vxLoggerInfo::GetDefaultLogLevel();
  else
    return logger->GetLogLevel();
} // GetDefaultLoggerFilename()

/**
 * Gets the specified logger from the map
 *
 * @param sHandle      Loggers sHandle/name
 *
 * @return Returns the logger if found or NULL if not
 */
std::shared_ptr<vxLogger>  vxLogFactory::GetLogger(const vxLoggerInfo & loggerInfo)
{
 
  // ===========================================
  // Check against the local default logger
  // =========================================
  if ( loggerInfo.GetHandle() == GetDefaultLogger() && loggerInfo.GetSystem() == vxLogUtility::GetHostname() )
    return m_pDefaultLogger;
  else
  {
    LOGITERATOR it;
    for ( it = m_loggers.begin(); it != m_loggers.end() && ! (*it)->Equals(loggerInfo); it++ );

    if ( it != m_loggers.end() )
      return (*it);
    else
      return std::shared_ptr<vxLogger>(__nullptr);
  }



} // GetLogger(const std::string & sHandle)


/**
 * Writes the message to the specified logger
 *
 * @param sName       Name of the logger
 * @param record      Log record
 */
void vxLogFactory::Message( const std::string & sHandle, const vxLogRecord & record ) 
{
  

  // =============================
  // Make sure logging is enabled
  // =============================
  if (m_bLoggingEnabled ) 
  {

    std::shared_ptr<vxLogger> logger(GetLogger(sHandle));

    if ( logger != __nullptr )
    {
      logger->Trace(record);
    }
    else if ( record.GetClassName().substr(0,5) == "vxLog" )
    {

      if ( const_cast<vxLogRecord &>(record).GetSeverityLevel() == vxLogRecord::SL_VERBOSE )
      {

#ifdef DEBUG
        vxLogUtility::LogMessage(record.GetTextMessage());
#endif
        ;

      }
      else if ( const_cast<vxLogRecord &>(record).GetSeverityLevel() == vxLogRecord::SL_ERROR )
      {
        vxLogUtility::LogMessage(record.GetTextMessage());
      }
      
    }
    else
     vxLogUtility::LogMessage("Logger " + sHandle + " was not found : " + record.GetTextMessage());

  } // END ... Logging os enabled
  
 

}// Message( const std::string & sName, const vxLogRecord & record ) 


/**
 * Releases the specified logger
 *
 * @sHandle sName      Name of the logger to be released
 *
 */
void vxLogFactory::ReleaseLogger( const std::string & sHandle )
{

  LOGITERATOR it;
  for ( it = m_loggers.begin(); it != m_loggers.end() && (*it)->GetHandle() != sHandle; it++ );

  if ( it != m_loggers.end() )
    m_loggers.erase(it);

  m_bLoggingEnabled = ( m_loggers.size() > 0 );
 
} // ReleaseLogger( const std::string & sName )


/**
 * Releases all loggers
 */
void vxLogFactory::ReleaseLoggers(  )
{
  //vxLogMutex mutex(GetMutexName());

  //mutex.Wait();

  m_bLoggingEnabled = false;

  /// Clear 
  m_loggers.clear();


} // ReleaseLogger()

/**
 * Get the log level for the sHandle specified
 */
vxLogRecord::SeverityEnum vxLogFactory::GetLogLevel(  const std::string  & sHandle )
{
 
  std::shared_ptr<vxLogger> logger(GetLogger(sHandle));

  return  ( logger != __nullptr ? logger->GetLogLevel() : vxLoggerInfo::GetDefaultLogLevel() );

} // GetLogLevel(  const std::string & sSource )



