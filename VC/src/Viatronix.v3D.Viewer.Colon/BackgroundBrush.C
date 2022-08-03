// $Id: BackgroundBrush.C,v 1.4 2007/03/01 21:40:08 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// includes
#include "stdafx.h"
#include "BackgroundBrush.h"

#define FILE_REVISION "$Revision: 1.4 $"

using namespace WinUtils;


/**
 * Constructor
 */
BackgroundBrush::BackgroundBrush() : 
  m_hBitmap( NULL ),
  m_hBrush( NULL )
{
} // BackgroundBrush()


/**
 * Destructor
 */
BackgroundBrush::~BackgroundBrush()
{
  Reset();
}  // ~BackgroundBrush()


/**
 * Returns the resource image cache
 *
 * @return  Static resource image cache
 */
BackgroundBrush::ResourceImageCache & BackgroundBrush::GetResourceImageCache()
{
  static BackgroundBrush::ResourceImageCache cache;
  return cache;
} // GetResourceImageMap


/**
 * Creates a new BackgroundBrush using the specified CWnd and resource id.
 *
 * @param   pWnd          CWnd owning this background brush.
 * @param   iResourceId   Resource id of background image.
 */
bool BackgroundBrush::Create( CWnd * pWnd, int4 iResourceId )
{  
  Gdiplus::Status status( Gdiplus::Ok );

  // reset to default state
  Reset();

  // check if the resource image is in the already cache?
  if( GetResourceImageCache().find( iResourceId ) ==  GetResourceImageCache().end()  )
  {
    // load the resource
    Gdiplus::Bitmap * pBitmap = new Gdiplus::Bitmap( AfxGetInstanceHandle(), MAKEINTRESOURCEW( iResourceId ) );
    if( ( status = pBitmap->GetLastStatus() )!= Gdiplus::Ok )
    {
      LogErr( "Constructor for resource bitmap failed. GetLastStatus: " + ToAscii( (int) status ), "BackgroundBrush", "Create" );
      return false;
    }
    
    // add the image to the cache
    GetResourceImageCache()[ iResourceId ] = pBitmap;
  }

  // grab the resource image from the cache
  Gdiplus::Bitmap * pResourceBitmap = GetResourceImageCache()[ iResourceId ];

  // get the entire client area of the parent CWnd
  CRect rc; pWnd->GetClientRect( &rc );
  
  // create a bitmap whose size matches the CWnd's 
  Gdiplus::Bitmap bitmap( rc.Width(), rc.Height(), PixelFormat24bppRGB );  
  if( ( status = bitmap.GetLastStatus() ) != Gdiplus::Ok )
  {
    LogErr( "Constructor for new bitmap failed. GetLastStatus: " + ToAscii( (int) status ), "BackgroundBrush", "Create" );
    return false;
  }

  // set the source rectangle ( scale x and crop y )
  Gdiplus::RectF rcSource( 0, 0,  pResourceBitmap->GetWidth(), min( pResourceBitmap->GetHeight(), rc.Height() ) );

  // set the destination rectangle ( scale x and crop y )
  Gdiplus::RectF rcDestination( 0, 0,  rc.Width(), rc.Height() );

  // select the bitmap into the graphics object
  Gdiplus::Graphics gfx( &bitmap );  
  if( ( status = gfx.GetLastStatus() ) != Gdiplus::Ok )
  {
    LogErr( "Constructor for graphics failed. GetLastStatus: " + ToAscii( (int) status ), "BackgroundBrush", "Create" );
    return false;
  }
  
  // resize
  gfx.DrawImage( pResourceBitmap, rcDestination, rcSource.X, rcSource.Y, rcSource.Width, rcSource.Height, Gdiplus::UnitPixel );
  if( ( status = gfx.GetLastStatus() ) != Gdiplus::Ok )
  {
    LogErr( "Resize failed. GetLastStatus: " + ToAscii( (int) status ), "BackgroundBrush", "Create" );
    return false;
  }
  
  // create an HBITMAP 
  bitmap.GetHBITMAP( Gdiplus::Color::Black, &m_hBitmap );
  if( m_hBitmap == NULL )
  {
    LogErr( "GetHBITMAP() failed. GetLastError: " + ToAscii( GetLastError() ), "BackgroundBrush", "Create" );
    return false;
  }

  // create a pattern brush
  m_hBrush = CreatePatternBrush( m_hBitmap );
  if( m_hBrush  == NULL )
  {
    LogErr( "CreatePatternBrush() failed. GetLastError: " + ToAscii( GetLastError() ), "BackgroundBrush", "Create" );
    return false;
  }
  return true;
} // Create() 


/**
 * Resets the brush to a default state
 */
void BackgroundBrush::Reset()
{
  // free brush
  if( m_hBrush != NULL )
  {
    DeleteObject( m_hBrush );
    m_hBrush = NULL;
  }
  
  // free bitmap
  if( m_hBitmap != NULL )
  {
    DeleteObject( m_hBitmap );
    m_hBitmap = NULL;
  }
} // Reset()


// $Log: BackgroundBrush.C,v $
// Revision 1.4  2007/03/01 21:40:08  geconomos
// code review preparation
//
// Revision 1.3  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.2  2005/08/16 22:56:12  jmeade
// namespaces for WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.3  2005/06/23 14:55:33  geconomos
// Issue# 4182: "x" in Viatronix logo no longer intersects with the group box
//
// Revision 1.1.2.2  2005/06/22 19:46:57  geconomos
// reimplemented using HBITMAP and HBRUSH (no MFC)
//
// Revision 1.1.2.1  2005/06/17 13:02:09  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/BackgroundBrush.C,v 1.4 2007/03/01 21:40:08 geconomos Exp $
// $Id: BackgroundBrush.C,v 1.4 2007/03/01 21:40:08 geconomos Exp $
