// $Id: ViatronSplash.h,v 1.7.2.1 2007/04/03 20:28:52 gdavidson Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Jeff Meade  (mailto:jmeade@viatronix.net)


// pragmas
#pragma once


// includes
#include "resource.h"
#include "ViatronDialog.h"
#include "WinUtils.h"
#include <string>
#include <functional>


// namespaces
namespace ColonViews
{
  namespace Dialogs
  {

    /// application splash dialog
    class ViatronSplash : public ViatronDialog
    {
    public:

      /// called when ok button is pressed
      typedef std::function< bool( ViatronSplash & dlg )> FunctionPtr;

    public:

      /// Standard constructor
      ViatronSplash( const bool bCheckLogin = false, CWnd * pParent = NULL );

      /// Standard constructor
      ViatronSplash( const bool bCheckLogin, FunctionPtr okPtr, FunctionPtr cancelPtr, CWnd * pParent = NULL );

      /// Sets position (0-100 of progress bar)
      void SetProgress( int4 iPercentage, const std::string & sText = std::string() );

      /// Prompts user to enter a valid username and password
      BOOL CheckLogin();

      /// returns the username
      const std::string GetUsername() const { return m_sUser; }

      /// sets the username
      void SetUsername( const std::string & sUserName ) { m_sUser = sUserName; }

      /// returns the password
      const std::string GetPassword() const { return m_sPass; }

      /// resets the dialog
      void Reset();

      /// Sets the close viewer flag
      void SetCloseViewer(bool bCloseViewer)
      { m_bCloseViewer = bCloseViewer; }

      /// Gets the close viewer flag
      const bool GetCloseViewer() const
      { return m_bCloseViewer; }

    private:
      // Dialog Data
      //{{AFX_DATA(ViatronSplash)
      enum { IDD = IDD_SPLASH_SCREEN };
      /// username static
      CStatic m_usernameLabel;
      /// password static
      CStatic m_passwordLabel;
      /// password enty
      CEdit m_passwordEdit;
      /// username entry
      CEdit m_usernameEdit;
      /// app version information display
      CStatic m_versionStatic;
      /// user login button
      CButton  m_loginBtn;
      /// close button
      CButton  m_closeBtn;
      /// login progress
      CProgressCtrl m_progressCtrl;
      /// progress text
      CString m_sProgress;
      //}}AFX_DATA


     /// Close viewer flag
      bool m_bCloseViewer;


      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(ViatronSplash)
    public:
    protected:
      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
      //}}AFX_VIRTUAL

      // Implementation
    protected:

      /// OK button handler
      virtual void OnOK();

      /// Cancel button handler
      virtual void OnCancel();

      // Generated message map functions
      //{{AFX_MSG(ViatronSplash)
      /// init dialog
      virtual BOOL OnInitDialog();
      /// on login attempted
      afx_msg void OnLogin();
      /// on login attempted
      afx_msg void OnCloseHandler();
      /// timer method
      afx_msg void OnTimer(UINT_PTR nIDEvent);
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

      /// WM_PAINT handler
      void OnPaint();

    private:

      /// user-entered username
      std::string m_sUser;
      /// user-entered password
      std::string m_sPass;

      /// Whether or not the user login will be displayed
      bool m_bCheckUserLogin;

      /// ok function ptr
      FunctionPtr m_okFunction;

      /// cancel function ptr
      FunctionPtr m_cancelFunction;

    }; // class ViatronSplash


    // namespaces
  } // namespace Dialogs
} // namespace ColonViews


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


// $Log: ViatronSplash.h,v $
// Revision 1.7.2.1  2007/04/03 20:28:52  gdavidson
// Issue #5558: Set the username before displaying
//
// Revision 1.7  2007/03/07 22:33:53  jmeade
// code standards.
//
// Revision 1.6  2006/10/04 23:14:46  jmeade
// login screen.
//
// Revision 1.5  2006/01/31 14:28:25  frank
// code review
//
// Revision 1.4  2005/10/07 13:12:20  geconomos
// removed disclaimer related items
//
// Revision 1.3  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.5  2002/04/18 18:35:09  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.4  2002/01/14 21:06:50  jmeade
// Splash screen with status area.
//
// Revision 3.3  2002/01/02 22:28:42  jmeade
// White text (dark bkgd) for version, Progress text, Code standards.
//
// Revision 3.2  2001/12/22 00:15:17  jmeade
// New backgrounds for Console and splash.
//
// Revision 3.1  2001/11/17 03:48:47  jmeade
// New splash screen.  Coding standards.
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:43:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:28   ingmar
// Initial revision.
// Revision 1.10  2001/08/31 16:17:14  jmeade
// Cleaned up the splash screen; pushpin icon; V3D Viewer text
//
// Revision 1.9  2001/07/05 14:47:35  jmeade
// code for new logos; coding standards
//
// Revision 1.8  2001/04/13 02:35:22  jmeade
// headers and footers
//
// Revision 1.7  2001/03/30 22:55:53  jmeade
// derive from ViatronDialog (instead of CDialog) for common skin code
//
// Revision 1.6  2001/02/23 20:11:32  jmeade
// Removed most unnecessary 'C' before class names (everything's a class!)
//
// Revision 1.5  2001/02/21 17:46:39  jeff
// users cannot sidestep logging into system
//
// Revision 1.4  2001/02/21 15:46:36  binli
// added 'm_bAbort' for system quit control
//
// Revision 1.3  2001/02/19 21:43:54  jeff
// ontimer handler
//
// Revision 1.2  2001/02/07 21:22:02  geconomos
// Added login to splash screen
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// 
// 4     8/28/00 2:43p Antonio
// updated header and footer
// 
// *****************  Version 3  *****************
// User: Jeff            Date:  8/24/00  Time:  7:21a
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Added a needed include file
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  7/18/00  Time:  1:58p
// Branched at version 2
// Comment:
//   
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  7/12/00  Time:  9:14p
// Created ViatronSplash.h
// Comment:
//   
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViatronSplash.h,v 1.7.2.1 2007/04/03 20:28:52 gdavidson Exp $
// $Id: ViatronSplash.h,v 1.7.2.1 2007/04/03 20:28:52 gdavidson Exp $
