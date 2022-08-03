// $Id: AboutDlg.h,v 1.1 2005/11/07 20:36:40 geconomos Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.net


#pragma once

#include "ViatronDialog.h"
#include "resource.h"

namespace ColonViews
{

namespace Dialogs
{

class AboutDlg : public ViatronDialog
{
public:
  AboutDlg(const uint4 uBkgdImageResID);

  // Dialog Data
  //{{AFX_DATA(AboutDlg)
  enum { IDD = IDD_ABOUTBOX };
  //}}AFX_DATA

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(AboutDlg)
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
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
  void OnPaint();
};

} }

// $Log: AboutDlg.h,v $
// Revision 1.1  2005/11/07 20:36:40  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/AboutDlg.h,v 1.1 2005/11/07 20:36:40 geconomos Exp $
// $Id: AboutDlg.h,v 1.1 2005/11/07 20:36:40 geconomos Exp $
