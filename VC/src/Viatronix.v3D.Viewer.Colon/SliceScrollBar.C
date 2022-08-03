// $Id: SliceScrollBar.C,v 1.3.2.2 2010/04/09 02:26:12 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SliceScrollBar.C
// Description: 
// Created: 
// Author(s): Jeffrey Meade  jmeade@viatronix.net
//

#include "stdafx.h"
#include "SliceScrollBar.h"
#include <winuser.h>

using namespace WinUtils;

unsigned int SliceScrollBar::m_uScrollMessageID(0);

//////////////////////////////////////////////////////////////////////
// SliceScrollBar Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * constructor
 */
SliceScrollBar::SliceScrollBar() : m_uParentViewID(0)
{
  // no code
} // constructor


/**
 * destructor
 */
SliceScrollBar::~SliceScrollBar()
{
  // no cods
} // destructor


/**
 * message map
 */
BEGIN_MESSAGE_MAP(SliceScrollBar, CScrollBar)
  //{{AFX_MSG_MAP(SliceScrollBar)
  ON_WM_HSCROLL_REFLECT()
  ON_WM_VSCROLL_REFLECT()
  ON_WM_CREATE()
  ON_WM_SETCURSOR()
  ON_WM_SHOWWINDOW()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SliceScrollBar creation

/**
 * create method
 * @param lpCreateStruct   create parameters
 * @return                 success/failure
 */
int SliceScrollBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CScrollBar::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }
  DWORD dwStyle = (lpCreateStruct->style&WS_VISIBLE) | WS_CHILD;
  CWnd * pParent = CWnd::FromHandle(lpCreateStruct->hwndParent);
  m_myRealParent.Create(LPCTSTR(NULL), dwStyle, CRect(), pParent, GetDlgCtrlID());
  this->SetParent(&m_myRealParent);

  // TODO: Add your specialized creation code here
  SetScrollRange(0, 100);
  return 0;
} // OnCreate()

/////////////////////////////////////////////////////////////////////////////
// SliceScrollBar operations

 /**
 * move the control
 * @param x          move destination x
 * @param y          move destination y
 * @param iWidth     new width
 * @param iHeight    new height
 * @param bRepaint   whether to paint the window after move
 */
void SliceScrollBar::MoveWindow(int x, int y, int iWidth, int iHeight, BOOL bRepaint)
{
  m_myRealParent.MoveWindow(x, y, iWidth, iHeight, bRepaint);
  CScrollBar::MoveWindow(0, 0, iWidth, iHeight, bRepaint);
} // MoveWindow()


/**
 * move the control
 * @param lpRect     move destination
 * @param bRepaint   whether to paint the window after move
 */
void SliceScrollBar::MoveWindow(LPCRECT lpRect, BOOL bRepaint)
{
  MoveWindow(lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint);
} // MoveWindow()

/////////////////////////////////////////////////////////////////////////////
// SliceScrollBar message handlers


/**
 * horizontal scrollbar msg
 * @param uSBCode   scroll type
 * @param uPos      position of scrollbar
 */
void SliceScrollBar::HScroll(UINT uSBCode, UINT uPos)
{
  // TODO: Add your message handler code here
  switch (uSBCode)
  {
    case SB_LEFT:
    case SB_LINELEFT:
      uPos = max(GetScrollPos() - 1, 0);
      break;


    case SB_RIGHT:
    case SB_LINERIGHT:
      uPos = GetScrollPos() + 1;
      break;


    case SB_PAGELEFT:
      uPos = max(GetScrollPos() - 10, 0);
      break;


    case SB_PAGERIGHT:
      uPos = GetScrollPos() + 10;
      break;


    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
      // only case where uPos is already set
      break;

    case SB_ENDSCROLL:
    default:
      // Don't do anything for remaining codes
      return;
  }

  SetScrollPos(uPos);
  m_myRealParent.GetParent()->PostMessage(m_uScrollMessageID, MAKEWPARAM(m_uParentViewID, WM_HSCROLL), MAKELPARAM(uPos, uSBCode));
} // OnHScroll()


/**
 * vertical scrollbar msg
 * @param uSBCode   scroll type
 * @param uPos      position of scrollbar
 */
void SliceScrollBar::VScroll(UINT uSBCode, UINT uPos)
{
  // TODO: Add your message handler code here
  // Same code for both
  HScroll(uSBCode, uPos);
} // VScroll()


/**
 * WM_SETCURSOR msg
 *
 * @param pWnd       pointer to this window
 * @param uHitTest   identifies where the cursor is
 * @param uMessage   message that triggered set cursor msg
 */
BOOL SliceScrollBar::OnSetCursor(CWnd * pWnd, UINT uHitTest, UINT uMessage) 
{
  // TODO: Add your message handler code here and/or call default

//  SetCursor(LoadCursor(NULL, IDC_HAND));
  SetCursor(LoadCursor(NULL, IDC_ARROW));
  return TRUE;
} // OnSetCursor(CWnd * pWnd, UINT uHitTest, UINT uMessage)


void SliceScrollBar::OnShowWindow(BOOL bShow, UINT uStatus)
{
  m_myRealParent.ShowWindow(bShow==TRUE ? SW_SHOW : SW_HIDE);
} // OnShowWindow


// Revision History:
// $Log: SliceScrollBar.C,v $
// Revision 1.3.2.2  2010/04/09 02:26:12  dongqing
// hide control when the style bit is set accordingly
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.3.2.1  2009/11/27 19:59:30  dongqing
// scroll bars
//
// Revision 1.3  2007/03/09 01:11:49  jmeade
// code standards.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.6.16.1.2.1  2005/07/08 18:04:14  jmeade
// Issue 4256: prevent unsigned int out-of-range (for negative values).
//
// Revision 3.6.16.1  2005/06/14 22:13:59  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 3.6.22.1  2005/06/14 17:44:43  jmeade
// pass scrollbar code as part of WM_HSCROLL message from slicescrollbar.
//
// Revision 3.6  2002/06/04 18:37:00  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.5.2.1  2002/05/07 22:39:28  jmeade
// Issue 2046:  Reverting to hidden-CScrollBar derived method.  (Generic looks, proper operation.)
//
// Revision 3.5  2001/12/06 20:33:06  binli
// adjustment:
// 1> m_iScrollMax = sliceNum: avoid flahing problem.
// 2> one press (down) on left/right arrow: scroll pos adjusted once at least.
// 3> pagesize ~ slicenum.
//
// Revision 3.4  2001/11/23 15:06:56  jmeade
// Use a separate parent view identifier to ssend scroll messages.
//
// Revision 3.3  2001/11/20 04:24:40  jmeade
// Complete first-cut of scrollbar reimplement without MFC scrollbar; Corrected a floating point calculation
//
// Revision 3.2  2001/11/20 02:59:12  jmeade
// More picturesque bitmapped scrollbars.
//
// Revision 3.1  2001/11/09 01:17:30  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/11/06 17:12:50  jmeade
// Removed message dependency on Viatron project
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:00   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:46   ingmar
// Initial revision.
// Revision 1.8  2001/05/11 22:46:59  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SliceScrollBar.C,v 1.3.2.2 2010/04/09 02:26:12 dongqing Exp $
// $Id: SliceScrollBar.C,v 1.3.2.2 2010/04/09 02:26:12 dongqing Exp $
