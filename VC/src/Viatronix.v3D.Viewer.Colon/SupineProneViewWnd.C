// $Id: SupineProneViewWnd.C,v 1.18.2.19 2010/04/21 20:04:07 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade (jmeade@viatronix.com)

/**
 * Implentation of dual overview and dual orthogonal view windows
 */


// includes
#include "stdafx.h"
#include "SupineProneViewWnd.h"
#include "MainAppFrame.h"
#include "OverView.h"
#include "SliceChildView.h"
#include "ViatronStudy.h"
#include "V1KMessages.h"
#include "WinUtils.h"
#include "DrawOverview.h"
#include "Theme.h"
#include "MonitorDisplay.h"
#include "ApplicationController.h"
#include "vcUtils.h"


// defines
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define FILE_REVISION "$Revision: 1.18.2.19 $"


// namespaces
using namespace ColonViews::Layouts;
using namespace ReaderLib;
using namespace WinUtils;

//////////////////////////////////////////////////////////////////////
// SupineProneViewWnd::DualOverViewFrame
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(DualOverviewLayoutFrame, CMDIChildWnd)


/**
 * Constructor 
 */
DualOverviewLayoutFrame::DualOverviewLayoutFrame() : ChildLayoutFrame(new SupineProneLayout(SupineProneLayout::eSPL_OVERVIEW))
{
} // constructor()


//////////////////////////////////////////////////////////////////////
// DualOrthogonalLayoutFrame
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(DualOrthogonalLayoutFrame, CMDIChildWnd)


/**
 * Constructor 
 */
DualOrthogonalLayoutFrame::DualOrthogonalLayoutFrame() : ChildLayoutFrame(new SupineProneLayout(SupineProneLayout::eSPL_ORTHOGONAL))
{
} // constructor()

//////////////////////////////////////////////////////////////////////
// SupineProneLayout
//////////////////////////////////////////////////////////////////////

enum { NUM_SUPINE_PRONE_VIEWS = 2 };

/* message map */
BEGIN_MESSAGE_MAP(SupineProneLayout, ColonViatronixWnd)
  ON_COMMAND(IDM_TOGGLE_AUTOFLY, OnToggleAutofly)
  ON_COMMAND(IDM_REPORT_ADDSNAPSHOT, OnReportAddSnapshot)
  ON_UPDATE_COMMAND_UI(IDM_REPORT_ADDSNAPSHOT, OnUpdateReportAddSnapshot)
  ON_BN_CLICKED(IDC_TEMPLATE_LINKVIEWS, OnLinkViewsClicked)
  ON_MESSAGE(WM_USER_MOVIE_RECORD_GET_VIEW_ID, OnMovieRecordGetViewID)
  ON_MESSAGE(WM_APP_QUERY_ENABLE_SUPINE_PRONE_TOGGLE, OnQueryEnableSupineProneSwitch)
  ON_MESSAGE(WM_APP_SYNCRONIZE_DATASET_LOCATIONS, OnSyncronizeDatasetLocations)
  ON_COMMAND(ID_OPTIONS_CONTROLPAD_FLIGHTCONTROLS, OnControlPadFlightControls)
  ON_COMMAND(ID_OPTIONS_CONTROLPAD_ADJUSTTRANSLUCENCY, OnControlPadAdjustTranslucency)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_CONTROLPAD_FLIGHTCONTROLS, OnUpdateControlPadFlightControls)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_CONTROLPAD_ADJUSTTRANSLUCENCY, OnUpdateControlPadAdjustTranslucency)
  ON_COMMAND(IDM_MEASUREMENTS_DELETEALL, OnMeasurementsDeleteAll)
  ON_UPDATE_COMMAND_UI(IDM_MEASUREMENTS_DELETEALL, OnUpdateMeasurementsDeleteAll)
END_MESSAGE_MAP()


/**
 * Constructor 
 *
 * @param eViewType   Type of the childviews (overview or orthogonal).
 */
SupineProneLayout::SupineProneLayout(const SPLayoutTypeEnum & eLayoutType, bool bSupportsCAD ) : ColonViatronixWnd( bSupportsCAD ),
  m_eDrawViews(SupineProneLayout::eVIEWALL), 
  m_eLastActive(SupineProneLayout::eVIEWLEFT),
  m_pSliceChildViewLeft( NULL ),
  m_pSliceChildViewRight( NULL ),
  m_eSupineProneLayoutType(eLayoutType),
  m_bBypassDatasetLockingCheck( true ),
	m_bToolbarAtBottom( rdrGlobal.m_bToolbarOnBottomForWideScreen )
{
} // SupineProneLayout()


/**
 * Destructor 
 */
SupineProneLayout::~SupineProneLayout()
{
  SliceChildView::ShowCamera(m_bWas2DCameraShowingWhenEnteringLayout);
} // destructor


/**
 * Called when the window is being created.
 *
 * @param lpCreateStruct  CREATESTRUCT structure that contains information about the CWnd object being created
 * @return                0 to continue creation, -1 to destroy the window  
 */
int SupineProneLayout::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
  if( ColonViatronixWnd::OnCreate( lpCreateStruct ) == -1 )
    return -1;
  
  m_bBypassDatasetLockingCheck = false;

  return 0;
} // OnCreate()


/**
 * Called when the window is being destroyed
 */
void SupineProneLayout::OnDestroy()
{  
  m_bBypassDatasetLockingCheck = true;
  ColonViatronixWnd::OnDestroy();
} // OnDestroy();


/**
 * Called on child view creation.
 *
 * @param hWndThis   Handle of this window.
 */
void SupineProneLayout::CreateChildViews( HWND hWndThis )
{
  ColonViatronixWnd::CreateChildViews(hWndThis);
  
  m_bWas2DCameraShowingWhenEnteringLayout = SliceChildView::GetIsCameraShowing();

  if ( rdrGlobal.m_bDualDataDetected )
  {
    OverView * pOverViewL = (OverView *) GetDlgItem(IDC_TEMPLATE_OVERVIEW_SUPINE);
    OverView * pOverViewR = (OverView *) GetDlgItem(IDC_TEMPLATE_OVERVIEW_PRONE);

    if ( ( pOverViewL != NULL ) && ( pOverViewR != NULL ) )
    {
      pOverViewL->Initialize(pOverViewR, &(rdrGlobal.m_supine.m_trackballStandard));
      pOverViewR->Initialize(pOverViewL, &(rdrGlobal.m_prone.m_trackballStandard));
    }

    if ( m_eSupineProneLayoutType == eSPL_OVERVIEW )
    {
    }
    else
    {
      SliceChildView * pSliceViewL = (SliceChildView *) GetDlgItem(IDC_TEMPLATE_SLICEVIEW_SUPINE);
      SliceChildView * pSliceViewR = (SliceChildView *) GetDlgItem(IDC_TEMPLATE_SLICEVIEW_PRONE);
      m_pSliceChildViewLeft = pSliceViewL;
      m_pSliceChildViewRight = pSliceViewR;

      SliceChildView::ShowCamera(false);

      pSliceViewL->m_pNeighbourSliceView = pSliceViewR;
      pSliceViewR->m_pNeighbourSliceView = pSliceViewL;
    }
  }
  else
  {
    ;
  }

  ViatronStudy::SetCurrentView(GetChildViewByPos(0).GetViewWndType(), ((ViatronChildView&)GetChildViewByPos(0)).GetDatasetRef().GetOrientation());

  // default (S/P) status is 'locked'.
  SkinButton * pAnchorBtn = dynamic_cast<SkinButton*> (GetDlgItem(IDC_TEMPLATE_LINKVIEWS));
  if( Utilities::Datasets::IsLocked() )
  {
    SetButtonIcon(*pAnchorBtn, IDB_CHAIN_LINK);
  }
  else
  {
    SetButtonIcon(*pAnchorBtn, IDB_CHAIN_UNLINK);
  }

  GetDlgItem(IDC_SUPINELABEL)->SetWindowText(PatientOrientationToString(rdrGlobal.m_supine).c_str());
  GetDlgItem(IDC_PRONELABEL)->SetWindowText(PatientOrientationToString(rdrGlobal.m_prone).c_str());

  GetDlgItem( IDC_VERIFY_LEFTINFO )->SetWindowText( "" );
  GetDlgItem( IDC_VERIFY_RIGHTINFO )->SetWindowText( "" );
  if ( rdrGlobal.IsStudyLoading() & DatasetOrientations::eSUPINE || rdrGlobal.m_supine.IsLoadingComplete() )
    GetDlgItem( IDC_VERIFY_LEFTINFO )->SetWindowText( rdrGlobal.m_supine.m_studyinfo.Format().c_str() );

  if ( rdrGlobal.IsStudyLoading() & DatasetOrientations::ePRONE || rdrGlobal.m_prone.IsLoadingComplete() )
    GetDlgItem( IDC_VERIFY_RIGHTINFO )->SetWindowText( rdrGlobal.m_prone.m_studyinfo.Format().c_str() );

  dynamic_cast< SkinButton * >( GetDlgItem( IDC_TEMPLATE_LINKVIEWS ) )->SetBackBrush( Theme::DefaultBackgroundBrush );

} // CreateChildViews()


/**
 * Called on idle
 */
void SupineProneLayout::GLIdle()
{
  if( ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D )
  {
    m_eDrawViews = Utilities::Datasets::IsLocked()? eVIEWBOTH : m_eLastActive;
    m_uInvalidChildViews = GLChildView::eGVT_SLICE;
  }  

  /*
  bool bFlying2d = ( ViatronStudy::m_orthogViewMode & ViewModes::AUTOFLY2D ) || 
                   ( ViatronStudy::m_obliqueViewMode & ViewModes::AUTOFLY2D );
  
  if( bFlying2d && rdrGlobal.IsDualDataLoaded() && Utilities::Datasets::IsLocked() )
  {
    SliceChildView& supineSlice = (SliceChildView&) GetChildViewByPos( 0 );
    SliceChildView& proneSlice  = (SliceChildView&) GetChildViewByPos( 1 );
    if( supineSlice.GetAutoflyMode() )
      rdrGlobal.UpdateWorldPosition( rdrGlobal.m_supine, rdrGlobal.m_prone );
    else
      rdrGlobal.UpdateWorldPosition( rdrGlobal.m_prone, rdrGlobal.m_supine );
  }*/
  ColonViatronixWnd::GLIdle();
} // GLIdle()


/**
 * Called when the child views need to be rendered.
 */
void SupineProneLayout::GLRenderViews()
{
  if ( m_uInvalidChildViews != uint4(GLChildView::eGVT_NONE ) )
  {
    if( ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D )
    {
      if( Utilities::Datasets::IsLocked() )
      {
        GLRenderView( 0 );
        GLRenderView( 1 );

			  if( ((SliceChildView&)GetChildViewByPos(0)).GetAutoflyMode() )
           rdrGlobal.UpdateWorldPosition(rdrGlobal.m_supine, rdrGlobal.m_prone);
			  else if( ((SliceChildView&)GetChildViewByPos(1)).GetAutoflyMode() )
           rdrGlobal.UpdateWorldPosition(rdrGlobal.m_prone, rdrGlobal.m_supine);
      }
      else
      {
        if( m_pSliceChildViewLeft->GetAutoflyMode() )
          GLRenderView( 0 );
      
        if( m_pSliceChildViewRight->GetAutoflyMode() )
          GLRenderView( 1 );  
      }
    }
    else
    {

      if( ( m_eDrawViews & eVIEWLEFT ) && ( m_uInvalidChildViews & GLChildView::eGVT_SLICE ) ) 
      {
        GLRenderView( 0 );
      }
      
      if( ( m_eDrawViews & eVIEWRIGHT ) && ( m_uInvalidChildViews & GLChildView::eGVT_SLICE ) )
      {
        GLRenderView( 1 );
      }
    }

    GLRenderView( 2 );
    if ( rdrGlobal.m_bDualDataDetected )
      GLRenderView( 3 );
  }


  m_uInvalidChildViews = GLChildView::eGVT_NONE;
  //m_eDrawViews  = eVIEWNONE;

} // GLRenderViews()


/**
 * Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void SupineProneLayout::OnSize( UINT uType, int iCX, int iCY ) 
{
  ColonViatronixWnd::OnSize(uType, iCX, iCY);

  // going back to resource-based layout placement....thank goodness!
  return;


  //
  // custom resizing code
  //

  // header fixed height at top
  const int4 iHeaderHeight = WinUtils::GetClientRect(GetDlgItem( IDC_TEMPLATE_PATIENTINFO )).Height();
  GetDlgItem( IDC_TEMPLATE_PATIENTINFO )->SetWindowPos( NULL, 0, 0, iCX, iHeaderHeight, SWP_NOZORDER );

  // determine the image size
  const int4 iLinkSize = 65;
  const int4 iControlsHeight = WinUtils::GetClientRect(GetDlgItem( IDC_TEMPLATE_CONTROLS )).Height();
  const int4 iGap = LAYOUT_GAP;
  const int4 iScrollbarHeight = LAYOUT_SCROLLBAR_THICKNESS;
  int4 iImageSize = min( ( iCX - 3 * iGap ) / 2, iCY - iHeaderHeight - iLinkSize - iScrollbarHeight - iControlsHeight - 4 * iGap );
  if ( SliceChildView::GetScrollBarLocationEnum()==SliceChildView::SBO_RIGHT )
  {
    if (iImageSize * 2 - iCX < iScrollbarHeight * 2)
    {
      iImageSize = min(iImageSize, (iCX - iScrollbarHeight*4 - 3 * iGap) / 2);
    }
  }

  const int4 iGapH = ( iCX - 2 * iImageSize ) / 3;
  const int4 iGapV = ( iCY - iHeaderHeight - iLinkSize - iImageSize - iScrollbarHeight - iControlsHeight ) / 4;

  // layout the link control
  const int4 iLinkX = iCX / 2 - iLinkSize / 2;
  const int4 iLinkY = iHeaderHeight + iGapV;
  GetDlgItem( IDC_TEMPLATE_LINKVIEWS )->SetWindowPos( NULL, iLinkX, iLinkY, iLinkSize, iLinkSize, SWP_NOZORDER );

  // layout the images
  const int4 iLeftX = iGapH;
  const int4 iRightX = 2 * iGapH + iImageSize;
  const int4 iImageY = iLinkY + iLinkSize + iGapV;
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW_SUPINE )->SetWindowPos( NULL, iLeftX, iImageY, iImageSize, iImageSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW_PRONE )->SetWindowPos( NULL, iRightX, iImageY, iImageSize, iImageSize, SWP_NOZORDER );

  // place the labels over the images
  const int4 iTextHeight = 24;
  const int4 iTextY = iImageY - iTextHeight - iGap;
  const int4 iTextXLeft = iLeftX + iImageSize / 2 - iImageSize / 2;
  const int4 iTextXRight = iRightX + iImageSize / 2 - iImageSize / 2;
  GetDlgItem( IDC_SUPINELABEL )->SetWindowPos( NULL, iTextXLeft, iTextY, iImageSize, iTextHeight, SWP_NOZORDER );
  GetDlgItem( IDC_PRONELABEL )->SetWindowPos( NULL, iTextXRight, iTextY, iImageSize, iTextHeight, SWP_NOZORDER );

  // layout the overview
  const int4 iOverviewWidth = iCX * 0.20F - iGap * 1.5F;
  const int4 iOverviewHeight = iControlsHeight - iGap;
  const int4 iOverviewY = iCY - iControlsHeight;
  const int4 iOverviewX = iGap;
  GetDlgItem( IDC_TEMPLATE_OVERVIEW )->SetWindowPos( NULL, iOverviewX, iOverviewY, iOverviewWidth, iOverviewHeight, SWP_NOZORDER );

  // layout the controls
  const int4 iControlsWidth = iCX - iOverviewWidth - iGap;
  const int4 iControlsX = iOverviewX + iOverviewWidth + iGap;
  const int4 iControlsY = iCY - iControlsHeight;
  GetDlgItem( IDC_TEMPLATE_CONTROLS )->SetWindowPos( NULL, iControlsX, iControlsY, iControlsWidth, iControlsHeight, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_ADJUSTTRANSLUCENCY )->SetWindowPos( NULL, iControlsX, iControlsY, iControlsWidth, iControlsHeight, SWP_NOZORDER );

  // make the overview be the "other" window if there is only one view
  if( !rdrGlobal.m_bDualDataDetected )
  {
    GetDlgItem(IDC_TEMPLATE_LINKVIEWS)->EnableWindow( FALSE );
    if( rdrGlobal.m_prone.IsLoadingComplete() || (rdrGlobal.IsStudyLoading() == DatasetOrientations::ePRONE) )
    {
      // Hide supine
      SwapViewRects(GetChildViewDlgItem(IDC_TEMPLATE_SLICEVIEW_SUPINE), GetChildViewDlgItem(IDC_TEMPLATE_OVERVIEW));
      GetChildViewDlgItem(IDC_TEMPLATE_SLICEVIEW_SUPINE).ShowWindow(false);
      GetDlgItem(IDC_TEMPLATE_SLICESCROLL1)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SUPINELABEL)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_TEMPLATE_OVERVIEW_SUPINE)->ShowWindow(SW_HIDE);
    }
    else
    {
      // Hide prone
      SwapViewRects(GetChildViewDlgItem(IDC_TEMPLATE_SLICEVIEW_PRONE), GetChildViewDlgItem(IDC_TEMPLATE_OVERVIEW));
      GetChildViewDlgItem(IDC_TEMPLATE_SLICEVIEW_PRONE).ShowWindow(false);
      GetDlgItem(IDC_TEMPLATE_SLICESCROLL2)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_PRONELABEL)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_TEMPLATE_OVERVIEW_PRONE)->ShowWindow(SW_HIDE);
    }
  } // if( !rdrGlobal.m_bDualDataDetected )

} // OnSize( UINT uType, int iCX, int iCY ) 


/**
 * Called when the mouse is moved over the view.
 *
 * @param   uFlags    Indicates button and keyboard state
 * @param   point     Mouse position in client coordinates
 */
void SupineProneLayout::OnMouseMove( UINT uFlags, CPoint point )
{
  ColonViatronixWnd::OnMouseMove( uFlags, point );

  GLChildViewRef currView = GetCurrentView();

  if( typeid( currView ) == typeid( SliceChildView ) )
    m_eLastActive = currView.GetClassInstanceID() == GetChildViewByPos(0).GetClassInstanceID() ? eVIEWLEFT : eVIEWRIGHT;
  
  if( m_uInvalidChildViews != GLChildView::eGVT_NONE )
    GLIdle();
} // OnMouseMove( UINT uFlags, CPoint point )


/**
 * Notifies view window of the current progress of study load
 *
 * @param eCompletedStep   The previously completed step
 * @param eCurrentStep     The next (current) study loading step
 * @param eOrientation     Orientation on which the next step is performing
 */
void SupineProneLayout::OnStudyLoadStepNotify(  const StudyLoad::StepsEnum eCompletedStep,
                                                  const StudyLoad::StepsEnum eCurrentStep, 
                                                  const DatasetOrientations::TypeEnum eOrientation )
/**********************************************************************************************************/
{
  try
  {
    if( typeid(GetChildViewByPos( 0 ) ) == typeid( SliceChildView ) )
    {
      if( rdrGlobal.m_supine.IsLoadingComplete() || rdrGlobal.m_supine.m_fields.m_sliceRendererData.IsValid() )
      {
        ( (SliceChildView &) GetChildViewByPos(0)).OnVolumeLoaded(0,0);
      }
      
      if( rdrGlobal.m_prone.IsLoadingComplete() || rdrGlobal.m_prone.m_fields.m_sliceRendererData.IsValid() )
      {
        ( (SliceChildView &) GetChildViewByPos(1)).OnVolumeLoaded(0,0);
      }
    }

    SliceOrientations::TypeEnum eSliceOrientation(SliceOrientations::eAXIAL);

    switch( eCompletedStep )
    {
      case StudyLoad::eHEADER:
        break;

      case StudyLoad::eVOLFIELDS: // Slice volumes is being loaded at this stage
        // just to trigger a slice update
        ((SliceChildView &) GetChildViewByPos(0)).ScrollSlice(1, 0); ((SliceChildView &) GetChildViewByPos(0)).ScrollSlice(-1, 0);
        ((SliceChildView &) GetChildViewByPos(1)).ScrollSlice(1, 0); ((SliceChildView &) GetChildViewByPos(1)).ScrollSlice(-1, 0);

        m_eDrawViews = eVIEWBOTH;
        UpdateWindow();
        m_eDrawViews = eVIEWALL;
        break;

      case StudyLoad::eSAGITTAL:
        eSliceOrientation = SliceOrientations::eSAGITTAL;
        break;

      case StudyLoad::eCORONAL:
        eSliceOrientation = SliceOrientations::eCORONAL;
        break;

      case StudyLoad::eAXIAL:
      case StudyLoad::eLOAD_COMPLETE:
      case StudyLoad::eINITIALIZE:
        // Set orientation according to current ViatronStudy::ViewWndMode
        switch (ViatronStudy::ViewWndModes.m_modeSPOrthoLayout)
        {
          case ViewModes::AXIAL:
            eSliceOrientation = SliceOrientations::eAXIAL;
            break;

          case ViewModes::SAGITTAL:
            eSliceOrientation = SliceOrientations::eSAGITTAL;
            break;

          case ViewModes::CORONAL:
            eSliceOrientation = SliceOrientations::eCORONAL;
            break;
          
          case ViewModes::CROSS_SECTION:
            eSliceOrientation = SliceOrientations::eCROSS_SECTION;
            break;
        }
        break;

      default:
        break;
    } // endswitch (step) 

    if (typeid(GetChildViewByPos(0)) == typeid(SliceChildView))
    {
      ((SliceChildView &) GetChildViewByPos(0)).SetOrientation(eSliceOrientation);
      ((SliceChildView &) GetChildViewByPos(1)).SetOrientation(eSliceOrientation);
    }
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "SupineProneLayout", "OnStudyLoadStepNotify" );
  }
} // OnStudyLoadStepNotify()


/**
 * Called when the linked button is clicked.
 */
void SupineProneLayout::OnLinkViewsClicked()
{
  Utilities::Datasets::ToggleLock();
} // OnLinkViewsClicked()


/**
 * Message: IDM_TOGGLE_AUTOFLY: Toggles Autofly mode.
 */
void SupineProneLayout::OnToggleAutofly()
{
  ColonViatronixWnd::OnToggleAutofly();

  bool bAutofly(ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D);
  ((SliceChildView&)GetChildViewByPos(m_eLastActive == eVIEWLEFT ? 0 : 1)).SetAutoflyMode(bAutofly);
  ((SliceChildView&)GetChildViewByPos(m_eLastActive == eVIEWLEFT ? 1 : 0)).SetAutoflyMode(false);
} // OnToggleAutofly()


/**
 * Adds a new snapshot with the overview and the main view window
 */
void SupineProneLayout::OnReportAddSnapshot()
{
  if ( m_eSupineProneLayoutType == eSPL_ORTHOGONAL )
  {
    try
    {
      std::list<GLChildViewPtr> childList, displayList;

      if (rdrGlobal.IsDualDataLoaded())
      {
        SliceChildView * pLeftSlice = dynamic_cast<SliceChildView*>(GetDlgItem(IDC_TEMPLATE_SLICEVIEW_SUPINE));
        SliceChildView * pRightSlice = dynamic_cast<SliceChildView*>(GetDlgItem(IDC_TEMPLATE_SLICEVIEW_PRONE));

        DrawOverView drawOVLeft (this, pLeftSlice ->GetDatasetRef());     
        DrawOverView drawOVRight(this, pRightSlice->GetDatasetRef());     

        childList.push_back(&drawOVLeft);
        childList.push_back(pLeftSlice);     displayList.push_back(pLeftSlice);

        childList.push_back(&drawOVRight);
        childList.push_back(pRightSlice);    displayList.push_back(pRightSlice);

        AddReportSnapshot(childList, displayList, MeasureLineList());
     }
      else
      {
        DrawOverView ovDrawImage(this, *rdrGlobal.m_pCurrDataset );     
        ovDrawImage.SetDisplayDistanceFromRectum(false);
        childList.push_back(&ovDrawImage);

        SliceChildView * pSlice = dynamic_cast<SliceChildView*>(GetDlgItem(rdrGlobal.m_supine.IsLoadingComplete() ? IDC_TEMPLATE_SLICEVIEW_SUPINE : IDC_TEMPLATE_SLICEVIEW_PRONE));

        childList.push_back(pSlice);      displayList.push_back(pSlice);

        AddReportSnapshot(childList, displayList, MeasureLineList());
      }

    }
    catch (...)
    {
    }
  }
} // OnReportAddSnapshot()


/**
 * Message: WM_USER_MOVIE_RECORD_GET_VIEW_ID: Returns the ID of the view to record for a movie.
 *
 * @param WPARAM   Unused.
 * @param LPARAM   Unused.
 * @return         ID of GLChildView to record.
 */
LRESULT SupineProneLayout::OnMovieRecordGetViewID(WPARAM, LPARAM)
{
  if (rdrGlobal.IsDualDataLoaded())
  {
    return GLChildView::GLVID(0);
  }
  else if (rdrGlobal.m_supine.IsLoadingComplete())
  {
    return GetChildViewByPos(0).GetClassInstanceID();
  }
  else if (rdrGlobal.m_prone.IsLoadingComplete())
  {
    return GetChildViewByPos(1).GetClassInstanceID();
  }
  else
  {
    return GLChildView::GLVID(0);
  }
} // OnMovieRecordGetViewID(WPARAM, LPARAM)


/**
 * Message: WM_APP_QUERY_ENABLE_SUPINE_PRONE_TOGGLE: Enables switching between Supine/Prone (by PatientInfoDlg)
 *
 * @param WPARAM   Unused
 * @param LPARAM   Unused
 * @return         TRUE to indicate toggling between Supine & Prone allowed
 */
LRESULT SupineProneLayout::OnQueryEnableSupineProneSwitch(WPARAM, LPARAM)
{
  return 1;
} // OnQueryEnableSupineProneSwitch()



/**
 * Handles the dataset locked changing 
 *
 * @param       args            DatasetsLockedArgs
 */
void SupineProneLayout::OnDatasetsLocked( const Colon::DatasetsLockedArgs & args )
{
  SetButtonIcon( *dynamic_cast< SkinButton * >( GetDlgItem( IDC_TEMPLATE_LINKVIEWS )), ( args.IsLocked() ) ? IDB_CHAIN_LINK : IDB_CHAIN_UNLINK );

  // compute the new offset for this orientation
  SliceChildView & view0 = (SliceChildView &) GetChildViewByPos(0);
  SliceChildView & view1 = (SliceChildView &) GetChildViewByPos(1);

  if( args.IsLocked() && view0.m_sliceViewer.GetOrientation() == view1.m_sliceViewer.GetOrientation() )
  {
    switch ( view0.m_sliceViewer.GetOrientation() )
    {
    case SliceOrientations::eAXIAL:
      rdrGlobal.m_vProneOffset.m_z = view1.m_sliceViewer.GetSliceNumber() - view0.m_sliceViewer.GetSliceNumber();
      break;
    case SliceOrientations::eSAGITTAL:
      rdrGlobal.m_vProneOffset.m_x = view1.m_sliceViewer.GetSliceNumber() - view0.m_sliceViewer.GetSliceNumber();
      break;
    case SliceOrientations::eCORONAL:
      rdrGlobal.m_vProneOffset.m_y = view1.m_sliceViewer.GetSliceNumber() - view0.m_sliceViewer.GetSliceNumber();
      break;
    default:
      break;
    }
  }

  // enable the button if the viewer's are showing the same orientations
  GetDlgItem( IDC_TEMPLATE_LINKVIEWS )->EnableWindow( view0.m_sliceViewer.GetOrientation() == view1.m_sliceViewer.GetOrientation() );

  if( ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D ) 
    ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D; 
} // OnDatasetsLocked( args )


/**
 * Is datasets locking tracked by this view
 */
bool SupineProneLayout::IsDatasetsLockingEnabled() const
{
  if( m_bBypassDatasetLockingCheck )
    return true;

  CWnd * pWnd( GetDlgItem( IDC_TEMPLATE_LINKVIEWS ));
  if( pWnd )
    return pWnd->IsWindowEnabled();

  return true;
} // IsDatasetsLockingEnabled()