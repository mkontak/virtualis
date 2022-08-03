// $Id: vxInputAdapter.h,v 1.7 2007/10/24 20:35:24 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

/*
  This class is responsible for adapting from the messaging system of 
  windows to the interface of vxInputs.
*/

//pragmas
#pragma once


// forward declarations
class vxViewer;


// includes
#include "vxInput.h"
#include "vxInteractingMonitor.h"


// class definition
class vxInputAdapter
{
// functions
public:
	
  /// constructor
  vxInputAdapter();
	
  /// desturctor
  virtual ~vxInputAdapter();

  /// adds a viewer to the adapter
  void AddViewer( vxViewer * pViewer );

  /// removes a viewer from the adapter
  void RemoveViewer( vxViewer * pViewer );

  /// removes all viewers from the adapter
  void RemoveAllViewers();

  /// adapts a window's message
  bool AdaptMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

  /// check the process virtual memory condition and warn the user if it's low
  static void CheckMemory();

private:

  /// converts a low-order word to an Input state enum
  uint4 CreateState( uint4 uState );

  /// conerts a window message to a vxInput::ButtonEnum
  vxInput::ButtonEnum ToButton( uint4 uMessage );

  /// marks the WinRect in all the viewer's environment's dirty
  static void __stdcall UpdateWinRect( vxInputAdapter * pAdapter );

// data
private:

  // type for Viewer collection type
  typedef std::list< vxViewer *> Viewers;
  
  // viewers
  Viewers m_viewers;

  // viewer that has captured the mouse input
  Viewers::iterator m_captured;

  // captured mouse button message id
  int4 m_iMsg;

  /// "user interacting" monitor
  vxInteractingMonitor m_monitor;

  // the viewer that will recieve input in the current state
  static vxViewer * m_pActiveViewer;

  // indicates if the timer has been set
  bool m_bTimerSet;
  
  // stores if the user user was interacting during the last message processed
  bool m_bLastInteracting;

  /// if we have warned the user about the memory once this execution
  static bool m_bUserWarnedMemory;

}; // vxInputAdapter  


// $Log: vxInputAdapter.h,v $
// Revision 1.7  2007/10/24 20:35:24  gdavidson
// Added back UpdateWinRect
//
// Revision 1.6  2007/08/24 14:35:59  romy
// Modifying the bit for all viewers in multiple times is not necessary
//
// Revision 1.5  2007/08/01 15:43:07  gdavidson
// Issue #5738: Added function pointer for custom monitoring of the WM_SIZE message
//
// Revision 1.4  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.3  2006/12/19 16:46:44  gdavidson
// Issue #5188: Added vxSizeMonitor for WM_SIZE messages
//
// Revision 1.2  2006/11/11 03:10:56  geconomos
// added mouse wheel monitor
//
// Revision 1.1  2006/09/27 17:06:56  geconomos
// using vx naming convention for unmanaged classes
//
// Revision 1.2  2006/06/27 20:18:49  geconomos
// added flag a check to see  if the "user interacting" flag has changed since the last message processed
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2004/02/05 13:34:47  frank
// generalized memory checking
//
// Revision 1.11  2004/02/02 16:31:57  frank
// typo in comments
//
// Revision 1.10  2004/01/23 18:23:31  frank
// working on warning to user when the memory gets low
//
// Revision 1.9  2003/08/20 14:31:11  michael
// added "mailto:"
//
// Revision 1.8  2003/05/16 13:11:04  frank
// formatting
//
// Revision 1.7  2003/05/13 14:05:22  geconomos
// Fixed up after code walkthru.
//
// Revision 1.6  2003/05/13 13:28:45  michael
// code review
//
// Revision 1.5  2003/05/06 17:56:58  geconomos
// Coding standards
//
// Revision 1.4  2002/10/02 13:52:01  dmitry
// msg saved to member var on button down
//
// Revision 1.3  2002/09/25 16:59:48  geconomos
// Imporved message handling.
//
// Revision 1.2  2002/07/02 20:40:42  geconomos
// Wrapping vxBase with managed wrappers.
//
// Revision 1.1  2002/06/21 21:13:59  geconomos
// Initial revision.
//
// Revision 1.8  2002/06/18 21:23:52  michael
// changed flushing of input queue to flush right after getting the event and
// BEFORE processing it instead of after processing it. Has better response.
//
// Revision 1.7  2002/05/17 23:36:07  dmitry
// EventTimer used
//
// Revision 1.6  2002/05/06 21:46:50  dmitry
// Timer based on enum implemented.
//
// Revision 1.5  2002/04/25 18:54:55  uday
// Made active window static (Dmitry)
//
// Revision 1.4  2002/04/24 21:28:13  dmitry
// timer for mouse leave added
//
// Revision 1.3  2002/04/23 20:48:53  dmitry
// MouseEnter and MouseLeave called.
//
// Revision 1.2  2002/04/04 07:58:02  geconomos
// Changed RouteMessage() to AdaptMessage().
//
// Revision 1.1  2002/04/04 07:35:31  geconomos
// renamed v3D_MessageRouter to v3D_InputAdapter.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/vxInputAdapter.h,v 1.7 2007/10/24 20:35:24 gdavidson Exp $
// $Id: vxInputAdapter.h,v 1.7 2007/10/24 20:35:24 gdavidson Exp $
