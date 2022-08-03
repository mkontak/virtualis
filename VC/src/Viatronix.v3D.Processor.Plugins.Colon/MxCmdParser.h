#ifndef MXCMDPARSER_INCLUDED // -*- C++ -*-
#define MXCMDPARSER_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxCmdParser

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxCmdParser.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxDynBlock.h"
#include "MxAsp.h"

typedef MxDynBlock<int1 *> MxCmdPhrase;

class MxCmd 
{
public:
  int1 *op;
  MxDynBlock<MxCmdPhrase> phrases;

  MxCmd(int4 N) : phrases(N) { op=NULL; }
};

class MxCmdParser 
{
private:
  MxCmd cmd;
  MxAspStore store;

public:
  bool will_ignore_unknown;

public:
  MxCmdParser();

  MxAspStore *asp_store() { return &store; }

  void parse_line(int1 *, void *closure=NULL);

  virtual bool execute_command(const MxCmd& cmd, void *closure=NULL);
};

// MXCMDPARSER_INCLUDED
#endif
