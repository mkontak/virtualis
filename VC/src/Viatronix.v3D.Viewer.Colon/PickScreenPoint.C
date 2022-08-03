// $Id: PickScreenPoint.C,v 1.3 2007/03/10 06:09:14 jmeade Exp $
//
// Copyright© 2000-2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PickScreenPoint.C
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


#include "stdafx.h"
#include "PickScreenPoint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ColonViews::Dialogs;

/////////////////////////////////////////////////////////////////////////////
// PickScreenPointDlg dialog

/**
 * Constructor 
 *
 * @param pParent   Parent window.
 */
PickScreenPointDlg::PickScreenPointDlg(CWnd* pParent /*=NULL*/)
  : CDialog(PickScreenPointDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(PickScreenPointDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
} // constructor


/**
 * message map
 */
BEGIN_MESSAGE_MAP(PickScreenPointDlg, CDialog)
  //{{AFX_MSG_MAP(PickScreenPointDlg)
  ON_WM_TIMER()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  ON_WM_CTLCOLOR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PickScreenPointDlg message handlers


/**
 * Dialog initialization.
 *
 * @return   TRUE unless focus is set to a control.
 */
BOOL PickScreenPointDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  
  // TODO: Add extra initialization here
  m_bkgBrush.CreateSolidBrush(GetSysColor(COLOR_INFOBK));

  CRgn rgnTop, rgnBtm, rgn;
  CRect rect;  GetClientRect(rect);

  rgnTop.CreateRoundRectRgn(0, rect.top, int(rect.Width()*0.2), 50, 10,10);
  rgnBtm.CreateRoundRectRgn(rect.left, int(rect.Height()*0.4), rect.right, rect.bottom, 10,10);
  rgn.CreateRectRgn(0,0,50,50);
  rgn.CombineRgn(&rgnTop, &rgnBtm, RGN_OR);
  
  SetWindowRgn(rgn, TRUE);
  rgn.Detach();

  SetWindowLoc();
  SetTimer(1,20,NULL);
  return TRUE;  // return TRUE  unless you set the focus to a control
} // OnInitDialog()


/**
 * WM_TIMER handler.
 *
 * @param uIDEvent   ID of timer event.
 */
void PickScreenPointDlg::OnTimer(UINT_PTR uIDEvent) 
{
  SetWindowLoc();  
  CDialog::OnTimer(uIDEvent);
} // OnTimer()


/**
 * Left button down handler.
 *
 * @param uFlags   Indicates which virtual keys are currently pressed.
 * @param point    Point of button down.
 */
void PickScreenPointDlg::OnLButtonDown(UINT uFlags, CPoint point) 
{
  CDialog::OnLButtonDown(uFlags, point);
  OnOK();
} // OnLButtonDown()


/**
 * Right button down handler.
 *
 * @param uFlags   Indicates which virtual keys are currently pressed.
 * @param point    Location of mouse at event.
 */
void PickScreenPointDlg::OnRButtonDown(UINT uFlags, CPoint point) 
{
  CDialog::OnRButtonDown(uFlags, point);
  OnCancel();
} // OnRButtonDown()


/**
 * WM_CTLCOLOR handler.
 *
 * @param pDC         Device context for dialog.
 * @param pWnd        The window being colored.
 * @param uCtlColor   Identifies the type of control being colored.
 * @return            Brush to draw the control's background.
 */
HBRUSH PickScreenPointDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor)
{
  HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, uCtlColor);
  
  // TODO: Change any attributes of the DC here
  pDC->SetBkColor(GetSysColor(COLOR_INFOBK));
  return m_bkgBrush;
} // OnCtlColor()


/**
 * Sets the location of the window just to the right of the mouse cursor and makes it topmost.
 */
void PickScreenPointDlg::SetWindowLoc()
{
  CPoint pt;    GetCursorPos(&pt);
  CRect rect;   GetClientRect(rect);
  SetWindowPos(&CWnd::wndTopMost, pt.x-4, pt.y-4, 0,0, SWP_NOSIZE);
} // SetWindowLoc()


// Revision History:
// $Log: PickScreenPoint.C,v $
// Revision 1.3  2007/03/10 06:09:14  jmeade
// code standards.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.1  2003/03/12 20:32:58  jmeade
// Issue 2827:  Allow user to select which view to capture for movie.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/PickScreenPoint.C,v 1.3 2007/03/10 06:09:14 jmeade Exp $
// $Id: PickScreenPoint.C,v 1.3 2007/03/10 06:09:14 jmeade Exp $
