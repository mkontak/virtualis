// $Id: vxInteractingMonitor.h,v 1.5 2007/10/24 20:35:04 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// pragmas
#pragma once


// includes
#include "Timer.h"


// forward declaration
class vxInputAdapter;


/**
 * Monitors the user interacting flag.  This class is used in the input adpater.  Certain
 * known WM_xxx messages cause the interating flag to be set top true (WM_KEYDOWN, WM_MOUSEWHEEL).
 * After a specified delta the flag is set back to false.  Subsequent calls to monitor will reset the
 * timout period.
 */
class vxInteractingMonitor
{
// member functions 
public:
  
  /// constructor
  vxInteractingMonitor();

  /// monitors the specified HWND
  void Monitor( HWND hWnd );    

  /// function pointer for the execute method
  typedef void (__stdcall* ExecuteFuncPtr )( vxInputAdapter * adapter );

  /// monitors the specified HWND
  void Monitor( HWND hWnd, vxInputAdapter * pInputAdapter, ExecuteFuncPtr pFuncPtr );       

private:

  /// constains information about the mouse wheel for a given HWND
  struct vxSizeInfo
  {
    // elapsed time between calls
    Timer m_elapsedTime;

    /// indicates if the timer is running
    bool m_bRunning;

    /// InputAdapter
    vxInputAdapter * m_pInputAdapter;

    /// optional function pointer
    ExecuteFuncPtr m_pFuncPtr;

  };

  /// timer callback
  static void __stdcall TimerProc( HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime );

// member variables
private:
  
  // typedef'd map
  typedef std::map< HWND, vxSizeInfo> vxSizeMap;
  
  /// HWND map
  static vxSizeMap m_map;

}; // class vxInteractingMonitor


// $Log: vxInteractingMonitor.h,v $
// Revision 1.5  2007/10/24 20:35:04  gdavidson
// Restored the function pointer definition
//
// Revision 1.4  2007/08/24 14:36:42  romy
// took the erase out for better performance
//
// Revision 1.3  2007/08/01 15:43:07  gdavidson
// Issue #5738: Added function pointer for custom monitoring of the WM_SIZE message
//
// Revision 1.2  2007/03/05 15:23:36  geconomos
// code review preparation
//
// Revision 1.1  2007/02/05 15:28:33  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/vxInteractingMonitor.h,v 1.5 2007/10/24 20:35:04 gdavidson Exp $
// $Id: vxInteractingMonitor.h,v 1.5 2007/10/24 20:35:04 gdavidson Exp $
