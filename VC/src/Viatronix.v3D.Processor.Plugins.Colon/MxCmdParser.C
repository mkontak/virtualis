/************************************************************************

  MxCmdParser

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxCmdParser.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxCmdParser.h"

#include "MxDynBlock.h"
#include <ctype.h>

typedef MxDynBlock<int1 *> string_list;

#define MXCMD_PHRASE_SEPARATOR ';'

static int1 *trimleft(int1 *line)
{
  if ( !line ) return NULL;
  while (isspace(*line)) line++;
  return line;
}

static int1 *shift_token(int1 *line)
{
  if( !line )  return NULL;

  int1 *rest = NULL;
  while ( *line && !isspace(*line) )  line++;
  if( *line )  rest = line+1;
  *line = '\0';
  return trimleft(rest);
}

static int1 *shift_phrase(int1 *line)
{
  if ( !line )  
    return NULL;

  int1 *rest = NULL;
  while ( *line && *line != MXCMD_PHRASE_SEPARATOR ) line++;
  if ( *line )  rest = line+1;
  *line = '\0';

  return trimleft(rest);
}

MxCmdParser::MxCmdParser()  : cmd(16)
{
  will_ignore_unknown = true;
}

void MxCmdParser::parse_line(int1 *line, void *closure)
{
  uint4 i,j;

  line = trimleft(line);
  if (!line[0] || line[0] == '#')  
    return; // Ignore comments and empty lines

  // Extract operator
  int1 *op = line;   
  line = shift_token(line);

  // Extract phrases
  string_list phrases;
  while (line)
  {
    phrases.add(line);
    line = shift_phrase(line);
  }
  cmd.phrases.reset();
  cmd.op = op;

  // Extract args in phrases
  string_list args;
  for (i=0; i<phrases.length(); i++)
  {
    int1 *phrase = phrases[i];
    args.reset();
    while ( phrase )
    {
      int1 *arg = phrase;
      phrase = shift_token(phrase);
      if ( *arg )  
        args.add(arg); // trailing space doesn't count
    }
    cmd.phrases.add();
    cmd.phrases[i].reset();
        
    if ( args.length() )
    {
      for(j=0; j<args.length(); j++)
        cmd.phrases[i].add(args[j]);
    }
  }

  bool result = execute_command(cmd, closure);
  if( !result && phrases.length()==1 )
	result = store.execute_command(cmd.phrases[0].length(), cmd.op, (const int1 **)&cmd.phrases[0][0]);

  if( !result && !will_ignore_unknown )
    MxMsg::signal(MXMSG_WARN, "Unknown operator on input", line);
}

bool MxCmdParser::execute_command(const MxCmd& cmd, void *)
{
  return false;
}