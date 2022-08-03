// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Greg Davidson  gdavidson@viatronix.net


#pragma once

#include "ViatronDialog.h"
#include "resource.h"
#include <functional>

namespace ColonViews
{

namespace Dialogs
{

class ProgressDlg : public ViatronDialog
{
public:
  ProgressDlg( const CString & caption );

  // Dialog Data
  //{{AFX_DATA(AboutDlg)
  enum { IDD = IDD_PROGRESS_DLG };
  //}}AFX_DATA

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(AboutDlg)

  INT_PTR DoModal();

  void Wait();

  void CloseDialog();

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  //{{AFX_MSG(AboutDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnLButtonDown(UINT nHitTest, CPoint point);
  afx_msg void OnMButtonDown(UINT nHitTest, CPoint point);
  afx_msg void OnRButtonDown(UINT nHitTest, CPoint point);
  afx_msg LRESULT OnCloseDialog( WPARAM wParam, LPARAM lParam );
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
  void OnPaint();

private:

  CProgressCtrl m_progress;

  /// release delay associated progress generator
  std::shared_ptr< void > m_hStartProgressEvent;

  CString m_caption;

};

} }

// $Log: AboutDlg.h,v $
// Revision 1.1  2005/11/07 20:36:40  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/AboutDlg.h,v 1.1 2005/11/07 20:36:40 geconomos Exp $
// $Id: AboutDlg.h,v 1.1 2005/11/07 20:36:40 geconomos Exp $
