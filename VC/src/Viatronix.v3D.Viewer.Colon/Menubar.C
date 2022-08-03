// $Id: Menubar.C,v 1.3 2007/03/13 03:10:51 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// includes
#include "StdAfx.h"
#include "Menubar.h"
#include "MenubarItem.h"
#include "Theme.h"

//namespaces
using namespace WinUtils;

/**
 * Consturctor
 */
Menubar::Menubar() : 
 m_pWndParent( NULL ),
 m_pBackground( NULL )
{
} // Menubar()


/**
 * Destructor
 */
Menubar::~Menubar()
{
  Detach();

  if( m_pBackground )
    delete m_pBackground;

} // ~Menubar()


/**
 *  Attaches the menu bar to the specified window
 *
 * @param   pWnd    parent window
 */
void Menubar::Attach( CWnd * pWnd )
{
  Detach();

  // cache the owner window
  m_pWndParent = pWnd;

  // iterate over all the menu items
  CMenu * pMenu = pWnd->GetMenu();
  for( UINT i( 0 ); i < pMenu->GetMenuItemCount(); ++i )
  {
    // get the menu item text
    CString sMenuText;
    pMenu->GetMenuString( i, sMenuText, MF_BYPOSITION );

    // create the data to associate with the menu item
    MenubarItem * pItem  = new MenubarItem( i, static_cast<const char *>( sMenuText ) );
    
    // make the menu item owner drawn and pass the user specific data
    pMenu->ModifyMenu( i, MF_BYPOSITION | MF_OWNERDRAW, pMenu->GetMenuItemID(i), reinterpret_cast< char * >( pItem ) );

    // add the user data to the vector for quick access
    m_items.push_back( pItem );
  } // for( UINT i( 0 ); i < pMenu->GetMenuItemCount(); ++i )
} // Attach( CWnd * pWnd )


/**
 * Detaches the menubar from it's parent window
 */
void Menubar::Detach()
{
  // are we attached to a window
  if( m_pWndParent )
  {
    // delete the user data
    for( int i(0); i < m_items.size(); ++ i )
      delete m_items[i];
    m_items.clear();
    m_pWndParent = NULL;
  }
} // Detach()


/**
 * Indicates if the menubar can process the specified message
 *
 * @param   hwnd      window handle
 * @param   uMsg      message code
 * @param   wParam    additional data  
 * @param   lParam    additional data  
 * @return  true if message can be handled
 */
bool Menubar::CanProcessMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  return  uMsg == WM_NCACTIVATE || uMsg == WM_SETTEXT || uMsg == WM_NCPAINT ||
        ( uMsg == WM_DRAWITEM && reinterpret_cast< LPDRAWITEMSTRUCT >( lParam )->CtlType == ODT_MENU ) ||
        ( uMsg == WM_MEASUREITEM && reinterpret_cast< LPMEASUREITEMSTRUCT >( lParam )->CtlType == ODT_MENU );
} // CanProcessMessage( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )


/**
 * Processes the specified message
 *
 * @param   hwnd      window handle
 * @param   uMsg      message code
 * @param   wParam    additional data  
 * @param   lParam    additional data  
 * @return  true if message can be handled
 */
LRESULT Menubar::ProcessMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  LRESULT lResult( 0 );
  switch( uMsg )
  {
	  case WM_NCPAINT:
      lResult = OnNcPaint( reinterpret_cast< HRGN >( wParam ) );
      break;

    case WM_NCACTIVATE:
	  case WM_SETTEXT:
      lResult = WrapMessage( uMsg, wParam, lParam );
      break;

    case WM_MEASUREITEM:
      lResult = OnMeasureItem( reinterpret_cast< LPMEASUREITEMSTRUCT >( lParam ) );

    case WM_DRAWITEM:
      lResult = OnDrawItem( reinterpret_cast< LPDRAWITEMSTRUCT >( lParam ) );
      break;
  } // switch( uMsg )
  return lResult;
} // WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )


/**
 * WM_NCPAINT handler
 *
 * @param   hrgn    handle to the update region of the window
 * @return  an application returns zero if it processes this message
 */
LRESULT Menubar::OnNcPaint( HRGN hrgn )
{
  // get the window rectangle
  RECT windowRect; GetWindowRect( m_pWndParent->GetSafeHwnd(), &windowRect );

  // create a region which covers the whole window ( must be in screen coordinates )
	hrgn = CreateRectRgnIndirect( &windowRect );

  // create the rectangle representing the menu bar
  RECT menuRect; GetWindowRect(  m_pWndParent->GetSafeHwnd(), &menuRect );

  // get the system sizes for the caption, border and menu
  static int iCaptionHeight = GetSystemMetrics( SM_CYCAPTION );
  static int iFrameWidth    = GetSystemMetrics( SM_CXFRAME );
  static int iFrameHeight   = GetSystemMetrics( SM_CYFRAME );
  static int iMenubarHeight = GetSystemMetrics( SM_CYMENU );
  
  // position the the rectangle to the menubar area
  menuRect.left += iFrameWidth;
  menuRect.right -= iFrameWidth;
  menuRect.top += iCaptionHeight + iFrameHeight;
  menuRect.bottom = menuRect.top + iMenubarHeight;

  // create a region based on the menubar area
  HRGN hrgnMenubar = CreateRectRgnIndirect( &menuRect );

	// cut out a menu area
	CombineRgn( hrgn, hrgn, hrgnMenubar, RGN_XOR );

  // free temporary region
	DeleteObject( hrgnMenubar );

	// call the default window procedure with modified window region ( must be in screen coordinates )
  DefWindowProc(  m_pWndParent->GetSafeHwnd(), WM_NCPAINT, reinterpret_cast< WPARAM >( hrgn ), 0 );

  // get a window dc
	HDC hdc = GetWindowDC(  m_pWndParent->GetSafeHwnd() );

  m_offset = Gdiplus::PointF( -windowRect.left, -windowRect.top );

  // all drawing is is done in client coordinates
  OffsetRect( &menuRect, m_offset.X, m_offset.Y );

  // create a graphics objects based on the windows dc
  Gdiplus::Graphics gfx( hdc );

  // calculate width and height of toolbar
  int4 iWidth( menuRect.right - menuRect.left );
  int4 iHeight( menuRect.bottom - menuRect.top );

  if( m_pBackground == NULL || m_pBackground->GetWidth() != iWidth  )
    CreateBackgroundImage( iWidth, Theme::MenubarBkgndMiddle->GetHeight() );

  gfx.DrawImage( m_pBackground, menuRect.left, menuRect.top, iWidth, iHeight );

  // position the first menu item
  float4 x( 4.0F );
  float4 y( static_cast< float4 >( menuRect.top ) );

  // iterate over all the menu items
  CMenu * pMenu = m_pWndParent->GetMenu();
  for( UINT i(0); i < pMenu->GetMenuItemCount(); ++i )
  {
    // grab the associated item data from the vector (by position)
    MenubarItem * pItem = m_items[ i ];
    
    // draw the menu item
    DrawMenuItem( gfx, *pItem, Gdiplus::RectF( x, y, pItem->GetSize().first, pItem->GetSize().second ) );

    // increment to the next menu item position
    x += pItem->GetSize().first;
  } // for( UINT i(0); i < pMenu->GetMenuItemCount(); ++i )

  ReleaseDC(  m_pWndParent->GetSafeHwnd(), hdc );
	
	DeleteObject( hrgn );

  return 0;
} // OnNcPaint( HWND hwnd, HRGN hrgn )


/**
 * WM_DRAWITEM handler
 *
 * @param   pDrawStruct   contains information about the item to be drawn 
 * @return  an application returns zero if it processes this message
 */
LRESULT Menubar::OnDrawItem( LPDRAWITEMSTRUCT pDrawStruct )
{
  USES_CONVERSION;

  // check that the ODA_SELECT or ODA_DRAWENTIRE actions are set
  // ( this differs from the msdn documentation, it does work however )
  if( pDrawStruct->itemAction !=  ODA_SELECT && pDrawStruct->itemAction != ODA_DRAWENTIRE )
    return FALSE;
  
  // get the associated data
  MenubarItem * pItem = reinterpret_cast< MenubarItem * >( pDrawStruct->itemData );

  // update the menu item state accordingly
  if( pDrawStruct->itemState & ODS_SELECTED )
  {
    pItem->SetState( MenubarItem::SELECTED );
  }
  else if( pDrawStruct->itemState & ODS_HOTLIGHT )
  {
    pItem->SetState( MenubarItem::HOVER );
  }
  else
  {
    pItem->SetState( MenubarItem::NORMAL );
  }

  // create a graphics object based on the dc
  Gdiplus::Graphics gfx( pDrawStruct->hDC );

  // format the recnagle for draing
  Gdiplus::RectF textrect( 
    static_cast< Gdiplus::REAL >( pDrawStruct->rcItem.left ), 
    static_cast< Gdiplus::REAL >( pDrawStruct->rcItem.top ),
    static_cast< Gdiplus::REAL >( pDrawStruct->rcItem.right  - pDrawStruct->rcItem.left ),
    static_cast< Gdiplus::REAL >( pDrawStruct->rcItem.bottom - pDrawStruct->rcItem.top ) );
  
  // draw the item
  DrawMenuItem( gfx, *pItem, textrect );  
  
  return TRUE;
} // OnDrawItem( LPDRAWITEMSTRUCT pDrawStruct )


/**
 * WM_MEASUREITEM handler
 *
 * @param   pMeasureStruct   contains the dimensions of the owner-drawn control or menu item
 * @return  an application returns zero if it processes this message
 */
LRESULT Menubar::OnMeasureItem( LPMEASUREITEMSTRUCT pMeasureStruct )
{
  USES_CONVERSION;

  // get the data associated with the menu item
  MenubarItem * pItem = reinterpret_cast< MenubarItem * >( pMeasureStruct->itemData );

  // graphics object based on the parent window
  Gdiplus::Graphics gfx( m_pWndParent->GetSafeHwnd() );

  // default format
  Gdiplus::StringFormat format;

  // measure the text and get its dimensions in pixels
  Gdiplus::RectF textrect;
  gfx.MeasureString( A2W( pItem->GetText().c_str() ), static_cast< INT >( pItem->GetText().length() ), Theme::MenubarFont, Gdiplus::PointF(0,0), &format, &textrect );

  // update the dimensions in the MEASUREITEMSTRUCT
  pMeasureStruct->itemHeight = static_cast< UINT >( textrect.Height );
  pMeasureStruct->itemWidth = static_cast< UINT >( textrect.Width );
  
  // this padding is what windows adds to the menu item
  // TODO: figure out if we can get it programatically
  const int4 iHorizontalPadding( 12 );
  
  // update the size in our user data
  pItem->SetSize( std::make_pair( static_cast< float4 >( pMeasureStruct->itemWidth + iHorizontalPadding ), static_cast< float4 >( pMeasureStruct->itemHeight )) );

  return TRUE;
} // OnMeasureItem( LPMEASUREITEMSTRUCT pMeasureStruct )


/**
 *  Disables paint when caption is updated
 *
 * @param     uMsg      windows message
 * @param     wParam    wParam
 * @param     lParam    lParam
 * @return    return from DefWndProc
 */
LRESULT Menubar::WrapMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  DWORD dwStyle( GetWindowLong( m_pWndParent->GetSafeHwnd(), GWL_STYLE ) );

  // turn OFF WS_VISIBLE
  SetWindowLong(m_pWndParent->GetSafeHwnd(), GWL_STYLE, dwStyle & ~WS_VISIBLE);

  // perform the default action, minus painting
  LRESULT ret = DefWindowProc( m_pWndParent->GetSafeHwnd(), uMsg, wParam, lParam);

  // turn ON WS_VISIBLE
  SetWindowLong( m_pWndParent->GetSafeHwnd(), GWL_STYLE, dwStyle);

  // perform custom painting
  OnNcPaint( (HRGN) 1 );

  return ret;
} // WrapMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )


/**
 * Draws the specified menu item.
 *
 * @param   item    menu item to drae
 */
void Menubar::DrawMenuItem( Gdiplus::Graphics & gfx, MenubarItem & item, Gdiplus::RectF & rect )
{
  USES_CONVERSION;

  // text is left-justified and centered vertically
  Gdiplus::StringFormat format;
  format.SetAlignment( Gdiplus::StringAlignmentCenter );

  // draw menu item based on it's state
  if( item.GetState() == MenubarItem::HOVER || item.GetState() == MenubarItem::SELECTED)
  {
    Gdiplus::RectF selrect = rect;
    selrect.Inflate( -2, -2 );
    gfx.FillRectangle( Theme::MenubarSelectedBrush, selrect );
    gfx.DrawRectangle( Theme::MenubarSelectedPen, selrect );
  }

  gfx.DrawString( A2W( item.GetText().c_str() ), static_cast< INT >( item.GetText().length() ), Theme::MenubarFont, rect, &format, Theme::MenubarTextBrush );

} // DrawMenuItem( Graphics & gfx, MenuItem & item )


/**
 * Creates the background image
 *
 * @param   iWidth      width of background image
 * @param   iHeight     height of background image
 */
void Menubar::CreateBackgroundImage( const int4 iWidth, const int4 iHeight )
{
  if( m_pBackground )
  {
    delete m_pBackground;
    m_pBackground = NULL;
  }
  m_pBackground = new Gdiplus::Bitmap ( iWidth, iHeight );

  Gdiplus::Graphics gfxtmp( m_pBackground );

  Gdiplus::TextureBrush brush( Theme::MenubarBkgndMiddle );

  gfxtmp.FillRectangle( &brush, 0, 0, iWidth, Theme::MenubarBkgndMiddle->GetHeight() );

  gfxtmp.DrawImage( Theme::MenubarBkgndLeft,  0, 0, Theme::MenubarBkgndLeft->GetWidth(), Theme::MenubarBkgndMiddle->GetHeight() );

  gfxtmp.DrawImage( Theme::MenubarBkgndRight, iWidth - Theme::MenubarBkgndRight->GetWidth(), 0, Theme::MenubarBkgndRight->GetWidth(), Theme::MenubarBkgndMiddle->GetHeight());

} // CreateBackgroundImage( int4 iWidth, int4 iHeight )


// $Log: Menubar.C,v $
// Revision 1.3  2007/03/13 03:10:51  jmeade
// code standards.
//
// Revision 1.2  2007/03/01 21:40:08  geconomos
// code review preparation
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Menubar.C,v 1.3 2007/03/13 03:10:51 jmeade Exp $
// $Id: Menubar.C,v 1.3 2007/03/13 03:10:51 jmeade Exp $
