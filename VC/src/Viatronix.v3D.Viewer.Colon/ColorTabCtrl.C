// $Id: ColorTabCtrl.C,v 1.2 2005/08/16 21:53:47 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ColorTabCtrl.C
// Author(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "ColorTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ColorTabCtrl

using namespace WinUtils;

ColorTabCtrl::ColorTabCtrl()
{
}

ColorTabCtrl::~ColorTabCtrl()
{
}


BEGIN_MESSAGE_MAP(ColorTabCtrl, CTabCtrl)
  //{{AFX_MSG_MAP(ColorTabCtrl)
  ON_NOTIFY_REFLECT_EX(TCN_SELCHANGING, OnSelChangingEx)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ColorTabCtrl message handlers

BOOL ColorTabCtrl::SubclassDlgItem(UINT uID, CWnd * pParent)
{
  if (!CTabCtrl::SubclassDlgItem(uID, pParent))
    return FALSE;

  ModifyStyle(0, TCS_OWNERDRAWFIXED);

  // If first tab is disabled, go to next enabled tab
  if (!IsItemEnabled(0))
  {
    int iTab = NextEnabledTab(0, true);
    CTabCtrl::SetCurSel(iTab);
  }
  return TRUE;
}

int ColorTabCtrl::InsertItem(int iItem, LPCTSTR sItem, const bool bEnabled)
{
  iItem = CTabCtrl::InsertItem(iItem, sItem);
  EnableItem(iItem, bEnabled);
  return iItem;
}

BOOL ColorTabCtrl::SetItem(int iItem, LPCTSTR sItem, const bool bEnabled)
{
  TCITEM tci;
  tci.mask = TCIF_TEXT;
  tci.pszText = const_cast<LPTSTR>(sItem);
  return CTabCtrl::SetItem(iItem, &tci); 
}

bool ColorTabCtrl::EnableItem(const int iTab, const bool bEnable)
{
#ifdef _DEBUG
  if ( (iTab < 0) || (iTab >= GetItemCount()) )   return false;
#endif
  TCITEM tcitem;
  tcitem.mask = TCIF_PARAM;
  tcitem.lParam = bEnable ? 1 : 0;
  RedrawWindow();
  return (CTabCtrl::SetItem(iTab, &tcitem) != FALSE);
}

bool ColorTabCtrl::IsItemEnabled(const int iTab)
{
#ifdef _DEBUG
  if ( (iTab < 0) || (iTab >= GetItemCount()) )   return false;
#endif
  TCITEM tcitem;
  tcitem.mask = TCIF_PARAM;
  GetItem(iTab, &tcitem);
  return (tcitem.lParam != 0);
}

int ColorTabCtrl::NextEnabledTab(const int iCurrentTab, const bool bWrap)
{
  const int uNumTabs = GetItemCount();
  if (iCurrentTab >= uNumTabs)  return -1;

  for (int iTab = iCurrentTab+1; iTab != iCurrentTab; iTab++)
  {
    if (iTab >= uNumTabs)
    {
      if (!bWrap)
        return -1;
      iTab = 0;
    }

    if (IsItemEnabled(iTab))
    {
      return iTab;
    }
  }
  return -1;
}

int ColorTabCtrl::PrevEnabledTab(const int iCurrentTab, const bool bWrap)
{
  for (int iTab = iCurrentTab-1; iTab != iCurrentTab; iTab--)
  {
    if (iTab < 0)
    {
      if (!bWrap)
        return -1;
      iTab = GetItemCount() - 1;
    }
    if (IsItemEnabled(iTab))
    {
      return iTab;
    }
  }
  return -1;
}

int ColorTabCtrl::SetCurSelWithSelChangingMsg(int iNewTab)
{
  // send the parent TCN_SELCHANGING
  NMHDR nmh;
  nmh.hwndFrom = m_hWnd;
  nmh.idFrom = GetDlgCtrlID();
  nmh.code = TCN_SELCHANGING;

  if (GetParent()->SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh) >=0)
  {
    // OK to change: set the new tab
    CTabCtrl::SetCurSel(iNewTab);

    // send parent TCN_SELCHANGE
    nmh.code = TCN_SELCHANGE;
    GetParent()->SendMessage(WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
    return TRUE;
  }
  return FALSE;
}


int ColorTabCtrl::GetItemAtCursorPos()
{
  TC_HITTESTINFO htinfo;
  GetCursorPos(&htinfo.pt);
  ScreenToClient(&htinfo.pt);
  return HitTest(&htinfo);
}


BOOL ColorTabCtrl::OnSelChangingEx(NMHDR * pNMHDR, LRESULT * pResult)
{
  // Figure out index of new tab we are about to go to, as opposed to the
  // current one we're at. Believe it or not, Windows doesn't pass this info!!!!

  const int iNewTabSel(GetItemAtCursorPos());

  if ( (iNewTabSel >= 0) && !IsItemEnabled(iNewTabSel))
  {
    *pResult = TRUE; // tab disabled: prevent selection
    return TRUE;    // Do not allow parent to handle as the tab is disabled
  }

  return FALSE;  // Allow the parent to handle the message
}

BOOL ColorTabCtrl::PreTranslateMessage(MSG* pMsg)
{
  if ( (pMsg->message == WM_KEYDOWN) && ((pMsg->wParam == VK_LEFT) || (pMsg->wParam == VK_RIGHT)) )
  {
    const int iNewTab((pMsg->wParam == VK_LEFT) ? PrevEnabledTab(GetCurSel(), FALSE) : NextEnabledTab(GetCurSel(), FALSE));
    if (iNewTab >= 0)
    {
      SetCurSelWithSelChangingMsg(iNewTab);
    }
    return TRUE;
  }
  return CTabCtrl::PreTranslateMessage(pMsg);
}

void ColorTabCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  DRAWITEMSTRUCT & ds = *lpDrawItemStruct;

  const int iItem = ds.itemID;

  // Get tab item info
  char sText[128];
  TCITEM tci;
  tci.mask = TCIF_TEXT;
  tci.pszText = sText;
  tci.cchTextMax = sizeof(sText);
  GetItem(iItem, &tci);

  // use draw item DC
  CDC & dc = *CDC::FromHandle(ds.hDC);

  // calculate text rectangle and color
  CRect rect = ds.rcItem;
  rect += CPoint(0, 5);

  if (GetCurSel() == iItem)
  {
    CRect itemRect; GetItemRect(iItem, itemRect);
    dc.FillSolidRect(itemRect, GetSysColor(COLOR_HIGHLIGHT));

    dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
    dc.DrawText(sText, &(rect + CPoint(1,3)), DT_CENTER|DT_VCENTER);
  }
  else
  {
    // draw the text
    OnDrawText(dc, rect, sText, IsItemEnabled(iItem));
  }
}

void ColorTabCtrl::OnDrawText(CDC & dc, CRect rect, const CString & sText, const bool bEnabled)
{
  dc.SetTextColor(GetSysColor(bEnabled ? COLOR_BTNTEXT : COLOR_3DHILIGHT));
  dc.DrawText(sText, &rect, DT_CENTER|DT_VCENTER);

  if (!bEnabled)
  {
    // disabled: draw again shifted northwest for shadow effect
    rect += CPoint(-1,-1);
    dc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
    dc.DrawText(sText, &rect, DT_CENTER|DT_VCENTER);
  }
}

BOOL ColorTabCtrl::TranslatePropSheetMsg(MSG * pMsg)
{
  WPARAM key = pMsg->wParam;
  if (pMsg->message == WM_KEYDOWN && GetAsyncKeyState(VK_CONTROL) < 0 &&
    (key == VK_TAB || key == VK_PRIOR || key == VK_NEXT))
  {
    int iNewTab = (key==VK_PRIOR || GetAsyncKeyState(VK_SHIFT) < 0) ?
      PrevEnabledTab(GetCurSel(), TRUE) : NextEnabledTab(GetCurSel(), TRUE);
    
    if (iNewTab >= 0)
    {
      SetCurSelWithSelChangingMsg(iNewTab);
    }
    return TRUE;
  }
  return FALSE;
}

// Revision History:
// $Log: ColorTabCtrl.C,v $
// Revision 1.2  2005/08/16 21:53:47  jmeade
// namespaces for WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.2  2002/01/14 20:40:07  jmeade
// "Overridden" SetCurSel() function should not send selchange message, which
//  would be in direct contrast to base CTabCtrl class.
//
// Revision 3.1  2001/11/09 01:09:02  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/11/06 23:30:03  jmeade
// Allow parent to also handle TCN_SELCHANGING; Method to get item at current cursor position.
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:50   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:36   ingmar
// Initial revision.
// Revision 1.1  2001/05/23 16:53:01  jmeade
// Color tab control, replete with enabled OR DISABLED tabs!!!!!
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ColorTabCtrl.C,v 1.2 2005/08/16 21:53:47 jmeade Exp $
// $Id: ColorTabCtrl.C,v 1.2 2005/08/16 21:53:47 jmeade Exp $
