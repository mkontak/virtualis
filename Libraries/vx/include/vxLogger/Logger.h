// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Logger.h
//
// Description: Main include for this library
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <Windows.h>


#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <map>
#include <vector>
#include <memory>

#include <iostream>
#include <functional>
#include <exception>
#include <comutil.h>
#include <comdef.h>
#include <wmistr.h>
#include <sstream>
#include <time.h>
#include <atltime.h>
#include <vector>
#include <stdlib.h>
#include <tchar.h>
#include <initguid.h>
#include <guiddef.h>
#include <io.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <tdh.h>

#include <evntprov.h>
#include <evntcons.h>
#include <evntrace.h>



#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

#ifdef VX_LOGGER_BUILD
  #define VX_LOGGER_DLL __declspec(dllexport)
  #define VX_LOGGER_EXTERN 
#else
  #define VX_LOGGER_DLL __declspec(dllimport)
  #define VX_LOGGER_EXTERN extern
#endif


#include "scope_exit.h"
#include "vxLogEnums.h"
#include "vxLogRecord.h"
#include "vxLogExceptions.h"
#include "vxLogUtility.h"
#include "vxLogCallback.h"
#include "vxLogThread.h"
#include "vxLogProvider.h"
#include "vxLogSession.h"
#include "vxLogger.h"
#include "vxLogFactory.h"
#include "vxLogTimer.h"
#include "vxLogServer.h"
#include "vxLoggerInfo.h"
#include "vxLoggerClassic.h"
#include "vxLoggerServer.h"
#include "vxLogServer.h"
#include "vxUtilIO.h"
#include "vxUtilStrings.h"
#include "vxUtilThread.h"
#include "vxXmlExceptions.h"
#include "vxXmlNode.h"
#include "vxXmlDocument.h"
#include "vxXmlSchema.h"



// =======================================================
// Original Log macros used to log to the default logger
// =======================================================
#define LogRec(MSG,CLASS,FUNC) vxLogRecord(__FILE__, __LINE__, CLASS, FUNC, MSG)

#define vxLogRec(RECORD)      vxLogFactory::Message( vxLogFactory::GetDefaultLogger(), RECORD );

#define LogErr(MSG,CLASS,FUNC) vxLogFactory::Message( vxLogFactory::GetDefaultLogger(), vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_EXCEPTION ) );
#define LogWrn(MSG,CLASS,FUNC) vxLogFactory::Message( vxLogFactory::GetDefaultLogger(), vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_WARNING ) );
#define LogFyi(MSG,CLASS,FUNC) vxLogFactory::Message( vxLogFactory::GetDefaultLogger(), vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_INFORMATION ) );
#define LogDbg(MSG,CLASS,FUNC) vxLogFactory::Message( vxLogFactory::GetDefaultLogger(), vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_DEBUG ) );
#define LogTmp(MSG,CLASS,FUNC) vxLogFactory::Message( vxLogFactory::GetDefaultLogger(), vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_TEMPORARY ) );
#define LogCrt(MSG,CLASS,FUNC) vxLogFactory::Message( vxLogFactory::GetDefaultLogger(), vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_CRITICAL )  );
#define LogTime(MSG,CLASS,FUNC) vxLogFactory::Message( vxLogFactory::GetDefaultLogger(), vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_TIMING )  );

// ===================================================
// Extended Logging Macros used to specify the HANDLE
// ===================================================

#define LogRecEx(MSG, CLASS, FUNC, SEVERITY)   vxLogRecord(__FILE__, __LINE__, CLASS, FUNC, MSG, (vxLogRecord::SeverityEnum)SEVERITY )

#define vxLogRecEx(LOGHANDLE,RECORD)      vxLogFactory::Message( LOGHANDLE, RECORD );

#define LogErrEx(LOGHANDLE,MSG,CLASS,FUNC) vxLogFactory::Message( LOGHANDLE, vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_EXCEPTION ) );
#define LogWrnEx(LOGHANDLE,MSG,CLASS,FUNC) vxLogFactory::Message( LOGHANDLE, vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_WARNING ) );
#define LogFyiEx(LOGHANDLE,MSG,CLASS,FUNC) vxLogFactory::Message( LOGHANDLE, vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_INFORMATION ) );
#define LogDbgEx(LOGHANDLE,MSG,CLASS,FUNC) vxLogFactory::Message( LOGHANDLE, vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_DEBUG ) );
#define LogTmpEx(LOGHANDLE,MSG,CLASS,FUNC) vxLogFactory::Message( LOGHANDLE, vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_TEMPORARY ) );
#define LogCrtEx(LOGHANDLE,MSG,CLASS,FUNC) vxLogFactory::Message( LOGHANDLE, vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_CRITICAL )  );
#define LogTimeEx(LOGHANDLE,MSG,CLASS,FUNC) vxLogFactory::Message( LOGHANDLE, vxLogRecord( __FILE__, __LINE__, CLASS, FUNC, MSG , vxLogRecord::SL_TIMING )  );
