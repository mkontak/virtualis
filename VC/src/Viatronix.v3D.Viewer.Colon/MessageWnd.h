// $Id: MessageWnd.h,v 1.4 2007/03/12 20:47:11 jmeade Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author(s): Jeffrey Meade  jmeade@viatronix.net


// pragmas
#pragma once


namespace WinUtils
{
  /// small compact class to display a message to user
  class MessageWnd : public CFrameWnd
  {
  public:
    /// type of message window to invoke
    enum MsgStyle { MW_DEFAULT = 0, MW_OK = 1, MW_WAIT = 2 };

  public:
    /// show the dialog
    static void Show(const CString& sMessage, const uint4 uDurationMS = 2000, const MsgStyle eStyle = MW_DEFAULT);

  protected:
    /// constructor
	  MessageWnd();
    /// destructor
	  virtual ~MessageWnd();

  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(MessageWnd)
	  public:
	  protected:
	  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	  //}}AFX_VIRTUAL

  // Implementation
  protected:
	  // Generated message map functions
	  //{{AFX_MSG(MessageWnd)
	  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	  afx_msg void OnTimer(UINT_PTR uIDEvent);
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()

  private:
    /// dynamic creation
	  DECLARE_DYNCREATE(MessageWnd)
    /// create dialog
	  void Create(const CString& sMessage, const uint4 uDurationMS = 2000, const MsgStyle eStyle = MW_DEFAULT);
    /// dismiss dialog
	  void EndMessageWnd();

  private:
    /// button IDs
    enum { eWAITBTNID = 1000, eOKBTNID = 1001 };

  private:
    /// box for the message
    CStatic m_staticMessage;
    /// background image
    CStatic m_staticBkgnd;
    /// ok button
    CButton m_btnOK;

    /// ID of timer to destroy dialog
    uint4 m_uDestroyWinTimerID;
    /// duration of the window
    uint4 m_uDurationMS;
    /// the message
    CString m_sMessage;
    /// type of message window
    MsgStyle m_eStyle;
    /// dialog font
    CFont m_font;
  }; // MessageWnd

} // namespace WinUtils

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// Revision History:
// $Log: MessageWnd.h,v $
// Revision 1.4  2007/03/12 20:47:11  jmeade
// code standards.
//
// Revision 1.3  2006/01/31 14:26:44  frank
// code review
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
// 
//    Rev 2.0   Sep 16 2001 23:39:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:44   ingmar
// Initial revision.
// Revision 1.1  2001/06/14 00:38:14  jmeade
// method (and accompanying class) for temp message boxes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MessageWnd.h,v 1.4 2007/03/12 20:47:11 jmeade Exp $
// $Id: MessageWnd.h,v 1.4 2007/03/12 20:47:11 jmeade Exp $
