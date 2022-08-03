/************************************************************************

  Implementation of error reporting and notification facilities.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: mixmsg.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $

 ************************************************************************/

#include "stdmix.h"
#include "mixmsg.h"


#include <stdarg.h>
#include <stdio.h>
 
ostream& MxMsg::error_stream = cerr;
#define FILE_REVISION "$Revision: 1.2 $"


MxSeverityLevel MxMsg::current_severity =
#if (SAFETY >= 0)
	MXMSG_NOTE;
#else
	MXMSG_WARN;
#endif

MxSeverityLevel MxMsg::current_lethality = MXMSG_ASSERT;

uint4 MxMsg::current_indent = 0;

mxmsg_handler MxMsg::current_handler = MxMsg::default_handler;


void MxMsg::prefix()
{
    for (uint4 i=0; i<current_indent; i++)
	error_stream << "    ";
}

void MxMsg::signal(MxSeverityLevel severity,
		  const int1 *msg, const int1 *context,
		  const int1 *filename, int4 line)
{
  if( severity <= current_severity )
  {
    MxMsgInfo info;
    
    info.severity = severity;
    info.message = msg;
    info.context = context;
    info.filename = filename;
    info.line = line;
    
    bool result = (*current_handler)(&info);
    
    if( !result )
    {
      cerr << "MXMSG PANIC: Error while reporting signal!" << endl;
      exit(1);
    }
    
    if( severity <= current_lethality )
    {
      if( severity == MXMSG_ASSERT )
      {
        //throw ex::AbortDataProcessingException( LogRec( "MxMsg", "Signal", "Error in signal") );
        ; // For certain reason, the above code cause compilation problem. Need to fix it later. 04/19/2011 dongqing
      }
      else exit(1);
    }
  }
}

void MxMsg::signalf(MxSeverityLevel severity, const int1 *format, ...)
{
  va_list args;
  
  // !!BUG: It would be nice not to rely on a fixed-size buffer here.
  int1 msg[512];
  
  va_start(args, format);
  // Mkk: Replaced vsprintf with vsnprintf_s
  //vsprintf(msg, format, args);
  vsnprintf_s(msg, 512, format, args);
  va_end(args);
  signal(severity, msg);
}


bool MxMsg::default_handler(MxMsgInfo *info)
{
    prefix();
    error_stream << MxMsg::severity_name(info->severity) << ": ";
    error_stream << info->message << endl;
    if( info->context )
    {
	prefix();
	error_stream << "  [Location: " << info->context << "]" << endl;
    }
    if( info->filename )
    {
	prefix();
	error_stream << "  [File: " << info->filename
		     << " at line " << info->line << "]" << endl;
    }

    return true;
}

void MxMsg::set_handler(mxmsg_handler h)
{
    current_handler = h;
}

MxSeverityLevel MxMsg::lethality_level() { return current_lethality; }
void MxMsg::lethality_level(MxSeverityLevel l) { current_lethality = l; }

MxSeverityLevel MxMsg::severity_level() { return current_severity; }
void MxMsg::severity_level(MxSeverityLevel l) { current_severity = l; }

static int1 *severity_names[] =
{
    "FATAL ERROR",
    "ASSERT",
    "ERROR",
    "WARNING",
    "NOTE",
    "DEBUG",
    "TRACE"
};

const int1 *MxMsg::severity_name(MxSeverityLevel l)
{
    if( l <= MXMSG_TRACE )
	return severity_names[l];
    else
	return "USER";
}

void MxMsg::indent(uint4 i) { current_indent += i; }
void MxMsg::dedent(uint4 i) { current_indent -= i; }

