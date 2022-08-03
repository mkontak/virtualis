// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmLog.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmLog.h"


using namespace dcm;

std::string DcmLog::m_sDefaultSource("");



/**
 * Gets the default source
 *
 *  @return the string
 */
const std::string & DcmLog::GetDefaultSource()
{
    if ( m_sDefaultSource.empty() )
      m_sDefaultSource = vxLogFactory::GetDefaultLogger();

    return m_sDefaultSource;
} // GetDeafultSource()


#pragma region Logging



/**
 * Logs a Timing message
 *  
 *  @param sSource
 *  @param sMessage
 *  @param sClass
 *  @param sMethod
 */
void DcmLog::Timing( const std::string & sSource, const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
  LogTimeEx(sSource, sMessage, sClass, sMethod );

} // DebugTiming( std::string & sSource, std::string & sMessage, std::string sClass, std::string & sMethod);



/**
 * Logs a debug message
 *
 *  @param sMessage
 *  @param sClass
 *  @param sMethod
 */
void DcmLog::Timing( const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
    LogTime(sMessage, sClass, sMethod );

} // DebugTiming(  std::string & sMessage, std::string sClass, std::string & sMethod);



/**
 * Logs a debug message
 *  
 *  @param sSource
 *  @param sMessage
 *  @param sClass
 *  @param sMethod
 */
void DcmLog::Debug( const std::string & sSource, const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
    LogDbgEx(sSource, sMessage, sClass, sMethod );

} // Debug( std::string & sSource, std::string & sMessage, std::string sClass, std::string & sMethod);


/**
 * Logs a debug message
 *
 *  @param sMessage
 *  @param sClass
 *  @param sMethod
 */
void DcmLog::Debug( const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
    LogDbg(sMessage, sClass, sMethod );

} // Debug(  std::string & sMessage, std::string sClass, std::string & sMethod);


/**
 * Logs a warning message
 *
 *  @param sSource
 *  @param sMessage
 *  @param sClass
 *  @param sMethod
 */
void DcmLog::Warning( const std::string & sSource, const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
    LogWrnEx(sSource, sMessage, sClass, sMethod );

} // Warning( std::string & sSource, std::string & sMessage, std::string sClass, std::string & sMethod)



/**
 * Logs a warning message
 *  
 *  @param sMessage
 *  @param sClass
 *  @param sMethod
 */
void DcmLog::Warning( const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
    LogWrn(sMessage, sClass, sMethod );

} // Warning( std::string & sMessage, std::string sClass, std::string & sMethod)


/**
 * Logs a error message
 *
 *  @param sSource
 *  @param sMessage
 *  @param sClass
 *  @param sMethod
 */
void DcmLog::Error( const std::string & sSource, const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
  LogErrEx(sSource, sMessage, sClass, sMethod );

} // Error( std::string & sSource, std::string & sMessage, std::string sClass, std::string & sMethod)


/**
 * Logs a error message
 *  
 *  @param sMessage
 *  @param sClass
 *  @param sMethod
 */
void DcmLog::Error( const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
  LogErr(sMessage, sClass, sMethod );
} // Error(  std::string & sMessage, std::string sClass, std::string & sMethod)



/**
 * Logs a error message
 *
 *  @param sSource
 *  @param record
 */
void DcmLog::Error( const std::string & sSource,  const vxLogRecord & record )
{
  vxLogRecEx(sSource, record );
} // Error(  const DcmLogRec & logRec )


/**
 * Logs a error message
 *
 *  @param record
 */
void DcmLog::Error( const vxLogRecord & record )
{
  vxLogRec(record );
} // Error(  const DcmLogRec & logRec )


/**
 * Logs a information message
 *
 *  @param sSource
 *  @param sMessage
 *  @param sClass
 *  @param sMethod
 */
void DcmLog::Information( const std::string & sSource, const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
  LogFyiEx(sSource, sMessage, sClass, sMethod );
} // Information( std::string & sSource, std::string & sMessage, std::string sClass, std::string & sMethod)


/**
 * Logs a information message
 *
 *  @param sMessage
 *  @param sClass
 *  @param sMethod
 */
void DcmLog::Information( const std::string & sMessage, const std::string sClass, const std::string & sMethod )
{
  LogFyi(sMessage, sClass, sMethod );
} // Information( std::string & sMessage, std::string sClass, std::string & sMethod)

#pragma endregion
