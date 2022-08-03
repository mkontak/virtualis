// $Id: ReportGroupCtrl.C,v 1.4.2.2 2009/07/30 17:58:20 kchalupa Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Owner draw list control simulates radio groups and checkboxes
// Owner: George Economos

#include "stdafx.h"
#include "viatron.h"
#include "ReportGroupCtrl.h"
#include "resource.h"
#include "ExeResourceSwapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ColonReport;

/////////////////////////////////////////////////////////////////////////////
// ReportGroupCtrl

/**
 * constructor
 */
ReportGroupCtrl::ReportGroupCtrl() :
m_bBoldSelected(false),
m_bCheckBoxes(false),
m_bUseGroups(false)
{
} // ReportGroupCtrl()


/**
 * message map
 */
BEGIN_MESSAGE_MAP(ReportGroupCtrl, CListCtrl)
  //{{AFX_MSG_MAP(ReportGroupCtrl)
  ON_WM_ERASEBKGND()
  ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemchanging)
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONDBLCLK()
  ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, OnItemChanged)
  ON_WM_VSCROLL()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * gets check state of specified item
 *
 * @param uData   item to query
 * @return        true if item is checked
 */
bool ReportGroupCtrl::GetCheck(const uint4 uData) const
{
  // are we using checkboxes?
  if(!m_bCheckBoxes)
  {
    return false;
  }

  // find item by associated data
  int iItem = FindItem(uData);
  if(iItem == -1)
  {
    return false; // item not found
  }

  // we check by seeing if the item's images is the checked one
  return GetItemImage(iItem) == CHECKED;

} // GetCheck( uData )


/**
 * sets check state of specified item
 *
 * @param uData    item to set
 * @param bCheck   true if item is checked
 */
void ReportGroupCtrl::SetCheck(const uint4 uData, const bool bCheck)
{
  // are we using checkboxes?
  if(!m_bCheckBoxes)
  {
    return;
  }

  // find item by associated data
  int iItem = FindItem(uData);
  if(iItem == -1)
  {
    return;
  }

  SetItemImage(iItem, bCheck? CHECKED:UNCHECKED);
} // SetCheck( uData, bCheck )


/**
 * adds a new group
 *
 * @param iGroupID   group ID
 * @param pszName    group name
 * @return           position/count of added item
 */
int4 ReportGroupCtrl::InsertGroup(const int4 iGroupID, LPCTSTR pszName)
{
  // insert item at end of list control
  LVITEM lvi;
  lvi.mask      = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
  lvi.iItem     = GetItemCount();
  lvi.iSubItem  = 0;
  lvi.pszText   = const_cast<LPTSTR>(pszName);
  lvi.iImage    = 0;
  lvi.lParam    = iGroupID;

  return InsertItem(&lvi);
} // InsertGroup( iGroupID, pszName )


/**
 * adds a new item to specified group
 *
 * @param iGroupID   group ID
 * @param pszText    item name
 * @return           success/failure
 */
bool ReportGroupCtrl::InsertGroupItem(const int4 iGroupID, LPCTSTR pszText, const uint4 uData)
{
  LVITEM lvi;
  lvi.mask      = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM|LVIF_INDENT;
  lvi.iSubItem  = 0;
  lvi.lParam    = uData;
  lvi.pszText   = const_cast<LPTSTR>(pszText);

  // if this is a group item insert at the end of the group, otherwise insert at end of list
  lvi.iItem = iGroupID == NO_GROUP? GetItemCount() : GetNextGroupItem(iGroupID);

  // if we are using checkboxes, insert unchecked
  lvi.iImage = m_bCheckBoxes ? UNCHECKED : 1;

  // if this is a group intem ident it
  lvi.iIndent = iGroupID==NO_GROUP? 0:1;

  return Update(InsertItem(&lvi))? true:false; // get rid of annoying message
} // InsertGroupItem( iGroupID, pszText, uData )


/**
 * locates item by 32-bit data ID
 *
 * @param uDatat   item ID
 * @return         position/count of item, -1 if not found
 */
int4 ReportGroupCtrl::FindItem(const uint4 uData) const
{
  // for each item in the list
  for(int4 iItem(0); iItem < GetItemCount(); iItem++)
  {
    // return the item if the data items match
    if(GetItemData(iItem) == uData)
    {
      return iItem;
    }
  } // end for each item

  return -1; // not found
} // FindItem( uData )


/**
 * selects an item
 *
 * @param uData   item ID
 */
void ReportGroupCtrl::SelectItem(const uint4 uData)
{
  // find item by associated data
  int4 iItem = FindItem(uData);
  if(iItem == -1)
  {
    return; // not found
  }

  // get selected item and deselect it
  int4 iSelected = GetNextItem(-1, LVNI_SELECTED);
  if(iSelected != -1)
  {
    SetItemState(iSelected, 0, 0x000F);
  }

  // select the new item
  SetItemState(iItem, LVIS_SELECTED|LVIS_FOCUSED, 0x000F);
} // SelectItem( uData )


/**
 * gets selected item
 *
 * @return   selected item ID, -1 if none selected
 */
uint4 ReportGroupCtrl::GetSelectedItem() const
{
  // return selected item's data
  int4 iItem = GetNextItem(-1, LVNI_SELECTED);
  if(iItem != -1)
  {
    return GetItemData(iItem);
  }

  return -1; // nothing  selected
} // GetSelectedItem()


/**
 * selects all items
 */
void ReportGroupCtrl::SetSelectionAll(const bool bSelectAll)
{
  const int iFlag = bSelectAll ? LVNI_SELECTED : 0;
  for (int iCurr = -1; (iCurr = GetNextItem(iCurr, 0)) >= 0; )
  {
    SetItemState(iCurr, LVNI_SELECTED, iFlag);
  }
} // SetSelectionAll( bSelectAll )


/////////////////////////////////////////////////////////////////////////////
// ReportGroupCtrl message handlers


/**
 * Draws the item.
 *
 * @param lpDrawItemStruct   Draw parameters.
 */
void ReportGroupCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

  CRect rcItem(lpDrawItemStruct->rcItem);

  // store draw item's index
  int4 iItem = lpDrawItemStruct->itemID;

  // save the state of the DC
  int4 iSavedDC = pDC->SaveDC();

  // retrieve the items attributes
  LV_ITEM lvi;
  lvi.mask      = LVIF_IMAGE | LVIF_STATE|LVIF_PARAM|LVIF_INDENT;
  lvi.iItem     = iItem;
  lvi.iSubItem  = 0;
  lvi.stateMask = 0xFFFF;   // get all state flags
  GetItem(&lvi);

  CString sLabel = GetItemText(iItem, 0);

  // retrieve bounding rectangles
  CRect rcBounds, rcLabel, rcIcon;
  GetItemRect(iItem, rcBounds, LVIR_BOUNDS);
  GetItemRect(iItem, rcLabel, LVIR_LABEL);
  GetItemRect(iItem, rcIcon, LVIR_ICON);

  CRect rcCol(rcBounds);

  int4 iOffset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;

  CRect rcClient;

  // if we have a background image draw it
  if(m_bmBackground.m_hObject != NULL)
  {
    // setup a memory DC
    CDC dcMem;
    dcMem.CreateCompatibleDC(pDC);

    // select background bitmap into it
    CBitmap* pOldBitmap = dcMem.SelectObject(&m_bmBackground);

    GetClientRect(&rcClient);

    CRgn rgnBitmap;
    CRect rcBitmap(rcItem);

    rcBitmap.right = rcClient.right;

    if(iItem == GetItemCount() - 1 )
      rcBitmap.bottom = rcClient.bottom;

    rgnBitmap.CreateRectRgnIndirect(&rcBitmap);
    pDC->SelectClipRgn(&rgnBitmap);
    rgnBitmap.DeleteObject();

    pDC->BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);

    dcMem.SelectObject(pOldBitmap);
    dcMem.DeleteDC();
  }


  rcCol.right = rcCol.left + GetColumnWidth(0);

  CRgn rgn;
  rgn.CreateRectRgnIndirect(&rcCol);

  pDC->SelectClipRgn(&rgn);

  rgn.DeleteObject();

  // draw the image for the item
  CImageList* piml = GetImageList(LVSIL_SMALL);
  ASSERT(piml);
   piml->Draw(pDC, lvi.iImage, CPoint(rcIcon.left, rcIcon.top),ILD_TRANSPARENT);

  // setup the origin for the text
   rcLabel.left += iOffset/2;
  rcLabel.right -= iOffset;

  CFont* pOldFont = NULL;

  // select the font, if this is a group header item use bold
  if(m_bUseGroups && IsGroup(iItem))
  {
    pOldFont = pDC->SelectObject(&m_fontBold);
  }

  // if we are showing selected item in bold select it
  else if(m_bBoldSelected && lvi.state & LVIS_SELECTED)
  {
    pOldFont = pDC->SelectObject(&m_fontBold);
  }

  // output the text
  pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);

  if(pOldFont)
  {
    pDC->SelectObject(pOldFont);
  }

  rgn.CreateRectRgnIndirect(&rcBounds);
  pDC->SelectClipRgn(&rgn);

  pDC->RestoreDC(iSavedDC);
} // DrawItem( lpDrawItemStruct )


/**
 * WM_ERASEBKGND handler, called when background needs erasing
 *
 * @param   pdc     Device context for window
 * @return          Non-zero to indicate NOT to erase, zero to let framework erase window
 */
BOOL ReportGroupCtrl::OnEraseBkgnd(CDC* pDC)
{
  // always return true for owner drawn controls
  return TRUE;
} // OnEraseBkgnd( pdc )


/**
 * Allows for initialization prior to subclassing
 */
void ReportGroupCtrl::PreSubclassWindow()
{
  CListCtrl::PreSubclassWindow();

  // load bitmap images from the exe
  {
    ExeResourceSwapper swapper;
    // load image list and attach
    VERIFY(m_imlNormal.Create(IDB_RPT_GROUP_24, 16, 1, RGB(255,0,255)));
    SetImageList(&m_imlNormal, LVSIL_SMALL);
  }

  // insert hidden column into list control
  CRect rc;
  GetClientRect(rc);
  InsertColumn(0, "", LVCFMT_LEFT, rc.Width());

  // create font to be used when item is selected
  VERIFY(m_fontBold.CreateFont(
    18,
    0,
    0,
    0,
    FW_BOLD,
    FALSE,
    FALSE,
    0,
    ANSI_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_SWISS,
    "Arial"));
} // PreSubclassWindow()


/**
 * Called before an item is changed
 *
 * @param   iPrevious   index of previous item
 * @param   iNew        index of new item
 */
void ReportGroupCtrl::OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

  *pResult = 0;

  // if we are not using checkboxes and the state is trying to change, disallow
  if(!m_bCheckBoxes)
  {
    if(plv->uNewState & LVIS_SELECTED && (m_bUseGroups && IsGroup(plv->iItem)))
    {
      *pResult = 1;
    }
  }
} // OnItemChanging( pNMHDR, pResult ) 


/**
 * Called after an item has changed
 *
 * @param   iPrevious   index of previous item
 * @param   iNew        index of new item
 */
BOOL ReportGroupCtrl::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
  *pResult = 0;

  // just return if we are using checkboxes
  if(m_bCheckBoxes)
  {
    return FALSE;
  }

  // toggle the checkmark image
  if(plv->uOldState & LVIS_SELECTED  && (plv->uNewState & LVIS_SELECTED) == 0)
  {
    SetItemImage(plv->iItem, 1);
  }
  else if(plv->uNewState & LVIS_SELECTED)
  {
    SetItemImage(plv->iItem, 2);
  }

  *pResult = 0;

  return FALSE;
} // OnItemChanged( pNMHDR, pResult ) 


/**
 * Set image for an item
 *
 * @param   iItem    item to change
 * @param   iImage   New image for item
 */
void ReportGroupCtrl::SetItemImage(const int4 iItem, const int4 iImage)
{
  // set the item's image
  LVITEM lvi;
  lvi.mask      = LVIF_IMAGE;
  lvi.iItem     = iItem;
  lvi.iSubItem  = 0;
  lvi.iImage    = iImage;

  SetItem(&lvi);
} // SetItemImage( iItem, iImage )


/**
 * Get image for an item
 *
 * @param   iItem    item to change
 * @return           image for the item
 */
int4 ReportGroupCtrl::GetItemImage(const int4 iItem) const
{
  // get item's image index
  LVITEM lvi;
  lvi.mask = LVIF_IMAGE;
  lvi.iItem  = iItem;
  lvi.iSubItem  = 0;

  GetItem(&lvi);

  return lvi.iImage;
} // GetItemImage( iItem )


/**
 * return position at which new group item should be inserted
 *
 * @param   iGroupID   group to query
 * @return             position for a new item
 */
int4 ReportGroupCtrl::GetNextGroupItem(const int4 iGroupID) const
{
  // number of items in list
  int4 iNumItems = GetItemCount();

  // find item based on Group ID
  int iItem = FindItem(iGroupID);

  // if not found are the item is the last item in the list return last position
  if(iItem == -1 || iItem == iNumItems - 1)
  {
    return iNumItems;
  }

  iItem++;

  // while the item is not a Group Header
  while(!IsGroup(iItem))
  {
    // get the next item
    int4 iNewItem = GetNextItem(iItem, LVNI_ALL);
    if(iNewItem == -1)
    {
      iItem++;
      break;
    }
    iItem = iNewItem;
  }

  return iItem;
} // GetNextGroupItem( iGroupID )


/**
 * query if a specified item is a group
 *
 * @param   iItem    item to query
 * @return           true if the item refers to a group
 */
bool ReportGroupCtrl::IsGroup(const int4 iItem) const
{
  // are we even using groups?
  if (m_bUseGroups == false)
  {
    return false;
  }

  // if we are we differentiate using the indent level(I'm the King of Kludge!)
  LVITEM lvi;
  lvi.mask = LVIF_INDENT;
  lvi.iItem = iItem;
  lvi.iSubItem = 0;
  GetItem(&lvi);

  // index = 0 group header, otherwise false
  return lvi.iIndent == 0;
} // IsGroup( iItem )


/**
 * WM_LBUTTONDOWN down handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void ReportGroupCtrl::OnLButtonDown(UINT uFlags, CPoint point)
{
  int iItem = HitTest(point);

  // Was something clicked and not a Group separator
  if(iItem == -1 || (m_bUseGroups && IsGroup(iItem)) )
  {
    return;
  }

  // are we using checkboxes?
  if(m_bCheckBoxes)
  {
    // toggle check state
    int4 iImage = GetItemImage(iItem);
    SetItemImage(iItem, iImage==UNCHECKED? CHECKED:UNCHECKED);

    // send LVN_ITEMCHANGED to parent
    NM_LISTVIEW nm;
    nm.hdr.hwndFrom = m_hWnd;
    nm.hdr.idFrom = GetDlgCtrlID();
    nm.hdr.code = LVN_ITEMCHANGED;
    nm.iItem = iItem;
    nm.iSubItem = 0;
    nm.uNewState = LVIS_STATEIMAGEMASK;

    GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nm);

  } // end using checkboxes

  CListCtrl::OnLButtonDown(uFlags, point);
} // OnLButtonDown( uFlags, point )


/**
 * WM_RBUTTONDOWN down handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void ReportGroupCtrl::OnRButtonDown(UINT uFlags, CPoint point)
{
  int4 iItem = HitTest(point);

  // Was something clicked and not a Group separator
  if (iItem == -1 || (m_bUseGroups && IsGroup(iItem)))
  {
    CListCtrl::OnRButtonDown(uFlags, point);
    return;
  }

  // Are we using checkboxes?
  if(m_bCheckBoxes)
  {
    // item's image indicates check state
    int iImage = GetItemImage(iItem);

    // toggle check state
    SetItemImage(iItem, iImage==UNCHECKED? CHECKED:UNCHECKED);
    SetItemState(iItem, INDEXTOSTATEIMAGEMASK(iImage == UNCHECKED? 1: 0), LVIS_STATEIMAGEMASK);
  }

  CListCtrl::OnRButtonDown(uFlags, point);
} // OnRButtonDown( uFlags, point )


/**
 * WM_LBUTTONDBLCLK handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void ReportGroupCtrl::OnLButtonDblClk(UINT uFlags, CPoint point)
{
  // Eat it
} // OnLButtonDblClk( uFlags, point )


/**
 * WM_RBUTTONDBLCLK handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void ReportGroupCtrl::OnRButtonDblClk(UINT uFlags, CPoint point)
{
  // Eat it
} // OnRButtonDblClk( uFlags, point )


/**
 * Event handler for scrolling
 * 
 * @param uSBCode      Scroll event code
 * @param uPos         Current slider position, validity depends on nSBCode
 * @param pScrollBar   Pointer to control receiving the event
 */
void ReportGroupCtrl::OnVScroll(UINT uSBCode, UINT uPos, CScrollBar* pScrollBar)
{
  CListCtrl::OnVScroll(uSBCode, uPos, pScrollBar);

  // prevent painting problems
  Invalidate(TRUE);
} // OnVScroll( uSBCode, uPos, pScrollBar )


// Revision History:
// $Log: ReportGroupCtrl.C,v $
// Revision 1.4.2.2  2009/07/30 17:58:20  kchalupa
// Code Walkthrough
//
// Revision 1.4.2.1  2008/02/12 20:32:25  jmeade
// Issue 5924: adding other-findings info to extra-colonic colon findings.
//
// Revision 1.4  2007/03/10 04:10:43  jmeade
// code standards.
//
// Revision 1.3  2005/11/07 20:38:18  geconomos
// modified headers
//
// Revision 1.2  2005/09/13 17:21:07  jmeade
// ColonReport namespace.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.0.20.2  2004/03/30 23:45:46  jmeade
// Issue 3590: Fix bug where more than one finding type could be highlighted.
//
// Revision 3.0.20.1  2003/04/09 21:41:25  jmeade
// Framework to process rt-click msg when not on an item; Coding standards.
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:34   ingmar
// Initial revision.
// Revision 1.10  2001/04/19 15:30:42  binli
// Initialized font pointer to null
//
// Revision 1.9  2001/04/18 15:49:32  geconomos
// coding standards
//
// Revision 1.8  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.7  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportGroupCtrl.C,v 1.4.2.2 2009/07/30 17:58:20 kchalupa Exp $
// $Id: ReportGroupCtrl.C,v 1.4.2.2 2009/07/30 17:58:20 kchalupa Exp $
