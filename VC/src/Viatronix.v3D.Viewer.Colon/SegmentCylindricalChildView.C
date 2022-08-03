// $Id: SegmentCylindricalChildView.C,v 1.5.2.1 2009/10/07 16:13:15 dongqing Exp $
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
#include "SegmentCylindricalChildView.h"
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
#define FILE_REVISION "$Revision: 1.5.2.1 $"


// namespaces
using namespace ReaderLib;
using namespace WinUtils;


const uint4 iSetScrollMessageID(SliceScrollBar::m_uScrollMessageID = WM_APP_CHILD_VIEW_MSG);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * Constructor 
 * 
 * @param eOrientation   Axis orientation for this slice instance
 * @param hParent        Parent window
 */
SegmentCylindricalChildView::SegmentCylindricalChildView(const DatasetOrientations::TypeEnum eDatasetOrientation, HWND hParent, const uint4 uControlID)
 : ViatronChildView(eGVT_3DSEGMENTS, hParent, eDatasetOrientation),
 m_hScrollbar(HWND(NULL)), m_bMeasureActive(false),
 m_bUpdateEndoluminalView(false), m_pDatasetLastRendered((Dataset*) NULL)
{
  // SetDatasetType(eDatasetOrientation);
  m_cylindricalSegmentViewer.SetDatasetType(eDatasetOrientation);
} // constructor


/**
 * Destructor 
 */
SegmentCylindricalChildView::~SegmentCylindricalChildView()
{
  m_pDatasetLastRendered = (Dataset*) NULL;
} // destructor


/**
 * Initialize slice volume data
 * 
 * @param WPARAM   Unused
 * @param LPARAM   Unused
 * @return         Result
 */
LRESULT SegmentCylindricalChildView::OnVolumeLoaded(WPARAM, LPARAM) // wParam,lParam: unused
{
  UpdateSliceScrollPos(true);
  return 0;
} // OnVolumeLoaded()


/**
 * Render slice
 * 
 * @param void*   Unused
 */
void SegmentCylindricalChildView::GLRenderScene(void *)
{
  if (!rdrGlobal.m_pCurrDataset->IsLoadingComplete())
  {
    ViatronChildView::GLRenderScene();
    return;
  }

  if (m_pDatasetLastRendered != m_cylindricalSegmentViewer.GetDataset())
  {
    UpdateSliceScrollPos(true);
    m_pDatasetLastRendered = m_cylindricalSegmentViewer.GetDataset();
  }
  else
  {
    UpdateSliceScrollPos(false);
  }
  
  //glClear(GL_COLOR_BUFFER_BIT);
  
  // rendering the slice
  Rect<int4> rect = GetViewRect();
  m_cylindricalSegmentViewer.RenderSegment(GetViewport().m_left, GetViewport().m_top);
  
} // GLRenderScene()


/**
 * Render text and other 2D objects 
 *
 * @param hDC   Device context for drawing
 */
void SegmentCylindricalChildView::GLRasterScene()
{
  glPushAttrib(GL_CURRENT_BIT);

  CRect viewRect;
  GetClientRect(&viewRect);
  CRect clientRect = viewRect - viewRect.TopLeft();
  int1 sTmpStr[101];

  // Positions where window text will be drawn
  const CPoint sliceOrtnPos(6,5);

  // OpenGL font
  GLFont & glFont = GetNormalFont();

  // Color for window text & menus
  glColor3f(1,0,0);

  // Zoom factor; assume that it is square (and it should be!)
  {
    sprintf(sTmpStr, "%d%%", m_cylindricalSegmentViewer.GetZoomPercentage());
    // zoom factor
    m_zoomMenu.Draw(sTmpStr, glFont, GDT_ABOVELEFT, IsMainViewWindow());
  } // end draw zoom factor

  // Angle
  const uint4 uAngle(m_cylindricalSegmentViewer.GetCurrentPlaneAngle());
  sprintf(sTmpStr, "%03d", uAngle);
  glColor3f(1,1,1);
  // At top-right
  glFont.DrawText(clientRect.right - 2, clientRect.bottom, sTmpStr, GDT_BELOWLEFT);

  glPopAttrib();
} // GLRasterScene


/**
 * Set up slice viewer rendering context
 * 
 * @param void*   Unused
 */
void SegmentCylindricalChildView::GLSetupRC()
{
  m_cylindricalSegmentViewer.SetupOpenGLContext();
} // GLSetupRC()


/**
 * Called on view window resize
 *
 * @param iWidth    Width of child view
 * @param iHeight   Height of child view
 */
void SegmentCylindricalChildView::GLResize(GLsizei iWidth, GLsizei iHeight)
{

  m_zoomMenu.Initialize(this,Rect<int4>(Point2D<int4>(iWidth-3,iHeight-5),Point2D<int4>(40,15)),IDM_SLICE_ZOOM);

  m_cylindricalSegmentViewer.ResizeDisplay(iWidth, iHeight);

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
 * @param void*   Unused
 * @return        True to render view, false otherwise
 */
bool SegmentCylindricalChildView::GLIdle()
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
void SegmentCylindricalChildView::GetMouseKeyStringIDs(uint4 & iLeftID, uint4 & iMidID, uint4 & iRightID) const
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
SliceScrollBar & SegmentCylindricalChildView::GetScrollBar()
{
  SliceScrollBar & scrollBar = *(dynamic_cast<SliceScrollBar*>(CWnd::FromHandle(m_hScrollbar)));
  if (!scrollBar)
  {
    TRACE("Couldn't find scrollbar dialog item ID %d\n", GetClassInstanceID());
  }
  return scrollBar;
}


/**
 * Updates the scrollbar position and range (if indicated)
 *
 * @param bSetRange   Whether or not to update scrollbar range as well
 */
void SegmentCylindricalChildView::UpdateSliceScrollPos(const bool bSetRange)
{
  SliceScrollBar & scrollBar = GetScrollBar();
  if (!scrollBar)
  {
    return;
  }

  //int4 iMaxSlice(m_sliceViewer.GetNumberOfSlices() - 1);
  int4 iMaxSlice(360);
  if (iMaxSlice <= 0)   iMaxSlice = 1;

  if (bSetRange)
  {
    scrollBar.SetScrollRange(0, iMaxSlice);
  }

  const int4 iScrollPos(m_cylindricalSegmentViewer.GetCurrentPlaneAngle());
  if (scrollBar.GetScrollPos() != iScrollPos)
  {
    scrollBar.SetScrollPos(iScrollPos);
  }
} // UpdateSliceScrollPos().


/**
 * Returns ID of the popup menu to display
 *
 * @param uFlags   Mouse/key down flags
 * @param point    Point of mouse event triggering popup menu
 * @return         Resource ID of popup menu
 */
uint4 SegmentCylindricalChildView::GetPopupMenuID(UINT uFlags, CPoint point) const
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
void SegmentCylindricalChildView::AdjustPopupMenu(CMenu & menu, const uint4 uMenuID) const
{
  switch (uMenuID)
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
} // AdjustPopupMenu()


/**
 * Message handler
 *
 * @param uMessage   Message
 * @param lParam     Message-dependent parameter
 * @return           Non-zero if message is handled
 */
int4 SegmentCylindricalChildView::OnMessage(uint2 uMessage, int4 lParam)
{
  switch (uMessage)
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
  return TRUE;  // Command handled
} // OnMessage()


/**
 * Sets the orientation of the slice
 *
 * @param eOrientation   Slice orientation
 */
void SegmentCylindricalChildView::SetDatasetType(const DatasetOrientations::TypeEnum eOrientation)
{
  m_cylindricalSegmentViewer.SetDatasetType(eOrientation);
  UpdateSliceScrollPos(true);
} // SetDatasetType()


/**
 * WM_COMMAND handler
 *
 * @param wParam   Command ID (LOWORD) / Type of control (HIGHWORD)
 * @param lParam   Control handle, or 0 if message not from a control
 * @return         Whether or not message was handled
 */
BOOL SegmentCylindricalChildView::OnCommand(WPARAM wParam, LPARAM lParam)
{
  switch (LOWORD(wParam))
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
  return TRUE;  // Command handled
} // OnCommand().


/**
 * Left mouse button down
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void SegmentCylindricalChildView::OnLButtonDown(UINT uFlags, CPoint point)
{
  CRect rect;
  Point<float4> vPosWorldCoord;

  // All subsequent mouse input will be sent to this view
  SetCapture();

  // ??? Left-click on option popup menus acts like right-click
  switch (GetPopupMenuID(0, point))
  {
    case IDM_SLICE_ZOOM:
    {
      GLChildView::OnLButtonDown(uFlags, point);
      ClientToScreen( &point );
      GetParent()->ScreenToClient( &point );
      GetParent()->PostMessage( WM_RBUTTONDOWN, WPARAM(uFlags), MAKELPARAM(point.x,point.y) );
      return;
    }
    default:
      break;
  } // switch(PopupMenuID)

} // OnLButtonDown().


/**
 * Left mouse button up
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void SegmentCylindricalChildView::OnLButtonUp(UINT uFlags, CPoint point)
{
  // release mouse input capture
  ReleaseCapture();

  Invalidate(FALSE);
} // OnLButtonUp()


/**
 * Dbl click Left mouse button
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void SegmentCylindricalChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{

  CRect rect;
  GetClientRect( & rect );

  float4 fPosX = float4(point.x)/float4(rect.Width());
  float4 fPosY = float4(point.y)/float4(rect.Height());
  Normal3D<float4> viewDirection;
  Point<float4> vPosWorldCoord = m_cylindricalSegmentViewer.GetPickedLocationWorldCoord(fPosX, fPosY, viewDirection);

  GetDatasetRef().SelectSegment( vPosWorldCoord );
  GetDatasetRef().m_submarine.SetPositionWorld( vPosWorldCoord );
  GetDatasetRef().m_submarine.SetOrientation( viewDirection );
  GetDatasetRef().m_submarine.EnforceOrthogonality();
  GetDatasetRef().m_submarine.UpdatePosition();
  GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();

  InvalidateViews( GLChildView::eGVT_ALL, 0 );

}// OnLButtonDblClk()


/**
 * Right mouse button down
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void SegmentCylindricalChildView::OnRButtonDown(UINT uFlags, CPoint point)
{
  SetCapture();

  // stop autoflying (centerline).
  if ( (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D) )
  {
    ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;

    InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);

    const long UPDATE_OVERVIEW = 1;
    const long RERENDER_OVERVIEW = 0;
    GetParent()->SendMessage( WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(0), MAKELPARAM(RERENDER_OVERVIEW,UPDATE_OVERVIEW ) );

    return;
  }

  VtxViewModes sliceModes = ViatronStudy::GetViewModes(GetViewWndType());
  bool bMeasure = (sliceModes == ViewModes::LINEMEASURE) || (sliceModes == ViewModes::ROIMEASURE);

  GLChildView::OnRButtonDown(uFlags, point);
} // OnRButtonDown(). 


/**
 * Right mouse button up
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void SegmentCylindricalChildView::OnRButtonUp(UINT uFlags, CPoint point)
{
  ReleaseCapture();
  //RoiMeasurement(SegmentCylindricalChildView::MEASUREFUNC_UPDATE, CPoint(0,0));
  GLChildView::OnRButtonUp(uFlags, point);
} // OnRButtonUp()


/**
 * Sets mouse cursor
 *
 * @param pWnd       Pointer to current window
 * @param uHitTest   Identifies location of the mouse
 * @param uMessage   Type of message triggering set cursor message
 * @return           TRUE if cursor is set, FALSE otherwise (to set default cursor)
 */
BOOL SegmentCylindricalChildView::OnSetCursor(CWnd * pWnd, UINT uHitTest, UINT uMessage)
{
  UINT uCursorID = IDC_CROSSMC;

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
  return TRUE;
} // OnSetCursor()


/**
 * Mouse event handlers: mouse move.
 *
 * @param  uFlags Indicates whether various virtual keys are down. 
 * @param  point  specifies the x- and y-coordinate of the cursor.
 */
void SegmentCylindricalChildView::OnMouseMove(UINT uFlags, CPoint point)
{
      
  Invalidate(FALSE);
  GLChildView::OnMouseMove(uFlags, point);
} // OnMouseMove()



/**
 * Mouse wheel spin handler, scrolls slice
 *
 * @param uFlags    Mouse/keyboard flags
 * @param izDelta   Amount of mouse wheel spin
 * @param pt        Point at which wheel event occurs
 * @return          TRUE to continue enabling mousewheel
 */
BOOL SegmentCylindricalChildView::OnMouseWheel(UINT uFlags, int2 iZDelta, CPoint pt)
{
  // each mouse wheel click is 120 (WHEEL_DELTA) delta values
  int4 iMove(iZDelta / WHEEL_DELTA);

  // go much faster if the user holds down the ctrl key
  static const int4 iFactor = 10;
  if (uFlags & MK_CONTROL) iMove *= iFactor;

  m_cylindricalSegmentViewer.Scroll(iMove);
  UpdateSliceScrollPos();

  m_cylindricalSegmentViewer.GetDataset()->m_fields.m_volumeRender.FlushAccumulationBuffer();
  Invalidate(FALSE);
  InvalidateViews( WPARAM(GLChildView::eGVT_OVERVIEW), 0);
  
  // return true to continue enabling mouse wheel scrolling
  return TRUE;
} // OnMouseWheel().


/**
 * Update window's visibility state.
 *
 * @param bShow   Whether or not the window is now visible.
 */
void SegmentCylindricalChildView::OnShowWindow(const bool bShow)
{
  SliceScrollBar & scroll = GetScrollBar();
  if (scroll)
  {
    scroll.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
  }
} // OnShowWindow()


/**
 * proc response to action that mouse leaves the window.
 */
void SegmentCylindricalChildView::OnMouseLeave()
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
void SegmentCylindricalChildView::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags)
{
  switch (uChar)
  {
    case 'T':
      break;

    default:
      GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
      return;
  }

  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
  Invalidate(FALSE);

  GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
} // OnKeyDown()


// $Log: SegmentCylindricalChildView.C,v $
// Revision 1.5.2.1  2009/10/07 16:13:15  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.5  2007/03/09 15:53:30  jmeade
// code standards.
//
// Revision 1.4  2007/02/06 16:40:55  jmeade
// user data in render method.
//
// Revision 1.3  2006/09/27 19:20:23  frank
// fixed view synchronization
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.2.2.3.4.6.2.1  2005/07/15 17:21:43  geconomos
// Issue # 4225: Drawing is choppy while switching views
//
// Revision 1.2.2.3.4.6  2005/06/14 22:13:59  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 1.2.2.3.4.5  2005/06/10 18:20:38  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 1.2.2.3.4.4  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.2.2.3.4.3.2.1  2005/06/14 17:37:49  jmeade
// pass scrollbar code as part of WM_HSCROLL message from slicescrollbar.
//
// Revision 1.2.2.3.4.3  2005/04/21 12:57:27  frank
// rollback to Tuesday's version - stable but slow
//
// Revision 1.2.2.3.4.2.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 1.2.2.3.4.2  2005/04/19 13:40:56  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 1.2.2.3.4.1  2005/03/21 19:54:00  frank
// Issue #4038: Sped up font drawing
//
// Revision 1.2.2.3  2003/04/24 20:32:56  jmeade
// Using equivalent draw text location enum values.
//
// Revision 1.2.2.2  2003/02/27 03:22:03  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 1.2.2.1  2003/02/12 01:03:02  jmeade
// Clarifying names for 2D flight modes.
//
// Revision 1.2  2002/11/14 19:38:06  kevin
// Clean up unused code
//
// Revision 1.1  2002/11/11 16:17:01  kevin
// Initial version of cylindrical -- still needs some polishing
//
// Revision 1.6  2002/09/27 14:21:14  ingmar
// replaced Size2D with Point2D
//
// Revision 1.5  2002/09/10 15:47:56  jmeade
// Use one function to handle changing main dataset.
//
// Revision 1.4  2002/08/28 20:52:21  kevin
// Lots of updates to segmental viewing
//
// Revision 1.3  2002/08/22 13:50:00  jmeade
// Don't render when no dataset loaded.
//
// Revision 1.2  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 1.1  2002/08/20 19:15:33  kevin
// 2D and 3D segment views.
//
// Revision 1.1  2002/08/15 15:54:23  kevin
// Added Segmental Visualization prototype/test code.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SegmentCylindricalChildView.C,v 1.5.2.1 2009/10/07 16:13:15 dongqing Exp $
// $Id: SegmentCylindricalChildView.C,v 1.5.2.1 2009/10/07 16:13:15 dongqing Exp $
