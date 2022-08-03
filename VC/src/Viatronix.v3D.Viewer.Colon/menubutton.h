// $Id: menubutton.h,v 1.3 2007/03/13 03:10:51 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: menubutton.h
// Description: 
// Created: 
// Author(s): Jeffrey Meade  jmeade@viatronix.net

#if !defined(AFX_MENUBUTTON_H__192E50B2_8D70_11D4_B6E7_00D0B7BF4092__INCLUDED_)
#define AFX_MENUBUTTON_H__192E50B2_8D70_11D4_B6E7_00D0B7BF4092__INCLUDED_

#include "Global.h"
#include "SkinButton.h"
#include <string>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMenuList window

namespace WinUtils
{
  /// a list contained within a window, much like a popup menu
  class MenuList : public CCheckListBox
  {
  public:
    /// constructor
    MenuList();
    /// destructor
    virtual ~MenuList();
    /// WM_SHOWWINDOW handler
    BOOL ShowWindow(int iCmdShow);
    /// updates the size and location of the list
    void UpdateSizeAndPosition();

  // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMenuList)
    protected:
    //}}AFX_VIRTUAL

    // Generated message map functions
  protected:
    //{{AFX_MSG(MenuList)
    afx_msg void OnKillFocus();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLButtonDown(UINT uFlags, CPoint point);
    afx_msg void OnChar(UINT uChar, UINT uRepCnt, UINT uFlags);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

  private:
    /// parent window
    CWnd m_wndParent;
    /// window font
    CFont m_font;
  }; // class MenuList


  /////////////////////////////////////////////////////////////////////////////
  // MenuButton window


  /////////////////////////////////////////////////////////////////////////////
  // Style of menu items stored/determined by following:
  //
  //  (fType & MFT_RADIOCHECK)      fItemData                     style
  //  ----------------------------------------------------------------------
  //  MFT_RADIOCHECK                DWORD(grouplow, high)         MBS_RADIO
  //  0                             1                             MBS_ACTION
  //  0                             0                             MBS_CHECK
  //

  /// a button that displays a list of items
  class MenuButton : public SkinButton
  {
  public:
    /// info on a menu item
    struct MenuItemStruct
    {
      long m_iId;
      UINT m_uFlags;
    }; // struct MenuItemStruct

    /// menu button styles
    enum MBItemStylesEnum { MBS_CHECK = 1, MBS_RADIO = 2, MBS_ACTION = 3 };
    /// menu button messages
    enum { WM_MENUITEM_STATE_CHANGED = WM_APP };

  public:
    /// constructor
    MenuButton();
    /// destructor
    virtual ~MenuButton();
    /// adds an item to the menu button
    void AddMenuItem(const std::string & sItem, const int4 iId, const UINT uFlags = 0, const MBItemStylesEnum eStyle = MBS_CHECK);
    /// adds an item to the menu button
    void AddMenuItem(const MenuItemStruct & item, const MBItemStylesEnum eStyle = MBS_CHECK);
    /// adds list of items to the menu button
    void AddMenuItems(const UINT uMenuID, const UINT uSubMenuPos = 0, const MBItemStylesEnum eStyle = MBS_CHECK);

    /// removes an item from the menu button
    void RemoveMenuItem();

    /// sets the default selection (for single-selection buttons)
    void SetDefaultIcon(const UINT uIconId);
    /// adds icons for each item
    void AddItemIcons(const UINT vIconIds[], const int4 iNumItems);

    /// enables/disables an item
    void EnableMenuItem(const UINT uId, const bool bEnable);
    /// checks/unchecks a set of items
    bool CheckMenuItems(const UINT vuIDs[], const bool vbChecks[], const int4 iNumItems);
    /// checks a radio item
    bool CheckRadioItem(const UINT vuIDs[], const UINT iIDOnRadioItem, const int4 iNumItems);

  // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(MenuButton)
    protected:
    //}}AFX_VIRTUAL

    // Generated message map functions
  protected:
    //{{AFX_MSG(MenuButton)
    afx_msg virtual void OnClicked();
    afx_msg virtual void OnLButtonDown(UINT uFlags, CPoint point);
    afx_msg virtual void OnRButtonDown(UINT uFlags, CPoint point);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    /// when the state of one of the menu items has changed
    afx_msg LRESULT OnMenuItemStateChanged(WPARAM wParam, LPARAM);

    DECLARE_MESSAGE_MAP()

    /// when the mouse leaves the window area
    void OnMouseLeave();

    /// sets characteristics of a menu item
    bool ModifyMenuItem(const UINT uPosition, const UINT uFlags, UINT uIDNewItem = 0, LPCTSTR lpszNewItem = NULL);
    /// modifies characteristics of a number of menu items
    bool ModifyMenuItems(const UINT vuPos[], const UINT vuFlags[], const int4 iItems);

  private:
    /// initializes the listbox that represents the menu
    void InitListbox();

  protected:
    /// stores menu items
    CMenu m_menu;

  private:
    /// the listbox that represents the menu
    MenuList m_listBox;

    /// currently-selected item ID
    UINT m_uIconID;
    /// current radio item selected
    int4 m_iRadioItem;

  }; // MenuButton


  /**
   * A combination pushbutton/menubutton, where a click in right 25% of button sends a default command
   */
  class ComboMenuButton : public MenuButton
  {
  public:
    /// Constructor
    ComboMenuButton();

  protected:
    //{{AFX_MSG(ComboMenuButton)
    afx_msg virtual void OnClicked();
    afx_msg virtual void OnLButtonDown(UINT uFlags, CPoint point);
    afx_msg virtual void OnRButtonDown(UINT uFlags, CPoint point);
    //}}AFX_MSG(ComboMenuButton)

  private:
    /// true if the last left or right mouse click was in the area reserved as the pulldown menu click area
    bool m_bInPulldownMenu;
  }; // ComboMenuButton

} // namespace WinUtils

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MENUBUTTON_H__192E50B2_8D70_11D4_B6E7_00D0B7BF4092__INCLUDED_)

// Revision History:
// $Log: menubutton.h,v $
// Revision 1.3  2007/03/13 03:10:51  jmeade
// code standards.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.4.24.1  2005/07/20 15:58:11  jmeade
// Issue 4284: Initialize listbox upon creation, so as to keep list and menu synched at all times.
//
// Revision 3.4  2002/06/04 18:37:01  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.3  2002/05/30 16:15:53  jmeade
// ComboMenuButton (Outlook-style button/menu button);
//  long overdue bug fix to dismiss menu on selection of an action menu item.
//
// Revision 3.2  2002/04/18 18:21:22  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.1.2.1  2002/05/13 17:25:37  jmeade
// Issue 2208:  Reimplement CMenuButton using checklistbox instead of menu.
//
// Revision 3.1  2001/11/17 01:49:42  jmeade
// OnMouseLeave(); Code standards
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:42   ingmar
// Initial revision.
// Revision 1.12  2001/06/14 00:38:48  jmeade
// enum encapsulation
//
// Revision 1.11  2001/05/11 20:22:31  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/menubutton.h,v 1.3 2007/03/13 03:10:51 jmeade Exp $
// $Id: menubutton.h,v 1.3 2007/03/13 03:10:51 jmeade Exp $
