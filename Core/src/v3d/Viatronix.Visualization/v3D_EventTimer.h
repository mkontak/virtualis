// $Id: v3D_EventTimer.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once

// includes
#include "v3D_Interfaces.h"
#include "vxEventTimer.h"
#include "vxShareableObject.h"

// forward declarations
class vxManagedObserver;

// namespaces
OPEN_VISUALIZATION_NS

// class definition
public ref class EventTimer: public IElement, public IObserver
{
// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pEventTimer ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer);

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;


// IObserver implementation
public:

  /// processes modified queue events
  virtual void OnModified( System::IntPtr modifiedList );

// functions
public:
  
  /// initializer
  EventTimer();
  
  /// destructor
  virtual ~EventTimer();

  /// finalizer
  !EventTimer();

  /// reset
  void Reset();

  /// returns the shared transfer function object
  vxShareableObject< vxEventTimer > * GetEventTimerPtr() { return m_pEventTimer; }

  /// sets the elapse time for the specified event
  void SetElapseTime( System::String ^ mpEventName, int iElapse );

  /// returns the elapse time for the specified event
  int GetElapseTime( System::String ^ mpEventName );

  /// starts the specified timer
  void SetTimer( System::String ^ mpEventName, int iElapse );

  /// kills the spcified timer
  void KillTimer( System::String ^ mpEventName );

  /// kill all timer
  void KillAllTimers();

  /// indicates if the event timer is started.
  bool IsStarted( System::String ^ mpEventName );

private:

  /// sets a timer on the window
  void SetTimer( vxEventTimer::EventID eEvent );

  /// kills a timer on the window
  void KillTimer( vxEventTimer::EventID eEvent );

// data
private:

  // shared event timer object
  vxShareableObject< vxEventTimer > * m_pEventTimer;
  
  // observer
  vxManagedObserver * m_pObserver;

  // collection of handles to win with active timers
  std::map< int, std::list< HWND > > * m_pHandles;

}; // EventTimer

CLOSE_VISUALIZATION_NS

// $Log: v3D_EventTimer.h,v $
// Revision 1.4  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2004/10/04 15:10:34  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2004/03/02 01:10:45  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.11.2.1  2004/02/24 20:07:56  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.11  2003/12/16 13:25:51  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.10  2003/09/29 17:25:32  michael
// added a mechanism to kill all ongoing timers
//
// Revision 1.9  2003/09/22 15:52:31  michael
// added derivation from vxManagedObserver so that we can call it modified
//
// Revision 1.8  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.7  2003/05/16 13:10:13  frank
// code formatting
//
// Revision 1.6  2003/05/13 13:28:45  michael
// code review
//
// Revision 1.5  2003/05/06 17:23:31  geconomos
// Coding standards
//
// Revision 1.4  2002/11/08 14:49:33  geconomos
// Added SetTimer and KillTimer
//
// Revision 1.3  2002/11/02 03:11:12  frank
// spelling
//
// Revision 1.2  2002/09/24 17:36:31  geconomos
// Exception handling and formatting.
//
// Revision 1.1  2002/08/05 16:24:56  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_EventTimer.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_EventTimer.h,v 1.4 2006/10/04 18:29:04 gdavidson Exp $