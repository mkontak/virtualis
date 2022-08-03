// $Id: ConsoleBuffer.C,v 1.5 2007/03/07 15:05:41 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#include "StdAfx.h"
#include "ConsoleBuffer.h"
#include "WaitCursor.h"
//#include "LicenseValidator.h"


/**
 * Writes the specified text to the console
 *
 * @param   sText   output text
 * @param   eColor  text color
 */
void ConsoleBuffer::Write( const char * sText, const ConsoleBuffer::TextColorEnum & eColor )
{
  // set the text color
  SetConsoleTextAttribute( GetScreenBuffer(), static_cast< WORD >( eColor ) );
  
  // write the text to the console
  DWORD dwBytesWritten( 0 );
  WriteConsole( GetScreenBuffer(), sText, lstrlen( sText ), &dwBytesWritten, NULL );
  
  // update state
  UpdateScreenBufferInfo();
} // Write( const char * sText, const ConsoleBuffer::TextColorEnum & eColor )


/**
 * Writes the supplied text to the console at the specified position
 *
 * @param   sText   output text
 * @param   x       x position
 * @param   y       y position
 * @param   eColor  text color
 */
void ConsoleBuffer::Write( const char * sText, const int4 x, const int4 y, const ConsoleBuffer::TextColorEnum & eColor )
{
  // position the console's cursor
  COORD pos;
  pos.X = x;
  pos.Y = y;
  SetConsoleCursorPosition( GetScreenBuffer(), pos );

  Write( sText, eColor );
} // Write( const char * sText, const ConsoleBuffer::TextColorEnum & eColor )


/**
 * Writes the specified text to the console and moves the cursor to the beginning of the next line
 *
 * @param   sText   output text
 * @param   eColor  text color
 */
void ConsoleBuffer::WriteLine( const char * sText, const ConsoleBuffer::TextColorEnum & eColor )
{
  Write( sText, eColor );
  Newline();
} // WriteLine( const char * sText, const ConsoleBuffer::TextColorEnum & eColor )


/**
 * Positions the cursor at the begining next buffer line
 *
 * @param   iNumberOfLines   number of lines to skip
 */
void ConsoleBuffer::Newline( const int4 iNumberOfLines )
{
  COORD pos;
  pos.X = 0;
  pos.Y = GetScreenBufferInfo().dwCursorPosition.Y + iNumberOfLines;
  SetConsoleCursorPosition( GetScreenBuffer(), pos );
  UpdateScreenBufferInfo();
} // Newline()


/**
 * Writes out a command line argument to the console
 *
 * @param   sArgumentName     argument name
 * @param   sArgumentValue    argument value
 */
void ConsoleBuffer::WriteArgument( const char * sArgumentName, const char * sArgumentValue )
{
  // write out argument name with trailing ellipses
  const TextColorEnum & eArgumentNameColor( ConsoleBuffer::PURPLE_DIM );
  Write( sArgumentName, eArgumentNameColor );
  
  // write out the argument value
  const TextColorEnum & eArgumentValueColor( ConsoleBuffer::YELLOW );
  Write( sArgumentValue, eArgumentValueColor );

  Newline();

} // WriteArgument( const char * sArgumentName, const char * sArgumentValue )


/**
 * Writes the necessary text for beginning a step
 *
 * @param   sText   step text
 */
void ConsoleBuffer::WriteStepBegin( const char * sText )
{
  DWORD dwWritten( 0 );

  HANDLE hStdout = GetStdHandle( STD_OUTPUT_HANDLE ); 

  CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo( hStdout, &info );  

  Write( " * ", ConsoleBuffer::GREEN ); 
  Write( sText, ConsoleBuffer::WHITE ); 

  // strat a wait cursor
  WaitCursor::Start();
} // WriteStepBegin( const char * sText )


/**
 * Writes the necessary text for ending a step
 *
 * @param   bSuccess   indicates step's success
 */
void ConsoleBuffer::WriteStepEnd( bool bSuccess )
{
  // stop the wait cursor
  WaitCursor::Stop();

  // position the text to right of the current position
  const uint4 xpos = 70;
  const uint4 ypos = GetScreenBufferInfo().dwCursorPosition.Y;

  if( bSuccess )
  {
    Write( "[ ", xpos, ypos, ConsoleBuffer::BLUE );
    Write( "OK", ConsoleBuffer::GREEN );
    Write( " ]", ConsoleBuffer::BLUE );
  }
  else
  {
    Write( "[ ", xpos, ypos, ConsoleBuffer::RED );
    Write( "ERROR", ConsoleBuffer::RED );
    Write( " ]",    ConsoleBuffer::RED );
  }
  Newline();
} // WriteStepEnd( bool bSuccess )


/**
 * Writes out the specified com exception
 *
 * @param   e   com exception
 */
void ConsoleBuffer::WriteComException( _com_error & e )
{
  WriteLine( "--------------------", ConsoleBuffer::RED );
  WriteLine( "COM EXCEPTION CAUGHT", ConsoleBuffer::RED );
  WriteLine( "--------------------", ConsoleBuffer::RED );
  Newline();
  WriteLine( e.ErrorMessage(), ConsoleBuffer::RED );
  Newline();
  WriteLine( "See LOGFILE for additional details", ConsoleBuffer::RED );
  
} // WriteComException( _com_error & e )


/**
 * Writes out the specified viatronix exception
 *
 * @param   e   viatronix exception
 */
void ConsoleBuffer::WriteViatronixException( ex::VException & e )
{
  WriteLine( "--------------------------", ConsoleBuffer::RED );
  WriteLine( "VIATRONIX EXCEPTION CAUGHT", ConsoleBuffer::RED );
  WriteLine( "--------------------------", ConsoleBuffer::RED );
  Newline();
  WriteLine( e.GetLogRecord().GetTextMessage().c_str(), ConsoleBuffer::RED );
  Newline();
  WriteLine( "See LOGFILE for additional details", ConsoleBuffer::RED );
} // WriteViatronixException( ex::VException & e )


/**
 * Writes out the unhandled exception
 */
void ConsoleBuffer::WriteUnhandledException()
{
  WriteLine( "--------------------------", ConsoleBuffer::RED );
  WriteLine( "UNHANDLED EXCEPTION CAUGHT", ConsoleBuffer::RED );
  WriteLine( "--------------------------", ConsoleBuffer::RED );
  Newline();
  WriteLine( "See LOGFILE for additional details", ConsoleBuffer::RED );
} // WriteUnhandledException()


/**
 * Gets a handle to the screen buffer
 *
 * @return  handle to screen buffer
 */
HANDLE ConsoleBuffer::GetScreenBuffer()
{
  static HANDLE hScreenBuffer = GetStdHandle( STD_OUTPUT_HANDLE ); 
  return hScreenBuffer;
} // GetScreenBuffer()


/**
 * Gets a CONSOLE_SCREEN_BUFFER_INFO for the current state 
 *
 * @return     CONSOLE_SCREEN_BUFFER_INFO for the current state    
 */
CONSOLE_SCREEN_BUFFER_INFO & ConsoleBuffer::GetScreenBufferInfo()
{
  static CONSOLE_SCREEN_BUFFER_INFO info;
  return info;
} // GetScreenBufferInfo()


/**
 * Updates the CONSOLE_SCREEN_BUFFER_INFO member
 */
void ConsoleBuffer::UpdateScreenBufferInfo()
{
  GetConsoleScreenBufferInfo( GetScreenBuffer(), &GetScreenBufferInfo() );  
} // UpdateScreenBufferInfo()


// $Log: ConsoleBuffer.C,v $
// Revision 1.5  2007/03/07 15:05:41  gdavidson
// Code review
//
// Revision 1.4  2007/03/01 20:40:36  geconomos
// code review preparation
//
// Revision 1.3  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.2.2.1  2006/01/16 15:43:05  geconomos
// spelling
//
// Revision 1.2  2005/09/23 14:33:40  geconomos
// improved error handing
//
// Revision 1.1  2005/09/23 12:07:29  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/ConsoleBuffer.C,v 1.5 2007/03/07 15:05:41 gdavidson Exp $
// $Id: ConsoleBuffer.C,v 1.5 2007/03/07 15:05:41 gdavidson Exp $
