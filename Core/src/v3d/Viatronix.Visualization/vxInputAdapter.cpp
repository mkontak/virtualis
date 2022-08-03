// $Id: vxInputAdapter.cpp,v 1.15.2.1 2009/02/05 15:35:35 kchalupa Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// includes
#include "stdafx.h"
#include "vxInputAdapter.h"
#include "vxViewer.h"
#include "vxEnvironment.h"
#include "vxWinRect.h"
#include "vxIRenderingContext.h"
#include "Logger.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.15.2.1 $"
#ifdef MessageBox
#undef MessageBox
#endif


// static member initialization
vxViewer * vxInputAdapter::m_pActiveViewer = NULL;
bool vxInputAdapter::m_bUserWarnedMemory( false );


/**
 * Constructor
 */
vxInputAdapter::vxInputAdapter()
{
  m_captured  = m_viewers.end();
  m_bTimerSet = false;
  m_iMsg = -1;
  m_bLastInteracting = false;
} // vxInputAdapter()


/**
 * Destructor
 */
vxInputAdapter::~vxInputAdapter()
{
} // ~vxInputAdapter()


/**
 * Adds a viewer to the adapter
 * @param pViewer Viewer to add.
 */
void vxInputAdapter::AddViewer( vxViewer * pViewer )
{
  m_viewers.push_front( pViewer );
} // AddViewer()


/**
 * Removes a viewer from the adapter.
 * @param pViewer Viewer to remove.
 */
void vxInputAdapter::RemoveViewer( vxViewer * pViewer )
{
  Viewers::iterator iter = find( m_viewers.begin(), m_viewers.end(), pViewer );
  if ( iter != m_viewers.end() )
    m_viewers.erase( iter );
} // RemoveViewer()


/**
 * Removes all viewers from the adapter.
 */
void vxInputAdapter::RemoveAllViewers()
{
  m_viewers.clear();
} // RemoveAllViewers()


/**
 * Routes a message to a viewer.
 *
 * @param uMsg Message ID of mesage.
 * @param wParam WPARAM value for message.
 * @param lParam LPARAM vlaue for message.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
bool vxInputAdapter::AdaptMessage( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  bool bHandled( false );
  bool bTest( false );
  bool bFlushQueue( false );
  std::string sMessageName;

  // if the manipulators have entered us into a modal state bail out
  if ( vxInput::GetModal() )
  {
    return false;
  }

  try
  {
    switch( uMsg )
    {

//---------------------------------------------------------------------------------------
// WM_SIZE handler
//---------------------------------------------------------------------------------------

    case WM_SIZE:  
    {
      sMessageName = "WM_SIZE";
      const int4 iWidth = LOWORD( lParam );
      const int4 iHeight = HIWORD(lParam );

      if( iWidth == 0 || iHeight == 0 )
        break;

      m_monitor.Monitor( hwnd, this, &vxInputAdapter::UpdateWinRect );

      // for each viewer
      for (Viewers::iterator iter = m_viewers.begin(); iter != m_viewers.end(); ++iter)
      {
        vxViewer * pViewer(*iter);      
        vxWinRect winRect(pViewer->GetWinRect());       
        winRect.SetWindowSize( make_pair( iWidth, iHeight ) );
				pViewer->Resize( winRect );
      } // for each viewer

      //bHandled = true;
      break;
    }

//---------------------------------------------------------------------------------------
// WM_MOUSEWHEEL handler
//---------------------------------------------------------------------------------------

    case WM_MOUSEWHEEL:
    {
      CheckMemory();
      sMessageName = "WM_MOUSEWHEEL";
      
      // WM_MOUSEWHEEL lParam is based on the screen coords, while WM_MOUSEMOVE is based on the client's coords.
      // Need to translate the coordinates from global (frame buffer) to local (client) first.
      POINT pt = { LOWORD( lParam ), HIWORD( lParam ) };
      ScreenToClient(hwnd, &pt);    
      Point2D<int2> pos(pt.x, pt.y);

      // accumulate the mouse wheel delta values
      long iAccumulatedMotion = HIWORD( wParam );
      MSG msg;
      bool bWaitingForNonWheelRelatedMessage( true );
      while ( bWaitingForNonWheelRelatedMessage )
      {
        if ( PeekMessage(&msg, hwnd, WM_MOUSEWHEEL, WM_MOUSEWHEEL, PM_REMOVE) )
          iAccumulatedMotion += HIWORD( msg.wParam );
        else
          bWaitingForNonWheelRelatedMessage = false;
      }
      PeekMessage( &msg, hwnd, 0, 0, PM_NOREMOVE );

      m_monitor.Monitor( hwnd );
      for( Viewers::iterator iter = m_viewers.begin(); iter != m_viewers.end(); ++iter )
      {
        vxViewer * pViewer(*iter);
        
        if ( pViewer->IsEnabled() && pViewer->HitTest(pos) )
        {
          pViewer->GetEnvironment().GetContext()->MakeCurrent();
          pViewer->OnMouseWheel( pos, iAccumulatedMotion, CreateState( LOWORD( wParam ) ) );
          break;
        }
      }
      bHandled = true;
      bFlushQueue = true;
      break;
    }
  
//---------------------------------------------------------------------------------------
// WM_MOUSEMOVE handler
//---------------------------------------------------------------------------------------

    case WM_MOUSEMOVE:
    {  
      sMessageName = "WM_MOUSEMOVE";
      
      Point2D<int2> pos( LOWORD( lParam ), HIWORD( lParam ) );
    
      // if the mouse has been captured route the message to the viewer that captured it
      if ( m_captured != m_viewers.end() )
      {
        (*m_captured)->GetEnvironment().GetContext()->MakeCurrent();
        (*m_captured)->OnMouseMove( pos, CreateState( LOWORD( wParam ) ) );
      }
      else
      {        
        for ( Viewers::iterator iter = m_viewers.begin(); iter != m_viewers.end(); ++iter )
        {
          vxViewer * pViewer(*iter);

          if ( pViewer->IsEnabled() && pViewer->HitTest( pos ) && !vxInput::GetModal())
          {
            // current view different from active view
            if ( pViewer != m_pActiveViewer )
            {
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof( TRACKMOUSEEVENT );
                tme.hwndTrack = hwnd;
                tme.dwFlags = TME_LEAVE;
                TrackMouseEvent( &tme );

              // leave active view
              if ( m_pActiveViewer != NULL ) 
              {
                m_pActiveViewer->GetEnvironment().GetContext()->MakeCurrent();
                m_pActiveViewer->OnMouseLeave(); 
              }
              
              // set active view to current view
              m_pActiveViewer = pViewer;
              m_pActiveViewer->GetEnvironment().GetContext()->MakeCurrent();
              m_pActiveViewer->OnMouseEnter();
            } 

            pViewer->GetEnvironment().GetContext()->MakeCurrent();
            pViewer->OnMouseMove(pos, CreateState(LOWORD(wParam)));
            break;
          }
        }
      }
      bHandled = true;

      MSG msg;
      while( PeekMessage(&msg, hwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE) );
      break;
    }
    
//---------------------------------------------------------------------------------------
// WM_MOUSELEAVE handler
//---------------------------------------------------------------------------------------
    case WM_MOUSELEAVE:
    {
      sMessageName = "WM_MOUSELEAVE";
      if ( m_pActiveViewer != NULL )
      {
        m_pActiveViewer->OnMouseLeave();
        m_pActiveViewer = NULL;
      }
      bHandled = true;
      break;
    }

//---------------------------------------------------------------------------------------
// WM_LBUTTONDOWN, WM_MBUTTONDOWN, WM_RBUTTONDOWN handler
//---------------------------------------------------------------------------------------
    
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    {
      CheckMemory();
      sMessageName = "WM_LBUTTONDOWN||WM_MBUTTONDOWN||WM_RBUTTONDOWN";
      
      Point2D<int2> pos(LOWORD(lParam), HIWORD(lParam));

      if ( m_captured == m_viewers.end() )
      for (Viewers::iterator iter = m_viewers.begin(); iter != m_viewers.end(); ++iter)
      {
        vxViewer * pViewer(*iter);
        
        if ( pViewer->IsEnabled() && pViewer->HitTest( pos ) && !vxInput::GetModal())
        {
          SetCapture( hwnd );
          m_captured = iter;
          m_iMsg = uMsg;
          
          pViewer->GetEnvironment().GetContext()->MakeCurrent();
          pViewer->OnButtonDown(pos, ToButton( uMsg ), CreateState( LOWORD( wParam ))); 
          break;
        }
      }
      bHandled = true;
      break;
    }  
  
//---------------------------------------------------------------------------------------
// WM_LBUTTONUP, WM_MBUTTONUP, WM_RBUTTONUP handler
//---------------------------------------------------------------------------------------

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    {
      CheckMemory();
      sMessageName = "WM_LBUTTONUP||WM_MBUTTONUP||WM_RBUTTONUP";

      Point2D<int2> pos( LOWORD( lParam ), HIWORD( lParam ) );

      if ( m_captured != m_viewers.end() && !vxInput::GetModal() && m_iMsg + 1 == uMsg )
      { 
        ReleaseCapture();
        
        (*m_captured)->GetEnvironment().GetContext()->MakeCurrent();
        (*m_captured)->OnButtonUp(pos, ToButton( uMsg ), CreateState(LOWORD(wParam)));
        
        m_captured = m_viewers.end();
      }  
     
      bHandled = true;
      break;
    }  

//---------------------------------------------------------------------------------------
// WM_LBUTTONDBLCLK, WM_MBUTTONDBLCLK, WM_RBUTTONDBLCLK handler
//---------------------------------------------------------------------------------------
  
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    {
      CheckMemory();
      sMessageName = "WM_LBUTTONDBLCLK||WM_MBUTTONDBLCLK||WM_RBUTTONDBLCLK";
      
      Point2D<int2> pos(LOWORD(lParam), HIWORD(lParam));
      for (Viewers::iterator iter = m_viewers.begin(); iter != m_viewers.end(); ++iter)
      {
        vxViewer * pViewer(*iter);
        if ( pViewer->IsEnabled() && pViewer->HitTest( pos ) )
        {
          pViewer->GetEnvironment().GetContext()->MakeCurrent();
          pViewer->OnDoubleClick( pos, ToButton( uMsg ), CreateState( LOWORD( wParam ) ) );
          break;
        }
      }
      bHandled = true;
      break;
    }  
  
//---------------------------------------------------------------------------------------
// WM_KEYDOWN handler
//---------------------------------------------------------------------------------------
  
    case WM_KEYDOWN:
    {
      CheckMemory();

      sMessageName = "WM_KEYDOWN";
      
      // get the current mouse posion in screen coordinates
      DWORD dwPos = GetMessagePos();
      
      // convert to client coordinates
      POINT ptPos;
      ptPos.x = LOWORD( dwPos );
      ptPos.y = HIWORD( dwPos );      
      ScreenToClient( hwnd, &ptPos );
      
      Point2D<int2> pos( ptPos.x, ptPos.y );
      for( Viewers::iterator iter = m_viewers.begin(); iter != m_viewers.end(); ++iter )
      {
        vxViewer * pViewer(*iter);

        if( pViewer->IsEnabled() && pViewer->HitTest( pos ) )
        {
          m_monitor.Monitor( hwnd );
          pViewer->GetEnvironment().GetContext()->MakeCurrent();
          pViewer->OnKeyPressed( wParam, CreateState(0) );
          bHandled = true;
          break;
        }
      }
      
      break;
    } // WM_KEYDOWN

//---------------------------------------------------------------------------------------
// WM_KEYUP handler
//---------------------------------------------------------------------------------------

  case WM_KEYUP:
    {
      CheckMemory();
      sMessageName = "WM_KEYUP";
      if ( m_pActiveViewer != NULL && m_pActiveViewer->IsEnabled() )
      {
        m_pActiveViewer->GetEnvironment().GetContext()->MakeCurrent();
        m_pActiveViewer->OnKeyReleased( wParam, CreateState(0) );
      }

      bHandled = true;
      break;
    } // WM_KEYUP

//---------------------------------------------------------------------------------------
// WM_SETCURSOR handler
//---------------------------------------------------------------------------------------
    case WM_SETCURSOR:
    {
      sMessageName = "WM_SETCURSOR";
      DWORD messagePos = GetMessagePos();

      POINT pt = {  LOWORD( messagePos ), HIWORD( messagePos )  };
      ScreenToClient(hwnd, &pt);    
      Point2D<int2> pos(pt.x, pt.y);
 
      for ( Viewers::iterator iter(m_viewers.begin()); iter != m_viewers.end(); ++iter )
      {
        vxViewer * pViewer(*iter);
        if ( pViewer->IsEnabled() && pViewer->HitTest( pos ) )
        {
          pViewer->GetEnvironment().GetContext()->MakeCurrent();
          bTest |= pViewer->OnSetCursor();
          break;
        }
      }
      break;
    }
//---------------------------------------------------------------------------------------
    } // end switch(uMsg)

    if ( bHandled == true )
      vxModifiedQueue::Flush();

  } // end try
  catch( ex::OutOfMemoryException & e )
  {
    if (m_captured != m_viewers.end())
    {
      ReleaseCapture();
      m_captured = m_viewers.end();
    }
    LogDbg( "Memory has been exhausted!", "vxInputAdapter", "AdaptMessage" );
    CheckMemory();
    throw e;
  }
  catch( ex::VException & e )
  {
    if (m_captured != m_viewers.end())
    {
      ReleaseCapture();
      m_captured = m_viewers.end();
    }
    throw e;
  }
  catch (...)
  {
    if (m_captured != m_viewers.end())
    {
      ReleaseCapture();
      m_captured = m_viewers.end();
    }
    
    std::string sLog("Unhandled exception in vxInputAdapter::AdaptMessage\nMessage Adapted: ");
    sLog += sMessageName;
    throw ex::VException( LogRec( sLog, "v3D_InputAdpater", "AdpatMessage"));    
  }
  return bHandled || bTest;
} // AdaptMessage()


/**
 * Converts a WM_XXX button message to a vxInput::ButtonEnum.
 * @param uMessage Window's message identifier.
 * @return vxInput::ButtonEnum
 */
vxInput::ButtonEnum vxInputAdapter::ToButton( uint4 uMessage )
{
  vxInput::ButtonEnum eButton(vxInput::BUTTON_LEFT);
  switch( uMessage )
  {
  case WM_LBUTTONUP:
  case WM_LBUTTONDOWN:
  case WM_LBUTTONDBLCLK:
    eButton = vxInput::BUTTON_LEFT;
    break;
  
  case WM_RBUTTONUP:
  case WM_RBUTTONDOWN:
  case WM_RBUTTONDBLCLK:
    eButton = vxInput::BUTTON_RIGHT;
    break;

  case WM_MBUTTONUP:
  case WM_MBUTTONDOWN:
  case WM_MBUTTONDBLCLK:
    eButton = vxInput::BUTTON_MIDDLE;
    break;
  
  default:
    LogErr( "Invalid Message ID", "ToButton", "v3D_InputAdapter" );
    break;
  }

  return eButton;
} // ToButton()


/**
 * Converts a low-order word to an Input state enum.
 * @param uState Bit state retrieved from windows PARAM.
 * @return Input state bit state.
 */
uint4 vxInputAdapter::CreateState( uint4 uState )
{
  uint4 uNewState( 0 );
  
  if (GetAsyncKeyState(VK_MENU) & 0x8000 || uState & MK_ALT)
    uNewState |= vxInput::STATE_ALT;
  
  if (GetAsyncKeyState(VK_CONTROL) & 0x8000 || uState & MK_CONTROL)
    uNewState |= vxInput::STATE_CTRL;
  
  if (GetAsyncKeyState(VK_SHIFT) & 0x8000 || uState & MK_SHIFT)
    uNewState |= vxInput::STATE_SHIFT;
  
  if (uState & MK_LBUTTON)
    uNewState |= vxInput::STATE_LBUTTON;
  
  if (uState & MK_MBUTTON)
    uNewState |= vxInput::STATE_MBUTTON;
  
  if (uState & MK_RBUTTON)
    uNewState |= vxInput::STATE_RBUTTON;
  
  return uNewState;
} // CreateState()


/**
 * Class to check the process virtual memory condition and warn the user if it's low
 */
void vxInputAdapter::CheckMemory()
{
  int4 RENABLE_MEMORY_CHECK;

  return;
  if( m_bUserWarnedMemory )
    return;

  int4 iFreeVM(0);
  HANDLE hProcess = GetCurrentProcess();
  LPCVOID pvAddress( NULL );
  MEMORY_BASIC_INFORMATION memInfo;

  // calculate the amount of free virtual memory
  while( VirtualQueryEx( hProcess, pvAddress, & memInfo, sizeof( memInfo )))
  {
    if( memInfo.State == MEM_FREE )
    {
      iFreeVM += memInfo.RegionSize; 
    }

    pvAddress = ((PBYTE) memInfo.BaseAddress + memInfo.RegionSize );
  }

  // set the safe memory threshold
  const int4 iMinimumSafeMemoryBytes( 400 * 1024 * 1024 ); // 400 * MB * KB 
  if ( iFreeVM < iMinimumSafeMemoryBytes )
  {
    std::string sWarning( std::string( "The application is running low on memory.\n" ) +
                          std::string( "Please save your work and avoid creating additional datasets.\n" ) +
                          std::string( "Try loading just a single dataset at a time from the console." ) );
    Viatronix::UI::MessageBox::ShowError( gcnew System::String( sWarning.c_str() ));
    m_bUserWarnedMemory = true;
  }
} // CheckMemory()


/**
 * Marks the WinRect in all the viewer's environment's dirty
 */
void vxInputAdapter::UpdateWinRect( vxInputAdapter * pAdapter )
{
  // for each viewer
  for( Viewers::iterator iter = pAdapter->m_viewers.begin(); iter != pAdapter->m_viewers.end(); ++iter) 
    (*iter)->GetEnvironment().SetModified( vxEnvironment::WIN_RECT, true );
} // UpdateWinRect()


// undefines
#undef FILE_REVISION


// Revision History:
// $Log: vxInputAdapter.cpp,v $
// Revision 1.15.2.1  2009/02/05 15:35:35  kchalupa
// Issue #6220: WM_MOUSEWHEEL was handled incorrectly, need to convert from global(frame buffer) coords to local (viewer) before passing them to the manipulator.
//
// Revision 1.15  2007/10/24 20:35:39  gdavidson
// Added back UpdateWinRect
//
// Revision 1.14  2007/08/24 14:35:59  romy
// Modifying the bit for all viewers in multiple times is not necessary
//
// Revision 1.13  2007/08/01 15:43:07  gdavidson
// Issue #5738: Added function pointer for custom monitoring of the WM_SIZE message
//
// Revision 1.12  2007/06/28 19:34:14  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.11.2.1  2007/04/25 17:51:45  geconomos
// Issue# 5613: incorect handling of WM_SETCURSOR
//
// Revision 1.11  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.10  2007/01/23 18:48:49  gdavidson
// Issue #5320: Disregard wm_size for windows with widths or heigths of 0
//
// Revision 1.9  2007/01/18 20:17:42  geconomos
// forced minimum window size in WM_SIZE
//
// Revision 1.8  2007/01/16 17:07:03  gdavidson
// Removed timer
//
// Revision 1.7  2006/12/19 16:46:44  gdavidson
// Issue #5188: Added vxSizeMonitor for WM_SIZE messages
//
// Revision 1.6  2006/11/11 03:10:55  geconomos
// added mouse wheel monitor
//
// Revision 1.5  2006/11/10 03:20:20  geconomos
// Issue 5089: aspect ratio incorrect for 3d; no longer calling flush in parent window classes when parent is resized
//
// Revision 1.4  2006/10/05 17:08:47  geconomos
// added user interacting flag around WM_KEYDOWN and WM_KEYUP
//
// Revision 1.3  2006/10/04 18:29:07  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1  2006/09/27 17:06:56  geconomos
// using vx naming convention for unmanaged classes
//
// Revision 1.11  2006/09/20 14:31:47  geconomos
// code cleanup
//
// Revision 1.10  2006/08/03 14:22:24  geconomos
// removed user interaction handling
//
// Revision 1.9  2006/07/07 13:45:55  geconomos
// update for changes to "user interacting" flag
//
// Revision 1.8  2006/06/28 01:21:59  geconomos
// corrected condition in which AdaptMessage would crash while processing interaction state after associated HWND was destroyed
//
// Revision 1.7  2006/06/27 20:18:49  geconomos
// added flag a check to see  if the "user interacting" flag has changed since the last message processed
//
// Revision 1.6  2006/02/09 14:10:06  geconomos
// corrected calls to wndproc for 64-bit builds
//
// Revision 1.5  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.4  2005/02/02 20:31:28  gdavidson
// Removed managed code used to check virtual memory.
//
// Revision 1.3  2004/04/06 16:31:24  frank
// unified environment access naming
//
// Revision 1.2  2004/04/02 14:02:13  frank
// working on unifying the various rendering contexts
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.29  2004/03/02 01:10:44  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.28.2.2  2004/02/27 18:24:36  frank
// memory checking function was commented out - now it should check memory properly
//
// Revision 1.28.2.1  2004/02/24 20:10:27  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.28  2004/02/11 22:39:26  michael
// added warning variable to remind us of accounting for non 2GB machines
//
// Revision 1.27  2004/02/05 13:35:53  frank
// set the safe memory threshold to 1.6 GB
//
// Revision 1.26  2004/01/30 01:19:33  geconomos
// Added modification of vxEnvironment::WIN_RECT in WM_SIZE.
//
// Revision 1.25  2004/01/26 12:36:55  geconomos
// Passed along key presses to viewer when mouse is over the viewer bar.
//
// Revision 1.24  2004/01/23 18:23:31  frank
// working on warning to user when the memory gets low
//
// Revision 1.23  2003/12/16 14:19:44  michael
// took out logging messages
//
// Revision 1.22  2003/12/16 13:25:51  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.21  2003/08/20 14:31:11  michael
// added "mailto:"
//
// Revision 1.20  2003/07/24 13:04:51  michael
// cosmetics (fixed wrong comment)
//
// Revision 1.19  2003/05/16 13:13:17  frank
// Added KeyUp message handling and coalesced multiple scroll wheel events
//
// Revision 1.18  2003/05/13 13:28:45  michael
// code review
//
// Revision 1.17  2003/05/06 17:56:58  geconomos
// Coding standards
//
// Revision 1.16  2003/05/06 16:22:55  frank
// Fixed bHandled flag in WM_SETCURSOR to take "or" instead of "last"
//
// Revision 1.15  2003/04/08 16:59:30  michael
// fixed most issues related to appending to NONE which is not possible
//
// Revision 1.14  2003/03/20 13:49:25  geconomos
// Some code cleanup.
//
// Revision 1.13  2003/01/28 15:05:03  frank
// Const access functions should be const!
//
// Revision 1.12  2002/11/27 18:51:03  geconomos
// Proper removal of queued up mouse messages.
//
// Revision 1.11  2002/11/25 21:17:02  geconomos
// Increased speed while switching layouts.
//
// Revision 1.10  2002/11/25 14:28:26  geconomos
// Fixed hit testing in WM_SETCURSOR
//
// Revision 1.9  2002/11/25 13:46:17  geconomos
// Added handling for WM_SETCURSOR
//
// Revision 1.8  2002/10/02 13:52:46  dmitry
// msg saved to member var on button down and checked before button up is processed
// button down is not processed if captured is not null
//
// Revision 1.7  2002/09/25 16:59:48  geconomos
// Imporved message handling.
//
// Revision 1.6  2002/09/24 18:17:54  geconomos
// Exception handling and formatting.
//
// Revision 1.5  2002/09/05 16:56:33  dmitry
// Mearged some code from the previous adapter.
//
// Revision 1.4  2002/08/05 16:27:02  dmitry
// WM_TIMER revisited.
//
// Revision 1.3  2002/07/16 23:23:26  geconomos
// no message
//
// Revision 1.2  2002/07/02 20:40:42  geconomos
// Wrapping vxBase with managed wrappers.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/vxInputAdapter.cpp,v 1.15.2.1 2009/02/05 15:35:35 kchalupa Exp $
// $Id: vxInputAdapter.cpp,v 1.15.2.1 2009/02/05 15:35:35 kchalupa Exp $
