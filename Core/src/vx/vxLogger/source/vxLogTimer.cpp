// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogTimer.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// include
#include "stdafx.h"
#include "vxLogTimer.h"
#include "vxLogger.h"



 /**
  *  Constructor
  *
  * @param sLogLevel      Type of log level for the timer
  */
vxLogTimer::vxLogTimer(vxLogRecord::SeverityEnum eLogLevel) :
m_eLogLevel(eLogLevel),
#ifndef WINXP
m_start(GetTickCount64())
#else
m_start(GetTickCount())
#endif
{
} // vxLogTimer(vxLogRecord::SeverityEnum eLogLevel)

/**
 * Resets the timer
 */
void vxLogTimer::Reset()
{
#ifndef WINXP
  m_start =GetTickCount64();
#else
  m_start = GetTickCount();
#endif
} // Reset()

/**
 * Returns the elapsed time from the creation of the object in milliseconds.
 *
 * @return Elapsed time in milliseconds.
 */
float vxLogTimer::GetElapsedTime()
{
#ifndef WINXP
  float elapsed = (float)(GetTickCount64() - m_start);
#else
  float elapsed = (float)(GetTickCount() - m_start);
#endif
  return elapsed  / 1000.0F;
}   // GetElapsedTime()


/**
 * Writes out the timer message to the default log file
 *
 * @param sFormat       Format string with the %f form the elapsed time
 *
 */
void vxLogTimer::Trace(const std::string & sClass, const std::string & sMethod, const std::string & sMessage)
{
  float fElapsedTime(GetElapsedTime() * 1000.0F);

  std::ostringstream os;

  os << sMessage << " took " << fElapsedTime << " milliseconds ";

  LogTime(os.str(), sClass, sMethod);

} // Trace(const std::string & sFomat)

/**
 * Writes out the timer message to the default log file
 *
 * @param sMessage       Format string with the %f form the elapsed time
 *
 */
void vxLogTimer::Trace(std::shared_ptr<vxLogger> logger, const std::string & sMessage)
{
  float fElapsedTime(GetElapsedTime());

  std::ostringstream os;

  os << sMessage << " took " << fElapsedTime << " seconds ";

  logger->Trace( LogRecEx( os.str(), "vxLogTimer", "Trace", vxLogRecord::SL_TIMING) );

} // Trace(const std::string & sFomat, std::shared_ptr<vxLogger> logger)


/**
 * Writes out the timer message to the default log file
 *
 * @param sMessage       Format string with the %f form the elapsed time
 *
 */
void vxLogTimer::Trace(const std::string & sHandle, const std::string & sMessage )
{
  float fElapsedTime(GetElapsedTime());

  std::ostringstream os;

  os << sMessage << " took " << fElapsedTime << " seconds ";


  LogTimeEx(sHandle, os.str(), "vxLogTimer", "Trace");
 
} // Trace(const std::string & sFomat)