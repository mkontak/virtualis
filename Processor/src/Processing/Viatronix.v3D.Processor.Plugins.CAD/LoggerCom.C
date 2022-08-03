// $Id: LoggerCom.C,v 1.2 2007/03/01 20:40:36 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#include "StdAfx.h"
#include "LoggerCom.h"


/**
 * Logs the specified error message
 *
 * @param   bsMsg         message text
 * @param   bsFunction    function name
 * @param   bsClass       class name
 * @return            S_OK for success
 */
STDMETHODIMP Logger::LogError( BSTR bsMsg, BSTR bsFunction, BSTR bsClass )
{
  LogMessage( bsMsg, bsFunction, bsClass, vxLogRecord::SL_EXCEPTION );
  return S_OK;
} // LogErr()


/**
 * Logs the specified warning message
 *
 * @param   bsMsg         message text
 * @param   bsFunction    function name
 * @param   bsClass       class name
 * @return            S_OK for success
 */
STDMETHODIMP Logger::LogWarning( BSTR bsMsg, BSTR bsFunction, BSTR bsClass )
{
  LogMessage( bsMsg, bsFunction, bsClass, vxLogRecord::SL_WARNING );
  return S_OK;
} // LogWrn()


/**
 * Logs the specified informational message
 *
 * @param   bsMsg         message text
 * @param   bsFunction    function name
 * @param   bsClass       class name
 * @return            S_OK for success
 */
STDMETHODIMP Logger::LogInformation( BSTR bsMsg, BSTR bsFunction, BSTR bsClass )
{
  LogMessage( bsMsg, bsFunction, bsClass, vxLogRecord::SL_INFORMATION );
  return S_OK;
} // LogFyi()


/**
 * Logs the specified debug message
 *
 * @param   bsMsg         message text
 * @param   bsFunction    function name
 * @param   bsClass       class name
 * @return            S_OK for success
 */
STDMETHODIMP Logger::LogDebug( BSTR bsMsg, BSTR bsFunction, BSTR bsClass )
{
  LogMessage( bsMsg, bsFunction, bsClass, vxLogRecord::SL_DEBUG );
  return S_OK;
} // LogDbg()


/**
 * Logs the specified temporary message
 *
 * @param   bsMsg         message text
 * @param   bsFunction    function name
 * @param   bsClass       class name
 * @return            S_OK for success
 */
STDMETHODIMP Logger::LogTemporary( BSTR bsMsg, BSTR bsFunction, BSTR bsClass )
{
  LogMessage( bsMsg, bsFunction, bsClass, vxLogRecord::SL_TEMPORARY );
  return S_OK;
} // LogTmp()


/**
 * Logs a message with the specified values
 *
 * @param   bsMsg         message text
 * @param   bsFunction    function name
 * @param   bsClass       class name
 * @param   eSeverity     severity
 */
void Logger::LogMessage( BSTR bsMsg, BSTR bsFunction, BSTR bsClass, const vxLogRecord::SeverityEnum & eSeverity )
{
  std::string sMsg( static_cast< const char * >( _bstr_t( bsMsg ) ) );
  std::string sFunction( static_cast< const char * >( _bstr_t( bsFunction ) ) );
  std::string sClass( static_cast< const char * >( _bstr_t( bsClass ) ) );

  vxLogFactory::Message( vxLogFactory::GetDefaultLogger(), vxLogRecord( "", sClass, sFunction, eSeverity ) );
} // LogMessage( BSTR bsMsg, BSTR bsFunction, BSTR bsClass, const vxLogRecord::SeverityEnum & eSeverity )


// $Log: LoggerCom.C,v $
// Revision 1.2  2007/03/01 20:40:36  geconomos
// code review preparation
//
// Revision 1.1  2005/09/23 12:07:29  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/LoggerCom.C,v 1.2 2007/03/01 20:40:36 geconomos Exp $
// $Id: LoggerCom.C,v 1.2 2007/03/01 20:40:36 geconomos Exp $
