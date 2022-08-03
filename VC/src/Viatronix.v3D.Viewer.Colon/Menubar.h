// $Id: Menubar.h,v 1.4 2007/03/13 03:10:51 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// pragms
#pragma once

// include declarations


OPEN_WINUTILS_NS

class MenubarItem;

// class declaration
class Menubar
{
// member functions
public:
 
  // constructor
  Menubar();
  
  // destructor
  virtual ~Menubar();

  /// attaches the menubar to the specified window
  void Attach( CWnd * pWnd );

  /// detached the menubar from it's parent window
  void Detach();

  /// gets if the menubar can process the specified message
  bool CanProcessMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );

  /// gets if the menubar can process the specified message
  LRESULT ProcessMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );

private:

  /// WM_NCPAINT handler
  LRESULT OnNcPaint( HRGN hRegion );

  /// WM_DRAWITEM handler
  LRESULT OnDrawItem( LPDRAWITEMSTRUCT pDrawStruct );

  /// WM_MEASUREITEM handler
  LRESULT OnMeasureItem( LPMEASUREITEMSTRUCT pMeasureStruct );

  /// draw the specified menu item
  virtual void DrawMenuItem( Gdiplus::Graphics & gfx, MenubarItem & item, Gdiplus::RectF & rect );

  /// creates the background image
  void CreateBackgroundImage( int4 iWidth, int4 iHeight );

  /// Disables paint when caption is updated
  LRESULT WrapMessage( UINT msg, WPARAM wParam, LPARAM lParam );
  
// member variables
private:

  // parent window
  CWnd * m_pWndParent;

  /// background image for the menubar
  Gdiplus::Bitmap * m_pBackground;

  /// location of menu
  Gdiplus::PointF m_offset;

  // quick access to menu item data
  std::vector< MenubarItem * > m_items;

}; // class Menubar

CLOSE_WINUTILS_NS

// $Log: Menubar.h,v $
// Revision 1.4  2007/03/13 03:10:51  jmeade
// code standards.
//
// Revision 1.3  2007/03/07 19:45:03  romy
// Uses forward declaration instead of including the .h File - Part of code review
//
// Revision 1.2  2007/03/01 21:40:08  geconomos
// code review preparation
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Menubar.h,v 1.4 2007/03/13 03:10:51 jmeade Exp $
// $Id: Menubar.h,v 1.4 2007/03/13 03:10:51 jmeade Exp $
