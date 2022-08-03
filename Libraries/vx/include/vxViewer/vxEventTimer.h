// $Id: vxEventTimer.h,v 1.2 2004/03/26 21:16:04 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)

#ifndef vxEventTimer_h
#define vxEventTimer_h


// includes
#include <map>


// class definition
class VX_VIEWER_DLL vxEventTimer
{
// typedefs
public:

  /// event for unique identification
  typedef std::string EventID;

  /// event for elapse time
  typedef uint4 TimeElapse;
  
// enums
public:
  
  /// states enumeration
  enum StateEnum
  {
    KILLED  = 0,
    STARTED = 1,
  };

// functions
public:

  /// constructor
  vxEventTimer();

  /// destructor
  virtual ~vxEventTimer(){};

  /// initializes the static members
  static void Initialize();

  /// returns the handle to the client window
  HWND GetHwnd() const { return m_hwnd; }

  /// sets the handle for the client window
  void SetHwnd(HWND hwnd) { m_hwnd = hwnd; }

  /// sets the cpecified event
  void SetTimer(EventID event);

  /// sets the elapse time and the specified timer
  void SetTimer(EventID event, TimeElapse elapse);

  /// kills the specified event
  void KillTimer(EventID event);

  /// returns the elapse time for the specified event
  static TimeElapse GetElapseTime(EventID event);

  /// sets the elapse time for the cpecified event
  static void SetElapseTime(EventID event, TimeElapse elapse);

  /// returns the state of the event for the client window
  StateEnum GetState(EventID event);

  /// returns the event id from the system id
  static EventID GetEventID(uint4 uID);

  /// returns the system identification number for the specified event
  static uint4 GetSystemID(EventID event);

  /// returns the collection of event ids and window handles
  static std::map<int, std::list<HWND> > & GetHandles();

  /// prefix for quality-speed tradeoff timers (hack until we have more flexible timers)
  static std::string GetQualitySpeedTradeoffTimerName() { return "QUALITYSPEED"; }

  /// the maximum number of quality-speed tradeoff timers (hack until we have more flexible timers)
  static uint4 GetMaxNumberQualitySpeedTradoffTimers() { return 32; }

// data
private:

  /// next id value
  static uint4 & NextId();

  /// elapse time for all of the events
  static std::map< EventID, std::pair< uint4, TimeElapse > > & EventMap();

  /// event states for the client window
  std::map<EventID, StateEnum> m_states;

  /// handle to the client window
  HWND m_hwnd;
}; // vxEventTimer


#endif // vxEventTimer_h


// Revision History:
// $Log: vxEventTimer.h,v $
// Revision 1.2  2004/03/26 21:16:04  frank
// fixed rendering timers so that two renderers can co-exist in a single environment without stepping on each other with the timers
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/08/13 12:43:42  michael
// added derivation for 70_vxUtils
//
// Revision 1.4  2003/05/16 15:09:08  michael
// code review
//
// Revision 1.3  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.2  2003/01/06 20:08:06  ingmar
// moved static data members to static member functions
//
// Revision 1.1  2002/08/05 16:19:13  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxEventTimer.h,v 1.2 2004/03/26 21:16:04 frank Exp $
// $Id: vxEventTimer.h,v 1.2 2004/03/26 21:16:04 frank Exp $
