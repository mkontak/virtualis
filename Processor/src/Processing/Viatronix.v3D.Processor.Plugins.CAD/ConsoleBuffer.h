// $Id: ConsoleBuffer.h,v 1.5 2007/03/07 15:05:41 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// pragma declarations
#pragma once

// forward delcarations
class LicenseValidator;

// class declaration
class ConsoleBuffer
{
  /// available console colors
  enum TextColorEnum
  {
    BLACK       = 0,
    RED         = FOREGROUND_RED   | FOREGROUND_INTENSITY,
    GREEN       = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    BLUE        = FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
    YELLOW      = FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    PURPLE      = FOREGROUND_RED   | FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
    CYAN        = FOREGROUND_GREEN | FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
    WHITE       = FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    RED_DIM     = FOREGROUND_RED,
    GREEN_DIM   = FOREGROUND_GREEN,
    BLUE_DIM    = FOREGROUND_BLUE,
    YELLOW_DIM  = FOREGROUND_RED   | FOREGROUND_GREEN,
    PURPLE_DIM  = FOREGROUND_RED   | FOREGROUND_BLUE,
    CYAN_DIM    = FOREGROUND_GREEN | FOREGROUND_BLUE,
    WHITE_DIM   = FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE,
  }; // enum TextColorEnum

// member functions
public:

  /// writes the specified text to the console
  static void Write( const char * sText, const ConsoleBuffer::TextColorEnum & eColor = ConsoleBuffer::WHITE );

  /// writes the supplied text to the console at the specified position
  static void Write( const char * sText, const int4 x, const int4 y, const ConsoleBuffer::TextColorEnum & eColor = ConsoleBuffer::WHITE );

  /// writes the specified text to the console and moves the cursor to the beginning of the next line
  static void WriteLine( const char * sText, const ConsoleBuffer::TextColorEnum & eColor = ConsoleBuffer::WHITE );

  /// writes a command line argument to the console
  static void WriteArgument( const char * sArgumentName, const char * sArgumentValue );
  
  /// writes the necessary text for beginning a step
  static void WriteStepBegin( const char * sText );

  /// writes the necessary text for ending a step
  static void WriteStepEnd( bool bSuccess );

  /// writes out the specified com exception
  static void WriteComException( _com_error & e );

  /// writes out the specified viatronix exception
  static void WriteViatronixException( ex::VException & e );

  /// writes out the unhandled exception
  static void WriteUnhandledException();

  /// positions the cursor at the begining next buffer line
  static void Newline( const int4 iNumberOfLines = 1 );

  /// gets a CONSOLE_SCREEN_BUFFER_INFO for the current state 
  static CONSOLE_SCREEN_BUFFER_INFO & GetScreenBufferInfo();

private:

  /// gets a handle to the screen buffer
  static HANDLE GetScreenBuffer();

  /// updates the CONSOLE_SCREEN_BUFFER_INFO member
  static void UpdateScreenBufferInfo();

}; // class ConsoleBuffer

// $Log: ConsoleBuffer.h,v $
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/ConsoleBuffer.h,v 1.5 2007/03/07 15:05:41 gdavidson Exp $
// $Id: ConsoleBuffer.h,v 1.5 2007/03/07 15:05:41 gdavidson Exp $
