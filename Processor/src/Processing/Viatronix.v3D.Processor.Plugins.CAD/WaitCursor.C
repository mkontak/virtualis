// $Id: WaitCursor.C,v 1.3 2007/03/07 16:25:06 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#include "StdAfx.h"
#include "WaitCursor.h"
#include "ConsoleBuffer.h"

// static initialization
HANDLE WaitCursor::m_hThread=NULL;
HANDLE WaitCursor::m_hAbortEvent=NULL;

/**
 * Starts the wait cursor animation
 */
void WaitCursor::Start()
{
  m_hAbortEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

  uint4 uThreadId( 0 );
  m_hThread = reinterpret_cast< HANDLE >( _beginthreadex( NULL, 0, &WaitCursor::ThreadProc, NULL, 0, &uThreadId ) );
} // Start()


/**
 * Stops the wait cursor animation
 */
void WaitCursor::Stop()
{
  // signal that we are aborting
  SetEvent( m_hAbortEvent );

  // wait for the thread to terminate
  WaitForSingleObject( m_hThread, INFINITE );
  
  // clean up handles
  CloseHandle( m_hThread );
  CloseHandle( m_hAbortEvent );

  m_hThread = NULL;
  m_hAbortEvent = NULL;

} // Stop()


/**
 * Animates the cursor until the abort event becomes signaled
 *
 * @param     unused
 * @return    0 for success
 */
unsigned int __stdcall WaitCursor::ThreadProc( void * )
{
  // number of frame in the cursor animation
  const int4 iNumFrames( 8 );
  
  // cursor animation frames
  const char * sFrames[] = { "|", "/", "-", "\\", "|", "/", "-", "\\" };
  
  // current frame index
  int4 iCurrentFrame( 0 );

  // get the current mouse position
  int xpos = ConsoleBuffer::GetScreenBufferInfo().dwCursorPosition.X;
  int ypos = ConsoleBuffer::GetScreenBufferInfo().dwCursorPosition.Y;
  
  // loop at the specified interval and break when the abort event becomes signaled
  DWORD dwReturn( 0 );
  while( dwReturn = WaitForSingleObject( m_hAbortEvent, 100 ) )
  {
    // abort event signaled?
    if( dwReturn == WAIT_OBJECT_0 )
      break;

    // write the current frame
    ConsoleBuffer::Write( sFrames[ (iCurrentFrame++)%iNumFrames ], xpos, ypos );
  }

  // overwrite last frame with a blank
  ConsoleBuffer::Write( " ", xpos, ypos );

  // clean up
  _endthreadex( 0 );

  return 0;
} // ThreadProc( void * )


// $Log: WaitCursor.C,v $
// Revision 1.3  2007/03/07 16:25:06  gdavidson
// Code review
//
// Revision 1.2  2007/03/01 20:40:36  geconomos
// code review preparation
//
// Revision 1.1  2005/09/23 12:07:29  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/WaitCursor.C,v 1.3 2007/03/07 16:25:06 gdavidson Exp $
// $Id: WaitCursor.C,v 1.3 2007/03/07 16:25:06 gdavidson Exp $
