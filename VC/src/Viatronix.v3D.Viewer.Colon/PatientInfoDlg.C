// $Id: PatientInfoDlg.C,v 1.13.2.15 2010/04/11 11:56:14 dongqing Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author(s): Jeffrey Meade  jmeade@viatronix.net


// includes
#include "stdafx.h"
#include "PatientInfoDlg.h"
#include "Theme.h"
#include "GdiplusUtilities.h"
#include "ViatronViewWnd.h"
#include "MonitorDisplay.h"
#include "PersonName.h"
#include "VxVCReport.h"
#include <tchar.h>

// namespaces
using namespace ColonViews::Dialogs;
using namespace WinUtils;
using namespace ReaderLib;
using namespace ColonViews::Layouts;

#define FILE_REVISION "$Revision: 1.13.2.15 $"


/**
 * Constructor 
 * 
 * @param pParent   Parent window
 */
PatientInfoDlg::PatientInfoDlg( CWnd* pParent, const uint4 uDlgBkgdResourceID, const bool bDualDataDisp ) : 
  m_pDrawBuffer( NULL ), m_bDualDatasetDisplay( bDualDataDisp )
{
  //{{AFX_DATA_INIT(PatientInfoDlg)
  m_sPatientName = _T("");
  m_sPhysicianName = _T("");
  m_sPatientData = _T("");
  m_sPatientID = _T("");
  m_sExamTime = _T("");
  m_sExamSite = _T("");
	//}}AFX_DATA_INIT

  m_bContainedInPane = uDlgBkgdResourceID == IDB_PATIENTINFO_VERT_BKGD;
} // PatientInfoDlg()


/**
 * Destructor 
 */
PatientInfoDlg::~PatientInfoDlg()
{
  if( m_pDrawBuffer != NULL )
  {
    delete m_pDrawBuffer;
    m_pDrawBuffer = NULL;
  }
} // ~PatientInfoDlg()


/**
 * Initializes dialog control variables
 * 
 * @param pDX   Dialog data info
 */
void PatientInfoDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(PatientInfoDlg)
  DDX_Text(pDX, IDC_PID_PATIENTNAME, m_sPatientName);
  DDX_Text(pDX, IDC_PID_PHYSICIANNAME, m_sPhysicianName);
  DDX_Text(pDX, IDC_PID_PATIENTDATA, m_sPatientData);
  DDX_Text(pDX, IDC_PID_PATIENTID, m_sPatientID);
  DDX_Text(pDX, IDC_PID_EXAMTIME, m_sExamTime);
  DDX_Text(pDX, IDC_PID_EXAMSITE, m_sExamSite);
  DDX_Control(pDX, IDC_PID_PATIENTID, m_patientIDStatic);
  //}}AFX_DATA_MAP
} // DoDataExchange()


/**
 * message map
 **/
BEGIN_MESSAGE_MAP(PatientInfoDlg, CDialog)
  //{{AFX_MSG_MAP(PatientInfoDlg)
  ON_WM_PAINT()
  ON_WM_CTLCOLOR()
  ON_WM_ERASEBKGND()
  ON_WM_SIZE()
  ON_WM_SETCURSOR()
  ON_WM_TIMER()
  ON_MESSAGE(PatientInfoDlg::WM_USER_INITIALIZE_STUDY, OnMessageInitStudy)
  ON_COMMAND(IDC_PID_SUPINEPRONE_BTN, OnToggleSupineProneInfoDisplayButton)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PatientInfoDlg methods


/**
 * Handles message to initialize study
 * @param WPARAM   unused
 * @param LPARAM   unused
 * @return         0
 */
LRESULT PatientInfoDlg::OnMessageInitStudy(WPARAM, LPARAM)
{
  UpdateSupineProneButton();

  if ( m_bDualDatasetDisplay )
  {
    if ( rdrGlobal.m_supine.IsLoadingComplete() || (rdrGlobal.m_supine.GetOrientation() & rdrGlobal.IsStudyLoading()) )
    {
      rdrGlobal.m_supine.m_studyinfo = GetStudyInfo(rdrGlobal.m_supine);
      m_sPatientName =   (ColonViatronixWnd::PatientOrientationToString(rdrGlobal.m_supine) + ": " + rdrGlobal.m_supine.m_studyinfo.m_sPatientID).c_str();
      m_sPatientID = (rdrGlobal.m_supine.m_studyinfo.m_sExamTime).c_str();
    }

    if ( rdrGlobal.m_prone.IsLoadingComplete() || (rdrGlobal.m_prone.GetOrientation() & rdrGlobal.IsStudyLoading()) )
    {
      rdrGlobal.m_prone.m_studyinfo = GetStudyInfo(rdrGlobal.m_prone);
      m_sPhysicianName = (ColonViatronixWnd::PatientOrientationToString(rdrGlobal.m_prone ) + ": " + rdrGlobal.m_prone.m_studyinfo.m_sPatientID).c_str();
      m_sExamTime  =  (rdrGlobal.m_prone.m_studyinfo.m_sExamTime).c_str();
    }

    m_sPatientData.Empty();
    m_sExamSite.Empty();
  }
  else if ( rdrGlobal.m_pCurrDataset->IsLoadingComplete() || (rdrGlobal.m_pCurrDataset->GetOrientation() & rdrGlobal.IsStudyLoading()) )
  {
    ReaderLib::StudyInfo studyInf;
    if ( rdrGlobal.m_bDualDataDetected && IsWindow(m_btnToggleDisplay) && m_btnToggleDisplay.IsWindowEnabled() )
    {
      ReaderLib::Dataset & rDataset = (m_btnToggleDisplay.GetCheck()==FALSE) ? rdrGlobal.m_supine : rdrGlobal.m_prone;
      studyInf = rDataset.m_studyinfo = GetStudyInfo(rDataset);

      m_btnToggleDisplay.SetWindowText(LoadResourceStringWithUpperHead((m_btnToggleDisplay.GetCheck()==FALSE)?IDS_LEFT:IDS_RIGHT));
    }
    else
    {
      studyInf = rdrGlobal.m_pCurrDataset->m_studyinfo = GetStudyInfo(*rdrGlobal.m_pCurrDataset);
    }

    m_sPatientName = studyInf.m_sPatientName.c_str();
    m_sPhysicianName = studyInf.m_sPhysicianName.c_str();
    m_sPatientData = studyInf.m_sPatientData.c_str();
    m_sPatientID = studyInf.m_sPatientID.c_str();
    m_sExamTime = studyInf.m_sExamTime.c_str();
    m_sExamSite = studyInf.m_sExamSite.c_str();
  }

  SetWindowText(m_sPatientName);

  UpdateData(FALSE);
  RedrawBackground();

  return 0;
} // OnMessageInitStudy()


/**
 * when the toggle button is clicked
 */
void PatientInfoDlg::OnToggleSupineProneInfoDisplayButton()
{
  AfxGetMainWnd()->SendMessage(WM_APP_SET_MAIN_DATASET, WPARAM((m_btnToggleDisplay.GetCheck() == 0)
    ? DatasetOrientations::eSUPINE : DatasetOrientations::ePRONE), 0);

  OnMessageInitStudy(0,0);
} // OnToggleSupineProneInfoDisplayButton()


/**
 * Sets patient study information
 */
ReaderLib::StudyInfo PatientInfoDlg::GetStudyInfo(const ReaderLib::Dataset & rDataset)
{
  ReaderLib::StudyInfo info;

  // Patient name and ID (non-encrypted).
  if ((info.m_sPatientName = VxConsole::GetVolumeHeaderVariable(rDataset.m_volumeHeader, "patientName")).empty())
  {
    info.m_sPatientName = LPCTSTR(LoadResourceString(IDS_PATIENT));
  }

  // Referring physician's name
  if ((info.m_sPhysicianName = VxConsole::GetVolumeHeaderVariable(rDataset.m_volumeHeader, "physicianName")).empty())
  {
    info.m_sPhysicianName = LPCTSTR(LoadResourceString(IDS_PHYSICIANNAME));
  }
  else
  {
    info.m_sPhysicianName.insert(0, LPCTSTR(LoadResourceString(IDS_PHYSICIANPREFIX)));
  }
  // (end) Referring physician's name

  // Study date
  // Date stored as yyyymmdd; display it as yyyy-mm-dd
  if ((info.m_sExamTime = VxConsole::GetVolumeHeaderVariable(rDataset.m_volumeHeader, "studyDate")).empty())
  {
    info.m_sExamTime = LPCTSTR(LoadResourceString(IDS_SCANDATE));
    info.m_dateExam = COleDateTime();
  }
  else
  {
    info.m_dateExam = VxVCReport::StringToDate(info.m_sExamTime.c_str());        
    info.m_sExamTime.insert(6, "-"); info.m_sExamTime.insert(4, "-");
  }
  // (end) Study date
  
  // Examination site
  if ((info.m_sExamSite = VxConsole::GetVolumeHeaderVariable(rDataset.m_volumeHeader, "institutionName")).empty())
  {
    info.m_sExamSite = LPCTSTR(LoadResourceString(IDS_EXAMSITE));
  }
  // (end) Examination site

  // Patient ID
  info.m_sPatientID = VxConsole::GetVolumeHeaderVariable(rDataset.m_volumeHeader, "patientID");
  // (end) Patient ID

  // Patient data
  info.m_sPatientData = VxConsole::GetVolumeHeaderVariable(rDataset.m_volumeHeader, "patientSex");
  if (!info.m_sPatientData.empty()) info.m_sPatientData = toupper(info.m_sPatientData[0]);
  info.m_sPatientData += VxConsole::GetVolumeHeaderVariable(rDataset.m_volumeHeader, "patientAge");

  if (info.m_sPatientData.empty())
  {
    info.m_sPatientData = LPCTSTR(LoadResourceString(IDS_PATIENTINFO));
  }
  // (end) Patient data

  return info;
} // GetStudyInfo()


/**
 * Dialog init
 * 
 * @return       Success of init
 */
BOOL PatientInfoDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  if (GetDlgItem(IDC_PID_SUPINEPRONE_BTN) != NULL)
  {
    m_btnToggleDisplay.SubclassDlgItem(IDC_PID_SUPINEPRONE_BTN, this);

    // Quick fix for keeping supine/prone combo up-to-date
    m_updateSPTimer.SetTimer(this, 1000);

    UpdateSupineProneButton();
  }

  OnMessageInitStudy(0,0);

  return TRUE;
} // OnInitDialog()


/**
 * WM_PAINT message handler
 */
void PatientInfoDlg::OnPaint() 
{
  CPaintDC dc( this );

  // draw the buffer to the screen
  if( m_pDrawBuffer != NULL )
  {
    Gdiplus::Graphics gfx( dc.GetSafeHdc() );
    gfx.DrawImage( m_pDrawBuffer, 0, 0, m_pDrawBuffer->GetWidth(), m_pDrawBuffer->GetHeight() );
  } 

} // OnPaint() 


/**
 * WM_SIZE handler, Called when window is resized
 *
 * @param uType   Type of resizing (e.g. maximize, restore)
 * @param iCX     Width
 * @param iCY     Height
 */
void PatientInfoDlg::OnSize( UINT uType, int iCX, int iCY )
{
  CDialog::OnSize( uType, iCX, iCY );

  // free the previous background image
  if( m_pDrawBuffer != NULL )
  {
    delete m_pDrawBuffer;
    m_pDrawBuffer = NULL;
  }

  // create new drawing buffer based on new window size
  m_pDrawBuffer = new Gdiplus::Bitmap( iCX, iCY, PixelFormat32bppARGB );

  RedrawBackground();

  if ( GetDlgItem(IDC_CONTAINER_GROUPBOX) != NULL )
  {
    CenterChildControls(IDC_CONTAINER_GROUPBOX);
  }

/*
  int4 iRightMostPos = iCX - 10;

  CWnd * pWndBox = GetDlgItem(IDC_PID_GROUPBOX);
  if ( pWndBox )
  {
    CRect rctBox = WinUtils::GetChildRect(pWndBox, this);
    rctBox.right = iRightMostPos;
    pWndBox->SetWindowPos(NULL, 0,0, rctBox.Width(), rctBox.Height(), SWP_NOMOVE | SWP_NOZORDER);
  }

  uint4 iID[2] = { IDC_PID_EXAMTIME, IDC_PID_PHYSICIANNAME };
  for ( int4 i(0); i < sizeof(iID)/sizeof(iID[0]); i++)
  {
    CWnd * pWnd = GetDlgItem(iID[i]);
    if ( pWnd )
    {
      CRect rct = WinUtils::GetChildRect(pWnd, this);
      rct.MoveToX(iRightMostPos -= rct.Width()+5);
      pWnd->SetWindowPos(NULL, rct.left,rct.top, 0,0, SWP_NOSIZE | SWP_NOZORDER);
    }
  }
*/
} // OnSize( UINT uType, int cx, int cy )


/**
 * WM_ERASEBKGND handler, called when background needs erasing
 *
 * @param   pdc     Device context for window
 * @return          Non-zero to indicate NOT to erase, zero to let framework erase window
 */
BOOL PatientInfoDlg::OnEraseBkgnd( CDC * pdc )
{
  return TRUE;
} // OnEraseBkgnd( CDC * pdc )


/**
 * Redraws the dialog into the draw buffer
 */
void PatientInfoDlg::RedrawBackground()
{
  Gdiplus::Graphics gfx( m_pDrawBuffer );

  if( m_bContainedInPane )
  {
    Gdiplus::Bitmap bkgnd( AfxGetInstanceHandle(), MAKEINTRESOURCEW( IDB_PATIENTINFO_VERT_BKGD ) );
    gfx.DrawImage( &bkgnd, 0, 0, m_pDrawBuffer->GetWidth(), m_pDrawBuffer->GetHeight() );
    DrawStatic( gfx, IDC_PID_PHYSICIANNAME, Gdiplus::StringAlignmentNear );
    DrawStatic( gfx, IDC_PID_PATIENTDATA, Gdiplus::StringAlignmentNear );
    DrawStatic( gfx, IDC_PID_PATIENTID, Gdiplus::StringAlignmentNear );
    DrawStatic( gfx, IDC_PID_EXAMTIME,Gdiplus::StringAlignmentNear );
    DrawStatic( gfx, IDC_PID_EXAMSITE, Gdiplus::StringAlignmentNear );
  }
  else
  {

    if ( WinUtils::MonitorDisplay::IsWideScreen() ) // widescreen
    {
      Gdiplus::TextureBrush brush( Theme::BkgndSideTop );
      gfx.FillRectangle( &brush, 0, 0, m_pDrawBuffer->GetWidth(), Theme::BkgndHeaderMiddle->GetHeight() );
    }
    else
    {
      Gdiplus::TextureBrush brush( Theme::BkgndHeaderMiddle );
      gfx.FillRectangle( &brush, 0, 0, m_pDrawBuffer->GetWidth(), Theme::BkgndHeaderMiddle->GetHeight() );
      gfx.DrawImage( Theme::BkgndHeaderLeft, 0, 0 );
      gfx.DrawImage( Theme::BkgndHeaderRight, m_pDrawBuffer->GetWidth() - Theme::BkgndHeaderRight->GetWidth(), 0, Theme::BkgndHeaderRight->GetWidth(), Theme::BkgndHeaderRight->GetHeight() );
    }

    DrawGroupbox( gfx, IDC_PID_GROUPBOX );
    
    DrawStatic( gfx, IDC_PID_PATIENTNAME, Gdiplus::StringAlignmentNear );
    DrawStatic( gfx, IDC_PID_PHYSICIANNAME, Gdiplus::StringAlignmentFar );
    DrawStatic( gfx, IDC_PID_PATIENTDATA, Gdiplus::StringAlignmentNear );
    DrawStatic( gfx, IDC_PID_PATIENTID, Gdiplus::StringAlignmentNear );
    DrawStatic( gfx, IDC_PID_EXAMTIME,Gdiplus::StringAlignmentFar );
    DrawStatic( gfx, IDC_PID_EXAMSITE, Gdiplus::StringAlignmentCenter );
  }

  Invalidate();
} // RedrawBackground()


/**
 * Draws a group box into the draw buffer.
 *
 * @param   gfx   graphics object
 * @param   uId   id of group box to draw
 */
void PatientInfoDlg::DrawGroupbox( Gdiplus::Graphics & gfx, const uint4 uId )
{  
  CWnd * pWnd = GetDlgItem( uId );
  if( pWnd == NULL )
    return;
  if ( m_bDualDatasetDisplay )
    return;

  CRect rc; pWnd->GetWindowRect( &rc );
  ScreenToClient( &rc );
  rc.InflateRect( 0, -2 );
  Gdiplus::RectF rect( rc.left, rc.top, rc.Width(),  rc.Height() );
  GdiplusUtilities::DrawRoundedRectangle( gfx, *Theme::PatientInfoPen, rect, 5 );
} // DrawGroupbox()


/**
 * Draws a static text control into the draw buffer.
 *
 * @param   gfx          graphics object
 * @param   uId          id of static control to draw
 * @param   eAlignment   alignment of static text
 */
void PatientInfoDlg::DrawStatic( Gdiplus::Graphics & gfx, const uint4 uId, const Gdiplus::StringAlignment eAlignment )
{
  USES_CONVERSION;

  CWnd * pWnd = GetDlgItem( uId );
  if ( pWnd == NULL )
    return;
  if ( m_bDualDatasetDisplay )
    return;

  CRect rc;
  pWnd->GetWindowRect( &rc );
  ScreenToClient( &rc );
  Gdiplus::RectF rect( rc.left, rc.top, rc.Width(),  rc.Height() );

  CString sText;
  pWnd->GetWindowText( sText );

  Gdiplus::StringFormat format;
  format.SetAlignment( eAlignment );

  gfx.DrawString( A2W( (LPCTSTR)sText), sText.GetLength(), Theme::PatientInfoFont, rect, &format, Theme::PatientInfoBrush );

} // DrawStatic( Gdiplus::Graphics & gfx, uint4 uId, Gdiplus::StringAlignment alignment )


/**
 * Repaints backgrounds for controls
 * 
 * @param pDC         Control device context
 * @param pWnd        Pointer to control window
 * @param uCtlColor   Identifies the type of control
 * @return            Brush to be used to paint the control's background
 */
HBRUSH PatientInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor) 
{
  HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, uCtlColor);
 /* pDC->SetTextColor(0x000000);
  pDC->SetBkMode(TRANSPARENT);*/
  return hbr;
} // OnCtlColor()


/**
 * Sets the dataset orientation appropriately
 */
void PatientInfoDlg::OnClickedSupineProneBtn()
{
  //AfxGetMainWnd()->SendMessage(WM_APP_SET_MAIN_DATASET, WPARAM((m_supineProneBtn.GetCheck() == 0)
  //  ? DatasetOrientations::eSUPINE : DatasetOrientations::ePRONE), 0);

  //UpdateSupineProneButton();

} // OnClickedSupineProneBtn()


/**
 * Sets the proper icon for the supine/prone button
 */
void PatientInfoDlg::UpdateSupineProneButton()
{
  m_btnToggleDisplay.SetCheck( ( rdrGlobal.m_pCurrDataset == &rdrGlobal.m_prone ) ? TRUE : FALSE );

  //const bool bSupine( rdrGlobal.m_pCurrDataset->GetOrientation() == DatasetOrientations::eSUPINE );
  //SetButtonIcon(m_supineProneBtn, IDB_BUTTON_SMALL);

  //m_supineProneBtn.SetWindowText(ColonViews::Layouts::ColonViatronixWnd::PatientOrientationToString(*rdrGlobal.m_pCurrDataset, true).c_str());
} // UpdateSupineProneButton()


/**
 * Sets the mouse pointer in the window
 * 
 * @param pWnd       Pointer to this window
 * @param uHitTest   Code identifying location of cursor
 * @param uMessage   Mouse message generating the set cursor event
 * @return           Whether callee has set the cursor
 */
BOOL PatientInfoDlg::OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage) 
{
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  return TRUE;
} // OnSetCursor()


/**
 * Updates the supine/prone combo appropriately
 * 
 * @param uIDEvent   Event ID, assumed to be the supine/prone update timer event (only one for this dialog)
 */
void PatientInfoDlg::OnTimer(UINT_PTR uIDEvent) 
{
  // Quick fix for keeping supine/prone combo up-to-date
  if (uIDEvent == m_updateSPTimer)
  {
    OnMessageInitStudy(0,0);
  }

  CDialog::OnTimer(uIDEvent);
} // OnTimer()


// Revision History:
// $Log: PatientInfoDlg.C,v $
// Revision 1.13.2.15  2010/04/11 11:56:14  dongqing
// centering child controls
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.14  2010/04/01 06:45:50  dongqing
// putting back the F4l/toggle datasets functionality I was previously told to remove.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.13  2010/03/05 09:00:34  dongqing
// set date struct member of studyinfo
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.12  2010/03/03 08:58:55  dongqing
// update
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.11  2010/02/16 09:02:46  dongqing
// don't draw anythnig in dialog if we're showing 2 datasets (information will be shown elsewhere)
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.10  2010/02/01 19:24:11  dongqing
// study info into the dataset class
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.9  2010/01/31 04:16:12  dongqing
// set the patient name in the window title
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.8  2010/01/17 18:29:16  dongqing
// update
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.7  2010/01/16 04:03:53  dongqing
// only show data for loading/loaded datasets
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.6  2010/01/14 14:43:47  dongqing
// displaying info of both datasets on dual-dataset layouts
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.5  2010/01/07 18:44:37  dongqing
// identify studies associated with both datasets (for cases where datasets from different studies are loaded)
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.13.2.4  2009/11/25 22:11:02  dongqing
// moved dataset switch button to controls
//
// Revision 1.13.2.3  2009/11/16 21:57:31  dongqing
// widescreen detection
//
// Revision 1.13.2.2  2008/10/08 18:43:47  jmeade
// spacing (no code change)
//
// Revision 1.13.2.1  2007/09/14 20:39:42  jmeade
// Issue 5776: removed double-processing of patient name.
//
// Revision 1.13  2007/03/10 06:09:14  jmeade
// code standards.
//
// Revision 1.12  2007/03/08 22:22:20  romy
// code review - header files adjusted
//
// Revision 1.11  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.10  2006/08/24 01:28:44  jmeade
// Issue 4946: strings for decubitus studies.
//
// Revision 1.9  2006/07/24 19:46:57  jmeade
// Issue 4657: Update patient info upon volume header info read.
//
// Revision 1.8  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.7  2006/01/31 14:27:03  frank
// code review
//
// Revision 1.6  2005/11/16 04:52:07  jmeade
// Merge from Merge_VC_2-0_051115.
//
//
// Revision 1.5.2.3  2006/05/22 12:42:06  geconomos
// code cleanup
//
// Revision 1.5.2.2  2006/02/28 17:29:39  geconomos
// modified for SkinButton changes
//
// Revision 1.5.2.1  2005/11/09 02:15:13  jmeade
// comments.
//
// Revision 1.5  2005/10/06 20:35:59  geconomos
// reimplemented drawing
//
// Revision 1.4  2005/08/25 13:37:43  geconomos
// Issue# 4387: Enable Supine/Prone toggle for orthogonal view
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.15.2.2.2.1.2.2  2005/07/08 18:58:24  jmeade
// privatize dataset modality variable.
//
// Revision 3.15.2.2.2.1.2.1  2005/06/17 13:17:31  geconomos
// commented out OnCtlColor handler (let's see if its needed )
//
// Revision 3.15.2.2.2.1  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.15.2.2.4.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 3.15.2.2  2005/02/15 19:43:21  jmeade
// merge from vc1.4 branch.
//
// Revision 3.15.2.1.2.1  2004/11/18 20:48:28  jmeade
// MR dataset toggle buttons.
//
// Revision 3.15.2.1  2003/02/27 02:22:55  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.15  2003/01/29 00:55:09  jmeade
// Removed link button (to be place elsewhere).
//
// Revision 3.14  2003/01/18 16:27:14  kevin
// Made the "link" button on the Supine/Prone screen effect the linking of the registration too
//
// Revision 3.13  2002/11/28 02:21:32  jmeade
// Two supine/prone toggle images to always display the current dataset.
//
// Revision 3.12  2002/11/26 18:43:02  jmeade
// Default to datasets being linked.
//
// Revision 3.11  2002/11/26 01:31:35  jmeade
// Supine/prone toggle, dataset link buttons.
//
// Revision 3.10  2002/09/10 15:47:35  jmeade
// Use one function to handle changing main dataset.
//
// Revision 3.9  2002/08/29 22:11:16  jmeade
// Changed supine-prone toggle combo to an auto check button; Dataset link button.
//
// Revision 3.8  2002/07/26 22:29:01  jmeade
// Load default text for empty patient name, set patient name in title bar.
//
// Revision 3.7  2002/07/18 20:25:38  jmeade
// Moved report functionality to ConsoleLib.
//
// Revision 3.6  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.5  2002/04/18 18:28:01  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.4  2002/04/12 02:13:35  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.3.2.2  2002/04/05 18:52:49  jmeade
// Issue 2035: Use 1 character for gender (to save real estate).
//
// Revision 3.3.2.1  2002/04/05 18:18:39  jmeade
// Issue 2035: Split ID out of patient data to its own field; Abbreviate patient ID display when displaying if necessary.
//
// Revision 3.3  2001/11/20 09:50:34  jmeade
// Allow caller to set background image resource.
//
// Revision 3.2  2001/11/18 01:12:31  jmeade
// Custom specification of patient info resource template.
//
// Revision 3.1  2001/11/17 03:34:46  jmeade
// Derive from ViatronDialog to handle looks (threw out the old carpet).
//
// Revision 3.0  2001/10/14 23:01:58  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Sep 27 2001 11:05:56   jmeade
// No more encrypted patient name/ID
// 
//    Rev 2.2   Sep 19 2001 15:14:14   binli
// no change 
// 
//    Rev 2.1   Sep 19 2001 15:06:42   binli
// (no track ID): bad patient name('0' ) and ID ('0')
// 
//    Rev 2.0   Sep 16 2001 23:41:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:24   ingmar
// Initial revision.
// Revision 1.35  2001/09/12 21:59:42  jmeade
// Let's see how many iterations will it take me to get bug in 1.33 condensed correctly
//
// Revision 1.34  2001/09/12 17:11:49  jmeade
// Oops, fixed bug in 1.33 code condense (a dash must exist in anonymous patient name)
//
// Revision 1.33  2001/08/30 17:19:03  jmeade
// Condensed some code with a standard C function call
//
// Revision 1.32  2001/08/17 19:53:03  binli
// load study from private CD.
//
// Revision 1.31  2001/07/16 23:40:31  jmeade
// text color back to original white in anticipation of darker bkgd
//
// Revision 1.30  2001/07/05 15:07:09  jmeade
// Get dialog color from parent
//
// Revision 1.29  2001/06/18 17:06:31  jmeade
// better bug 357(487)/489 fix by making individual views aware of the last
// study that was rendered from
//
// Revision 1.28  2001/06/16 00:22:44  jmeade
// using WmTimer class
//
// Revision 1.27  2001/06/14 00:48:32  jmeade
// coding conventions; encrypted patient name
//
// Revision 1.26  2001/06/05 22:57:58  binli
// bug 000357: fixed
// (different number of slices in S&P studies)
//
// Revision 1.25  2001/05/15 20:11:17  binli
// code conventions: don't forget those code within non-final-release mode.
//
// Revision 1.24  2001/05/15 18:55:44  jmeade
// Code conventions
//
// Revision 1.23  2001/05/04 15:33:13  binli
// reflected works of displaylists.
//
// Revision 1.22  2001/04/18 22:18:36  binli
// set up flag for ModelView matrix updating in overview.
//
// Revision 1.21  2001/04/13 02:34:11  jmeade
// headers and footers
//
// Revision 1.20  2001/03/13 22:31:15  jmeade
// Quick fix for keeping the supine/prone combobox up to date
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/PatientInfoDlg.C,v 1.13.2.15 2010/04/11 11:56:14 dongqing Exp $
// $Id: PatientInfoDlg.C,v 1.13.2.15 2010/04/11 11:56:14 dongqing Exp $
