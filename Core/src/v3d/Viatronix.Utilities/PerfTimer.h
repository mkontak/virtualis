// $Id: PerfTimer.h,v 1.2 2007/03/01 19:13:08 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

// pragama declarations
#pragma once

// include declarations
#include "Timer.h"

// namespace declarations
OPEN_UTILITIES_NS

// class declaration
public ref class PerfTimer
{
// member functions
public:
  
  /// constructor
  PerfTimer();
  
  /// destructor
  ~PerfTimer()
  { this->!PerfTimer();}

  /// Finalizer
  !PerfTimer();

  /// reset the timer to zero and activate at the same time
  void Reset();

  /// stop the timer (and freeze the elapsed time)
  void Stop();

  /// continiue the timer from where Stop() had frozen it
  void Continue();

  /// gets the elapsed time 
  property float ElapsedTime
  {  float get(); }

// member variables
private:
  
  /// underlying timer
  Timer * m_pTimer;

}; // class PerfTimer

CLOSE_UTILITIES_NS

// $Log: PerfTimer.h,v $
// Revision 1.2  2007/03/01 19:13:08  geconomos
// code review preparation
//
// Revision 1.1  2006/04/04 14:04:01  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/PerfTimer.h,v 1.2 2007/03/01 19:13:08 geconomos Exp $
// $Id: PerfTimer.h,v 1.2 2007/03/01 19:13:08 geconomos Exp $
