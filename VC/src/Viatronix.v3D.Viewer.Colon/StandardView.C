// $Id: StandardView.C,v 1.21.2.14 2010/09/20 18:41:34 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: StandardView.C  
// Owner: Jeff Meade  jmeade@viatronix.net


#include "stdafx.h"
#include "viatron.h"
#include "StandardView.h"
#include "NavigationView.h"
#include "SliceChildView.h"
#include "MainAppFrame.h"
#include "OverView.h"
#include "NavigationView.h"
#include "SliceChildView.h"
#include "V1KMessages.h"
#include "Global.h"
#include "WinUtils.h"
#include "ReportViewWnd.h"
#include "OverViewDlgBar.h"
#include "MonitorDisplay.h"
#include <mmsystem.h>
#include "DrawOverView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ColonViews::Layouts;
using namespace ReaderLib;

IMPLEMENT_DYNCREATE(StandardLayoutFrame, CMDIChildWnd)

#define FILE_REVISION "$Revision:"

int4 StandardLayout::m_iGlobalMainWnd( ePOS_NAVIGATION );

/**
 * Constructor
 */
StandardLayoutFrame::StandardLayoutFrame() : ChildLayoutFrame((GLWnd *) new StandardLayout)
{
} // StandardLayoutFrame constructor


/**
 * Constructor
 */
StandardLayout::StandardLayout() :
 m_bBarTop( rdrGlobal.m_bToolbarOnBottomForWideScreen ),
 ColonViatronixWnd(!WinUtils::MonitorDisplay::IsWideScreen() ? IDT_STANDARDVIEW : (m_bBarTop ? IDT_STANDARDVIEW_WIDESCREEN : IDT_STANDARDVIEW_WIDESCREEN_BARTOP) ),
 m_iCurrMainWnd(ePOS_NAVIGATION)
{
} // StandardLayout constructor


BEGIN_MESSAGE_MAP(StandardLayout, ColonViatronixWnd)
  
  ON_MESSAGE(WM_APP_SET_MAIN_VIEW_WINDOW, OnSetMainViewWnd)
  ON_MESSAGE(WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID, OnGetMainViewWndID)
  ON_MESSAGE(WM_APP_QUERY_ENABLE_SUPINE_PRONE_TOGGLE, OnQueryEnableSupineProneSwitch)
  ON_MESSAGE(WM_APP_2DAUTOFLIGHT_UPDATE, OnUpdate2DAutoFlight)
  ON_MESSAGE(WM_USER_MOVIE_RECORD_GET_VIEW_ID, OnMovieRecordGetViewID)
  
  ON_COMMAND(IDM_REPORT_ADDSNAPSHOT, OnReportAddSnapshot)
  ON_COMMAND(IDM_TOGGLE_AUTOFLY, OnToggleAutofly)
  ON_COMMAND(IDM_MEASUREMENTS_DELETEALL, OnMeasurementsDeleteAll)
  ON_COMMAND(ID_OPTIONS_CONTROLPAD_FLIGHTCONTROLS, OnControlPadFlightControls)
  ON_COMMAND(ID_OPTIONS_CONTROLPAD_ADJUSTTRANSLUCENCY, OnControlPadAdjustTranslucency)
  
  ON_UPDATE_COMMAND_UI(IDM_REPORT_ADDSNAPSHOT, OnUpdateReportAddSnapshot)
  ON_UPDATE_COMMAND_UI(IDM_MEASUREMENTS_DELETEALL, OnUpdateMeasurementsDeleteAll)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_CONTROLPAD_FLIGHTCONTROLS, OnUpdateControlPadFlightControls)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_CONTROLPAD_ADJUSTTRANSLUCENCY, OnUpdateControlPadAdjustTranslucency)

END_MESSAGE_MAP()


/**
 * Create the child views for this window
 *
 * @param hWndThis   Handle to this window
 */
void StandardLayout::CreateChildViews( HWND hWndThis )
{

  ColonViatronixWnd::CreateChildViews( hWndThis );

  OverView & rOverView = (OverView &) GetChildViewByPos(ePOS_OVERVIEW);
  rOverView.GetDatasetRef().m_pTrackball = & (rOverView.GetDatasetRef().m_trackballStandard);
  rOverView.GetOppositeDatasetRef().m_pTrackball = & (rOverView.GetOppositeDatasetRef().m_trackballStandard);

  m_uInvalidChildViews = uint4(GLChildView::eGVT_NONE);

  const bool bCad( rdrGlobal.m_supine.IsCadFindingsLoaded() || rdrGlobal.m_prone.IsCadFindingsLoaded() );
  GetDlgItem(IDC_TEMPLATE_MISSED_REGIONS_DLG)->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_TEMPLATE_SLICEVIEW1)->ShowWindow   (bCad ? SW_HIDE : SW_SHOW);
  GetDlgItem(IDC_TEMPLATE_SLICESCROLL1)->ShowWindow (bCad ? SW_HIDE : SW_SHOW);
  GetDlgItem(IDC_TEMPLATE_CADRESULTSDLG)->ShowWindow(bCad ? SW_SHOW : SW_HIDE);

} // CreateChildViews()


/**
 * Renders the child views
 */
void StandardLayout::GLRenderViews()
{
  // update the navigation and overview no more than five times per second when interacting
  const float4 fMinimumUpdateTime = 0.2F;
  bool bInvalidateEndoluminalView = false;
  bool bInvalidateOverview = false;

  if( ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D )
  {
    GLChildViewRef view = GetChildView( GLChildView::GLVID( OnGetMainViewWndID( 0, 0 ) ) );
    GLRenderView( view );
    if (SliceChildView::m_bUpdateEndoluminalView && ( typeid( view ) == typeid( SliceChildView ) ) )
    {
      GLRenderView( ePOS_NAVIGATION );
    }
    GLRenderView( ePOS_OVERVIEW );
  }
  else if( rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() )
  {
    Timer timer;
    GLRenderView( ePOS_OVERVIEW );
    GLRenderView( ePOS_NAVIGATION );
    rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.SetLastRenderTime( timer.ElapsedTime() );
  }
  else if( ViatronixWnd::m_uInvalidChildViews != uint4( GLChildView::eGVT_NONE ) )
  {

    if( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_AXIAL )
      GLRenderView( ePOS_AXIAL );
    
    if( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_SAGITTAL )
      GLRenderView( ePOS_SAGITTAL );
    
    if( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_CORONAL )
      GLRenderView( ePOS_CORONAL );
    
    if( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_CROSS_SECTION )
      GLRenderView( ePOS_CROSSSECTION );

    // Optimize the display updates of the navigation view.
    // Don't update too frequently when we are interacting with the 2D slices.
    // If we don't update the 3D view at all, it gets about 60 fps, but only 10 with updates.
    // Let's aim for something in between 10 and 60.
    if( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_NAVIGATION )
    {
      if ( m_lastNavigationUpdateTimer.ElapsedTime() > fMinimumUpdateTime )
      {
        GLRenderView( ePOS_NAVIGATION );
        m_lastNavigationUpdateTimer.Reset();
      }
      else
      {
        bInvalidateEndoluminalView = true;
      }
    }

    // do the same thing as the navigation view
    if( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_OVERVIEW )
    {
      if ( m_lastOverviewUpdateTimer.ElapsedTime() > fMinimumUpdateTime )
      {
        GLRenderView( ePOS_OVERVIEW );
        m_lastOverviewUpdateTimer.Reset();
      }
      else
      {
        bInvalidateOverview = true;
      }
    }
  }

  m_uInvalidChildViews = uint4( GLChildView::eGVT_NONE );

  // keep the navigation view and overview unupdated if we did not update it in this cycle
  if ( bInvalidateEndoluminalView )
    m_uInvalidChildViews |= uint4( GLChildView::eGVT_NAVIGATION );
  if ( bInvalidateOverview )
    m_uInvalidChildViews |= uint4( GLChildView::eGVT_OVERVIEW );

} // GLRenderViews()


/**
 * Message: WM_APP_SET_MAIN_VIEW_WINDOW: Called when sizing window to resize child views
 *
 * @param uType   Identifies size event
 * @param iCX     Width of window
 * @param iCY     Height of view window
 */
void StandardLayout::OnSize( UINT uType, int iCX, int iCY ) 
{
  // The navigation view is assumed to be the main view when resizing view windows
  const int4 iSavedCurrMainWnd(m_iGlobalMainWnd);
  if ( (iCX > 0) && (iCY > 0) && (uType == SIZE_RESTORED) )
  {
    OnSetMainViewWnd( GetChildViewByPos( ePOS_NAVIGATION ).GetClassInstanceID(), 0 );
  }

  ColonViatronixWnd::OnSize(uType, iCX, iCY);
/*
  return;

  //
  // custom resizing code
  //

  // header fixed height at top
  const int4 iHeaderHeight = WinUtils::GetClientRect(GetDlgItem( IDC_TEMPLATE_PATIENTINFO )).Height();
  GetDlgItem( IDC_TEMPLATE_PATIENTINFO )->SetWindowPos( NULL, 0, 0, iCX, iHeaderHeight, SWP_NOZORDER );

  // place center view
  const int4 iControlsHeight = WinUtils::GetClientRect(GetDlgItem( IDC_TEMPLATE_CONTROLS )).Height();
  const int4 iGap = LAYOUT_GAP;
  const int4 iBuddyHeight = LAYOUT_BUDDY_HEIGHT;
  int4 iMainSize = iCY - 2 * iGap - iHeaderHeight - iControlsHeight - iBuddyHeight;
  int4 iMainX = iCX / 2 - iMainSize / 2;
  int4 iMainY = iHeaderHeight + iGap;
  // make the main window smaller if we are a thin client (so that it goes faster) 
  if ( ViatronApp::GetViatronApp()->GetCommandLine().GetThinClient() )
  {
    const int4 iShrinkAmount = iMainSize * 0.12F;
    iMainX += iShrinkAmount;
    iMainY += iShrinkAmount;
    iMainSize -= 2 * iShrinkAmount;
  }
  GetDlgItem( IDC_TEMPLATE_ENDOVIEW )->SetWindowPos( NULL, iMainX, iMainY, iMainSize, iMainSize, SWP_NOZORDER );

  // place MPRs on right
  const int4 iScrollbarHeight = LAYOUT_SCROLLBAR_THICKNESS;
  const int4 iRightGapExtra = ViatronApp::GetViatronApp()->GetCommandLine().GetThinClient() ? 0 : 4;
  int4 iRightMprSize = min( ( iCY - iHeaderHeight - 4 * ( iGap + iRightGapExtra ) - 3 * iScrollbarHeight ) / 3, ( iCX - 4 * iGap - iMainSize ) / 2 );
  if ( SliceChildView::GetScrollBarLocationEnum() == SliceChildView::SBO_RIGHT )
  {
    iRightMprSize -= LAYOUT_SCROLLBAR_THICKNESS;
  }
  const int4 iMprX = ( iMainX + iMainSize + iGap + iCX - iGap ) / 2 - iRightMprSize / 2;
  const int4 iRightGap = ( iCY - iHeaderHeight - 2 * iGap - iRightGapExtra - 3 * iRightMprSize - 3 * iScrollbarHeight ) / 2;
  const int4 iMprY1 = iHeaderHeight + iGap;
  const int4 iMprY2 = iMprY1 + iRightMprSize + iScrollbarHeight + iRightGap;
  const int4 iMprY3 = iMprY2 + iRightMprSize + iScrollbarHeight + iRightGap;
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW2 )->SetWindowPos( NULL, iMprX, iMprY1, iRightMprSize, iRightMprSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW3 )->SetWindowPos( NULL, iMprX, iMprY2, iRightMprSize, iRightMprSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW4 )->SetWindowPos( NULL, iMprX, iMprY3, iRightMprSize, iRightMprSize, SWP_NOZORDER );

  // place MPRs on left
  const bool bCad( rdrGlobal.m_supine.IsCadFindingsLoaded() || rdrGlobal.m_prone.IsCadFindingsLoaded() );

  const int4 iToolsHeight = WinUtils::GetClientRect(GetDlgItem( IDC_TEMPLATE_OVERVIEWTOOLS )).Height();
  const int4 iLeftAvailableHeight = iCY - iHeaderHeight - iControlsHeight - iToolsHeight - 4 * iGap - iScrollbarHeight;//    -15;
  //int4 iLeftMprSize = min( iRightMprSize, min( iLeftAvailableHeight / 2, iMainX - 2 * iGap ) );//     -15;

  const int4 iLeftAvailbleWidth = iMainX - (2 * iGap)
    - (( SliceChildView::GetScrollBarLocationEnum() == SliceChildView::SBO_RIGHT ) ? (LAYOUT_SCROLLBAR_THICKNESS+4) : 0);
  const int4 iLeftMprSize = min(iLeftAvailbleWidth, iLeftAvailableHeight / 2);

  const int4 iCadHeight = bCad ? iLeftAvailableHeight - iLeftMprSize : iLeftMprSize;
  const int4 iLeftGap = iCY - 3 * iGap - iHeaderHeight - iLeftMprSize - iCadHeight - iToolsHeight - iControlsHeight - iScrollbarHeight;
  const int4 iToolsY = iMprY1 + iLeftMprSize + iGap;
  const int4 iObliqueY = iToolsY + iToolsHeight + iLeftGap;
  const int4 iObliqueScrollbarY = iObliqueY + iLeftMprSize;
  const int4 iLeftX = iMainX / 2 - iLeftMprSize / 2;
  const int4 iSliceLeftX = iLeftX - (( SliceChildView::GetScrollBarLocationEnum() == SliceChildView::SBO_RIGHT ) ? (LAYOUT_SCROLLBAR_THICKNESS/2) : 0);
  GetDlgItem( IDC_TEMPLATE_OVERVIEW )->SetWindowPos( NULL, iLeftX, iMprY1, iLeftMprSize, iLeftMprSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_OVERVIEWTOOLS )->SetWindowPos( NULL, iLeftX, iToolsY, iLeftMprSize, iToolsHeight, SWP_NOZORDER );
  dynamic_cast< ColonViews::Dialogs::OverViewDlgBar * >( GetDlgItem( IDC_TEMPLATE_OVERVIEWTOOLS ) )->ResizeWindow( iLeftMprSize, iToolsHeight );
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW1 )->SetWindowPos( NULL, iSliceLeftX, iObliqueY, iLeftMprSize, iLeftMprSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_CADRESULTSDLG )->SetWindowPos( NULL, iLeftX, iObliqueY, iLeftMprSize, iCadHeight, SWP_NOZORDER );

  // controls fixed height at bottom
  const int4 iControlsWidth = iCX - ( iCX - iMprX ) - iGap;
  const int4 iControlsY = iCY - iControlsHeight;
  GetDlgItem( IDC_TEMPLATE_CONTROLS )->SetWindowPos( NULL, 0, iControlsY, iControlsWidth, iControlsHeight, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_ADJUSTTRANSLUCENCY )->SetWindowPos( NULL, 0, iControlsY, iControlsWidth, iControlsHeight, SWP_NOZORDER );

  // translucency dialog
  GetDlgItem( IDC_TEMPLATE_ADJUSTTRANSLUCENCY )->SetWindowPos( NULL, 0, iControlsY, iControlsWidth, iControlsHeight, SWP_NOZORDER );
*/
  // Set the main window to the last saved value, but only when the window is being resized to valid values
  if ( ( iCX > 0 ) && ( iCY > 0 ) )
    OnSetMainViewWnd( GetChildViewByPos(iSavedCurrMainWnd).GetClassInstanceID(), 0 );

  // flush 3D images
  rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
  rdrGlobal.m_prone.m_fields.m_volumeRender.FlushAccumulationBuffer();
} // OnSize()


/**
 * Mouse move handler.
 * 
 * @param uFlags   Identifies which mouse buttons and/or keyboards keys are currently held down
 * @param point    Location of mouse cursor
 */
void StandardLayout::OnMouseMove(UINT uFlags, CPoint point)
{
  ColonViatronixWnd::OnMouseMove( uFlags, point );

  const uint4 uViewModes( ViewModes::AUTOFLY | ViewModes::AUTOFLY2D );
  if( !rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() && !(ViatronStudy::GetCurrentViewModes() & uViewModes ))
    GLRenderViews();
} // OnMouseMove()


/**
 * Gets the resource ID for the popup menu for this window
 *
 * @param childView      Child view whose popup menu will be displayed.
 * @param uPopupMenuID   ID Popup menu requested to be displayed.
 * @return               Resource ID for the popup menu, or 0 for no menu
 */
uint4 StandardLayout::OnShowPopupMenu(const GLChildView& childView, const uint4 uPopupMenuID) const
{
  // Show only IDM_SLICE_CROSS_SECTION_ORIENTATION for xsec; Don't show IDM_SLICE_ORIENTATION popup menu for orthogonals
  if (uPopupMenuID == IDM_SLICE_ORIENTATION)
  {
    return (childView.GetViewWndType() & GLChildView::eGVT_CROSS_SECTION)? IDM_SLICE_CROSS_SECTION_ORIENTATION : 0;
  }
  return uPopupMenuID;
} // OnShowPopupMenu


/**
 * Message: IDM_TOGGLE_AUTOFLY: Toggles Autofly mode.
 */
void StandardLayout::OnToggleAutofly()
{
  ColonViatronixWnd::OnToggleAutofly();

  ((SliceChildView&)GetChildViewByPos(ePOS_AXIAL)).SetAutoflyMode(false);
  ((SliceChildView&)GetChildViewByPos(ePOS_SAGITTAL)).SetAutoflyMode(false);
  ((SliceChildView&)GetChildViewByPos(ePOS_CORONAL)).SetAutoflyMode(false);
  ((SliceChildView&)GetChildViewByPos(ePOS_CROSSSECTION)).SetAutoflyMode(false);

  GLChildViewRef view = GetChildView(GLChildView::GLVID(OnGetMainViewWndID(0,0)));
  if( typeid( view ) == typeid( SliceChildView ) )
  {
    ((SliceChildView&)view).SetAutoflyMode(ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D);
  }
} // OnToggleAutofly


/**
 * Message: WM_APP_CHILD_VIEW_MSG: On message intended for a child view.
 *
 * @param wParam   Message-dependent code.
 * @param lParam   Message-dependent code.
 * @return         TRUE if message is handled, FALSE otherwise.
 */
LRESULT StandardLayout::OnAppChildViewMessage( WPARAM wParam, LPARAM lParam )
{
  LRESULT result = ColonViatronixWnd::OnAppChildViewMessage(wParam, lParam);

  //if( HIWORD(wParam) == WM_HSCROLL ) 
  //  GLRenderViews();

  return result;
} // OnAppChildViewMessage()


/**
 * Redirects a generic child view message to the main view window
 *
 * @param iWndTypes   Child view types for message destination
 * @param uMsg        Message
 */
void StandardLayout::RedirectBroadcastMessage( int4 & iWndTypes, const UINT uMsg ) const
{
  ColonViatronixWnd::RedirectBroadcastMessage(iWndTypes, uMsg);

  if (iWndTypes == GLChildView::eGVT_NONE)
  {
    iWndTypes = GetChildViewByPos(m_iCurrMainWnd).GetViewWndType();
  }
} // RedirectBroadcastMessage()


/**
 * Reset the globally-stored main view window.
 */
void StandardLayout::ResetGlobalMainWindow()
{
  m_iGlobalMainWnd = ePOS_NAVIGATION;
} // ResetGlobalMainWindow()


/**
 * Sets a view window to be the main view
 *
 * @param newMainWndID   The new main view
 * @param LPARAM         Unused
 * @return               Always TRUE (indicating message was handled)
 */
LRESULT StandardLayout::OnSetMainViewWnd( WPARAM iNewMainWndID, LPARAM )
{

  // Ensure the window specified is not already the main view window
  if (iNewMainWndID == OnGetMainViewWndID(0,0))
  {
    return TRUE;
  }
  else
  {
    // Stop autofly if we have actually switched views around
    ViatronStudy::m_modeEndoView -= ViewModes::AUTOFLY;
    rdrGlobal.m_pCurrDataset->m_submarine.FullStop();
    ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;

  } // if main window already

  // Swap the view rectangles for the current and new main view
  ViatronChildView & rNewView = (ViatronChildView &) GetChildView( iNewMainWndID );

  if (rNewView.GetClassInstanceID() != GLWnd::m_nullChildView.GetClassInstanceID())
  {
    // The center view window
    GLChildViewRef rCenterView = GetChildViewByPos( m_iCurrMainWnd );
    int iOldCenterClassInstanceID = rCenterView.GetClassInstanceID();

    // Record new main view window, before changes, for views that may perform query during resize
    m_iGlobalMainWnd = m_iCurrMainWnd = GetChildViewPos( iNewMainWndID );

    // get the views to swap
    GLChildViewRef rEndoluminalView = GetChildViewByPos(ePOS_NAVIGATION);

    // Navigation back to center location
    if (iNewMainWndID == rEndoluminalView.GetClassInstanceID())
    {
      SwapViewRects( rEndoluminalView, rCenterView );
    }

    // Move a view to the center location
    else
    {

      SwapViewRects( rCenterView, rNewView );

      // Swap two non-navigation views in center location
      if ( rCenterView.GetClassInstanceID() != rEndoluminalView.GetClassInstanceID() )
      {
        SwapViewRects( rEndoluminalView, rCenterView );
      }
    }

    UpdateWindow();

    ViatronStudy::SetCurrentView( rNewView.GetViewWndType(), rNewView.GetDatasetRef().GetOrientation() );
    
  }

  rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
  rdrGlobal.m_prone.m_fields.m_volumeRender.FlushAccumulationBuffer();

  return TRUE;
} // OnSetMainViewWnd()


/**
 * Overridden to prevent normal setting of current ViatronStudy view window by mouse movement
 *  (current view window in StandardView is determined by the current main view window; see OnSetMainViewWnd())
 */
void StandardLayout::SetCurrentViatronStudyView()
{
} // SetCurrentViatronStudyView()


/**
 * Message: WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID: Gets the class instance ID of the current main view window
 *
 * @param WPARAM   Unused
 * @param LPARAM   Unused
 * @return         Main child view's ID
 */
LRESULT StandardLayout::OnGetMainViewWndID(WPARAM, LPARAM)
{
  return GetChildViewByPos(m_iCurrMainWnd).GetClassInstanceID();
} // OnGetMainViewWndID()


/**
 * Message: WM_APP_QUERY_ENABLE_SUPINE_PRONE_TOGGLE: Enables switching between Supine/Prone (by PatientInfoDlg)
 *
 * @param WPARAM   Unused
 * @param LPARAM   Unused
 * @return         TRUE to indicate toggling between Supine & Prone allowed
 */
LRESULT StandardLayout::OnQueryEnableSupineProneSwitch(WPARAM, LPARAM)
{
  return 1;
} // OnQueryEnableSupineProneSwitch()


/** Message: WM_APP_2DAUTOFLIGHT_UPDATE: 2D auto-flight: signal to set up new skeleton position.
 *
 * @param WPARAM   position on current skeleton. 
 * @param LPARAM   Unused
 * @return         TRUE to set up new position on current skeleton.
 */
LRESULT StandardLayout::OnUpdate2DAutoFlight(WPARAM wParam, LPARAM lParam)
{
  if (lParam == 0) // update skeleton position
  {
    if (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D)
    {
      GLChildViewRef childView = GetChildViewByPos(m_iCurrMainWnd);
      if (typeid(childView) == typeid(SliceChildView))
      {
        SliceChildView & sliceView = (SliceChildView &) childView;
        sliceView.OnUpdate2DAutoFlight((int4)wParam);
      }
    }
  }

  return ColonViatronixWnd::OnUpdate2DAutoFlight(wParam, lParam);
} // OnUpdate2DAutoFlight()


/**
 * Notifies view window of the current progress of dataset load
 *
 * @param eCompletedStep   The previously completed step
 * @param eCurrentStep     The next (current) study loading step
 * @param eOrientation     Orientation on which the next step is performing
 */
void StandardLayout::OnStudyLoadStepNotify(const StudyLoad::StepsEnum eCompletedStep,
  const StudyLoad::StepsEnum eCurrentStep, const DatasetOrientations::TypeEnum eOrientation)
{
  for (int4 i(0); i < NumGLViews(); i++)
  {
    if (typeid(GetChildViewByPos(i)) == typeid(SliceChildView))
    {
      ((SliceChildView&) GetChildViewByPos(i)).OnVolumeLoaded(0,0);
    }
  }
} // OnStudyLoadStepNotify


/**
 * Message: IDM_REPORT_ADDSNAPSHOT: Adds a new report entry with snapshots.
 */
void StandardLayout::OnReportAddSnapshot()
{
  DrawOverView rOverview(this, *rdrGlobal.m_pCurrDataset);
  rOverview.SetDisplayDistanceFromRectum(false);

  std::list<GLChildViewPtr> childList;
  childList.push_back(&rOverview);
  GLChildViewRef childView = GetChildViewByPos(m_iCurrMainWnd);

  if (typeid(childView) == typeid(EndoluminalView))
  {
    childList.push_back(&childView);
    AddReportSnapshot(childList, rdrGlobal.m_pCurrDataset->m_session.m_measureLineList);
  }
  else if (typeid(childView) == typeid(SliceChildView))
  {
    SliceChildView& rSlice = dynamic_cast<SliceChildView&>(childView);
    childList.push_back(&childView);
    AddReportSnapshot(childList, rSlice.m_sliceViewer.m_measureLineList);
  }
  else
  {
    AddReportSnapshot(childList, MeasureLineList());
  }

  rOverview.SetDisplayDistanceFromRectum(true);
} // OnReportAddSnapshot()


/**
 * Message: WM_USER_MOVIE_RECORD_GET_VIEW_ID: Returns the ID of the view to record for a movie.
 *
 * @param WPARAM   Unused.
 * @param LPARAM   Unused.
 * @return         ID of GLChildView to record.
 */
LRESULT StandardLayout::OnMovieRecordGetViewID(WPARAM, LPARAM)
{
  return OnGetMainViewWndID(0,0);
} // OnMovieRecordGetViewID()


// $Log: StandardView.C,v $
// Revision 1.21.2.14  2010/09/20 18:41:34  dongqing
// Jeff's bug fix for his various monitor aspect ratio work
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.13  2010/03/10 11:13:48  dongqing
// set main view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.12  2010/03/10 00:44:47  dongqing
// localized overview for snapshot
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.11  2010/03/09 20:25:27  dongqing
// print study info on every snapshot image
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.10  2010/01/31 02:14:48  dongqing
// removed tabctrl for dialog toggling
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.9  2009/12/03 22:21:55  dongqing
// report snapshot of just overview
//
// Revision 1.21.2.8  2009/11/27 20:46:12  dongqing
// missed regions dialog
//
// Revision 1.21.2.7  2009/11/27 18:04:00  dongqing
// tabbed cad/missed regions pane on widescreen standard layout
//
// Revision 1.21.2.6  2009/11/25 22:09:34  dongqing
// widescreen
//
// Revision 1.21.2.5  2009/10/07 16:13:16  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.21.2.4  2008/02/07 20:20:57  jmeade
// fit scrollbar.
//
// Revision 1.21.2.3  2008/02/07 19:30:21  jmeade
// left mpr sizing.
//
// Revision 1.21.2.2  2008/01/30 20:18:44  jmeade
// Issue 5917: view spacing for vertical scrollbars.
//
// Revision 1.21.2.1  2008/01/16 01:25:45  jmeade
// Issue 5917: SliceChildViews position their own scrollbars.
//
// Revision 1.21  2007/03/09 01:11:49  jmeade
// code standards.
//
// Revision 1.20  2007/03/07 20:49:55  jmeade
// code standards.
//
// Revision 1.19  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.18  2007/02/02 21:27:09  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.17  2006/10/02 17:54:13  frank
// improved thin client sizing
//
// Revision 1.16  2006/09/29 21:02:08  frank
// added a thin-client mode
//
// Revision 1.15  2006/09/14 20:00:56  frank
// Issue #4843: Improved speed of 2D slicing
//
// Revision 1.14  2006/02/07 21:40:28  dongqing
// code walk through
//
// Revision 1.13  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.12.2.6  2006/01/11 16:47:09  frank
// Issue #4596: Moved adjust translucency dialog to proper location and size
//
// Revision 1.12.2.5  2006/01/10 20:36:41  geconomos
// issue 4596: adjust translucency dialog not positioned correctly
//
// Revision 1.12.2.4  2006/01/10 16:46:53  frank
// Fixed display of CAD vendor image when only 1 of 2 cad files exists
//
// Revision 1.12.2.3  2006/01/05 18:31:16  frank
// Issue #4587: Cleaned up spacing on the right MPRs so the lumen coverage indicator is never overlapped
//
// Revision 1.12.2.2  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.12.2.1  2005/12/09 04:01:25  jmeade
// Issue 4571: Display cad dialog if either dataset has cad findings.
//
// Revision 1.12  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.11  2005/10/13 16:04:43  jmeade
// Issue 4469: Turn off distance-to-rectum text during snapshot.
//
// Revision 1.10  2005/10/04 18:10:55  jmeade
// no separate distance-to-rectum text field, text only in overview.
//
// Revision 1.9  2005/09/23 12:02:04  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.8  2005/09/21 17:19:26  jmeade
// display cad findings dialog if cad findings available.
//
// Revision 1.7  2005/09/14 14:41:53  jmeade
// cad results dialog, initial checkin.
//
// Revision 1.6  2005/09/09 14:31:32  geconomos
// Issue# 4349: Image resolution not increasing at higher framerates.
//
// Revision 1.5  2005/09/06 17:11:17  geconomos
// Issue #4412: Movement in 2d causing hiccups during endoluminal flight
//
// Revision 1.4  2005/08/26 17:07:05  geconomos
// Prevent redraws on mose move while 3d is flying
//
// Revision 1.3  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.35.2.18.2.16.2.5  2005/07/18 18:27:25  geconomos
// fixed FILE_REVISION macro
//
// Revision 3.35.2.18.2.16.2.4  2005/07/15 18:25:45  geconomos
// Added methods to invalidate parent window where external controls are placed.
//
// Revision 3.35.2.18.2.16.2.3  2005/07/06 15:54:39  geconomos
// fixed bug in GlRedrawViews()
//
// Revision 3.35.2.18.2.16.2.2  2005/07/05 16:12:36  geconomos
// Issue #4243: In standard view, non-main 2d windows no linger  updating while flying main view 2d.
//
// Revision 3.35.2.18.2.16.2.1  2005/06/28 21:47:40  jmeade
// Issue 4186: Show distance to rectum when overview in main view.
//
// Revision 3.35.2.18.2.16  2005/06/14 22:13:59  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 3.35.2.18.2.15  2005/06/14 19:13:27  geconomos
// Un-did changes to high resolution 3d rendering.
//
// Revision 3.35.2.18.2.14  2005/06/10 18:20:38  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 3.35.2.18.2.13  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.35.2.18.2.11.2.2  2005/06/08 14:01:11  geconomos
// continued effort ( sorry for lame comment! )
//
// Revision 3.35.2.18.2.11.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 3.35.2.18.2.12.2.1  2005/06/08 18:24:00  jmeade
// comments.
//
// Revision 3.35.2.18.2.12  2005/04/28 17:29:29  jmeade
// comment.
//
// Revision 3.35.2.18.2.11  2005/04/07 22:18:09  jmeade
// Issue 4109: Let childview class handle update triggers.
//
// Revision 3.35.2.18.2.10  2005/04/05 22:55:25  jmeade
// Issue 4100: updates when show cleansed option.
//
// Revision 3.35.2.18.2.9  2005/04/01 05:00:42  jmeade
// redraws when sizing window, moving views.
//
// Revision 3.35.2.18.2.8  2005/03/31 17:36:52  jmeade
// comments.
//
// Revision 3.35.2.18.2.7  2005/03/31 17:29:42  jmeade
// Issue 4038: Generalized idle time processing.
//
// Revision 3.35.2.18.2.6  2005/03/31 02:08:54  jmeade
// Issue 4038: eliminating some multiple redraws due to dependence on GLChildViewType.
//
// Revision 3.35.2.18.2.5  2005/03/30 21:55:01  jmeade
// Issue 4038: framework changes.
//
// Revision 3.35.2.18.2.4  2005/03/29 05:03:50  jmeade
// Issue 4038.
//
// Revision 3.35.2.18.2.3  2005/03/28 20:44:55  jmeade
// Issue 4066.
//
// Revision 3.35.2.18.2.2  2005/03/22 15:26:38  frank
// Issue #3860: Fixed hesitation when starting 3D reverse flight
//
// Revision 3.35.2.18.2.1  2005/03/12 14:36:03  jmeade
// Snapshots wit 2d measurements interpreted as polyps (as with 3d measurements).
//
// Revision 3.35.2.18  2005/02/15 20:08:00  jmeade
// merge from vc1.4 branch.
//
// Revision 3.35.2.17.2.2  2004/12/14 20:10:12  jmeade
// .
//
// Revision 3.35.2.17.2.1  2004/11/18 02:02:52  jmeade
// Render only captured view when appropriate.
//
// Revision 3.35.2.17  2004/09/23 17:37:55  jmeade
// Toggle between adjust translucency and flight controls in menu.
//
// Revision 3.35.2.16  2004/09/21 18:13:36  jmeade
// Items to toggle between flight and adjust translucency controls in menubar.
//
// Revision 3.35.2.15  2004/05/05 23:22:25  jmeade
// switch btw adjust translucency and control panel with 'a' key.
//
// Revision 3.35.2.14  2003/07/25 21:45:11  jmeade
// Issue 3401: Let child windows handle their own command messages.
//
// Revision 3.35.2.13  2003/07/24 17:32:55  jmeade
// Issue 3401: Logic of which views to draw to be controlled by the individual views.
//
// Revision 3.35.2.12  2003/07/17 19:54:51  jmeade
// Comments.
//
// Revision 3.35.2.11  2003/04/25 18:03:28  jmeade
// Removed commented code.
//
// Revision 3.35.2.10  2003/04/15 21:58:07  jmeade
// Issue 3001: Use original layout view rects, allow child view resizing (particularly for NavigationView).
//
// Revision 3.35.2.9  2003/04/14 16:50:20  jmeade
// Issue 2841: Handle delete all measurements msg.
//
// Revision 3.35.2.8  2003/03/17 21:24:16  jmeade
// Issue 2888:  You think I'd follow a rule I created (in Revision 3.35.2.6).
//
// Revision 3.35.2.7  2003/03/12 20:33:41  jmeade
// Issue 2827:  Allow user to select which view to capture for movie.
//
// Revision 3.35.2.6  2003/03/07 04:41:29  jmeade
// Issue 2888: To enable snapshot, derived classes have to declare both command and command_ui handlers.
//
// Revision 3.35.2.5  2003/02/27 03:25:28  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.35.2.4  2003/02/22 19:06:41  jmeade
// Issue 2837:  Install non-overriden routine to capture movie frames during recording.
//
// Revision 3.35.2.3  2003/02/22 12:20:27  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.35.2.2  2003/02/19 22:36:26  jmeade
// Issue 2842:  Implemented variable number of snapshots per report entry.
//
// Revision 3.35.2.1  2003/02/12 01:07:48  jmeade
// Clarifying names for 2D flight modes.
//
// Revision 3.35  2003/02/10 17:12:53  jmeade
// Issue 2832: Redundant, excess code for snapshot notifications.
//
// Revision 3.34  2003/01/29 00:48:14  jmeade
// Distance from rectum text display determined by parent window), not by IsMainWindow();
// Fixed two method declarations.
//
// Revision 3.33  2003/01/29 00:47:51  jmeade
// Distance from rectum text display determined by parent window), not by IsMainWindow();
// Fixed two method declarations.
//
// Revision 3.32  2002/12/12 22:07:52  jmeade
// Unused or unneeded view modes code cleanup.
//
// Revision 3.31  2002/11/27 22:31:15  jmeade
// Reverse of paradigm for popup menus:  assume show popup menu on right-click in childview,
//  have view window instances validate/refuse popup as necessary; Cleanup of unneeded code.
//
// Revision 3.30  2002/11/22 00:29:55  jmeade
// Coding standard on m_trackball* variables.
//
// Revision 3.29  2002/11/19 20:24:35  jmeade
// Comments.
//
// Revision 3.28  2002/11/15 00:39:19  jmeade
// No hard-coding of # of child views; Hide child view storage (use access methods).
//
// Revision 3.27  2002/09/06 16:10:26  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.26  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.24  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.23  2002/06/04 18:37:00  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.22  2002/05/09 20:54:15  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_05-06-2002)
//
// Revision 3.21  2002/04/26 15:57:32  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.20  2002/04/18 18:33:50  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.19  2002/04/12 02:15:09  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.18  2002/03/21 02:08:28  jmeade
// Fixed render speed slowing caused by rendering cross-section view
//  introduced in v3.3.
//
// Revision 3.17  2002/03/13 22:01:41  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.16  2002/02/06 20:33:58  binli
// issue 1796: update indicators when stop flying (3d & 2d flight)
// (message WM_APP_2DFLIGHT_UPDATEVIEW)
//
// Revision 3.15.2.14  2002/08/01 14:46:47  jmeade
// Issue 2587:  Made a number of 2D slice options global.
//
// Revision 3.15.2.13  2002/06/17 18:19:24  jmeade
// Issue 2453:  Add bookmark override in base, enabled for any view with an overview.
//
// Revision 3.15.2.12  2002/05/21 20:17:03  jmeade
// Issue 2271:  "Delete all" menu items for measurements and bookmarks.
//
// Revision 3.15.2.11  2002/04/27 00:19:52  jmeade
// Issue 2063:  Handle overview refresh in base class (i.e. in all views).
//
// Revision 3.15.2.10  2002/04/24 18:13:45  jmeade
// Issue 2044:  Override default base class setting of current ViatronStudy view
//  (current ViatronStudy view here is that in the center view).
//
// Revision 3.15.2.9  2002/04/03 01:03:23  jmeade
// Don't move mouse cursor on snapshot, the user is not expecting it;
// Removed unnecessary override.
//
// Revision 3.15.2.8  2002/03/28 22:30:49  jmeade
// Fixed render speed slowing caused by rendering cross-section view
//  introduced in v3.3.
//
// Revision 3.15.2.7  2002/03/05 20:17:33  binli
// adjustment of 3D measurement line drawing.
//
// Revision 3.15.2.6  2002/03/01 17:01:26  binli
// issue 1966: when measurement line is visible, default to 'polyp'
//
// Revision 3.15.2.5  2002/02/26 16:54:15  binli
// issue 1543: inverse image when taking snapshot.
//
// Revision 3.15.2.4  2002/02/22 20:40:38  binli
// issue 1543: 3> cursor in the center of mainviewwindow.
//
// Revision 3.15.2.3  2002/02/21 18:12:06  jmeade
// Issue 1910:  Push handlers for bookmark menu to ViatronViewWnd; enable bookmarks for Endo-Ortho view.
//
// Revision 3.15.2.2  2002/02/18 16:10:08  binli
// issue 1863: send message to StandardView/EndoSliceView when there is action of mousewheel/change scrollbar.
//
// Revision 3.15.2.1  2002/02/11 18:02:15  binli
// issue 1796: update indicators when stop flying (3d & 2d flight)
//
// Revision 3.15  2001/01/01 06:02:51  binli
// issue 1588: response to message 'WM_APP_2DAUTOFLIGHT_UPDATE'
//
// Revision 3.14  2002/01/23 15:57:09  jmeade
// Coding standards.
//
// Revision 3.13  2002/01/09 16:17:26  jmeade
// Movie record button implemented.
//
// Revision 3.12  2001/12/22 00:13:04  jmeade
// Moved movie recording (for use in other views); Code standards.
//
// Revision 3.11  2001/11/20 22:03:02  binli
// solved problem of unstable slice image: reset viewport when rendering buffered slice image
// (SliceChildView: try to center the slice, so viewport is different from viewwnd.)
//
// Revision 3.10  2001/11/19 15:54:15  jmeade
// Standard view form.
//
// Revision 3.9  2001/11/17 03:35:36  jmeade
// Patient view stored in base class.
//
// Revision 3.8  2001/11/11 19:56:09  kevin
// Fixes for rdrGlobal.m_pStudy when renderin 2 endoscopic views in registration mode
//
// Revision 3.7  2001/11/09 22:25:06  binli
// slice flight.
//
// Revision 3.6  2001/11/09 04:50:21  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.5  2001/11/09 00:42:49  jmeade
// Added control bar to registration view.
//
// Revision 3.4  2001/11/08 20:43:34  binli
// 2D flight
//
// Revision 3.3  2001/11/06 21:50:03  binli
// 2D flight along centerline
//
// Revision 3.2.2.5  2001/10/29 18:46:00  binli
// followed Jeff M's change on issue 1278
//
// Revision 3.2.2.4  2001/10/29 16:39:47  binli
// no call of SliceChildView fucntion, as suggested by Jeff M.
//
// Revision 3.2.2.3  2001/10/28 11:11:27  jmeade
// Coding standards; Safer references instead of pointers
//
// Revision 3.2.2.2  2001/10/28 04:43:01  jmeade
// Proper fix to Issue 1278: Child view specific code does not belong
// in parent views.
//
// Revision 3.2.2.1  2001/10/26 15:11:49  binli
// issue 1278: no call of ShowPopupMenu() when R-ButtonDown+Ctrl.
//
// Revision 3.2  2001/10/19 02:46:37  jmeade
// Code standards
//
// Revision 3.1  2001/10/17 21:57:57  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:02:00  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.6   Oct 12 2001 23:05:40   jmeade
// Properly handle reset of main view when resizing dialog
// 
//    Rev 2.5   Oct 08 2001 17:03:48   jmeade
// Consolidated code specifying study orientation; allow creator of NavigationView to specify study orientation
// 
//    Rev 2.4   Oct 05 2001 02:58:14   jmeade
// Used OnSize() override, OnSetMainWnd() to save and restore that last main view window
// 
//    Rev 2.3   Oct 03 2001 14:51:56   jmeade
// Removed duplicate Bookmark menu update code
// 
//    Rev 2.2   Oct 03 2001 12:15:22   jmeade
// Snapshot function moved to ViatronViewWnd for all to use; Menubar break in bookmarks after 20; comments, coding standards
// 
//    Rev 2.1   Sep 26 2001 23:54:38   jmeade
// Bug:  Displaying opposite study's bookmark list in menu, grayed, after switch between.  Fix:  Delete previous list of bookmarks as first operation on updating list
// 
//    Rev 2.0   Sep 16 2001 23:42:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:58   ingmar
// Initial revision.
// Revision 1.109  2001/09/05 17:01:15  jmeade
// Returning the snapshot sound
//
// Revision 1.108  2001/08/29 14:50:00  binli
// task 178: updated ROI
//
// Revision 1.107  2001/08/28 21:47:25  jmeade
// Enable add bookmarks menu only when a study is open
//
// Revision 1.106  2001/08/22 02:22:55  jmeade
// Removed CAxialSliceView class
//
// Revision 1.105  2001/08/22 01:14:31  jmeade
// Better encapsulation in OverView (ResetTimer has no meaning outside of class)
//
// Revision 1.104  2001/08/16 14:24:38  binli
// bug 000486:
//
// Revision 1.103  2001/08/10 18:01:33  jmeade
// generalized bug fix for popup menu
//
// Revision 1.102  2001/07/31 16:38:31  binli
// appdix to bug fixing of OverView update.
//
// Revision 1.101  2001/07/30 22:41:10  binli
// bug fixed: update overview when change slices in StandardView/EndoAxialView.
//
// Revision 1.100  2001/07/05 16:04:38  binli
// bug 000536: fixed.
// No to show up popup menu, if: (1) click not in a view, (2) shift/control is held
// (little appendix to bug 542 fixing)
//
// Revision 1.99  2001/06/26 14:44:41  binli
// rewrote the control logic to rerender overview: made memory stable.
//
// Revision 1.98  2001/06/21 21:15:32  binli
// speed up overview/slice render in StandardView/Endo-OrthogonalView.
// (defect 263, partial)
//
// Revision 1.97  2001/05/17 20:42:05  binli
// VerifyView vs. StandardVIew/S&PView: controled under different trackball.
//
// Revision 1.96  2001/05/15 17:49:38  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.95  2001/05/12 00:01:00  jmeade
// Coding conventions
//
// Revision 1.94  2001/04/30 12:32:54  frank
// Reflected changes in Study class.
//
// Revision 1.93  2001/04/30 02:20:04  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.92  2001/04/16 13:49:36  geconomos
// Removed snapshot sound!!!
//
// Revision 1.91  2001/04/13 02:34:36  jmeade
// headers and footers
//
// Revision 1.90  2001/03/28 22:33:29  jmeade
// sizes views according to view window height
//
// Revision 1.89  2001/03/21 15:47:27  geconomos
// Fixed bug with read only report based on user level
//
// Revision 1.88  2001/03/19 18:18:13  binli
// no to access protect/private variables now.
//
// Revision 1.87  2001/03/09 22:06:03  geconomos
// Fixed some STUPID TYPO bugs!!!
//
// Revision 1.86  2001/03/08 23:09:12  jmeade
// HARD-CODED STRINGS!!!  OH THE HORROR!!!
//
// Revision 1.85  2001/03/08 16:34:55  geconomos
// Added read only only report functionailty.
//
// Revision 1.84  2001/03/08 03:04:25  jmeade
// sound and text notification for adding bookmark
//
// Revision 1.83  2001/03/07 15:28:48  jmeade
// coding standards
//
// Revision 1.82  2001/03/07 15:27:17  geconomos
// Extended reporting to take a snapshot of main window.
//
// Revision 1.81  2001/03/07 15:26:16  geconomos
// Extended reporting to take a snapshot of main window.
//
// Revision 1.80  2001/03/07 15:01:31  jmeade
// bookmarks menu only for standard view; new coding standards;
// play sound on report snapshot
//
// Revision 1.79  2001/02/28 18:52:46  jmeade
// tabs to spaces (no code changes)
//
// Revision 1.78  2001/02/27 23:44:38  jmeade
// Study::GetOrientation() for m_eOrientation query
//
// Revision 1.77  2001/02/23 20:11:32  jmeade
// Removed most unnecessary 'C' before class names (everything's a class!)
//
// Revision 1.76  2001/02/21 20:33:42  jeff
// COverview constructor now takes a StudyOrientation as a parameter (Study parameters unused/obsolete)
//
// Revision 1.75  2001/02/12 17:21:00  geconomos
// Only allow snapshot if study is loaded
//
// Revision 1.74  2001/02/11 04:58:20  jeff
// cross-section sub menu for eGVT_CROSS_SECTION
//
// Revision 1.73  2001/02/07 20:28:02  jeff
// set new main view window prior to switching view rectangles
//
// Revision 1.72  2001/02/07 20:23:03  frank
// Removed gray-out functionality.
//
// Revision 1.71  2001/02/06 14:16:29  frank
// Revamped study load progress display.
//
// Revision 1.70  2001/02/05 22:50:00  liwei
// Flag showing whether non-active views grayed out moved to ViatronViewWnd
//
// Revision 1.69  2001/02/05 22:19:30  liwei
// Non-active views grayed out during flying
//
// Revision 1.68  2001/02/05 21:23:22  jeff
// always render centerline as gradient in PICK or VERIFY mode (no need for separate mode)
//
// Revision 1.67  2001/02/05 18:51:01  frank
// Removed the centerline gradient in all-but-verify view.
//
// Revision 1.66  2001/02/02 12:35:05  frank
// Made the navigation come to a complete stop when dbl-clicking on non-nav views.
//
// Revision 1.65  2001/02/01 00:55:45  jeff
// private member access
//
// Revision 1.64  2001/01/31 19:47:57  kevin
// Fixed a compile bug when I finally remove mprview.h
//
// Revision 1.63  2001/01/31 18:20:17  jeff
// CreateDlgBar(): doesn't need to take in a pointer
//
// Revision 1.62  2001/01/31 13:09:40  frank
// Stopped autofly on view swap.
//
// Revision 1.61  2001/01/30 04:41:32  kevin
// FIxed the live update of painted information
//
// Revision 1.60  2001/01/27 02:57:13  kevin
// Hooked up the display paintinfo to the overview.
// (note there are still some bugs with the grab image,
// but I can't seemm to get it to connect from readerlib
// into viatron. ARGGHH I am a coding dinosaur)
//
// Revision 1.59  2001/01/27 02:02:01  kevin
// 1) Fixed 90% missed Patch error
// 2) Moved GetPotenetil to fields from Submarine
// as part of this fix.
//
// Revision 1.58  2001/01/26 19:59:41  binli
// defect 000188: default Overview position
//
// Revision 1.57  2001/01/25 04:29:38  jeff
// supine/prone toggle (combobox) in patient info dialog (will "sexy" it up later);
// doesn't yet change on command in verifyview
//
// Revision 1.56  2001/01/23 15:50:24  frank
// Completely stopped submarine during missed patch jumping.
//
// Revision 1.55  2001/01/13 00:51:01  jeff
// Using new Rect<> and Tuple<> classes; more CPoint and CRect translation convenience functions
//
// Revision 1.54  2001/01/11 23:46:28  kevin
// Added navigation based missed patch viewing
//
// Revision 1.53  2001/01/11 22:40:38  geconomos
// Added handler for adding report snapshot
//
// Revision 1.52  2001/01/08 02:43:40  kevin
// Cleaned up missed patch viewing by multiple methods
//
// Revision 1.51  2001/01/05 19:27:40  frank
// Moved marking functions into Study class.
//
// Revision 1.50  2001/01/03 00:17:20  jeff
// pass events to immediate base class (ViatronViewWnd not GLViewWnd)
//
// Revision 1.49  2001/01/02 23:02:32  frank
// Stopped flying when jumping to next missed region.
//
// Revision 1.48  2000/12/28 19:52:08  kevin
// Added new algo to compute viewpoint and dir for missed regions
//
// Revision 1.47  2000/12/28 00:04:03  jeff
// global GetViatronMainFrame() ==> static MainAppFrame::GetMainFrame();
// TRANSVERSE ==> AXIAL, MPR & CROSSSECT ==> CROSS_SECTION
//
// Revision 1.46  2000/12/26 23:55:36  jeff
// don't show axial slice orientation popup menu on standard view
//
// Revision 1.45  2000/12/22 16:41:46  jeff
// rbuttonup bug fixed in GLViewWnd::ShowPopupMenu()
//
// Revision 1.44  2000/12/21 16:35:50  kevin
// SMall steps towards mreging slicesview and mpr view
//
// Revision 1.43  2000/12/21 00:51:27  jeff
// Spread out child view windows
//
// Revision 1.42  2000/12/20 19:52:21  jeff
// Reducing file proliferation (added StandardViewFrame class to the file)
//
// Revision 1.41  2000/12/15 18:02:09  jeff
// Implemented navigation view tab selection
//
// Revision 1.40  2000/12/12 23:50:49  jeff
// adjustments to view window placement; popup menu displayed for main view
//
// Revision 1.39  2000/12/12 18:09:40  jeff
// missed patch list --> controls dialog
//
// Revision 1.38  2000/12/08 16:18:47  jeff
// Correct switch to report view (on generate report)
//
// Revision 1.37  2000/11/25 03:05:51  kevin
// Fixed multi-segment patch jumping
//
// Revision 1.36  2000/11/25 00:28:08  jeff
// Ensure a specified new main window is not already the main
//
// Revision 1.35  2000/11/24 20:48:39  kevin
// Fixed bug on jump before missed patches found
//
// Revision 1.34  2000/11/24 20:37:01  kevin
// FIxed crash on jump to next when no missed regions yet
//
// Revision 1.33  2000/11/24 17:25:04  jeff
// Moved PatchListView::Create() to .C, added button icons
//
// Revision 1.32  2000/11/24 18:25:21  kevin
// Added ExitReplayMode on missed patch jump
//
// Revision 1.31  2000/11/23 00:56:22  jeff
// PatchListView (listbox and buttons) class
//
// Revision 1.30  2000/11/22 20:57:23  jeff
// Removed second check for main view:
//    (currView->ClassInstanceID() == GetMainViewWndID(0,0))
// before displaying popup menu
//
// Revision 1.29  2000/11/22 19:18:03  kevin
// Added grab image to auto-detect (since it also updates paint info)
//
// Revision 1.28  2000/11/22 02:23:06  binli
// followed changes (made by Jeff) of the rendering order (verify->standard->Supine & Prone views)
// the m_trackball defined in rdrGlobal has replaced by those defined in m_pStudy and m_pSecondStudy.
//
// Revision 1.27  2000/11/21 05:42:29  jeff
// Condensed patchListView debug code and enclosed in #ifdef _DEBUG
//
// Revision 1.26  2000/11/20 19:02:58  kevin
// Fixed the overview buffer on patch jump
//
// Revision 1.25  2000/11/16 17:41:05  kevin
// Added cliking on string in patchListBox
//
// Revision 1.24  2000/11/14 23:51:48  liwei
// Add support for VolumePro
//
// Revision 1.23  2000/11/14 18:56:21  frank
// Changed study reference to pointer
//
// Revision 1.22  2000/11/14 18:36:50  binli
// followed the changes of Overview constructor.
//
// Revision 1.21  2000/11/13 19:26:28  binli
// changed construction of OverView (Supine/Prone application)
//
// Revision 1.20  2000/11/10 22:36:48  binli
// worked on both Supine/Prone overview rendering. In global variable 'rdrGlobal',
// two local trackballs are added. Some changes are made in TrackBall and OverView.
// Followed the change in OverView, VerifyView, EndoVisView, StandardView are
// changed a little.
//
// Revision 1.19  2000/11/09 20:11:29  frank
// Moved Study-specific information to study class from ReaderGlobal
//
// Revision 1.18  2000/11/09 02:55:29  kevin
// Many changes to different methods of visible surface determination
//
// Revision 1.17  2000/11/08 22:35:04  jeff
// Increased font on patient info dialog
//
// Revision 1.16  2000/11/08 17:39:18  frank
// Added m_study to ReaderGlobal
//
// Revision 1.15  2000/11/06 20:31:39  jeff
// Moved patchListView string addition to CreateChildViews() method
//
// Revision 1.14  2000/11/04 00:54:26  jeff
// Generate report, add snapshot handlers
//
// Revision 1.13  2000/11/02 17:37:15  kevin
// Added "not-yet-seen" list box
//
// Revision 1.12  2000/10/25 17:58:30  jeff
// Popup menu handlers generalized in GLViewWnd (base class)
//
// Revision 1.11  2000/10/14 22:02:23  jeff
// Oops!!  When flying, we were rendering the first orthogonal view (holdover
// from when that view was the visible surface view, as with EndoVisiView)
//
// Revision 1.10  2000/10/14 20:43:46  jeff
// Patient info dialog
//
// Revision 1.9  2000/10/14 18:57:06  kevin
// Temporary comment of paitent Info and Logo for frame rate
//
// Revision 1.8  2000/10/14 01:21:09  jeff
// Canned patient information
//
// Revision 1.7  2000/10/13 22:19:25  jeff
// Fixed location (not based on the moving view windows) for the patient ID frame
//
// Revision 1.6  2000/10/13 17:31:13  jeff
// no message
//
// Revision 1.5  2000/10/13 16:52:01  kevin
// Added ViatronViewWnd Message Map
//
// Revision 1.4  2000/10/13 04:59:11  jeff
// A bunch of changes
//
// Revision 1.3  2000/10/12 21:07:41  kevin
// Added bufferImage flag to ovwerview constructor
//
// Revision 1.2  2000/10/05 00:31:22  jeff
// Added the EndoVisiView (visible-surface view) and removed visible surface view from StandardView
//
// Revision 1.1  2000/10/05 00:00:23  jeff
// Changed name from MainVtView... to StandardView...
//
// Revision 1.21  2000/10/02 21:56:46  jeff
// Changes for staggered study load (had to check in because my cheapo computer is crashing)
//
// Revision 1.20  2000/09/30 22:17:04  jeff
// ViatronViewWnd class for more study-specific, V1K specific, etc. things
//
// Revision 1.19  2000/09/30 18:30:21  jeff
// Fix the problem of the moving dialog bars
//
// Revision 1.18  2000/09/30 02:54:22  jeff
// GetViewRect, SetViewRect, SwapViewRects access functions
//
// Revision 1.17  2000/09/27 14:45:07  jeff
// Readjusting/resizing views to fill up unused parent view space
//
// Revision 1.16  2000/09/26 19:05:18  jeff
// Moved slice dialog bars, then eventually removed them
//
// Revision 1.15  2000/09/22 23:17:49  jeff
// button menus
//
// Revision 1.14  2000/09/21 19:45:21  kevin
// Changes to unwrapped display
//
// Revision 1.13  2000/09/19 16:12:29  kevin
// Commented grey-screen debug (After 4 days with no success, I give up)
//
// Revision 1.12  2000/09/18 15:07:05  kevin
// Added GLDebug to find grey-screen error
//
// Revision 1.11  2000/09/15 22:46:24  jeff
// no message
//
// Revision 1.10  2000/09/15 19:21:05  jeff
// Updating views only when necessary
//
// Revision 1.9  2000/09/15 16:09:28  jeff
// All rendering/drawing now done in OpenGL (no more GDI draw calls)
//
// Revision 1.8  2000/09/15 01:58:25  jeff
// Moving to text draw in OpenGL
//
// Revision 1.7  2000/09/14 16:14:51  jeff
// Borders around all view windows
//
// Revision 1.6  2000/09/14 14:52:25  jeff
// GetGLChildViewWndType() reduced to GetViewWndType();
// GLChildViewWndType reduced to ViewWndType
//
// Revision 1.5  2000/09/13 19:23:32  jeff
// Send a mouse move message (effectively setting current cursor)
// after setting a new main view window
//
// Revision 1.4  2000/09/13 15:34:04  jeff
// Visible surface view
//
// Revision 1.3  2000/09/12 20:47:03  jeff
// Resizing and repositioning overview and (coming soon) visible-surface views
//
// Revision 1.2  2000/09/11 22:20:16  jeff
// Corrected a comment
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 26    9/08/00 5:09p Jeff
// GetSafeWinHdc() returns m_pCDC->GetSafeHdc()
// 
// 25    9/07/00 1:32p Jeff
// Embedded control dialog ("bars") in the GLChildView class, resized in
// OnSize
// 
// 24    9/06/00 10:55a Jeff
// 
// 23    9/05/00 5:25p Jeff
// Moving around views and "toolbars"
// 
// 22    9/03/00 6:44p Kevin
// Added update of ovreview window without redrawing all the
// ploygons by grabbing a texture image. Then the position
// can be updated ont he display constantly. Also added
// double click for bookmarks
// 
// 21    9/01/00 12:10p Jeff
// Another 'modes' incremental checkin
// 
// 20    8/31/00 9:09a Jeff
// Incremental checkin (for view mode changes)
// 
// 19    8/29/00 6:42p Jeff
// Mode changes
// 
// 18    8/28/00 5:35p Jeff
// Bug: nFlags is a bitfield (and should be tested using &, not ==)
// 
// 17    8/28/00 2:13p Jeff
// Overriding OnRButtonDown to give popup menus (same across all views)
// 
// 16    8/28/00 1:25p Antonio
// updated header and footer
//
//*****************  Version 15  *****************
//User: Jeff            Date:  8/25/00  Time: 12:15p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Allow GLViewWnd derivates to redirect broadcasted messages
//
//*****************  Version 14  *****************
//User: Jeff            Date:  8/24/00  Time: 11:24a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Adjusting view window positions
//
//*****************  Version 13  *****************
//User: Jeff            Date:  8/24/00  Time: 10:12a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Added slice view dialog "bars"
//
//*****************  Version 12  *****************
//User: Jeff            Date:  8/24/00  Time:  6:26a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Overview dialog bar
//
//*****************  Version 11  *****************
//User: Jeff            Date:  8/23/00  Time:  3:38p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Implemented handler for WM_GET_MAIN_VIEW_WND_CLASSID
//
//*****************  Version 10  *****************
//User: Jeff            Date:  8/23/00  Time: 12:46p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Overridding OnGLChildViewInvalidate
//
//*****************  Version 9  *****************
//User: Jeff            Date:  8/22/00  Time: 11:38p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Pass the WM_ENDOVIEW_MODE_CHANGED message along to the controls
//dialog
//
//*****************  Version 8  *****************
//User: Jeff            Date:  8/18/00  Time: 12:23a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Removed m_flying from ReaderGlobal
//
//*****************  Version 7  *****************
//User: Jeff            Date:  8/17/00  Time: 10:00p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  No need to update the controls view everytime
//
//*****************  Version 6  *****************
//User: Kevin           Date:  8/17/00  Time:  9:48p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Fixed window jumping around
//
//*****************  Version 5  *****************
//User: Jeff            Date:  8/16/00  Time:  8:47p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  
//
//**********************
//Label: Version 0.1
//User: Jeff            Date:  8/15/00  Time:  5:43p
//Labeled 'Version 0.1'
//Label comment:
//  First half-decent working version
//
//*****************  Version 4  *****************
//User: Jeff            Date:  8/15/00  Time:  5:41p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Changed display order of slice views
//
//*****************  Version 3  *****************
//User: Jeff            Date:  8/12/00  Time: 10:45p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Added the Endoscopic-Axial view window (first beta)
//Added a small edit field to dialog bar to test that it doesn't "steal" the mouse
//(it doesn't)
//Many other class organizational changes for (multiple view windows)
//
//*****************  Version 2  *****************
//User: Jeff            Date:  8/12/00  Time:  8:24p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Created two overriddable functions for GLViewWnd derivates
//(GLRenderViews(default renders all views) and PostGLRenderViews(default calls
//PostGLRender() for all views))
//
//*****************  Version 1  *****************
//User: Jeff            Date:  8/12/00  Time:  8:00p
//Created MainVtView.cpp
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/StandardView.C,v 1.21.2.14 2010/09/20 18:41:34 dongqing Exp $
// $Id: StandardView.C,v 1.21.2.14 2010/09/20 18:41:34 dongqing Exp $
