// $Id: vxEventTimer.C,v 1.3 2005/05/23 12:48:20 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)


// includes
#include "stdafx.h"
#include "vxEventTimer.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"
#define DEFAULT_TIME 300


/**
 * Constructor
 */
vxEventTimer::vxEventTimer()
//*******************************************************************
{
  Initialize();
} // constructor


/** 
 * Static member functions wrapping static members
 * @return id
 */
uint4 & vxEventTimer::NextId()
//*******************************************************************
{
  static uint4 nextId(1);

  return nextId;
} // NextId()


/** 
 * Get handles
 * @return handles
 */
std::map<int, std::list<HWND> > & vxEventTimer::GetHandles() 
//*******************************************************************
{ 
  static std::map<int, std::list<HWND> > handlesMap;

  return handlesMap; 
} // GetHandles() 


/** 
 * Event map
 * @return map of events
 */
std::map<vxEventTimer::EventID, std::pair<uint4, vxEventTimer::TimeElapse> > & vxEventTimer::EventMap()
//*******************************************************************
{
  static std::map<EventID, std::pair<uint4, TimeElapse> > eventMap;

  return eventMap;
}


/**
 * Initializes the static members
 */
void vxEventTimer::Initialize()
//*******************************************************************
{
  static bool bIsInitialized(false);

  if (bIsInitialized == false)
  {
    EventMap()["CINE"]         = std::make_pair(NextId()++, DEFAULT_TIME);
    EventMap()["MOMENTUM"]     = std::make_pair(NextId()++, DEFAULT_TIME);

    // create 32 qualityspeed timers (16 possible windows with two renderers each)
    for ( int4 i(0); i<GetMaxNumberQualitySpeedTradoffTimers(); ++i )
    {
      std::string sTimerName = GetQualitySpeedTradeoffTimerName() + ToAscii( i );
      EventMap()[ sTimerName ] = std::make_pair( NextId()++, 100 );
    }

    bIsInitialized = true;
  }
} // Initialize()


/**
 * Sets the cpecified event
 * @param event
 */
void vxEventTimer::SetTimer(vxEventTimer::EventID event) 
//*******************************************************************
{ 
  m_states[event] = STARTED; 
} // SetTimer()


/**
 * Sets the elapse time and the specified timer
 * @param event
 * @param elapsed time
 */
void vxEventTimer::SetTimer(vxEventTimer::EventID event, 
                            vxEventTimer::TimeElapse elapse) 
//*******************************************************************
{ 
  SetElapseTime(event, elapse); 
  SetTimer(event); 
} // SetTimer()


/**
 * Kills the specified event.
 * @param event
 */
void vxEventTimer::KillTimer(vxEventTimer::EventID event) 
//*******************************************************************
{ 
  m_states[event] = KILLED; 
} // KillTimer()


/**
 * Returns the elapse time for the specified event
 * @param event
 * @retrun elapsed time
 */
vxEventTimer::TimeElapse vxEventTimer::GetElapseTime(vxEventTimer::EventID event)
//*******************************************************************
{ 
  std::map<EventID, std::pair<uint4, TimeElapse> >::iterator iter(EventMap().find(event));

  if (iter == EventMap().end())
  {
    throw ex::IllegalArgumentException(LogRec("event not found", "vxEventTimer", "GetElapseTime"));
  }

  return EventMap()[event].second; 
} // GetElapseTime()


/**
 * Sets the elapse time for the cpecified event
 * @param event
 * @param elapsed time
 */
void vxEventTimer::SetElapseTime(vxEventTimer::EventID event, 
                                 vxEventTimer::TimeElapse elapse) 
//*******************************************************************
{ 
  std::map<EventID, std::pair<uint4, TimeElapse> >::iterator iter(EventMap().find(event));

  if (iter == EventMap().end())
  {
    EventMap()[event] = std::make_pair(NextId()++, elapse);
  }
  else
  {
    EventMap()[event].second = elapse; 
  }
} // SetElapseTime()


/**
 * Returns the state of the event for the client window
 * @param event
 * @return state
 */
vxEventTimer::StateEnum vxEventTimer::GetState(vxEventTimer::EventID event)  
//*******************************************************************
{
  std::map<EventID, StateEnum>::iterator iter(m_states.find(event));

  if (iter == m_states.end())
  {
    m_states[event] = KILLED;
  }

  return m_states[event];
} // GetState()


/**
 * Returns the event id from the system id
 * @param id
 * @return event
 */
vxEventTimer::EventID vxEventTimer::GetEventID(uint4 uID)
//*******************************************************************
{
  for (std::map< EventID, std::pair<uint4, TimeElapse> >::iterator iter(EventMap().begin()); iter != EventMap().end(); ++iter)
  {
    if ((*iter).second.first == uID)
    {
      return (*iter).first;
    }
  }

  return "";
} // GetEventID()


/**
 * Returns the system identification number for the specified event
 * @param event
 * @param id
 */
uint4 vxEventTimer::GetSystemID(vxEventTimer::EventID event)
//*******************************************************************
{
  std::map< EventID, std::pair< uint4, TimeElapse > >::iterator iter = EventMap().find(event);
  if (iter == EventMap().end())
  {
    SetElapseTime(event, DEFAULT_TIME);
  }

  return EventMap()[event].first;
} // GetSystemID()


#undef DEFAULT_TIME
#undef FILE_REVISION


// Revision History:
// $Log: vxEventTimer.C,v $
// Revision 1.3  2005/05/23 12:48:20  frank
// fixed file revision definition so that we get correct information in log files
//
// Revision 1.2  2004/03/26 21:16:04  frank
// fixed rendering timers so that two renderers can co-exist in a single environment without stepping on each other with the timers
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/16 15:09:08  michael
// code review
//
// Revision 1.4  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.3  2003/02/18 20:17:15  frank
// Threw an exception if the specified timer does not exist prior to the query.
//
// Revision 1.2  2003/01/06 20:08:06  ingmar
// moved static data members to static member functions
//
// Revision 1.1  2002/08/05 16:19:13  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxEventTimer.C,v 1.3 2005/05/23 12:48:20 frank Exp $
// $Id: vxEventTimer.C,v 1.3 2005/05/23 12:48:20 frank Exp $
