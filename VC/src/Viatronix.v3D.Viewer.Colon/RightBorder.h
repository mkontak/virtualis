// $Id: RightBorder.h,v 1.2 2007/03/01 21:40:09 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once

OPEN_WINUTILS_NS

// class declaration
class RightBorder : public CWnd
{
// member functions
public:
  
  /// constructor
  RightBorder();
  
  /// destruuctor
  virtual ~RightBorder();

protected:

  /// mfc message map
  DECLARE_MESSAGE_MAP()

  /// WM_PAINT handler
  void OnPaint();

  /// WM_ERASEBKGND handler
  BOOL OnEraseBkgnd( CDC * pdc );

  /// WM_SIZE handler
  void OnSize( UINT uType, int cx, int cy );

  /// WM_LBUTTONDOWN handler
  void OnLButtonDown( UINT nFlags, CPoint point );

  /// WM_SIZEOPARENT handler
  LRESULT OnSizeParent( WPARAM wParam, LPARAM lParam );

// member variables
private:

  /// draw buffer
  Gdiplus::Bitmap * m_pDrawBuffer;

}; // class RightBorder

CLOSE_WINUTILS_NS

// $Log: RightBorder.h,v $
// Revision 1.2  2007/03/01 21:40:09  geconomos
// code review preparation
//
// Revision 1.1  2005/10/03 12:57:54  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/RightBorder.h,v 1.2 2007/03/01 21:40:09 geconomos Exp $
// $Id: RightBorder.h,v 1.2 2007/03/01 21:40:09 geconomos Exp $
