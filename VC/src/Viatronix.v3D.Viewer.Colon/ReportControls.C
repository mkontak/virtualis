// $Id: ReportControls.C,v 1.3 2007/03/10 05:06:25 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Owner draw list control
// Author: George Economos

#include "stdafx.h"
#include "ReportControls.h"
#include "WinUtils.h"
#include "resource.h"
#include "ExeResourceSwapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ColonReport;

BEGIN_MESSAGE_MAP(ReportListCtrl, CListCtrl)
  //{{AFX_MSG_MAP(ReportListCtrl)
  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_CREATE()
  ON_WM_DESTROY()
  ON_WM_TIMER()
  ON_WM_RBUTTONUP()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

const uint4 TIMER_EVENT = 1000;
const uint4 TIMER_INTERVAL = 250;

/**
 * constructor
 **/
ReportListCtrl::ReportListCtrl() : 
 m_szOffset(10,0),
 m_iTrackItem(-1),
 m_bItemDown(FALSE)
{
} // constructor


/**
 * initializations
 **/
void ReportListCtrl::PreSubclassWindow()
{
  CListCtrl::PreSubclassWindow();
 
  CommonInit(); 
} // PreSubclassWindow()


/**
 * WM_CREATE, called during the creation phase of the window.
 *
 * @param   lpCreateStruct   Info on the window being created
 * @return                   Return 0 to continue the creation of the CWnd object. If –1, the window will be destroyed.
 */
int ReportListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CListCtrl::OnCreate(lpCreateStruct) == -1)
    return -1;

  CommonInit(); 
  
  return 0;
} // OnCreate()


/**
 * Common initialization
 */
BOOL ReportListCtrl::CommonInit()
{
  /// Insert a column that is the entire width of the control
  CRect rc;
  GetClientRect(rc);
  InsertColumn(0, "", LVCFMT_LEFT, rc.Width());

  /// Start timer that is is used for updates
  SetTimer(TIMER_EVENT, TIMER_INTERVAL, NULL);

  /// Load and set image list
  HIMAGELIST himl = ImageList_Create(32, 64, ILC_COLOR8|ILC_MASK, 0, 1);
  
  // load bitmap images from the exe
  ExeResourceSwapper swapper;
  CBitmap bm;
  VERIFY(bm.LoadBitmap(IDB_RPT_LIST));
  
  ImageList_AddMasked(himl, bm, RGB(255,0,255));
  
  ListView_SetImageList(GetSafeHwnd(), himl, LVSIL_SMALL);

  return TRUE;
} // CommonInit()


/**
 * Draws the item.
 *
 * @param lpDrawItemStruct   Draw parameters.
 */
void ReportListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  CDC* pdc = CDC::FromHandle(lpDrawItemStruct->hDC);
  
  int4 iItem(lpDrawItemStruct->itemID);
  
  int4 nSavedDC = pdc->SaveDC();

  /// Get item image and state info
  LV_ITEM lvi;
  lvi.mask = LVIF_IMAGE | LVIF_STATE;
  lvi.iItem = iItem;
  lvi.iSubItem = 0;
  lvi.stateMask = 0xFFFF;
  GetItem(&lvi);
  
  /// reteieve bounding rectangle for all aspects of list item
  CRect rcBounds, rcLabel, rcIcon, rcItem(lpDrawItemStruct->rcItem);
 
  GetItemRect(iItem, rcBounds, LVIR_BOUNDS);
  GetItemRect(iItem, rcLabel, LVIR_LABEL);
  GetItemRect(iItem, rcIcon, LVIR_ICON);

  /// offset all rectangles
  CSize szOffset(m_szOffset.cx, m_szOffset.cy * (iItem+1));

  rcItem.OffsetRect(szOffset);
  rcBounds.OffsetRect(szOffset);
  rcLabel.OffsetRect(szOffset);
  rcIcon.OffsetRect(szOffset);

  CString sLabel = GetItemText(iItem, 0);

  int4 offset = pdc->GetTextExtent(_T(" "), 1 ).cx*2;
 
  /// draw the image
  CRect rcClient;
  GetClientRect(rcClient); 
  GetImageList(LVSIL_SMALL)->Draw(pdc, lvi.iImage, CPoint(rcIcon.left, rcIcon.top), ILD_TRANSPARENT);

  /// draw the text
  rcLabel.left += offset/2;
  rcLabel.right = rcClient.right - m_szOffset.cx;
  
  pdc->DrawText(sLabel,-1,rcLabel,DT_LEFT|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP|DT_VCENTER|DT_END_ELLIPSIS);

  /// draw bounding box based on selection state and mouse position
  CRect rc(rcBounds);
  rc.right = rcClient.right - m_szOffset.cx - 1;
  rc.top += 10;
  rc.bottom -= 10;

  if(lvi.state & LVIS_SELECTED)
    pdc->Draw3dRect(rc, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DHIGHLIGHT));

  else if(lvi.state & LVIS_DROPHILITED)
    pdc->Draw3dRect(rc, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DHIGHLIGHT));

  else if(iItem == m_iTrackItem)
  {
    if(m_bItemDown)
      pdc->Draw3dRect(rc, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DHIGHLIGHT));
    else
      pdc->Draw3dRect(rc, GetSysColor(COLOR_3DHIGHLIGHT), GetSysColor(COLOR_3DDKSHADOW));
  } 
  
  pdc->RestoreDC(nSavedDC);
} // DrawItem()


/**
 * Right button down handler.
 *
 * @param uFlags   Virtual key flags.
 * @param point    Location of mouse click.
 */
void ReportListCtrl::OnRButtonDown(UINT uFlags, CPoint point) 
{
  ButtonDown(point);
} // OnRButtonDown()


/**
 * Left button down handler.
 *
 * @param uFlags   Virtual key flags.
 * @param point    Location of mouse click.
 */
void ReportListCtrl::OnLButtonDown(UINT uFlags, CPoint point) 
{
  ButtonDown(point);
} // OnLButtonDown()


/**
 * Mouse move handler; displays help text if available for button, tracks mouse for mouse leave event
 * 
 * @param uFlags   Control key/mouse key flags
 * @param point    Mouse move point
 */
void ReportListCtrl::OnMouseMove(UINT uFlags, CPoint point) 
{
  /// check that the mouse is over an item that is not currently selected
  int4 iItem = HitTest(point);
  if(iItem == -1 && m_iTrackItem == -1 || iItem == m_iTrackItem || GetNextItem(-1, LVNI_SELECTED) == iItem)
    return;
    
  /// update old and new items
  int4 iOldTrackItem = m_iTrackItem;
  
  m_iTrackItem = iItem;
  
  Update(iOldTrackItem);
  
  if(m_iTrackItem != -1)
  {
    m_bItemDown = (uFlags&MK_LBUTTON);
    Update(m_iTrackItem);  
  }
} // OnMouseMove()


/**
 * Left button up handler.
 *
 * @param uFlags   Virtual key flags.
 * @param point    Location of mouse click.
 */
void ReportListCtrl::OnLButtonUp(UINT uFlags, CPoint point) 
{
  ButtonUp(point);
} // OnLButtonUp()


/**
 * right button up handler.
 *
 * @param uFlags   Virtual key flags.
 * @param point    Location of mouse click.
 */
void ReportListCtrl::OnRButtonUp(UINT uFlags, CPoint point) 
{
  ButtonUp(point);
  
  /// forward the NM_RCLICK message to parent(used for context menus)
  NMHDR nmhdr;
  nmhdr.hwndFrom = m_hWnd;
  nmhdr.idFrom = ::GetDlgCtrlID(m_hWnd);
  nmhdr.code = NM_RCLICK;
  
  ::SendMessage(::GetParent(m_hWnd), WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&nmhdr));
} // OnRButtonUp()


/**
 * button down handler.
 *
 * @param point    Location of mouse click.
 */
void ReportListCtrl::ButtonDown(const CPoint& point)
{
  SetFocus();
  
  /// if there aren't any items in control return immediately
  int4 iItemCount = GetItemCount();
  if(iItemCount== 0)
    return;
  
  /// check if user clicked an item
  int4 iItem = HitTest(point);
  if(iItem == -1)
    return;
  
  /// checked that the clicked item isn't the currently selected item
  if(iItem == GetNextItem(-1, LVNI_SELECTED))
    return;
  
  /// item is selected update flags
  m_iTrackItem = iItem;  
  m_bItemDown = TRUE;
  
  /// redraw the item
  Update(iItem);  
} // ButtonDown()


/**
 * button up handler.
 *
 * @param point    Location of mouse click.
 */
void ReportListCtrl::ButtonUp(const CPoint& point)
{
  int4 iItemCount = GetItemCount();
  if(iItemCount== 0)
    return;
  
  int4 iSelectedItem = GetNextItem(-1, LVNI_SELECTED);
  
  int4 iItem = HitTest(point);
  if(iItem != -1 && iItem != iSelectedItem)
  { 
    SetItemState(iSelectedItem, 0, LVIS_SELECTED|LVIS_FOCUSED);
    Update(iSelectedItem);
    
    m_iTrackItem = -1;
    
    SetItemState(iItem, LVIS_SELECTED|LVIS_FOCUSED, 0x000F);    
  }
  else
  {
    iItem = m_iTrackItem;
    m_iTrackItem = -1;
    
    Update(iItem);    
  }
} // ButtonUp()


/**
 * WM_DESTROY handler
 */
void ReportListCtrl::OnDestroy() 
{
  KillTimer(TIMER_EVENT);
  
  CListCtrl::OnDestroy();
} // OnDestroy()


/**
 * WM_TIMER handler
 *
 * @param uIDEvent   ID of the timer event
 */
void ReportListCtrl::OnTimer(UINT_PTR uIDEvent) 
{
  /// the timer is used to handle quick movement of the mouse outside of the control
  if(uIDEvent == TIMER_EVENT)
  {
    if(m_iTrackItem != -1)
    {
      DWORD pos = GetMessagePos();
      CPoint pt(LOWORD(pos), HIWORD(pos));

      CRect rc;
      GetWindowRect(rc);
      if(!rc.PtInRect(pt))
      {
        int4 iItem = m_iTrackItem;
        m_iTrackItem = -1;
        Update(iItem);
      }
    }
    return;
  }

  CListCtrl::OnTimer(uIDEvent);
} // OnTimer()


// Revision History:
// $Log: ReportControls.C,v $
// Revision 1.3  2007/03/10 05:06:25  jmeade
// code standards.
//
// Revision 1.2  2005/09/13 17:21:07  jmeade
// ColonReport namespace.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.1  2002/04/18 18:29:20  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:40   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:32   ingmar
// Initial revision.
// Revision 1.9  2001/04/18 15:49:32  geconomos
// coding standards
//
// Revision 1.8  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.7  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportControls.C,v 1.3 2007/03/10 05:06:25 jmeade Exp $
// $Id: ReportControls.C,v 1.3 2007/03/10 05:06:25 jmeade Exp $
