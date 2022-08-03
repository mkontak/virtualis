#ifndef MIXMSG_INCLUDED // -*- C++ -*-
#define MIXMSG_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MixKit error reporting and message notification support.

  This module is meant to provide a small but reasonably flexible way of
  reporting errors and other interesting events to the user.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: mixmsg.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

//
// Message severity levels
//

enum MxSeverityLevel
{
  MXMSG_FATAL = 0,
    MXMSG_ASSERT,
    MXMSG_ERROR,
    MXMSG_WARN,
    MXMSG_NOTE,
    MXMSG_DEBUG,
    MXMSG_TRACE
};

class MxMsgInfo 
{
public:
  MxSeverityLevel severity;
  const int1 *message;
  const int1 *context;
  const int1 *filename;
  int4 line;
};

typedef bool (*mxmsg_handler)(MxMsgInfo *);

class MxMsg  {
public:
  static ostream&        error_stream;
  static MxSeverityLevel current_severity;
  static mxmsg_handler   current_handler;
  static MxSeverityLevel current_lethality;
  static uint4            current_indent;

  static void prefix();

  static void signal(MxSeverityLevel severity,
			 const int1 *msg, const int1 *context=NULL,
       const int1 *filename=NULL,
       int4 line=0);

  static void signalf(MxSeverityLevel severity, const int1 *format, ...);
  
  
  static bool default_handler(MxMsgInfo *);
  static void set_handler(mxmsg_handler h=default_handler);
  
  static MxSeverityLevel severity_level();
  static void            severity_level(MxSeverityLevel);
  static const int1 *    severity_name(MxSeverityLevel);
  
  static MxSeverityLevel lethality_level();
  static void            lethality_level(MxSeverityLevel);
  
  static void indent(uint4 i=1);
  static void dedent(uint4 i=1);
};

// MIXMSG_INCLUDED
#endif
