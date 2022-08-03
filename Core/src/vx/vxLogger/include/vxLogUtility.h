// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogUtility.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// pargams
#pragma warning(push)
#pragma warning(disable:4251)

/**
 * Used to extract information from the SplitPath vector
 */
typedef enum PathSections
{
  PS_DRIVE = 0,
  PS_DIR = 1,
  PS_FILE = 2,
  PS_EXT = 3
} PathSections;

/**
 * Contains utility methods used throughout the logger library
 */
class VX_LOGGER_DLL vxLogUtility
{

public:

  /// Logs an error message
  static void LogMessage( const std::string & sMessage );

   /// Logs an error message
  static void LogDebugMessage( const std::string & sMessage );


  /// Gets the Day of Week 
  static std::string DayoftheWeek( const WORD & wDayOfWeek );

  /// Generate a GUID and returns the string representation
  static std::string GenerateGuid();

  /// Gets the log file
  static std::string GetLogFile();


  /// Gets the host name
  static const std::string & GetHostname();



 
private:

  /// Log filename
  static std::string m_sLogFile;


  /// Hostname
  static std::string m_sHostname;

};  // class vxLogUtility

#pragma warning(pop)