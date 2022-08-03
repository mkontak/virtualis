// $Id: AboutDlg.C,v 1.5.2.1 2009/03/27 16:51:10 dongqing Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.net


#include "StdAfx.h"
#include "ProgressDlg.h"
#include "VersionInfo.h"
#include "ReaderGlobal.h"
#include "Theme.h"
#include "resource.h"


using namespace ReaderLib;
using namespace WinUtils;
using namespace ColonViews::Dialogs;

#define PROGRESS_DLG_CLOSE (WM_APP + 1)


/**
 * Constructor
 */
ProgressDlg::ProgressDlg( const CString & caption ) : ViatronDialog(ProgressDlg::IDD, NULL),
  m_caption( caption )
{
  //{{AFX_DATA_INIT(AboutDlg)
  //}}AFX_DATA_INIT

  m_hStartProgressEvent = std::shared_ptr< void >( CreateEvent( NULL, FALSE, FALSE, NULL ), []( HANDLE handle )
  {
    SetEvent( handle );
    CloseHandle( handle );
  });
}


/**
 * Initializes dialog variables
 *
 * @param CDataExchange* pDX
 */
void ProgressDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control( pDX, IDC_PROGRESS, m_progress );
}

BEGIN_MESSAGE_MAP(ProgressDlg, ViatronDialog)
  //{{AFX_MSG_MAP(AboutDlg)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_MBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  ON_MESSAGE( PROGRESS_DLG_CLOSE, OnCloseDialog )
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


INT_PTR ProgressDlg::DoModal()
{
  SetEvent( m_hStartProgressEvent.get() );
  return ViatronDialog::DoModal();
}


void ProgressDlg::Wait()
{
  WaitForSingleObject( m_hStartProgressEvent.get(), INFINITE );
}


void ProgressDlg::CloseDialog()
{
  PostMessage( PROGRESS_DLG_CLOSE, 0, 0 );
}


/**
 * Define window shape, load splash screen and version info
 *
 * @return BOOL   Success of init dialog
 */
BOOL ProgressDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  m_progress.ModifyStyle( 0, PBS_MARQUEE, 0);
  m_progress.SendMessage( PBM_SETMARQUEE, TRUE, 50 ); 

  SetDlgItemText( IDC_PROGRESS_LABEL, m_caption );

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}


/**
 * on paint
 */
void ProgressDlg::OnPaint()
{
  CPaintDC dc( this );

  // do we need to display the beta disclaimer?
  if( rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::DrawDisclaimer()) )
  {
    Gdiplus::Graphics gfx ( dc.GetSafeHdc() );
    Theme::DrawDisclaimer( gfx, Gdiplus::RectF() );
  }
}


/**
 * Left mutton down; dismisses dialog
 *
 * @param uHitTest   Unused
 * @param point      Unused
 */
void ProgressDlg::OnLButtonDown(UINT uHitTest, CPoint point) 
{
  //OnOK();
}


/**
 * Middle mutton down; dismisses dialog
 *
 * @param uHitTest   Unused
 * @param point      Unused
 */
void ProgressDlg::OnMButtonDown(UINT uHitTest, CPoint point) 
{
  //OnOK();
}


/**
 * Right mutton down; dismisses dialog
 *
 * @param uHitTest   Unused
 * @param point      Unused
 */
void ProgressDlg::OnRButtonDown(UINT uHitTest, CPoint point) 
{
  //OnOK();
}



LRESULT ProgressDlg::OnCloseDialog( WPARAM wParam, LPARAM lParam )
{
  OnOK();
  return 0;
}