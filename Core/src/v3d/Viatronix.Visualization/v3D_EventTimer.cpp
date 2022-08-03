// $Id: v3D_EventTimer.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// includes
#include "StdAfx.h"
#include "v3D_EventTimer.h"
#include "vxManagedObserver.h"

// defines
#define FILE_REVISION "$Revision: 1.4 $"

// namespaces
USING_VISUALIZATION_NS


/**
 * Initializes an EventTimer object with default values.
 */
EventTimer::EventTimer()
{
  m_pEventTimer = new vxShareableObject< vxEventTimer >();
  
  // create and connect observer
  m_pObserver = new vxManagedObserver( this );
  m_pEventTimer->Connect( m_pObserver );

  // get the collection of event ids and window handles from the vx class
  m_pHandles = &m_pEventTimer->GetHandles();
} // EventTimer()


/**
 * Destructor
 */
EventTimer::~EventTimer()
{
  this->!EventTimer();
} // ~EventTimer()


/**
 * Finalizer
 */
EventTimer::!EventTimer()
{
  if( m_pEventTimer != NULL ) 
    m_pEventTimer->Disconnect( m_pObserver );
  
  delete m_pEventTimer; m_pEventTimer = NULL;
  delete m_pObserver;  m_pObserver = NULL;
} // ~EventTimer()


/**
 * Reset
 */
void EventTimer::Reset()
{
  KillAllTimers();
} // ~EventTimer()


/**  
 * Connects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void EventTimer::Connect( System::IntPtr observer )
{
  m_pEventTimer->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/**  
 * Disconnects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void EventTimer::Disconnect( System::IntPtr  observer )
{
  m_pEventTimer->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Sets the specified timer.
 * @param mpTimerName Name of event timer to start.
 * @param iElapse Elapsed time between events.
 */
void EventTimer::SetTimer( System::String ^ mpTimerName, int iElapse )
{
  try
  {
    m_pEventTimer->SetTimer( ss( mpTimerName ), iElapse );
    m_pEventTimer->Modified(vxModInfoTxt( vxEventTimer, vxUtils::EVENTTIMER_STARTED, ss( mpTimerName ) ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SetTimer()


/**
 * Kills the specified event timer.
 * @param mpTimerName Name of event timer to kill.
 */
void EventTimer::KillTimer( System::String ^ mpTimerName )
{
  try
  {
    m_pEventTimer->KillTimer( ss( mpTimerName ) );
    m_pEventTimer->Modified(vxModInfoTxt( vxEventTimer, vxUtils::EVENTTIMER_KILLED, ss( mpTimerName ) ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // KillTimer()


/**
 * Kill all timers
 */
void EventTimer::KillAllTimers()
{
  try
  {
    std::map< int, std::list< HWND > >::iterator iterMap;
    for( iterMap = (*m_pHandles).begin(); iterMap != (*m_pHandles).end(); ++iterMap )
    {
      std::list< HWND >::iterator iterHandles;

      for( iterHandles = iterMap->second.begin(); iterHandles != iterMap->second.end(); ++iterHandles )
      {
        if (*iterHandles == m_pEventTimer->GetHwnd())
        {
          m_pEventTimer->KillTimer(vxEventTimer::GetEventID(iterMap->first));
          ::KillTimer( (*iterHandles), iterMap->first);
        }
      }
    }
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // KillTimer()


/**
 * Indicates if the specified event timer is started.
 * @param mpTimerName Name of event timer to check if started.
 * @return True if timer is started; false otherwise.
 */
bool EventTimer::IsStarted( System::String ^ mpTimerName )
{
  try
  {
    return m_pEventTimer->GetState( ss( mpTimerName ) ) == vxEventTimer::STARTED;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // IsStarted()


/** 
 * Sets the elapse time for the specified event timer.
 * @param mpTimerName Name of event timer to set.
 * @param iElapse Elapsed time between events.
 */
void EventTimer::SetElapseTime( System::String ^ mpTimerName, int iElapse )
{
  try
  {
    vxEventTimer::SetElapseTime( ss( mpTimerName ), Bound(0, iElapse, iElapse) );
    uint4 uID = vxEventTimer::GetSystemID( ss( mpTimerName ) );

    // reset the elapse time on this event for all existing ones
    for ( std::list< HWND >::iterator iter = (*m_pHandles)[ uID ].begin(); iter != (*m_pHandles)[ uID ].end(); ++iter )
    {
      ::KillTimer( (*iter), uID );
      ::SetTimer( (*iter), uID, Bound( 0, iElapse, iElapse ), NULL );
    }
  } // try
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SetElapseTime()


/** 
 * Returns the elapse time for the specified event timer.
 * @param mpTimerName Name of event timer to retrieve elapsed time.
 */
int EventTimer::GetElapseTime( System::String ^ mpTimerName )
{
  try
  {
    return vxEventTimer::GetElapseTime( ss( mpTimerName ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetElapseTime()


/**
 * Sets a timer on the window.
 * @param eEvent Event timer ID
 */
void EventTimer::SetTimer( vxEventTimer::EventID eEvent )
{
  try
  {
    // kill this event if it already exists
    KillTimer( eEvent );

    // set the event on the window
    ::SetTimer( m_pEventTimer->GetHwnd(), m_pEventTimer->GetSystemID( eEvent ), m_pEventTimer->GetElapseTime( eEvent ), NULL );
    (*m_pHandles)[ m_pEventTimer->GetSystemID( eEvent ) ].push_back( m_pEventTimer->GetHwnd() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
  
} // SetTimer()


/**
 * Kills a timer on the window.
 * @param eEvent Event timer ID
 */
void EventTimer::KillTimer( vxEventTimer::EventID eEvent )
{
  try
  {
    uint4 uID = m_pEventTimer->GetSystemID( eEvent );
    HWND hWnd = m_pEventTimer->GetHwnd();

    // kill this event if it already exists
    for ( std::list< HWND >::iterator iter = (*m_pHandles)[ uID ].begin(); iter != (*m_pHandles)[ uID ].end(); ++iter )
    {
      if ( (*iter) == hWnd )
      { 
        ::KillTimer( hWnd, uID );
        (*m_pHandles)[ uID ].erase( iter );
        break;
      }
    }
  } // try
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // KillTimer()


/**
 * Processes modified queue events.
 * @param modifiedList Modified info list
 */
void EventTimer::OnModified( System::IntPtr modifiedList )
{
  vxModifiedInfoList * pInfoList = reinterpret_cast< vxModifiedInfoList * >( modifiedList.ToPointer() );
  
  for ( const vxModifiedInfo * pInfo = pInfoList->Begin(); pInfo != NULL; pInfo = pInfoList->Next() )
  {  
    vxEventTimer::EventID eventID = pInfo->GetText();
    
    switch ( pInfo->GetFlag() )
    {
    case vxUtils::EVENTTIMER_STARTED:
      SetTimer( eventID );
      break;

    case vxUtils::EVENTTIMER_KILLED: 
      KillTimer( eventID );
      break;
    }
  } // for all modified informations
} // OnModified()


#undef FILE_REVISION


// $Log: v3D_EventTimer.cpp,v $
// Revision 1.4  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2004/03/02 01:10:45  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.15.2.1  2004/02/24 20:07:56  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.15  2003/12/16 13:25:51  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.14  2003/10/03 12:12:57  michael
// took out log messages that were used for debugging
//
// Revision 1.13  2003/09/29 17:25:32  michael
// added a mechanism to kill all ongoing timers
//
// Revision 1.12  2003/09/22 15:52:31  michael
// added derivation from vxManagedObserver so that we can call it modified
//
// Revision 1.11  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.10  2003/07/24 13:04:51  michael
// cosmetics (fixed wrong comment)
//
// Revision 1.9  2003/05/16 13:10:13  frank
// code formatting
//
// Revision 1.8  2003/05/13 13:28:45  michael
// code review
//
// Revision 1.7  2003/05/06 17:23:31  geconomos
// Coding standards
//
// Revision 1.6  2003/01/22 22:08:56  ingmar
// fixed compiler warming of using this in member initializer
//
// Revision 1.5  2002/11/25 18:34:20  ingmar
// added FILE_REVISION define
//
// Revision 1.4  2002/11/08 14:49:33  geconomos
// Added SetTimer and KillTimer
//
// Revision 1.3  2002/09/24 18:17:54  geconomos
// Exception handling and formatting.
//
// Revision 1.2  2002/09/24 17:36:31  geconomos
// Exception handling and formatting.
//
// Revision 1.1  2002/08/05 16:24:56  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_EventTimer.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_EventTimer.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $