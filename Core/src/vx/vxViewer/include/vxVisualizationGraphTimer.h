// $Id: vxVisualizationGraphTimer.h,v 1.4 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#pragma once


// includes
#include <atlbase.h>
#include <atlwin.h>
#include "Timer.h"


// forward declarations
class vxVisualizationGraph;


class vxVisualizationGraphTimer : public CWindowImpl<vxVisualizationGraphTimer>
{
public:

  BEGIN_MSG_MAP( vxVisualizationGraphTimer )
  MESSAGE_HANDLER( WM_TIMER, OnTimer ) 
  MESSAGE_HANDLER( WM_DESTROY, OnDestroy ) 
  END_MSG_MAP() 

  /// constructor
  vxVisualizationGraphTimer( vxVisualizationGraph & visualizationGraph );

  /// destructor
  virtual ~vxVisualizationGraphTimer();

  /// start method
  void Start();

  /// stop method
  void Stop();

  /// Gets the timer interval
  inline int4 GetInterval() { return m_iInterval; }

  /// Sets the timer interval
  inline void SetInterval( int4 iInterval ) { m_iInterval = iInterval; }

  /// gets if the timer is running
  inline bool IsRunning() { return m_bIsRunning; }

private:

  /// timer callback function
  LRESULT OnTimer( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

  /// destroy callback function
  LRESULT OnDestroy( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

private:

  /// timer
  Timer m_timer;

  /// reference to a visualization graph
  vxVisualizationGraph & m_visualizationGraph;

  /// timer interval
  int4 m_iInterval;

  /// indicates if the timer is running
  bool m_bIsRunning;

  /// timer id
  static uint4 m_uTimerId;

}; // class vxVisualizationGraphTimer


// $Log: vxVisualizationGraphTimer.h,v $
// Revision 1.4  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.3  2007/01/24 14:41:31  gdavidson
// Added OnDestroy method
//
// Revision 1.2  2007/01/17 16:41:37  geconomos
// added IsRunning
//
// Revision 1.1  2007/01/16 16:50:25  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxVisualizationGraphTimer.h,v 1.4 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxVisualizationGraphTimer.h,v 1.4 2007/03/07 17:04:39 geconomos Exp $
