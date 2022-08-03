// $Id: TabStrip.h,v 1.3 2007/03/02 14:33:04 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragma declarations
#pragma once

// include declarations
#include "TabStripItem.h"

OPEN_WINUTILS_NS
  
// class declaration
class TabStrip : public CWnd 
{
// enums
public:
  
  // tab strip orientation enumeration
  enum TabStripOrientationEnum
  {
    LEFT  = 0,
    BOTTOM = 1,
  };

// member functions
public:
  
  /// constructor
  TabStrip();
  
  /// destructor
  virtual ~TabStrip();

  /// prevents the control from being redrawn as tabs are add and removed
  void BeginInit() { m_bInitializing = true; }

  /// sets initialization as completed and redraws the controls
  void EndInit();
  
  /// gets the orientation 
  const TabStrip::TabStripOrientationEnum GetOrientation() const { return m_eOrientation; }
  
  /// sets the orientation
  void SetOrientation( const TabStrip::TabStripOrientationEnum  eOrientation );

  /// gets the index of the selected tab
  int4 GetSelectedIndex() const { return m_iSelectedIndex; }

  /// sets the selected tab index
  void SetSelectedIndex( const int4 iIndex );

  /// gets the number of tabs
  const int4  GetItemCount() const { return static_cast< int4 >( m_items.size() ); }

  /// clears all tabs 
  void ClearItems();

  /// adds a tab to the end 
  void AddItem( const TabStripItem & item );

  /// inserts a tab at the specified index
  void InsertItem( int4 iIndex, const TabStripItem & item );

  /// removes the tab at the specified index
  void RemoveItem( int4 iIndex );

  /// enables/disables the specified tab
  void EnableItem( int4 iIndex, bool bEnable );

protected:

  /// mfc message map
  DECLARE_MESSAGE_MAP()

  /// WM_PAINT handler
  void OnPaint();

  /// WM_ERASEBKGND handler
  BOOL OnEraseBkgnd( CDC * pdc );

  /// WM_SIZE handler
  void OnSize( UINT uType, int4 cx, int4 cy );

  /// WM_LBUTTONDOWN handler
  void OnLButtonDown( UINT nFlags, CPoint point );

  /// WM_SIZEPARENT handler
  LRESULT OnSizeParent( WPARAM wParam, LPARAM lParam );

  /// called before the current tab changes
  virtual void OnItemChanging( int4 iPreviousItem, int4 iNewItem ) { }

  /// called after the current tab changes
  virtual void OnItemChanged( int4 iPreviousItem, int4 iNewItem )  { }

private:
  
  /// sets the size and location of the tabs
  void UpdateTabs();

  /// redraws the control into the draw buffer
  void RedrawControl();

  /// draws the specified tab item
  void DrawTabItem( Gdiplus::Graphics & gfx, int4 iIndex );

// member variables
private:

  /// current orientation
  TabStripOrientationEnum m_eOrientation;

  /// typedef for TabStripItem 
  typedef std::list< TabStripItem * > TabStripItems;
  
  /// tab strip items
  TabStripItems m_items;

  /// selected item index
  int4 m_iSelectedIndex;

  /// draw buffer
  Gdiplus::Bitmap * m_pDrawBuffer;

  /// indicates if the control is being initialized
  bool m_bInitializing;

}; // class TabStrip

CLOSE_WINUTILS_NS


// $Log: TabStrip.h,v $
// Revision 1.3  2007/03/02 14:33:04  geconomos
// code review preparation
//
// Revision 1.2  2005/10/06 20:34:55  geconomos
// spelling correction
//
// Revision 1.1  2005/10/03 12:57:54  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/TabStrip.h,v 1.3 2007/03/02 14:33:04 geconomos Exp $
// $Id: TabStrip.h,v 1.3 2007/03/02 14:33:04 geconomos Exp $
