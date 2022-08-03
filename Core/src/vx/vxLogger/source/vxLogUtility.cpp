// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogUtility.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


// includes
#include "stdafx.h"
#include "vxLogUtility.h"
#include "vxUtilIO.h"
#include <Psapi.h>

// statics
std::string vxLogUtility::m_sLogFile("");
std::string vxLogUtility::m_sHostname("");


/**
 * Logs the supplied message. Used internally only.
 *
 * @param sMessage  Message to log
 *
 */
void vxLogUtility::LogMessage( const std::string & sMessage)
{

  FILE * fp(__nullptr);

  try
  {
    if ( ( fp = fopen(GetLogFile().c_str(), "a") ) != __nullptr )
    {

      time_t ltime;
      time( &ltime );    //Get time as long integer. 
      struct tm *newtime = localtime( &ltime ); // Convert to local time.

 
      fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d : %s\n", newtime->tm_year + 1900, newtime->tm_mon, newtime->tm_mday, newtime->tm_hour, newtime->tm_min , newtime->tm_sec, sMessage.c_str());

 
      fclose(fp);


    }
  }
  catch ( ... )
  {
    if ( fp != __nullptr )
      fclose(fp);
  }


} // LogMessage( const std::string & sId, const std::string & sMessage

/**
 * Logs a debug message
 *
 * @param sMessage    Message to be logged
 */
void vxLogUtility::LogDebugMessage(const std::string & sMessage)
{

#ifdef DEBUG
  LogMessage(sMessage);
#endif

} // LogDebugMessage(const std::string & sMessage)


/**
 *  Determine day of the week.
 *
 * @param wDayOfWeek    day of the week in DWORD.
 *
 * @return day          day in std::string
 */
std::string vxLogUtility::DayoftheWeek( const WORD & wDayOfWeek )
{
  std::string day;
  switch ( wDayOfWeek )
  {
  case 0:
    day = "Sunday";
    break;

  case 1:
    day = "Monday";
    break;

  case 2:
    day = "Tuesday";
    break;

  case 3:
    day = "Wednesday";
    break;

  case 4:
    day = "Thursday";
    break;

  case 5:
    day = "Friday";
    break;

  case 6:
    day = "Saturday";
    break;
  }
  return day;

} // DayoftheWeek( const WORD & wDayOfWeek )const

/**
 * Generates a GUID and returns the string
 */
std::string vxLogUtility::GenerateGuid()
{
  
  GUID guid;

  if( FAILED( CoCreateGuid( &guid ) ))
    throw vxLogException(LogRec(util::Strings::Format(std::string(""),"Failed to generate guid [%ld]", GetLastError()) , "vxLogUtility", "GenerateGuid"));

  return util::Strings::ToString(guid);
} // GenerateGuid()

/**
 * Gets the log file for errors generated in the logger. These files are 
 * processor id based.
 */
std::string vxLogUtility::GetLogFile()
{
  if ( m_sLogFile.empty() )
  {

    std::string sPath(util::IO::GetDefaultLogDirectory() + "vxLogger");


    // Make sure the directory exists
    BOOL bReults = ::CreateDirectory(sPath.c_str(), NULL); 

    sPath.append("\\");

    HANDLE handle = GetCurrentProcess();
  
    char imageFileName[513];
    memset(imageFileName,0,513);
    DWORD size(512);

    ::GetProcessImageFileName(handle, imageFileName, size);

    char file[_MAX_DIR + 1];
    memset(file,0,_MAX_DIR + 1);
    strcpy(file,"vxLogger");

    errno_t err = _splitpath_s(imageFileName, __nullptr, 0, __nullptr, 0, file, _MAX_DIR, __nullptr, 0);

    std::ostringstream os;

    /// Make sure that the logger has its own sub-directory
    os << sPath <<  file << ".txt";

    m_sLogFile = os.str();
  }

  return m_sLogFile;
} // GetLogFile()


/**
 * Gets the host name
 */
const std::string & vxLogUtility::GetHostname()
{

  if ( m_sHostname.empty() )
  {

    WORD wVersionRequested = MAKEWORD(2,2);
    WSADATA wsaData;

    if ( WSAStartup(wVersionRequested,&wsaData) == 0 )
    {
      char host[65];
      memset(host,0,65);
      ::gethostname(host, 64);
      m_sHostname.assign(host);

      WSACleanup();
    }
  }

  return m_sHostname;

} // GetHostname()