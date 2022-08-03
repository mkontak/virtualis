// $Id: MenuButton.cpp,v 1.5.2.2 2010/03/24 07:21:22 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: MenuButton.cpp
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "MenuButton.h"
#include "MenuUtils.h"
#include "WinUtils.h"
#include "MonitorDisplay.h"

/////////////////////////////////////////////////////////////////////////////
// MenuList

using namespace WinUtils;

/**
 * Constructor 
 */
MenuList::MenuList()
{
} // constructor


/**
 * Destructor 
 */
MenuList::~MenuList()
{
} // destructor


/**
 * message map
 */
BEGIN_MESSAGE_MAP(MenuList, CCheckListBox)
  //BEGIN_MESSAGE_MAP(MenuList, CListBox)
  //{{AFX_MSG_MAP(MenuList)
  ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillFocus)
  ON_WM_CREATE()
  ON_WM_LBUTTONDOWN()
  ON_WM_CHAR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * WM_CREATE handler.
 *
 * @param lpCreateStruct   Create window parameters.
 * @return                 -1 if failed, 0 otherwise.
 */
int MenuList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CListBox::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }

  ModifyStyle(0, LBS_WANTKEYBOARDINPUT, 0);

  CRect listRect;
  GetClientRect(listRect);

  CRect parRect(CPoint(lpCreateStruct->x,lpCreateStruct->y),CSize(listRect.Width()*1.09,listRect.Height()*0.9));
  CWnd * pOwnerWnd = CWnd::FromHandle(lpCreateStruct->hwndParent);
  pOwnerWnd->GetParentOwner()->ClientToScreen(parRect);

  m_wndParent.CreateEx(0, AfxRegisterWndClass(0), LPCTSTR(NULL), WS_POPUP|WS_DLGFRAME|WS_VISIBLE, parRect, pOwnerWnd, 0);
  m_wndParent.SetDlgCtrlID(GetDlgCtrlID());

  m_font.DeleteObject();
  LOGFONT logFont;
  CFont * pFont = pOwnerWnd->GetParentOwner()->GetFont();
  if (0 && pFont)
  {
    pFont->GetLogFont(&logFont);
    m_font.CreateFontIndirect(&logFont);
    SetFont(&m_font);
  }

  SetDlgCtrlID(1);
  SetParent(&m_wndParent);
  SetOwner(pOwnerWnd);
  SetWindowPos(NULL,0,0,0,0,SWP_NOSIZE|SWP_NOZORDER);
  UpdateSizeAndPosition();

  return 0;
} // OnCreate()


/**
 * Show window override, to perform other necessary updating when toggling show state.
 *
 * @param iCmdShow   Show command.
 * @return           Result of ShowWindow call.
 */
BOOL MenuList::ShowWindow(int iCmdShow)
{
  CCheckListBox::ShowWindow(iCmdShow);
  if (IsWindowVisible())
  {
    CCheckListBox::ShowWindow(SW_HIDE);
    UpdateSizeAndPosition();
    CCheckListBox::ShowWindow(iCmdShow);
  }

  SetSel(-1, FALSE);
  return m_wndParent.ShowWindow(iCmdShow);
} // ShowWindow()


/**
 * WM_KILLFOCUS handler.
 */
void MenuList::OnKillFocus() 
{
  ShowWindow(SW_HIDE);
} // OnKillFocus()


/**
 * WM_CHAR handler.
 *
 * @param uChar     Character code for the key pressed.
 * @param uRepCnt   Repeat count.
 * @param uFlags    State flags.
 */
void MenuList::OnChar(UINT uChar, UINT uRepCnt, UINT uFlags) 
{
  if (uChar == VK_ESCAPE)
  {
    ShowWindow(SW_HIDE);
  }

  CCheckListBox::OnChar(uChar, uRepCnt, uFlags);
} // OnChar()


/**
 * Left mouse down handler.
 *
 * @param uFlags   Indicates which virtual keys are engaged.
 * @param point    Location of mouse click.
 */
void MenuList::OnLButtonDown(UINT uFlags, CPoint point) 
{
  BOOL bOutside(TRUE);
  uint4 uItem = ItemFromPoint(point, bOutside);
  if (!bOutside && IsEnabled(uItem))
  {
    SetCheck(uItem, !GetCheck(uItem));
    GetOwner()->PostMessage(MenuButton::WM_MENUITEM_STATE_CHANGED, MAKEWPARAM(uItem, GetCheck(uItem)?1:0), 0);
  }

  CListBox::OnLButtonDown(uFlags, point);
} // OnLButtonDown()


/**
 * Updates the current size and position of the menu listbox.
 */
void MenuList::UpdateSizeAndPosition()
{
  CRect rcWindow, rcWndClient;
  CRect rcListClient, rcList, rcItem;
  int4 iNumberOfItemsInList;
  int4 i;

  // Insert items
  iNumberOfItemsInList = GetCount();
  if (iNumberOfItemsInList <= 0)
  {
    return;
  }

  // Calculate the size of the border of the parent window.

  GetClientRect(&rcWndClient);
  GetWindowRect(&rcWindow);

  CSize sizeText;
  CDC* pListDC = NULL;
  int4 iCheckMarkWidth = 0;
  int4 iListCtrlHeight = 0;
  int4 iLongestTextItem=0;

  // The size of the last item
  GetItemRect(iNumberOfItemsInList - 1, &rcItem);
  iListCtrlHeight = rcItem.bottom;

  GetItemRect(0, &rcItem);
  iCheckMarkWidth = rcItem.left+24;

  // A pointer to the list ctrl CDC
  pListDC = GetDC();

  /// Go thru each item and calculate its width - save the widest 
  for (i=0 ; i<iNumberOfItemsInList ; i++)
  { 
    CString sText;
    GetText(i, sText);
    GetTextExtentPoint32(*pListDC, sText, sText.GetLength(), &sizeText);
    iLongestTextItem = max(sizeText.cx, iLongestTextItem);
  }

  // Now Resize the window to the right size - the width should never be 
  // smaller than the button though
  CRect rcParent;
  m_wndParent.GetWindowRect(&rcParent);

  // Add the width of the frame as the difference between
  // the client-area and the outer window border
  const int4 iFrameWidth((rcWindow.Width() - rcWndClient.Width())/2);

  const int4 iWidth(iLongestTextItem + iCheckMarkWidth + (iFrameWidth*2));
  const int4 iHeight(iListCtrlHeight + (iFrameWidth*2));
  const int4 iButtonWidth(rcParent.Width());

  CRect rcButton;
  CWnd * pOwnerButton = GetOwner();
  pOwnerButton->GetWindowRect(rcButton);

  if (rcButton.bottom+iHeight > MonitorDisplay::GetCurrentDisplayRect().bottom - 100)
  {
    rcButton.OffsetRect(0, -(rcButton.Height()+iHeight+6));
  }
  if (rcButton.right+iWidth > MonitorDisplay::GetCurrentDisplayRect().right - 100)
  {
    rcButton.OffsetRect(-(iWidth+6), 0);
  }


//  SetWindowPos(NULL, rcButton.left, rcButton.top + rcButton.Height(), max(iWidth, iButtonWidth), iHeight, 0);//SWP_NOMOVE);
  m_wndParent.SetWindowPos(NULL, rcButton.left+5,rcButton.bottom, iWidth+6, iHeight+6, SWP_NOZORDER);

  // Resize the listctrl to fit the client
  GetClientRect(&rcWndClient);  
  SetWindowPos(NULL, 0, 0, iWidth, iHeight, SWP_NOZORDER|SWP_NOMOVE);

  // Resize the first column to fit the listctrl client
  GetClientRect(&rcList);
//  SetColumnWidth(0, rcList.Width());
} // UpdateSizeAndPosition()


/////////////////////////////////////////////////////////////////////////////
// MenuButton


/**
 * Constructor 
 */
MenuButton::MenuButton() : SkinButton()
{
  m_menu.CreatePopupMenu();
  m_uIconID = 0;

  m_iRadioItem = -1;
} // constructor


/**
 * Destructor 
 */
MenuButton::~MenuButton()
{
} // destructor


/**
 * message map
 */
BEGIN_MESSAGE_MAP(MenuButton, SkinButton)
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  ON_WM_CREATE()
  ON_MESSAGE(MenuButton::WM_MENUITEM_STATE_CHANGED, OnMenuItemStateChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MenuButton windows message handlers


/**
 * WM_CREATE handler.
 *
 * @param lpCreateStruct   Create window parameters.
 * @return                 -1 if failed, 0 otherwise.
 */
int MenuButton::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (SkinButton::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }

  InitListbox();
  return 0;
} // OnCreate()


/**
 * Initializes the menu list box.
 */
void MenuButton::InitListbox()
{
  if (!IsWindow(m_listBox.GetSafeHwnd()))
  {
    CWnd * pParent = GetParentOwner();
    CRect rect;

    GetWindowRect(rect);  pParent->ScreenToClient(rect);

    // Place just below, and off left side, of the button
    CPoint topLeftPt(rect.left+int4(rect.Width()*0.25), rect.bottom);
    m_listBox.Create(WS_BORDER | WS_OVERLAPPED | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS,
                  CRect(topLeftPt, CSize(100,100)),
                  this, GetDlgCtrlID()+1000);

    m_listBox.ModifyStyle(0, LBS_MULTIPLESEL);
    m_listBox.ModifyStyleEx(0, WS_EX_TRANSPARENT);

    CClientDC btnDC(this), listBoxDC(&m_listBox);
    listBoxDC.SetTextColor(btnDC.GetTextColor());
    listBoxDC.SetBkColor(::GetSysColor(COLOR_3DFACE));

    m_listBox.ResetContent();
    for (int4 i(0); i < m_menu.GetMenuItemCount(); i++)
    {
      CString sItem;
      m_menu.GetMenuString(i, sItem, MF_BYPOSITION);
      sItem.Remove('&');
      m_listBox.AddString(sItem);
      m_listBox.SetCheck(i, (m_menu.GetMenuState(i, MF_BYPOSITION) & MF_CHECKED) ? 1 : 0);
      m_listBox.Enable(i, (m_menu.GetMenuState(i, MF_BYPOSITION) & MF_GRAYED) ? FALSE : TRUE);
    }
  }

  m_listBox.UpdateSizeAndPosition();
} // InitListbox()


#if 1
#define _SEND_COMMANDS_DURING_
#else
#define _SEND_COMMANDS_AFTER_
#endif


/**
 * WM_MOUSELEAVE handler.
 */
void MenuButton::OnMouseLeave()
{
  SkinButton::OnMouseLeave();
} // OnMouseLeave()


/**
 * Handler for button press.
 */
void MenuButton::OnClicked()
{
  InitListbox();
  m_listBox.ShowWindow(!m_listBox.IsWindowVisible() ? SW_SHOW : SW_HIDE);
  m_listBox.SetFocus();
} // OnClicked()


/**
 * Called when state (e.g. selection) of a menu list item changes.
 *
 * @param wParam     ID of the item which has changed.
 * @param LPARAM     Unused.
 * @return           TRUE to accept selection change, FALSE otherwise.
 */
LRESULT MenuButton::OnMenuItemStateChanged(WPARAM wParam, LPARAM)
{
  const int4 iSelID(m_menu.GetMenuItemID(LOWORD(wParam)));
  GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(iSelID, HIWORD(wParam)?1:0), 0);

  MENUITEMINFO menuItem;
  ResetItem(menuItem, MIIM_DATA);
  m_menu.GetMenuItemInfo(iSelID, &menuItem, FALSE);
  uint4 uLoByte = LOBYTE(menuItem.dwItemData);
  if (uLoByte > 0)
  {
    // Radio item, must deselect all others
    for (int4 i(0); i < m_listBox.GetCount(); i++)
    {
      if (i == LOWORD(wParam))  continue;
      m_listBox.SetCheck(i, FALSE);
    }
  }

  return 1;
} // OnMenuItemStateChanged()


/**
 * Adds an item to the menu list.
 *
 * @param sItem    Item text.
 * @param iId      Item ID.
 * @param uFlags   Menu item flags.
 * @param eStyle   Menu item styles.
 */
void MenuButton::AddMenuItem(const std::string & sItem, const int4 iId, const UINT uFlags, const MBItemStylesEnum eStyle)
{
  m_menu.AppendMenu(uFlags, iId, sItem.c_str());

  MENUITEMINFO menuItem;
  ResetItem(menuItem, MIIM_DATA);
  menuItem.dwItemData = (eStyle == MBS_ACTION) ? MAKEWORD(0,1) : 0;
  SetMenuItemInfo(HMENU(m_menu), iId, FALSE, &menuItem);

  if (HWND(m_listBox) != NULL)
  {
    CString sListItem(sItem.c_str());
    sListItem.Remove('&');
    int4 iNew = m_listBox.AddString(sListItem);
  }
} // AddMenuItem()


/**
 * Adds item to menu list.
 *
 * @param item     Parameters for the menu item.
 * @param eStyle   Styles for the menu item.
 */
void MenuButton::AddMenuItem(const MenuItemStruct & item, const MBItemStylesEnum eStyle)
{
  AddMenuItem(LPCTSTR(LoadResourceString(item.m_iId)), item.m_iId, item.m_uFlags, eStyle);
} // AddMenuItem()


// USED by COLON VIEWER ONLY:
// determines if a bit is set in a value representing a set of bits
/**
 * determines if a bit is set
 *
 * @param tBits    list of bits.
 * @param tBit     bit to test.
 * @return         true/false if bit is set.
 */
template <class T, class U>
inline bool IsBitSet(const T& tBits, const U& tBit)
{
  return ((tBits&tBit)==tBit);
} // IsBitSet()


/**
 * Adds items from a menu to the menu list.
 *
 * @param uMenuID       ID of the menu.
 * @param uSubMenuPos   Sub-menu position of the menu to be loaded.
 * @param eStyle        Style for each menu item to be added.
 */
void MenuButton::AddMenuItems(const UINT uMenuID, const UINT uSubMenuPos, const MBItemStylesEnum eStyle)
{
  const uint4 uItemCount = m_menu.GetMenuItemCount();

  // Separate these items from others already added
  if (uItemCount > 0)
  {
    m_menu.AppendMenu(MF_SEPARATOR, 0, LPCTSTR(NULL));
  }

  CMenu newMenu;
  CMenu* pPopupMenu;
  if ((!newMenu.LoadMenu(uMenuID)) || ((pPopupMenu = newMenu.GetSubMenu(uSubMenuPos)) == NULL))
  {
    return;
  }

  int4 i;
  MENUITEMINFO menuItem;
  ResetItem(menuItem, MIIM_DATA);

  if (eStyle == MBS_RADIO)
  {
    // dwItemData contains low and high radio item position
    //   MUST BE stored as 1-based item position, to distinguish
    //   from action and check items
    menuItem.dwItemData = MAKEWORD(uItemCount + 1, uItemCount + pPopupMenu->GetMenuItemCount());
  }
  else if (eStyle == MBS_ACTION)
  {
    menuItem.dwItemData = MAKEWORD(0, 1);
  }
  else if (eStyle == MBS_CHECK)
  {
    menuItem.dwItemData = MAKEWORD(0, 0);
  }

  for (i = 0; i < pPopupMenu->GetMenuItemCount(); i++)
  {
    const uint4 uID(pPopupMenu->GetMenuItemID(i));
    if (uID <= 0)
    {
      m_menu.AppendMenu(MF_SEPARATOR, 0, LPCTSTR(NULL));
      continue;
    }

    CString sItem;
    pPopupMenu->GetMenuString(i, sItem, MF_BYPOSITION);
    AddMenuItem(LPCTSTR(sItem), uID, 0, eStyle);

    // Set low and high group ids for each radio item
    menuItem.wID = uID;
    SetMenuItemInfo(HMENU(m_menu), uID, FALSE, &menuItem);

    // Check menu item if appropriate
    const bool bChecked(IsBitSet(pPopupMenu->GetMenuState(i, MF_BYPOSITION), MF_CHECKED));
    ModifyMenuItem(uID, bChecked ? MF_CHECKED : MF_UNCHECKED, uID, sItem);
  }
} // AddMenuItems()


/**
 * Sets the default button ID for the button.
 *
 * @param uIconId   ID for the default icon.
 */
void MenuButton::SetDefaultIcon(const UINT uIconId)
{
  m_uIconID = uIconId;
  SetButtonIcon(*this, uIconId);
} // SetDefaultIcon()


/**
 * Adds icons for each menu item (to be shown when selected).
 *
 * @param vuIconIds[]    Vector of icon IDs.
 * @param iNumNewItems   Number of icons (size of vector).
 */
void MenuButton::AddItemIcons(const UINT vuIconIds[], const int4 iNumNewItems)
{
  int4 i;
  MENUITEMINFO menuItem;
  ResetItem(menuItem, MIIM_DATA);

  for (i = 0; i < iNumNewItems; i++)
  {
    GetMenuItemInfo(m_menu, i, TRUE, &menuItem);
    
    menuItem.dwItemData = MAKELONG(LOWORD(menuItem.dwItemData), vuIconIds[i]);

    SetMenuItemInfo(m_menu, i, TRUE, &menuItem);

//    m_bitmaps[i].DeleteObject();
//    m_bitmaps[i].LoadBitmap(iconIds[i]);
//    m_menu.SetMenuItemBitmaps(i,MF_BYPOSITION,&m_bitmaps[i],&m_bitmaps[i]);
  }
} // AddItemIcons()


/**
 * Toggle enable state of specified item.
 *
 * @param uId       ID of item.
 * @param bEnable   Whether to enable or disable item.
 */
void MenuButton::EnableMenuItem(const UINT uId, const bool bEnable)
{
  m_menu.EnableMenuItem(uId, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));
  if (HWND(m_listBox) != NULL)
  {
    for (int4 i(0); i < m_menu.GetMenuItemCount(); i++)
    {
      if (m_menu.GetMenuItemID(i) != uId)  continue;
      m_listBox.Enable(i, bEnable);
      break;
    }
  }
} // EnableMenuItem()


/**
 * Toggles checkmark on a single radio item.
 *
 * @param vuIDs[]          List of IDs making up the radio items group.
 * @param iIDOnRadioItem   ID of the checked radio item (or 0 to uncheck all).
 * @param iNumItems        Number of items (i.e. size of vector).
 * @return bool            Success/failure.
 */
bool MenuButton::CheckRadioItem(const UINT vuIDs[], const UINT iIDOnRadioItem, const int4 iNumItems)
{
  bool bRetVal = true;

  if (iIDOnRadioItem > 0)
  {
    // Select a radio item
    int4 iMinPos=0, iMaxPos=0;
    for (int4 i = 0; i < iNumItems; i++)
    {
      int4 iPos = GetMenuItemPosition(m_menu, vuIDs[i]);
      if (iPos < iMinPos)         iMinPos = iPos;
      else if (iPos > iMaxPos)    iMaxPos = iPos;

      if (HWND(m_listBox) != NULL)
      {
        m_listBox.SetCheck(iPos, iIDOnRadioItem==vuIDs[i] ? TRUE : FALSE);
      }
    }

    m_iRadioItem = iIDOnRadioItem;

    {
      MENUITEMINFO menuItem;
      ResetItem(menuItem, MIIM_DATA);
      GetMenuItemInfo(m_menu, iIDOnRadioItem, FALSE, &menuItem);
      if (HIWORD(menuItem.dwItemData))
      {
        SetButtonIcon(*this, HIWORD(menuItem.dwItemData));
      }
    }

    // Set/unset all menu items...
    bRetVal = (::CheckRadioItem(m_menu, MAKEWORD(iMinPos, iMaxPos), iIDOnRadioItem, true, false) != FALSE);
    // ...and set the MF_CHECKED property on the appropriate item for later reference
    ModifyMenuItem(iIDOnRadioItem, MF_BYCOMMAND | MF_CHECKED);
  }
  else
  {
    m_iRadioItem = -1;

    for (int4 i = 0; i < iNumItems; i++)
    {
      m_menu.CheckMenuItem(vuIDs[i], MF_BYCOMMAND | MF_UNCHECKED);
      if (HWND(m_listBox) != NULL)
      {
        m_listBox.SetCheck(i, FALSE);
      }
    }
    
    SetButtonIcon(*this, m_uIconID);
  }

  return bRetVal;
} // CheckRadioItem()


/**
 * Changes check state of a group of items.
 *
 * @param vuIDs[]      IDs of items to check/uncheck.
 * @param vbChecks[]   State of each item.
 * @param iNumItems    Number of items (size of both vectors).
 * @return bool        Success/failure.
 */
bool MenuButton::CheckMenuItems(const UINT vuIDs[], const bool vbChecks[], const int4 iNumItems)
{
  bool bRetVal = true;
  for (int4 i = 0; i < iNumItems; i++)
  {
    //bRetVal &= m_menu.CheckMenuItem(vuIDs[i], (vbChecks[i] ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
    bRetVal &= ModifyMenuItem(vuIDs[i], (vbChecks[i] ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
    if (IsWindow(m_listBox))
      m_listBox.SetCheck(i, vbChecks[i] ? 1 : 0);
  }
  return bRetVal;
} // CheckMenuItems()


/**
 * Modifies a set of menu items.
 *
 * @param vuPos[]     Positions of menu items.
 * @param vuFlags[]   Modify flags for each item.
 * @param iNumItems   Number of items (size of vectors).
 * @return bool       Success/failure.
 */
bool MenuButton::ModifyMenuItems(const UINT vuPos[], const UINT vuFlags[], const int4 iNumItems)
{
  bool bRetVal = true;
  for (int4 i = 0; i < iNumItems; i++)
  {
    bRetVal &= ModifyMenuItem(vuPos[i], vuFlags[i]);
  }
  return bRetVal;
} // ModifyMenuItems()


/**
 * Modifies a menu item.
 *
 * @param uPos          Position of item.
 * @param uFlags        Modify flags.
 * @param uIDNewItem    Item's ID (if changing).
 * @param lpszNewItem   Text for item.
 * @return bool         Success/failure.
 */
bool MenuButton::ModifyMenuItem(const UINT uPos, const UINT uFlags, UINT uIDNewItem, LPCTSTR lpszNewItem)
{
  uIDNewItem = (uIDNewItem != 0) ? uIDNewItem : uPos;
  if (lpszNewItem == NULL)
  {
    return (m_menu.ModifyMenu(uPos, uFlags, uIDNewItem, LoadResourceString(uPos)) == TRUE);
  }
  else
  {
    return (m_menu.ModifyMenu(uPos, uFlags, uIDNewItem, lpszNewItem) == TRUE);
  }
} // ModifyMenuItem()


/**
 * Left button down handler.
 *
 * @param uFlags   Virtual key flags.
 * @param point    Point of mouse click.
 */
void MenuButton::OnLButtonDown(UINT uFlags, CPoint point)
{
  SkinButton::OnLButtonDown(uFlags, point);
} // OnLButtonDown()


/**
 * Right button down handler.
 *
 * @param uFlags   Virtual key flags.
 * @param point    Location of mouse click.
 */
void MenuButton::OnRButtonDown(UINT uFlags, CPoint point) 
{
  OnClicked();  
  SkinButton::OnRButtonDown(uFlags, point);
} // OnRButtonDown()


/////////////////////////////////////////////////////////////////////////////
// ComboMenuButton


/**
 * Constructor 
 */
ComboMenuButton::ComboMenuButton() : m_bInPulldownMenu(false)
{
} // constructor


/**
 * Called when button is clicked.
 */
void ComboMenuButton::OnClicked()
{
  if (m_bInPulldownMenu)
  {
  }
  else
  {
    GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), 1), 0);
  }
} // OnClicked()


/**
 * Left button down handler.
 *
 * @param uFlags   Virtual key flags.
 * @param point    Point of mouse click.
 */
void ComboMenuButton::OnLButtonDown(UINT uFlags, CPoint point)
{
  CRect rect;
  GetClientRect(&rect);
  m_bInPulldownMenu = point.x > (float4(rect.Width()) * 0.75F);
  MenuButton::OnLButtonDown(uFlags, point);
  if (m_bInPulldownMenu)
  {
    m_menu.SetDefaultItem(GetDlgCtrlID(), FALSE);
    MenuButton::OnClicked();
  }
} // OnLButtonDown()


/**
 * Right button down handler.
 *
 * @param uFlags   Virtual key flags.
 * @param point    Point of mouse click.
 */
void ComboMenuButton::OnRButtonDown(UINT uFlags, CPoint point)
{
  CRect rect;
  GetClientRect(&rect);
  m_bInPulldownMenu = point.x > (float4(rect.Width()) * 0.75F);
  MenuButton::OnRButtonDown(uFlags, point);
  if (m_bInPulldownMenu)
  {
    m_menu.SetDefaultItem(GetDlgCtrlID(), FALSE);
    MenuButton::OnClicked();
  }
} // OnRButtonDown()


// Revision History:
// $Log: MenuButton.cpp,v $
// Revision 1.5.2.2  2010/03/24 07:21:22  dongqing
// use selected monitor for calculations
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.5.2.1  2010/03/11 01:38:38  dongqing
// show menu above/left-of button if near bottom/right of screen
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.5  2007/03/13 03:10:51  jmeade
// code standards.
//
// Revision 1.4  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.3  2006/01/31 14:26:19  frank
// code review
//
// Revision 1.2.4.1  2006/02/28 17:28:56  geconomos
// modified for SkinButton changes
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.5.10.1.12.4  2005/07/20 15:58:11  jmeade
// Issue 4284: Initialize listbox upon creation, so as to keep list and menu synched at all times.
//
// Revision 3.5.10.1.12.3  2005/07/19 21:05:07  jmeade
// Issue 4284: Must update menu-button's listbox on menu item state updates.
//
// Revision 3.5.10.1.12.2  2005/07/18 22:29:11  jmeade
// Issue 4282.
//
// Revision 3.5.10.1.12.1  2005/07/06 19:47:22  geconomos
// Issue #4247: Regenerating the list box every time before it is shown
//
// Revision 3.5.10.1  2003/02/13 23:42:47  jmeade
// Issue 2840: Update menu listbox location on every show, and properly calculate location based on parent's location.
//
// Revision 3.5  2002/06/06 18:36:52  jmeade
// Remove any ampersands before adding a list box string.
//
// Revision 3.4  2002/06/04 18:37:01  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.3  2002/05/30 16:15:53  jmeade
// ComboMenuButton (Outlook-style button/menu button);
//  long overdue bug fix to dismiss menu on selection of an action menu item.
//
// Revision 3.2.2.1  2002/05/13 17:25:37  jmeade
// Issue 2208:  Reimplement CMenuButton using checklistbox instead of menu.
//
// Revision 3.2  2001/12/29 00:16:41  jmeade
// Merge from 1.1 branch.
//
// Revision 3.1  2001/11/17 01:48:49  jmeade
// Attempt to improve interaction slightly by closing menu on second click; Code standards
//
// Revision 3.0.2.1  2001/11/14 18:51:32  jmeade
// Depress/release menu button when clicked (toggles menu off when clicked second time!)
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:40   ingmar
// Initial revision.
// Revision 1.14  2001/07/16 23:30:05  jmeade
// unnecessary includes in header files moved to implementation files
//
// Revision 1.13  2001/05/11 20:22:31  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MenuButton.cpp,v 1.5.2.2 2010/03/24 07:21:22 dongqing Exp $
// $Id: MenuButton.cpp,v 1.5.2.2 2010/03/24 07:21:22 dongqing Exp $

