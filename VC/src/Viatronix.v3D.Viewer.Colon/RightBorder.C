// $Id: RightBorder.C,v 1.3 2007/03/09 15:59:11 jmeade Exp $
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
#include "RightBorder.h"
#include "Theme.h"
#include <afxpriv.h>

// namespaces
using namespace Gdiplus;
using namespace WinUtils;

/**
 * Constructor
 */
RightBorder::RightBorder() : m_pDrawBuffer( NULL )
{
} // RightBorder()


/**
 * Destructor
 */
RightBorder::~RightBorder() 
{
  if( m_pDrawBuffer )
    delete m_pDrawBuffer;
} // ~RightBorder()


/**
 * MFC message map
 */
BEGIN_MESSAGE_MAP( RightBorder, CWnd )
  ON_WM_ERASEBKGND()
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_MESSAGE( WM_SIZEPARENT, OnSizeParent )
END_MESSAGE_MAP()


/**
 * WM_PAINT handler
 */
void RightBorder::OnPaint()
{
  CPaintDC dc( this );

  // draw the buffer to the screen
  if( m_pDrawBuffer != NULL )
  {
    Graphics gfx( dc.GetSafeHdc() );
    gfx.DrawImage( m_pDrawBuffer, 0, 0, m_pDrawBuffer->GetWidth(), m_pDrawBuffer->GetHeight() );
  } 
} // OnPaint()


/**
 * WM_ERASEBKGND handler, called when background needs erasing
 *
 * @param   pdc     Device context for window
 * @return          Non-zero to indicate NOT to erase, zero to let framework erase window
 */
BOOL RightBorder::OnEraseBkgnd( CDC * pdc )
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
void RightBorder::OnSize( UINT uType, int cx, int cy )
{
  CWnd::OnSize( uType, cx, cy );

  // free the previous background image
  if( m_pDrawBuffer != NULL )
  {
    delete m_pDrawBuffer;
    m_pDrawBuffer = NULL;
  }

  // create new drawing buffer based on new window size
  m_pDrawBuffer = new Bitmap( cx, cy, PixelFormat24bppRGB );

  // draw the right edge
  Graphics gfx( m_pDrawBuffer );

  TextureBrush brush( Theme::BkgndSideRight );
  //brush.TranslateTransform((cx - Theme::BkgndSideRight->GetWidth() ) % Theme::BkgndSideRight->GetWidth(), 0 );
  gfx.FillRectangle( &brush, 0, 0, Theme::BkgndSideRight->GetWidth(), cy );
 // brush.ResetTransform();
  

} // OnSize( UINT uType, int cx, int cy )


/**
 * WM_SIZEPARENT handler. Called by mfc framework when negotiating the client area size.
 *
 * @param   wParam      not used
 * @param   lParam      pointer to AFX_SIZEPARENTPARAMS 
 * @return              always 0
 */
LRESULT RightBorder::OnSizeParent( WPARAM wParam, LPARAM lParam )
{
  AFX_SIZEPARENTPARAMS * p = reinterpret_cast< AFX_SIZEPARENTPARAMS * >( lParam );

  p->rect.right -= Theme::BkgndSideRight->GetWidth();

  return 0;
} // OnSizeParent( WPARAM wParam, LPARAM lParam )


// $Log: RightBorder.C,v $
// Revision 1.3  2007/03/09 15:59:11  jmeade
// code standards.
//
// Revision 1.2  2007/03/02 14:33:04  geconomos
// code review preparation
//
// Revision 1.1  2005/10/03 12:57:54  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/RightBorder.C,v 1.3 2007/03/09 15:59:11 jmeade Exp $
// $Id: RightBorder.C,v 1.3 2007/03/09 15:59:11 jmeade Exp $
