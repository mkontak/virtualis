// $Id: SkinButton.h,v 1.7.2.1 2009/12/11 22:32:32 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 

#pragma once


/// includes
#include "WinGDIUtils.h"
#include "typedef.h"
#include <memory>

namespace WinUtils
{

  /**
   * Skinned button, a button with four state images in one bmp (up, down, focus, disabled)
   */
  class LinkButton : public CWnd
  {
  
  /// enums
  public:
    /// current button state
    enum LinkButtonStateEnum { UP = 0, DOWN, HOVER, DISABLED };

  /// member functions
  public:
    
    /// constructor
    LinkButton();
    
    /// destructor
    virtual ~LinkButton();

    /// creates the button
    bool Create( LPCTSTR sText, DWORD dwStyle, CRect & rc, CWnd * pParent, DWORD dwId );

    /// gets the button state
    LinkButtonStateEnum GetButtonState() const { return m_eButtonState; }

  protected:

    /// MFC message map
    DECLARE_MESSAGE_MAP()

    /// WM_ERASEBKGND handler
    afx_msg BOOL OnEraseBkgnd( CDC * pDC );

    /// WM_PAINT handler
    afx_msg void OnPaint();

    /// WM_LBUTTONDOWN handler
    afx_msg void OnLButtonDown( UINT uFlags, CPoint point);
    
    /// WM_LBUTTONUP handler
    afx_msg void OnLButtonUp( UINT uFlags, CPoint point );

    /// WM_MOUSEMOVE handler
    afx_msg void OnMouseMove( UINT uFlags, CPoint point );
    
    /// WM_SIZE
    afx_msg void OnSize( UINT uType, int iCX, int iCY );

    /// WM_ENABLE handler
    afx_msg void OnEnable( BOOL bEnable );

    /// window procedure
    virtual LRESULT WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam );

    /// called when the mouse enters the control
    virtual void OnMouseEnter();

    /// called when the mouse leaves the control
    virtual void OnMouseLeave();

    /// sets the current button state
    void SetButtonState( const LinkButton::LinkButtonStateEnum & eState );

    /// called when the button is clicked
    virtual void OnClicked() {}

    /// gets th if the mouse cursor is over the button
    bool IsMouseOverButton() const;

  private:

    /// button state
    LinkButtonStateEnum m_eButtonState;

    /// true when mouse cursor over button
    bool m_bMouseOver;

    std::shared_ptr< Gdiplus::SolidBrush > m_brushNormal;

    std::shared_ptr< Gdiplus::SolidBrush > m_brushHover;

    std::shared_ptr< Gdiplus::Font > m_font;

  }; // class LinkButton
} // namespace WinUtils
