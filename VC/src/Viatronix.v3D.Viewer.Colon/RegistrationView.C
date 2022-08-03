// $Id: RegistrationView.C,v 1.35.2.16 2010/04/17 05:15:41 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: RegistrationView.C
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

/// includes
#include "stdafx.h"
#include <mmsystem.h>

//#include "OverView.h"
//#include "V1KMessages.h"
//#include "WinUtils.h"
//#include "ReaderGlobal.h"
//#include "SlidingPaneView.h"

#include "DrawOverView.h"
#include "Theme.h"
#include "NavigationView.h"
#include "SliceChildView.h"
#include "MainAppFrame.h"
#include "ColorProgressCtrl.h"

#include "RegistrationView.h"
#include "ApplicationSettings.h"
#include "ApplicationController.h"
#include "vcUtils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// usings
using namespace ColonViews::Layouts;
using namespace ReaderLib;
using namespace WinUtils;

#define FILE_REVISION "$Revision: 1.35.2.16 $"
#define APP_SETTING_FLY_BOTH "FlyBoth"


/////////////////////////////////////////////////////////////////////////////
// RegistrationViewFrame

IMPLEMENT_DYNCREATE(RegistrationLayoutFrame, CMDIChildWnd)


/**
 * Constructor 
 */
RegistrationLayoutFrame::RegistrationLayoutFrame() : ChildLayoutFrame( new RegistrationLayout )
{
} // RegistrationViewFrame()


BEGIN_MESSAGE_MAP(RegistrationLayoutFrame, ChildLayoutFrame)
  //{{AFX_MSG_MAP(RegistrationLayoutFrame)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////
// RegistrationLayout Class
//////////////////////////////////////////////////////////////////////

// Positions of views and controls in vectors
enum RegViewVectorPosEnum { eReg_SUPINE_SLICEVIEW = 0, 
                            eReg_PRONE_SLICEVIEW, 
                            eReg_SUPINE_ENDOVIEW,
                            eReg_PRONE_ENDOVIEW, 
                            NUM_REGISTRATION_VIEWS };

/**
 * Constructor: Note: Begins with Suzi's registration activated.
 */
RegistrationLayout::RegistrationLayout()
: ColonViatronixWnd(),
  m_pPrimaryOverviewWnd( NULL ),
  m_pDualOverviewWnd( NULL ),
  m_bMiddleMouseDown( false ),
  m_bMiddleMouseSupine( false ),
  m_bPanesInitialized( false ),
  m_bBypassDatasetLockingCheck( true )
{
  if( rdrGlobal.m_pCurrDataset->GetOrientation()==DatasetOrientations::eSUPINE )
  {
    m_eUpdateRegPosn = eUpdateProne2d; 
  }
  else
  {
    m_eUpdateRegPosn=eUpdateSupine3d; 
  }

  // initialize 'fly both' data
	ApplicationSettings::UserSettings().SetBool(APP_SETTING_FLY_BOTH, rdrGlobal.m_bDual3DEndoFly);
} // RegistrationLayout()


/**
 * Destructor 
 */
RegistrationLayout::~RegistrationLayout()
{
} // ~RegistrationLayout()
  

BEGIN_MESSAGE_MAP(RegistrationLayout, ColonViatronixWnd)
  //{{AFX_MSG_MAP(RegistrationLayout)
  ON_WM_PAINT()
  ON_COMMAND(IDM_REPORT_ADDSNAPSHOT, OnReportAddSnapshot)
  ON_UPDATE_COMMAND_UI(IDM_REPORT_ADDSNAPSHOT, OnUpdateReportAddSnapshot)
  ON_COMMAND(IDM_MEASUREMENTS_DELETEALL, OnMeasurementsDeleteAll)
  ON_UPDATE_COMMAND_UI(IDM_MEASUREMENTS_DELETEALL, OnUpdateMeasurementsDeleteAll)
  ON_COMMAND(IDM_BOOKMARKS_ADDBOOKMARK, OnBookmarksAddBookmark)
  ON_COMMAND(IDM_JUMP_PREVSEGMENT, OnJumpPrevSegment)
  ON_COMMAND(IDM_JUMP_NEXTSEGMENT, OnJumpNextSegment)

  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE, OnToggleLineMeasure)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_LIGHTING, OnToggleLighting)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_ROIMEASURE, OnToggleRoiMeasure)
  ON_COMMAND(ID_ENDOSCOPICVIEW_OPTIONS_TRANSLUCENT_DISPLAY, OnToggleTranslucent)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION, OnToggleSegmentRegion)

  ON_COMMAND(IDM_TOGGLE_AUTOFLY, OnToggleAutofly)
  ON_COMMAND(IDM_TOGGLE_VIEW_DIRECTION, OnToggleViewDirection)
  ON_MESSAGE(WM_APP_QUERY_ENABLE_SUPINE_PRONE_TOGGLE, OnQueryEnableSupineProneSwitch)
  ON_MESSAGE(WM_APP_SYNCRONIZE_DATASET_LOCATIONS, OnSyncronizeDatasetLocations)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * Called when the window is being created.
 *
 * @param lpCreateStruct  CREATESTRUCT structure that contains information about the CWnd object being created
 * @return                0 to continue creation, -1 to destroy the window  
 */
int RegistrationLayout::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
  if( ColonViatronixWnd::OnCreate( lpCreateStruct ) == -1 )
    return -1;
  
  m_bBypassDatasetLockingCheck = false;

  return 0;
} // OnCreate()


/**
 * Called when the window is being destroyed
 */
void RegistrationLayout::OnDestroy()
{  
  m_bBypassDatasetLockingCheck = true;
  ColonViatronixWnd::OnDestroy();
} // OnDestroy();


/**
 * Derived classes can override to remove areas from the parent region
 *
 * @param   parentRgn     Updates the parent region removing areas to update as necessary
 */
void RegistrationLayout::OnUpdateBackgroundRegion( CRgn & parentRgn )
{
  // remove the logo from the update 
  CRgn logoRgn;
  logoRgn.CreateRectRgn( 0, 0, Theme::LogoRegistrationView->GetWidth(), Theme::LogoRegistrationView->GetHeight() );
  parentRgn.CombineRgn( &parentRgn, &logoRgn, RGN_DIFF );

} // OnUpdateBackgroundRegion( CRgn & parentRgn )


/**
 * returns the resource ID for landscape window orientation
 * @return resource ID
 */
uint4 RegistrationLayout::GetLandscapeLayoutID()
{
  return (rdrGlobal.m_supine.m_cadFindings.GetCount()>0 || rdrGlobal.m_prone.m_cadFindings.GetCount()>0) ? IDT_REGISTRATION_CAD_LANDSCAPE : IDT_REGISTRATION_LANDSCAPE;
} // GetLandscapeLayoutID()


/**
 * returns the resource ID for widescreen window orientation
 * @return resource ID
 */
uint4 RegistrationLayout::GetWidescreenLayoutID()
{
  return (rdrGlobal.m_supine.m_cadFindings.GetCount()>0 || rdrGlobal.m_prone.m_cadFindings.GetCount()>0) ? IDT_REGISTRATION_CAD_WIDESCREEN : IDT_REGISTRATION_WIDESCREEN;
} // GetWidescreenLayoutID()


/**
 * returns the resource ID for portrait window orientation
 * @return resource ID
 */
uint4 RegistrationLayout::GetPortraitLayoutID()
{
  return (rdrGlobal.m_supine.m_cadFindings.GetCount()>0 || rdrGlobal.m_prone.m_cadFindings.GetCount()>0) ? IDT_REGISTRATION_CAD : IDT_REGISTRATION;
} // GetPortraitLayoutID()


/**
 * returns the resource ID for a large portrait window orientation
 * @return resource ID
 */
uint4 RegistrationLayout::GetPortraitLargeLayoutID()
{
  return (rdrGlobal.m_supine.m_cadFindings.GetCount()>0 || rdrGlobal.m_prone.m_cadFindings.GetCount()>0) ? IDT_REGISTRATION_CAD_PORTRAIT_LARGE : IDT_REGISTRATION_PORTRAIT_LARGE;
} // GetPortraitLargeLayoutID()


/**
 * OnPaint message handler
 */
void RegistrationLayout::OnPaint() 
{
  ColonViatronixWnd::OnPaint();

  if ( GetDlgItem(IDC_TEMPLATE_PATIENTINFO2) == NULL )
  {
    Gdiplus::Graphics gfx( GetSafeHwnd() );
    gfx.DrawImage( Theme::LogoRegistrationView, 0, 0 );
  }

} // OnPaint()


/**
 * WM_SIZE: Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void RegistrationLayout::OnSize( UINT uType, int iCX, int iCY ) 
{

  ColonViatronixWnd::OnSize(uType, iCX, iCY);

  if ( iCX < 1 || iCY < 1 )
    return;

  // obsolete functionality follows
  return;
  
  //
  // custom resizing code
  //

  // size the sliding panes
  const int4 iGap = LAYOUT_GAP;
  const int4 iPaneWidth = 327;
  const int4 iPaneStartY = 80;
  const int4 iPaneHeight = iCY - iPaneStartY;
  
  // layout the pane headers
  const int4 iPaneHeaderHeight = 25;
  const int4 iPaneHeaderGap = 2;
  const int4 iPanePatientY = iPaneStartY;
  const int4 iPanePrimaryY = iPanePatientY + iPaneHeaderHeight + iPaneHeaderGap;
  const int4 iPaneCadY = iPanePrimaryY + iPaneHeaderHeight + iPaneHeaderGap;
  const int4 iPaneDualY = iPaneCadY + iPaneHeaderHeight + iPaneHeaderGap;
  const int4 iPaneControlsY = iPaneDualY + iPaneHeaderHeight + iPaneHeaderGap;
  const int4 iPaneNotationsY = iPaneControlsY + iPaneHeaderHeight + iPaneHeaderGap;

  // layout the pane contents
  const int4 iPanePatientHeight = 147;
  const int4 iPanePrimaryHeight = 251;
  const int4 iPaneCadHeight = 380;
  const int4 iPaneDualHeight = 505;
  const int4 iPaneControlsHeight = 404;
  const int4 iPaneNotationsHeight = 146;

  if( !m_bPanesInitialized )
  {
    GetDlgItem( IDC_RSP_PANEHDR_PATIENTINFO )->SetWindowPos( NULL, 0, iPanePatientY, iPaneWidth, iPaneHeaderHeight, SWP_NOZORDER );
    if ( GetDlgItem( IDC_RSP_PANEHDR_SUPINECAD ) != NULL ) GetDlgItem( IDC_RSP_PANEHDR_SUPINECAD )->SetWindowPos( NULL, 0, iPanePrimaryY, iPaneWidth, iPaneHeaderHeight, SWP_NOZORDER );
    GetDlgItem( IDC_RSP_PANEHDR_DUALOVERVIEW )->SetWindowPos( NULL, 0, iPaneDualY, iPaneWidth, iPaneHeaderHeight, SWP_NOZORDER );
    GetDlgItem( IDC_RSP_PANEHDR_CONTROLS )->SetWindowPos( NULL, 0, iPaneControlsY, iPaneWidth, iPaneHeaderHeight, SWP_NOZORDER );
    GetDlgItem( IDC_RSP_PANEHDR_NOTATIONS )->SetWindowPos( NULL, 0, iPaneNotationsY, iPaneWidth, iPaneHeaderHeight, SWP_NOZORDER );
    
    GetDlgItem( IDC_TEMPLATE_PATIENTINFO )->SetWindowPos( NULL, 0, iPanePatientY + iPaneHeaderHeight, iPaneWidth, iPanePatientHeight, SWP_NOZORDER );
    if ( GetDlgItem( IDT_PANE_OVERVIEW_PRIMARY ) != NULL ) GetDlgItem( IDT_PANE_OVERVIEW_PRIMARY )->SetWindowPos( NULL, 0, iPanePrimaryY + iPaneHeaderHeight, iPaneWidth, iPanePrimaryHeight, SWP_NOZORDER );
    if ( GetDlgItem( IDC_TEMPLATE_CADRESULTSDLG ) != NULL ) GetDlgItem( IDC_TEMPLATE_CADRESULTSDLG )->SetWindowPos( NULL, 0, iPaneCadY + iPaneHeaderHeight, iPaneWidth, iPaneCadHeight, SWP_NOZORDER );
    GetDlgItem( IDT_PANE_OVERVIEW_DUAL )->SetWindowPos( NULL, 0, iPaneDualY + iPaneHeaderHeight, iPaneWidth, iPaneDualHeight, SWP_NOZORDER );
    GetDlgItem( IDC_TEMPLATE_CONTROLS )->SetWindowPos( NULL, 0, iPaneControlsY + iPaneHeaderHeight, iPaneWidth, iPaneControlsHeight, SWP_NOZORDER );
    GetDlgItem( IDC_TEMPLATE_NOTATIONS )->SetWindowPos( NULL, 0, iPaneNotationsY + iPaneHeaderHeight, iPaneWidth, iPaneNotationsHeight, SWP_NOZORDER );

    m_bPanesInitialized = true;
  }
  // layout the embedded overviews
  const int4 iOverviewWidth = iPaneWidth - 2;
  const int4 iOverviewPrimaryHeight = iPanePrimaryHeight - 2;
  const int4 iOverviewGap = 2;
  const int4 iOverviewDualHeight = ( iPaneDualHeight - 2 - iOverviewGap ) / 2;
  const int4 iOverviewDualY = iOverviewDualHeight + iOverviewGap;
  if ( m_pPrimaryOverviewWnd != NULL ) m_pPrimaryOverviewWnd->GetChildViewByPos( 0 ).SetWindowPos( NULL, 1, 1, iOverviewWidth, iOverviewPrimaryHeight, SWP_NOZORDER );
  m_pDualOverviewWnd->GetChildViewByPos( 0 ).SetWindowPos( NULL, 1, 1, iOverviewWidth, iOverviewDualHeight, SWP_NOZORDER );
  m_pDualOverviewWnd->GetChildViewByPos( 1 ).SetWindowPos( NULL, 1, iOverviewDualY, iOverviewWidth, iOverviewDualHeight, SWP_NOZORDER );

  // tell the panes to re-stack
  GetDlgItem( IDC_SLIDING_REGISTRATION_PANE )->SetWindowPos( NULL, 0, iPaneStartY, iPaneWidth, iPaneHeight, SWP_NOZORDER );

  // layout slice headers
  const int4 iHeaderHeight = 30;
  const int4 iCertaintyWidth = 20;
  const int4 iScrollbarHeight = LAYOUT_SCROLLBAR_THICKNESS;
  const int4 iBuddyHeight = LAYOUT_BUDDY_HEIGHT;
  const int4 iImageSizeHorizontal = ( iCX - iPaneWidth - 5 * iGap - iCertaintyWidth ) / 2;
  const int4 iImageSizeVertical = ( iCY - iHeaderHeight - iScrollbarHeight - 2 * iGap - iBuddyHeight ) / 2;
  const int4 iImageSize = min( iImageSizeHorizontal, iImageSizeVertical );
  const int4 iLinkWidth = 93;
  const int4 iLinkHeight = 20;
  const int4 iHorizontalGap = ( iCX - iGap - iPaneWidth - 2 * iImageSize - iCertaintyWidth ) / 4;
  const int4 iLeftX = iPaneWidth + iGap + iHorizontalGap;
  const int4 iRightX = iCX - iHorizontalGap - iImageSize;
  const int4 iCenterX = ( iLeftX + iImageSize + iRightX ) / 2;
  const int4 iLinkX = iCenterX - iLinkWidth / 2;
  const int4 iLinkY = iHeaderHeight / 2 - iLinkHeight / 2;
  GetDlgItem( IDC_SUPINELABEL )->SetWindowPos( NULL, iLeftX, 0, iImageSize, iHeaderHeight - 2, SWP_NOZORDER );
  GetDlgItem( IDC_PRONELABEL )->SetWindowPos( NULL, iRightX, 0, iImageSize, iHeaderHeight - 2, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_LINKVIEWS )->SetWindowPos( NULL, iLinkX, iLinkY, iLinkWidth, iLinkHeight, SWP_NOZORDER );

  // layout slice views
  const int4 iScrollbarY = iHeaderHeight + iImageSize;
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW_SUPINE )->SetWindowPos( NULL, iLeftX, iHeaderHeight, iImageSize, iImageSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW_PRONE )->SetWindowPos( NULL, iRightX, iHeaderHeight, iImageSize, iImageSize, SWP_NOZORDER );

  // layout endoscopic views
  const int4 iEndoY = iCY - iGap - iBuddyHeight - iImageSize;
  GetDlgItem( IDC_TEMPLATE_ENDOSCOPIC_SUPINE )->SetWindowPos( NULL, iLeftX, iEndoY, iImageSize, iImageSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_ENDOSCOPIC_PRONE )->SetWindowPos( NULL, iRightX, iEndoY, iImageSize, iImageSize, SWP_NOZORDER );

  // layout 3D link
  const int4 iLink3DWidth = iLinkHeight;
  const int4 iLink3DHeight = iLinkWidth;
  const int4 iLink3DX = iCenterX - iLink3DWidth / 2;
  GetDlgItem( IDC_TEMPLATE_FLYBOTH )->SetWindowPos( NULL, iLink3DX, iEndoY, iLink3DWidth, iLink3DHeight, SWP_NOZORDER );

  // layout the registration certainty meter
  const int4 iCertaintyX = iCenterX - iCertaintyWidth / 2;
  const int4 iCertaintyY = iEndoY + iLink3DHeight + iGap;
  const int4 iCertaintyHeight = iCY - iGap - iBuddyHeight - iCertaintyY;
  GetDlgItem( IDC_RV_PROGRESS_CTRL_REGCONFIDENCE )->SetWindowPos( NULL, iCertaintyX, iCertaintyY, iCertaintyWidth, iCertaintyHeight, SWP_NOZORDER );

} // OnSize()


/**
 * Create child views for this window.
 *
 * @param hWndThis   Handle to this window.
 */
void RegistrationLayout::CreateChildViews(HWND hWndThis)
{
  ColonViatronixWnd::CreateChildViews(hWndThis);

  SkinButton * pBtn;
  if (pBtn = dynamic_cast<SkinButton*> (GetDlgItem(IDC_TEMPLATE_LINKVIEWS)) )
  {
    if( Utilities::Datasets::IsLocked() )
      SetButtonIcon(*pBtn, IDB_DATASET_LINK);
    else
      SetButtonIcon(*pBtn, IDB_DATASET_UNLINK);
    
    pBtn->SetBackBrush( Theme::DefaultBackgroundBrush );
  }
  if (pBtn = dynamic_cast<SkinButton*> (GetDlgItem(IDC_TEMPLATE_FLYBOTH)))
  {
    
    const bool bEnable(rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::AllowDual3DFlight()));
    pBtn->ShowWindow(bEnable ? SW_SHOW : SW_HIDE);
    pBtn->SetBackBrush( Theme::DefaultBackgroundBrush );
    pBtn->EnableWindow(Utilities::Datasets::IsLocked());

		SetButtonIcon(*pBtn, rdrGlobal.m_bDual3DEndoFly ? IDB_DATASET_LINK_VERTICAL : IDB_DATASET_UNLINK_VERTICAL);
  }

  // Initialize slice view neighbours
  SliceChildView & rSliceView0 = (SliceChildView &)GetChildViewByPos(eReg_SUPINE_SLICEVIEW);
  SliceChildView & rSliceView1 = (SliceChildView &)GetChildViewByPos(eReg_PRONE_SLICEVIEW);
  rSliceView0.m_pNeighbourSliceView = &rSliceView1;
  rSliceView1.m_pNeighbourSliceView = &rSliceView0;

  // Initialize confidence control
  ColorProgressCtrl * pRegConfidenceCtrl = dynamic_cast<ColorProgressCtrl*>(GetDlgItem(IDC_RV_PROGRESS_CTRL_REGCONFIDENCE));
  pRegConfidenceCtrl->SetRange(0,100);
  pRegConfidenceCtrl->SetPos(0);

  //{{ Initialize pane view
  Panes::SlidingPaneManager* pPaneMgr = dynamic_cast<Panes::SlidingPaneManager*>(GetDlgItem(IDC_SLIDING_REGISTRATION_PANE));
  GetDlgItem(IDC_SLIDING_REGISTRATION_PANE)->ShowWindow( SW_HIDE );
  
  // Show patient info and dual overview pane
  if ( GetDlgItem(IDC_RSP_PANEHDR_PATIENTINFO) != NULL )
    pPaneMgr->ShowPane(IDC_RSP_PANEHDR_PATIENTINFO, true);
  if ( GetDlgItem(IDC_RSP_PANEHDR_DUALOVERVIEW) != NULL )
    pPaneMgr->ShowPane(IDC_RSP_PANEHDR_DUALOVERVIEW, true);
  else if ( GetDlgItem(IDC_RSP_PANEHDR_CONTROLS) != NULL )
    pPaneMgr->ShowPane(IDC_RSP_PANEHDR_CONTROLS, true);

  m_pPrimaryOverviewWnd = dynamic_cast< GLWnd * >( GetDlgItem( IDT_PANE_OVERVIEW_PRIMARY ) );
  m_pDualOverviewWnd = dynamic_cast< GLWnd * >( GetDlgItem( IDT_PANE_OVERVIEW_DUAL ) );  


  if (m_pPrimaryOverviewWnd != NULL)
  {
    OverView& rPrimaryOverview = (OverView&) GLChildViewRef(*(dynamic_cast<ViatronixChildWnd*>(GetDlgItem(IDT_PANE_OVERVIEW_PRIMARY))));
    rPrimaryOverview.Initialize(NULL, NULL, OverviewRender::eRegisteredViews);
  }

  // Set title in dual overview pane
  CString sDualTitle;  sDualTitle.LoadString(IDS_CHILDVIEW_TITLE_VTXFMT);
  sDualTitle.Replace("#<view>", LoadResourceString(IDS_OVERVIEW));

  if ( GetDlgItem(IDC_RSP_PANEHDR_DUALOVERVIEW) )
  {
    std::string sDatasets = ColonViatronixWnd::PatientOrientationToString(rdrGlobal.m_supine, true)
      + '/' + ColonViatronixWnd::PatientOrientationToString(rdrGlobal.m_prone, true);
    sDualTitle.Replace("#<orientation>", sDatasets.c_str());
    pPaneMgr->SetPaneText(IDC_RSP_PANEHDR_DUALOVERVIEW, LPCTSTR(sDualTitle));
  }

// Sets the title of the cad panes
  SetDatasetPaneTitles();

  OverView* pSupineOverview = (OverView*) (GetDlgItem(IDC_TEMPLATE_OVERVIEW_SUPINE));
  OverView* pProneOverview = (OverView*) (GetDlgItem(IDC_TEMPLATE_OVERVIEW_PRONE));
  if (m_pDualOverviewWnd != NULL)
  {
    GLWndRef rDualView = dynamic_cast<GLWndRef>(*(GetDlgItem(IDT_PANE_OVERVIEW_DUAL)));

    pSupineOverview = (OverView*) (rDualView.GetDlgItem(IDC_TEMPLATE_OVERVIEW_SUPINE));
    pProneOverview = (OverView*) (rDualView.GetDlgItem(IDC_TEMPLATE_OVERVIEW_PRONE));
  }

  if ( (pSupineOverview != NULL) && (pProneOverview != NULL) )
  {
    // Initialize neighbours in dual overview pane
    pSupineOverview->Initialize(pProneOverview, NULL, OverviewRender::eRegisteredViews);
    pProneOverview->Initialize(pSupineOverview, NULL, OverviewRender::eRegisteredViews);
  }

  ReaderLib::StudyInfo infoLeft  = rdrGlobal.m_supine.m_studyinfo;
  ReaderLib::StudyInfo infoRight = rdrGlobal.m_prone .m_studyinfo;

  std::string  sLeftLabel = PatientOrientationToString(rdrGlobal.m_supine) + ": " + infoLeft .m_sPatientID + ", " + infoLeft .m_sExamTime;
  std::string sRightLabel = PatientOrientationToString(rdrGlobal.m_prone)  + ": " + infoRight.m_sPatientID + ", " + infoRight.m_sExamTime;

  GetDlgItem(IDC_SUPINELABEL)->SetWindowText( sLeftLabel.c_str() );
  GetDlgItem(IDC_PRONELABEL)->SetWindowText ( sRightLabel.c_str() );

  //}} (end) Initialize pane view

  rdrGlobal.m_vProneOffset = Triple<int4>(0, 0, 0);
  rdrGlobal.UpdateWorldPosition( *rdrGlobal.m_pCurrDataset, *rdrGlobal.m_pSecondaryDataset );
} // CreateChildViews(HWND hWndThis)


/**
 * Handles the initialized event
 */
void RegistrationLayout::OnInitialized()
{
  ColonViatronixWnd::OnInitialized();
  if (m_pPrimaryOverviewWnd != NULL)
    m_pPrimaryOverviewWnd->OnInitialized();
  if (m_pDualOverviewWnd != NULL)
    m_pDualOverviewWnd->OnInitialized();
  SetFocus();
} // OnInitialized()


/**
 * Command handler.
 *
 * @param wParam   Identifies menu command.
 * @param lParam   Unused.
 * @return BOOL    TRUE if command handled, FALSE (for default handler) otherwise.
 */
BOOL RegistrationLayout::OnCommand(WPARAM wParam, LPARAM lParam)
{
  switch (wParam)
  {
    case IDC_TEMPLATE_LINKVIEWS:
    {
      Utilities::Datasets::ToggleLock();
      break;
    }

    case IDC_TEMPLATE_FLYBOTH:
    {
		  rdrGlobal.m_bDual3DEndoFly = !rdrGlobal.m_bDual3DEndoFly;
      SkinButton * pBtn = dynamic_cast< SkinButton * >( GetDlgItem( IDC_TEMPLATE_FLYBOTH ) );
      if (pBtn != NULL )
				SetButtonIcon(*pBtn, rdrGlobal.m_bDual3DEndoFly ? IDB_DATASET_LINK_VERTICAL : IDB_DATASET_UNLINK_VERTICAL);
      break;
    }

    default:
      break;
  }

  return ColonViatronixWnd::OnCommand(wParam, lParam);
} // OnCommand(WPARAM wParam, LPARAM lParam)


/**
 * Translate window messages before they are dispatched to theTranslateMessage andDispatchMessage Windows functions
 *
 * @param pMsg   Structure that contains the message to process.
 * @return BOOL  Non-zero if msg should not be dispatched, 0 if system should translate and dispatch msg.
 */
BOOL RegistrationLayout::PreTranslateMessage(MSG* pMsg) 
{
  // Since Notations menu is disabled (see constructor), we must specifically handle the F2 add-bookmark msg
  if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_F2))
  {
    AfxGetMainWnd()->PostMessage(WM_COMMAND, MAKEWPARAM(IDM_BOOKMARKS_ADDBOOKMARK, 0), 0);
    return TRUE;
  }

  return ColonViatronixWnd::PreTranslateMessage(pMsg);
} // PreTranslateMessage(MSG* pMsg) 


/**
 * Idle-time processing
 */
void RegistrationLayout::GLIdle()
{
  ColorProgressCtrl * pRegConfidenceCtrl = dynamic_cast<ColorProgressCtrl*>(GetDlgItem(IDC_RV_PROGRESS_CTRL_REGCONFIDENCE));
  if( pRegConfidenceCtrl && rdrGlobal.m_supine.IsLoadingComplete() && rdrGlobal.m_prone.IsLoadingComplete( ))
  {
    Dataset& rDataset = *(rdrGlobal.m_pCurrDataset);
    uint4 uSupineJoinedCenterlineIndex = rDataset.m_joinedSkeleton.ConvertToJoinedIndex(
      rDataset.m_segmentList.GetCurrentSegment(), rDataset.m_fields.GetClosestSkeletonIndex( rDataset.m_submarine.GetPositionVolume() ));
    float4 fConfidence = rDataset.m_joinedSkeleton.GetRegistrationConfidence(uSupineJoinedCenterlineIndex);

    if (pRegConfidenceCtrl->GetPos() != int4(fConfidence*100.0))
    {
      const float4 fColorVal(rdrGlobal.ColorFunction(fConfidence));
      pRegConfidenceCtrl->SetPosAndColor(fConfidence*100, RGB(224*(1.0-fColorVal), 255*fColorVal, 32*(1.0-fColorVal)));
    }
  }
  
  ColonViatronixWnd::GLIdle();

  if( m_bMiddleMouseDown )
    return;

  if( m_pPrimaryOverviewWnd != NULL && m_pPrimaryOverviewWnd->IsWindowVisible() )
    m_pPrimaryOverviewWnd->PostMessage( WM_APP_GL_IDLE, 0, 0 );
  
  if( m_pDualOverviewWnd != NULL && m_pDualOverviewWnd->IsWindowVisible() )
    m_pDualOverviewWnd->PostMessage( WM_APP_GL_IDLE, 0, 0 );

} // GLIdle()


/**
 * Renders views.
 */
void RegistrationLayout::GLRenderViews()
{
  CWnd * pOverview = NULL;

  // are we flying in 3d for supine?
  if( rdrGlobal.m_supine.m_submarine.IsFlying() )
  {
    Timer timer;

    GLRenderView( eReg_SUPINE_ENDOVIEW );
    if ( (pOverview = GetDlgItem(IDC_TEMPLATE_OVERVIEW_SUPINE)) != NULL )
    {
      GLRenderView(GLChildViewRef(*pOverview));
    }
		if (rdrGlobal.m_bDual3DEndoFly && Utilities::Datasets::IsLocked())
    {
      GLRenderView( eReg_PRONE_ENDOVIEW );
      if ( (pOverview = GetDlgItem(IDC_TEMPLATE_OVERVIEW_PRONE)) != NULL )
      {
        GLRenderView(GLChildViewRef(*pOverview));
      }
    }

    m_eUpdateRegPosn |= eUpdateSupine2d;
    if( Utilities::Datasets::IsLocked() )
    {
      rdrGlobal.UpdateWorldPosition(rdrGlobal.m_supine, rdrGlobal.m_prone);
      m_eUpdateRegPosn |= eUpdateProne2d;
      m_eUpdateRegPosn |= eUpdateProne3d;
    }
    rdrGlobal.m_supine.m_fields.m_volumeRender.SetLastRenderTime( timer.ElapsedTime() );

  } // 3d supine flying
  
  // are we flying in 3d for prone?
  else if ( rdrGlobal.m_prone.m_submarine.IsFlying() )
  {
    Timer timer;

    GLRenderView( eReg_PRONE_ENDOVIEW );
    if ( (pOverview = GetDlgItem(IDC_TEMPLATE_OVERVIEW_PRONE)) != NULL )
    {
      GLRenderView(GLChildViewRef(*pOverview));
    }
		if (rdrGlobal.m_bDual3DEndoFly && Utilities::Datasets::IsLocked())
    {
      GLRenderView( eReg_SUPINE_ENDOVIEW );
      if ( (pOverview = GetDlgItem(IDC_TEMPLATE_OVERVIEW_SUPINE)) != NULL )
      {
        GLRenderView(GLChildViewRef(*pOverview));
      }
    }

    m_eUpdateRegPosn |= eUpdateProne2d;
    if( Utilities::Datasets::IsLocked() )
    {
      rdrGlobal.UpdateWorldPosition( rdrGlobal.m_prone, rdrGlobal.m_supine );
      m_eUpdateRegPosn |= eUpdateSupine2d;
      m_eUpdateRegPosn |= eUpdateSupine3d;
    }
    rdrGlobal.m_prone.m_fields.m_volumeRender.SetLastRenderTime( timer.ElapsedTime() );
  } // 3d prone flying

  // are we flying in 2d?
  else if( ( ViatronStudy::m_modeOrthogView & ViewModes::AUTOFLY2D ) || ( ViatronStudy::m_modeObliqueView & ViewModes::AUTOFLY2D))
  {
    Timer timer;
    SliceChildView& supineSlice = (SliceChildView&) GetChildViewByPos( eReg_SUPINE_SLICEVIEW );
    SliceChildView& proneSlice  = (SliceChildView&) GetChildViewByPos( eReg_PRONE_SLICEVIEW );

    if( supineSlice.GetAutoflyMode() )
    {
      GLRenderView( supineSlice );

      if( SliceChildView::m_bUpdateEndoluminalView ) 
        GLRenderView( eReg_SUPINE_ENDOVIEW );
      else
        m_eUpdateRegPosn |= eUpdateSupine3d;
      
      if( Utilities::Datasets::IsLocked() )
      {
        rdrGlobal.UpdateWorldPosition( rdrGlobal.m_supine, rdrGlobal.m_prone );
        GLRenderView( proneSlice );
        m_eUpdateRegPosn |= eUpdateProne3d;
      }
      rdrGlobal.m_supine.m_fields.m_volumeRender.SetLastRenderTime( timer.ElapsedTime() );
    }
    
    if( proneSlice.GetAutoflyMode() )
    {
      GLRenderView( proneSlice );

      if( SliceChildView::m_bUpdateEndoluminalView )   
        GLRenderView( eReg_PRONE_ENDOVIEW );
      else
        m_eUpdateRegPosn |= eUpdateProne3d; 
      
      if( Utilities::Datasets::IsLocked() )
      {
        rdrGlobal.UpdateWorldPosition( rdrGlobal.m_prone, rdrGlobal.m_supine );
        GLRenderView( supineSlice );
        m_eUpdateRegPosn |= eUpdateSupine3d;
      }
      rdrGlobal.m_prone.m_fields.m_volumeRender.SetLastRenderTime( timer.ElapsedTime() );
    }

    if ( (pOverview = GetDlgItem(IDC_TEMPLATE_OVERVIEW_SUPINE)) != NULL )
    {
      GLRenderView(GLChildViewRef(*pOverview));
    }
    if ( (pOverview = GetDlgItem(IDC_TEMPLATE_OVERVIEW_PRONE)) != NULL )
    {
      GLRenderView(GLChildViewRef(*pOverview));
    }
  } // 2d flying
  
  // not flying in 2d or 3d but requires an update
  else if( m_eUpdateRegPosn != eUpdateNone || m_uInvalidChildViews != GLChildView::eGVT_NONE )
  {
    const bool bCtrl(IsKeyStateDown(VK_CONTROL));

    if( !(m_bMiddleMouseDown&&!bCtrl) && ( m_eUpdateRegPosn & eUpdateSupine2d || m_uInvalidChildViews &  GLChildView::eGVT_SLICE ) )
      GLRenderView( eReg_SUPINE_SLICEVIEW );

    if( !(m_bMiddleMouseDown&&!bCtrl) && ( m_eUpdateRegPosn & eUpdateProne2d || m_uInvalidChildViews &  GLChildView::eGVT_SLICE ) )
      GLRenderView( eReg_PRONE_SLICEVIEW );

    if( m_eUpdateRegPosn & eUpdateSupine3d || m_uInvalidChildViews &  GLChildView::eGVT_NAVIGATION )
    {
      if( ( m_bMiddleMouseDown && m_bMiddleMouseSupine ) || !m_bMiddleMouseDown )
        GLRenderView( eReg_SUPINE_ENDOVIEW );
    }

    if( m_eUpdateRegPosn & eUpdateProne3d || m_uInvalidChildViews &  GLChildView::eGVT_NAVIGATION )
    {
      if( ( m_bMiddleMouseDown && !m_bMiddleMouseSupine ) || !m_bMiddleMouseDown )
        GLRenderView( eReg_PRONE_ENDOVIEW );
    }

    if ( (pOverview = GetDlgItem(IDC_TEMPLATE_OVERVIEW_SUPINE)) != NULL )
    {
      GLRenderView(GLChildViewRef(*pOverview));
    }
    if ( (pOverview = GetDlgItem(IDC_TEMPLATE_OVERVIEW_PRONE)) != NULL )
    {
      GLRenderView(GLChildViewRef(*pOverview));
    }

    m_eUpdateRegPosn = eUpdateNone;
    m_uInvalidChildViews = GLChildView::eGVT_NONE;
  }  // not flying in 2d or 3d but requires an update

} //  GLRenderViews()


/**
 * WM_MOUSEMOVE: Mouse movement handler.
 *
 * @param uFlags   Indicates whether various virtual keys are down. 
 * @param point    specifies the x- and y-coordinate of the cursor.
 */
void RegistrationLayout::OnMouseMove( UINT uFlags, CPoint point ) 
{
  ColonViatronixWnd::OnMouseMove(uFlags, point);

  GLChildViewRef childView = GetChildView( m_idMainView );
  const DatasetOrientations::TypeEnum eCurrentDSOrient( rdrGlobal.m_pCurrDataset->GetOrientation() );
  const DatasetOrientations::TypeEnum eChildDS(((ViatronChildView&)childView).GetDatasetRef().GetOrientation());

  m_bMiddleMouseDown = false;

  switch( childView.GetControlID() )
  {
    case IDC_TEMPLATE_SLICEVIEW_SUPINE:
    case IDC_TEMPLATE_SLICEVIEW_PRONE:
    {
      // Ensure that the main view will be the 2D view of the current view's dataset
      const uint4 uCurrSliceViewID
        = (eChildDS == DatasetOrientations::eSUPINE) ? IDC_TEMPLATE_SLICEVIEW_SUPINE : IDC_TEMPLATE_SLICEVIEW_PRONE;
      m_idMainView = GetChildViewDlgItem(uCurrSliceViewID).GetClassInstanceID();

      m_bMiddleMouseDown = ( ( uFlags & MK_MBUTTON  )) != 0 &&  !Utilities::Datasets::IsLocked() ;
      m_bMiddleMouseSupine = ( eChildDS == DatasetOrientations::eSUPINE );
      break;
    }

    case IDC_TEMPLATE_ENDOSCOPIC_SUPINE:
    case IDC_TEMPLATE_ENDOSCOPIC_PRONE:
    {
      // Ensure that the main view will be the 3D view of the current view's dataset
      const uint4 uCurrEndoViewID
        = (eChildDS == DatasetOrientations::eSUPINE) ? IDC_TEMPLATE_ENDOSCOPIC_SUPINE : IDC_TEMPLATE_ENDOSCOPIC_PRONE;
      m_idMainView = GetChildViewDlgItem(uCurrEndoViewID).GetClassInstanceID();

      if ( (ViatronStudy::m_modeEndoView.GetMutexModes() == ViewModes::BIOPSY) && (uFlags == MK_LBUTTON) )
      {
        Point<float> sourcePt = rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.m_magicWindowStartPt;
        const float4 fConvFactor(rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.GetRenderSize() /
          float4(rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.GetRenderSize()));
        sourcePt.m_x = sourcePt.m_x / fConvFactor;
        sourcePt.m_y = sourcePt.m_y / fConvFactor;
        rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.m_magicWindowStartPt = sourcePt;
        rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
      }
      break;
    }

    default:
    {
      // Ensure that the main view will always be the 3D view of the current dataset
      const uint4 uCurrEndoViewID
        = (eCurrentDSOrient == DatasetOrientations::eSUPINE) ? IDC_TEMPLATE_ENDOSCOPIC_SUPINE : IDC_TEMPLATE_ENDOSCOPIC_PRONE;
      m_idMainView = GetChildViewDlgItem(uCurrEndoViewID).GetClassInstanceID();
      break;
    }
  }

  ViatronStudy::SetCurrentView( GetChildView(m_idMainView).GetViewWndType(), rdrGlobal.m_pCurrDataset->GetOrientation() );
  
  const uint4 uViewModes( ViewModes::AUTOFLY | ViewModes::AUTOFLY2D );
  if( !rdrGlobal.m_supine.m_submarine.IsFlying() && !rdrGlobal.m_prone.m_submarine.IsFlying() && !(ViatronStudy::GetCurrentViewModes() & uViewModes ))
    GLIdle();
} // OnMouseMove( UINT uFlags, CPoint point ) 


/**
 * IDM_BOOKMARKS_ADDBOOKMARK: Menu handler, adds a new bookmark at current position in each dataset
 */
void RegistrationLayout::OnBookmarksAddBookmark()
{
  // add supine bkmk
  if (rdrGlobal.m_supine.IsLoadingComplete())
  {
    CString bknm;
    bknm.Format(IDS_BOOKMARK_NAME_FORMAT, rdrGlobal.m_supine.GetCurrentDistanceFromRectumMM() / 10.0f);
    rdrGlobal.m_supine.m_session.m_bookmarkList.AddBookmark(Bookmark().LoadCurrentPos(rdrGlobal.m_supine, LPCTSTR(bknm)));
  }
  // add prone bkmk
  if (rdrGlobal.m_prone.IsLoadingComplete())
  {
    CString bknm;
    bknm.Format(IDS_BOOKMARK_NAME_FORMAT, rdrGlobal.m_prone.GetCurrentDistanceFromRectumMM() / 10.0f);
    rdrGlobal.m_prone.m_session.m_bookmarkList.AddBookmark(Bookmark().LoadCurrentPos(rdrGlobal.m_prone, LPCTSTR(bknm)));
  }
  
  PlaySound(LPCTSTR(IDW_BOOKMARK_DROP), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);

  // Add temporary (quiet) notification that report snapshot has been added
  MainAppFrame::GetMainFrame()->PostMessage(WM_APP_DISPLAY_HELP_TEXT, IDS_BOOKMARK_ADDED, 1);

  /// Invalidate the views
  InvalidateViews( WPARAM(GLChildView::eGVT_OVERVIEW), 0);

}  // OnBookmarksAddBookmark()

/**
 * WM_RBUTTONDOWN handler, for popup menus.
 *
 * @param uFlags   Current button-state flags.
 * @param point    Location of click.
 */
void RegistrationLayout::OnRButtonDown(UINT uFlags, CPoint point) 
{
  // TODO: Add your message handler code here and/or call default
  ColonViatronixWnd::OnRButtonDown(uFlags, point);

//  const int4 iMenuCmd(ShowPopupMenu(uFlags, point));
//  switch (iMenuCmd)      
//  {
//    // Orientation change
//    case ID_SLICEORIENTATION_AXIAL:    case ID_SLICEORIENTATION_SAGITTAL:
//    case ID_SLICEORIENTATION_CORONAL:  case ID_SLICEORIENTATION_CROSSSECTIONAL:    case ID_SLICEORIENTATION_VIEWDIRECTION:   
//    {
//      SkinButton * pAnchorBtn = dynamic_cast<SkinButton*> (GetDlgItem(IDC_TEMPLATE_LINKVIEWS));
//      if (pAnchorBtn && pAnchorBtn->GetCheck())  // Linked messages
//      {
//        GetChildViewByPos(eReg_SUPINE_SLICEVIEW).SendMessage(WM_COMMAND, iMenuCmd, 0);
//        GetChildViewByPos(eReg_PRONE_SLICEVIEW).SendMessage(WM_COMMAND, iMenuCmd, 0);
//        break;
//      }
//    }
//    default: 
//      break; 
//  } // end-switch (iMenuCmd).
} // nRButtonDown(UINT uFlags, CPoint point) 


/**
 * WM_LBUTTONDBLCLK handler.
 *
 * @param uFlags   Current button-state flags.
 * @param origPt   Point of click.
 */
void RegistrationLayout::OnLButtonDblClk(UINT uFlags, CPoint origPt)
{
  ColonViatronixWnd::OnLButtonDblClk(uFlags, origPt);

  if (typeid(GetCurrentView()) == typeid(EndoluminalView))
  {
    Panes::SlidingPaneManager * pPaneMgr = dynamic_cast<Panes::SlidingPaneManager*>(GetDlgItem(IDC_SLIDING_REGISTRATION_PANE));
    if ( (GetDlgItem(IDC_RSP_PANEHDR_DUALOVERVIEW) != NULL) && (!pPaneMgr->IsPaneVisible(IDC_RSP_PANEHDR_DUALOVERVIEW)) )
    {
      pPaneMgr->ShowPane(IDC_RSP_PANEHDR_SUPINECAD, true);
      pPaneMgr->ShowPane(IDC_RSP_PANEHDR_CONTROLS, true);
    }
  }
} // OnLButtonDblClk(UINT uFlags, CPoint origPt)


/**
 * Keyboard event handler: key pressed
 *
 * @param uChar     Key code of that pressed.
 * @param uRepCnt   Repeat count of keystroke.
 * @param uFlags    Various key state codes.
 */
void RegistrationLayout::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags )
{
  CPoint position( GetMessagePos() );
  ScreenToClient( &position );

  if( m_pPrimaryOverviewWnd != NULL && IsMouseOverPaneControl( position, *m_pPrimaryOverviewWnd ) )
  {
    m_pPrimaryOverviewWnd->OnKeyDown( uChar, uRepCnt, uFlags );
  }
  else if( m_pDualOverviewWnd != NULL && IsMouseOverPaneControl( position, *m_pDualOverviewWnd ) )
  {
    m_pDualOverviewWnd->OnKeyDown( uChar, uRepCnt, uFlags );
  }
  else
  {
    ColonViatronixWnd::OnKeyDown(uChar, uRepCnt, uFlags);
  }
} // OnKeyDown()


/**
 * Indicates if the mouse is over the specified pane control
 *
 * @param     wndPane   Pane control to check
 * @return    true if over pane control
 */
bool RegistrationLayout::IsMouseOverPaneControl( CPoint & point, CWnd & wndPane )
{

  bool bMouseOverControl( false );

  if( wndPane.IsWindowVisible() )
  {
    CRect rc;
    wndPane.GetWindowRect( &rc );
    ScreenToClient( &rc );    
    bMouseOverControl = rc.PtInRect( point );
  }

  return bMouseOverControl;

} // IsMouseOverPaneControl( CPoint & point, CWnd & wndPane )


/**
 * Set text in CAD pane headers
 */
void RegistrationLayout::SetDatasetPaneTitles()
{
  const bool bCAD( m_pPrimaryOverviewWnd == NULL );

  //{{ Set title in cad panes
  if ( GetDlgItem(IDC_RSP_PANEHDR_SUPINECAD) )
  {
    CString sTitle;  sTitle.LoadString(IDS_CHILDVIEW_TITLE_VTXFMT);
    sTitle.Replace("#<view>", LoadResourceString(bCAD ? IDS_CAD_RESULTS : IDS_OVERVIEW));
    sTitle.Replace("#<orientation>", ColonViatronixWnd::PatientOrientationToString(rdrGlobal.m_supine, true).c_str());
    Panes::SlidingPaneManager * pPaneView = dynamic_cast<Panes::SlidingPaneManager*>(GetDlgItem(IDC_SLIDING_REGISTRATION_PANE));
    pPaneView->SetPaneText(IDC_RSP_PANEHDR_SUPINECAD, LPCTSTR(sTitle));
  }

  if ( GetDlgItem(IDC_RSP_PANEHDR_PRONECAD) )
  {
    CString sTitle;  sTitle.LoadString(IDS_CHILDVIEW_TITLE_VTXFMT);
    sTitle.Replace("#<view>", LoadResourceString(bCAD ? IDS_CAD_RESULTS : IDS_OVERVIEW));
    sTitle.Replace("#<orientation>", ColonViatronixWnd::PatientOrientationToString(rdrGlobal.m_prone, true).c_str());
    Panes::SlidingPaneManager * pPaneView = dynamic_cast<Panes::SlidingPaneManager*>(GetDlgItem(IDC_SLIDING_REGISTRATION_PANE));
    pPaneView->SetPaneText(IDC_RSP_PANEHDR_PRONECAD, LPCTSTR(sTitle));
  }
  //}} (end) Set title in cad panes

  return;

} // SetDatasetPaneTitles()


/**
 * Message: IDM_TOGGLE_AUTOFLY: Toggles Autofly mode.
 */
void RegistrationLayout::OnToggleAutofly()
{
  ColonViatronixWnd::OnToggleAutofly();

  SliceChildView& supineSlice = (SliceChildView&) GetChildViewByPos(eReg_SUPINE_SLICEVIEW);
  SliceChildView& proneSlice  = (SliceChildView&) GetChildViewByPos(eReg_PRONE_SLICEVIEW);

  supineSlice.SetAutoflyMode(false);
  proneSlice. SetAutoflyMode(false);

  if (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D)
  {
    if (rdrGlobal.m_pCurrDataset->GetOrientation() == DatasetOrientations::eSUPINE)
      supineSlice.SetAutoflyMode(true);
    else
      proneSlice.SetAutoflyMode(true);
  }
} // OnToggleAutofly()


/**
* Message: IDM_TOGGLE_VIEW_DIRECTION: Toggles view direction.
*/
void RegistrationLayout::OnToggleViewDirection()
{

  ColonViatronixWnd::OnToggleViewDirection();

	if (rdrGlobal.m_bDual3DEndoFly && Utilities::Datasets::IsLocked())
  {
    rdrGlobal.m_pSecondaryDataset->m_submarine.SetFlightDirection(rdrGlobal.m_pCurrDataset->m_submarine.GetFlightDirection());
  }

} // OnToggleViewDirection()


/**
 * Message: WM_APP_QUERY_ENABLE_SUPINE_PRONE_TOGGLE: Enables switching between Supine/Prone (by PatientInfoDlg)
 *
 * @param WPARAM   Unused
 * @param LPARAM   Unused
 * @return         TRUE to indicate toggling between Supine & Prone allowed
 */
LRESULT RegistrationLayout::OnQueryEnableSupineProneSwitch(WPARAM, LPARAM)
{
  return 1;
} // OnQueryEnableSupineProneSwitch(WPARAM, LPARAM)


/** Message: WM_APP_2DAUTOFLIGHT_UPDATE: 2D auto-flight: signal to set up new skeleton position.
 *
 * @param WPARAM   position on current skeleton. 
 * @param LPARAM   Unused
 * @return         TRUE to set up new position on current skeleton.
 */
LRESULT RegistrationLayout::OnUpdate2DAutoFlight(WPARAM wParam, LPARAM lParam)
{
  if (GetDlgItem(IDT_PANE_OVERVIEW_PRIMARY) != NULL)
    GetDlgItem(IDT_PANE_OVERVIEW_PRIMARY)->SendMessage(WM_APP_2DAUTOFLIGHT_UPDATE, wParam, lParam);

  if (GetDlgItem(IDT_PANE_OVERVIEW_DUAL) != NULL)
    GetDlgItem(IDT_PANE_OVERVIEW_DUAL)->SendMessage(WM_APP_2DAUTOFLIGHT_UPDATE, wParam, lParam);

  ColonViatronixWnd::OnUpdate2DAutoFlight(wParam, lParam);

  return TRUE;

} // OnUpdate2DAutoFlight(WPARAM wParam, LPARAM lParam)



/**
 * Child view application message.
 *
 * @param wParam     Message-dependent parameter.
 * @param lParam     Message-dependent parameter.
 * @return LRESULT   Message-dependent.
 */
LRESULT RegistrationLayout::OnAppChildViewMessage(WPARAM wParam, LPARAM lParam)
{
  // Scroll messages must be passed to both windows when they are linked
  if (HIWORD(wParam) == WM_HSCROLL)   
  {
//    if (Utilities::Datasets::IsLocked())
    {
      // target of scroll msg. is either the first or second (dual ortho) view
      const bool bTargetLeft((LOWORD(wParam) == GetChildViewByPos(eReg_SUPINE_SLICEVIEW).GetClassInstanceID()));
      const int4 iScrollReturn(GetChildViewByPos(bTargetLeft ? eReg_SUPINE_SLICEVIEW : eReg_PRONE_SLICEVIEW).SendMessage(HIWORD(wParam), 0, lParam));

      return TRUE;
    }
  }

  return ColonViatronixWnd::OnAppChildViewMessage(wParam, lParam);
} // OnAppChildViewMessage(WPARAM wParam, LPARAM lParam)


/**
 * IDM_JUMP_PREVSEGMENT: Jumps to the next segment towards the cecum.
 */
void RegistrationLayout::OnJumpPrevSegment()
{

  ColonViatronixWnd::OnJumpPrevSegment();

  if (rdrGlobal.IsDualDataLoaded() && Utilities::Datasets::IsLocked()) // For Linked actions
  {
    rdrGlobal.UpdateWorldPosition(*rdrGlobal.m_pCurrDataset, *rdrGlobal.m_pSecondaryDataset);
    rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
    InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
  }

} // OnJumpPrevSegment()
  

/**
 * IDM_JUMP_NEXTSEGMENT: Jumps to the next segment towards the rectum.
 */
void RegistrationLayout::OnJumpNextSegment()
{

  ColonViatronixWnd::OnJumpNextSegment();

  if (rdrGlobal.IsDualDataLoaded() && Utilities::Datasets::IsLocked()) // For Linked actions
  {
    rdrGlobal.UpdateWorldPosition(*rdrGlobal.m_pCurrDataset, *rdrGlobal.m_pSecondaryDataset);
    rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
    InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
  }

} // OnJumpNextSegment()


/**
 * ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE: Toggles Line Measure Mode.
 */
void RegistrationLayout::OnToggleLineMeasure()
{
  ToggleViewMode(ViewModes::LINEMEASURE);
} // OnToggleLineMeasure()


/**
 * ID_ENDOSCOPICVIEW_CURRENTMODE_LIGHTING: Toggles Lighting Mode.
 */
void RegistrationLayout::OnToggleLighting()
{
  ToggleViewMode(ViewModes::LIGHTING);
} // OnToggleLighting()


/**
 * ID_ENDOSCOPICVIEW_CURRENTMODE_ROIMEASURE: Toggles ROI Measure Mode.
 */
void RegistrationLayout::OnToggleRoiMeasure()
{
  ToggleViewMode(ViewModes::ROIMEASURE);
} // OnToggleRoiMeasure()


/**
 * ID_ENDOSCOPICVIEW_OPTIONS_TRANSLUCENT_DISPLAY: Toggles Translucent Mode.
 */
void RegistrationLayout::OnToggleTranslucent()
{
  if ( (rdrGlobal.m_supine.GetModality() == Dataset::MR) || (rdrGlobal.m_prone.GetModality() == Dataset::MR) )
    return;

  GLChildViewRef view = GetChildView( GLChildView::GLVID( OnGetMainViewWndID( 0, 0 ) ) );

  const ViewModesValue validModes(ViatronStudy::GetCurrentViewModes().GetValid());

  if ( typeid( view ) == typeid( SliceChildView ) && ( validModes & ViewModes::PSEUDOCOLOR_VALID) == ViewModes::PSEUDOCOLOR_VALID )
  {
    SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEWWINDOW_PSEUDOCOLORDISPLAY,0), 0);
  }
  else if ( (validModes & ViewModes::BIOPSY_VALID) == ViewModes::BIOPSY_VALID ) 
  {
    rdrGlobal.m_bBiopsyMode = ViatronStudy::m_modeEndoView.Toggle(ViewModes::BIOPSY);
    ViatronStudy::m_modeSegments3D.Toggle(ViewModes::BIOPSY);
    InvalidateViews( WPARAM(GLChildView::eGVT_NAVIGATION), 0);
  }

} // OnToggleTranslucent()


/**
 * IDM_REPORT_ADDSNAPSHOT: Adds a new report entry.
 */
void RegistrationLayout::OnReportAddSnapshot()
{
  std::list<GLChildViewPtr> snapshotList;
  std::list<GLChildViewPtr> displayList;

  // Use localized draw overview image, prevents stretching of non-square overview image
  DrawOverView ovDrawImage( this, *rdrGlobal.m_pCurrDataset );
  snapshotList.push_back(&ovDrawImage);

  if (rdrGlobal.m_supine.IsLoadingComplete())
  {
    snapshotList.push_back(&GetChildViewByPos(eReg_SUPINE_SLICEVIEW));
    snapshotList.push_back(&GetChildViewByPos(eReg_SUPINE_ENDOVIEW));
    displayList.push_back(&GetChildViewByPos(eReg_SUPINE_SLICEVIEW));
    displayList.push_back(&GetChildViewByPos(eReg_SUPINE_ENDOVIEW));
  }

  if (rdrGlobal.m_prone.IsLoadingComplete())
  {
    snapshotList.push_back(&GetChildViewByPos(eReg_PRONE_SLICEVIEW));
    snapshotList.push_back(&GetChildViewByPos(eReg_PRONE_ENDOVIEW));
    displayList.push_back(&GetChildViewByPos(eReg_PRONE_SLICEVIEW));
    displayList.push_back(&GetChildViewByPos(eReg_PRONE_ENDOVIEW));
  }

  AddReportSnapshot(snapshotList, displayList, rdrGlobal.m_pCurrDataset->m_session.m_measureLineList);

} // OnReportAddSnapshot()



/**
 * Handles the dataset locked changing 
 *
 * @param       args            DatasetsLockedArgs
 */
/// handles datasets locked changes
void RegistrationLayout::OnDatasetsLocked( const Colon::DatasetsLockedArgs & args )
{
  SetButtonIcon( *dynamic_cast< SkinButton * >( GetDlgItem( IDC_TEMPLATE_LINKVIEWS )), ( args.IsLocked() ) ? IDB_DATASET_LINK : IDB_DATASET_UNLINK );
  if( args.IsLocked() )
  {
		SetButtonIcon(*dynamic_cast< SkinButton * >(GetDlgItem(IDC_TEMPLATE_FLYBOTH)), rdrGlobal.m_bDual3DEndoFly ? IDB_DATASET_LINK_VERTICAL : IDB_DATASET_UNLINK_VERTICAL);

    rdrGlobal.UpdateWorldPosition( *rdrGlobal.m_pCurrDataset, *rdrGlobal.m_pSecondaryDataset );
    rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();

    // reset overviews
    CWnd * pSupineOverview = GetDlgItem( IDC_TEMPLATE_OVERVIEW_SUPINE );
    CWnd* pProneOverview = GetDlgItem( IDC_TEMPLATE_OVERVIEW_PRONE );
    if( m_pDualOverviewWnd != NULL )
    {
      CWnd * pDuelView = GetDlgItem( IDT_PANE_OVERVIEW_DUAL );

      pSupineOverview = pDuelView->GetDlgItem( IDC_TEMPLATE_OVERVIEW_SUPINE );
      pProneOverview = pDuelView->GetDlgItem( IDC_TEMPLATE_OVERVIEW_PRONE );
    }

    pSupineOverview->PostMessage( WM_COMMAND, ID_OVERVIEW_RESETDISPLAY, 0 );
    pProneOverview->PostMessage( WM_COMMAND, ID_OVERVIEW_RESETDISPLAY, 0 );

    if( !args.IsMainAppFrameChanging() )
    {
      InvalidateViews( WPARAM(GLChildView::eGVT_ALL ), 0 );
      GLRenderViews();
    }
  }
  else
    SetButtonIcon( *dynamic_cast< SkinButton * >( GetDlgItem( IDC_TEMPLATE_FLYBOTH )), IDB_DATASET_UNLINK_VERTICAL );

  // enable the button if the viewer's are showing the same orientations
  SliceChildView & rSliceView0 = (SliceChildView &) GetChildViewByPos( eReg_SUPINE_SLICEVIEW );
  SliceChildView & rSliceView1 = (SliceChildView &) GetChildViewByPos( eReg_PRONE_SLICEVIEW );
  GetDlgItem( IDC_TEMPLATE_LINKVIEWS )->EnableWindow( rSliceView0.m_sliceViewer.GetOrientation() == rSliceView1.m_sliceViewer.GetOrientation() );

  GetDlgItem( IDC_TEMPLATE_FLYBOTH )->EnableWindow( args.IsLocked() );
} // OnDatasetsLocked( args )


/**
 * Is datasets locking tracked by this view
 */
bool RegistrationLayout::IsDatasetsLockingEnabled() const
{
  if( m_bBypassDatasetLockingCheck )
    return true;

  CWnd * pWnd( GetDlgItem( IDC_TEMPLATE_LINKVIEWS ));
  if( pWnd )
    return pWnd->IsWindowEnabled();

  return true;
} // IsDatasetsLockingEnabled()
