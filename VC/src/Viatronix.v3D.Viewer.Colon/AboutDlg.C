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
#include "AboutDlg.h"
#include "VersionInfo.h"
#include "ReaderGlobal.h"
#include "Theme.h"


using namespace ReaderLib;
using namespace WinUtils;
using namespace ColonViews::Dialogs;

/**
 * Constructor
 */
AboutDlg::AboutDlg(const uint4 uBkgdImageResID) : ViatronDialog(AboutDlg::IDD, NULL, uBkgdImageResID)
{
  //{{AFX_DATA_INIT(AboutDlg)
  //}}AFX_DATA_INIT
}


/**
 * Initializes dialog variables
 *
 * @param CDataExchange* pDX
 */
void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(AboutDlg)
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AboutDlg, ViatronDialog)
  //{{AFX_MSG_MAP(AboutDlg)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_MBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * Define window shape, load splash screen and version info
 *
 * @return BOOL   Success of init dialog
 */
BOOL AboutDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  VersionInformation vinfo(HMODULE(NULL));
  CString sVersion;
  GetDlgItemText(IDC_V1K_VERSION_STATIC, sVersion);
  SetDlgItemText(IDC_V1K_VERSION_STATIC, (LPCTSTR(sVersion) + vinfo.m_sFixedFileVersion + vinfo.m_sFixedFileFlags).c_str());

  GetDlgItemText(IDC_CAD_VERSION, sVersion);

  //const std::string sVersInfo(rdrGlobal.m_pCurrDataset->m_cadFindings.GetCadVendor() + ' ' + rdrGlobal.m_pCurrDataset->m_cadFindings.GetCadApiVersion());
  const std::string sVersInfo( rdrGlobal.m_pCurrDataset->m_cadFindings.GetCadVendor() );  /// We'll only display vender name from now on. Dongqing 03/26/2009
  SetDlgItemText(IDC_CAD_VERSION, !sVersInfo.empty() ? ((LPCTSTR(sVersion) + sVersInfo).c_str()) : "" );

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}


/**
 * on paint
 */
void AboutDlg::OnPaint()
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
void AboutDlg::OnLButtonDown(UINT uHitTest, CPoint point) 
{
  OnOK();
}


/**
 * Middle mutton down; dismisses dialog
 *
 * @param uHitTest   Unused
 * @param point      Unused
 */
void AboutDlg::OnMButtonDown(UINT uHitTest, CPoint point) 
{
  OnOK();
}


/**
 * Right mutton down; dismisses dialog
 *
 * @param uHitTest   Unused
 * @param point      Unused
 */
void AboutDlg::OnRButtonDown(UINT uHitTest, CPoint point) 
{
  OnOK();
}


// $Log: AboutDlg.C,v $
// Revision 1.5.2.1  2009/03/27 16:51:10  dongqing
// change iCAD product name as VeraLook
//
// Revision 1.5  2006/10/04 20:49:10  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.4  2006/06/19 16:05:55  jmeade
// comments.
//
// Revision 1.3  2006/05/22 15:17:30  jmeade
// Issue 4800: display cad vendor with version number.
//
// Revision 1.2  2006/05/20 00:40:50  jmeade
// Issue 4800: display cad version.
//
// Revision 1.1  2005/11/07 20:36:40  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/AboutDlg.C,v 1.5.2.1 2009/03/27 16:51:10 dongqing Exp $
// $Id: AboutDlg.C,v 1.5.2.1 2009/03/27 16:51:10 dongqing Exp $
