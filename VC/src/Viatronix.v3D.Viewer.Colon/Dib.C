// $Id: Dib.C,v 1.2 2005/08/16 22:02:28 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (george@viatronix.com)

// includes
#include "stdafx.h"
#include "Dib.h"

using namespace WinUtils;

/**
 * Default constructor.
 */
Dib::Dib() : 
  m_hdc( NULL ),
  m_hBitmap( NULL ),
  m_hBitmapOld( NULL ),
  m_puBits( NULL ),
  m_iWidth( 0 ),
  m_iHeight( 0 )
{
} // Dib()


/**
 * Virtual destructor.
 */
Dib::~Dib()
{
  Destroy();
} // ~Dib()


/**
 * Creates a dib with the specified width and height.
 */
void Dib::Create( uint4 uWidth, uint4 uHeight )
{
   // destroy parts of objects if we are recreating it
   if( ( m_hdc != NULL ) || ( m_hBitmap != NULL ) ) 
    Destroy();
  
   // create a BITMAPINFOHEADER structure to describe the DIB
   BITMAPINFOHEADER bih ;
   memset( &bih, 0, sizeof( BITMAPINFOHEADER ));

   // fill in the header info
   bih.biSize = sizeof( BITMAPINFOHEADER );
   bih.biWidth = m_iWidth = uWidth;
   bih.biHeight = m_iHeight = uHeight;
   bih.biSizeImage = ( ( (uWidth * 24 + 31) & (~31) ) / 8 ) * m_iHeight;
   bih.biPlanes = 1;
   bih.biBitCount = 24;
   bih.biCompression = BI_RGB;

   // create a new DC
   m_hdc = CreateCompatibleDC( NULL );

   // create the DIB section.
   m_hBitmap = CreateDIBSection( m_hdc, (BITMAPINFO*)&bih, DIB_PAL_COLORS, (void**)&m_puBits, NULL, 0 );

  // select the new bitmap into the buffer DC
  if( m_hBitmap )
      m_hBitmapOld = (HBITMAP)SelectObject( m_hdc, m_hBitmap );
} // Create( uint4 uWidth, uint4 uHeight );


/**
 * Frees all resources associated with the dib.
 */
void Dib::Destroy()
{
  if( m_hBitmapOld )
    SelectObject( m_hdc, m_hBitmapOld );

  // delete bitmap
  if( m_hBitmap ) 
    DeleteObject( m_hBitmap );

  // delete DC
  if( m_hdc ) 
    ReleaseDC( NULL, m_hdc );
} // Destroy()


// $Log: Dib.C,v $
// Revision 1.2  2005/08/16 22:02:28  jmeade
// namespaces for WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.3  2005/07/20 15:53:47  geconomos
// Issue# 4286: Snapshot crash with DrawOverImage.
//
// Revision 1.1.2.2  2005/06/23 18:50:27  geconomos
// Issue #4189: Added width and height properties
//
// Revision 1.1.2.1  2005/06/21 15:12:53  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Dib.C,v 1.2 2005/08/16 22:02:28 jmeade Exp $
// $Id: Dib.C,v 1.2 2005/08/16 22:02:28 jmeade Exp $
