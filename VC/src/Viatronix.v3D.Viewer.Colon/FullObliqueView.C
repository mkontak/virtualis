// $Id: FullObliqueView.C,v 1.10.2.17 2009/12/09 20:26:02 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: FullObliqueView.C
// Owner(s): Jeff Meade jmeade@viatronix.com

#include "stdafx.h"

#include "FullObliqueView.h"
#include "NavigationView.h"
#include "SliceChildView.h"
#include "OverView.h"
#include "V1KMessages.h"
#include "Plane.h"
#include "Segment3DChildView.h"
#include "DrawOverView.h"
#include "VFrameManager.h"
#include "BackgroundProcess.h"
#include "FullScreenController.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FILE_REVISION "$Revision: 1.10.2.17 $"

using namespace std;
using namespace ColonViews::Layouts;
using namespace ReaderLib;
using namespace WinUtils;

/// type of child view window
GLChildView::ViewWndTypeEnum FullObliqueLayout::m_eViewWndType(GLChildView::eGVT_AXIAL);
DatasetOrientations::TypeEnum FullObliqueLayout::m_eDatasetOrientation(DatasetOrientations::eSUPINE);

/////////////////////////////////////////////////////////////////////////////
// FullObliqueViewFrame

IMPLEMENT_DYNCREATE(FullObliqueLayoutFrame, CMDIChildWnd)

/**
 * Constructor
 */
FullObliqueLayoutFrame::FullObliqueLayoutFrame() : ChildLayoutFrame(new FullObliqueLayout)
{
} // FullObliqueLayoutFrame()


//{{AFX_MSG_MAP(FullObliqueLayoutFrame)
//}}AFX_MSG_MAP

//////////////////////////////////////////////////////////////////////
// FullObliqueView Class

BEGIN_MESSAGE_MAP(FullObliqueLayout, ColonViatronixWnd)
  //{{AFX_MSG_MAP(FullObliqueLayout)
  ON_MESSAGE(WM_APP_SET_MAIN_DATASET, OnChangeMainDataset)
  ON_COMMAND(IDM_TOGGLE_AUTOFLY, OnToggleAutofly)
  ON_COMMAND(IDM_TOGGLE_VIEW_DIRECTION, OnToggleFlightDir)
  ON_COMMAND(IDM_REPORT_ADDSNAPSHOT, OnReportAddSnapshot)
  ON_UPDATE_COMMAND_UI(IDM_REPORT_ADDSNAPSHOT, OnUpdateReportAddSnapshot)
  ON_UPDATE_COMMAND_UI(IDM_NEXT_MISSED_REGION, OnUpdateNextMissedRegion)
  ON_MESSAGE(WM_USER_MOVIE_RECORD_GET_VIEW_ID, OnMovieRecordGetViewID)
  ON_COMMAND(IDM_TOGGLE_SUPINE_PRONE, OnToggleSupineProne)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE, OnToggleLineMeasure)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION, OnToggleSegmentRegion)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/**
 * Constructor
 */
FullObliqueLayout::FullObliqueLayout() : ColonViatronixWnd()
{
  // wait handle for view completion
  m_hViewWait = CreateEvent( NULL, TRUE, FALSE, NULL );

  m_brushBlack.CreateSolidBrush( RGB(0,0,0) );
} // FullObliqueColonWnd()


/**
 * Destructor
 */
FullObliqueLayout::~FullObliqueLayout()
{
  ViatronStudy::m_modeOverView -= ViewModes::CROSS_SECTION;

  if( m_hViewWait )
  {
    SetEvent(m_hViewWait);
    CloseHandle( m_hViewWait );
    m_hViewWait = NULL;
  }
} // ~FullObliqueLayout()


/**
 * Perform view-specific initialization.
 *
 * @param hWndThis   Window handle.
 */
void FullObliqueLayout::CreateChildViews(HWND hWndThis)
{
  ColonViatronixWnd::CreateChildViews(hWndThis);

  // init slice
  SliceChildView& slice = (SliceChildView&) GetChildViewDlgItem(IDC_TEMPLATE_SLICEVIEW);
  
  SliceChildView::SliceChildViewState state;
  if( Colon::FullScreenController::GetInstance().GetFullScreenState( state ))
    slice.SetSliceChildViewState( state );

  switch (m_eViewWndType)
  {
    case GLChildView::eGVT_SAGITTAL:
    {
      slice.SetOrientation(SliceOrientations::eSAGITTAL);
      break;
    }

    case GLChildView::eGVT_CORONAL:
    {
      slice.SetOrientation(SliceOrientations::eCORONAL);
      break;
    }

    case GLChildView::eGVT_CROSS_SECTION:
    {
      slice.SetOrientation(SliceOrientations::eCROSS_SECTION);
      break;
    }

    case GLChildView::eGVT_AXIAL:
    default:
    {
      slice.SetOrientation(SliceOrientations::eAXIAL);
      break;
    }
  }

  AfxGetMainWnd()->PostMessage(WM_APP_SET_MAIN_DATASET, WPARAM(m_eDatasetOrientation), 0);

  // init scrollbar
  m_scroll.Create(SBS_VERT | WS_VISIBLE, CRect(0,0,1,1), this, slice.GetControlID());
  OnAppChildViewMessage(MAKEWPARAM(0, WM_HSCROLL), 0);
  slice.SetSliceScrollBar(m_scroll);
  m_scroll.SetParentView(slice.GetClassInstanceID());

  ViatronStudy::SetCurrentView(GLChildView::eGVT_CROSS_SECTION, rdrGlobal.m_pCurrDataset->GetOrientation());

} // CreateChildViews()


/**
 * Called when window is resized
 *
 * @param uType   Type of resizing (e.g. maximize, restore)
 * @param iCX     Width
 * @param iCY     Height
 */
void FullObliqueLayout::OnSize(UINT uType, int iCX, int iCY) 
{
  ColonViatronixWnd::OnSize(uType, iCX, iCY);
  CalculateViewRects( iCX, iCY );
} // OnSize()


/**
 * renders the views
 */
void FullObliqueLayout::GLRenderViews()
{
  if (ViatronixWnd::m_uInvalidChildViews & uint4(GLChildView::eGVT_SLICE))
  {
    GLRenderView( 0 );
  }
} // GLRenderViews()


/**
 * OnCtlColor
 *
 * @param pDC         device context
 * @param pWnd        window
 * @param uCtlColor   the control
 */
HBRUSH FullObliqueLayout::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor)
{
  HBRUSH hbr(NULL);
 // if (uCtlColor != CTLCOLOR_SCROLLBAR)
    hbr = ColonViatronixWnd::OnCtlColor(pDC, pWnd, uCtlColor);
  pDC->SetTextColor(0x0);
  return hbr;
} // OnCtlColor()


/**
 * Keyboard event handler: key pressed
 * @param uChar     Character key count
 * @param uRepCnt   repeat count
 * @param uFlags    modifier flags
 */
void FullObliqueLayout::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags)
{
  switch (uChar)
  {
    case 'F':
    {
      AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NOCHANGE,
        MAKELPARAM(0, FramesManager::VFrameManager::NavFrameManager::ePOS_FULLVIEW));
      return;
    }
    case VK_ESCAPE:
    {
      AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NOCHANGE,
        MAKELPARAM(0, FramesManager::VFrameManager::NavFrameManager::ePOS_FULLVIEW));
      break;
    }
    default:
    {
      break;
    }
  }

  ColonViatronixWnd::OnKeyDown(uChar, uRepCnt, uFlags);
} // OnKeyDown( uChar, uRepCnt, uFlags )


/**
 * Gets the resource ID for the popup menu for this window
 *
 * @param childView      Child view whose popup menu will be displayed.
 * @param uPopupMenuID   ID Popup menu requested to be displayed.
 * @return               Resource ID for the popup menu, or 0 for no menu
 */
uint4 FullObliqueLayout::OnShowPopupMenu(const GLChildView& childView, const uint4 uPopupMenuID) const
{
  return uPopupMenuID;
} // OnShowPopupMenu( childView, uPopupMenuID )


/**
 * Message: WM_APP_CHILD_VIEW_MSG - Child view application message.
 *
 * @param wParam     Message-dependent parameter.
 * @param lParam     Message-dependent parameter.
 * @return LRESULT   Message-dependent.
 */
LRESULT FullObliqueLayout::OnAppChildViewMessage(WPARAM wParam, LPARAM lParam)
{
  return ColonViatronixWnd::OnAppChildViewMessage(wParam, lParam);
} // OnAppChildViewMessage( wParam, lParam )


/**
 * WM_TIMER event handler.
 *
 * @param iIDEvent   ID of the timer event.
 */
void FullObliqueLayout::OnTimer(UINT uIDEvent)
{
  if (uIDEvent == m_timerProgRefine)
  {
    if (!(ViatronStudy::m_modeObliqueView & ViewModes::AUTOFLY2D))
    {
      SliceChildView& slice = (SliceChildView&) GetChildViewDlgItem(IDC_TEMPLATE_SLICEVIEW);
      slice.m_sliceViewer.m_sliceRenderer.SetObliqueImageSize(512, 512);
      m_timerProgRefine.KillTimer();

      InvalidateViews( WPARAM(GLChildView::eGVT_SLICE), 0);
      GLRenderViews();
    }
  }

  ColonViatronixWnd::OnTimer(uIDEvent);
} // OnTimer( uIDEvent )


/**
 * Message handler sent when views are switched.
 *
 * @param WPARAM    Unused.
 * @param LPARAM    Unused.
 * @return LRESULT  Always 0.
 */
LRESULT FullObliqueLayout::OnChangeMainDataset(WPARAM, LPARAM)
{
  return 0;
} // SwitchUpdateView()


/**
 * Message: IDM_TOGGLE_AUTOFLY: Toggles Autofly mode.
 */
void FullObliqueLayout::OnToggleAutofly()
{
	if( !rdrGlobal.IsLoadingComplete() )
		return;

  ColonViatronixWnd::OnToggleAutofly();

  if (ViatronStudy::m_modeObliqueView & ViewModes::AUTOFLY2D)
  {
    ViatronStudy::m_modeObliqueView += ViewModes::AUTOFLY2D;
		ViatronStudy::m_modeOrthogView += ViewModes::AUTOFLY2D;
  }
  else
  {
    ViatronStudy::m_modeObliqueView -= ViewModes::AUTOFLY2D;
		ViatronStudy::m_modeOrthogView -= ViewModes::AUTOFLY2D;
  }

  SliceChildView& childView((SliceChildView&)GetChildViewDlgItem(IDC_TEMPLATE_SLICEVIEW));

  if (childView.GetViewWndType() == GLChildView::eGVT_CROSS_SECTION)
    childView.SetAutoflyMode(ViatronStudy::m_modeObliqueView & ViewModes::AUTOFLY2D);
  else
    childView.SetAutoflyMode( ViatronStudy::m_modeOrthogView & ViewModes::AUTOFLY2D);

} // OnToggleAutofly()


/**
 * Message: IDM_TOGGLE_VIEW_DIRECTION: Toggles flight direction
 */
void FullObliqueLayout::OnToggleFlightDir()
{
  rdrGlobal.m_pCurrDataset->m_submarine.ToggleFlightDirection();
} // OnToggleFlightDir()


/**
 * Called when mouse wheel is scrolled.
 *
 * @param UINT uFlags      Indicates whether various virtual keys are down.
 * @param short iDelta     Indicates distance wheel was rotated.
 * @param CPoint mousePt   Specifies the x- and y-coordinate of the cursor.
 * @return BOOL            Nonzero if mouse wheel scrolling is enabled; otherwise 0.
 */
BOOL FullObliqueLayout::OnMouseWheel(UINT uFlags, short iDelta, CPoint mousePt) 
{
  ColonViatronixWnd::OnMouseWheel(uFlags, iDelta, mousePt);
  return TRUE;
} // OnMouseWheel( uFlags, iDelta, mousePt )


/**
 * WM_COMMAND handler; here, handles BN_CLICKED event for each of the window buttons
 *
 * @param WPARAM wParam   Indicates the button pressed
 * @param LPARAM lParam   Here, unused
 * @return BOOL           TRUE/FALSE indicating whether command was processed
 */
BOOL FullObliqueLayout::OnCommand(WPARAM wParam, LPARAM lParam)
{
  return ColonViatronixWnd::OnCommand(wParam, lParam);
} // OnCommand( wParam, lParam )


/**
 * return the brush to be used for the view's background
 *
 * @return brush used for the view's background
 */
CBrush * FullObliqueLayout::GetViewBackgroundBrush()
{
  return & m_brushBlack;
} // GetViewBackgroundBrush()


/**
 * Background thread function, waits for event to complete
 *
 * @param pParam   Event handle
 */
UINT FullObliqueLayout::WaitOnViewCompletion(LPVOID pParam)
{
  WaitForSingleObject((HANDLE)pParam, INFINITE);
  return 1;
} // WaitOnMovieCompletion( pParam )


/**
 * Mouse movement handler.
 *
 * @param uFlags   Indicates whether various virtual keys are down. 
 * @param point    specifies the x- and y-coordinate of the cursor.
 */
void FullObliqueLayout::OnMouseMove(UINT uFlags, CPoint point) 
{
  if (uFlags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON))
  {
    SliceChildView& slice = (SliceChildView&) GetChildViewDlgItem(IDC_TEMPLATE_SLICEVIEW);
    slice.m_sliceViewer.m_sliceRenderer.SetObliqueImageSize(128, 128);
    m_timerProgRefine.SetTimer(this, 1500);
  }

  ColonViatronixWnd::OnMouseMove(uFlags, point);

	ViatronStudy::SetCurrentView(GLChildView::eGVT_CROSS_SECTION, rdrGlobal.m_pCurrDataset->GetOrientation());
} // OnMouseMove( uFlags, point )


/**
 * WM_LBUTTONDBLCLK handler.
 *
 * @param uFlags   Current button-state flags.
 * @param origPt   Point of click.
 */
void FullObliqueLayout::OnLButtonDblClk(UINT uFlags, CPoint origPt)
{
  GLChildViewRef currView = GetCurrentView();
  if (currView.GetControlID() == IDC_TEMPLATE_SLICEVIEW)
  {
    ViatronStudy::m_modeObliqueView += ViewModes::AUTOFLY2D;
  }
} // OnLButtonDblClk( uFlags, origPt )


/**
 * Message: IDM_REPORT_ADDSNAPSHOT - add snapshot of view to report
 */
void FullObliqueLayout::OnReportAddSnapshot()
{
  // Add a snapshot of the current view and a blank view (there's no overview to display?)
  std::list<GLChildViewPtr> lViews, displayList;

  DrawOverView ovDrawImage(this, *rdrGlobal.m_pCurrDataset );
  lViews.push_back(&ovDrawImage);

  lViews.push_back(&GetChildViewDlgItem(IDC_TEMPLATE_SLICEVIEW));
  displayList.push_back(&GetChildViewDlgItem(IDC_TEMPLATE_SLICEVIEW));
  AddReportSnapshot(lViews, displayList);
} // OnReportAddSnapshot()


/**
 * Message WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE: Flags all child views of a specified type to be updated on next render (WM_PAINT)
 *
 * @param wpGLChildViewType   GLChildView::ViewTypeEnum value, indicates the child view type to refresh
 * @param lpGLChildViewID     GLVID value, the unique ID of the child view
 * @return                    TRUE
 */
LRESULT FullObliqueLayout::OnAppGLChildViewTypeInvalidate(WPARAM wpGLChildViewType, LPARAM lpGLChildViewID)
{
  return ColonViatronixWnd::OnAppGLChildViewTypeInvalidate(wpGLChildViewType, lpGLChildViewID);
} // OnAppGLChildViewTypeInvalidate( wpGLChildViewType, lpGLChildViewID )


/**
 * Updates arrow in 2D slice center menu item.
 *
 * @param CCmdUI* pCmdUI   Update command handler.
 */
void FullObliqueLayout::OnUpdateViewWindowArrowInSliceCenter(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( FALSE );
} // OnUpdateViewWindowArrowInSliceCenter( pCmdUI )


/**
 * Updates navigation view update during 2D flight option menu item.
 *
 * @param CCmdUI* pCmdUI   Update command handler.
 */
void FullObliqueLayout::OnUpdateViewWindowUpdateEndoluminalView(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( FALSE );
} // OnUpdateViewWindowUpdateEndoluminalView( pCmdUI )


/**
 * Updates navigation view update during 2D flight option menu item.
 *
 * @param CCmdUI* pCmdUI   Update command handler.
 */
void FullObliqueLayout::OnUpdateViewWindowFlightAlongCenterline(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( FALSE );
} // OnUpdateViewWindowFlightAlongCenterline( pCmdUI )


/**
 * Message: ON_UPDATE_COMMAND_UI, IDM_NEXT_MISSED_REGION
 *
 * @param CCmdUI* pCmdUI   Update command handler.
 */
void FullObliqueLayout::OnUpdateNextMissedRegion(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( FALSE );
} // OnUpdateNextMissedRegion( pCmdUI )


/**
 * Message: WM_USER_MOVIE_RECORD_GET_VIEW_ID: Returns the ID of the view to record for a movie.
 *
 * @param WPARAM   Unused.
 * @param LPARAM   Unused.
 * @return         ID of GLChildView to record.
 */
LRESULT FullObliqueLayout::OnMovieRecordGetViewID(WPARAM, LPARAM)
{
  return GetChildViewDlgItem(IDC_TEMPLATE_SLICEVIEW).GetClassInstanceID();
} // OnMovieRecordGetViewID()


/**
 * Message, IDM_TOGGLE_SUPINE_PRONE, toggles between supine and prone datasets
 */
void FullObliqueLayout::OnToggleSupineProne()
{
  if (rdrGlobal.IsDualDataLoaded())
  {
    ColonViatronixWnd::OnToggleSupineProne();
  }
} // OnToggleSupineProne()


/**
 * ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE: Toggles Line Measure Mode.
 */
void FullObliqueLayout::OnToggleLineMeasure()
{
  ToggleViewMode(ViewModes::LINEMEASURE);
} // OnToggleLineMeasure()


/**
 * Update Message: ID_2DDISPLAYOPTIONS_VERTICALSCROLLBARS
 * @param pCmdUI   Update command handler.
 */
void FullObliqueLayout::OnUpdateToggleScrollbarOrientation(CCmdUI* pCmdUI)
{ 
  pCmdUI->Enable(FALSE);
} // OnUpdateToggleScrollbarOrientation()


// Revision History:
// $Log: FullObliqueView.C,v $
// Revision 1.10.2.17  2009/12/09 20:26:02  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.10.2.16  2009/10/07 16:13:13  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.10.2.15  2009/07/30 17:58:19  kchalupa
// Code Walkthrough
//
// Revision 1.10.2.14  2008/01/16 01:41:36  jmeade
// Issue 5917: only vertical scrollbars on the fullscreen view.
//
// Revision 1.10.2.13  2008/01/11 02:48:45  jmeade
// roi editing.
//
// Revision 1.10.2.12  2007/11/05 22:09:51  jmeade
// Issue 5359: vertical scrollbar.
//
// Revision 1.10.2.11  2007/10/18 21:18:19  jmeade
// keyboard shortcuts for WL presets in all slice windows.
//
// Revision 1.10.2.10  2007/10/03 20:03:50  jmeade
// Issue 5359: enable line-measure functionality via f6.
//
// Revision 1.10.2.9  2007/10/03 02:28:57  jmeade
// allow childviews to set their own window background colour.
//
// Revision 1.10.2.8  2007/10/02 01:27:30  jmeade
// f-key toggles full-screen.
//
// Revision 1.10.2.7  2007/10/02 01:00:26  jmeade
// scrollbar implemented by hand.
//
// Revision 1.10.2.6  2007/09/28 01:25:40  jmeade
// set current dataset in fullview mode.
//
// Revision 1.10.2.5  2007/09/26 23:48:34  jmeade
// Issue 5359: disable the tabs.
//
// Revision 1.10.2.4  2007/09/25 23:15:08  jmeade
// Issue 5359: disable the tabs.
//
// Revision 1.10.2.3  2007/09/25 22:45:16  jmeade
// Issue 5359: set the child view type.
//
// Revision 1.10.2.2  2007/09/25 21:34:06  jmeade
// Issue 5359: use for full-screen single view functionality.
//
// Revision 1.10.2.1  2007/09/20 23:07:09  jmeade
// full screen view
//
// Revision 1.10  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.9  2007/02/02 22:10:28  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.8  2006/06/01 20:19:49  frank
// updated to visual studio 2005
//
// Revision 1.7  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.6  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.5.2.1  2005/11/09 02:15:00  jmeade
// cad pane on supine/prone view.
//
// Revision 1.5  2005/10/25 15:37:28  jmeade
// ParentViews ==> Layouts
//
// Revision 1.4  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.3.2.1  2005/09/21 13:28:47  geconomos
// modified DrawImage to implicitly use an image size of 512
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
// Revision 1.1.2.16.2.4  2005/07/07 00:53:19  jmeade
// Issue 4242: Ignore F4 with only one dataset.
//
// Revision 1.1.2.16.2.3  2005/06/30 16:35:21  geconomos
// Issue: 4237 Hooked up keyboard handling for overview viewer
//
// Revision 1.1.2.16.2.2  2005/06/21 15:15:55  geconomos
// Broke out DrawOverView class into it's own .h and .C
//
// Revision 1.1.2.16.2.1  2005/06/20 14:34:13  geconomos
// Issue #4193: Unable to fly in full oblique view
//
// Revision 1.1.2.16  2005/06/14 22:13:59  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 1.1.2.15  2005/06/10 18:20:48  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 1.1.2.14  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.1.2.13.2.3  2005/06/14 17:27:31  jmeade
// Downgrade image during mouse movement.
//
// Revision 1.1.2.13.2.2  2005/06/08 18:46:20  jmeade
// override to specify the movie record view.
//
// Revision 1.1.2.13.2.1  2005/06/08 16:27:46  jmeade
// fixed problem with overview snapshot cutoff on oblique view.
//
// Revision 1.1.2.13  2005/06/06 22:29:42  jmeade
// don't progressively refine during autoflight.
//
// Revision 1.1.2.12  2005/06/06 18:21:29  jmeade
// snapshot and next-region-jump disable....turn off autofly at colon end.
//
// Revision 1.1.2.11  2005/06/03 23:24:09  jmeade
// progressive refinement when scrolling.
//
// Revision 1.1.2.10  2005/06/03 15:02:24  jmeade
// oblique view incremental.
//
// Revision 1.1.2.9  2005/06/02 21:07:09  jmeade
// paint event.
//
// Revision 1.1.2.8  2005/06/02 00:41:22  jmeade
// oblique view incremental.
//
// Revision 1.1.2.7  2005/06/01 23:38:24  jmeade
// set slice to oblique.
//
// Revision 1.1.2.6  2005/06/01 23:05:58  jmeade
// oblique view: controls pad.
//
// Revision 1.1.2.5  2005/06/01 01:36:02  jmeade
// slice count update.
//
// Revision 1.1.2.4  2005/05/31 22:48:23  jmeade
// double-click flight, snapshot, r-key in overview.
//
// Revision 1.1.2.3  2005/05/27 15:57:39  jmeade
// oblique review: flight direction.
//
// Revision 1.1.2.2  2005/05/25 21:32:42  jmeade
// more oblique review functionality.
//
// Revision 1.1.2.1  2005/05/25 01:53:36  jmeade
// oblique review.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/FullObliqueView.C,v 1.10.2.17 2009/12/09 20:26:02 dongqing Exp $
// $Id: FullObliqueView.C,v 1.10.2.17 2009/12/09 20:26:02 dongqing Exp $
