// $Id: Segments3DView.C,v 1.28.2.28 2010/04/17 08:33:59 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Kevin Kreeger (mailto:kevin@viatronix.com)


// includes
#include "stdafx.h"
#include "Segments3DView.h"
#include "NavigationView.h"
#include "SliceChildView.h"
#include "OverView.h"
#include "V1KMessages.h"
#include "Plane.h"
#include "Segment3DChildView.h"
#include "Theme.h"
#include "DrawOverView.h"


// defines
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// namespaces
using namespace ColonViews::Layouts;
using namespace ReaderLib;
using namespace WinUtils;


/////////////////////////////////////////////////////////////////////////////
// Segments3DLayoutFrame

IMPLEMENT_DYNCREATE(Segments3DLayoutFrame, CMDIChildWnd)

/**
 * constructor
 */
Segments3DLayoutFrame::Segments3DLayoutFrame() : ChildLayoutFrame(new Segments3DLayout)
{
} // Segments3DLayoutFrame()

//{{AFX_MSG_MAP(Segments3DLayoutFrame)
//}}AFX_MSG_MAP


//////////////////////////////////////////////////////////////////////
// Segments3DLayout Class

/**
 * constructor
 */
Segments3DLayout::Segments3DLayout() : ColonViatronixWnd()
{
} // Segments3DLayout()


/**
 * message map
 */
BEGIN_MESSAGE_MAP(Segments3DLayout, ColonViatronixWnd)
  //{{AFX_MSG_MAP(Segments3DLayout)
  ON_COMMAND(IDM_JUMP_PREVSEGMENT, OnJumpPrevSegment)
  ON_COMMAND(IDM_JUMP_NEXTSEGMENT, OnJumpNextSegment)
  ON_COMMAND(IDM_REPORT_ADDSNAPSHOT, OnReportAddSnapshot)
  ON_MESSAGE(WM_APP_QUERY_ENABLE_SUPINE_PRONE_TOGGLE, OnQueryEnableSupineProneSwitch)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE, OnToggleLineMeasure)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_LIGHTING, OnToggleLighting)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_ROIMEASURE, OnToggleRoiMeasure)
  ON_COMMAND(ID_ENDOSCOPICVIEW_OPTIONS_TRANSLUCENT_DISPLAY, OnToggleTranslucent)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION, OnToggleSegmentRegion)
  ON_UPDATE_COMMAND_UI(IDM_REPORT_ADDSNAPSHOT, OnUpdateReportAddSnapshot)
  ON_COMMAND(ID_OPTIONS_CONTROLPAD_FLIGHTCONTROLS, OnControlPadFlightControls)
  ON_COMMAND(ID_OPTIONS_CONTROLPAD_ADJUSTTRANSLUCENCY, OnControlPadAdjustTranslucency)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_CONTROLPAD_FLIGHTCONTROLS, OnUpdateControlPadMenuItemAdjust)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_CONTROLPAD_ADJUSTTRANSLUCENCY, OnUpdateControlPadMenuItemAdjust)
  ON_COMMAND(IDM_MEASUREMENTS_DELETEALL, OnMeasurementsDeleteAll)
  ON_UPDATE_COMMAND_UI(IDM_MEASUREMENTS_DELETEALL, OnUpdateMeasurementsDeleteAll)
  ON_COMMAND(IDM_TOGGLE_AUTOFLY, OnToggleAutofly)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * Renders the child views
 */
void Segments3DLayout::GLRenderViews()
{
  // update the navigation , overview & UD View no more than time per second when interacting
  const float4 fMinimumUpdateTime = 0.2F;
  bool bInvalidateEndoluminalView = false;
  bool bInvalidateOverview = false;
  bool bInvalidateUDview = false;

  if( rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() )
  {
    Timer timer;
    GLRenderView( ePOS_OVERVIEW );
    GLRenderView( ePOS_NAVIGATION );

    rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.SetLastRenderTime( timer.ElapsedTime() );
  }
  else if( ViatronixWnd::m_uInvalidChildViews != uint4( GLChildView::eGVT_NONE ) )
  {
    if( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_AXIAL    || 
      ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_SAGITTAL ||
      ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_CORONAL  ||
      ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_CROSS_SECTION     )
    {
      GLRenderView( ePOS_ORTHO );
      GLRenderView( ePOS_OBLIQUE );
    }


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

    if( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_OVERVIEW  ) 
    {
      if ( m_lastNavigationUpdateTimer.ElapsedTime() > fMinimumUpdateTime )
      {
        GLRenderView( ePOS_OVERVIEW );
        m_lastNavigationUpdateTimer.Reset();
      }
      else
      {
        bInvalidateOverview = true;
      }
    }

    if ( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_3DSEGMENTS )
    {
      if ( m_lastOverviewUpdateTimer.ElapsedTime() > fMinimumUpdateTime )
      {
        GLRenderView( ePOS_UDVIEW );
        m_lastUDViewUpdateTimer.Reset();
      }
      else
      {
        bInvalidateUDview = true;
      }
    }
  }
  m_uInvalidChildViews = uint4( GLChildView::eGVT_NONE );

  // keep the navigation view and overview unupdated if we did not update it in this cycle
  if ( bInvalidateEndoluminalView )
    m_uInvalidChildViews |= uint4( GLChildView::eGVT_NAVIGATION );
  if ( bInvalidateOverview )
    m_uInvalidChildViews |= uint4( GLChildView::eGVT_OVERVIEW );
  if ( bInvalidateUDview )
    m_uInvalidChildViews |= uint4( GLChildView::eGVT_3DSEGMENTS );

} // GLRenderViews()


/**
 * Keyboard event handler: key pressed
 * @param uChar     key pressed
 * @param uRepCnt   repeat count
 * @param uFlags    current state flags
 */
void Segments3DLayout::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags)
{
  if (!rdrGlobal.m_pCurrDataset->IsLoadingComplete())
  {
    ColonViatronixWnd::OnKeyDown(uChar, uRepCnt, uFlags);
    return;
  }

  switch (uChar)
  {
    case VK_ADD:
    case '+':
    case VK_OEM_PLUS:
    {
      if( rdrGlobal.m_pCurrDataset->m_currentSegment < rdrGlobal.m_pCurrDataset->m_straightSegmentNodes.size()-2 ) // the m_straightSegmentNodes.size() is number of nodes rather than that of segments
      rdrGlobal.m_pCurrDataset->m_currentSegment++;
      break;
    }
    case VK_SUBTRACT:
    case '-':
    case VK_OEM_MINUS:
    {
      if(rdrGlobal.m_pCurrDataset->m_currentSegment>0)
      rdrGlobal.m_pCurrDataset->m_currentSegment--;
      break;
    } 
    default:
    {
      // Let the base class do all the dirty work
      ColonViatronixWnd::OnKeyDown(uChar, uRepCnt, uFlags);
      return;
    }
  } // end of switch

  // Set the viewpoint to be viewing along the axis
  uint4 uIndex1 = rdrGlobal.m_pCurrDataset->m_straightSegmentNodes[rdrGlobal.m_pCurrDataset->m_currentSegment];
  uint4 uIndex2 = rdrGlobal.m_pCurrDataset->m_straightSegmentNodes[rdrGlobal.m_pCurrDataset->m_currentSegment+1];
  Point<float4> Point1 = rdrGlobal.m_pCurrDataset->m_joinedSkeleton.GetWorldPos(uIndex1);
  Point<float4> Point2 = rdrGlobal.m_pCurrDataset->m_joinedSkeleton.GetWorldPos(uIndex2);
  Normal<float4> nViewDirection(Point2-Point1);
  Plane rotationPlane(Point1,nViewDirection);
  Normal<float4> nUp = Normal<float4>(rotationPlane.GetProjectionParallelToPlane(Vector<float4>(rdrGlobal.m_pCurrDataset->m_joinedSkeleton.GetSkelNode(uIndex1).m_vWorldUp)));
  rdrGlobal.m_pCurrDataset->m_submarine.SetView(Point1, nViewDirection, nUp);

  // re-render all images
  rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);

  ((Segment3DChildView&) GetChildViewDlgItem(IDC_TEMPLATE_3DSEGMENTSVIEW)).m_3DsegmentViewer.SetDesiredPlaneAngle( 0 );
  GLRenderViews();

  ColonViatronixWnd::OnKeyDown(uChar, uRepCnt, uFlags);
} // OnKeyDown( uChar, uRepCnt, uFlags )


/**
 * OnPaint message handler
 */
void Segments3DLayout::OnPaint() 
{
  ColonViatronixWnd::OnPaint();

  //Gdiplus::Graphics gfx( GetSafeHwnd() );

  //gfx.DrawImage( Theme::LogoRegistrationView, 0, 0 );
} // OnPaint()


/**
 * Jumps to the next segment towards the rectum.
 */
void Segments3DLayout::OnJumpNextSegment()
{
  OnKeyDown( VK_SUBTRACT, 0, 0 );
} // OnJumpNextSegment()


/**
 * Jumps to the next segment towards the cecum.
 */
void Segments3DLayout::OnJumpPrevSegment()
{
  OnKeyDown( VK_ADD, 0, 0 );
} // OnJumpPrevSegment()


/**
 * Adds a new report entry.
 */
void Segments3DLayout::OnReportAddSnapshot()
{
  if ( !rdrGlobal.m_pCurrDataset->IsLoadingComplete() )
    return;

  std::list<GLChildViewPtr> snapshotList;
  std::list<GLChildViewPtr> displayList;


  DrawOverView ovDrawImage(this, *rdrGlobal.m_pCurrDataset);
  DrawSegmentView sgDraw(this, *rdrGlobal.m_pCurrDataset);

  snapshotList.push_back( (1) ? &ovDrawImage : dynamic_cast<GLChildViewPtr> (GetDlgItem( IDC_TEMPLATE_OVERVIEW )) ); // OVERVIEW
  snapshotList.push_back( (1) ? &sgDraw : dynamic_cast<GLChildViewPtr> (GetDlgItem( IDC_TEMPLATE_3DSEGMENTSVIEW )) ); // SEGMENTS
  snapshotList.push_back( dynamic_cast<GLChildViewPtr> (GetDlgItem( IDC_TEMPLATE_ENDOSCOPIC )) ); // ENDOSCOPIC
  snapshotList.push_back( dynamic_cast<GLChildViewPtr> (GetDlgItem( IDC_TEMPLATE_SLICEVIEW )) ); // SLICE

  displayList.push_back( dynamic_cast<GLChildViewPtr> (GetDlgItem( IDC_TEMPLATE_OVERVIEW )) ); // OVERVIEW
  displayList.push_back( dynamic_cast<GLChildViewPtr> (GetDlgItem( IDC_TEMPLATE_3DSEGMENTSVIEW )) ); // SEGMENTS
  displayList.push_back( dynamic_cast<GLChildViewPtr> (GetDlgItem( IDC_TEMPLATE_ENDOSCOPIC )) ); // ENDOSCOPIC
  displayList.push_back( dynamic_cast<GLChildViewPtr> (GetDlgItem( IDC_TEMPLATE_SLICEVIEW )) ); // SLICE


  AddReportSnapshot( snapshotList, displayList, rdrGlobal.m_pCurrDataset->m_session.m_measureLineList );

} // OnReportAddSnapshot()


/**
 * Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void Segments3DLayout::OnSize( UINT uType, int iCX, int iCY ) 
{
  ColonViatronixWnd::OnSize(uType, iCX, iCY);

  if ( iCX < 1 || iCY < 1 )
  {
    return;
  }

  return;

  //
  // custom resizing code
  //

  CWnd *pWnd = NULL;
  CRect rectCtl;

  // layout the controls
  const int4 iGap = LAYOUT_GAP;
  const int4 iControlsWidth = 330;
  const int4 iControlsHeight = 180;
  const int4 iControlsX = iGap;
  const int4 iLogoY = 80;

  pWnd = GetDlgItem( IDC_TEMPLATE_PATIENTINFO );
  rectCtl = WinUtils::GetClientRect(pWnd);
  const int4 iPatientHeight = rectCtl.Height();
  GetDlgItem( IDC_TEMPLATE_PATIENTINFO )->SetWindowPos( NULL, iControlsX, iLogoY + iGap, iControlsWidth, iPatientHeight, SWP_NOZORDER );

  pWnd = GetDlgItem( IDC_TEMPLATE_CONTROLS );
  rectCtl = WinUtils::GetClientRect(pWnd);
  const int4 iControlsY = iLogoY + iGap*2 + iPatientHeight;
//  const int4 iControlHeight = iCY / 2 - iLogoY - iPatientHeight - 2 * iGap - 136;
  const int4 iControlHeight = (iCY/2-iGap*4)+4 - iControlsY;
  pWnd->SetWindowPos( NULL, iControlsX, iControlsY, iControlsWidth, iControlHeight, SWP_NOZORDER );

  // layout the main segment view
  const int4 iSegmentWidth = iCX - 3 * iGap - iControlsWidth;
  const int4 iSegmentX = 2 * iGap + iControlsWidth;
  const int4 iScrollbarHeight = LAYOUT_SCROLLBAR_THICKNESS;
  const int4 iSegmentHeight = iCY / 2 - 1.5 * iGap - iScrollbarHeight;
  GetDlgItem( IDC_TEMPLATE_3DSEGMENTSVIEW )->SetWindowPos( NULL, iSegmentX, iGap, iSegmentWidth, iSegmentHeight, SWP_NOZORDER );
  const int4 iScrollbarY = iGap + iSegmentHeight + 2;
//  GetDlgItem( IDC_TEMPLATE_3DSEGMENTSCROLL )->SetWindowPos( NULL, iSegmentX, iScrollbarY, iSegmentWidth, iScrollbarHeight, SWP_NOZORDER );

  // layout the endoscopic view, slice-view, and overview
  const int4 iPossibleWidth = ( iCX - 4 * iGap ) / 3;
  const int4 iBuddyHeight = LAYOUT_BUDDY_HEIGHT;
  const int4 iPossibleHeight = iCY / 2 - 1.5 * iGap - iBuddyHeight - iScrollbarHeight;
  const int4 iSelectedSize = min( iPossibleWidth, iPossibleHeight );
  const int4 iGapX = ( iCX - 3 * iSelectedSize ) / 4;
  const int4 iGapY = ( iCY / 2 - iSelectedSize ) / 2;
  const int4 iSliceX = iGapX + iSelectedSize + iGapX;

  const int4 iMiddleY = iScrollbarY + iScrollbarHeight;
  // put the endoscopic first because it is closer to the controls (moving over the overview disables them so we put that out of the way)
  GetDlgItem( IDC_TEMPLATE_ENDOSCOPIC )->SetWindowPos( NULL, iGapX, iMiddleY + iGapY/2, iSelectedSize, iSelectedSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW )->SetWindowPos( NULL, iSliceX, iMiddleY + iGapY/2, iSelectedSize, iSelectedSize, SWP_NOZORDER );

  int4 iOverviewX = iSliceX + iSelectedSize + iGapX;
  int4 iOverviewWidth = iSelectedSize, iOverviewHeight = iSelectedSize;
  if ( SliceChildView::GetScrollBarLocationEnum() == SliceChildView::SBO_RIGHT )
  {
    iOverviewX += LAYOUT_SCROLLBAR_THICKNESS + iGapX/2;
    iOverviewWidth -= LAYOUT_SCROLLBAR_THICKNESS + iGapX/2;
  }
  GetDlgItem( IDC_TEMPLATE_OVERVIEW )->SetWindowPos( NULL, iOverviewX, iMiddleY + iGapY/2, iOverviewWidth, iOverviewHeight, SWP_NOZORDER );

  // tell the panes to re-stack
  //GetDlgItem( IDC_SLIDING_REGISTRATION_PANE )->SetWindowPos( NULL, 0, 0, 100, 500, SWP_NOZORDER );

} // OnSize( uType, iCX, iCY ) 


/**
 * Message: WM_APP_QUERY_ENABLE_SUPINE_PRONE_TOGGLE: Enables switching between Supine/Prone (by PatientInfoDlg)
 *
 * @param WPARAM   Unused
 * @param LPARAM   Unused
 * @return         TRUE to indicate toggling between Supine & Prone allowed
 */
LRESULT Segments3DLayout::OnQueryEnableSupineProneSwitch(WPARAM, LPARAM)
{
  return 1;
} // OnQueryEnableSupineProneSwitch()


/**
 * ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE: Toggles Line Measure Mode.
 */
void Segments3DLayout::OnToggleLineMeasure()
{
  ToggleViewMode(ViewModes::LINEMEASURE);
} // OnToggleLineMeasure()


/**
 * ID_ENDOSCOPICVIEW_CURRENTMODE_LIGHTING: Toggles Lighting Mode.
 */
void Segments3DLayout::OnToggleLighting()
{
  ToggleViewMode(ViewModes::LIGHTING);
} // OnToggleLighting()


/**
 * ID_ENDOSCOPICVIEW_CURRENTMODE_ROIMEASURE: Toggles ROI Measure Mode.
 */
void Segments3DLayout::OnToggleRoiMeasure()
{
  ToggleViewMode(ViewModes::ROIMEASURE);
} // OnToggleRoiMeasure()


/**
 * ID_ENDOSCOPICVIEW_OPTIONS_TRANSLUCENT_DISPLAY: Toggles Translucent Mode.
 */
void Segments3DLayout::OnToggleTranslucent()
{
  if ( (rdrGlobal.m_supine.GetModality() == Dataset::MR) || (rdrGlobal.m_prone.GetModality() == Dataset::MR) )
  {
    return;
  }

  GLChildViewRef view = GetChildView( GLChildView::GLVID( OnGetMainViewWndID( 0, 0 ) ) );

  const ViewModesValue validModes(ViatronStudy::GetCurrentViewModes().GetValid());

  if ( ( typeid( view ) == typeid( SliceChildView ) ) && ( validModes & ViewModes::PSEUDOCOLOR_VALID) == ViewModes::PSEUDOCOLOR_VALID )
  {
    SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEWWINDOW_PSEUDOCOLORDISPLAY,0), 0);
  }
  else if ( view.GetViewWndType() == GLChildView::eGVT_3DSEGMENTS || ( (validModes & ViewModes::BIOPSY_VALID) == ViewModes::BIOPSY_VALID ) )
  {
    // toggle the mode, then ensure that the 3d mode corresponds
    if (rdrGlobal.m_bBiopsyMode = ViatronStudy::m_modeEndoView.Toggle(ViewModes::BIOPSY))
    {
      ViatronStudy::m_modeSegments3D += ViewModes::BIOPSY;
    }
    else
    {
      ViatronStudy::m_modeSegments3D -= ViewModes::BIOPSY;
    }

    InvalidateViews( WPARAM(GLChildView::eGVT_NAVIGATION), 0);
    InvalidateViews( WPARAM(GLChildView::eGVT_3DSEGMENTS), 0);
  }
 
} // OnToggleTranslucent()


/**
 * Gets the resource ID for the popup menu for this window
 *
 * @param childView      Child view whose popup menu will be displayed.
 * @param uPopupMenuID   ID Popup menu requested to be displayed.
 * @return               Resource ID for the popup menu, or 0 for no menu
*/
uint4 Segments3DLayout::OnShowPopupMenu(const GLChildView& childView, const uint4 uPopupMenuID) const
{
  if (uPopupMenuID == IDM_OVERVIEW)
  {
    return IDM_OVERVIEW_ABBREV_PLUS_DISSECTION;
  }
  return uPopupMenuID;
} // OnShowPopupMenu()


/**
 * Updates menu item
 *
 * @param pCmdUD   Command update handler
 */
void Segments3DLayout::OnUpdateControlPadMenuItemAdjust(CCmdUI* pCmdUI)
{
  CWnd *pAdjust = GetDlgItem(IDC_TEMPLATE_ADJUSTTRANSLUCENCY);
  if ( pAdjust == NULL )
  {
    pCmdUI->Enable(FALSE);
  }

  if ( pCmdUI->m_nID == ID_OPTIONS_CONTROLPAD_FLIGHTCONTROLS )
    ColonViatronixWnd::OnUpdateControlPadFlightControls(pCmdUI);
  else
    ColonViatronixWnd::OnUpdateControlPadAdjustTranslucency(pCmdUI);
}  // OnUpdateControlPadMenuItemAdjust(CCmdUI* pCmdUI);


/**
 * Message: IDM_TOGGLE_AUTOFLY: Toggles Autofly mode.
 */
void Segments3DLayout::OnToggleAutofly()
{
  ColonViatronixWnd::OnToggleAutofly();
  
  SliceChildView& childView((SliceChildView&)GetChildViewByPos(ePOS_ORTHO));

  if (childView.GetViewWndType() == GLChildView::eGVT_CROSS_SECTION)
    childView.SetAutoflyMode(ViatronStudy::m_modeObliqueView & ViewModes::AUTOFLY2D);
  else
    childView.SetAutoflyMode(ViatronStudy::m_modeOrthogView & ViewModes::AUTOFLY2D);

  } // OnToggleAutofly()


// $Log: Segments3DView.C,v $
// Revision 1.28.2.28  2010/04/17 08:33:59  dongqing
// enable "delete all measurements" menu item in segments and orthogonal layouts
//
// Revision 1.28.2.27  2010/04/14 04:11:43  dongqing
// portrait changes
//
// Revision 1.28.2.26  2010/03/22 15:31:55  dongqing
// added menu item for dissection plane display in overview
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.28.2.25  2010/03/10 00:49:07  dongqing
// localized segment3d for snapshot
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.28.2.24  2010/03/09 20:21:18  dongqing
// print study info on every snapshot image
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.28.2.23  2010/02/26 16:43:46  dongqing
// allow missed region jump on this layout now that we have a missed regions list displayed
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.28.2.22  2010/01/14 14:47:45  dongqing
// added an oblique view to 3d segments (to fill out empty space)
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.28.2.21  2010/01/07 21:50:36  dongqing
// proper layout of 3D segments view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.28.2.20  2010/01/07 18:49:11  dongqing
// widescreen 3d segments view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.28.2.19  2009/12/16 15:14:17  dongqing
// fix the error display of UD segments total number. The m_straightNodes.size()
// is the number of nodes rather than number of segments
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.28.2.18  2009/12/09 20:22:48  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.28.2.17  2009/11/25 22:09:47  dongqing
// widescreen
//
// Revision 1.28.2.16  2009/10/07 16:13:15  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.28.2.15  2009/07/30 17:58:20  kchalupa
// Code Walkthrough
//
// Revision 1.28.2.14  2009/02/06 02:45:08  jmeade
// resizing controls dialog.
//
// Revision 1.28.2.13  2009/01/31 01:14:44  jmeade
// Issue 6236: add oem plus and minus keys to also forward/backward segment jump.
//
// Revision 1.28.2.12  2008/03/20 21:58:11  jmeade
// Issue 5992: don't place ud segments scrollbar till we know what we're doing with it.
//
// Revision 1.28.2.11  2008/02/07 19:32:27  jmeade
// fit scrollbar.
//
// Revision 1.28.2.10  2008/01/31 20:18:03  jmeade
// removing code checked in before it's time.
//
// Revision 1.28.2.9  2008/01/31 19:59:08  jmeade
// Issue 5917: support for vertical scrollbars throughout application.
//
// Revision 1.28.2.8  2008/01/11 02:46:43  jmeade
// issue 5907.
//
// Revision 1.28.2.7  2007/10/23 19:24:59  jmeade
// Issue 5852: move segment re-creation function down to segment child view.
//
// Revision 1.28.2.6  2007/10/23 00:37:16  jmeade
// Issue 5844: disable missed region jump.
//
// Revision 1.28.2.5  2007/08/24 17:06:02  jmeade
// Issue 5788: changed next and previous keys to vk_add and vk_subtract (+ and -)
//
// Revision 1.28.2.4  2007/08/24 01:30:26  jmeade
// fixed crash related to view window position.
//
// changed next and previous keys to vk_add and vk_subtract (+ and -)
//
// Revision 1.28.2.3  2007/06/27 01:35:23  jmeade
// Issue 5704: update ui command for snapshots.
//
// Revision 1.28.2.2  2007/06/08 00:07:17  jmeade
// Issue 5676: ensure BIOPSY option is toggled appropriately.
//
// Revision 1.28.2.1  2007/05/03 17:40:25  jmeade
// spaces.
//
// Revision 1.28  2007/03/09 15:53:30  jmeade
// code standards.
//
// Revision 1.27  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.26  2007/02/27 00:46:49  jmeade
// Issue 5422: toggle translucency for segments and 3d together.
//
// Revision 1.25  2007/02/09 20:49:56  romy
// fixed the Translucent display
//
// Revision 1.24  2007/02/09 19:01:18  romy
// Renderer conditions modify to fix Issue#5339.
//
// Revision 1.23  2007/02/06 18:58:18  dongqing
// Set back the UD segments go through the last segment
// for 'N' key down
//
// Revision 1.22  2007/02/02 21:52:35  romy
// added GLRendererViewers  method
//
// Revision 1.21  2007/02/02 21:25:11  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.20  2007/01/25 19:47:44  jmeade
// Issue 5167: toggle sliceview show parameter appropriately.
//
// Revision 1.19  2007/01/24 21:37:59  romy
// adjusted the control height to cope with different resolutions
//
// Revision 1.18  2007/01/11 02:47:18  jmeade
// comments.
//
// Revision 1.17  2007/01/11 02:37:24  jmeade
// variable naming.
//
// Revision 1.16  2006/11/24 22:42:05  jmeade
// Issue 5179: Show speed slider on abbreviated controls pane.
//
// Revision 1.15  2006/11/16 19:17:01  dongqing
// 'N' key stroke ends at # of segments-2 instead of # of segments-1
//
// Revision 1.14  2006/10/18 23:54:57  jmeade
// more descriptive view identifiers in resource and code for 3d segments view.
//
// Revision 1.13  2006/10/13 19:36:07  jmeade
// Issue 5073: correct ordering/numbering of views.
//
// Revision 1.12  2006/10/12 20:48:58  jmeade
// Issue 5048: default text colour for layouts.
//
// Revision 1.11  2006/10/09 19:13:40  frank
// Issue #5048: Fixed scrollbar behavior of UD segments view
//
// Revision 1.10  2006/10/04 17:57:12  jmeade
// allow parent to handle view mode update.
//
// Revision 1.9  2006/10/04 15:43:08  jmeade
// patient info dialog; enable dataset toggling.
//
// Revision 1.8  2006/10/04 04:17:17  jmeade
// reverse segment jump; render logo; remove popup menu.
//
// Revision 1.7  2006/09/28 16:00:24  frank
// added controls to 3D segment view
//
// Revision 1.6  2006/09/19 16:12:11  frank
// fixing up the experimental layouts
//
// Revision 1.5  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.4  2005/10/25 16:46:49  jmeade
// ParentViews ==> Layouts
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
// Revision 1.9.2.4.4.1  2005/06/10 18:20:38  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 1.9.2.4  2003/02/27 03:22:03  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 1.9.2.3  2003/02/25 17:55:27  jmeade
// Enabling recognition via Visual Studio classwizard.
//
// Revision 1.9.2.2  2003/02/22 12:08:44  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 1.9.2.1  2003/02/21 18:05:37  jmeade
// Line measurement mode same for both 2D and 3D subviews in this view window.
//
// Revision 1.9  2003/01/24 20:26:25  jmeade
// Plane is no longer a template class.
//
// Revision 1.8  2003/01/22 22:34:29  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.7  2002/11/24 17:41:53  kevin
// Renamed ProjectOntoPlane
//
// Revision 1.6  2002/11/15 00:34:20  jmeade
// No hard-coding of # of child views.
//
// Revision 1.5  2002/11/11 16:17:42  kevin
// Initial version of cylindrical -- still needs some polishing
//
// Revision 1.4  2002/08/28 20:52:21  kevin
// Lots of updates to segmental viewing
//
// Revision 1.3  2002/08/27 18:28:49  jmeade
// Class wizard specification.
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Segments3DView.C,v 1.28.2.28 2010/04/17 08:33:59 dongqing Exp $
// $Id: Segments3DView.C,v 1.28.2.28 2010/04/17 08:33:59 dongqing Exp $
