// $Id: PickScreenPoint.h,v 1.3 2007/03/10 06:09:14 jmeade Exp $
//
// Copyright© 2000-2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PickScreenPoint.h
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


#if !defined(AFX_PICKSCREENPOINT_H__DB80F446_6947_4E93_9F36_B8EC9ADD7A1F__INCLUDED_)
#define AFX_PICKSCREENPOINT_H__DB80F446_6947_4E93_9F36_B8EC9ADD7A1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PickScreenPoint.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// PickScreenPoint dialog

namespace ColonViews
{

  namespace Dialogs
  {

    class PickScreenPointDlg : public CDialog
    {
    public:
      /// Constructor
      PickScreenPointDlg(CWnd* pParent = NULL);

      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(PickScreenPointDlg)
    protected:
      //}}AFX_VIRTUAL

      // Generated message map functions
      //{{AFX_MSG(PickScreenPointDlg)
      /// initializations
      virtual BOOL OnInitDialog();
      /// wm_timer
      afx_msg void OnTimer(UINT_PTR uIDEvent);
      /// left mouse button
      afx_msg void OnLButtonDown(UINT uFlags, CPoint point);
      /// right mouse button
      afx_msg void OnRButtonDown(UINT uFlags, CPoint point);
      /// control colouring
      afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor);
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

    private:
      /// Locates the dialog near the mouse cursor and makes it topmost
      void SetWindowLoc();

    private:
      /// Background brush for dialog
      CBrush m_bkgBrush;

      //{{AFX_DATA(PickScreenPointDlg)
      enum { IDD = IDD_PICKSCREENPOINT };
      // NOTE: the ClassWizard will add data members here
      //}}AFX_DATA
    }; // class PickScreenPointDlg

  } // namespace Dialogs

} // namespace ColonViews

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICKSCREENPOINT_H__DB80F446_6947_4E93_9F36_B8EC9ADD7A1F__INCLUDED_)

// Revision History:
// $Log: PickScreenPoint.h,v $
// Revision 1.3  2007/03/10 06:09:14  jmeade
// code standards.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.1.2.1  2003/03/12 20:32:58  jmeade
// Issue 2827:  Allow user to select which view to capture for movie.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/PickScreenPoint.h,v 1.3 2007/03/10 06:09:14 jmeade Exp $
// $Id: PickScreenPoint.h,v 1.3 2007/03/10 06:09:14 jmeade Exp $
