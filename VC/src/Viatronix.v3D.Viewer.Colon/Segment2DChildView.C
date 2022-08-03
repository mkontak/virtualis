// $Id: Segment2DChildView.C,v 1.11.2.2 2009/10/07 16:13:14 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Segment2DChildView.C
// Description: 
// Created: 
// Author(s): Kevin Kreeger    kevin@viatronix.net

#include "stdafx.h"
#include "Segment2DChildView.h"
#include "MainAppFrame.h"

#include "ReaderGlobal.h"
#include "ViatronStudy.h"
#include "resource.h"

#include "HUconverter.h"
#include <sstream>

#include "Rect.h"
#include "RenderUtil.h"
#include "VCRenderUtils.h"

#include "ReportViewWnd.h"
#include "SliceChildView.h"
#include "User.h"
#include "vcUtils.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ColonViews::Layouts;
using namespace ReaderLib;
using namespace WinUtils;

#define FILE_REVISION "$Revision: 1.11.2.2 $"

// #define ZOOM_BY_SELECTION

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
Segment2DChildView::Segment2DChildView(const DatasetOrientations::TypeEnum eDatasetOrientation, HWND hParent, const uint4 uControlID)
 : ViatronChildView(eGVT_2DSEGMENTS, hParent, eDatasetOrientation),
 m_hScrollbar(HWND(NULL)),
 m_iWindow(2000), m_iLevel(0), m_bMeasureActive(false), m_iCurrentWLPreset(-1),
 m_iCurrHounsfieldValue(0), m_bCurrHounsfieldValueValid(false), m_currPoint(CPoint(9999,9999)), 
 m_bMouseWheelUpdated(false), m_bMouseWheelCalculate(true), m_bUpdateEndoluminalView(false),
 m_bUpdateRoiMeasurement(false), m_pNeighbourSliceView(NULL),
 m_pDatasetLastRendered((Dataset*) NULL)
{
  // SetDatasetType(eDatasetOrientation);
  m_2DsegmentViewer.SetDatasetType(eDatasetOrientation);

  // Load the popup menus to retrieve text when needed
  m_presetsMenu.LoadMenu(IDM_WLPRESETS);

  m_bAutoRotate = false;
} // constructor


/**
 * Destructor 
 */
Segment2DChildView::~Segment2DChildView()
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
LRESULT Segment2DChildView::OnVolumeLoaded(WPARAM, LPARAM) // wParam,lParam: unused
{
  UpdateSliceScrollPos(true);
  return 0;
} // OnVolumeLoaded()


/**
 * Render slice
 * 
 * @param void*   Unused
 */
void Segment2DChildView::GLRenderScene(void *)
{
  if (!rdrGlobal.m_pCurrDataset->IsLoadingComplete())
  {
    ViatronChildView::GLRenderScene();
    return;
  }

  if (m_pDatasetLastRendered != m_2DsegmentViewer.GetDataset())
  {
    UpdateSliceScrollPos(true);
    m_pDatasetLastRendered = m_2DsegmentViewer.GetDataset();
  }
  else
  {
    UpdateSliceScrollPos(false);
  }
  
  //glClear(GL_COLOR_BUFFER_BIT);
  // window-iLevel
  float4 fScale = 2000.0F/float4(m_iWindow);
  int4 iLevel = m_iLevel + 1000 - (m_iWindow/2);
  float4 fBias = 0.0F-((float4(iLevel)/2000.0F)*fScale);
  
  glPixelTransferf(GL_RED_SCALE,fScale);
  glPixelTransferf(GL_GREEN_SCALE,fScale);
  glPixelTransferf(GL_BLUE_SCALE,fScale);
  glPixelTransferf(GL_RED_BIAS,fBias);
  glPixelTransferf(GL_GREEN_BIAS,fBias);
  glPixelTransferf(GL_BLUE_BIAS,fBias);
  
  // rendering the slice
  Rect<int4> rect = GetViewRect();
  m_2DsegmentViewer.RenderSegment(GetViewport().m_left, GetViewport().m_top);
  
  // reset window-iLevel
  glPixelTransferf(GL_RED_SCALE,1.0);
  glPixelTransferf(GL_GREEN_SCALE,1.0);
  glPixelTransferf(GL_BLUE_SCALE,1.0);
  glPixelTransferf(GL_RED_BIAS,0.0);
  glPixelTransferf(GL_GREEN_BIAS,0.0);
  glPixelTransferf(GL_BLUE_BIAS,0.0);
  
  if(m_bAutoRotate)
  {
    m_2DsegmentViewer.SetDesiredPlaneAngle(m_2DsegmentViewer.GetCurrentPlaneAngle()+3);
    if (m_2DsegmentViewer.GetCurrentPlaneAngle() >= 179) m_bAutoRotate = false;
  }
} // GLRenderScene()


/**
 * Render text and other 2D objects 
 *
 * @param hDC   Device context for drawing
 */
void Segment2DChildView::GLRasterScene()
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

  // Window/Level Preset
  {
    CString sWindowLevel;
    if (m_iCurrentWLPreset >= 0)
    {
      m_presetsMenu.GetSubMenu(0)->GetMenuString(m_iCurrentWLPreset, sWindowLevel, MF_BYPOSITION);
    }
    else
    {
      sWindowLevel.LoadString(IDS_CUSTOM);
    }

    // window/level preset name
    m_winlevMenu.Draw(LPCTSTR(sWindowLevel), glFont, GDT_BELOW);

    // window/level value
    sWindowLevel.Format("%d/%d", m_iWindow, m_iLevel);
    glFont.DrawText(2, clientRect.bottom-15, LPCTSTR(sWindowLevel), GDT_BELOW); // Second Line At top-left
  } // endblock(Window/Level Preset)

  // Zoom factor; assume that it is square (and it should be!)
  {
    sprintf(sTmpStr, "%d%%", m_2DsegmentViewer.GetZoomPercentage());
    // zoom factor
    m_zoomMenu.Draw(sTmpStr, glFont, GDT_ABOVELEFT, IsMainViewWindow());
  } // end draw zoom factor

  // Draw measurement from m_measureList
  const VtxViewModes viewModes(ViatronStudy::GetViewModes(GetViewWndType()));
  if ( (viewModes & ViewModes::LINEMEASURE) && (!m_2DsegmentViewer.m_measureLineList.empty()) )
  {
    static const float4 fGray(0.7F);
    // draw all of them in gray
    glColor3f(fGray, fGray, fGray);
    for (MeasureLineList::iterator listCurr = m_2DsegmentViewer.m_measureLineList.begin(); listCurr != m_2DsegmentViewer.m_measureLineList.end(); listCurr++)
    {
      //RenderLinearMeasurement(listCurr, clientRect);
    } // end for loop thru measure list

    Dataset& myDatasetRef = *m_2DsegmentViewer.GetDataset();

    // draw the primary line in white
    if (m_2DsegmentViewer.m_measureLineList.m_currentIter != m_2DsegmentViewer.m_measureLineList.end())
    {
      // glow primary line if more than one
      static Timer glowTimer;
      const float4 fFlashInterval(1.5F); // seconds
      const float4 fDistFromOne(1.0F - fabs( fmod( static_cast< float4 >( glowTimer.ElapsedTime() ), fFlashInterval) - fFlashInterval / 2.0F));
      const float4 fIntensity(Bound(0.0F, fGray + (1.0F - fGray) * fDistFromOne, 1.0F));
      glColor3f(1,1,1);

      //RenderLinearMeasurement(m_sliceViewer.m_measureLineList.m_currentIter, clientRect);
    } // glow primary line if more than one

    // 2D Line Measurement profile
    if (ColonViatronixWnd::IsDisplaying2DMeasurementProfile())
    {
      // Draw the profile view
      try
      {
        VCRenderUtils::RenderMeasurementProfile(m_2DsegmentViewer.m_measureLineList,
          Rect<uint4>(clientRect.left, clientRect.top, clientRect.right, clientRect.bottom),
          myDatasetRef.m_biopsyTransferFunction,
          myDatasetRef.m_fields.m_volumeRender,
          myDatasetRef.m_fields.m_volumeRender.GetWorldToVolumeMatrix());
      }
      catch (...)
      {
        // Error::WarnLog("<Segment2DChildView::GLRasterScene> Unhandled exception.");
        ex::RuntimeException(LogRec("<Segment2DChildView::GLRasterScene> Unhandled exception.", "Segment2DChildView", "GLRasterScene"));
      }
    } // if displaying 2D measure profile
  } // end draw measurement list

  if ( viewModes & ViewModes::ROIMEASURE )
  {
    if (m_bUpdateRoiMeasurement)
    {
      //RoiMeasurement(Segment2DChildView::MEASUREFUNC_UPDATE, CPoint(0,0));
    }

    // Draw roi measurement
    MeasureAreaList::iterator listCurr = m_2DsegmentViewer.m_measureAreaList.begin();
    MeasureAreaList::iterator listEnd = m_2DsegmentViewer.m_measureAreaList.end();
    for (; listCurr != listEnd; listCurr++)
    {
      Triple<float4> vColor(1.0F, 1.0F, 1.0F);
      if (m_2DsegmentViewer.m_measureAreaList.size() > 1)
      {
        // Highlight the current line
        if (listCurr == m_2DsegmentViewer.m_measureAreaList.m_currentIter)
        {
          vColor = Triple<float4>(1.0F, 1.0F, 1.0F);
        }
        else
        {
          vColor = Triple<float4>(0.8F, 0.8F, 0.8F);
        }
      } // if more than one line

      //RenderAreaMeasurement(listCurr, clientRect, vColor);
    } // for loop thru measure area list

    glColor3f(1,1,1);
    // ROI Measurement Histogram
    if (m_2DsegmentViewer.m_measureAreaList.m_currentIter != m_2DsegmentViewer.m_measureAreaList.end())
    {
      // Display histogram statistics
      CString sDiam, sArea, sAvgHu, sMaxHu, sStdHu;

      sDiam.Format(IDS_ROI_MAJOR_DIAMETER_FORMAT, int4(m_2DsegmentViewer.m_measureAreaList.m_currentIter->m_fMajorDiameterMM));
      glFont.DrawText(2, clientRect.Height()-45, LPCTSTR(sDiam), GDT_ABOVE); // third line at upper left

      sArea.Format(IDS_ROI_AREA_FORMAT, int4(m_2DsegmentViewer.m_measureAreaList.m_currentIter->m_fAreaSquareMM));
      glFont.DrawText(2, clientRect.Height()-60, LPCTSTR(sArea), GDT_ABOVE); // fourth line at upper left

      sAvgHu.Format(IDS_ROI_AVERAGE_HU_FORMAT, m_2DsegmentViewer.m_measureAreaList.m_currentIter->m_fAverageHU);
      glFont.DrawText(2, clientRect.Height()-75, LPCTSTR(sAvgHu), GDT_ABOVE); // fifth line at upper left

      sMaxHu.Format(IDS_ROI_MAX_HU_FORMAT, m_2DsegmentViewer.m_measureAreaList.m_currentIter->m_fMaximumHU);
      glFont.DrawText(2, clientRect.Height()-90, LPCTSTR(sMaxHu), GDT_ABOVE); // sixth line at upper leftt

      sStdHu.Format(IDS_ROI_STD_HU_FORMAT, m_2DsegmentViewer.m_measureAreaList.m_currentIter->m_fStandardDeviationHU);
      glFont.DrawText(2, clientRect.Height()-105, LPCTSTR(sStdHu), GDT_ABOVE); // seventh line at upper left

    } // if render histogram for the current roi
  } // endif (viewModes & ViewModes::ROIMEASURE)

  // Slice Number
  const uint4 uSlice(m_2DsegmentViewer.GetCurrentPlaneAngle());
  sprintf(sTmpStr, "%03d", uSlice);
  glColor3f(1,1,1);
  // At top-right
  glFont.DrawText(clientRect.right - 2, clientRect.bottom, sTmpStr, GDT_BELOWLEFT);

  // Hounsfield Unit Value
  glColor3f(0,1,0);
  sprintf(sTmpStr, "%s: %03d", LoadResourceString(IDS_VALUE), m_iCurrHounsfieldValue);
  m_bMouseWheelUpdated = false;
  if ( ! m_bCurrHounsfieldValueValid ) sprintf( sTmpStr, "%s: --", LoadResourceString(IDS_VALUE) );
  glFont.DrawText(int4(clientRect.Width() * 0.46), clientRect.bottom, sTmpStr, GDT_BELOW);

  glPopAttrib();
} // GLRasterScene


/**
 * Set up slice viewer rendering context
 */
void Segment2DChildView::GLSetupRC()
{
  m_2DsegmentViewer.SetupOpenGLContext();
} // GLSetupRC()


/**
 * Called on view window resize
 *
 * @param iWidth    Width of child view
 * @param iHeight   Height of child view
 */
void Segment2DChildView::GLResize(GLsizei iWidth, GLsizei iHeight)
{
  m_2DsegmentViewer.m_measureLineList.clear();
  m_2DsegmentViewer.m_measureAreaList.clear();

  m_winlevMenu.Initialize(this,Rect<int4>(Point2D<int4>(3,0),Point2D<int4>(70,15)),IDM_WLPRESETS);
  m_zoomMenu.Initialize(this,Rect<int4>(Point2D<int4>(iWidth-3,iHeight-5),Point2D<int4>(40,15)),IDM_SLICE_ZOOM);

  m_2DsegmentViewer.ResizeDisplay(iWidth, iHeight);

  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0 );

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
bool Segment2DChildView::GLIdle()
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
void Segment2DChildView::GetMouseKeyStringIDs(uint4 & iLeftID, uint4 & iMidID, uint4 & iRightID) const
{
  // middle wheel always scrolls
  iMidID = IDS_SCROLL_SLICE;

  VtxViewModes sliceModes = ViatronStudy::GetViewModes(GetViewWndType());
  const bool bMeasure = (sliceModes == ViewModes::LINEMEASURE) || (sliceModes == ViewModes::ROIMEASURE);

  if (GetAsyncKeyState(VK_SHIFT)&0x8000)
  {
    iLeftID = IDS_WINDOWLEVEL;
    iRightID = IDS_MOVE_VIEW_DIRECTION;
  }
  else if (GetAsyncKeyState(VK_CONTROL)&0x8000)
  {
    // Pan allowed only on main view & non-cross-sectional view/non-view-dir view.
    iLeftID = ((IsMainViewWindow() && !(GetViewWndType() & eGVT_CROSS_SECTION))? IDS_PAN_VIEW : 0);

    // Delete allowed only when main view and measure option selected
    if (GetViewWndType() & eGVT_CROSS_SECTION)
      iRightID = 0;
    else
      iRightID = bMeasure ? (IsMainViewWindow() ? IDS_MEASURE_DELETE : 0) : IDS_MOVE_VIEW;
  }
  else
  {
    iLeftID = IsMainViewWindow() ? (bMeasure ? IDS_MEASURE : IDS_ZOOM_VIEW) : 0;
    iRightID = IDS_POPUP_MENU;
  }
} // GetMouseKeyStringIDs().

//////////////////////////////////////////////////////////////////////////////////////
// ... Methods


/**
 * Returns a temporary-use reference to the accompanying scrollbar instance
 *
 * @return   Temporary-use reference to the slice scrollbar
 */
SliceScrollBar & Segment2DChildView::GetScrollBar()
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
void Segment2DChildView::UpdateSliceScrollPos(const bool bSetRange)
{
  SliceScrollBar & scrollBar = GetScrollBar();
  if (!scrollBar) return;
 
  if (bSetRange)
  {
    int4 iMaxAngle(360);
    scrollBar.SetScrollRange(0, iMaxAngle);
  }

  const int4 iScrollPos(m_2DsegmentViewer.GetCurrentPlaneAngle());
  if (scrollBar.GetScrollPos() != iScrollPos)
  {
    scrollBar.SetScrollPos(iScrollPos);
  }
} // UpdateSliceScrollPos().


/**
 * Returns ID of the popup menu to display
 *
 * @param uFlags    Mouse/key down flags
 * @param point     Point of mouse event triggering popup menu
 * @return          Resource ID of popup menu
 */
uint4 Segment2DChildView::GetPopupMenuID(UINT uFlags, CPoint point) const
{
  // Don't show a popup menu when control key is down
  if (uFlags & MK_CONTROL)
  {
    return 0;
  }

  //
  static const Rect<int4> rEdgeClick(0,0,70,20);

  // Window/level presets on top/left
  if (rEdgeClick.PtInRect(point.x,point.y))
  {
    return IDM_WLPRESETS;
  }

  // Slice orientation on bottom/left
  if ( ((GetViewWndType() & eGVT_CROSS_SECTION) || IsMainViewWindow()) &&
    (rEdgeClick.GetOffset(0, GetViewRect().Height() - rEdgeClick.m_bottom).PtInRect(point.x,point.y)) )
  {
    return IDM_SLICE_ORIENTATION;
  }

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
void Segment2DChildView::AdjustPopupMenu(CMenu & menu, const uint4 uMenuID) const
{
  switch (uMenuID)
  {
    case IDM_SLICE_VIEW_WINDOW:
    {
      // Check pseudo-color display if enabled
      menu.CheckMenuItem(ID_VIEWWINDOW_PSEUDOCOLORDISPLAY,
        ((false) ? MF_CHECKED: MF_UNCHECKED) | MF_BYCOMMAND);

      // Check E-cleaning result
      if ( (m_2DsegmentViewer.GetDataset()->m_fields.DoesDiffVolExist() == false)) // || // if .diff not exist, gray it.
           // )// (m_eDbgShow==SHOW_BIOPSY) )  // if pseudo-color, gray it.
      {
        menu.EnableMenuItem(ID_VIEWWINDOW_SHOWCLEANSED, MF_GRAYED);
      }
      else
      {
        menu.CheckMenuItem(ID_VIEWWINDOW_SHOWCLEANSED,
          (m_2DsegmentViewer.IsShowUncleansedSet() ? MF_CHECKED: MF_UNCHECKED) | MF_BYCOMMAND);
      }

      // Line/ROI measurement only when the view is a main view
      if (!IsMainViewWindow())
      {
        menu.EnableMenuItem(ID_VIEWWINDOW_LINEMEASUREMENT, MF_GRAYED|MF_BYCOMMAND);
        menu.EnableMenuItem(ID_VIEWWINDOW_ROIMEASUREMENT, MF_GRAYED|MF_BYCOMMAND);
      }

      // Check Line/ROI measurement options if selected
      const VtxViewModes viewModes(ViatronStudy::GetViewModes(GetViewWndType()));
      menu.CheckMenuItem(ID_VIEWWINDOW_LINEMEASUREMENT,
        ((viewModes & ViewModes::LINEMEASURE) ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
      menu.CheckMenuItem(ID_VIEWWINDOW_ROIMEASUREMENT,
        ((viewModes & ViewModes::ROIMEASURE) ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);

      // Snapshot option
      const bool bEnableReport(ReportLayout::HasPermissions() && ReportLayout::CanModifyReport());
      if (!IsMainViewWindow() || !rdrGlobal.m_pCurrDataset->IsLoadingComplete() || !bEnableReport)
      {
        menu.EnableMenuItem(ID_VIEWWINDOW_SNAPSHOT, MF_GRAYED | MF_BYCOMMAND);
      }

      break;
    } // case:IDM_SLICE_VIEW_WINDOW

    case IDM_WLPRESETS:
    {
      // Prepare the current window/level selection
      for (int4 n = 0, nPresets = menu.GetMenuItemCount(); n < nPresets; n++)
      {
        menu.CheckMenuItem(n, ((m_iCurrentWLPreset == n) ? MF_CHECKED : MF_UNCHECKED) | MF_BYPOSITION);
      }
      break;
    } // case:IDM_WLPRESETS

    case IDM_SLICE_ZOOM:
      if (m_2DsegmentViewer.GetZoomPercentage() == 100)
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
int4 Segment2DChildView::OnMessage(uint2 uMessage, int4 lParam)
{
  switch (uMessage)
  {
    case WM_HSCROLL:    // Scroll to absolute position
    case WM_VSCROLL:    // Scroll by relative position
      {
        InvalidateViews( WPARAM(GLChildView::eGVT_ORTHOGONAL), 0);

        uint4 uNewAngle  = LOWORD(lParam);
        uint4 uCurrAngle = m_2DsegmentViewer.GetCurrentPlaneAngle();

        if (uMessage == WM_HSCROLL)
        {
          // Scroll by difference with current slice
          m_2DsegmentViewer.Scroll(uNewAngle - uCurrAngle);
        }
        else // message == WM_VSCROLL
        {
          m_2DsegmentViewer.Scroll(LOWORD(lParam));
        }
        // get the actual slice scrolled to
        uNewAngle = m_2DsegmentViewer.GetCurrentPlaneAngle();

        m_2DsegmentViewer.GetDataset()->m_fields.m_volumeRender.FlushAccumulationBuffer();
        Invalidate(FALSE);
        InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);

        return (uNewAngle<<16) + (uCurrAngle);
      }
      break;

    case ID_VIEWWINDOW_JUMPTOVIEWPOSITION:    // Jump to current view position, i.e. cancel scrolling
      //m_sliceViewer.ResetCurrentVolumePosition();
      UpdateSliceScrollPos();
      InvalidateViews( WPARAM(GLChildView::eGVT_ORTHOGONAL), 0);
      m_2DsegmentViewer.GetDataset()->m_fields.m_volumeRender.FlushAccumulationBuffer();
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
void Segment2DChildView::SetDatasetType(const DatasetOrientations::TypeEnum eOrientation)
{
  m_2DsegmentViewer.SetDatasetType(eOrientation);
  UpdateSliceScrollPos(true);
} // SetDatasetType()


/**
 * WM_COMMAND handler
 *
 * @param wParam   Command ID (LOWORD) / Type of control (HIGHWORD)
 * @param lParam   Control handle, or 0 if message not from a control
 * @return         Whether or not message was handled
 */
BOOL Segment2DChildView::OnCommand(WPARAM wParam, LPARAM lParam)
{
  switch (LOWORD(wParam))
  {
  // Window/level presets
    case ID_WINDOWLEVEL_RESET:
      m_iCurrentWLPreset = -1;
      m_iWindow = 2000;
      m_iLevel = 0;
      break;

    case ID_WINDOWLEVEL_ABDOMEN:
      m_iCurrentWLPreset = 0;
      m_iWindow = 350;
      m_iLevel = 40;
      break;

    case ID_WINDOWLEVEL_ABDOMEN2:
      m_iCurrentWLPreset = 1;
      m_iWindow = 1349;
      m_iLevel = 385;
      break;

    case ID_WINDOWLEVEL_LUNG:
      m_iCurrentWLPreset = 2;
      m_iWindow = 1500;
      m_iLevel = -500;
      break;

    case ID_WINDOWLEVEL_BONE:
      m_iCurrentWLPreset = 3;
      m_iWindow = 2000;
      m_iLevel = 500;
      break;

    case ID_WINDOWLEVEL_LIVER:
      m_iCurrentWLPreset = 4;
      m_iWindow = 363;
      m_iLevel = 128;
      break;

    // Reset slice zoom
    case ID_SLICEZOOM_100:
      //m_sliceViewer.m_rZoom.Set(0.0F,0.0F,1.0F,1.0F);
      //m_sliceViewer.m_rSelect.Set(9999,9999,9999,9999);

      // if necessary, update ROI statistics
      if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::ROIMEASURE)
        m_bUpdateRoiMeasurement = true;

      break;

    case ID_VIEWWINDOW_SHOWCLEANSED: // Toggle display cleansed/uncleansed slice
      if (m_2DsegmentViewer.IsShowUncleansedSet())
        m_2DsegmentViewer.SetShowUncleansedFlag(false);
      else
        m_2DsegmentViewer.SetShowUncleansedFlag(true);
      m_2DsegmentViewer.SetDesiredPlaneAngle(-1);

      if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::ROIMEASURE)
        m_bUpdateRoiMeasurement = true;

      break;

    case ID_VIEWWINDOW_JUMPTOVIEWPOSITION:    // Jump to current view position, i.e. cancel scrolling
      //m_sliceViewer.ResetCurrentVolumePosition();
      UpdateSliceScrollPos();
      break;

    //case ID_VIEWWINDOW_PSEUDOCOLORDISPLAY:
    //  m_eDbgShow = (m_eDbgShow != SHOW_BIOPSY) ? SHOW_BIOPSY : SHOW_NONE;
    //  break;

    case ID_VIEWWINDOW_LINEMEASUREMENT:
      ViatronStudy::GetViewModes(GetViewWndType()).Toggle(ViewModes::LINEMEASURE);
      m_2DsegmentViewer.m_measureAreaList.clear();
      m_2DsegmentViewer.m_measureLineList.clear();
      if ( Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
      {
        m_pNeighbourSliceView->m_2DsegmentViewer.m_measureAreaList.clear();
        m_pNeighbourSliceView->m_2DsegmentViewer.m_measureLineList.clear();
      }
      break;

    case ID_VIEWWINDOW_ROIMEASUREMENT:
      ViatronStudy::GetViewModes(GetViewWndType()).Toggle(ViewModes::ROIMEASURE);
      m_2DsegmentViewer.m_measureAreaList.clear();
      m_2DsegmentViewer.m_measureLineList.clear();
      if ( Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
      {
        m_pNeighbourSliceView->m_2DsegmentViewer.m_measureAreaList.clear();
        m_pNeighbourSliceView->m_2DsegmentViewer.m_measureLineList.clear();
      }
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
void Segment2DChildView::OnLButtonDown(UINT uFlags, CPoint point)
{
  CRect rect;
  Point<float4> vPosWorldCoord;

  // All subsequent mouse input will be sent to this view
  SetCapture();

  // stop autoflying (centerline).
  if ( (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D) )
  {
    ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;
    InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
    
    const long UPDATE_OVERVIEW = 1;
    const long RERENDER_OVERVIEW = 0;
    GetParent()->SendMessage(WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(0), MAKELPARAM(RERENDER_OVERVIEW,UPDATE_OVERVIEW));

    return;
  }

  // set up this point every time!
  SetStartPointWindowLevel(point);

  // ??? Left-click on option popup menus acts like right-click
  switch (GetPopupMenuID(0, point))
  {
    case IDM_WLPRESETS:
    case IDM_SLICE_ORIENTATION:
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

  if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::ROIMEASURE)
  {
    //RoiMeasurement(MEASUREFUNC_START, point);
  }
  else if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::LINEMEASURE)
  {
    //LineMeasurement(MEASUREFUNC_START, point);
  } // end line measure mode;
  else if (uFlags & MK_CONTROL)
  {
    if (IsMainViewWindow() && !(GetViewWndType() & eGVT_CROSS_SECTION))
    {
      //m_sliceViewer.SetValueOfDisplayOverviewSliceGrasp(true);
      m_2DsegmentViewer.m_viStartMovePt.m_x = (uint4) point.x;
      m_2DsegmentViewer.m_viStartMovePt.m_y = (uint4) point.y;

      // need to pass the zooming parameters to the coupled slice view.
      if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
      {
        //m_pNeighbourSliceView->m_sliceViewer.SetValueOfDisplayOverviewSliceGrasp(true);
        m_pNeighbourSliceView->m_2DsegmentViewer.m_viStartMovePt.m_x = (uint4) point.x;
        m_pNeighbourSliceView->m_2DsegmentViewer.m_viStartMovePt.m_y = (uint4) point.y;
      }
    } // endif (IsMainViewWindow())
  }
  else if ((uFlags & (MK_CONTROL|MK_SHIFT)) == 0)   // Start zoom selection or zoom to selection
  {
    if (IsMainViewWindow())
    {
      // Detect a click in zoom rect; if so, then zoom
      Rect<uint4> selRect(m_2DsegmentViewer.m_rSelect);
      selRect.Normalize();

#ifdef ZOOM_BY_SELECTION
      m_2DsegmentViewer.m_rSelect.Set(point.x,point.y,point.x,point.y);
#else
      if (m_2DsegmentViewer.m_vfAnchorPt.m_x < 0 || m_2DsegmentViewer.m_vfAnchorPt.m_x > 1)
      {
        m_2DsegmentViewer.m_vfAnchorPt.m_x = (float4)point.x / (float4)m_2DsegmentViewer.m_uDisplayWidth;
        m_2DsegmentViewer.m_vfAnchorPt.m_y = (float4)point.y / (float4)m_2DsegmentViewer.m_uDisplayHeight;
        m_2DsegmentViewer.m_viStartMovePt.m_x = (uint4) point.x;
        m_2DsegmentViewer.m_viStartMovePt.m_y = (uint4) point.y;

        // need to pass the zooming parameters to the coupled slice view.
        if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
        {
          m_pNeighbourSliceView->m_2DsegmentViewer.m_vfAnchorPt.m_x = m_2DsegmentViewer.m_vfAnchorPt.m_x;
          m_pNeighbourSliceView->m_2DsegmentViewer.m_vfAnchorPt.m_y = m_2DsegmentViewer.m_vfAnchorPt.m_y;
          m_pNeighbourSliceView->m_2DsegmentViewer.m_viStartMovePt.m_x = (uint4) point.x;
          m_pNeighbourSliceView->m_2DsegmentViewer.m_viStartMovePt.m_y = (uint4) point.y;
        } // endif (m_bSPLocked).
      } // endif (m_sliceViewer.m_vfAnchorPt)
#endif
      if ( (selRect.Width() <= GetViewRect().Width()) && (selRect.Height() <= GetViewRect().Height()) )
      {
        if (selRect.PtInRect(point.x,point.y))
        {  // zoom
          OnKeyDown('Z',0,0);
        }
        else
        {  // cancel
          m_2DsegmentViewer.m_rSelect.Set(9999,9999,9999,9999);
        }
      }
      else
      {    // No current zoom selection
      }
    } // endif (IsMainViewWindow)
  } // endif (ViatronStudy::ViewModes)
} // OnLButtonDown().


/**
 * Left mouse button up
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void Segment2DChildView::OnLButtonUp(UINT uFlags, CPoint point)
{
  // release mouse input capture
  ReleaseCapture();

  switch(ViatronStudy::GetViewModes(GetViewWndType()))
  {
    case ViewModes::ROIMEASURE:
      //RoiMeasurement(MEASUREFUNC_END, point);
      break;
    case ViewModes::LINEMEASURE:
      //LineMeasurement(MEASUREFUNC_END, point);
      break;
    default:
      //m_sliceViewer.SetValueOfDisplayOverviewSliceGrasp(false);
#ifdef ZOOM_BY_SELECTION
#else
      m_2DsegmentViewer.m_vfAnchorPt.m_x = -1;
      m_2DsegmentViewer.m_vfAnchorPt.m_y = -1;
      //m_sliceViewer.SetValueOfDisplayOverviewSliceGrasp(false);
      if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
      {
        m_pNeighbourSliceView->m_2DsegmentViewer.m_vfAnchorPt.m_x = -1;
        m_pNeighbourSliceView->m_2DsegmentViewer.m_vfAnchorPt.m_y = -1;
        //m_pNeighbourSliceView->m_sliceViewer.SetValueOfDisplayOverviewSliceGrasp(false);
      }
#endif
      break;
  } // endswitch m_currMode;

  Invalidate(FALSE);
} // OnLButtonUp()


/**
 * Dbl click Left mouse button
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void Segment2DChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  if (!IsMainViewWindow())
  {
    GLChildView::OnLButtonDblClk(nFlags, point);
    return;
  }

  if ( IsMainViewWindow() )
  {
    AfxGetMainWnd()->SendMessage(WM_APP_SET_MAIN_DATASET, WPARAM(m_2DsegmentViewer.GetDatasetType()), 0);
    ViatronStudy::GetCurrentViewModes() += ViewModes::AUTOFLY2D;
  }
} // OnLButtonDblClk()


/**
 * Right mouse button down
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void Segment2DChildView::OnRButtonDown(UINT uFlags, CPoint point)
{
  SetCapture();

  // stop autoflying (centerline).
  if ( (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D) )
  {
    ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;

    InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);

    const long UPDATE_OVERVIEW = 1;
    const long RERENDER_OVERVIEW = 0;
    GetParent()->SendMessage(WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(0), MAKELPARAM(RERENDER_OVERVIEW,UPDATE_OVERVIEW));

    return;
  }

  VtxViewModes sliceModes = ViatronStudy::GetViewModes(GetViewWndType());
  bool bMeasure = (sliceModes == ViewModes::LINEMEASURE) || (sliceModes == ViewModes::ROIMEASURE);

  if (bMeasure)
  {
    if (uFlags & MK_CONTROL)
    {    // CTRL-Right == Delete Measurement
      //DeleteMeasurement(sliceModes == ViewModes::ROIMEASURE, point);
      //RoiMeasurement(Segment2DChildView::MEASUREFUNC_UPDATE, CPoint(0,0));
      return;
    }
  }
  else
  {
    if (uFlags & MK_CONTROL)
    {
      //m_windowLevelLastPoint = point;
      // view point
      //MoveViewPoint(point);

      if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
      {
        //m_pNeighbourSliceView->m_windowLevelLastPoint = point;
        //m_pNeighbourSliceView->MoveViewPoint(point);
      }
    }
    else if (uFlags & MK_SHIFT)
    {
      // view direction
      //MoveViewDirection(point);
      if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
      {
        //m_pNeighbourSliceView->MoveViewDirection(point);
      }
    } // endif (uFlags&MK_CONTROL/MK_SHIFT)
  } // endif (bMeasure)

  GLChildView::OnRButtonDown(uFlags, point);
} // OnRButtonDown(). 


/**
 * Right mouse button up
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void Segment2DChildView::OnRButtonUp(UINT uFlags, CPoint point)
{
  ReleaseCapture();
  //RoiMeasurement(Segment2DChildView::MEASUREFUNC_UPDATE, CPoint(0,0));
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
BOOL Segment2DChildView::OnSetCursor(CWnd * pWnd, UINT uHitTest, UINT uMessage)
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
      case WM_MOUSEMOVE:
        Rect<uint4> selRect(m_2DsegmentViewer.m_rSelect);
        selRect.Normalize();
        CWnd * pParent = GetParent();
        CPoint currPt; GetCursorPos(&currPt);
        pParent->ScreenToClient(&currPt);
        currPt -= CPoint(GetViewRect().m_left,GetViewRect().m_top);

        if (selRect.PtInRect(currPt.x,currPt.y))
        {
          uCursorID = IDC_MAGNIFY;
        }
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
void Segment2DChildView::OnMouseMove(UINT uFlags, CPoint point)
{
  if (!(uFlags & MK_LBUTTON))
	{
    m_2DsegmentViewer.m_vfAnchorPt.m_x = point.x;
    m_2DsegmentViewer.m_vfAnchorPt.m_y = point.y;
    if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
    {
      m_pNeighbourSliceView->m_2DsegmentViewer.m_vfAnchorPt.m_x = point.x;
      m_pNeighbourSliceView->m_2DsegmentViewer.m_vfAnchorPt.m_y = point.y;
    }
  } // endif (!(uFlags & MK_LBUTTON))
  InvalidateViews( WPARAM(GLChildView::eGVT_ORTHOGONAL), 0);

  m_currPoint = point;

  // Get the Hounsfield units at the cursor position
  if ( m_bMouseWheelUpdated == false && m_bMouseWheelCalculate == true)
  {
    m_iCurrHounsfieldValue = GetHounsfieldValue(Point2D<int4>(point.x,point.y), m_bCurrHounsfieldValueValid);
  }

  VtxViewModes sliceModes = ViatronStudy::GetViewModes(GetViewWndType());

  // added here to prevent from bad operation.
  //if ( m_sliceViewer.GetValueOfDisplayOverviewSliceGrasp() )
  //{
  //  //Pan(m_sliceViewer, point);
  //  if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
  //  {
  //    //m_pNeighbourSliceView->Pan(m_pNeighbourSliceView->m_sliceViewer, point);
  //  }
  //}

  if (uFlags & MK_LBUTTON)
  {
    if (sliceModes == ViewModes::ROIMEASURE)
    {
      //RoiMeasurement(MEASUREFUNC_MOVE, point);
    }
    else if (sliceModes == ViewModes::LINEMEASURE)
    {
      //LineMeasurement(MEASUREFUNC_MOVE, point);
    }
    else
    {
      if (uFlags & MK_SHIFT)
      {
        WindowLevel(point);
      }
      else if (uFlags & MK_CONTROL)
      {
        //Pan(m_sliceViewer, point);
      }
      else
      {
        if ((uFlags & MK_LBUTTON) && IsCaptured())
        {
          //ZoomSelect(point);
        }
      }
    } // Not Measure mode
  }
  else if (uFlags & MK_RBUTTON)
  {
    if ((sliceModes != ViewModes::LINEMEASURE) && (sliceModes != ViewModes::ROIMEASURE))
    {
      if (uFlags & MK_SHIFT)
      {
        //MoveViewDirection(point);
        if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
        {
          //m_pNeighbourSliceView->MoveViewDirection(point);
        }
        InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
      }
      else if (uFlags & MK_CONTROL)
      {
        //MoveViewPoint(point);
        if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
        {
          //m_pNeighbourSliceView->MoveViewPoint(point);
        }
        InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
      }
    } // endif (MoveViewDirection/MoveViewPoint)
  } // endif (uFlags & MK_RBUTTON)

  Invalidate(FALSE);
  GLChildView::OnMouseMove(uFlags, point);
} // OnMouseMove()


/**
 * Mouse wheel spin handler, scrolls slice
 *
 * @param uFlags    Mouse/keyboard flags
 * @param izDelta   Amount of mouse wheel spin
 * @param pt        Point at which wheel event occurs
 * @return          true to continue enabling mouse wheel scrolling
 */
BOOL Segment2DChildView::OnMouseWheel(UINT uFlags, int2 iZDelta, CPoint pt)
{
  // each mouse wheel click is 120 (WHEEL_DELTA) delta values
  int4 iMove(iZDelta / WHEEL_DELTA);

  // go much faster if the user holds down the ctrl key
  static const int4 iFactor = 10;
  if (uFlags & MK_CONTROL) iMove *= iFactor;

  m_2DsegmentViewer.Scroll(iMove);
  m_bMouseWheelUpdated = true;
  UpdateSliceScrollPos();

  m_2DsegmentViewer.GetDataset()->m_fields.m_volumeRender.FlushAccumulationBuffer();
  Invalidate(FALSE);
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
  
  // return true to continue enabling mouse wheel scrolling
  return TRUE;
} // OnMouseWheel().


/**
 * Update window's visibility state.
 *
 * @param bShow   Whether or not the window is now visible.
 */
void Segment2DChildView::OnShowWindow(const bool bShow)
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
void Segment2DChildView::OnMouseLeave()
{
  CPoint cpt; GetCursorPos(&cpt);
  ScreenToClient( &cpt );

  if (Rect<int4>(0,0,GetViewRect().Width(),GetViewRect().Height()).PtInRect(cpt.x,cpt.y))
    m_currPoint = CPoint(-cpt.x,-cpt.y);
  else
    m_currPoint = CPoint(9999,9999);
  m_bCurrHounsfieldValueValid = false;

  Invalidate(FALSE);
  GLChildView::OnMouseLeave();
} // OnMouseLeave()


/////////////////////////////////////////////////////////////////////////////////////
//

/**
 * Get the Hounsfield unit value at the specified point in the view
 *
 * @param pt   Point in the slice view
 * @return     Hounsfield unit value at the point specified
 */
int4 Segment2DChildView::GetHounsfieldValue(const Point2D<int4>& pt, bool & bValidPosition) const
{
  static const int4 iDefaultValue(-1000);

  CRect rect;
  GetClientRect(&rect);

  if ((pt.m_x < 0) || (pt.m_x >= rect.Width()))
  {
    bValidPosition = false;
    return iDefaultValue;
  }

  Dataset& myDatasetRef = *m_2DsegmentViewer.GetDataset();
  const float4 fXPos(float4(pt.m_x)/float4(rect.Width()));
  const float4 fYPos(float4(pt.m_y)/float4(rect.Height()));
  Point<float4> vfPosVolumeCoord = m_2DsegmentViewer.GetPickedLocationVolumeCoord(fXPos,fYPos);

  // check if inside the volume
  bValidPosition = vfPosVolumeCoord.m_x >= 0 && vfPosVolumeCoord.m_x <= myDatasetRef.m_fields.m_sliceDimensions.m_x - 1 &&
                   vfPosVolumeCoord.m_y >= 0 && vfPosVolumeCoord.m_y <= myDatasetRef.m_fields.m_sliceDimensions.m_y - 1 &&
                   vfPosVolumeCoord.m_z >= 0 && vfPosVolumeCoord.m_z <= myDatasetRef.m_fields.m_sliceDimensions.m_z - 1;

  float4 fRawValue;
  if (m_2DsegmentViewer.IsShowUncleansedSet())
    fRawValue = myDatasetRef.m_fields.m_volumeRender.GetUncleansedResampledValue(vfPosVolumeCoord);
  else
    fRawValue = myDatasetRef.m_fields.m_volumeRender.GetResampledValue(vfPosVolumeCoord);

  return HUconverter::Uchar2HU(fRawValue);
} // GetHounsfieldValue()


/**
 * Calculates change in window/level as user drags mouse to specified point
 *
 * @param point   Drag point
 * @return        Success/failure
 */
bool Segment2DChildView::WindowLevel(const CPoint & point)
{
  const float4 fWinFactor(1000.0F / GetViewport().Width());
  const float4 fLevFactor(1000.0F / GetViewport().Height());

  m_iWindow += (m_windowLevelLastPoint.x - point.x) * fWinFactor;
  m_iLevel += (point.y - m_windowLevelLastPoint.y) * fLevFactor;

  m_windowLevelLastPoint = point;

  if (m_iWindow < 1) { m_iWindow = 1; }
  else if (m_iWindow > 2000) { m_iWindow = 2000; }
  if (m_iLevel < -1000) { m_iLevel = -1000; }
  else if (m_iLevel > 1000) { m_iLevel = 1000; }

  m_iCurrentWLPreset = -1;

  return true;
} // WindowLevel()


/**
 * Set up the start point for WindowLevel Control
 *
 * @param point   start point
 */
void Segment2DChildView::SetStartPointWindowLevel(const CPoint & point)
{
  m_windowLevelLastPoint = point;
} // SetStartPointWindowLevel()


/**
 * keyboard event handler: key press.
 *
 * @param uChar     specifies the virtual key code of the given key.
 * @param uRepCnt   specifies the repeat count.
 * @param uFlags    specifies the scan code, key-transition code, previous key state, and context code.
 */
void Segment2DChildView::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags)
{
  const bool bServiceUser(User::GetCurrentUser().GetType() == User::UserType_Service);

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


/**
 * calculate the world coordinate: mapping a 2D point (screen coordinates) on the window to its world coordinates.
 *
 * @param vPoint    point at the window.
 * @return          a 3D point which is a world coordinate.
 */
Point<float4> Segment2DChildView::Calculate3DPoint(const Point<float4> & vPoint)
{
  CRect rect;

  Point<float4> vPointWC;
  GetClientRect(&rect);
  float4 fXPos = float4(vPoint.m_x)/float4(rect.Width());
  float4 fYPos = float4(vPoint.m_y)/float4(rect.Height());
  vPointWC = m_2DsegmentViewer.GetPickedLocationWorldCoord(fXPos,fYPos);

  return vPointWC;
} // Calculate3DPoint().




// Revision History:
// $Log: Segment2DChildView.C,v $
// Revision 1.11.2.2  2009/10/07 16:13:14  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.11.2.1  2008/01/30 22:31:19  dongqing
// change the name and W/L value in the 2D view W/L list
//
// Revision 1.11  2007/03/09 15:53:30  jmeade
// code standards.
//
// Revision 1.10  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.9  2007/02/06 16:40:27  jmeade
// user data in render method.
//
// Revision 1.8  2006/08/24 01:21:31  jmeade
// spacing.
//
// Revision 1.7  2006/06/01 20:22:52  frank
// updated to visual studio 2005
//
// Revision 1.6  2005/11/15 18:52:17  geconomos
// reworked user to be compatible with the new console
//
// Revision 1.5  2005/10/25 16:46:49  jmeade
// ParentViews ==> Layouts
//
// Revision 1.4  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.11.2.5.4.9.2.1  2005/07/15 17:21:43  geconomos
// Issue # 4225: Drawing is choppy while switching views
//
// Revision 1.11.2.5.4.9  2005/06/14 22:13:59  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 1.11.2.5.4.8  2005/06/10 18:20:38  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 1.11.2.5.4.7  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.11.2.5.4.6.2.1  2005/06/14 17:37:49  jmeade
// pass scrollbar code as part of WM_HSCROLL message from slicescrollbar.
//
// Revision 1.11.2.5.4.6  2005/04/21 12:57:27  frank
// rollback to Tuesday's version - stable but slow
//
// Revision 1.11.2.5.4.5.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 1.11.2.5.4.5  2005/04/19 13:40:56  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 1.11.2.5.4.4  2005/03/23 22:01:45  frank
// Issue #4033: Fixed HU display when outside the volume
//
// Revision 1.11.2.5.4.3  2005/03/22 22:02:37  frank
// Issue #4033: Fixed HU display when outside the volume
//
// Revision 1.11.2.5.4.2  2005/03/22 20:57:27  frank
// working on current Hounsfield value display
//
// Revision 1.11.2.5.4.1  2005/03/21 19:54:00  frank
// Issue #4038: Sped up font drawing
//
// Revision 1.11.2.5  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 1.11.2.4  2003/04/24 20:32:44  jmeade
// Using equivalent draw text location enum values.
//
// Revision 1.11.2.3  2003/02/27 03:22:03  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 1.11.2.2  2003/02/22 12:08:44  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 1.11.2.1  2003/02/12 01:02:53  jmeade
// Clarifying names for 2D flight modes.
//
// Revision 1.11  2002/11/14 19:38:06  kevin
// Clean up unused code
//
// Revision 1.10  2002/11/11 16:18:07  kevin
// Initial version of cylindrical -- still needs some polishing
//
// Revision 1.9  2002/10/21 18:52:37  kevin
// Cleaned up some debug
//
// Revision 1.8  2002/09/27 14:21:14  ingmar
// replaced Size2D with Point2D
//
// Revision 1.7  2002/09/18 20:06:16  jmeade
// Moved UserAdmin to ConsoleLib.
//
// Revision 1.6  2002/09/12 18:08:15  jmeade
// No need for typedef'd iterator, changes in VolumeMeasure.h classes.
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
// Revision 1.1  2002/08/20 19:15:32  kevin
// 2D and 3D segment views.
//
// Revision 1.1  2002/08/15 15:54:23  kevin
// Added Segmental Visualization prototype/test code.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Segment2DChildView.C,v 1.11.2.2 2009/10/07 16:13:14 dongqing Exp $
// $Id: Segment2DChildView.C,v 1.11.2.2 2009/10/07 16:13:14 dongqing Exp $
