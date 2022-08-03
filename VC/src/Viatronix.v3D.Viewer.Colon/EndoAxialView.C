// $Id: EndoAxialView.C,v 1.17.2.14 2010/03/10 12:18:05 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: EndoAxialView.C  
// Owner: Jeffrey Meade  jmeade@viatronix.com


#include "stdafx.h"
#include "EndoAxialView.h"
#include "NavigationView.h"
#include "SliceChildView.h"
#include "DrawOverView.h"
#include "MonitorDisplay.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ColonViews::Layouts;
using namespace ReaderLib;

//////////////////////////////////////////////////////////////////////
// EndoOrthogonalViewFrame
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(EndoOrthogonalLayoutFrame, CMDIChildWnd)


/**
 * Constructor For view frame
 */
EndoOrthogonalLayoutFrame::EndoOrthogonalLayoutFrame() : ChildLayoutFrame((GLWnd *) new EndoOrthoLayout)
{

} // EndoOrthogonalLayoutFrame()


//////////////////////////////////////////////////////////////////////
// EndoOrthoLayout
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(EndoOrthoLayout, ColonViatronixWnd)
  ON_MESSAGE(WM_APP_QUERY_ENABLE_SUPINE_PRONE_TOGGLE, EndoOrthoLayout::OnQueryEnableSupineProneSwitch)
  ON_MESSAGE(WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID, EndoOrthoLayout::OnGetMainViewWndID)
  ON_MESSAGE(WM_APP_2DAUTOFLIGHT_UPDATE, EndoOrthoLayout::OnUpdate2DAutoFlight)
  ON_COMMAND(IDM_REPORT_ADDSNAPSHOT, EndoOrthoLayout::OnReportAddSnapshot)
  ON_COMMAND(IDM_MEASUREMENTS_DELETEALL, EndoOrthoLayout::OnMeasurementsDeleteAll)
  ON_COMMAND(ID_OPTIONS_CONTROLPAD_FLIGHTCONTROLS, EndoOrthoLayout::OnControlPadFlightControls)
  ON_COMMAND(ID_OPTIONS_CONTROLPAD_ADJUSTTRANSLUCENCY, EndoOrthoLayout::OnControlPadAdjustTranslucency)
  ON_UPDATE_COMMAND_UI(IDM_REPORT_ADDSNAPSHOT, EndoOrthoLayout::OnUpdateReportAddSnapshot)
  ON_UPDATE_COMMAND_UI(IDM_MEASUREMENTS_DELETEALL, EndoOrthoLayout::OnUpdateMeasurementsDeleteAll)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_CONTROLPAD_FLIGHTCONTROLS, EndoOrthoLayout::OnUpdateControlPadFlightControls)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_CONTROLPAD_ADJUSTTRANSLUCENCY, EndoOrthoLayout::OnUpdateControlPadAdjustTranslucency)
  ON_COMMAND(IDM_TOGGLE_AUTOFLY, OnToggleAutofly)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION, OnToggleSegmentRegion)

END_MESSAGE_MAP()

enum { EO_SLICE_VIEW_POS = 0, EO_NAVIGATION_VIEW_POS, EO_OVERVIEW_POS, NUM_ENDOORTHO_VIEWS };


/**
 * Constructor 
 */
EndoOrthoLayout::EndoOrthoLayout() : ColonViatronixWnd()
{
} // constructor


/**
 * Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void EndoOrthoLayout::OnSize( UINT uType, int iCX, int iCY )
{
  ColonViatronixWnd::OnSize(uType, iCX, iCY);

  //
  // NO! custom resizing code
  //
  return;

  // header fixed height at top
  const int4 iHeaderHeight = WinUtils::GetClientRect(GetDlgItem( IDC_TEMPLATE_PATIENTINFO )).Height();
  GetDlgItem( IDC_TEMPLATE_PATIENTINFO )->SetWindowPos( NULL, 0, 0, iCX, iHeaderHeight, SWP_NOZORDER );

  // determine the image size
  const int4 iControlsHeight = WinUtils::GetClientRect(GetDlgItem( IDC_TEMPLATE_CONTROLS )).Height();
  const int4 iGap = LAYOUT_GAP;
  const int4 iScrollbarHeight = LAYOUT_SCROLLBAR_THICKNESS;
  int4 iImageSize = min( ( iCX - 3 * iGap ) / 2, iCY - iHeaderHeight - iScrollbarHeight - iControlsHeight - 3 * iGap );
  if ( SliceChildView::GetScrollBarLocationEnum()==SliceChildView::SBO_RIGHT )
  {
    if (iImageSize * 2 - iCX < iScrollbarHeight * 2)
    {
      iImageSize = min(iImageSize, (iCX - iScrollbarHeight*4 - 3 * iGap) / 2);
    }
  }

  const int4 iGapH = ( iCX - 2 * iImageSize ) / 3;
  //const int4 iGapV = ( iCY - iHeaderHeight - iImageSize - iScrollbarHeight - iControlsHeight ) / 3;

  // layout the images
  const int4 iLeftX = iGapH;
  const int4 iRightX = 2 * iGapH + iImageSize;
  const int4 iImageY = iHeaderHeight + iGap;
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW )->SetWindowPos( NULL, iLeftX, iImageY, iImageSize, iImageSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_ENDOVIEW )->SetWindowPos( NULL, iRightX, iImageY, iImageSize, iImageSize, SWP_NOZORDER );

  // layout the overview
  const int4 iOverviewWidth = iCX * 0.20F - iGap * 1.5F;
  const int4 iImageBottomY = iImageY + iImageSize + iScrollbarHeight + iGap;
  const int4 iOverviewHeight = iCY - iImageBottomY - 2 * iGap;
  const int4 iOverviewY = iImageBottomY + iGap;
  const int4 iOverviewX = iGap;
  GetDlgItem( IDC_TEMPLATE_OVERVIEW )->SetWindowPos( NULL, iOverviewX, iOverviewY, iOverviewWidth, iOverviewHeight, SWP_NOZORDER );

  // layout the controls
  const int4 iControlsWidth = iCX - iOverviewWidth - iGap;
  const int4 iControlsX = iOverviewX + iOverviewWidth + iGap;
  const int4 iControlsY = ( iImageBottomY + iCY ) / 2 - iControlsHeight / 2;
  GetDlgItem( IDC_TEMPLATE_CONTROLS )->SetWindowPos( NULL, iControlsX, iControlsY, iControlsWidth, iControlsHeight, SWP_NOZORDER );

  // translucency dialog
  GetDlgItem( IDC_TEMPLATE_ADJUSTTRANSLUCENCY )->SetWindowPos( NULL, iControlsX, iControlsY, iControlsWidth, iControlsHeight, SWP_NOZORDER );

} // OnSize( UINT uType, int iCX, int iCY ) 


/**
 * Render child views
 */
void EndoOrthoLayout::GLRenderViews()
{
  if ( rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() )
  {
    Timer timer;
    GLRenderView( EO_OVERVIEW_POS );
    GLRenderView( EO_NAVIGATION_VIEW_POS );
    rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.SetLastRenderTime( timer.ElapsedTime() );
  }
  else if ( ( ViatronStudy::m_modeOrthogView  & ViewModes::AUTOFLY2D ) || 
           ( ViatronStudy::m_modeObliqueView & ViewModes::AUTOFLY2D ) )
  {
    Timer timer;

    GLRenderView( EO_SLICE_VIEW_POS );

    // Only re-render endoscopic view when flying
    if ( SliceChildView::m_bUpdateEndoluminalView )
      GLRenderView( EO_NAVIGATION_VIEW_POS );
    
    if ( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_OVERVIEW )    
      GLRenderView( EO_OVERVIEW_POS);

    rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.SetLastRenderTime( timer.ElapsedTime() );
  }
  else if ( ViatronixWnd::m_uInvalidChildViews != uint4( GLChildView::eGVT_NONE ) )
  {
    if ( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_SLICE )
      GLRenderView( EO_SLICE_VIEW_POS);
    
    if ( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_NAVIGATION )
      GLRenderView( EO_NAVIGATION_VIEW_POS );
    
    if ( ViatronixWnd::m_uInvalidChildViews & GLChildView::eGVT_OVERVIEW )
      GLRenderView( EO_OVERVIEW_POS );    
  }
  
  m_uInvalidChildViews = uint4(GLChildView::eGVT_NONE);
} // GLRenderViews()


/**
 * mouse movement handler
 *
 * @param uFlags   mouse/keyboard flags
 * @param point    location of mouse cursor
 */
void EndoOrthoLayout::OnMouseMove( UINT uFlags, CPoint point )
{
  ColonViatronixWnd::OnMouseMove( uFlags, point );
  
  // if not flying in either 2d or 3d
  if( !BitCheck(ViatronStudy::m_modeEndoView,    ViewModes::AUTOFLY   ) && 
      !BitCheck(ViatronStudy::m_modeOrthogView,  ViewModes::AUTOFLY2D ) && 
      !BitCheck(ViatronStudy::m_modeObliqueView, ViewModes::AUTOFLY2D ) )
  {
    // if the id is neither of the child views set the endoluminal as main view
    GLChildViewRef childView = GetChildView( m_idMainView );
    if ( (typeid(childView) != typeid(SliceChildView)) && (typeid(childView) != typeid(EndoluminalView)) )
    {
      m_idMainView = GetChildViewDlgItem( IDC_TEMPLATE_ENDOVIEW ).GetClassInstanceID();
    }

    ViatronStudy::SetCurrentView(GetChildView(m_idMainView).GetViewWndType(), rdrGlobal.m_pCurrDataset->GetOrientation());
  }
 // m_uInvalidChildViews |= GetCurrentView().GetViewWndType();
  if( m_uInvalidChildViews != GLChildView::eGVT_NONE )
    GLIdle();
} // OnMouseMove()



/**
 * Notifies view window of the current progress of study load
 *
 * @param eCompletedStep   The previously completed step
 * @param eCurrentStep     The next (current) study loading step
 * @param eOrientation     Orientation on which the next step is performing
 */
void EndoOrthoLayout::OnStudyLoadStepNotify(const StudyLoad::StepsEnum eCompletedStep,
  const StudyLoad::StepsEnum eCurrentStep, const DatasetOrientations::TypeEnum eOrientation)
{
  SliceChildView& rSlice = (SliceChildView &) GetChildViewDlgItem( IDC_TEMPLATE_SLICEVIEW );
  rSlice.OnVolumeLoaded(0,0);

  switch (eCompletedStep)
  {
    case StudyLoad::eAXIAL:
      rSlice.SetOrientation(SliceOrientations::eAXIAL);
      break;

    case StudyLoad::eLOAD_COMPLETE:
    default:
    {
      // Set orientation according to current ViatronStudy::ViewWndMode
      switch (ViatronStudy::ViewWndModes.m_modeEndoOrthoLayout)
      {
        case ViewModes::AXIAL:
          rSlice.SetOrientation(SliceOrientations::eAXIAL);
          break;

        case ViewModes::SAGITTAL:
          rSlice.SetOrientation(SliceOrientations::eSAGITTAL);
          break;

        case ViewModes::CORONAL:
          rSlice.SetOrientation(SliceOrientations::eCORONAL);
          break;

        case ViewModes::CROSS_SECTION:
          rSlice.SetOrientation(SliceOrientations::eCROSS_SECTION);
          break;
      }
      
      break;
    }
  } // endswitch(eStep)

  Invalidate(NULL);
} // OnStudyLoadStepNotify()


/**
 * Return 1 to enable switch between supine and prone studies in this view
 * 
 * @param WPARAM     Unused
 * @param LPARAM     Unused
 * @return LRESULT   Non-zero to enable toggle between supine and prone
 */
LRESULT EndoOrthoLayout::OnQueryEnableSupineProneSwitch(WPARAM, LPARAM)
{
  return TRUE;
} // OnQueryEnableSupineProneSwitch


/**
 * Gets the class instance ID of the current main view window
 *
 * @param WPARAM     Unused
 * @param LPARAM     Unused
 * @return LRESULT   Main child view's ID
 */
LRESULT EndoOrthoLayout::OnGetMainViewWndID(WPARAM, LPARAM)
{
  return LRESULT(m_idMainView);
} // OnGetMainViewWndID


/** 2D auto-flight: signal to set up new skeleton position.
 *
 * @param wParam     Position on current skeleton. 
 * @param lParam     Unused
 * @return LRESULT   TRUE to set up new position on current skeleton.
 */
LRESULT EndoOrthoLayout::OnUpdate2DAutoFlight(WPARAM wParam, LPARAM lParam)
{
  switch (lParam)
  {
  case 0: // update skeleton position.
    if ((ViatronStudy::m_modeOrthogView & ViewModes::AUTOFLY2D) || (ViatronStudy::m_modeObliqueView & ViewModes::AUTOFLY2D))
    {
      SliceChildView & sliceView = (SliceChildView &) GetChildViewDlgItem( IDC_TEMPLATE_SLICEVIEW );
      sliceView.OnUpdate2DAutoFlight((int4)wParam);
    }
    break;
          
  case MAKELPARAM(0,1):   // 0+1: render overview
  case MAKELPARAM(1,1):   // 1+1: rerender overview at once.
    {
      OverView & overView = (OverView&) GetChildViewDlgItem( IDC_TEMPLATE_OVERVIEW );
      overView.RefreshImage(/*time click=*/ 3);

      if (lParam == MAKELPARAM(1,1) /* rerender overview at once*/)
      {
        GLRenderView( EO_OVERVIEW_POS );
      }
    }
    break;
  
  default: break; 
  }

  return 1;
} // OnUpdate2DAutoFlight()



/**
 * Message: IDM_TOGGLE_AUTOFLY: Toggles Autofly mode.
 */
void EndoOrthoLayout::OnToggleAutofly()
{
  ColonViatronixWnd::OnToggleAutofly();
  
  SliceChildView& childView((SliceChildView&)GetChildViewByPos(EO_SLICE_VIEW_POS));

  if (childView.GetViewWndType() == GLChildView::eGVT_CROSS_SECTION)
    childView.SetAutoflyMode(ViatronStudy::m_modeObliqueView & ViewModes::AUTOFLY2D);
  else
    childView.SetAutoflyMode(ViatronStudy::m_modeOrthogView & ViewModes::AUTOFLY2D);

  } // OnToggleAutofly()


/**
 * Adds a new report entry.
 */
void EndoOrthoLayout::OnReportAddSnapshot()
{
  // Note: Increase size of the (small) view rectangle of overview image for the snapshot (SetViewRect calls)
  try
  {
    SliceChildView * pSlice = dynamic_cast<SliceChildView*>(GetDlgItem(IDC_TEMPLATE_SLICEVIEW));
    EndoluminalView * pEndo = dynamic_cast<EndoluminalView*>(GetDlgItem(IDC_TEMPLATE_ENDOVIEW));

    DrawOverView ovDrawImage(this, *rdrGlobal.m_pCurrDataset);
    ovDrawImage.SetDisplayDistanceFromRectum(false);

    std::list<GLChildViewPtr> childList;
    childList.push_back(&ovDrawImage);
    childList.push_back(pSlice);
    childList.push_back(pEndo);

    std::list<GLChildViewPtr> displayList;
    displayList.push_back(pSlice);
    displayList.push_back(pEndo);

    // use the current 2d measure line list if there is one, otherwise the 3d measure list
    MeasureLineList& list = pSlice->m_sliceViewer.m_measureLineList;
    
    AddReportSnapshot( childList,displayList, !list.empty() ? list : rdrGlobal.m_pCurrDataset->m_session.m_measureLineList );
  }
  catch (...)
  {
  }
} // OnReportAddSnapshot()


/**
* Gets the resource ID for the popup menu for this window
*
* @param childView      Child view whose popup menu will be displayed.
* @param uPopupMenuID   ID Popup menu requested to be displayed.
* @return               Resource ID for the popup menu, or 0 for no menu
*/
uint4 EndoOrthoLayout::OnShowPopupMenu(const GLChildView& childView, const uint4 uPopupMenuID) const
{
  // Show only IDM_SLICE_CROSS_SECTION_ORIENTATION for xsec; Don't show IDM_SLICE_ORIENTATION popup menu for orthogonals
  if (uPopupMenuID == IDM_OVERVIEW)
  {
    return IDM_OVERVIEW_ABBREV;
  }
  return uPopupMenuID;
} // OnShowPopupMenu()

// Revision History:
// $Log: EndoAxialView.C,v $
// Revision 1.17.2.14  2010/03/10 12:18:05  dongqing
// tidying up
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.17.2.13  2010/03/09 20:24:45  dongqing
// print study info on every snapshot image
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.17.2.12  2009/12/09 20:26:01  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.17.2.11  2009/11/27 18:58:56  dongqing
// more template size adjustments
//
// Revision 1.17.2.10  2009/11/27 16:36:58  dongqing
// more widescreen layouts
//
// Revision 1.17.2.9  2009/10/07 16:13:12  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.17.2.8  2009/01/28 00:15:13  jmeade
// Issue 6231: account for calculation of smaller-size windows when accounting for vertical scroll.
//
// Revision 1.17.2.7  2008/03/05 20:17:46  jmeade
// Issue 6036: shrink overview a bit to give room for full controls dialog width in 1280x1024.
//
// Revision 1.17.2.6  2008/02/07 19:32:47  jmeade
// removed hard-coded dialog sizes.
//
// Revision 1.17.2.5  2008/01/31 19:50:26  jmeade
// Issue 5917: support for vertical scrollbars throughout application.
//
// Revision 1.17.2.4  2008/01/25 19:23:43  jmeade
// Issue 5917: view spacing for vertical scrollbars.
//
// Revision 1.17.2.3  2008/01/11 02:18:54  jmeade
// issue 5907.
//
// Revision 1.17.2.2  2007/09/12 15:46:06  jmeade
// fixed crash.
//
// Revision 1.17.2.1  2007/09/10 18:14:58  jmeade
// fixed crash on endo-slice view.
//
// Revision 1.17  2007/03/08 22:21:56  romy
// code review - header files adjusted
//
// Revision 1.16  2007/03/01 01:18:55  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.15  2007/02/02 20:21:23  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.14  2007/01/24 22:41:53  romy
// added code to handle the SRT region tool click
//
// Revision 1.13  2006/06/01 20:19:24  frank
// updated to visual studio 2005
//
// Revision 1.12  2006/02/07 21:21:46  jmeade
// code review: comments.
//
// Revision 1.11  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.10.2.3  2006/01/11 16:47:09  frank
// Issue #4596: Moved adjust translucency dialog to proper location and size
//
// Revision 1.10.2.2  2006/01/10 20:36:41  geconomos
// issue 4596: adjust translucency dialog not positioned correctly
//
// Revision 1.10.2.1  2005/12/23 16:49:15  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.10  2005/10/25 15:37:28  jmeade
// ParentViews ==> Layouts
//
// Revision 1.9  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.8.2.1  2005/09/21 13:28:47  geconomos
// modified DrawImage to implicitly use an image size of 512
//
// Revision 1.8  2005/09/09 14:31:32  geconomos
// Issue# 4349: Image resolution not increasing at higher framerates.
//
// Revision 1.7  2005/09/06 17:11:17  geconomos
// Issue #4412: Movement in 2d causing hiccups during endoluminal flight
//
// Revision 1.6  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.5  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.4  2005/08/18 12:25:39  geconomos
// Modified add_snapshot to use the DrawOverview class for the overview image.
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
// Revision 3.28.2.11.2.9.2.3  2005/07/13 19:03:15  geconomos
// optimized for speed
//
// Revision 3.28.2.11.2.9.2.2  2005/06/30 13:50:20  geconomos
// Issue #4221: Endoluminal not  flying on double click when flyin 2d
//
// Revision 3.28.2.11.2.9.2.1  2005/06/24 13:02:20  geconomos
// Issue #4221: Unable able to fly in 3d after flying in 2d
//
// Revision 3.28.2.11.2.9  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.28.2.11.2.8.2.1  2005/05/25 15:50:22  geconomos
// initial backup
//
// Revision 3.28.2.11.2.8  2005/04/11 15:04:07  jmeade
// Issue 2815: Disable annotations for oblique views.
//
// Revision 3.28.2.11.2.7  2005/04/07 22:16:24  jmeade
// Issue 4109: Let childview class handle update triggers.
//
// Revision 3.28.2.11.2.6  2005/04/05 22:55:24  jmeade
// Issue 4100: updates when show cleansed option.
//
// Revision 3.28.2.11.2.5  2005/04/01 01:25:18  frank
// Issue #4079: fixed cross sectional image slicing
//
// Revision 3.28.2.11.2.4  2005/04/01 00:22:49  jmeade
// ensure update slice after scrollbar message.
//
// Revision 3.28.2.11.2.3  2005/03/31 17:36:10  jmeade
// Issue 4038: Generalized idle time processing; fixed for other messages.
//
// Revision 3.28.2.11.2.2  2005/03/28 20:44:55  jmeade
// Issue 4066.
//
// Revision 3.28.2.11.2.1  2005/03/12 14:36:03  jmeade
// Snapshots wit 2d measurements interpreted as polyps (as with 3d measurements).
//
// Revision 3.28.2.11  2005/02/15 19:33:19  jmeade
// merge from vc1.4 branch.
//
// Revision 3.28.2.10.2.3  2004/12/14 19:28:44  jmeade
// Fix Issue 3858 (2d autofly update when mouse cursor not in 2d view).
//
// Revision 3.28.2.10.2.2  2004/11/23 21:15:16  jmeade
// Ensure snapshot overview image is square.
//
// Revision 3.28.2.10.2.1  2004/11/18 02:02:51  jmeade
// Render only captured view when appropriate.
//
// Revision 3.28.2.10  2004/09/23 18:22:44  jmeade
// Adjust translucency on endo-slice view.
//
// Revision 3.28.2.9  2003/07/24 17:32:33  jmeade
// Issue 3401: Logic of which views to draw to be controlled by the individual views.
//
// Revision 3.28.2.8  2003/05/29 17:29:30  jmeade
// comments.
//
// Revision 3.28.2.7  2003/04/14 16:42:16  jmeade
// Issue 2841: Handle delete all measurements msg.
//
// Revision 3.28.2.6  2003/02/27 02:16:17  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.28.2.5  2003/02/22 19:06:41  jmeade
// Issue 2837:  Install non-overriden routine to capture movie frames during recording.
//
// Revision 3.28.2.4  2003/02/22 11:55:31  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.28.2.3  2003/02/22 05:50:59  jmeade
// Issue 2837:  Only 2D and 3D view are relevant as main views.
//
// Revision 3.28.2.2  2003/02/19 22:20:24  jmeade
// Issue 2842:  Implemented variable number of snapshots per report entry.
//
// Revision 3.28.2.1  2003/02/12 00:59:59  jmeade
// Clarifying names for 2D flight modes.
//
// Revision 3.28  2003/02/10 16:47:35  jmeade
// Issue 2832: Redundant, excess code for snapshot notifications.
//
// Revision 3.27  2002/11/27 22:24:14  jmeade
// Reverse of paradigm for popup menus:  assume show popup menu on right-click in childview,
//  have view window instances validate/refuse popup as necessary.
//
// Revision 3.26  2002/11/21 22:53:19  jmeade
// Overview render mode cleanup; now-unneeded method overrides.
//
// Revision 3.25  2002/11/15 00:29:19  jmeade
// No hard-coding of # of child views; Hide child view storage (use access methods).
//
// Revision 3.24  2002/09/10 20:49:36  jmeade
// Changed function name.
//
// Revision 3.23  2002/09/06 16:10:26  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.22  2002/08/22 13:20:00  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.20  2002/07/09 20:14:15  jmeade
// Code standards.
//
// Revision 3.19  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.18  2002/06/04 18:36:59  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.17  2002/05/09 20:54:15  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_05-06-2002)
//
// Revision 3.16  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.15  2002/04/12 02:10:17  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.14  2002/03/21 02:08:19  jmeade
// Fixed render speed slowing caused by rendering cross-section view
//  introduced in v3.2.
//
// Revision 3.13  2002/03/13 22:01:41  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.12  2002/02/06 20:33:58  binli
// issue 1796: update indicators when stop flying (3d & 2d flight)
// (message WM_APP_2DFLIGHT_UPDATEVIEW)
//
// Revision 3.11.2.17  2002/08/05 23:43:08  jmeade
// Issue 2591 (in previous revision):  Added default handlers for main view window identification in ViatronViewWnd.
//
// Revision 3.11.2.16  2002/08/05 23:39:09  jmeade
// Issue 2593:  Clarify the "object" of the measurements (3D) and annotations (2D) menus.
//
// Revision 3.11.2.15  2002/08/01 14:46:47  jmeade
// Issue 2587:  Made a number of 2D slice options global.
//
// Revision 3.11.2.14  2002/06/17 18:18:22  jmeade
// Issue 2453:  Add bookmark override in base, enabled for any view with an overview.
//
// Revision 3.11.2.13  2002/05/21 20:17:03  jmeade
// Issue 2271:  "Delete all" menu items for measurements and bookmarks.
//
// Revision 3.11.2.12  2002/04/24 18:11:12  jmeade
// Issue 2044:  Default base class handler sets current ViatronStudy view on mouse move (view with mouse focus).
//
// Revision 3.11.2.11  2002/04/23 21:38:07  jmeade
// Issue 2044:  Set proper current view in ViatronStudy (to enable appropriate
//  F-keys and control panel commands).
//
// Revision 3.11.2.10  2002/04/03 01:03:23  jmeade
// Don't move mouse cursor on snapshot, the user is not expecting it;
// Removed unnecessary override.
//
// Revision 3.11.2.9  2002/03/28 22:26:04  jmeade
// Fixed render speed slowing caused by rendering cross-section view
//  introduced in v3.2.
//
// Revision 3.11.2.8  2002/03/05 20:17:32  binli
// adjustment of 3D measurement line drawing.
//
// Revision 3.11.2.7  2002/03/01 17:01:25  binli
// issue 1966: when measurement line is visible, default to 'polyp'
//
// Revision 3.11.2.6  2002/02/26 17:11:06  binli
// issue 1543: appendix: no need to center cursor in this view
//
// Revision 3.11.2.5  2002/02/26 16:54:15  binli
// issue 1543: inverse image when taking snapshot.
//
// Revision 3.11.2.4  2002/02/22 20:40:38  binli
// issue 1543: 3> cursor in the center of mainviewwindow.
//
// Revision 3.11.2.3  2002/02/21 18:12:06  jmeade
// Issue 1910:  Push handlers for bookmark menu to ViatronViewWnd; enable bookmarks for Endo-Ortho view.
//
// Revision 3.11.2.2  2002/02/18 16:10:08  binli
// issue 1863: send message to StandardView/EndoSliceView when there is action of mousewheel/change scrollbar.
//
// Revision 3.11.2.1  2002/02/11 18:02:15  binli
// issue 1796: update indicators when stop flying (3d & 2d flight)
//
// Revision 3.11  2001/01/01 06:02:58  binli
// issue 1588: response to message 'WM_APP_2DAUTOFLIGHT_UPDATE'
//
// Revision 3.10  2002/01/23 21:47:53  binli
// issue 1589: Endo-SliceView: take snapshot from all 3 views now.
//
// Revision 3.9  2002/01/23 15:58:31  jmeade
// 1591:  Support for movie recording; Coding standards.
//
// Revision 3.8  2001/11/21 15:25:21  jmeade
// Update endoaxial view.
//
// Revision 3.7  2001/11/17 03:30:17  jmeade
// Cosmetic change.
//
// Revision 3.6  2001/11/09 22:25:06  binli
// slice flight.
//
// Revision 3.5  2001/11/09 04:42:44  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.4  2001/11/09 00:41:55  jmeade
// Added control bar to registration view.
//
// Revision 3.3  2001/11/08 20:43:34  binli
// 2D flight
//
// Revision 3.2  2001/11/06 21:50:03  binli
// 2D flight along centerline
//
// Revision 3.1.2.3  2001/10/28 11:11:27  jmeade
// Coding standards; Safer references instead of pointers
//
// Revision 3.1.2.2  2001/10/28 04:43:01  jmeade
// Proper fix to Issue 1278: Child view specific code does not belong
// in parent views.
//
// Revision 3.1.2.1  2001/10/26 15:11:49  binli
// issue 1278: no call of ShowPopupMenu() when R-ButtonDown+Ctrl.
//
// Revision 3.1  2001/10/17 21:35:23  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:01:57  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 08 2001 16:54:04   jmeade
// Consolidated code specifying study orientation; allow creator of NavigationView to specify study orientation
// 
//    Rev 2.1   Oct 03 2001 12:13:10   jmeade
// Snapshot menu handler; coding standards
// 
//    Rev 2.0   Sep 16 2001 23:41:06   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:06   ingmar
// Initial revision.
// Revision 1.39  2001/08/22 02:22:55  jmeade
// Removed CAxialSliceView class
//
// Revision 1.38  2001/08/22 01:18:31  jmeade
// Better encapsulation in OverView (ResetTimer has no meaning outside of class)
//
// Revision 1.37  2001/08/10 18:01:33  jmeade
// generalized bug fix for popup menu
//
// Revision 1.36  2001/07/30 22:41:10  binli
// bug fixed: update overview when change slices in StandardView/EndoAxialView.
//
// Revision 1.35  2001/07/05 16:04:38  binli
// bug 000536: fixed.
// No to show up popup menu, if: (1) click not in a view, (2) shift/control is held
// (little appendix to bug 542 fixing)
//
// Revision 1.34  2001/06/21 21:15:32  binli
// speed up overview/slice render in StandardView/Endo-OrthogonalView.
// (defect 263, partial)
//
// Revision 1.33  2001/06/15 22:12:13  binli
// bug 482: fixed.
//
// Revision 1.32  2001/06/15 22:11:45  binli
// bug 482: fixed.
//
// Revision 1.31  2001/06/04 20:11:32  binli
// defect 484: fixed.
// 1> in S&PView/Endo-OrthView, no display of cross-sectioanl plane.
// 2> use current study, not rdrGlobal.m_pStudy, for calculation.
//
// Revision 1.30  2001/05/12 00:00:04  jmeade
// Coding conventions
//
// Revision 1.29  2001/04/13 02:33:20  jmeade
// headers and footers
//
// Revision 1.28  2001/02/23 20:11:31  jmeade
// Removed most unnecessary 'C' before class names (everything's a class!)
//
// Revision 1.27  2001/02/07 20:23:03  frank
// Removed gray-out functionality.
//
// Revision 1.26  2001/02/06 14:14:29  frank
// Revamped study load progress display.
//
// Revision 1.25  2001/02/05 23:32:34  liwei
// Non-live views in EndoAxialView are also grayed out
//
// Revision 1.24  2001/02/01 00:55:33  jeff
// private member access
//
// Revision 1.23  2001/01/31 18:19:59  jeff
// CreateDlgBar(): doesn't need to take in a pointer
//
// Revision 1.22  2001/01/26 02:05:04  jeff
// Differentiate between PaintComplete and LoadComplete
//
// Revision 1.21  2001/01/25 04:29:38  jeff
// supine/prone toggle (combobox) in patient info dialog (will "sexy" it up later);
// doesn't yet change on command in verifyview
//
// Revision 1.20  2001/01/13 00:32:20  jeff
// Using new Rect<> and Tuple<> classes
//
// Revision 1.19  2001/01/02 20:05:18  jeff
// swapped axial and endoscopic view in code (to reflect swap on screen)
//
// Revision 1.18  2000/12/28 01:25:41  jeff
// Current standard orthogonal orientation (in main view)
//  ==> (current endo-ortho orthogonal orientation and current supine/prone orthogonal orientation)
// Must work on:
//  current endo-ortho orthogonal orientation <==> current supine/prone orthogonal orientation)
//
// Revision 1.17  2000/12/27 23:42:45  jeff
// EndoAxialView ==> EndoOrthogonalView;
// embedded child view class in view frame class;
// Store view modes for each navigation view (Standard, Endo-Ortho, etc);
// TRANSVERSE ==> AXIAL; MPR and CROSSSECT ==> CROSS_SECTION;
//
// Revision 1.16  2000/12/22 16:47:57  jeff
// move all views up to make room for larger overview
//
// Revision 1.15  2000/12/20 19:47:10  jeff
// Added small overview window; swapped position of endoscopic and orthogonal view
//
// Revision 1.14  2000/12/12 23:44:50  jeff
// Added Patient info dialog; selection of ortho view orientation now
//  embedded in slice view; popup menus
//
// Revision 1.13  2000/11/21 04:09:40  jeff
// Create one (set of) font(s) in MainAppFrame class and access throughout
//  system (instead of each view window creating their own fonts)
//
// Revision 1.12  2000/11/14 07:40:31  jeff
// allocate font in a member variable, delete in destructor (previous memory-leak fix)
//
// Revision 1.11  2000/11/09 19:30:46  liwei
// A bug of hybrid view fixed
//
// Revision 1.10  2000/11/08 22:36:55  jeff
// Increased font on patient info dialog
// Set nicer fonts for buttons
//
// Revision 1.9  2000/10/31 23:45:46  liwei
// Hybrid view added
//
// Revision 1.8  2000/10/25 19:05:11  jeff
// Fixed bug that ignored selected view slice and always displayed axial
//
// Revision 1.7  2000/10/13 15:56:09  kevin
// Added ViatronViewWnd Message Map
//
// Revision 1.6  2000/10/02 21:56:46  jeff
// Changes for staggered study load (had to check in because my cheapo computer is crashing)
//
// Revision 1.5  2000/09/30 22:17:04  jeff
// ViatronViewWnd class for more study-specific, V1K specific, etc. things
//
// Revision 1.4  2000/09/30 02:46:52  jeff
// SetViewRect() and GetViewRect() child view access functions
//
// Revision 1.3  2000/09/15 01:58:24  jeff
// Moving to text draw in OpenGL
//
// Revision 1.2  2000/09/14 16:14:51  jeff
// Borders around all view windows
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 8     9/07/00 1:32p Jeff
// Embedded control dialog ("bars") in the GLChildView class, resized in
// OnSize
// 
// 7     8/28/00 1:14p Antonio
// updated header and footer
//
//*****************  Version 6  *****************
//User: Jeff            Date:  8/24/00  Time:  6:14p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Fitting the views to a 1280x1024 resolution
//
//*****************  Version 5  *****************
//User: Jeff            Date:  8/24/00  Time:  1:23a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Place orthogonal view buttons horizontally (instead of vertically)
//
//*****************  Version 4  *****************
//User: Jeff            Date:  8/24/00  Time: 12:55a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Buttons on EndoOrthogonal view to change flip between the orthogonal
//views
//
//**********************
//Label: Version 0.1
//User: Jeff            Date:  8/15/00  Time:  5:43p
//Labeled 'Version 0.1'
//Label comment:
//  First half-decent working version
//
//*****************  Version 3  *****************
//User: Jeff            Date:  8/15/00  Time:  5:39p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Enable repeated changing of slice view orientation
//
//*****************  Version 2  *****************
//User: Jeff            Date:  8/14/00  Time:  1:27p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  
//
//*****************  Version 1  *****************
//User: Jeff            Date:  8/14/00  Time: 10:35a
//Created EndoAxialView.cpp
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/EndoAxialView.C,v 1.17.2.14 2010/03/10 12:18:05 dongqing Exp $
// $Id: EndoAxialView.C,v 1.17.2.14 2010/03/10 12:18:05 dongqing Exp $
