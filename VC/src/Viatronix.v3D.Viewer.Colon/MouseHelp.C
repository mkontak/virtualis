// $Id: MouseHelp.C,v 1.7 2007/03/12 20:47:11 jmeade Exp $
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
#include "MouseHelp.h"
#include "GdiplusUtilities.h"
#include "Theme.h"
#include "Resource.h"

// namespaces
using namespace WinUtils;


/**
 * Constructor
 */
MouseHelp::MouseHelp() : 
  m_pDrawBuffer( NULL ),
  m_leftMouseImage(   AfxGetResourceHandle(), MAKEINTRESOURCEW( IDB_LEFTMOUSE  ) ),
  m_middleMouseImage( AfxGetResourceHandle(), MAKEINTRESOURCEW( IDB_MIDMOUSE   ) ),
  m_rightMouseImage(  AfxGetResourceHandle(), MAKEINTRESOURCEW( IDB_RIGHTMOUSE ) ),
  m_bControlKeyEnabled( false ),
  m_bShiftKeyEnabled( false ),
  m_bInitializing( false )
{
  
  // defaults
  const int4 iButtonImageWidth = 25;
  const int4 iButtonTextWidth = 90;
  const int4 iKeyTextWidth  = 50;
  const int4 iHeight = 22;
  const int4 iPadding = 6;

  // position the Gdiplus::Rectangles
  m_rectLeftImage   = Gdiplus::Rect(   0, iPadding-2, iButtonImageWidth, iHeight );
  m_rectLeftText    = Gdiplus::RectF( m_rectLeftImage.GetRight()   + iPadding, iPadding-2, iButtonTextWidth,  iHeight );
  m_rectMiddleImage = Gdiplus::Rect( m_rectLeftText.GetRight()    + iPadding, iPadding-2, iButtonImageWidth, iHeight );
  m_rectMiddleText  = Gdiplus::RectF( m_rectMiddleImage.GetRight() + iPadding, iPadding-2, iButtonTextWidth,  iHeight );
  m_rectRightImage  = Gdiplus::Rect( m_rectMiddleText.GetRight()  + iPadding, iPadding-2, iButtonImageWidth, iHeight );
  m_rectRightText   = Gdiplus::RectF( m_rectRightImage.GetRight()  + iPadding, iPadding-2, iButtonTextWidth,  iHeight );
  m_rectShiftKeyText = Gdiplus::RectF( m_rectRightText.GetRight()  + iPadding, iPadding-2, iKeyTextWidth,  iHeight / 2 );
  m_rectControlKeyText  = Gdiplus::RectF( m_rectRightText.GetRight()  + iPadding, m_rectShiftKeyText.GetBottom() + 2, iKeyTextWidth,  iHeight / 2);

} // MouseHelp()


/**
 * Destructor
 */
MouseHelp::~MouseHelp()
{
  if( m_pDrawBuffer )
    delete m_pDrawBuffer;
} // ~MouseHelp()


/**
 * Sets initialization as completed and redraws the controls
 */
void MouseHelp::EndInit()
{
  m_bInitializing = false;
  RedrawControl();
} // EndInit()


/**
 * Sets the left button text
 *
 * @param   sLeftButtonText   button text
 */
void MouseHelp::SetLeftButtonText( const std::string & sLeftButtonText ) 
{ 
  m_sLeftButtonText = sLeftButtonText; 
  if( !m_bInitializing )
    RedrawControl();
} // SetLeftButtonText(  const std::string & sLeftButtonText ) 


/**
 * Sets the middle button text
 *
 * @param   sMiddleButtonText   button text
 */
void MouseHelp::SetMiddleButtonText( const std::string & sMiddleButtonText ) 
{ 
  m_sMiddleButtonText = sMiddleButtonText; 
  if( !m_bInitializing )
    RedrawControl();
} // SetMiddleButtonText(  const std::string & sMiddleButtonText ) 


/**
 * Sets the right button text
 *
 * @param   sRightButtonText   button text
 */
void MouseHelp::SetRightButtonText( const std::string & sRightButtonText ) 
{ 
  m_sRightButtonText = sRightButtonText; 
  if( !m_bInitializing )
    RedrawControl();
} // SetRightButtonText(  const std::string & sRightButtonText ) 


/**
 * Sets the control key enabled flag
 *
 * @param   bEnabled    enabled state
 */
void MouseHelp::SetControlKeyEnabled( const bool & bEnabled ) 
{ 
  m_bControlKeyEnabled = bEnabled; 
  if( !m_bInitializing )
    RedrawControl();
} // SetControlKeyEnabled( const bool & bEnabled ) 


/**
 * Sets the shift key enabled flag
 *
 * @param   bEnabled    enabled state
 */
void MouseHelp::SetShiftKeyEnabled( const bool & bEnabled ) 
{ 
  m_bShiftKeyEnabled = bEnabled; 
  if( !m_bInitializing )
    RedrawControl();
} // SetShiftKeyEnabled( const bool & bEnabled ) 


/**
 * MFC message map
 */
BEGIN_MESSAGE_MAP( MouseHelp, CWnd )
  ON_WM_ERASEBKGND()
  ON_WM_PAINT()
  ON_WM_SIZE()
END_MESSAGE_MAP()


/**
 * OnPaint message handler
 */
void MouseHelp::OnPaint() 
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
 * WM_ERASEBKGND handler, called when background needs erasing
 *
 * @param   pdc     Device context for window
 * @return          Non-zero to indicate NOT to erase, zero to let framework erase window
 */
BOOL MouseHelp::OnEraseBkgnd( CDC * pdc )
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
void MouseHelp::OnSize( UINT uType, int cx, int cy )
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

  Invalidate();
} // OnSize( UINT uType, int cx, int cy )


/**
 * Redraws the control into the draw buffer
 */
void MouseHelp::RedrawControl()
{
  USES_CONVERSION;

   // get the entire client area of the window
  CRect rc; GetClientRect( &rc );

  // create a gdi+ bounds
  Gdiplus::Rect bounds( rc.left, rc.top, rc.Width(), rc.Height() );

  /// create agraphics object based on the draw buffer
  Gdiplus::Graphics gfx( m_pDrawBuffer );

  // fill with the colon yellow theme
  Gdiplus::TextureBrush brush( Theme::BkgndSideBottom );
  brush.TranslateTransform( 0, ( bounds.Height - Theme::BkgndSideBottom->GetHeight() ) % Theme::BkgndSideBottom->GetHeight() );
  gfx.FillRectangle( &brush,  bounds.X, bounds.Height - Theme::BkgndSideBottom->GetHeight(), bounds.Width, Theme::BkgndSideBottom->GetHeight()  );
  brush.ResetTransform();

  // pen for drawing Gdiplus::Rectangles
  Gdiplus::Pen pen( Gdiplus::Color::DarkGray );
  
  // string format for drawing mouse help text
  Gdiplus::StringFormat format;
  format.SetLineAlignment( Gdiplus::StringAlignmentCenter );
  
  // bevel for rounded Gdiplus::Rectaangles
  const int4 iBevel( 5 );

  // left mouse image, rounded Gdiplus::Rect and text
  GdiplusUtilities::DrawImageScaled( gfx, &m_leftMouseImage, m_rectLeftImage );
  GdiplusUtilities::DrawRoundedRectangle( gfx, pen, m_rectLeftText, iBevel );
  gfx.DrawString( A2W( m_sLeftButtonText.c_str() ), m_sLeftButtonText.length(), Theme::MouseHelpFont, m_rectLeftText, &format, Theme::MouseHelpBrush  );
  
  // middle mouse image, rounded Gdiplus::Rect and text
  GdiplusUtilities::DrawImageScaled( gfx, &m_middleMouseImage, m_rectMiddleImage );
  GdiplusUtilities::DrawRoundedRectangle( gfx, pen, m_rectMiddleText, iBevel );
  gfx.DrawString( A2W( m_sMiddleButtonText.c_str() ), m_sMiddleButtonText.length(), Theme::MouseHelpFont, m_rectMiddleText, &format, Theme::MouseHelpBrush  );
  
  // right mouse image, rounded Gdiplus::Rect and text
  GdiplusUtilities::DrawImageScaled( gfx, &m_rightMouseImage, m_rectRightImage );
  GdiplusUtilities::DrawRoundedRectangle( gfx, pen, m_rectRightText, iBevel );
  gfx.DrawString( A2W( m_sRightButtonText.c_str() ), m_sRightButtonText.length(), Theme::MouseHelpFont, m_rectRightText, &format, Theme::MouseHelpBrush  );

  // adjust string aligment for key text
  format.SetLineAlignment( Gdiplus::StringAlignmentCenter );
  format.SetAlignment( Gdiplus::StringAlignmentCenter );

  // shift key state
  GdiplusUtilities::DrawRoundedRectangle( gfx, pen, m_rectShiftKeyText, iBevel );
  gfx.DrawString( L"SHIFT", 5, Theme::MouseHelpKeyFont, m_rectShiftKeyText, &format, m_bShiftKeyEnabled? Theme::MouseHelpKeyEnabledBrush :   Theme::MouseHelpKeyBrush );

  // control key state
  GdiplusUtilities::DrawRoundedRectangle( gfx, pen, m_rectControlKeyText, iBevel );
  gfx.DrawString( L"CTRL", 4, Theme::MouseHelpKeyFont, m_rectControlKeyText, &format, m_bControlKeyEnabled? Theme::MouseHelpKeyEnabledBrush :   Theme::MouseHelpKeyBrush );
  
  Invalidate();
} // RedrawControl()


// $Log: MouseHelp.C,v $
// Revision 1.7  2007/03/12 20:47:11  jmeade
// code standards.
//
// Revision 1.6  2007/03/01 21:40:09  geconomos
// code review preparation
//
// Revision 1.5  2007/02/15 00:08:21  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.4  2006/06/01 20:21:19  frank
// updated to visual studio 2005
//
// Revision 1.3.2.1  2006/05/16 12:37:07  geconomos
// correct usuage of texture brushes when drawing control
//
// Revision 1.3  2005/10/06 14:15:33  geconomos
// switched the position of the shift and control text
//
// Revision 1.2  2005/10/04 01:51:23  geconomos
// corrected placement of left mouse button image
//
// Revision 1.1  2005/10/03 12:57:54  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MouseHelp.C,v 1.7 2007/03/12 20:47:11 jmeade Exp $
// $Id: MouseHelp.C,v 1.7 2007/03/12 20:47:11 jmeade Exp $
