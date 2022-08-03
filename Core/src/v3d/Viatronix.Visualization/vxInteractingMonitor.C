// $Id: vxInteractingMonitor.C,v 1.7 2007/10/29 20:35:31 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxInteractingMonitor.h"
#include "vxInputAdapter.h"


// static initialization
vxInteractingMonitor::vxSizeMap vxInteractingMonitor::m_map;


/**
 * Constructor
 */
vxInteractingMonitor::vxInteractingMonitor()
{
} // vxMouseWheelMonitor()


/**
 * Monitors the specified hwnd
 *
 * @param   hWnd            HWND to monitor 
 */
void vxInteractingMonitor::Monitor( HWND hWnd )
{
  Monitor( hWnd, NULL, NULL );
} // Monitor( HWND hWnd )


/**
 * Monitors the specified hwnd
 *
 * @param   hWnd            HWND to monitor 
 * @param   pInputAdapter   input adapter
 * @param   pFuncPtr        ExecuteFuncPtr for custom monitoring
 */
void vxInteractingMonitor::Monitor( HWND hWnd, vxInputAdapter * pInputAdapter, ExecuteFuncPtr pFuncPtr )
{
  // check if the hwnd is already in the map, add it if it isn't
  // NOTE: subsequent calls for the same HWND will be ingnored until the original is processed
  vxSizeMap::iterator iter = m_map.find( hWnd );
  if( iter == m_map.end() )
  {
    vxSizeInfo newinfo;
    newinfo.m_bRunning = false;
    m_map.insert( std::make_pair( hWnd, newinfo ) );
  }

  // get the info for the hwnd
  vxSizeInfo & info = m_map[ hWnd ];

  info.m_pFuncPtr = pFuncPtr;
  info.m_pInputAdapter = pInputAdapter;
  info.m_elapsedTime.Reset();
  
  if( !info.m_bRunning )
  {
    // otherwise indicate the the user just started interacting
    vxEnvironment::SetUserInteracting( true );
    
    /// indicate were running
    info.m_bRunning = true;
    
    /// start the windows timer
    SetTimer( hWnd, 0, 50, vxInteractingMonitor::TimerProc );
  }
} // Monitor( HWND hWnd )


/**
 * Timer callback funtions
 *
 * @param   hWnd        HWND asspciated woth timer
 * @param   uMsg        WM_TIMER message
 * @param   idEvent     timer id
 * @param   dwTime      time since system clock was started
 */
void __stdcall vxInteractingMonitor::TimerProc( HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
  vxSizeInfo & info = m_map[ hWnd ];

  // time to wait before issuing a flush
  const float4 fTimeout = 0.10F;
  
  // get the elasped time since the last call
  const float4 fElapsedTime = info.m_elapsedTime.ElapsedTime();
  
  // are we over the time
  if( fElapsedTime > fTimeout )
  {
    // stop the timer
    KillTimer( hWnd, 0);

    // indicate were finished
    info.m_bRunning = false;
       
    // set the user as interacting
    vxEnvironment::SetUserInteracting( false );

    // call provided function pointer
    if( info.m_pFuncPtr != NULL )
      info.m_pFuncPtr( info.m_pInputAdapter );

    // get things rolling
    vxModifiedQueue::Flush();
  }
} // TimerProc( HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )


// $Log: vxInteractingMonitor.C,v $
// Revision 1.7  2007/10/29 20:35:31  gdavidson
// Issue #5858: reset the vxSizeInfo objects when initializing them
//
// Revision 1.6  2007/10/24 20:34:54  gdavidson
// Restored the function pointer definition
//
// Revision 1.4  2007/08/15 21:27:09  gdavidson
// Call function ptr before removing info struct from map
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/vxInteractingMonitor.C,v 1.7 2007/10/29 20:35:31 gdavidson Exp $
// $Id: vxInteractingMonitor.C,v 1.7 2007/10/29 20:35:31 gdavidson Exp $
