// $Id: IdleManager.h,v 1.2 2007/03/07 14:43:25 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

#pragma once

#include "Timer.h"


class IdleManager
{
// methods
public:

  /// default constructor
  IdleManager();

  /// sets the idle interval
  void SetIdleInterval( float8 iIdleInterval ) { m_iIdleInterval = iIdleInterval; }

  /// resets the internal timer
  void Reset();

  /// locks the application after a specified time of inactivity
  void Monitor();

  /// process windows message
  void ProcessMessage( MSG * pMsg );

// member variables
private:

  /// idle timer
  Timer m_timer;

  /// idle interval
  float8 m_iIdleInterval;

}; // class IdleManager


// $Log: IdleManager.h,v $
// Revision 1.2  2007/03/07 14:43:25  geconomos
// code review
//
// Revision 1.1  2006/12/19 15:35:05  gdavidson
// Issue #5019: Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/IdleManager.h,v 1.2 2007/03/07 14:43:25 geconomos Exp $
// $Id: IdleManager.h,v 1.2 2007/03/07 14:43:25 geconomos Exp $

