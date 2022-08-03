// $Id: WaitCursor.h,v 1.2 2007/03/07 16:25:06 gdavidson Exp $
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

// forward declarations
class ConsoleBuffer;

// class declaration
class WaitCursor
{
// member functions
public:

  /// starts the wait cursor animation
  static void Start();

  /// stops the wait cirsor animation
  static void Stop();

// private implementation
private:

  /// animates the cursor until the abort event becomes signaled
  static unsigned int __stdcall ThreadProc( void * );

// member variables
private:

  /// thread in which animation occurs
  static HANDLE m_hThread;

  /// used to abort thread
  static HANDLE m_hAbortEvent;
}; // class Wait Cursor


// $Log: WaitCursor.h,v $
// Revision 1.2  2007/03/07 16:25:06  gdavidson
// Code review
//
// Revision 1.1  2005/09/23 12:07:29  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/WaitCursor.h,v 1.2 2007/03/07 16:25:06 gdavidson Exp $
// $Id: WaitCursor.h,v 1.2 2007/03/07 16:25:06 gdavidson Exp $
