// $Id: TabStrip.C,v 1.6 2007/03/09 00:11:04 jmeade Exp $
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
#include "TabStrip.h"
#include "Theme.h"
#include "Exception.h"
#include "GdiplusUtilities.h"

// defines
#define FILE_REVISION "$Revision: 1.6 $"
#ifdef Point
#undef Point
#endif

// namespaces
using namespace WinUtils;

/**
 * Constructor
 */
TabStrip::TabStrip() :
  m_eOrientation( TabStrip::BOTTOM ),
  m_iSelectedIndex( -1 ),
  m_pDrawBuffer( NULL )
{
}  // TabStrip()


/**
 * Destructor
 */
TabStrip::~TabStrip()
{
  if( m_pDrawBuffer != NULL )
    delete m_pDrawBuffer;

 } // ~TabStrip()


/**
 * MFC message map
 */
BEGIN_MESSAGE_MAP( TabStrip, CWnd )
  ON_WM_ERASEBKGND()
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_LBUTTONDOWN()
  ON_MESSAGE( WM_SIZEPARENT, OnSizeParent )
END_MESSAGE_MAP()


/**
 * OnPaint message handler
 */
void TabStrip::OnPaint() 
{
  CPaintDC dc( this );

  // draw the buffer to the screen
  if( m_pDrawBuffer != NULL )
  {
    Gdiplus::Graphics gfx( dc.GetSafeHdc() );
    gfx.DrawImage( m_pDrawBuffer, 0, 0, m_pDrawBuffer->GetWidth(), m_pDrawBuffer->GetHeight() );
  } 
} // OnPaint()


/**
 * Sets initialization as completed and redraws the controls
 */
void TabStrip::EndInit()
{
  m_bInitializing = false;
  UpdateTabs();
  RedrawControl();
  RedrawWindow();
} // EndInit()


/**
 * Sets the orientation
 */
void TabStrip::SetOrientation( const TabStrip::TabStripOrientationEnum  eOrientation )
{
  m_eOrientation = eOrientation;
  if( !m_bInitializing )
    RedrawControl();
} // SetOrientation( const TabStrip::TabStripOrientationEnum  eOrientation )


/**
 * Sets the selected tab index
 *
 * @param   iSelectedIndex    selected tab index
 */
void TabStrip::SetSelectedIndex( const int4 iSelectedIndex )
{
  if( iSelectedIndex < -1 || iSelectedIndex > m_items.size() )
  {
    LogErr( "TabStrip index out of range", "TabStrip", "SetSelectedIndex" );
    return;
  }
  
  m_iSelectedIndex = iSelectedIndex;

  if( !m_bInitializing )
    RedrawControl();
} // SetSelectedIndex( const int4 iSelectedIndex )


/**
 * Clears all tabs 
 */
void TabStrip::ClearItems()
{
  TabStripItems::iterator iter = m_items.begin();
  for( ; iter != m_items.end(); ++iter )
    delete (*iter);    
  
  m_items.clear();
  if( !m_bInitializing )
    RedrawControl();
} // ClearItems()


/**
 * Adds a tab to the end 
 *
 * @param   item    tab to add
 */
void TabStrip::AddItem( const TabStripItem & item )
{
  m_items.push_back( new TabStripItem( item ) );
  if( !m_bInitializing )
    RedrawControl();
} // AddItem( const TabStripItem & item )


/**
 * Inserts a tab at the specified index
 *
 * @param   iIndex    index to insert at
 * @param   item      item to insert
 */
void TabStrip::InsertItem( int4 iIndex, const TabStripItem & item )
{
  TabStripItems::iterator iter = m_items.begin();
  std::advance( iter, iIndex);  
  m_items.insert( iter, new TabStripItem( item ) );

  if( !m_bInitializing )
    RedrawControl();
} // InsertItem( int4 iIndex, const TabStripItem & item )


/**
 * Removes the tab at the specified index
 *
 * @param   iIndex    index of tab to remove
 */
void TabStrip::RemoveItem( int4 iIndex )
{
  if( iIndex < 0 || iIndex >= m_items.size() )
  {
    LogErr( "TabStrip index out of range", "TabStrip", "RemoveItem" );
    return;
  }

  TabStripItems::iterator iter = m_items.begin();
  std::advance( iter, iIndex );  
  delete (*iter);
  m_items.erase( iter );

  if( !m_bInitializing )
    RedrawControl();
} // RemoveItem( int4 iIndex )


/**
 * enables/disables the specified tab
 *
 * @param   iIndex    index of tab to enable/disable
 * @param   bEnable   true to enable; false otherwise
 */
void TabStrip::EnableItem( int4 iIndex, bool bEnable )
{
  if( iIndex < 0 || iIndex >= m_items.size() )
  {
    LogErr( "TabStrip index out of range", "TabStrip", "GetItem" );
    return;
  }

  TabStripItems::iterator iter = m_items.begin();
  std::advance( iter, iIndex );  
  (*iter)->SetEnabled( bEnable );
  if( !m_bInitializing )
    RedrawControl();
} // EnableItem( int4 iIndex, bool bEnable )


/**
 * WM_ERASEBKGND handler, called when background needs erasing
 *
 * @param   pdc     Device context for window
 * @return          Non-zero to indicate NOT to erase, zero to let framework erase window
 */
BOOL TabStrip::OnEraseBkgnd( CDC * pdc )
{
  return TRUE;
} // OnEraseBkgnd( CDC * pdc )


/**
 * WM_SIZE handler, Called when window is resized
 *
 * @param uType   Type of resizing (e.g. maximize, restore)
 * @param iCX     Width
 * @param iCY     Height
 */
void TabStrip::OnSize( UINT uType, int cx, int cy )
{
  CWnd::OnSize( uType, cx, cy );

  // free the previous background image
  if( m_pDrawBuffer != NULL )
  {
    delete m_pDrawBuffer;
    m_pDrawBuffer = NULL;
  }

  // create new drawing buffer based on new window size
  m_pDrawBuffer = new Gdiplus::Bitmap( cx, cy, PixelFormat32bppARGB );

  RedrawControl();
} // OnSize( UINT uType, int cx, int cy )


/**
 * WM_SIZEPARENT handler. Called by mfc freamwork when negotiating the client area size.
 *
 * @param   wParam      not used
 * @param   lParam      pointer to AFX_SIZEPARENTPARAMS 
 * @return              not used
 */
LRESULT TabStrip::OnSizeParent( WPARAM wParam, LPARAM lParam )
{
  AFX_SIZEPARENTPARAMS * p = reinterpret_cast< AFX_SIZEPARENTPARAMS * >( lParam );

  if( m_eOrientation == TabStrip::LEFT )
    p->rect.left += Theme::BkgndSideLeft->GetWidth();
  else
    p->rect.bottom -= Theme::BkgndSideBottom->GetHeight();
  
  return 0;
} // OnSizeParent( WPARAM wParam, LPARAM lParam )


/**
 * WM_LBUTTONDOWN down handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void TabStrip::OnLButtonDown( UINT uFlags, CPoint point )
{
  CWnd::OnLButtonDown( uFlags, point );
  
  Gdiplus::Point pos( point.x, point.y );
  
  // if tabs are left aligned flip x and y
  if( m_eOrientation == TabStrip::LEFT )
  {
    Gdiplus::Point tmp( pos.X, pos.Y );
    pos.X = tmp.Y;
    pos.Y = tmp.X;
  }

  // iterate over all tabs
  int iIndex( 0 );
  TabStripItems::iterator iter = m_items.begin();
  for( ; iter != m_items.end(); ++iter, ++iIndex )
  {
    // did we click a tab?
    TabStripItem * pItem = (*iter);
    if( pItem->GetEnabled() && pItem->GetBounds().Contains( pos ) )
    {
      // is it already selected?
      if( iIndex != m_iSelectedIndex )
      {
        // store indices of new and previous tabs
        int4 iPrevious( m_iSelectedIndex );
        int4 iNew( iIndex );

        // notify derived classes that tab is about to change
        OnItemChanging( iPrevious, iNew );
        
        // update the selected tab and redraw
        SetSelectedIndex( iNew );
        RedrawWindow();
        
        // notify derived classes that tab has changed
        OnItemChanged( iPrevious, iNew );
        break;
      }
    } // if( pItem->GetEnabled() && pItem->GetBounds().Contains( pos ) )
  } // for( ; iter != m_items.end(); ++iter, ++iIndex )
} // OnLButtonDown( UINT nFlags, CPoint point )


/**
 * Sets the size and location of the tabs.
 */
void TabStrip::UpdateTabs()
{
  USES_CONVERSION;

  /// create a graphics object based on this control 
  Gdiplus::Graphics gfx( m_hWnd );
  
  /// width for the right part of the tab
  int4 iRightPartWidth( Theme::TabHorzInactiveRight->GetWidth() );
  
  // initial x and y position for first tab
  int4 x( m_eOrientation == TabStrip::LEFT ? 15 : 50 ); 
  int4 y( m_eOrientation == TabStrip::LEFT ? 0 : 0 ); 
  
  // tab height (use any image as the heights are the same )
  int4 iTabHeight( Theme::TabHorzInactiveLeft->GetHeight() );
  
  // iterate over all the tabs
  TabStripItems::iterator iter = m_items.begin();
  for( int4 iIndex( 0 ); iIndex < m_items.size(); ++iIndex, ++iter )
  {
    /// width for the left part of the tab
    int iLeftPartWidth ( iIndex == 0? Theme::TabHorzInactiveEnd->GetWidth() : Theme::TabHorzInactiveLeft->GetWidth() );

    /// grab the tab item
    TabStripItem * pItem = (*iter);
    
    // get the required size of the item's text
    Gdiplus::RectF size;
    gfx.MeasureString( A2W( pItem->GetText().c_str() ), pItem->GetText().length(), Theme::TabFont, Gdiplus::PointF( 0, 0 ),  &size );
    
    // how many times to we need to repeat the middle image?
    int iNextMultipleOf = Theme::TabHorzInactiveMiddle->GetWidth();
    int iMiddlePartWidth( GdiplusUtilities::GetNextMultiple( size.Width, iNextMultipleOf ) );
    
    // compute tab width
    int iTabWidth( iLeftPartWidth + iMiddlePartWidth + iRightPartWidth );
    iTabWidth = max( GdiplusUtilities::GetNextMultiple( 35, iNextMultipleOf ), iTabWidth );
    
    // update the item's bound
    pItem->SetBounds( Gdiplus::Rect( x, y,  iTabWidth, iTabHeight ) );
    
    // update the next starting x position (minus a fudge factor of course!)
    x += iTabWidth - 13;
  } // for( int index = 0; index < m_items.Count; ++index )
} // UpdateTabs()


/**
 * Redraws the control into the draw buffer
 */
void TabStrip::RedrawControl()
{
   // get the entire client area of the window
  CRect rc; GetClientRect( &rc );

  // create a gdi+ bounds
  Gdiplus::Rect bounds( rc.left, rc.top, rc.Width(), rc.Height() );

  // create using the newly created draw buffer
  Gdiplus::Graphics gfx( m_pDrawBuffer );

  if( m_eOrientation == TabStrip::LEFT )
  {
    Gdiplus::TextureBrush brush( Theme::BkgndSideLeft );
    gfx.FillRectangle( &brush, bounds.X, bounds.Y,Theme::BkgndSideLeft->GetWidth(), bounds.Height );
    
    // draw the top left corner
    gfx.DrawImage( Theme::BkgndCornerTopLeft, bounds.X, bounds.Y );

    // draw the bottom left corner
    gfx.DrawImage( Theme::BkgndCornerBottomLeft, bounds.X, bounds.Height - Theme::BkgndCornerBottomLeft->GetHeight() );
  }
  else
  {
    // draw the botom edge
    Gdiplus::TextureBrush brush( Theme::BkgndSideBottom );
    brush.TranslateTransform( 0, ( bounds.Height - Theme::BkgndSideBottom->GetHeight() ) % Theme::BkgndSideBottom->GetHeight() );
    gfx.FillRectangle( &brush,  bounds.X, bounds.Height - Theme::BkgndSideBottom->GetHeight(), bounds.Width, Theme::BkgndSideBottom->GetHeight()  );
    brush.ResetTransform();

    // draw the bottom left corner
    gfx.DrawImage( Theme::BkgndCornerBottomLeft, bounds.X, bounds.Height - Theme::BkgndCornerBottomLeft->GetHeight() );

  }
  
  // draw all the tabs
  int4 iIndex( 0 );
  TabStripItems::iterator iter( m_items.begin() );
  for( ; iter != m_items.end(); ++iter, ++iIndex )
    DrawTabItem( gfx, iIndex );

  if( m_eOrientation == TabStrip::BOTTOM )
  {
    // draw the bottom right corner
    gfx.DrawImage( Theme::BkgndCornerBottomRight, (Gdiplus::REAL)(bounds.Width - Theme::BkgndCornerBottomRight->GetWidth()), (Gdiplus::REAL)(bounds.Height - Theme::BkgndCornerBottomRight->GetHeight() ));
  }

  Invalidate();
} // RedrawControl()


/**
 * Draw a tab strip item.
 *
 * @param   gfx       graphics object
 * @param   iIndex    iIndex of item to draw
 */
void TabStrip::DrawTabItem( Gdiplus::Graphics & gfx, int4 iIndex )
{
  USES_CONVERSION;
  CRect rc;
  GetClientRect( &rc );

  // store the current text rendering hint
  Gdiplus::TextRenderingHint hint( gfx.GetTextRenderingHint() );
  
  // set to use high quality text rendering
  gfx.SetTextRenderingHint( Gdiplus::TextRenderingHintClearTypeGridFit );

  // store the current transformation matrix
  Gdiplus::Matrix matrix;
  gfx.GetTransform( &matrix );
 
  // are the tabs positioned along the side?
  if( m_eOrientation == TabStrip::LEFT )
  {
    // rotate 90 degress
    gfx.TranslateTransform( rc.Width(), 0 );
    gfx.RotateTransform( 90 );
  }

  /// get a pointer to the item we are about to draw
  TabStripItems::iterator iter = m_items.begin();
  std::advance( iter, iIndex );  
  TabStripItem * pItem = (*iter);

  /// placeholdes for appropriate images
  Gdiplus::Bitmap * pTabLeftImage( NULL );
  Gdiplus::Bitmap * pTabMiddleImage( NULL );
  Gdiplus::Bitmap * pTabRightImage( NULL );
      
  // the following selects the appropriate images that compose the tab
  if( iIndex == m_iSelectedIndex )
  {
    if( m_eOrientation == TabStrip::LEFT )
    {
      pTabLeftImage   = iIndex == 0? Theme::TabVertActiveEnd : Theme::TabVertActiveLeft;
      pTabMiddleImage = Theme::TabVertActiveMiddle;
      pTabRightImage  = Theme::TabVertActiveRight;
    }
    else
    {
      pTabLeftImage   = iIndex == 0? Theme::TabHorzActiveEnd : Theme::TabHorzActiveLeft;
      pTabMiddleImage = Theme::TabHorzActiveMiddle;
      pTabRightImage  = Theme::TabHorzActiveRight;
    }
  }
  else // non-selected item
  {
    if( m_eOrientation == TabStrip::LEFT )
    {
      pTabLeftImage   = iIndex == 0? Theme::TabVertInactiveEnd : Theme::TabVertInactiveLeft;
      pTabMiddleImage = Theme::TabVertInactiveMiddle;
      pTabRightImage  = Theme::TabVertInactiveRight;
    }
    else
    {
      pTabLeftImage   = iIndex == 0? Theme::TabHorzInactiveEnd : Theme::TabHorzInactiveLeft;
      pTabMiddleImage = Theme::TabHorzInactiveMiddle;
      pTabRightImage  = Theme::TabHorzInactiveRight;
    }
  }
  
  // draw left image
  if( pItem->GetEnabled() )
    gfx.DrawImage( pTabLeftImage, pItem->GetBounds().X, pItem->GetBounds().Y, pTabLeftImage->GetWidth(), pTabLeftImage->GetHeight() );
  else
   GdiplusUtilities::DrawImageTransparent( gfx, pTabLeftImage, pItem->GetBounds().X, pItem->GetBounds().Y, 0.5F );

  // draw middle image (repeated)
  int i = pItem->GetBounds().X + pTabLeftImage->GetWidth();
  for( ; i < pItem->GetBounds().GetRight() - pTabRightImage->GetWidth(); i += pTabMiddleImage->GetWidth() )
  {
    if( pItem->GetEnabled() )
      gfx.DrawImage( pTabMiddleImage, i, pItem->GetBounds().Y, pTabMiddleImage->GetWidth(), pTabMiddleImage->GetHeight() );
    else
      GdiplusUtilities::DrawImageTransparent( gfx, pTabMiddleImage, i, pItem->GetBounds().Y, 0.5F );
  }

  // draw right image
  if( pItem->GetEnabled() )
    gfx.DrawImage( pTabRightImage, i, pItem->GetBounds().Y, pTabRightImage->GetWidth(), pTabRightImage->GetHeight() );
  else
    GdiplusUtilities::DrawImageTransparent( gfx, pTabRightImage, i, pItem->GetBounds().Y, 0.5F );

  // set the format to center the text both in x and y
  Gdiplus::StringFormat format;
  format.SetAlignment( Gdiplus::StringAlignmentCenter  );
  format.SetLineAlignment( Gdiplus::StringAlignmentCenter  );
  
  // calcualte the the position for the tab text
  Gdiplus::RectF layout;
  layout.X = pItem->GetBounds().X;
  layout.Y = pItem->GetBounds().Y;
  layout.Width = pItem->GetBounds().Width;
  layout.Height = pTabMiddleImage->GetHeight() - 4;

   // is tab enabled
  if( pItem->GetEnabled() )
  {
    if( iIndex == m_iSelectedIndex )
      gfx.DrawString( A2W( pItem->GetText().c_str() ), pItem->GetText().length(), Theme::TabSelectedFont, layout, &format, Theme::TabSelectedBrush );
    else
      gfx.DrawString( A2W( pItem->GetText().c_str() ), pItem->GetText().length(), Theme::TabFont, layout, &format, Theme::TabBrush );
  }
  else // tan not enabled
  {
    if( iIndex == m_iSelectedIndex )
    {
      Gdiplus::Color color;
      Theme::TabSelectedBrush->GetColor( &color );
      Gdiplus::SolidBrush brush( Gdiplus::Color( 130, color.GetR(), color.GetG(), color.GetB() ) );
      gfx.DrawString( A2W( pItem->GetText().c_str() ), pItem->GetText().length(), Theme::TabSelectedFont, layout, &format, &brush );
    }
    else
    {
      Gdiplus::Color color;
      Theme::TabBrush->GetColor( &color );
      Gdiplus::SolidBrush brush( Gdiplus::Color( 100, color.GetR(), color.GetG(), color.GetB() ) );
      gfx.DrawString( A2W( pItem->GetText().c_str() ), pItem->GetText().length(), Theme::TabFont, layout, &format, &brush );
    }
  }

   // restore previous graphics state
  gfx.SetTextRenderingHint( hint );
  gfx.SetTransform( &matrix );
} // DrawTabItem( Graphics gfx, int iIndex )



// $Log: TabStrip.C,v $
// Revision 1.6  2007/03/09 00:11:04  jmeade
// code standards.
//
// Revision 1.5  2007/03/02 14:33:04  geconomos
// code review preparation
//
// Revision 1.4  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.3  2006/06/01 20:22:53  frank
// updated to visual studio 2005
//
// Revision 1.2.2.2  2006/05/16 12:38:50  geconomos
// corrected crash in destructor
//
// Revision 1.2.2.1  2006/05/09 13:51:25  geconomos
// Freeing tabs in destructor
//
// Revision 1.2  2005/10/05 22:07:50  jmeade
// removed potential unhandled exceptions from windows code.
//
// Revision 1.1  2005/10/03 12:57:54  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/TabStrip.C,v 1.6 2007/03/09 00:11:04 jmeade Exp $
// $Id: TabStrip.C,v 1.6 2007/03/09 00:11:04 jmeade Exp $
