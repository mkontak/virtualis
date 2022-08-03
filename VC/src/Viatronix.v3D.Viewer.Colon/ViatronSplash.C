// $Id: ViatronSplash.C,v 1.14.2.1 2007/04/03 20:28:52 gdavidson Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Jeffrey R. Meade  (mailto:jmeade@viatronix.net)


// includes
#include "stdafx.h"
#include "viatron.h"
#include "ViatronSplash.h"
#include "VersionInfo.h"
#include "VCSettings.h"
#include "Theme.h"


// defines
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// namespaces
using namespace ColonViews::Dialogs;
using namespace WinUtils;


/**
 * Constructor 
 *
 * @param bCheckLogin   Whether to show login fields.
 * @param pParent       Parent window.
 */
ViatronSplash::ViatronSplash( const bool bCheckLogin /*=false*/, CWnd * pParent /*=NULL*/ ) :
  ViatronDialog( ViatronSplash::IDD, pParent, IDB_V3D_COLON_LOGIN),
  m_bCheckUserLogin( bCheckLogin ),
  m_okFunction( nullptr ),
  m_cancelFunction( nullptr )
{
  m_sProgress = _T("");
} // ViatronSplash()


/**
 * Constructor 
 *
 * @param bCheckLogin   Whether to show login fields.
 * @param okPtr         ok function ptr
 * @param cancelPtr     cancel function ptr
 * @param pParent       Parent window.
 */
ViatronSplash::ViatronSplash( const bool bCheckLogin, FunctionPtr okPtr, FunctionPtr cancelPtr, CWnd * pParent) :
  ViatronDialog( ViatronSplash::IDD, pParent, IDB_V3D_COLON_LOGIN),
  m_bCheckUserLogin( bCheckLogin ),
  m_okFunction( okPtr ),
  m_cancelFunction( cancelPtr )
{
  //{{AFX_DATA_INIT(ViatronSplash)
  m_sProgress = _T("");
  //}}AFX_DATA_INIT
}


/**
 * Resets the dialog
 */
void ViatronSplash::Reset()
{
  SetDlgItemText(IDC_VSP_USERNAME_EDIT, m_sUser.c_str());
  SetDlgItemText(IDC_VSP_PASSWORD_EDIT, CString());

  if( m_sUser.length() > 0 )
    GetDlgItem( IDC_VSP_PASSWORD_EDIT )->SetFocus();
  else
    GetDlgItem( IDC_VSP_USERNAME_EDIT )->SetFocus();
} // Reset()


/**
 * Dialog member initializer.
 *
 * @param pDX   Dialog data exchange instance.
 */
void ViatronSplash::DoDataExchange( CDataExchange * pDX )
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(ViatronSplash)
  DDX_Control(pDX, IDC_VSP_USERNAME_LABEL, m_usernameLabel);
  DDX_Control(pDX, IDC_VSP_PASSWORD_LABEL, m_passwordLabel);
  DDX_Control(pDX, IDC_VSP_PASSWORD_EDIT, m_passwordEdit);
  DDX_Control(pDX, IDC_VSP_USERNAME_EDIT, m_usernameEdit);
  DDX_Control(pDX, IDC_VSP_VERSION, m_versionStatic);
  DDX_Control(pDX, IDC_VSP_LOGIN, m_loginBtn);
  DDX_Control(pDX, IDC_VSP_CLOSE, m_closeBtn);
  DDX_Control(pDX, IDC_VSP_PROGRESS, m_progressCtrl);
  DDX_Text(pDX, IDC_VSP_PROGRESS_TEXT, m_sProgress);
  //}}AFX_DATA_MAP
} // DoDataExchange()


BEGIN_MESSAGE_MAP(ViatronSplash, ViatronDialog)
  //{{AFX_MSG_MAP(ViatronSplash)
  ON_BN_CLICKED(IDC_VSP_LOGIN, OnLogin)
  ON_BN_CLICKED(IDC_VSP_CLOSE, OnCloseHandler)
  ON_WM_TIMER()
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * Dialog initialization handler.
 *
 * @return   TRUE if focus is set to a control.
 */
BOOL ViatronSplash::OnInitDialog()
{

  CDialog::OnInitDialog();
  
#ifdef FINAL_RELEASE
  // ensure this will come up as the topmost window
  SetWindowPos(&wndTop, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE);
  SetForegroundWindow();
#endif

  // Always check login in release mode; Check in debug if user wants to
#ifdef FINAL_RELEASE
  m_bCheckUserLogin = true;
#else
//  V3DSystemRegistryKey regKey(RegistryKey::eMACHINE);
//  m_bCheckUserLogin = false;
#endif

  CRect clientRect;
  GetClientRect(clientRect);

  if (!m_bCheckUserLogin)
  {
    m_usernameEdit.ShowWindow(SW_HIDE);   m_usernameLabel.ShowWindow(SW_HIDE);
    m_passwordEdit.ShowWindow(SW_HIDE);   m_passwordLabel.ShowWindow(SW_HIDE);
    m_loginBtn.ShowWindow(SW_HIDE);
    m_closeBtn.ShowWindow(SW_HIDE);
  }
  else
  {
    SetDlgItemText(IDC_VSP_USERNAME_EDIT, m_sUser.c_str());
    if( m_sUser.length() > 0 )
      GetDlgItem( IDC_VSP_PASSWORD_EDIT )->SetFocus();
  }

  // Set version information
  VersionInformation vinfo(HMODULE(NULL));
  CString sVersion;
  GetDlgItemText(IDC_VSP_VERSION, sVersion);
  SetDlgItemText(IDC_VSP_VERSION, sVersion + vinfo.m_sFixedFileVersion.c_str() + vinfo.m_sFixedFileFlags.c_str());

  // check the caps-lock key to indicate to user
  //SetTimer(1, 200, NULL);
  //OnTimer(1);

  SetWindowText(LoadResourceString(IDR_MAINFRAME));

  return FALSE;

} // OnInitDialog()


/**
 * WM_TIMER handler.
 *
 * @param uIDEvent   ID of timer event.
 */
void ViatronSplash::OnTimer(UINT_PTR uIDEvent) 
{

  // Check caps-lock key
  CString sCapsLock;
  GetDlgItemText(IDC_VSP_CAPSLOCK_DISP, sCapsLock);

  if (GetKeyState(VK_CAPITAL)&0X1)
  {
    if (sCapsLock.IsEmpty())
    {
      SetDlgItemText(IDC_VSP_CAPSLOCK_DISP, LoadResourceString(IDS_INDICATOR_CAPSLOCK));
    }
  }
  else if (!sCapsLock.IsEmpty())
  {
    SetDlgItemText(IDC_VSP_CAPSLOCK_DISP, CString());
  }

} // OnTimer()


/**
 * WM_PAINT handler
 */
void ViatronSplash::OnPaint()
{
  CPaintDC dc( this );
} // OnPaint()


/**
 * Sets position of progress bar.
 *
 * @param iPercentage   0-100 percentage of progress.
 * @param sText         Text to display.
 */
void ViatronSplash::SetProgress(int4 iPercentage, const std::string & sText)
{
  if (!IsWindow(GetSafeHwnd()))  return;

  if (iPercentage < 0)         iPercentage = 0;
  else if (iPercentage > 100)  iPercentage = 100;

  m_progressCtrl.SetPos(iPercentage);
  m_sProgress = sText.c_str();

  UpdateData(FALSE);
} // SetProgress()


/**
 * Prompts user to enter a valid username and password.
 *
 * @return   TRUE if a valid login supplied by user, FALSE otherwise.
 */
BOOL ViatronSplash::CheckLogin()
{
  if (!m_bCheckUserLogin)
  {
    m_progressCtrl.ShowWindow(SW_SHOW);
    return TRUE;
  }

  return (RunModalLoop()==IDOK);
} // CheckLogin()


/**
 * Validates login.
 */
void ViatronSplash::OnOK()
{
  OnLogin();
} // OnOK()


/**
 * Does nothing (user cannot exit software without logging in).
 */
void ViatronSplash::OnCancel()
{
  OnCloseHandler();
} // OnCancel()


/**
 * Validates user-supplied login information.
 */
void ViatronSplash::OnLogin()
{
  CString stext;
  GetDlgItemText(IDC_VSP_USERNAME_EDIT, stext);
  m_sUser = LPCTSTR(stext);
  GetDlgItemText(IDC_VSP_PASSWORD_EDIT, stext);
  m_sPass = LPCTSTR(stext);

	if( m_sPass.size() <= 0 )
		m_sPass = "EmptyPasswordIsNotAllowedInViatronix3DSystem!";

  // if the ok function ptr is not set or it returns true
  if( m_okFunction == nullptr || m_okFunction( *this ))
    EndModalLoop(IDOK);
} // OnLogin()


/**
 * Closes dialog
 */
void ViatronSplash::OnCloseHandler()
{
  if( m_cancelFunction == nullptr || m_cancelFunction( *this ))
    EndModalLoop(IDCANCEL);
} // OnCloseHandler()


// $Log: ViatronSplash.C,v $
// Revision 1.14.2.1  2007/04/03 20:28:52  gdavidson
// Issue #5558: Set the username before displaying
//
// Revision 1.14  2007/03/07 22:33:53  jmeade
// code standards.
//
// Revision 1.13  2006/10/04 23:14:46  jmeade
// login screen.
//
// Revision 1.12  2006/04/12 23:11:15  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.11  2006/01/31 14:28:25  frank
// code review
//
// Revision 1.10  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.9  2005/11/16 04:52:07  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.8  2005/11/15 18:52:17  geconomos
// reworked user to be compatible with the new console
//
// Revision 1.7  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.6.2.2  2006/02/21 18:04:39  jmeade
// Issue 4611: Always check login in release builds.
//
// Revision 1.6.2.1  2006/01/05 18:08:37  geconomos
// Issue# 4588: CAD vendor needs to be displayed on about box
//
// Revision 1.6  2005/10/07 13:12:20  geconomos
// removed disclaimer related items
//
// Revision 1.5  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.4  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.3  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.2  2005/08/05 14:14:23  geconomos
// enabled user logins in final release
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.9  2002/09/18 20:07:37  jmeade
// Moved UserAdmin to ConsoleLib.
//
// Revision 3.8  2002/05/15 20:33:44  jmeade
// Allowing for general use of registry (and not just Software\AppName\...).
//
// Revision 3.7  2002/01/14 21:06:50  jmeade
// Splash screen with status area.
//
// Revision 3.6  2002/01/08 22:42:48  jmeade
// Set progress text regardless if parameter string is empty (to clear text).
//
// Revision 3.5  2002/01/02 22:28:42  jmeade
// White text (dark bkgd) for version, Progress text, Code standards.
//
// Revision 3.4  2001/12/22 00:15:17  jmeade
// New backgrounds for Console and splash.
//
// Revision 3.3  2001/12/07 01:29:49  jmeade
// Try to set splash as foreground window, i.e. that which gets user input (not sure it even works).
//
// Revision 3.2  2001/11/17 03:48:47  jmeade
// New splash screen.  Coding standards.
//
// Revision 3.1  2001/11/09 01:17:50  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/11/05 22:10:34  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:43:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:28   ingmar
// Initial revision.
// Revision 1.27  2001/09/14 04:09:54  jmeade
// Topmost only in FINAL_RELEASE (to help debugging)
//
// Revision 1.26  2001/09/07 23:16:06  jmeade
// Set window title so that splash screen in windows taskbar has a tab title
//
// Revision 1.25  2001/08/31 16:42:19  jmeade
// More splash screen clean up
//
// Revision 1.24  2001/08/31 16:17:14  jmeade
// Cleaned up the splash screen; pushpin icon; V3D Viewer text
//
// Revision 1.23  2001/07/05 14:47:35  jmeade
// code for new logos; coding standards
//
// Revision 1.22  2001/05/25 08:45:27  jmeade
// user account types to UserAccountData class; code conventions
//
// Revision 1.21  2001/05/24 22:40:38  jmeade
// Bug fix: 469 - Version information
//
// Revision 1.20  2001/05/19 06:07:13  jmeade
// Automatically enable user check at login for FINAL_RELEASE
//
// Revision 1.19  2001/04/13 02:35:22  jmeade
// headers and footers
//
// Revision 1.18  2001/03/30 22:55:53  jmeade
// derive from ViatronDialog (instead of CDialog) for common skin code
//
// Revision 1.17  2001/03/08 17:21:24  jmeade
// oops!  WINVER probably being reset in <stdafx.h> include (==> windows.h)
//
// Revision 1.16  2001/03/08 16:56:11  jmeade
// added V1KWINDOWS2000 preprocessor definition for WINNT5.0 definitions
//
// Revision 1.15  2001/03/02 15:46:08  jmeade
// removed comments (no code changed)
//
// Revision 1.14  2001/02/23 20:11:32  jmeade
// Removed most unnecessary 'C' before class names (everything's a class!)
//
// Revision 1.13  2001/02/21 17:46:39  jeff
// users cannot sidestep logging into system
//
// Revision 1.12  2001/02/21 17:44:06  jeff
// must keep track of the current logged in user (and not just the user level)
//
// Revision 1.11  2001/02/21 16:21:28  binli
// changed OKCANCEL to YESNO
//
// Revision 1.10  2001/02/21 15:47:24  binli
// added control to quit the system right after the system is started up and before login in.
//
// Revision 1.9  2001/02/19 21:41:01  jeff
// set keyboard focus; indicate when capslock is on; code to force login check
//
// Revision 1.8  2001/02/12 19:52:09  jeff
// oops...redefined macro compile error
//
// Revision 1.7  2001/02/12 19:15:30  jeff
// Check for NT5 before using TransparentBlt
//
// Revision 1.6  2001/02/08 14:45:41  frank
// Changed background color.
//
// Revision 1.5  2001/02/08 03:29:35  jeff
// center splash bitmap in window
//
// Revision 1.4  2001/02/07 21:22:02  geconomos
// Added login to splash screen
//
// Revision 1.3  2001/02/01 02:12:00  jeff
// no message
//
// Revision 1.2  2001/02/01 00:57:10  jeff
// slight improvement to splash dialog look
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// 
// 4     8/28/00 1:52p Antonio
// updated header and footer
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 3  *****************
// User: Jeff            Date:  7/18/00  Time:  4:41p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Only release builds make the (splash) window the topmost (putting it
// on top of other windows is bothersome in Debug mode)
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  7/18/00  Time:  1:58p
// Branched at version 2
// Comment:
//   
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  7/12/00  Time:  9:14p
// Created ViatronSplash.cpp
// Comment:
//   
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViatronSplash.C,v 1.14.2.1 2007/04/03 20:28:52 gdavidson Exp $
// $Id: ViatronSplash.C,v 1.14.2.1 2007/04/03 20:28:52 gdavidson Exp $
