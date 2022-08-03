// $Id: vxVisualizationGraphTimer.C,v 1.5 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#include "StdAfx.h"
#include "vxVisualizationGraphTimer.h"
#include "vxVisualizationGraph.h"
#include "Timer.h"


#define FILE_REVISION "$Revision: 1.5 $"

// static initialization
uint4 vxVisualizationGraphTimer::m_uTimerId = 65481;

/**
 * constructor
 *
 * @param         visualizationGraph          VisualizationGraph reference
 */
vxVisualizationGraphTimer::vxVisualizationGraphTimer( vxVisualizationGraph & visualizationGraph ) :
  m_visualizationGraph( visualizationGraph ),
  m_iInterval( 50 ),
  m_bIsRunning( false )
{
    HWND hWnd = Create( NULL, 0, 0, WS_OVERLAPPEDWINDOW );
} // vxVisualizationGraphTimer( visualizationGraph )


/**
 * destructor
 */
vxVisualizationGraphTimer::~vxVisualizationGraphTimer()
{
  DestroyWindow();
} // ~vxVisualizationGraphTimer()


/**
 * Starts the timer
 */
void vxVisualizationGraphTimer::Start()
{
  Stop();

  // start the windows timer
  UINT_PTR retVal = SetTimer( m_uTimerId, m_iInterval, NULL );

  m_bIsRunning = true;
} // Start()


/**
 * Stops the timer
 */
void vxVisualizationGraphTimer::Stop()
{
  // stop the timer
  KillTimer( m_uTimerId );

  m_bIsRunning = false;
} // Stop()


/**
 * Handles the timer event
 */
LRESULT vxVisualizationGraphTimer::OnTimer( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  m_visualizationGraph.Run();

  return 0;
} // OnTimerCallback( hWnd, uMsg, idEvent, dwTime )


/**
 * Handles the destroy event
 */
LRESULT vxVisualizationGraphTimer::OnDestroy( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  // stop the timer
  KillTimer( m_uTimerId );

  return 0;
} // OnTimerCallback( hWnd, uMsg, idEvent, dwTime )

  
// $Log: vxVisualizationGraphTimer.C,v $
// Revision 1.5  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.4  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.3  2007/01/24 14:41:31  gdavidson
// Added OnDestroy method
//
// Revision 1.2  2007/01/17 16:41:37  geconomos
// added IsRunning
//
// Revision 1.1  2007/01/16 16:27:12  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxVisualizationGraphTimer.C,v 1.5 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxVisualizationGraphTimer.C,v 1.5 2007/03/07 17:04:39 geconomos Exp $

