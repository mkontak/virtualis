// $Id: UDSegmentsChildView.C,v 1.6.2.2 2009/12/16 15:14:17 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Kevin Kreeger (kevin@viatronix.net)


// includes
#include "stdafx.h"
#include <sstream>
#include "UDSegmentsChildView.h"
#include "MainAppFrame.h"
#include "ReaderGlobal.h"
#include "ViatronStudy.h"
#include "resource.h"
#include "RenderUtil.h"
#include "VCRenderUtils.h"
#include "SliceChildView.h"


// defines
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define FILE_REVISION "$Revision: 1.6.2.2 $"


// namespaces
using namespace ReaderLib;
using namespace WinUtils;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * Constructor 
 * 
 * @param eOrientation   Axis orientation for this slice instance
 * @param hParent        Parent window
 */
UDSegmentsMauiChildView::UDSegmentsMauiChildView(const DatasetOrientations::TypeEnum eDatasetOrientation, HWND hParent, const uint4 uControlID)
 : ViatronChildView(eGVT_UDSEGMENTSMAUI, hParent, eDatasetOrientation),
 m_hScrollbar(HWND(NULL))
{
} // constructor


/**
 * Destructor 
 */
UDSegmentsMauiChildView::~UDSegmentsMauiChildView()
{
} // destructor


/**
 * Initialize slice volume data
 * 
 * @param wParam   Unused
 * @param lParam   Unused
 * @return         Result
 */
LRESULT UDSegmentsMauiChildView::OnVolumeLoaded(WPARAM wParam, LPARAM lParam) // wParam,lParam: unused
{
  UpdateSliceScrollPos(true);
  return 0;
} // OnVolumeLoaded()


/**
 * Render slice
 * 
 * @param void*   Unused
 */
void UDSegmentsMauiChildView::GLRenderScene(void *)
{
  ViatronChildView::GLRenderScene();
  if (!rdrGlobal.m_pCurrDataset->IsLoadingComplete())
  {
    ViatronChildView::GLRenderScene();
    return;
  }

  // rendering the slice
  Rect<int4> rect = GetViewRect();

  // render the image
  m_stripViewer.SetInvertImage( GetInverseImage() );
  m_stripViewer.RenderStrip(GetViewport().m_left, GetViewport().m_top);

} // GLRenderScene()


/**
 * Render text and other 2D objects 
 *
 * @param hDC   Device context for drawing
 */
void UDSegmentsMauiChildView::GLRasterScene()
{
  if (!m_stripViewer.GetDataset()->IsLoadingComplete())
  {
    ViatronChildView::GLRasterScene();
    return;
  }

  glPushAttrib(GL_CURRENT_BIT);

  CRect viewRect;
  GetClientRect(&viewRect);
  CRect clientRect = viewRect - viewRect.TopLeft();

  // Color for window text & menus
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHT0);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glColor3f(1,0,0);

  GLRenderLogo();

  glColor3f( 0, 1, 0 );
  const int4 iTmpStrSz( 200 );
  int1 sTmpStr[ iTmpStrSz + 1 ];
  sprintf( sTmpStr, "Scroll position: %i", m_stripViewer.GetScrollPosition() );
  const int4 iTextHt(15);
  //GetNormalFont().DrawText( clientRect.right - m_iTextPadding, clientRect.bottom - iTextHt, sTmpStr, GDT_BELOWLEFT );
  //GetNormalFont().DrawText( clientRect.right - m_iTextPadding, clientRect.bottom - 2 * iTextHt, m_stripViewer.message, GDT_BELOWLEFT );
  m_stripViewer.message = "";

  // Draw the segment number at top right
  Dataset & rDataset = * m_stripViewer.GetDataset();
  sprintf( sTmpStr, "Segment: %d/%d", rDataset.m_currentSegment+1, rDataset.m_straightSegmentNodes.size()-1 ); // the m_straightSegmentNodes.size() is number of nodes rather than that of segments
  GetNormalFont().DrawText( clientRect.right - 2, clientRect.bottom - 2, sTmpStr, GDT_BELOWLEFT );

  glPopAttrib();
} // GLRasterScene()


/**
 * Set up slice viewer rendering context
 * 
 * @param void*   Unused
 */
void UDSegmentsMauiChildView::GLSetupRC()
{
  if (!m_stripViewer.GetDataset()->IsLoadingComplete())
  {
    ViatronChildView::GLSetupRC();
    return;
  }
  m_stripViewer.SetupOpenGLContext();
} // GLSetupRC()


/**
 * Called on view window resize
 *
 * @param iWidth    Width of child view
 * @param iHeight   Height of child view
 */
void UDSegmentsMauiChildView::GLResize(GLsizei iWidth, GLsizei iHeight)
{

  // this gets called prior to each frame!
  // let's optimize away repeated calls
  if ( iWidth == m_stripViewer.m_uDisplayWidth && iHeight == m_stripViewer.m_uDisplayHeight )
    return;

  m_stripViewer.ResizeDisplay(iWidth, iHeight);

  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);

  SliceScrollBar & scrollBar = GetScrollBar();
  if (scrollBar)
  {
    scrollBar.MoveWindow(CRect(CPoint(GetViewRect().m_left-2, GetViewRect().m_bottom+4),
      CSize(GetViewRect().Width()+4, 12)));
  }

} // GLResize()



/**
 * On idle handler, return true if any rendering on view should occur
 *
 * @return bool    True to render view, false otherwise
 */
bool UDSegmentsMauiChildView::GLIdle()
{
  return false;
} // GLIdle()

//////////////////////////////////////////////////////////////////////////////////////


/**
 * Gets string IDs for mouse button use
 *
 * @param iLeftID    Left button use string resource ID
 * @param iMidID     Middle button use string resource ID
 * @param iRightID   Right button use string resource ID
 */
void UDSegmentsMauiChildView::GetMouseKeyStringIDs(uint4 & iLeftID, uint4 & iMidID, uint4 & iRightID) const
{
  // middle wheel always scrolls
  iMidID = IDS_SCROLL_SLICE;

  if (GetAsyncKeyState(VK_SHIFT)&0x8000)
  {
    //iLeftID = ;
    //iRightID = ;
  }
  else if (GetAsyncKeyState(VK_CONTROL)&0x8000)
  {
    //iLeftID = ;
    //iRightID = ;
  }
  else
  {
    //iLeftID = ;
    //iRightID = ;
  }
} // GetMouseKeyStringIDs().

//////////////////////////////////////////////////////////////////////////////////////
// ... Methods


/**
 * Returns a temporary-use reference to the accompanying scrollbar instance
 *
 * @return   Temporary-use reference to the slice scrollbar
 */
SliceScrollBar & UDSegmentsMauiChildView::GetScrollBar()
{
  SliceScrollBar & scrollBar = *(dynamic_cast<SliceScrollBar*>(CWnd::FromHandle(m_hScrollbar)));
  if (!scrollBar)
  {
    TRACE("Couldn't find scrollbar dialog item ID %d\n", GetClassInstanceID());
  }
  return scrollBar;
} // GetScrollBar()


/**
 * Updates the scrollbar position and range (if indicated)
 *
 * @param bSetRange   Whether or not to update scrollbar range as well
 */
void UDSegmentsMauiChildView::UpdateSliceScrollPos(const bool bSetRange)
{
  SliceScrollBar & scrollBar = GetScrollBar();
  if (!scrollBar)
  {
    return;
  }

  int4 iMaxSlice = m_stripViewer.GetMaxScrollPosition();
  if (iMaxSlice <= 0)
    iMaxSlice = 1;

  if (bSetRange)
    scrollBar.SetScrollRange( 0, iMaxSlice );

  const int4 iScrollPos( m_stripViewer.GetScrollPosition() );
  if ( scrollBar.GetScrollPos() != iScrollPos )
    scrollBar.SetScrollPos( iScrollPos );

} // UpdateSliceScrollPos()


/**
 * Returns ID of the popup menu to display
 *
 * @param uFlags   Mouse/key down flags
 * @param point    Point of mouse event triggering popup menu
 * @return         Resource ID of popup menu
 */
uint4 UDSegmentsMauiChildView::GetPopupMenuID(UINT uFlags, CPoint point) const
{
  // Don't show a popup menu when control key is down
  if (uFlags & MK_CONTROL)
  {
    return 0;
  }

  //
  static const Rect<int4> rEdgeClick(0,0,70,20);

  // Zoom rect on bottom/right
  if (IsMainViewWindow() &&
    (rEdgeClick.GetOffset(GetViewRect().Width() - rEdgeClick.m_right, GetViewRect().Height() - rEdgeClick.m_bottom)
                      .PtInRect(point.x, point.y)))
  {
    return IDM_SLICE_ZOOM;
  }

  return IDM_SLICE_VIEW_WINDOW;
} // GetPopupMenuID().


/**
 * Prepares popup menu for display by enabling/disabling/removing items as appropriate
 *
 * @param menu      Popup menu
 * @param uMenuID   Id of menu
 */
void UDSegmentsMauiChildView::AdjustPopupMenu(CMenu & menu, const uint4 uMenuID) const
{
/*  switch (uMenuID)
  {
    case IDM_SLICE_ZOOM:
      if (m_cylindricalSegmentViewer.GetZoomPercentage() == 100)
      {
        menu.EnableMenuItem(ID_SLICEZOOM_100, MF_GRAYED | MF_BYCOMMAND);
      }
      break;

    default:
      break;
  } // endswitch(uMenuID)
  */
} // AdjustPopupMenu()



/**
 * Message handler
 *
 * @param uMessage   Message
 * @param lParam     Message-dependent parameter
 * @return           Non-zero if message is handled
 */
int4 UDSegmentsMauiChildView::OnMessage(uint2 uMessage, int4 lParam)
{
/*  switch (uMessage)
  {
    case WM_HSCROLL:    // Scroll to absolute position
    case WM_VSCROLL:    // Scroll by relative position
      {
        InvalidateViews( WPARAM(GLChildView::eGVT_ORTHOGONAL), 0);

        uint4 uNewAngle  = LOWORD(lParam);
        uint4 uCurrAngle = m_cylindricalSegmentViewer.GetCurrentPlaneAngle();

        if (uMessage == WM_HSCROLL)
        {
          // Scroll by difference with current slice
          m_cylindricalSegmentViewer.Scroll(uNewAngle - uCurrAngle);
        }
        else // message == WM_VSCROLL
        {
          m_cylindricalSegmentViewer.Scroll(LOWORD(lParam));
        }
        // get the actual slice scrolled to
        uNewAngle = m_cylindricalSegmentViewer.GetCurrentPlaneAngle();

        m_cylindricalSegmentViewer.GetDataset()->m_fields.m_volumeRender.FlushAccumulationBuffer();
        Invalidate(FALSE);
        InvalidateViews( WPARAM(GLChildView::eGVT_OVERVIEW), 0);

        return (uNewAngle<<16) + (uCurrAngle);
      }
      break;

    case ID_VIEWWINDOW_JUMPTOVIEWPOSITION:    // Jump to current view position, i.e. cancel scrolling
      //m_sliceViewer.ResetCurrentVolumePosition();
      UpdateSliceScrollPos();
      InvalidateViews( WPARAM(GLChildView::eGVT_ORTHOGONAL), 0);
      m_cylindricalSegmentViewer.GetDataset()->m_fields.m_volumeRender.FlushAccumulationBuffer();
      break;

    default:
      return FALSE; // Command not handled
  } // endswitch(uMessage)

  Invalidate(FALSE);
  */
  return TRUE;  // Command handled
} // OnMessage()


/**
 * Sets the orientation of the slice
 *
 * @param eOrientation   Slice orientation
 */
void UDSegmentsMauiChildView::SetDatasetType(const DatasetOrientations::TypeEnum eOrientation)
{
  //m_stripViewer.SetDatasetType(eOrientation);
//  UpdateSliceScrollPos(true);
} // SetDatasetType()


/**
 * WM_COMMAND handler
 *
 * @param wParam   Command ID (LOWORD) / Type of control (HIGHWORD)
 * @param lParam   Control handle, or 0 if message not from a control
 * @return         Whether or not message was handled
 */
BOOL UDSegmentsMauiChildView::OnCommand(WPARAM wParam, LPARAM lParam)
{
/*  switch (LOWORD(wParam))
  {
    // Reset slice zoom
    case ID_SLICEZOOM_100:
      break;

    case ID_VIEWWINDOW_SHOWCLEANSED: // Toggle display cleansed/uncleansed slice
      if (m_cylindricalSegmentViewer.IsShowUncleansedSet())
        m_cylindricalSegmentViewer.SetShowUncleansedFlag(false);
      else
        m_cylindricalSegmentViewer.SetShowUncleansedFlag(true);
      m_cylindricalSegmentViewer.SetDesiredPlaneAngle(-1);

      break;

    case ID_VIEWWINDOW_JUMPTOVIEWPOSITION:    // Jump to current view position, i.e. cancel scrolling
      //m_sliceViewer.ResetCurrentVolumePosition();
      UpdateSliceScrollPos();
      break;

    case ID_VIEWWINDOW_UPDATEENDOLUMINALVIEW: // if update navigation view while flying (centerline)
      m_bUpdateEndoluminalView = 1 - m_bUpdateEndoluminalView;
      break;

    case ID_VIEWWINDOW_SNAPSHOT:
      /// See handler(s) in EndoOrthogonalViewFrame::ViewWnd and SupineProneViewWnd
      AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_REPORT_ADDSNAPSHOT, 0), 0);
      break;

    default:
      return FALSE; // Command not handled
  } // endif seln value

  Invalidate(FALSE);
  */
  return TRUE;  // Command handled
} // OnCommand().



/**
 * Left mouse button down
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void UDSegmentsMauiChildView::OnLButtonDown(UINT uFlags, CPoint point)
{
} // OnLButtonDown().


/**
 * Left mouse button up
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void UDSegmentsMauiChildView::OnLButtonUp(UINT uFlags, CPoint point)
{
} // OnLButtonUp()


/**
 * Dbl click Left mouse button
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void UDSegmentsMauiChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}// OnLButtonDblClk()


/**
 * Right mouse button down
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void UDSegmentsMauiChildView::OnRButtonDown(UINT uFlags, CPoint point)
{
} // OnRButtonDown()


/**
 * Right mouse button up
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void UDSegmentsMauiChildView::OnRButtonUp(UINT uFlags, CPoint point)
{
} // OnRButtonUp()


/**
 * synchronizes the submarine to the given 2D image point
 *
 * @param point   the location of the mouse cursor
 */
void UDSegmentsMauiChildView::SynchronizeEndoluminalView( CPoint point )
{

  CRect rect;
  GetClientRect( & rect );

  float4 fPosX = static_cast< float4 >( point.x ) / static_cast< float4 >( rect.Width() );
  float4 fPosY = static_cast< float4 >( point.y ) / static_cast< float4 >( rect.Height() );
  Normal3D<float4> viewDirectionWorld, viewUpWorld;
  Point<float4> vPosWorldCoord = m_stripViewer.GetPickedLocationWorldCoord( fPosX, fPosY, viewDirectionWorld, viewUpWorld );
  //Point<float4> vPosWorldCoord = GetDatasetRef().m_submarine.GetPositionWorld();
  //Normal3D<float4> viewDirectionWorld = GetDatasetRef().m_submarine.GetOrientation(), viewUpWorld = GetDatasetRef().m_submarine.GetUpDirection();

  GetDatasetRef().SelectSegment( vPosWorldCoord );
  GetDatasetRef().m_submarine.SetView( vPosWorldCoord, viewDirectionWorld, viewUpWorld );
  GetDatasetRef().m_submarine.UpdatePosition();
  GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();

  InvalidateViews( GLChildView::eGVT_NAVIGATION, 0 );

} // SynchronizeEndoluminalView()


/**
 * Sets mouse cursor
 *
 * @param pWnd       Pointer to current window
 * @param nHitTest   Identifies location of the mouse
 * @param uMessage   Type of message triggering set cursor message
 * @return           TRUE if cursor is set, FALSE otherwise (to set default cursor)
 */
BOOL UDSegmentsMauiChildView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
/*  UINT uCursorID = IDC_CROSSMC;

  // Show the measure icon if a main view window and mode is line or roi measure
  if (IsMainViewWindow())
  {
    switch (ViatronStudy::GetViewModes(GetViewWndType()))
    {
      case ViewModes::LINEMEASURE:
      case ViewModes::ROIMEASURE:
        uCursorID = IDC_MEASURE;
        break;
      default:
        break;
    } // endswitch(ViatronStudy::ViewModes)
  }

  if (uCursorID == IDC_CROSSMC)
  {
    switch (uMessage)
    {
      case WM_LBUTTONDOWN:
        if (GetAsyncKeyState(VK_CONTROL)&0x8000)
        {
          if (IsMainViewWindow() && !(GetViewWndType() & eGVT_CROSS_SECTION))
          {
            // CTRL-LEFT: Pan in main view
            uCursorID = IDC_PAN;
          }
        }
        else if (GetAsyncKeyState(VK_SHIFT)&0x8000) // SHIFT-LEFT: Adjust Window/Level
        {
          uCursorID = IDC_WINDOWLEVEL;
        }
        break;

      case WM_RBUTTONDOWN:
        if (GetAsyncKeyState(VK_CONTROL)&0x8000)
        { ; }
        else if (GetAsyncKeyState(VK_SHIFT)&0x8000)
        { ; }
        break;

      default:
        break;
    } // endswitch(uMessage)
  } // endif(uCursorID)

  SetCursor(AfxGetApp()->LoadCursor(uCursorID));
  */
  return TRUE;
} // OnSetCursor()


/**
 * Mouse event handlers: mouse move.
 *
 * @param  uFlags Indicates whether various virtual keys are down. 
 * @param  point  specifies the x- and y-coordinate of the cursor.
 */
void UDSegmentsMauiChildView::OnMouseMove(UINT uFlags, CPoint point)
{

  if ( ! m_stripViewer.GetDataset()->IsLoadingComplete() )
  {
    ViatronChildView::OnMouseMove( uFlags, point );
    return;
  }
      
  Point2D<int4> pt( point.x, point.y );
  int4 iSegment = m_stripViewer.SetupHalfPlaneShadow( pt );
  m_stripViewer.GetDataset()->m_currentSegment = iSegment;

  if ( uFlags & MK_LBUTTON )
    SynchronizeEndoluminalView( point );

  Invalidate( FALSE );
  GLChildView::OnMouseMove( uFlags, point );

} // OnMouseMove()



/**
 * Mouse wheel spin handler, scrolls slice
 *
 * @param uFlags   Mouse/keyboard flags
 * @param zDelta   Amount of mouse wheel spin
 * @param pt       Point at which wheel event occurs
 * @return         Whether to continue enabling mouse wheel scrolling
 */
BOOL UDSegmentsMauiChildView::OnMouseWheel(UINT uFlags, int2 iZDelta, CPoint pt)
{
  // each mouse wheel click is 120 (WHEEL_DELTA) delta values
  int4 iMove( iZDelta / WHEEL_DELTA );

  // move much more than one pixel at a time
  iMove *= 50;

  // go much faster if the user holds down the ctrl key
  static const int4 iFactor = 10;
  if (uFlags & MK_CONTROL) iMove *= iFactor;

  m_stripViewer.SetScrollPosition( static_cast<int4>( m_stripViewer.GetScrollPosition() ) + iMove );
  UpdateSliceScrollPos();

  Invalidate(FALSE);
  InvalidateViews( WPARAM(GLChildView::eGVT_UDSEGMENTSMAUI), 0);
  
  // return true to continue enabling mouse wheel scrolling
  return TRUE;
} // OnMouseWheel().


/**
 * Update window's visibility state.
 *
 * @param bShow   Whether or not the window is now visible.
 */
void UDSegmentsMauiChildView::OnShowWindow(const bool bShow)
{
  SliceScrollBar & scroll = GetScrollBar();
  if (scroll)
  {
    scroll.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
  }
} // OnShowWindow()


/**
 * response to action that mouse leaves the window.
 */
void UDSegmentsMauiChildView::OnMouseLeave()
{
  Invalidate(FALSE);
  GLChildView::OnMouseLeave();
} // OnMouseLeave()


/////////////////////////////////////////////////////////////////////////////////////
//

/**
 * keyboard event handler: key press.
 *
 * @param uChar   specifies the virtual key code of the given key.
 * @param uRepCnt specifies the repeat count.
 * @param uFlags  specifies the scan code, key-transition code, previous key state, and context code.
 */
void UDSegmentsMauiChildView::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags)
{
  switch (uChar)
  {
    case 'T':
      // TODO: what is this for? to override something else?
      break;

    default:
      GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
      return;
  }

  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
  Invalidate(FALSE);

  GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
} // OnKeyDown()


// $Log: UDSegmentsChildView.C,v $
// Revision 1.6.2.2  2009/12/16 15:14:17  dongqing
// fix the error display of UD segments total number. The m_straightNodes.size()
// is the number of nodes rather than number of segments
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.6.2.1  2009/10/07 16:13:16  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.6  2007/03/09 00:11:04  jmeade
// code standards.
//
// Revision 1.5  2007/03/05 11:36:00  frank
// removed debug
//
// Revision 1.4  2007/02/14 20:58:56  frank
// fixed up scrolling and segment selection
//
// Revision 1.3  2007/02/02 23:42:37  jmeade
// variable type for vendor map.
//
// Revision 1.2  2007/01/16 10:49:11  frank
// working on scrolling
//
// Revision 1.1  2007/01/03 06:17:35  jmeade
// future ud segments layout.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/UDSegmentsChildView.C,v 1.6.2.2 2009/12/16 15:14:17 dongqing Exp $
// $Id: UDSegmentsChildView.C,v 1.6.2.2 2009/12/16 15:14:17 dongqing Exp $
