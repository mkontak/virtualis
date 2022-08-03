// $Id: LinkButton.cpp,v 1.10.2.1 2009/12/11 22:32:32 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

/// includes
#include "stdafx.h"
#include "LinkButton.h"
#include "Theme.h"

#define FILE_REVISION "$Revision: 1.10.2.1 $"

/// namespaces
using namespace WinUtils;

/**
 * Constructor
 */
LinkButton::LinkButton() : 
  m_eButtonState( LinkButton::UP ),
  m_bMouseOver( false ),
  m_brushNormal( new Gdiplus::SolidBrush( Gdiplus::Color::Blue )),
  m_brushHover( new Gdiplus::SolidBrush( Gdiplus::Color::LightBlue )),
  m_font( new Gdiplus::Font( L"Tahoma", 8 ))
{
} // LinkButton()


/**
 * Destructor
 */
LinkButton::~LinkButton()
{
} // ~LinkButton()



/**
 * MFC message map
 */
BEGIN_MESSAGE_MAP( LinkButton, CWnd )
  ON_WM_CREATE()
  ON_WM_PAINT()
  ON_WM_ERASEBKGND()
  ON_WM_SIZE()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_ENABLE()
END_MESSAGE_MAP()


/**  
 * Creates the button
 */
 bool LinkButton::Create( LPCTSTR sText, DWORD dwStyle, CRect & rc, CWnd * pParent, DWORD dwId )
{
  if( !CWnd::Create( NULL, sText, dwStyle, rc, pParent, dwId ) )
    return false;

  CString sButtonText;
  GetWindowText( sButtonText );
  if( sButtonText.IsEmpty() )
    throw ex::IllegalArgumentException( LogRec( "The linkButton does not have any text.", "LinkButton", "Create" ));

  return true;
} // Create( LPCTSTR sText, DWORD dwStyle, CRect & rc, CWnd * pParent, DWORD dwId )


/////////////////////////////////////////////////////////////////////////////
// LinkButton


/**
 * WM_PAINT handler
 */
void LinkButton::OnPaint()
{ 
  CPaintDC dc( this );

  const bool bButtonEnabled( IsWindowEnabled() );
  Gdiplus::Graphics gfx( GetSafeHwnd() );

  CString sButtonText;
  GetWindowText( sButtonText );

  CRect rect;
  GetClientRect( &rect );

  USES_CONVERSION;

  // draw the text
  Gdiplus::StringFormat format;
  format.SetAlignment( Gdiplus::StringAlignmentCenter );
  format.SetLineAlignment( Gdiplus::StringAlignmentCenter );
  format.SetHotkeyPrefix( Gdiplus::HotkeyPrefixShow  );

  Gdiplus::SolidBrush * pBrush = ( bButtonEnabled ) ? 
                                  (( m_eButtonState == LinkButton::HOVER ) ? m_brushHover.get() : m_brushNormal.get() ) :
                                  Theme::ButtonDisabledBrush;

  Gdiplus::RectF layout( 0, 0, rect.Width(), rect.Height() );
  gfx.DrawString( A2W( sButtonText ), sButtonText.GetLength(), m_font.get(), layout, &format, pBrush );
} // OnPaint()


/**
 * WM_ERASEBKGND handler
 * 
 * @param  pdc   device context for button
 * @return       whether to erase background
 */
BOOL LinkButton::OnEraseBkgnd( CDC * pdc )
{
  return TRUE;
} // OnEraseBkgnd( CDC * pdc )


/**
 * Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void LinkButton::OnSize( UINT uType, int iCX, int iCY ) 
{
  CWnd::OnSize( uType, iCX, iCY );
} // OnSize(UINT uType, int iCX, int iCY) 


/**
 * Left mouse click handler; pauses for 25ms to ensure animation (button drawn in down position) will be visible
 * 
 * @param uFlags   Control key/mouse key flags
 * @param point    Mouse click point
 */
void LinkButton::OnLButtonDown( UINT uFlags, CPoint point )
{
  SetCapture();
  SetButtonState( LinkButton::DOWN );
} // OnLButtonDown( UINT uFlags, CPoint point )


/**
 * Left mouse click handler; pauses for 25ms to ensure animation (button drawn in down position) will be visible
 * 
 * @param uFlags   Control key/mouse key flags
 * @param point    Mouse click point
 */
void LinkButton::OnLButtonUp( UINT uFlags, CPoint point)
{
  if( IsMouseOverButton() )
  {
    SetButtonState( LinkButton::HOVER );
    GetParent()->PostMessage( WM_COMMAND, MAKEWPARAM( GetDlgCtrlID(), BN_CLICKED ), 0 );
    OnClicked();
  }
  else
    SetButtonState( LinkButton::UP );

  ReleaseCapture();
} // OnLButtonUp( UINT uFlags, CPoint point)


/**
 * Mouse move handler; displays help text if available for button, tracks mouse for mouse leave event
 * 
 * @param uFlags   Control key/mouse key flags
 * @param point    Mouse move point
 */
void LinkButton::OnMouseMove( UINT uFlags, CPoint point ) 
{
  CWnd::OnMouseMove( uFlags, point );

  LinkButtonStateEnum eNewState;
  if( uFlags & MK_LBUTTON )
    eNewState = LinkButton::DOWN;
  else
    eNewState = LinkButton::HOVER;

  SetButtonState( eNewState );

} // OnMouseMove( UINT uFlags, CPoint point ) 


/**
 * Called when mouse enters the window
 */
void LinkButton::OnMouseEnter()
{
} // OnMouseEnter()


/**
 * Called when mouse leaves window
 */
void LinkButton::OnMouseLeave()
{
  m_eButtonState = LinkButton::UP;
  
  RedrawWindow();

} // OnMouseLeave()


/**
 * WM_ENABLED handler
 *
 * @param   bEnable   indicates if window is enable or not
 */
void LinkButton::OnEnable( BOOL bEnable )
{
  RedrawWindow();
} // OnEnable( BOOL bEnable )


/**
 * Window message procedure; resets help text when appropriate
 * 
 * @param uMessage   Window message
 * @param wParam     Message parameter
 * @param lParam     Message parameter
 * @return           Result of processed message
 */
LRESULT LinkButton::WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam )
{
  switch ( uMessage )
  {
  case WM_MOUSEMOVE:
  {
    if( !m_bMouseOver )
    {
      m_bMouseOver = true;
      TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, GetSafeHwnd(), 0 };
      _TrackMouseEvent( &tme );  
      OnMouseEnter();
    }
    break;
  }
  case WM_MOUSELEAVE:
    OnMouseLeave();
    m_bMouseOver = false;
    break;
  case WM_KEYDOWN:
  case WM_KEYUP:
  case WM_CHAR:
  case WM_SYSKEYDOWN:
  case WM_SYSKEYUP:
    GetParentOwner()->PostMessage(uMessage, wParam, lParam);
    break;
  }

  return CWnd::WindowProc( uMessage, wParam, lParam );
} // WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam )


/**
 * Gets if the mouse cursor is over the button
 *
 * @return    true if mouse is over button
 */
bool LinkButton::IsMouseOverButton() const
{
  CRect rc;
  GetWindowRect( &rc );

  CPoint point( GetMessagePos() );
  return rc.PtInRect( point );
} // bool IsMouseOverButton() 


/**
 * Sets the current button state
 *
 * @param   eState    new button state
 */ 
void LinkButton::SetButtonState( const LinkButtonStateEnum & eNewState )
{
  if( eNewState != m_eButtonState )
  {
    m_eButtonState = eNewState;
    RedrawWindow();
  }
} // SetButtonState( const LinkButtonState & eState )

