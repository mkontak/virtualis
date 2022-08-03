// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogFactory.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once


// includes
#include "vxLogEnums.h"
#include "vxLogger.h"
#include "vxLogRecord.h"

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)

// typedefs
typedef std::vector<std::shared_ptr<vxLogger>> LOGMAP;
typedef std::vector<std::shared_ptr<vxLogger>>::iterator  LOGITERATOR;


/**
 * Log Factory class creates Loggers
 */
class VX_LOGGER_DLL vxLogFactory
{

private:

  ///Private Constructor
  vxLogFactory();

public:

   ///Destructor
   virtual ~vxLogFactory(void);
  
   /// Creates the Logger and add it to the list for usage
   static inline std::shared_ptr<vxLogger> CreateLogger(  const std::string & sHandle, 
                                                          const std::string & sFilename, 
                                                          const vxLogRecord::SeverityEnum eSeverity, 
                                                          const vxLogMessageFormats eFormat,
                                                          bool bDefault = false, 
                                                          const std::string & sServer = "", 
                                                          const unsigned int iPort = 0 )
   { return CreateLogger( vxLoggerInfo(sHandle, sFilename, eSeverity, eFormat, sServer, iPort), bDefault ); }



   /// Creates a Logger with a unique name. 
   static inline std::shared_ptr<vxLogger> CreateLogger(  const std::string & sFilename, 
                                                          const vxLogRecord::SeverityEnum eSeverity, 
                                                          const vxLogMessageFormats eFormat,
                                                          bool bDefault = false, 
                                                          const std::string & sServer = "", 
                                                          const unsigned int iPort = 0 )
   { return CreateLogger( vxLoggerInfo(vxLogUtility::GenerateGuid(), sFilename, eSeverity, eFormat, sServer, iPort), bDefault ); }



   /// Creates a Logger with a unique name. 
   static std::shared_ptr<vxLogger>  CreateLogger(  const vxLoggerInfo & loggerInfo, bool bDefault = false);
  

                                                   

   ///Gets the  Log level for  Handle
   static vxLogRecord::SeverityEnum GetLogLevel( const std::string & sHandle = "" );


   ///Releases the Logger
   static void ReleaseLogger( const std::string & sHandle );

   ///Logs the Message
   static void Message( const std::string & sHandle, const vxLogRecord & record ) ;

   /// Releases all loggers
   static void ReleaseLoggers();

   /// Gets the current default loggers file name
   static std::string GetDefaultLoggerFilename();

   /// Gets the cuurent default loggers logging level
   static vxLogRecord::SeverityEnum GetDefaultLoggerLogLevel();

   /// Backward compatible
   static const std::string & GetDefaultLogger() 
   { static std::string sDefault("Default"); return sDefault; }

   /// Gets the process Id
   static const int GetProcessId() 
   { return m_iProcessId; }

    /// Gets the mutex name
   static const std::string GetMutexName();

   /// Gets the deffault filename
   static const std::string & GetDefaultFilename()
   { return vxLoggerInfo::GetDefaultFilename(); }

#pragma region Nutex Enabled Methods

private:

  /// Get the logger specified by the sHandle
  static std::shared_ptr<vxLogger> GetLogger(const vxLoggerInfo & loggerInfo);
 
  /// Get the logger specified by the sHandle
  static std::shared_ptr<vxLogger> GetLogger(const std::string & sHandle)
  { return GetLogger(vxLoggerInfo(sHandle,"")); }

  /// Inserts into the list
  static void  Insert(std::shared_ptr<vxLogger> logger);

#pragma endregion


#pragma region fields

private:

  ///Map object keeps the map of Handle and Logger Pair.
  static LOGMAP m_loggers;
  
  /// Flag used to stop logging
  static bool m_bLoggingEnabled;

  /// Process Id
  static int m_iProcessId;

  /// Mutex Name
  static std::string m_sMutexName;

  /// Default logger
  static std::shared_ptr<vxLogger> m_pDefaultLogger;


#pragma endregion

}; // class vxLogFactory


#pragma warning(pop)