// $Id: VerifyView.C,v 1.19.2.16 2010/11/11 20:56:50 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Jeff Meade  jmeade@viatronix.net

// includes
#include "stdafx.h"
#include "VerifyView.h"
#include "MainAppFrame.h"
#include "ReportViewWnd.h"
#include "Theme.h"
#include "MonitorDisplay.h"

#ifdef VOLUME_PRO
#include "VLIOverView.h"
#else
#include "OverView.h"
#endif

#include "SliceChildView.h"
#include "ViatronStudy.h"
#include "V1KMessages.h"

#include "stdstring.h"
#include "ApplicationSettings.h"
#include "AppSettings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FILE_REVISION "$Revision: 1.19.2.16 $"

using namespace ColonViews::Layouts;
using namespace WinUtils;
using namespace ReaderLib;

//////////////////////////////////////////////////////////////////////
// VerifyLayoutFrame

IMPLEMENT_DYNCREATE(VerifyLayoutFrame, CMDIChildWnd)

/**
 * Constructor
 */
VerifyLayoutFrame::VerifyLayoutFrame() : ChildLayoutFrame(new VerifyLayout)
{
} // VerifyLayoutFrame()

//////////////////////////////////////////////////////////////////////
// VerifySliceLayoutFrame

IMPLEMENT_DYNCREATE(VerifySliceLayoutFrame, CMDIChildWnd)

/**
 * Constructor
 */
VerifySliceLayoutFrame::VerifySliceLayoutFrame() : ChildLayoutFrame(new VerifySliceLayout)
{
} // VerifySliceLayoutFrame()

//////////////////////////////////////////////////////////////////////
// VerifySliceWnd

BEGIN_MESSAGE_MAP(VerifySliceLayout, SupineProneLayout)
  //{{AFX_MSG_MAP(VerifySliceLayout)
  //}}AFX_MSG_MAP
  ON_COMMAND(IDM_TOGGLE_AUTOFLY, OnToggleAutofly)
  ON_COMMAND(IDM_TOGGLE_AUTOFLY, OnToggleAddAnnotation)
  ON_COMMAND(IDM_JUMP_PREVSEGMENT, OnJumpPrevSegment)
  ON_COMMAND(IDM_JUMP_NEXTSEGMENT, OnJumpNextSegment)
  ON_COMMAND(IDM_NEXT_MISSED_REGION, OnNextMissedRegion)
  ON_COMMAND(IDM_BOOKMARKS_ADDBOOKMARK, OnBookmarksAddBookmark)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_LIGHTING, OnToggleLighting)
  ON_COMMAND(IDM_TOGGLE_VIEW_DIRECTION, OnToggleViewDirection)
  ON_COMMAND(IDM_SHOW_NEXT_VIEW, OnShowNextView)
  ON_COMMAND(IDM_REPORT_ADDSNAPSHOT, OnReportAddSnapshot)
  ON_COMMAND(ID_REPORT_MARKCOMPLETE, OnToggleReportCompleteMark)
END_MESSAGE_MAP()

/**
 * Constructor
 */
VerifySliceLayout::VerifySliceLayout() : SupineProneLayout( SupineProneLayout::eSPL_ORTHOGONAL, false )
{
} // VerifySliceLayout()


/**
 * Creates the child views for this view window, perform other window init
 *
 * @param hWndThis   Handle to this window
 */
void VerifySliceLayout::CreateChildViews(HWND hWndThis)
{
  SupineProneLayout::CreateChildViews(hWndThis);
  CWnd * pWnd = GetDlgItem(IDC_TEMPLATE_CONTROLS);
  if ( pWnd != NULL ) pWnd->ShowWindow(SW_HIDE);
} // CreateChildViews()


/**
 * Message: IDM_TOGGLE_AUTOFLY: Toggles Autofly mode.
 */
void VerifySliceLayout::OnToggleAutofly()
{
  // do nothing, no 2D autoflight
} // OnToggleAutofly()


/**
 * Message: IDM_: Toggles Add Annotation mode.
 */
void VerifySliceLayout::OnToggleAddAnnotation()
{
  // do nothing, no annotation editing
} // OnToggleAddAnnotation()


/**
 * Jump to the previous segment
 */
void VerifySliceLayout::OnJumpPrevSegment()
{
  // do nothing, no segments verified
} // OnJumpPrevSegment()


/**
 * Jump to the next segment
 */
void VerifySliceLayout::OnJumpNextSegment()
{
  // do nothing, no segments verified
} // OnJumpNextSegment()


/**
 * Jump to the next missed region
 */
void VerifySliceLayout::OnNextMissedRegion()
{
  // do nothing, have to verify before flying before finding missed regions before jumping to next
} // OnNextMissedRegion()


/**
 * Add a new bookmark
 */
void VerifySliceLayout::OnBookmarksAddBookmark()
{
  // do nothing, no flight yet, still verifying
} // OnBookmarksAddBookmark()


/**
 * Toggle the lighting mode
 */
void VerifySliceLayout::OnToggleLighting()
{
  // do nothing, no endoscopic view
} // OnToggleLighting()


/**
 * Toggle the view direction
 */
void VerifySliceLayout::OnToggleViewDirection()
{
  // do nothing, no endoscopic view
} // OnToggleViewDirection()


/**
 * Unused.  Was to mimic functionality of Windows <alt>-<tab>
 */
void VerifySliceLayout::OnShowNextView()
{
  // do nothing
} // OnShowNextView()


/**
 * Add a snapshot to the report
 */
void VerifySliceLayout::OnReportAddSnapshot()
{
  // do nothing
} // OnReportAddSnapshot()


/**
 * Toggle report completed
 */
void VerifySliceLayout::OnToggleReportCompleteMark()
{
  // do nothing
} // OnToggleReportCompleteMark()


/**
 * Updates 2D flight along centerline menu option.
 *
 * @param pCmdUI   Update command handler.
 */
void VerifySliceLayout::OnUpdateViewWindowFlightAlongCenterline(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( false );
} // OnUpdateViewWindowFlightAlongCenterline()


/**
 * Updates switch flight direction menu option.
 *
 * @param pCmdUI   Update command handler.
 */
void VerifySliceLayout::OnUpdateViewWindowSwitchFlightDirection(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( false );
} // OnUpdateViewWindowSwitchFlightDirection()


/**
 * Updates arrow in 2D slice center menu item.
 *
 * @param pCmdUI   Update command handler.
 */
void VerifySliceLayout::OnUpdateViewWindowArrowInSliceCenter(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( false );
} // OnUpdateViewWindowArrowInSliceCenter()


/**
 * Updates navigation view update during 2D flight option menu item.
 *
 * @param pCmdUI   Update command handler.
 */
void VerifySliceLayout::OnUpdateViewWindowUpdateEndoluminalView(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( false );
} // OnUpdateViewWindowUpdateEndoluminalView()


/**
 * Override this function to filter window messages before they are dispatched to the Windows functions .
 *
 * @param pMsg   message to translate
 * @return       Nonzero if the message was fully processed and should not be processed further
 */
BOOL VerifySliceLayout::PreTranslateMessage( MSG * pMsg )
{
  if( pMsg->message == WM_LBUTTONDBLCLK )
  {      
    return pMsg->hwnd != GetDlgItem(IDC_TEMPLATE_OVERVIEW)->GetSafeHwnd();
  }
  return SupineProneLayout::PreTranslateMessage( pMsg );
} // PreTranslateMessage()


//////////////////////////////////////////////////////////////////////
// VerifyLayout

enum { NUM_VERIFY_VIEWS = 2 };
bool VerifyLayout::m_bSegmentOrderChanged(false);

BEGIN_MESSAGE_MAP(VerifyLayout, ColonViatronixWnd)
  //{{AFX_MSG_MAP(VerifyLayout)
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * Constructor
 */
VerifyLayout::VerifyLayout() : ColonViatronixWnd( false ),
  m_bFinishOnLoadCompletion(false),
  m_bInitializingVisualization(false), m_bRegistrationRunFirstTime(false),m_bUpdateUI(false)
{
} // VerifyLayout()


/**
 * Creates the child views for this view window, perform other window init
 *
 * @param hWndThis   Handle to this window
 */
void VerifyLayout::CreateChildViews(HWND hWndThis)
{
  ColonViatronixWnd::CreateChildViews(hWndThis);

  SkinButton& nextBtn = *((SkinButton*)GetDlgItem(IDC_VERIFY_NEXT));
  SetButtonIcon(nextBtn, IDB_BUTTON_LARGE);
  nextBtn.EnableWindow(FALSE);
  nextBtn.SetWindowText("");

  SetButtonIcon((SkinButton&) *GetDlgItem(IDC_VERIFY_SUPINEBACK), IDB_BUTTON_LARGE);
  SetButtonIcon((SkinButton&) *GetDlgItem(IDC_VERIFY_PRONEBACK), IDB_BUTTON_LARGE);

  OverView & rOverView0 = dynamic_cast<OverView &>(GetChildViewByPos(0));
  OverView & rOverView1 = dynamic_cast<OverView &>(GetChildViewByPos(1));

  rOverView0.Initialize(&rOverView1, &(rOverView0.GetDatasetRef().m_trackballVerify), OverviewRender::eSegmentVerify);
  rOverView1.Initialize(&rOverView0, &(rOverView1.GetDatasetRef().m_trackballVerify), OverviewRender::eSegmentVerify);

  dynamic_cast< SkinButton * >( GetDlgItem( IDC_VERIFY_SUPINEBACK) )->SetBackBrush( Theme::DefaultBackgroundBrush );
  dynamic_cast< SkinButton * >( GetDlgItem( IDC_VERIFY_PRONEBACK ) )->SetBackBrush( Theme::DefaultBackgroundBrush );
  dynamic_cast< SkinButton * >( GetDlgItem( IDC_VERIFY_NEXT) )->SetBackBrush( Theme::DefaultBackgroundBrush );
  dynamic_cast< SkinButton * >( GetDlgItem( IDC_VERIFY_FINISH ) )->SetBackBrush( Theme::DefaultBackgroundBrush );

  SetSPVerifyUIState();

} // CreateChildViews()


/**
 * Formats a string with up to two string ID arguments
 *
 * @param iFormatID   Format string resource ID
 * @param iArg1       Argument 1 string resource ID
 * @param iArg2       Argument 2 string resource ID
 * @return            Formatted string
 */
CString LoadFormatString(const uint4 &iFormatID, const uint4 &iArg1 = 0, const uint4 &iArg2 = 0)
{
  CString sArg1Str;      sArg1Str.LoadString(iArg1);   sArg1Str.MakeLower();
  CString sArg2Str;      sArg2Str.LoadString(iArg2);   sArg2Str.MakeLower();

  CString sLoadStr;
  sLoadStr.Format(iFormatID, sArg1Str, sArg2Str);
  return sLoadStr;
} // LoadFormatString()


/**
 * Idle time processing
 */
void VerifyLayout::GLIdle()
{
  ColonViatronixWnd::GLIdle();
  if( m_bUpdateUI )
  {
    SetSPVerifyUIState();
    m_bUpdateUI = false;
  }
} // GLIdle()


/**
 * Translate window messages before they are dispatched 
 *
 * @param pMsg   message to process
 * @return       Nonzero if the message was translated and should not be dispatched;0 otherwise.
 */
BOOL VerifyLayout::PreTranslateMessage( MSG * pMsg )
{
  if( pMsg->message == WM_LBUTTONDBLCLK || pMsg->message == WM_RBUTTONDOWN )
    m_bUpdateUI = true;

  return ColonViatronixWnd::PreTranslateMessage( pMsg ); 
} // PreTranslateMessage()


/**
 * Message: WM_APP_SET_MAIN_VIEW_WINDOW: Called when sizing window to resize child views
 *
 * @param uType   Identifies size event
 * @param iCX     Width of window
 * @param iCY     Height of view window
 */
void VerifyLayout::OnSize( UINT uType, int iCX, int iCY ) 
{
  if ( iCX <= 0|| iCY <= 0 )
  {
    return;
  }

  ColonViatronixWnd::OnSize(uType, iCX, iCY);

  // returning to resource-based layout resizing and away from hard-coded madness
  return;

  //
  // custom resizing code
  //

  // header fixed height at top
  const int4 iHeaderHeight = WinUtils::GetClientRect(GetDlgItem( IDC_TEMPLATE_PATIENTINFO )).Height();
  GetDlgItem( IDC_TEMPLATE_PATIENTINFO )->SetWindowPos( NULL, 0, 0, iCX, iHeaderHeight, SWP_NOZORDER );

  // determine the image size, which determines the gaps and everything else
  const int4 iTitleWidth = 250;
  const int4 iTextHeight = 25;
  const int4 iGap = LAYOUT_GAP;
  const int4 iButtonHeight = 32;
  const int4 iExtraHorizontlGap = 3;
  const int4 iImageSize = min( ( iCX - 3 * ( iGap + iExtraHorizontlGap ) ) / 2, iCY - iHeaderHeight - 8 * iGap - 4 * iTextHeight - 2 * iButtonHeight );
  const int4 iGapH = ( iCX - 2 * iImageSize ) / 3;
  const int4 iGapV = ( iCY - iHeaderHeight - 4 * iTextHeight - iImageSize - 2 * iButtonHeight ) / 8;

  // layout the title
  const int4 iTitleX = iCX / 2 - iTitleWidth / 2;
  const int4 iTitleY = iHeaderHeight + iGapV;
  GetDlgItem( IDC_VERIFY_TITLE )->SetWindowPos( NULL, iTitleX, iTitleY, iTitleWidth, iTextHeight, SWP_NOZORDER );

  // determine the image size
  const int4 iTopLabelY = iTitleY + iTextHeight + iGapV;
  const int4 iImageY = iTopLabelY + iTextHeight + iGapV;
  const int4 iSupineX = iCX / 2 - iImageSize - iGapH / 2;
  const int4 iProneX = iCX / 2 + iGapH / 2;
  GetDlgItem( IDC_TEMPLATE_OVERVIEW_SUPINE )->SetWindowPos( NULL, iSupineX, iImageY, iImageSize, iImageSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_OVERVIEW_PRONE )->SetWindowPos( NULL, iProneX, iImageY, iImageSize, iImageSize, SWP_NOZORDER );
  
  // layout the labels above and below the images
  const int4 iBottomLabelY = iImageY + iImageSize + iGapV;
  GetDlgItem( IDC_VERIFY_SUPINELABEL )->SetWindowPos( NULL, iSupineX, iTopLabelY, iImageSize, iTextHeight, SWP_NOZORDER );
  GetDlgItem( IDC_VERIFY_PRONELABEL )->SetWindowPos( NULL, iProneX, iTopLabelY, iImageSize, iTextHeight, SWP_NOZORDER );
  GetDlgItem( IDC_VERIFY_SUPINEPROMPT )->SetWindowPos( NULL, iSupineX, iBottomLabelY, iImageSize, iTextHeight, SWP_NOZORDER );
  GetDlgItem( IDC_VERIFY_PRONEPROMPT )->SetWindowPos( NULL, iProneX, iBottomLabelY, iImageSize, iTextHeight, SWP_NOZORDER );

  // layout update buttons below images
  const int4 iUpdateWidth = 220;
  const int4 iSupineUpdateX = iSupineX + iImageSize / 2 - iUpdateWidth / 2;
  const int4 iProneUpdateX = iProneX + iImageSize / 2 - iUpdateWidth / 2;
  const int4 iUpdateY = iBottomLabelY + iTextHeight + iGapV;
  GetDlgItem( IDC_VERIFY_SUPINEBACK )->SetWindowPos( NULL, iSupineUpdateX, iUpdateY, iUpdateWidth, iButtonHeight, SWP_NOZORDER );
  GetDlgItem( IDC_VERIFY_PRONEBACK )->SetWindowPos( NULL, iProneUpdateX, iUpdateY, iUpdateWidth, iButtonHeight, SWP_NOZORDER );

  // layout the finish prompt
  const int4 iPromptWidth = 425;
  const int4 iPromptX = iCX / 2 - iPromptWidth / 2;
  const int4 iPromptY = iUpdateY + iButtonHeight + iGapV;
  GetDlgItem( IDC_VERIFY_FINISHPROMPT )->SetWindowPos( NULL, iPromptX, iPromptY, iPromptWidth, iTextHeight, SWP_NOZORDER );

  // layout the registration and finish buttons
  const int4 iRegistrationWidth = 200;
  const int4 iFinishWidth = 125;
  const int4 iRegistrationX = iCX / 2 - iGapH / 2 - iRegistrationWidth;
  const int4 iRegistrationY = iPromptY + iTextHeight + iGapV;
  const int4 iFinishX = iCX / 2 + iGapH / 2;
  GetDlgItem( IDC_VERIFY_NEXT )->SetWindowPos( NULL, iRegistrationX, iRegistrationY, iRegistrationWidth, iButtonHeight, SWP_NOZORDER );
  GetDlgItem( IDC_VERIFY_FINISH )->SetWindowPos( NULL, iFinishX, iRegistrationY, iFinishWidth, iButtonHeight, SWP_NOZORDER );

  SetLabelText();

} // OnSize()


/**
 * Centralize setting of all buttons and prompts, rather than having button enables/disables and text prompts all over the place
 */
void VerifyLayout::SetSPVerifyUIState()
{
  // sanity check 
  if( !IsWindow( this->GetChildViewByPos(0).GetSafeHwnd() ) || !IsWindow( this->GetChildViewByPos(1).GetSafeHwnd() ) )
    return;

  class DatasetVerifyState
  {
  public:
    /// constructor
    DatasetVerifyState(const Dataset& dset, CWnd& parentWnd, GLChildViewRef view, const bool bLeft) : m_dset(dset), m_bLeft(bLeft),
      m_segList(dset.m_segmentList), m_bSegList(dset.m_segmentList.IsLoaded()), m_overview(dynamic_cast<OverView&>(view)),
      m_prompt(*parentWnd.GetDlgItem(m_bLeft?IDC_VERIFY_SUPINEPROMPT:IDC_VERIFY_PRONEPROMPT)),
      m_eRenderMode(m_overview.GetRenderMode()),
      m_back(*parentWnd.GetDlgItem(m_bLeft?IDC_VERIFY_SUPINEBACK:IDC_VERIFY_PRONEBACK))
    {
    } // constructor

    /// ensure we have a valid dataset
    bool AtLeastOneSegment() const { return (!m_bSegList || (m_bSegList && (m_segList.GetNumSelectedSegments() > 0))); }
  public:
    /// dataset
    const Dataset& m_dset;
    /// overview segment list
    const SegmentList& m_segList;
    /// existence of segment list (whether dataset is valid)
    const bool m_bSegList;
    /// left or right dataset
    const bool m_bLeft;
    /// overview window
    OverView& m_overview;
    /// render mode
    const OverviewRender::RenderModeEnum m_eRenderMode;
    /// user prompt textbox
    CWnd& m_prompt;
    /// back button
    CWnd& m_back;
  }; // class DatasetVerifyState

  DatasetVerifyState supineState(rdrGlobal.m_supine, *this, this->GetChildViewByPos(0), true);
  DatasetVerifyState proneState (rdrGlobal.m_prone,  *this, this->GetChildViewByPos(1), false);

  CWnd & finishTextBox = *GetDlgItem(IDC_VERIFY_FINISHPROMPT);
  CWnd & nextBtn = *GetDlgItem(IDC_VERIFY_NEXT);
  CWnd & finishBtn = *GetDlgItem(IDC_VERIFY_FINISH);

  if ((!supineState.m_bSegList && !proneState.m_bSegList) || m_bInitializingVisualization)
  {
    static const CString sInitViz(LoadResourceString(IDS_STUDYLOAD_PAINTING) + TEXT("..."));
    supineState.m_prompt.SetWindowText(
      (m_bInitializingVisualization && rdrGlobal.m_supine.m_bNeedVisualizationReinit) ? sInitViz : CString());
    proneState.m_prompt.SetWindowText(
      (m_bInitializingVisualization && rdrGlobal.m_prone.m_bNeedVisualizationReinit) ? sInitViz : CString());
    finishTextBox.SetWindowText(sInitViz);

    supineState.m_back.EnableWindow(FALSE);
    proneState.m_back.EnableWindow(FALSE);
    finishBtn.EnableWindow(FALSE);
    return;
  }

  const bool bSupineVerify(supineState.m_bSegList && (supineState.m_eRenderMode == OverviewRender::eSegmentVerify));
  const bool bProneVerify (proneState.m_bSegList  &&  (proneState.m_eRenderMode == OverviewRender::eSegmentVerify));

  // Enable finish button if (1) Finish isn't pending (upon completion of load) and
  //                         (2) There's at least one segment selected for each of the valid datasets
  finishBtn.EnableWindow(!m_bFinishOnLoadCompletion && supineState.AtLeastOneSegment() && proneState.AtLeastOneSegment());

  static const CString sPickFirst(LoadFormatString(IDS_PICK_SEGMENT_FORMAT, IDS_FIRST));
  static const CString sPickNext(LoadFormatString(IDS_PICK_SEGMENT_FORMAT, IDS_NEXT));

  static const CString sLoadCompleting(LoadResourceString(IDS_STUDYLOAD_COMPLETING));
  static const CString sPickFinish(LoadResourceString(IDS_PICK_FINISH));

  static const CString sUpdate(LoadResourceString(IDS_UPDATE));
  static const CString sBack(CString(TEXT("<< ")) + LoadResourceString(IDS_BACK));

  DatasetVerifyState* vpStates[2] = {NULL,NULL};
  vpStates[0] = supineState.m_bSegList ? &supineState : &proneState;
  vpStates[1] = proneState.m_bSegList  ? &proneState  : &supineState;
  for (int4 i(0), iCount((vpStates[0]==vpStates[1])?1:2); i < iCount; i++)
  {
    DatasetVerifyState& dvs = *(vpStates[i]);
    CString sText;

    if (!dvs.m_bSegList)
    {
      dvs.m_back.SetWindowText(TEXT(""));
      dvs.m_back.EnableWindow(FALSE);
    }
    else if ((dvs.m_eRenderMode == OverviewRender::eSegmentVerify) || m_bFinishOnLoadCompletion)
    {
      CString stemp; stemp.Format(IDS_PICK_VERIFY_FORMAT, ColonViatronixWnd::PatientOrientationToString(dvs.m_dset.GetPatientOrientation()).c_str() );
      sText += stemp;

      dvs.m_back.SetWindowText(sUpdate);
      dvs.m_back.EnableWindow(dvs.AtLeastOneSegment());
    }
    else if (dvs.m_eRenderMode == OverviewRender::eRegistrationVerify)
    {
      sText += LoadResourceString(IDS_ADD_USER_REGISTRATION);

      dvs.m_back.SetWindowText(sUpdate);
      dvs.m_back.EnableWindow(FALSE);
    }
    else
    {
      const bool bFirstSegmt(dvs.m_segList.GetNumSelectedSegments() <= 0);
      sText += (bFirstSegmt ? sPickFirst : sPickNext);

      dvs.m_back.SetWindowText(sBack);
      dvs.m_back.EnableWindow(!bFirstSegmt ? TRUE : FALSE);
    }

    dvs.m_prompt.SetWindowText(sText);
  }

  finishTextBox.SetWindowText(m_bFinishOnLoadCompletion ? sLoadCompleting :
    (finishBtn.IsWindowEnabled() ? sPickFinish : CString()));

  if (supineState.m_eRenderMode == OverviewRender::eRegistrationVerify)
  {
    nextBtn.SetWindowText(CString(TEXT("<< ")) + LoadResourceString(IDS_VERIFICATION));
  }
  else
  {
    nextBtn.SetWindowText(LoadResourceString(IDS_REGISTRATION) + TEXT(" >>"));
  }
  nextBtn.EnableWindow((rdrGlobal.m_supine.IsDistanceValid() &&
                        rdrGlobal.m_prone.IsDistanceValid()  &&
                        supineState.AtLeastOneSegment()      &&
                        proneState.AtLeastOneSegment()         ) ? TRUE : FALSE);
  nextBtn.Invalidate();  /// force redraw
} // SetSPVerifyUIState()


/**
 * Sets the label text properly
 */
void VerifyLayout::SetLabelText()
{
  // set the left label properly
  GetDlgItem( IDC_VERIFY_SUPINELABEL )->SetWindowText( PatientOrientationToString( rdrGlobal.m_supine ).c_str() );

  // set the right label properly
  GetDlgItem( IDC_VERIFY_PRONELABEL )->SetWindowText( PatientOrientationToString( rdrGlobal.m_prone ).c_str() );

  if ( rdrGlobal.IsStudyLoading() & DatasetOrientations::eSUPINE || rdrGlobal.m_supine.IsLoadingComplete() )
    GetDlgItem( IDC_VERIFY_LEFTINFO )->SetWindowText( rdrGlobal.m_supine.m_studyinfo.Format().c_str() );
  else
    GetDlgItem( IDC_VERIFY_LEFTINFO )->SetWindowText( "" );

  if ( rdrGlobal.IsStudyLoading() & DatasetOrientations::ePRONE || rdrGlobal.m_prone.IsLoadingComplete() )
    GetDlgItem( IDC_VERIFY_RIGHTINFO )->SetWindowText( rdrGlobal.m_prone.m_studyinfo.Format().c_str() );
  else
    GetDlgItem( IDC_VERIFY_RIGHTINFO )->SetWindowText( "" );

} // SetLabelText()


/**
 * Notifies view window of the current progress of study load
 *
 * @param eCompletedStep   The previously completed step
 * @param eCurrentStep     The next (current) study loading step
 * @param eOrientation     Orientation on which the next step is performing
 */
void VerifyLayout::OnStudyLoadStepNotify(const StudyLoad::StepsEnum eCompletedStep,
  const StudyLoad::StepsEnum eCurrentStep, const DatasetOrientations::TypeEnum eOrientation)
{
  switch (eCompletedStep)
  {
    case StudyLoad::eSTART:
    {
      break;
    }

    case StudyLoad::eINITIALIZE:
    case StudyLoad::eHEADER:
    {
      ColonViews::Dialogs::PatientInfoDlg * pDlg = dynamic_cast<ColonViews::Dialogs::PatientInfoDlg*> (GetDlgItem(IDC_TEMPLATE_PATIENTINFO));
      if( pDlg != NULL )
        pDlg->SendMessage(ColonViews::Dialogs::PatientInfoDlg::WM_USER_INITIALIZE_STUDY, 0, 0);
      SetLabelText();

      break;
    }

    case StudyLoad::eSEGMENTS:
    {
      break;
    }

    case StudyLoad::eLOAD_COMPLETE:
    {
      // The following update is necessary for initializing the joint skeleton.
			// If it is not initialized, any function associated to locate camera will crash the app.
			if( rdrGlobal.m_supine.IsLoadingComplete() && rdrGlobal.m_supine.m_segmentList.GetNumSelectedSegments() > 0 )
				rdrGlobal.m_supine.UpdateSegmentOrder();
			if( rdrGlobal.m_prone.IsLoadingComplete() && rdrGlobal.m_prone.m_segmentList.GetNumSelectedSegments() > 0 )
				rdrGlobal.m_prone.UpdateSegmentOrder();

      if (m_bFinishOnLoadCompletion)
      { 
        SendMessage(WM_COMMAND, IDC_VERIFY_FINISH, 0);
      }
      SetLabelText();

	
      break;
    }
		case StudyLoad::eCSI:
		{
			int4 i = 2;
			break;
		}
    default:
      break;
  }

  dynamic_cast<OverView*>(GetDlgItem( IDC_TEMPLATE_OVERVIEW_SUPINE ))->ForceRedraw();
  dynamic_cast<OverView*>(GetDlgItem( IDC_TEMPLATE_OVERVIEW_PRONE ))->ForceRedraw();

  SetSPVerifyUIState();

  //Invalidate(NULL);
} // OnStudyLoadStepNotify()


/**
 * Background thread function, to do study initialization in a separate thread
 *  so that the user can still interact with the interface and the status can still be updated.
 *
 * @param pParam   HWND, handle to the main application window
 * @return         Always true
 */
UINT VerifyLayout::StudyInitializeVisualizationProc(LPVOID pParam)
{
  LogDbg("StudyInitializeVisualizationProc entered...........","VerifyLayout","StudyInitializeVisualization");

  CoInitialize(NULL);
  StudyLoad::LoadCallback loadCallback(MainAppFrame::StudyLoadNotify, HWND(pParam));

  // TODO:  The changes to check the success of verification and printing the message below
  //         were done in haste.  Please tidy up this code a bit.  Thanks :)
  int1 bVerifyFailed(0);

  Timer initVizTimer; initVizTimer.Reset();

  try
  {
    // initialize visualization of studies
    loadCallback.NextStep(StudyLoad::eINITIALIZE, 90, DatasetOrientations::eSUPINE);
    if (rdrGlobal.m_supine.IsLoadingComplete() && rdrGlobal.m_supine.m_bNeedVisualizationReinit)
    {
      bVerifyFailed += (rdrGlobal.m_supine.InitializeVisualization() ? 0 : 1);
    }
  }
  catch (...)
  {
    LogErr("Exception caught while initializing supine dataset.","VerifyLayout","StudyInitializeVisualization");
  }

  try
  {
    loadCallback.NextStep(StudyLoad::eINITIALIZE, 95, DatasetOrientations::ePRONE);
    if (rdrGlobal.m_prone.IsLoadingComplete() && rdrGlobal.m_prone.m_bNeedVisualizationReinit)
    {
      bVerifyFailed += (rdrGlobal.m_prone.InitializeVisualization() ? 0 : 2);
    }
  }
  catch (...)
  {
    LogErr("Exception caught while initializing prone dataset.","VerifyLayout","StudyInitializeVisualization");
  }

  std::strstream ssInitViz;
  ssInitViz << "Initializing Viz took " << initVizTimer.AsString().c_str() << std::ends;
  LogFyi(ssInitViz.str(),"VerifyLayout","StudyInitializeVisualizationProc");
  ssInitViz.freeze(false);

  if (bVerifyFailed != 0)
  {
    CString sStudy; sStudy.LoadString((bVerifyFailed == 3) ? IDS_BOTH : ((bVerifyFailed == 1) ? IDS_SUPINE : IDS_PRONE));
    CString sMsg;   sMsg.Format(IDS_PICK_LENGTH_WARNING_FORMAT, LPCTSTR(sStudy));
    AfxMessageBox(sMsg);
  }

  loadCallback.NextStep(StudyLoad::eINITIALIZE_COMPLETE, 0, DatasetOrientations::eDEFAULT);

  try
  {
    MainAppFrame* pFrame = dynamic_cast<MainAppFrame*>(AfxGetApp()->m_pMainWnd);
    ASSERT(pFrame);

    // open report
    VxVCReport::OpenStudyReport(rdrGlobal.m_pCurrDataset->m_volumeHeader,
      pFrame->GetReportPrefix(), rdrGlobal.m_bReadOnly,
      ReportLayout::HasPermissions(), rdrGlobal.m_bReportCreated);

		// bug fix attempt
		if (rdrGlobal.m_bReportCreated)
		{
      VxVCReport::OpenStudyReport(rdrGlobal.m_pCurrDataset->m_volumeHeader,
        pFrame->GetReportPrefix(), rdrGlobal.m_bReadOnly,
        ReportLayout::HasPermissions(), rdrGlobal.m_bReportCreated);
		} // if
  }
  catch (...)
  {
    LogErr("Exception caught while opening report.","VerifyLayout","StudyInitializeVisualization");
  }

  // switch to navigation view
  ::PostMessage(HWND(pParam), WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NAVIGATION, 0);

  // Set dataset orientation appropriately
  ::PostMessage(HWND(pParam), WM_APP_SET_MAIN_DATASET, WPARAM(rdrGlobal.m_supine.IsLoadingComplete()
    ? DatasetOrientations::eSUPINE : DatasetOrientations::ePRONE), 0);

  CoUninitialize();

  return true;
} // StudyInitializeVisualizationProc


/**
* WM_MOUSEMOVE handler
*
* @param uFlags   Current key flags
* @param point    Current point of mouse move
*/
void VerifyLayout::OnMouseMove( UINT uFlags, CPoint point )
{
  ColonViatronixWnd::OnMouseMove(uFlags, point);
  GLIdle();
} // OnMouseMove()


/**
 * WM_COMMAND handler; here, handles BN_CLICKED event for each of the window buttons
 *
 * @param wParam   Indicates the button pressed
 * @param lParam   Here, unused
 * @return         TRUE/FALSE indicating whether command was processed
 */
BOOL VerifyLayout::OnCommand(WPARAM wParam, LPARAM lParam)
{
  switch (wParam)
  {
    case IDC_VERIFY_SUPINEBACK:
    case IDC_VERIFY_PRONEBACK:
    {
      OnVerifyBack(wParam == IDC_VERIFY_SUPINEBACK);
      break;
    }

    case IDC_VERIFY_NEXT:
    {
      OnVerifyNext();
      break;
    }

    case IDC_VERIFY_FINISH:
    {
      OnVerifyFinish();
      break;
    }
    default:
      break;
  }
  SetSPVerifyUIState();
  return ColonViatronixWnd::OnCommand(wParam, lParam);
} // OnCommand()


/**
 * Handles 'back' button
 *
 * @param bSupine   Whether or not the supine back button was pressed (false for prone)
 */
void VerifyLayout::OnVerifyBack(const bool bSupine)
{
  OverView& overView = dynamic_cast<OverView&> (GetChildViewByPos(bSupine?0:1));
  Dataset&  dataset  = (bSupine ? rdrGlobal.m_supine : rdrGlobal.m_prone);

  if (overView.GetRenderMode() == OverviewRender::eSegmentVerify)
  {
    // Note that the segment order has been changed during this session
    m_bSegmentOrderChanged = true;

    // Set to segment pick mode
    overView.SetRenderMode(OverviewRender::eSegmentPicking);

    // This is a good place to set this, a better place may be in SetSPVerifyUIState() function
    // Set flags to initialize visualization and initialize painting surfaces
    dataset.m_bNeedVisualizationReinit = true;
    dataset.m_fields.m_paintVol.m_bSurfacesNeedReinit = true;

    // mark all segments as not selected
    dataset.m_segmentList.UnselectAllSegments();

    // A new list of segments invalidates any current user features (which may or may not be in newly selected segments)
    rdrGlobal.m_supine.m_session.m_vUserFeatures.clear();
    rdrGlobal.m_prone .m_session.m_vUserFeatures.clear();
  }
  else if (overView.GetRenderMode() != OverviewRender::eRegistrationVerify)
  {
    if (dataset.m_segmentList.GetNumSelectedSegments() > 0)
    {
      dataset.m_segmentList.UnselectTailSegment();
    }
  }

  m_bFinishOnLoadCompletion = false;
  dataset.m_fields.SetOpacityVolumeCreated(false);
  rdrGlobal.m_bReportCreated = true;

  overView.ForceRedraw();
} // OnVerifyBack()


/**
 * Handles 'next' button
 */
void VerifyLayout::OnVerifyNext()
{
  OverView& ovS = dynamic_cast<OverView&>(GetChildViewByPos(0));
  OverView& ovP = dynamic_cast<OverView&>(GetChildViewByPos(1));

  OverviewRender::RenderModeEnum eNewMode((ovS.GetRenderMode() == OverviewRender::eRegistrationVerify) ? OverviewRender::eSegmentVerify : OverviewRender::eRegistrationVerify);

  ovS.SetRenderMode(eNewMode);
  ovP.SetRenderMode(eNewMode);

  SetDlgItemText(IDC_VERIFY_TITLE, LoadResourceString(eNewMode == OverviewRender::eRegistrationVerify ? IDS_VERIFYTITLE_REGISTRATION: IDS_VERIFYTITLE_SEGMENTS));

  if(eNewMode == OverviewRender::eRegistrationVerify)
  {
    rdrGlobal.m_supine.UpdateSegmentOrder();
    rdrGlobal.m_prone.UpdateSegmentOrder();
    rdrGlobal.m_prone.CreateFlippedSkeleton();
    rdrGlobal.BuildMatchQuality();
    rdrGlobal.Register();
    m_bRegistrationRunFirstTime = true;
  }
  m_bFinishOnLoadCompletion = false;

  ovS.ForceRedraw();
  ovP.ForceRedraw();
} // OnVerifyNext()


/**
 * Handles 'finish' button
 */
void VerifyLayout::OnVerifyFinish()
{
  LogDbg("OnVerifyFinish begin.", "VerifyLayout", "OnVerifyFinish")
  try
  {
  OverView& ovS = dynamic_cast<OverView&>(GetChildViewByPos(0));
  OverView& ovP = dynamic_cast<OverView&>(GetChildViewByPos(1));

  ovS.SetRenderMode(OverviewRender::eSegmentVerify);
  ovP.SetRenderMode(OverviewRender::eSegmentVerify);

  // test if we need to lock NavigationBar to ensure the initial report file can be been created successfully.
  if ( !ReportLayout::ReportFileExists() )
  {
    rdrGlobal.m_bReportCreated = false;
  }

  if (rdrGlobal.IsLoadingComplete())
  {
    // Either (re)initialize the study...
    if (rdrGlobal.m_supine.m_bNeedVisualizationReinit || rdrGlobal.m_prone.m_bNeedVisualizationReinit)
    {
      m_bInitializingVisualization = true;

      if (!m_bRegistrationRunFirstTime)
      {
        if (rdrGlobal.m_supine.IsLoadingComplete())
        {
          rdrGlobal.m_supine.UpdateSegmentOrder();
        }
        if (rdrGlobal.m_prone.IsLoadingComplete())
        {
          rdrGlobal.m_prone.UpdateSegmentOrder();
          rdrGlobal.m_prone.CreateFlippedSkeleton();
        }
        if (rdrGlobal.m_prone.IsLoadingComplete()&&rdrGlobal.m_supine.IsLoadingComplete())
        {
          rdrGlobal.BuildMatchQuality();
          LogDbg("Register.", "VerifyLayout", "OnVerifyFinish")
          rdrGlobal.Register();
          LogDbg("Register done.", "VerifyLayout", "OnVerifyFinish")
          m_bRegistrationRunFirstTime = true;
        }
      }

      // Background thread to init study.  If thread fails to run, attempt to run procedure directly.
      const bool bThreadCreated(BackgroundProcess::Run(StudyInitializeVisualizationProc,
        (void *) (AfxGetMainWnd()->GetSafeHwnd()), "StudyInitializeVisualizationProc", eBKHIDE_MENU, THREAD_PRIORITY_HIGHEST));

      if (!bThreadCreated)
      {
        CWaitCursor waitCursor;
        StudyInitializeVisualizationProc((void *) (AfxGetMainWnd()->GetSafeHwnd()));
      }
    }
    // Or just jump back to Navigation view
    else
    {
      // Set study orientation appropriately
      AfxGetMainWnd()->SendMessage(WM_APP_SET_MAIN_DATASET, WPARAM(rdrGlobal.m_supine.IsLoadingComplete()
        ? DatasetOrientations::eSUPINE : DatasetOrientations::ePRONE), 0);

      // Just switch back to the navigation view
      AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NAVIGATION, 0);
    }
  }
  else
  {
    m_bFinishOnLoadCompletion = true;
  }

  ovS.ForceRedraw();
  ovP.ForceRedraw();
  }
  catch (...)
  {
    LogErr("An exception occurred.", "VerifyLayout", "OnVerifyFinish")
  }
} // OnVerifyFinish()


/**
 * Gets the resource ID for the popup menu for this window
 *
 * @param childView      Child view whose popup menu will be displayed.
 * @param uPopupMenuID   ID Popup menu requested to be displayed.
 * @return               Resource ID for the popup menu, or 0 for no menu
 */
uint4 VerifyLayout::OnShowPopupMenu(const GLChildView& childView, const uint4 uPopupMenuID) const
{
  if (typeid(childView) == typeid(OverView))
  {
    const OverviewRender::RenderModeEnum eMode(dynamic_cast<OverView&>(GetCurrentView()).GetRenderMode());
    if (eMode == OverviewRender::eRegistrationVerify)
    {
      return uPopupMenuID;
    }
  }

  return 0;
} // OnShowPopupMenu()


/**
 * WM_DESTROY message handler
 */
void VerifyLayout::OnDestroy()
{
  { // In case either segment list has no segments (for whatever reason)
    SegmentList *pSlist = &rdrGlobal.m_supine.m_segmentList;
    SegmentList *pPlist = &rdrGlobal.m_prone.m_segmentList;

    if (pSlist->IsLoaded() && (pSlist->GetNumSelectedSegments() <= 0))
    {
      pSlist->SelectAllSegments();
    }

    if (pPlist->IsLoaded() && (pPlist->GetNumSelectedSegments() <= 0))
    {
      pPlist->SelectAllSegments();
    }
  }

  ColonViatronixWnd::OnDestroy();
} // OnDestroy()


/**
 * Updates 2D flight along centerline menu option.
 *
 * @param pCmdUI   Update command handler.
 */
void VerifyLayout::OnUpdateViewWindowFlightAlongCenterline(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( false );
} // OnUpdateViewWindowFlightAlongCenterline()


/**
 * Updates switch flight direction menu option.
 *
 * @param pCmdUI   Update command handler.
 */
void VerifyLayout::OnUpdateViewWindowSwitchFlightDirection(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable( false );
} // OnUpdateViewWindowSwitchFlightDirection()


/**
 * Updates arrow in 2D slice center menu item.
 *
 * @param pCmdUI   Update command handler.
 */
void VerifyLayout::OnUpdateViewWindowArrowInSliceCenter(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( false );
} // OnUpdateViewWindowArrowInSliceCenter()


/**
 * Updates navigation view update during 2D flight option menu item.
 *
 * @param pCmdUI   Update command handler.
 */
void VerifyLayout::OnUpdateViewWindowUpdateEndoluminalView(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( false );
} // OnUpdateViewWindowUpdateEndoluminalView()


/**
 * menu command update handler for scrollbar orientation change
 *
 * @param pCmdUI   Update command handler.
 */
void VerifyLayout::OnUpdateToggleScrollbarOrientation(CCmdUI* pCmdUI)
{
  pCmdUI->Enable( false );
} // OnUpdateToggleScrollbarOrientation()


#undef FILE_REVISION


// $Log: VerifyView.C,v $
// Revision 1.19.2.16  2010/11/11 20:56:50  dongqing
// Add the btn.Invalidate() for the text button to do the correct refresh.
// That was caused by removing Invalidate for Win 7 fix
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.15  2010/10/29 13:07:27  mkontak
// New progress bar to fix the flashing that occurs in the NavigationView. Commented
// out the Invalidate of the entire Frame on the Verify view to fix the flashing
// during the volume loading.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.13  2010/02/25 22:43:48  dongqing
// removed overview controls from verify orthogonal layout
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.12  2010/02/01 19:24:50  dongqing
// study info into the dataset class
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.11  2010/01/18 09:49:16  dongqing
// vertical scrollbars and blank view handling
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.10  2010/01/07 18:48:26  dongqing
// identify studies associated with both datasets (for cases where datasets from different studies are loaded)
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.9  2009/12/09 20:25:22  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.19.2.8  2009/11/25 22:09:07  dongqing
// widescreen
//
// Revision 1.19.2.7  2009/10/07 16:13:16  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.19.2.6  2009/06/17 21:08:25  dongqing
// The report entry distance does not need to be updated after
// the segments being re-selected by design.
//
// Revision 1.19.2.5  2008/02/16 03:01:41  jmeade
// Issue 6016: force redraw after every study load update.
//
// Revision 1.19.2.4  2008/02/07 19:30:55  jmeade
// removed hard-coded dialog sizes.
//
// Revision 1.19.2.3  2007/08/22 17:09:39  jmeade
// Issue 5771: re-read report information because newly created reports are not storing the info.
// not a bug "fix" per say, more like a bug correction.
//
// Revision 1.19.2.2  2007/03/19 22:35:19  jmeade
// Issue 5456: ensure update state methods always reference valid datasets.
//
// Revision 1.19.2.1  2007/03/16 17:44:07  jmeade
// Issue 5456: ensure update state methods always reference valid datasets.
//
// Revision 1.19  2007/03/09 22:04:05  jmeade
// code standards.
//
// Revision 1.18  2007/03/01 01:18:57  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.17  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.16  2006/10/04 20:58:29  frank
// Issue #5041: Made automatic dataset linking a configuration setting
//
// Revision 1.15  2006/09/28 21:19:19  jmeade
// Issue 4946: display of decubitus patient positions.
//
// Revision 1.14  2006/07/24 19:46:57  jmeade
// Issue 4657: Update patient info upon volume header info read.
//
// Revision 1.13  2006/06/30 20:34:31  jmeade
// Read patient position from volume header.
//
// Revision 1.12  2006/06/28 18:26:13  jmeade
// removed unused code.
//
// Revision 1.11  2006/06/27 17:11:33  jmeade
// Read patient position from volume header.
//
// Revision 1.10  2006/06/08 18:21:28  frank
// working on showing decubitus left/right, etc
//
// Revision 1.9  2006/04/12 23:11:15  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.8  2006/02/07 23:33:29  jmeade
// code review: comments.
//
// Revision 1.7  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.6  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.5.2.4  2006/05/22 12:29:49  geconomos
// added some sanity checks to suppres assertions during DEBUG builds
//
// Revision 1.5.2.3  2006/03/08 00:17:33  jmeade
// Issue 4620: Shortcut key for annotation mode.
//
// Revision 1.5.2.2  2006/01/10 18:54:19  geconomos
// first crack at reducing flicker while resizing
//
// Revision 1.5.2.1  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.5  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.4  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.3  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.62.2.6.2.10.2.2  2005/07/13 17:31:13  geconomos
// Issue #4272: Flying not permitted.
//
// Revision 3.62.2.6.2.10.2.1  2005/07/08 18:59:51  jmeade
// privatize dataset modality variable.
//
// Revision 3.62.2.6.2.10  2005/06/14 22:13:59  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 3.62.2.6.2.9  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.62.2.6.2.8.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 3.62.2.6.2.8.4.1  2005/06/14 17:38:45  jmeade
// debug messages.
//
// Revision 3.62.2.6.2.8  2005/04/21 19:17:22  jmeade
// Issue 3851: Add debugging info for process exceptions; corrected exception.
//
// Revision 3.62.2.6.2.7  2005/04/08 18:23:40  frank
// Issue #4113: Disabled menu items in verification view
//
// Revision 3.62.2.6.2.6  2005/04/08 16:02:10  frank
// Issue #4113: permitted toggling supine/prone in verify view
//
// Revision 3.62.2.6.2.5  2005/03/31 15:38:06  frank
// Issue #4077: Fixed update of verify view when button is pressed.
//
// Revision 3.62.2.6.2.4  2005/03/30 20:54:13  jmeade
// drawing framework changes.
//
// Revision 3.62.2.6.2.3  2005/03/30 20:37:10  frank
// Issue #3997: Disabled function keys in verify slice view
//
// Revision 3.62.2.6.2.2  2005/03/29 17:33:50  jmeade
// Issue 3997: Return link button on verify slice view.
//
// Revision 3.62.2.6.2.1  2005/03/29 15:00:48  jmeade
// Issue 3997: No 2D flight in slice verification.
//
// Revision 3.62.2.6  2005/02/15 19:58:46  jmeade
// merge from vc1.4 branch.
//
// Revision 3.62.2.5.2.1  2004/11/16 22:19:29  jmeade
// Pre/post-contrast labels for MR studies.
//
// Revision 3.62.2.5  2004/07/21 20:26:26  jmeade
// Issue 3630: Keep track of when the segment orderings have been changed.
//
// Revision 3.62.2.4  2004/07/17 03:03:58  jmeade
// Update colon finding distances when colon segments are changed; Separated functions for individual wm_command messages.
//
// Revision 3.62.2.3  2003/04/07 20:47:49  jmeade
// Issue 2872: Reset user features when list of segments are updated.
//
// Revision 3.62.2.2  2003/02/22 12:20:28  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.62.2.1  2003/02/12 23:22:46  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 3.63  2003/02/05 22:18:01  kevin
// Fix bug where user could get into registration too soon
//
// Revision 3.62  2003/01/27 21:48:28  jmeade
// Missed one button resize.
//
// Revision 3.61  2003/01/21 19:37:09  jmeade
// Duplicate: Revision : 3.15.2.5: Resized buttons for Italian version.
//
// Revision 3.60  2002/12/24 03:45:21  jmeade
// Remove debug code (registering on 'r' key).
//
// Revision 3.59  2002/12/12 22:09:08  jmeade
// Unused or unneeded view modes code cleanup.
//
// Revision 3.58  2002/11/27 22:32:52  jmeade
// Reverse of paradigm for popup menus:  assume show popup menu on right-click in childview,
//  have view window instances validate/refuse popup as necessary; text in Update btn on reg'n mode.
//
// Revision 3.57  2002/11/26 00:38:26  kevin
// Changed surface initialization logic
//
// Revision 3.56  2002/11/22 00:31:34  jmeade
// OverView::Initialize convenience function.
//
// Revision 3.55  2002/11/21 22:56:12  jmeade
// Overview render mode cleanup.
//
// Revision 3.54  2002/11/15 00:41:19  jmeade
// No hard-coding of # of child views; Hide child view storage (use access methods).
//
// Revision 3.53  2002/11/13 21:57:43  jmeade
// Compiler warning (mis-used const on ref).
//
// Revision 3.52  2002/11/11 16:16:50  kevin
// Still trying to get the bug out
//
// Revision 3.51  2002/11/08 01:34:06  kevin
// Cleaned up a couple of small issues in S/P registration
//
// Revision 3.50  2002/10/31 19:54:26  kevin
// Changed user feature points to make them more robust and sorted
// so that they cannot become invalid (e.g. crossing user features)
//
// Revision 3.49  2002/10/21 18:52:08  kevin
// Kevin's refactored S/P Registration with new heirarchical algorithm
//
// Revision 3.48  2002/09/29 21:44:15  kevin
// Re-Factor of SUpine/Prone registration ot place data structures
// in more meaningful places and remove so much copying of
// large data strucutres (there was a lot of creating local copies of
// refernces passed into toomany classes).
//
// Suzi and Ana's original registration is still presetn. In fact the registration
// still runs off their data structures. I jsut added new ones. From here on, I will
// have to break theres. So I wanted a version I could always go back
// to in case I broke anything.
//
// Revision 3.47  2002/09/13 00:52:39  jmeade
// ColonSession class for appropriate data; Reset paint volume data as appropriate.
//
// Revision 3.46  2002/09/11 19:05:47  kevin
// Updates to when and how registration is run during the users traversal
// of the verification screen
//
// Revision 3.45  2002/09/10 20:51:50  jmeade
// Oops:  Each OverviewRender instance needs its own render mode variable.
//
// Revision 3.44  2002/09/10 19:10:55  jmeade
// Setting text for verify view registration mode.
//
// Revision 3.43  2002/09/10 15:49:24  jmeade
// Use one function to handle changing main dataset.
//
// Revision 3.42  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.40  2002/07/26 22:49:37  jmeade
// Fixes for Visual Studio class wizard database.
//
// Revision 3.39  2002/07/25 15:32:28  ana
// Cleanup comments.
//
// Revision 3.38  2002/07/24 18:02:57  jmeade
// Re-run verification (with only registration if applicable) when user points change.
//
// Revision 3.37  2002/07/23 16:57:42  jmeade
// Removed effective-less set log level statement (only can set once per app);
// Fixing log debug statements.
//
// Revision 3.36  2002/07/22 16:51:33  jmeade
// Code standards.
//
// Revision 3.35  2002/07/18 20:27:04  jmeade
// Moved report functionality to ConsoleLib.
//
// Revision 3.34  2002/07/18 16:14:48  ana
// Printing to the logger the seeds Jeff send to us.
//
// Revision 3.33  2002/07/15 21:02:38  suzi
// Registration refinements.
//
// Revision 3.32  2002/07/15 20:16:58  ana
// Compile error fixes because of data members changes.
//
// Revision 3.31  2002/07/15 18:13:07  suzi
// no message
//
// Revision 3.30  2002/07/15 15:56:11  jmeade
// More registration implementation: popup menu, specify segment/node index for user-defined points.
//
// Revision 3.29  2002/07/11 23:25:19  jmeade
// Implementing user-defined registration point specification.
//
// Revision 3.28  2002/07/09 20:14:15  jmeade
// Code standards.
//
// Revision 3.27  2002/07/03 03:23:43  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.26  2002/06/24 21:46:10  ana
// Multiple seeds being launched.
//
// Revision 3.25  2002/06/21 00:03:47  suzi
// Broke something, so set back to incremental registration via Associator.
//
// Revision 3.24  2002/06/19 21:14:05  ana
// Computing how long does registration takes to be computed.
//
// Revision 3.23  2002/06/19 19:50:08  ana
// Bugs fixes in the new registration.
//
// Revision 3.22  2002/06/12 21:09:36  suzi
// Removed offending code till registration is all working again.
//
// Revision 3.21  2002/06/12 20:12:57  suzi
// Huge refactoring and implementation of new registration with multiple seeds.
//
// Revision 3.20  2002/06/06 20:48:56  suzi
// Fixed the call to m_registration.Init()
//
// Revision 3.19  2002/06/06 19:14:54  suzi
// Changed the signature of the Register methods.
//
// Revision 3.18  2002/04/12 02:16:25  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.17  2002/03/28 18:51:36  jmeade
// More strings to string table.
//
// Revision 3.16  2002/03/13 22:01:16  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.15.2.4  2002/06/17 17:55:34  jmeade
// Memory cleanup of std::strstream after call to str() function.
//
// Revision 3.15.2.3  2002/02/25 15:59:57  kevin
// Instrumented the time it takes to perofrm Init Vizualization so we get
// debug logs in the field in case loading starts to slow down.
//
// Revision 3.15.2.2  2002/02/07 22:20:27  jmeade
// Background processes get their own class and file.
//
// Revision 3.15.2.1  2002/02/05 19:56:38  jmeade
// Correcting finish text at various stages; Code standards.
//
// Revision 3.15  2002/01/31 00:07:23  jmeade
// Clarifying ambiguities in finish instructions on verify view.
//
// Revision 3.14  2002/01/08 17:14:37  kevin
// Only create registration data if not in Fianl Release for this version
//
// Revision 3.13  2001/12/29 00:11:31  jmeade
// Merge from 1.1 branch.
//
// Revision 3.12  2001/12/12 17:50:38  binli
// artifact of control btns in VerifyView: 1> minus width & Height by 2. 2> avoid duplicate setting.
//
// Revision 3.11  2001/11/23 15:13:04  jmeade
// Set icons for buttons.
//
// Revision 3.10  2001/11/20 09:52:06  jmeade
// Removed a duplicate indirection.
//
// Revision 3.9  2001/11/19 13:18:08  kevin
// Fixed some of the problems with S/P orientation when segments are flipped
//
// Revision 3.8  2001/11/17 18:59:09  jmeade
// Verification view from form.
//
// Revision 3.7  2001/11/17 03:41:34  jmeade
// Patient view stored in base class.
//
// Revision 3.6  2001/11/14 01:34:04  kevin
// Cleaned up alot of Supine/Proneregistration stuff, fixed some
// of the multi-rendering and added feature match pair rendering
//
// Revision 3.5  2001/11/09 04:55:01  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.4  2001/11/05 17:50:05  ana
// Registration view added.
//
// Revision 3.3  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.2.2.5  2001/11/15 19:50:45  jmeade
// Display an error when the chosen segmentation produces a "short" colon.
//
// Revision 3.2.2.4  2001/11/14 18:04:46  jmeade
// Issue 1348:  Run verification in the main thread if unable to launch background thread.
//
// Revision 3.2.2.3  2001/11/06 17:16:33  jmeade
// Cleaned up V1KMessages.h enum, comments.
//
// Revision 3.2.2.2  2001/11/04 05:14:48  jmeade
// Code standards.
//
// Revision 3.2.2.1  2001/10/28 11:11:27  jmeade
// Coding standards; Safer references instead of pointers
//
// Revision 3.2  2001/10/19 20:37:20  jmeade
// Issue 1175:  An indirect fix, catching and logging any exception thrown
// during initialization.  This should prevent view from "getting stuck" at initialize
// stage, but (obviously) masks that an exception was thrown.
//
// Revision 3.1  2001/10/19 20:16:53  jmeade
// Coding standards:  References instead of pointers, load static resource strings only once
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
//
//    Rev 2.4   Oct 10 2001 11:57:32   binli
// issue 1027: transparent rendering of overview reflect selection
//
//    Rev 2.3   Oct 09 2001 12:20:30   jmeade
// Ensure that the dataset needs initialization before
// Resolution for 1116: Both datasets are re-initialized when the segment list of either is updated
//
//    Rev 2.2   Oct 08 2001 17:03:48   jmeade
// Consolidated code specifying study orientation; allow creator of NavigationView to specify study orientation
//
//    Rev 2.1   Oct 03 2001 11:08:16   binli
// report bug: ensure to create all report images.
// fixed: temporay lock of NavigationBar.
//
//    Rev 2.0   Sep 16 2001 23:43:02   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
//
//    Rev 1.0   Sep 16 2001 17:29:12   ingmar
// Initial revision.
// Revision 1.106  2001/08/22 02:22:55  jmeade
// Removed CAxialSliceView class
//
// Revision 1.105  2001/08/21 02:25:58  jmeade
// Enums for view window positions (no hardcoded values)
//
// Revision 1.104  2001/05/17 20:42:05  binli
// VerifyView vs. StandardVIew/S&PView: controled under different trackball.
//
// Revision 1.103  2001/05/15 17:49:38  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.102  2001/05/11 23:59:56  jmeade
// Coding conventions
//
// Revision 1.101  2001/05/11 17:37:03  binli
// reflected removement of duplicated OVModes in OverView class.
//
// Revision 1.100  2001/04/30 12:32:47  frank
// Reflected changes in Study class.
//
// Revision 1.99  2001/04/30 02:20:04  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.98  2001/04/13 02:35:22  jmeade
// headers and footers
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VerifyView.C,v 1.19.2.16 2010/11/11 20:56:50 dongqing Exp $
// $Id: VerifyView.C,v 1.19.2.16 2010/11/11 20:56:50 dongqing Exp $
