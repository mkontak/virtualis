// $Id: vxDib.C,v 1.2 2007/10/24 20:10:50 gdavidson Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (george@viatronix.com)

// includes
#include "stdafx.h"
#include "vxdib.h"

/**
 * Default constructor.
 */
vxDib::vxDib() : 
  m_hdc( NULL ),
  m_hBitmap( NULL ),
  m_hBitmapOld( NULL ),
  m_puBits( NULL ),
  m_uWidth( 0 ),
  m_uHeight( 0 )
{
} // vxDib()


/**
 * Virtual destructor.
 */
vxDib::~vxDib()
{
  Destroy();
} // ~vxDib()


/**
 * Creates a dib with the specified width and height.
 */
void vxDib::Create( uint4 uWidth, uint4 uHeight )
{
   // destroy parts of objects if we are recreating it
   if( ( m_hdc != NULL ) || ( m_hBitmap != NULL ) ) 
    Destroy();

   m_uWidth = uWidth;
   m_uHeight = uHeight;

   // create a BITMAPINFOHEADER structure to describe the DIB
   BITMAPINFOHEADER bih ;
   memset( &bih, 0, sizeof( BITMAPINFOHEADER ));

   // fill in the header info
   bih.biSize = sizeof( BITMAPINFOHEADER );
   bih.biWidth = uWidth;
   bih.biHeight = uHeight;
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
void vxDib::Destroy()
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


// $Log: vxDib.C,v $
// Revision 1.2  2007/10/24 20:10:50  gdavidson
// Added witdth and height accessor methods
//
// Revision 1.1  2004/03/30 17:24:02  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxDib.C,v 1.2 2007/10/24 20:10:50 gdavidson Exp $
// $Id: vxDib.C,v 1.2 2007/10/24 20:10:50 gdavidson Exp $
