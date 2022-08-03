// $Id: SlidingPaneView.C,v 1.9.2.2 2010/04/18 18:35:11 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SlidingPaneView.C
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "SlidingPaneView.h"
#include "resource.h"
#include "Theme.h"

using namespace ColonViews::Panes;
using namespace WinUtils;

#define FILE_REVISION "$Revision: 1.9.2.2 $"

/////////////////////////////////////////////////////////////////////////////
// PaneHeader class

/**
 * Constructor 
 *
 * @param hWndChild   Handle to child window (the pane).
 */
PaneHeader::PaneHeader(HWND hWndChild) : m_hPaneChild(hWndChild), m_hPaneManager(NULL)
{
} // constructor

BEGIN_MESSAGE_MAP(PaneHeader, CButton)
  //{{AFX_MSG_MAP(PaneHeader)
  ON_WM_LBUTTONDOWN()
  ON_WM_MOVE()
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_ERASEBKGND()
  //}}AFX_MSG_MAP
  ON_MESSAGE(PaneHeader::WM_USER_SHOWPANE, OnMessageShowPane)
  ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PaneHeader message handlers


/**
 * WM_CREATE handler.
 *
 * @param lpCreateStruct   Creation parameters.
 * @return                 0 if successful, non-zero otherwise.
 */
int PaneHeader::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CButton::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }

  ModifyStyle(0, BS_OWNERDRAW, 0);

  CClientDC dc(this);
  m_paneHdrBmpDC_Open.Reset();
  m_paneHdrBmpDC_Open.m_bmp.LoadBitmap(IDB_SLIDINGPANEHEADER_UP);
  m_paneHdrBmpDC_Open.CreateBmpDC(&dc);

  m_paneHdrBmpDC_Closed.Reset();
  m_paneHdrBmpDC_Closed.m_bmp.LoadBitmap(IDB_SLIDINGPANEHEADER_DN);
  m_paneHdrBmpDC_Closed.CreateBmpDC(&dc);

  m_tmrUpdateTitle.SetTimer(this, 1000);

  return 0;
} // OnCreate()


/**
 * validate when background is erased
 * @param pdc   device context for view
 * @return      TRUE to erase background
 **/
BOOL PaneHeader::OnEraseBkgnd( CDC * pdc ) 
{
  return TRUE;
} // OnEraseBkgnd()


/**
 * Updates the window title as appropriate
 * 
 * @param uIDEvent   Event ID
 */
void PaneHeader::OnTimer(UINT_PTR uIDEvent) 
{
  // Quick fix for keeping window title up-to-date
  if (uIDEvent == m_tmrUpdateTitle)
  {
    if (IsWindow(m_hPaneChild))
    {
      CWnd * pW = CWnd::FromHandle(m_hPaneChild);
      if (pW)
      {
        CString s;        pW->GetWindowText(s);
        CString sCurr;    this->GetWindowText(sCurr);
        if ( sCurr != s )
          this->Invalidate(FALSE);
      }
    }
  }

  CButton::OnTimer(uIDEvent);
} // OnTimer()


/**
 * PaneHeader::WM_USER_SHOWPANE: Handles the message to show or hide the pane
 *
 * @param wShowOrHide   1 to show; 0 to hide
 * @return              always 1
 */
LRESULT PaneHeader::OnMessageShowPane(WPARAM wShowOrHide, LPARAM)
{
  CClientDC dc(this);
  ShowChild(wShowOrHide != 0);
  ::PostMessage(GetManager(), WM_NCLBUTTONDBLCLK, WPARAM(HWND(*this)), MAKELPARAM(0, 0));
  return 1;
} // OnMessageShowPane()


/**
 * Draws the item.
 *
 * @param lpDrawItemStruct   Draw parameters.
 */
void PaneHeader::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  CRect rect;
  GetClientRect(rect);

  BitmapDC& paneHdrBDC = 1||IsChildVisible() ? m_paneHdrBmpDC_Open : m_paneHdrBmpDC_Closed;
  m_bmpDC.m_dc.SetStretchBltMode(HALFTONE);
  
  FillRect( lpDrawItemStruct->hDC, &rect, static_cast< HBRUSH >( *Theme::DefaultBackgroundBrush ) );
  
  const bool bSuccess(m_bmpDC.m_dc.StretchBlt(0,0,rect.Width(),rect.Height(),
    &paneHdrBDC.m_dc, 0,0, paneHdrBDC.m_info.bmWidth, paneHdrBDC.m_info.bmHeight,  SRCCOPY) == TRUE);

  if (IsWindow(m_hPaneChild))
  {
    CWnd * pW = CWnd::FromHandle(m_hPaneChild);
    if (pW)
    {
      CString s;
      pW->GetWindowText(s);
      this->SetWindowText(s);

      CFont * pFont = CFont::FromHandle(HFONT(::GetCurrentObject(lpDrawItemStruct->hDC, OBJ_FONT)));
      if (pFont) { m_bmpDC.m_dc.SelectObject(pFont); }

      m_bmpDC.m_dc.SetBkMode(TRANSPARENT);
      m_bmpDC.m_dc.DrawText(s, CRect(rect.left+10,rect.top+3,rect.right-10,rect.bottom-1), DT_LEFT | DT_VCENTER);
    }
  }

  TransparentBlt(lpDrawItemStruct->hDC,rect.left,rect.top,rect.Width(),rect.Height(),
    m_bmpDC.m_dc,rect.left,rect.top,rect.Width(),rect.Height(),m_bmpDC.m_dc.GetPixel(0,0));
} // DrawItem()


/**
 * WM_SIZE handler, initializes draw structures.
 *
 * @param uType   Identifies size type.
 * @param iCX     Width.
 * @param iCY     Height.
 */
void PaneHeader::OnSize(UINT uType, int iCX, int iCY) 
{
  CButton::OnSize(uType, iCX, iCY);
  m_bmpDC.Reset();
  CClientDC dc(this);
  m_bmpDC.m_bmp.CreateCompatibleBitmap(&dc, iCX, iCY);
  m_bmpDC.CreateBmpDC(&dc);
} // OnSize()


/**
 * WM_MOVE handler.
 *
 * @param x   Horizontal position.
 * @param y   Vertical position.
 */
void PaneHeader::OnMove(int iX, int iY) 
{
  CButton::OnMove(iX, iY);
  if (IsWindow(m_hPaneChild))
  {
    CRect rect;
    GetClientRect(rect);
    ::SetWindowPos(m_hPaneChild, NULL, iX, iY+rect.Height(), 0,0, SWP_NOSIZE | SWP_NOZORDER);

    if (IsChildVisible())
    {
      CWnd * pW = CWnd::FromHandle(m_hPaneChild);
      pW->Invalidate();
    }
  }
} // OnMove()


/**
 * WM_LBUTTONDOWN handler, indicates toggling of child visibility state.
 *
 * @param uFlags   Mouse key flags.
 * @param point    Point of button down.
 */
void PaneHeader::OnLButtonDown(UINT uFlags, CPoint point) 
{
  OnMessageShowPane(WPARAM(IsChildVisible() ? 0 : 1), 0);
  CButton::OnLButtonDown(uFlags, point);
  GetParent()->SetFocus();
} // OnLButtonDown()


/**
 * Shows or hides pane child.
 *
 * @param bShow   Whether to show or hide the pane child.
 */
void PaneHeader::ShowChild(const bool bShow) const
{
  if (IsWindow(m_hPaneChild))
  {
    ::ShowWindow(m_hPaneChild, bShow ? SW_SHOW : SW_HIDE);
  }

  CWnd *pWnd = CWnd::FromHandle(m_hPaneChild);
  if (pWnd)
  {
    pWnd->Invalidate(FALSE);
  }
} // ShowChild()


/**
 * Gets window rect of entire pane (pane header plus visible child rect).
 *
 * @return CRect  Window rect of pane header plus visible child rect.
 */
CRect PaneHeader::GetPaneWindowRect() const
{
  CRect winRect;
  GetWindowRect(winRect);
  if (IsChildVisible())
  {
    CRect childRect;
    ::GetClientRect(m_hPaneChild, childRect);
    winRect.bottom += childRect.Height();
  }

  return winRect;
} // GetPaneWindowRect()


/////////////////////////////////////////////////////////////////////////////
// SlidingPaneManager class

BEGIN_MESSAGE_MAP(SlidingPaneManager, CButton)
  //{{AFX_MSG_MAP(SlidingPaneManager)
  ON_WM_NCLBUTTONDBLCLK()
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * WM_SIZE handler.
 *
 * @param uType   Resize event identifier.
 * @param iCX     Width of window.
 * @param iCY     Height of window.
 */
void SlidingPaneManager::OnSize(UINT uType, int iCX, int iCY) 
{
  CButton::OnSize(uType, iCX, iCY);
  StackPanes();
} // OnSize()


/**
 * Handles non-client area double-click; Primarily handles non-client double click passed from (pane header) children windows.
 *
 * @param uHitTest   Hit test code if message generated by window; Handle to child window if message passed from child.
 * @param point      Point of double-click.
 */
void SlidingPaneManager::OnNcLButtonDblClk(UINT uHitTest, CPoint point) 
{
  // If this message is from a v3D control, the HIWORD of uHitTest will identify the control
  CWnd * pFromWnd = CWnd::FromHandlePermanent(HWND(uHitTest));
  if (!pFromWnd)
  {
    CButton::OnNcLButtonDblClk(uHitTest, point);
    return;
  }

  m_mapLastActive[HWND(*pFromWnd)] = COleDateTime::GetCurrentTime();

  PaneInfoVector vPanes(GetPanesInfo());
  if (!vPanes.empty())
  {
    CRect parentClientRect; GetClientRect(parentClientRect);
    CSize paneSizeSum(0,0);

    // Determine vertical real estate used
    PaneInfoVector::iterator vIt;
    for (vIt = vPanes.begin(); vIt != vPanes.end(); vIt++)
    {
      CRect paneRect = dynamic_cast<PaneHeader*>(CWnd::FromHandle(vIt->GetPaneHeader()))->GetPaneWindowRect();
      paneSizeSum.cx += paneRect.Width();
      paneSizeSum.cy += paneRect.Height();
    }

    // Close panes, by least recently active times, to conserve real estate
    if (paneSizeSum.cy > parentClientRect.Height())
    {
      // panes sort by time
      qsort(&vPanes[0], vPanes.size(), sizeof(vPanes[0]), ComparePaneInfo_Time);
      PaneInfoVector::iterator vIt;
      for (vIt = vPanes.begin(); (paneSizeSum.cy > parentClientRect.Height()) && (vIt != vPanes.end()); vIt++)
      {
        PaneHeader & hdr = *(dynamic_cast<PaneHeader*>(CWnd::FromHandle(vIt->GetPaneHeader())));
        if ( (HWND(*pFromWnd) != HWND(hdr)) && (hdr.IsChildVisible()) )
        {
          paneSizeSum.cy -= hdr.GetPaneWindowRect().Height();
          hdr.ShowChild(false);
          paneSizeSum.cy += hdr.GetPaneWindowRect().Height();
        }
      }
    }
  }  
  
  StackPanes();
} // OnNcLButtonDblClk()


/**
 * Sort compare function, for ascending sort of y-position of pointers to two PaneInfo instances.
 *
 * @param pVoid1   Void pointer to PaneInfo.
 * @param pVoid2   Void pointer to PaneInfo.
 * @return         < 0 if first PaneHeader at lower y-pos, 0 if at same y-pos, > 0 otherwise.
 */
int SlidingPaneManager::ComparePaneInfo_HeaderYPos(const void *pVoid1, const void *pVoid2)
{
  CRect rect1, rect2;
  dynamic_cast<PaneHeader*>(CWnd::FromHandle(((PaneInfo*)pVoid1)->GetPaneHeader()))->GetWindowRect(rect1);
  dynamic_cast<PaneHeader*>(CWnd::FromHandle(((PaneInfo*)pVoid2)->GetPaneHeader()))->GetWindowRect(rect2);
  return rect1.top - rect2.top;
} // ComparePaneInfo_HeaderYPos()


/**
 * Sort compare function, for ascending last access time of pointers to two PaneInfo instances.
 *
 * @param pVoid1   Void pointer to PaneInfo.
 * @param pVoid2   Void pointer to PaneInfo.
 * @return         < 0 if first PaneHeader at earlier access time, 0 if same times, > 0 otherwise.
 */
int SlidingPaneManager::ComparePaneInfo_Time(const void *pVoid1, const void *pVoid2)
{
  const COleDateTime t1((((PaneInfo*)pVoid1))->GetAccessTime());
  const COleDateTime t2((((PaneInfo*)pVoid2))->GetAccessTime());
  return ((t1 < t2) ? (-1) : (t1 == t2 ? 0 : 1));
} // ComparePaneInfo_Time()


/**
 * Adds a pane to be managed
 *
 * @param hPaneWnd   Window handle to pane to be managed.
 */
void SlidingPaneManager::AddPane(HWND hPaneWnd)
{
  m_paneList.push_back(hPaneWnd);
} // AddPane()


/**
 * Returns info on each of the panes.
 *
 * @return   Vector of info on each pane.
 */
SlidingPaneManager::PaneInfoVector SlidingPaneManager::GetPanesInfo() const
{
  PaneInfoVector vPanes(m_paneList.size());
  uint4 uCurrPane(0);
  for (HWNDList::const_iterator lIt = m_paneList.begin(); lIt != m_paneList.end(); lIt++)
  {
    vPanes[uCurrPane++] = PaneInfo(*lIt, m_mapLastActive.GetTime(*lIt));
  }
  return vPanes;
} // PaneInfoVector()


/**
 * Moves the panes to a stacked configuration
 *
 * @param pActivePaneWnd   Active pane window.
 */
void SlidingPaneManager::StackPanes()
{
  // Find all v3D panes
  PaneInfoVector vPanes(GetPanesInfo());
  if (vPanes.empty())
  {
    return;
  }

  // Sort the controls by y-position
  qsort(&vPanes[0], vPanes.size(), sizeof(vPanes[0]), ComparePaneInfo_HeaderYPos);

  CRect mgrRect;
  GetWindowRect(mgrRect);
  GetParent()->ScreenToClient(mgrRect);

  // Stack the controls
  CRect stackRect;
  int4 iLastYPos(mgrRect.top + 1);
  for (PaneInfoVector::iterator vIt = vPanes.begin(); vIt != vPanes.end(); vIt++)
  {
    PaneHeader & pHdr = *(dynamic_cast<PaneHeader*>(CWnd::FromHandle(vIt->GetPaneHeader())));
    stackRect = pHdr.GetPaneWindowRect();
    GetParent()->ScreenToClient(stackRect);
    pHdr.SetWindowPos(NULL, stackRect.left, iLastYPos, 0,0, SWP_NOSIZE | SWP_NOZORDER);
    iLastYPos += stackRect.Height() + 2;

    pHdr.Invalidate(TRUE);
  }

  Invalidate(TRUE);
} // StackPanes()


/**
 * Shows or hides the pane corresponding to the pane header with the specified control ID.
 *
 * @param uPaneHeaderControlID   Control ID for the pane header.
 * @param bShow                  Whether to show or hide the pane.
 */
void SlidingPaneManager::ShowPane(const uint4 uPaneHeaderControlID, const bool bShow)
{
  GetParent()->GetDlgItem(uPaneHeaderControlID)->PostMessage(PaneHeader::WM_USER_SHOWPANE, bShow ? 1 : 0, 0);
} // ShowPane()


/**
 * Returns true if the pane corresponding to the given pane header is visible, false otherwise
 *
 * @param uPaneHeaderControlID   Control ID for the pane header.
 * @return                       whether or not the pane is visible.
 */
bool SlidingPaneManager::IsPaneVisible(const uint4 uPaneHeaderControlID)
{
  return dynamic_cast<PaneHeader*>(GetParent()->GetDlgItem(uPaneHeaderControlID))->IsChildVisible();
} // IsPaneVisible()


/**
 * Returns true if the pane corresponding to the given pane header is visible, false otherwise
 *
 * @param uPaneHeaderControlID   Control ID for the pane header.
 */
void SlidingPaneManager::SetPaneText(const uint4 uPaneHeaderControlID, const std::string& sText)
{
  PaneHeader * pPane = dynamic_cast<PaneHeader*>(GetParent()->GetDlgItem(uPaneHeaderControlID));
  ::SetWindowText(pPane->m_hPaneChild, sText.c_str());
  pPane->Invalidate(FALSE);
} // SetPaneText()



// Revision History:
// $Log: SlidingPaneView.C,v $
// Revision 1.9.2.2  2010/04/18 18:35:11  dongqing
// timer to keep pane title updated
//
// Revision 1.9.2.1  2010/04/10 04:33:58  dongqing
// screen/client calculations must be based on the parent
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9  2007/03/10 05:06:25  jmeade
// code standards.
//
// Revision 1.8  2007/03/09 01:11:49  jmeade
// code standards.
//
// Revision 1.7  2006/02/07 21:40:28  dongqing
// code walk through
//
// Revision 1.6  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.5.2.2  2006/01/11 21:24:48  frank
// undid typo
//
// Revision 1.5.2.1  2006/01/11 21:03:33  geconomos
// filling in background of button with theme back color
//
// Revision 1.5  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.4  2005/10/05 22:59:52  jmeade
// comment.
//
// Revision 1.3  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.19.2.6.2.2  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.19.2.6.2.1  2005/04/22 03:24:49  jmeade
// Issue 4128: Bounce focus back to parent after mouse click.
//
// Revision 1.19.2.6  2005/02/15 19:58:46  jmeade
// merge from vc1.4 branch.
//
// Revision 1.19.2.5.2.2  2004/12/22 21:58:56  jmeade
// post visible msg. when opening child window.
//
// Revision 1.19.2.5.2.1  2004/11/17 21:57:24  jmeade
// Separate W/L for pre and post-contrast studies.
//
// Revision 1.19.2.5  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 1.19.2.4  2004/03/03 21:29:28  jmeade
// no message
//
// Revision 1.19.2.3  2003/05/05 19:40:02  geconomos
// Code walkthrough.
//
// Revision 1.19.2.2  2003/03/01 02:12:16  jmeade
// Cleanup on sliding pane header and notations.
//
// Revision 1.19.2.1  2003/02/22 17:58:56  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 1.19  2002/11/28 01:13:11  jmeade
// Draw with transparency to blend in pane header; erase bkgd to remove artifacts from moving window.
//
// Revision 1.18  2002/11/26 23:55:36  jmeade
// Cleaner sliding panes implementation with panes not embedded in another view, pane manager.
//
// Revision 1.17  2002/11/25 23:26:02  jmeade
// Is visible, set text pane convenience functions.
//
// Revision 1.16  2002/11/21 22:54:45  jmeade
// Don't go thru list, use GetDlgItem.
//
// Revision 1.15  2002/11/15 00:38:23  jmeade
// No hard-coding of # of child views.
//
// Revision 1.14  2002/09/09 21:14:43  jmeade
// Post message for Show/hide pane functionality, for proper handling show/update events.
//
// Revision 1.13  2002/09/06 20:15:51  jmeade
// Code for up arrow/down arrow pane header.
//
// Revision 1.12  2002/08/29 22:17:23  jmeade
// Force refresh for each pane header after moving.
//
// Revision 1.11  2002/08/29 19:57:07  jmeade
// Close panes, by least recently active times, to conserve real estate.
//
// Revision 1.10  2002/08/29 00:22:07  jmeade
// Image for sliding pane header; Comments.
//
// Revision 1.9  2002/08/28 20:03:01  jmeade
// Pane header beautifying; Unused code removal.
//
// Revision 1.8  2002/08/28 16:13:33  jmeade
// User control for sliding pane header.
//
// Revision 1.7  2002/08/27 18:27:28  jmeade
// Sliding pane incremental checkin; CWnd ptr list for controls.
//
// Revision 1.6  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 1.4  2002/08/13 18:50:01  jmeade
// Don't crash on an empty list of controls.
//
// Revision 1.3  2002/07/29 17:08:27  jmeade
// Enabling resize handlers; Ensuring a pane will be chosen on first close-up (only the active pane's last access time stored).
//
// Revision 1.2  2002/07/26 22:47:04  jmeade
// Commenting out resize handlers until I determine cause of the ViatronChildWnd crash on pane hide;
// Tabs to spaces.
//
// Revision 1.1  2002/07/26 22:31:23  jmeade
// Sliding pane view.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SlidingPaneView.C,v 1.9.2.2 2010/04/18 18:35:11 dongqing Exp $
// $Id: SlidingPaneView.C,v 1.9.2.2 2010/04/18 18:35:11 dongqing Exp $
