// $Id: GdiplusUtilities.C,v 1.9 2007/03/01 21:40:08 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (george@viatronix.com)


// includes
#include "StdAfx.h"
#include "GdiplusUtilities.h"


// defines
#define FILE_REVISION "$Revision: 1.9 $"


// namespaces
using namespace WinUtils;


/**
 * Draws an image using the specified opacity
 *
 * @param   gfx         graphics object to draw to
 * @param   pImage      image to draw
 * @param   x           x position to draw to 
 * @param   y           y position to draw to 
 * @param   fOpacity    opacity value (0-1)
 */
void GdiplusUtilities::DrawImageTransparent( Gdiplus::Graphics & gfx, Gdiplus::Image * pImage, int4 x, int4 y, float4 fOpacity )
{
  /// create color-adjustment matrix
  Gdiplus::ColorMatrix matrix  = {  1,    0,    0,    0,          0,
                                    0,    1,    0,    0,          0,
                                    0,    0,    1,    0,          0,
                                    0,    0,    0,    fOpacity,   0, 
                                    0,    0,    0,    0,          1}; 

  //  create attributes and set the matrix
  Gdiplus::ImageAttributes attributes;  
  attributes.SetColorMatrix( &matrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap );

  /// draw the image using the created attributes
  gfx.DrawImage( pImage, Gdiplus::Rect( x, y, pImage->GetWidth(), pImage->GetHeight() ), 
    0, 0, pImage->GetWidth(), pImage->GetHeight(), Gdiplus::UnitPixel, &attributes );

} // DrawImageTransparent( Graphics gfx, Image image, int x, int y, float opacity )


/**
 * Fits an image to the specified rectangle preserving the aspect ratio
 *
 * @param   gfx         graphics object to draw to
 * @param   pImage      image to fit
 * @param   rect        rectangle to fit image to 
 * @param   fOpacity    opacity value (0-1)
 */
void GdiplusUtilities::DrawImageScaled( Gdiplus::Graphics & gfx, Gdiplus::Image * pImage, Gdiplus::Rect rect, float4 fOpacity )
{
  // size of source image
  Gdiplus::SizeF size( pImage->GetWidth()  / pImage->GetHorizontalResolution(), pImage->GetHeight() /pImage->GetVerticalResolution() );

  // scale and keep aspect ratio of source image
  float fScale = min( rect.Width / size.Width, rect.Height / size.Height );
  size.Width *= fScale;
  size.Height *= fScale;

  // image layout in pixels
  Gdiplus::RectF layout( rect.X + ( rect.Width  - size.Width )  / 2 ,
                         rect.Y + ( rect.Height - size.Height ) / 2,
                         size.Width, 
                         size.Height );

  /// create color-adjustment matrix
  Gdiplus::ColorMatrix matrix  = {
    1,    0,    0,    0,          0,
    0,    1,    0,    0,          0,
    0,    0,    1,    0,          0,
    0,    0,    0,    fOpacity,   0, 
    0,    0,    0,    0,          1}; 

  //  create attributes and set the matrix
  Gdiplus::ImageAttributes attributes;  
  attributes.SetColorMatrix( &matrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap );

  // draw the image into the layout
  gfx.DrawImage( pImage, layout.X, layout.Y, layout.Width, layout.Height );
  
} // DrawImageScaled( gfx, image, rect, selected, fOpacity )


/**
 * Draws a rectangle with rounded edges
 *
 * @param   gfx       graphics object to draw to
 * @param   pen       pen for drawing
 * @param   rect      rectangle to draw
 * @param   iBevel    corner bevel
 */
void GdiplusUtilities::DrawRoundedRectangle( Gdiplus::Graphics & gfx, Gdiplus::Pen & pen, Gdiplus::RectF & rect, int iBevel )
{
  // define constants
  static float8 PI = 3.1415926573;
  static float4 COS_3PI_OVER_8 = cos( 3*PI/8 );
  static float4 SIN_3PI_OVER_8 = sin( 3*PI/8 );
  static float4 COS_PI_OVER_4  = cos( PI/4 );
  static float4 SIN_PI_OVER_4  = sin( PI/4 );
  static float4 COS_PI_OVER_8  = cos( PI/8 );
  static float4 SIN_PI_OVER_8  = sin( PI/8 ); 

  const int4 NUMBER_OF_POINTS = 19;
  Gdiplus::PointF points[ NUMBER_OF_POINTS ];
  int iIndex( 0 );
    
  // top right corner
  float xCenterTopRight( rect.GetRight() - iBevel );
  float yCenterTopRight( rect.GetTop() + iBevel );  
  points[ iIndex++ ] = Gdiplus::PointF( rect.GetRight() - iBevel + 1, rect.GetTop() );
  points[ iIndex++ ] = Gdiplus::PointF( xCenterTopRight + iBevel * COS_3PI_OVER_8, yCenterTopRight - iBevel * SIN_3PI_OVER_8 );
  points[ iIndex++ ] = Gdiplus::PointF( xCenterTopRight + iBevel * COS_PI_OVER_4,  yCenterTopRight - iBevel * SIN_PI_OVER_4 );
  points[ iIndex++ ] = Gdiplus::PointF( xCenterTopRight + iBevel * COS_PI_OVER_8,  yCenterTopRight - iBevel * SIN_PI_OVER_8 );


  // bottom right corner
  float xCenterBottomRight( rect.GetRight()  - iBevel );
  float yCenterBottomRight( rect.GetBottom() - iBevel );
  points[ iIndex++ ] = Gdiplus::PointF( rect.GetRight(), rect.GetBottom() - iBevel - 1 );
  points[ iIndex++ ] = Gdiplus::PointF( xCenterBottomRight + iBevel * COS_PI_OVER_8,  yCenterBottomRight + iBevel * SIN_PI_OVER_8 );
  points[ iIndex++ ] = Gdiplus::PointF( xCenterBottomRight + iBevel * COS_PI_OVER_4,  yCenterBottomRight + iBevel * SIN_PI_OVER_4 );
  points[ iIndex++ ] = Gdiplus::PointF( xCenterBottomRight + iBevel * COS_3PI_OVER_8, yCenterBottomRight + iBevel * SIN_3PI_OVER_8 );
  points[ iIndex++ ] = Gdiplus::PointF( rect.GetRight() - iBevel, rect.GetBottom() );


  // bottom left corner
  float xCenterBottomLeft = rect.GetLeft() + iBevel;
  float yCenterBottomLeft = rect.GetBottom() - iBevel;
  points[ iIndex++ ] = Gdiplus::PointF( rect.GetLeft() + iBevel, rect.GetBottom() );
  points[ iIndex++ ] = Gdiplus::PointF( xCenterBottomLeft - iBevel * COS_3PI_OVER_8, yCenterBottomLeft + iBevel * SIN_3PI_OVER_8 );
  points[ iIndex++ ] = Gdiplus::PointF( xCenterBottomLeft - iBevel * COS_PI_OVER_4,  yCenterBottomLeft + iBevel * SIN_PI_OVER_4 );
  points[ iIndex++ ] = Gdiplus::PointF( xCenterBottomLeft - iBevel * COS_PI_OVER_8,  yCenterBottomLeft + iBevel * SIN_PI_OVER_8 );
  points[ iIndex++ ] = Gdiplus::PointF( rect.GetLeft(), rect.GetBottom() - iBevel );
  
  // top left corner
  float xCenterTopLeft = rect.GetLeft() + iBevel;
  float yCenterTopLeft = rect.GetTop() + iBevel;
  points[ iIndex++ ] = Gdiplus::PointF( rect.GetLeft(), rect.GetTop() + iBevel );
  points[ iIndex++ ] = Gdiplus::PointF( xCenterTopLeft - iBevel * COS_PI_OVER_8,  yCenterTopLeft - iBevel * SIN_PI_OVER_8 );
  points[ iIndex++ ] = Gdiplus::PointF( xCenterTopLeft - iBevel * COS_PI_OVER_4,  yCenterTopLeft - iBevel * SIN_PI_OVER_4 );
  points[ iIndex++ ] = Gdiplus::PointF( xCenterTopLeft - iBevel * COS_3PI_OVER_8, yCenterTopLeft - iBevel * SIN_3PI_OVER_8 );
  points[ iIndex++ ] = Gdiplus::PointF( rect.GetLeft() + iBevel,  rect.GetTop() );

  gfx.DrawPolygon( &pen, points, NUMBER_OF_POINTS );
} // DrawRoundedRectangle( Gdiplus::Graphics & gfx, Gdiplus::Pen & pen, Gdiplus::RectF & rect, int iBevel )



/**
 * Creates a Bitmap instance from the specified resource image
 *
 * @param   sName       resource name
 * @param   sType       resource type
 * @param   hInstance   handle to instance containing requested resource
 * @return  bitmap instance on success; NULL otherwise
 */
Gdiplus::Bitmap * GdiplusUtilities::LoadBitmapFromResource( const char * sName, const char * sType, HMODULE hInstance )
{
  // locate the resource in the block
  HRSRC hResource ( ::FindResource( hInstance, sName, sType ) );
  if( hResource == NULL )
  {
    throw ex::ResourceImageException( LogRec( "Unable to locate resource in block", "ResourceLoader", "Load" ) );
  }

  // get the size of the resource ( in bytes )
  DWORD resourceSize = ( ::SizeofResource( hInstance, hResource ) );
  if( resourceSize == 0 )
  {
    throw ex::ResourceImageException( LogRec( "Invalid resource size", "ResourceLoader", "Load" ) );
  }
    
  // lock the resource
  const void * pData = ::LockResource( ::LoadResource( hInstance, hResource ) );
  if( pData == NULL )
  {
    throw ex::ResourceImageException( LogRec( "Unable to lock down the resource", "ResourceLoader", "Load" ) );
  }

  // allocate a buffer to copy the resource data into
  HGLOBAL hBuffer  = ::GlobalAlloc( GMEM_MOVEABLE, resourceSize );
  if( hBuffer == NULL )
  {
    throw ex::ResourceImageException( LogRec( "Unable to allocate buffer handler.", "ResourceLoader", "Load" ) );
  }

  // lock the allocated buffer and grap a pointer to the actual data
  void* pBuffer = ::GlobalLock( hBuffer );
  if( pBuffer == NULL )
  {
    throw ex::ResourceImageException( LogRec( "Unable to allocate buffer handler.", "ResourceLoader", "Load" ) );
  }

  // copy the memry from the resource data to the buffer
  CopyMemory( pBuffer, pData, resourceSize );

  //  create an IStream interface based on the allocated memory
  CComPtr< IStream >  spStream;
  if( FAILED( ::CreateStreamOnHGlobal( hBuffer, TRUE, &spStream ) ) )
  {
    throw ex::ResourceImageException( LogRec( "Unable to allocate buffer handler.", "ResourceLoader", "Load" ) );
  }

  // load the gdiplus bitmap from the IStream
  Gdiplus::Bitmap * pBitmap = Gdiplus::Bitmap::FromStream( spStream );

  // unlock the allocated buffer
  GlobalUnlock( hBuffer );

  // NOTE: No need to free hBuffer, when spStream is released the handle is released.
  //       Boundschecker reports and error nonetheless...

  return pBitmap;
} // LoadBitmap( const char * sName, const char * sType, HMODULE hInstance )


/**
 * Creates a viatronix image from the specified resource
 *
 * @param   sName       resource name
 * @param   sType       resource type
 * @param   hInstance   handle to instance containing requested resource
 * @return  Image< PixelRGBA< uint1> > instance on success; NULL otherwise
 */
Image< PixelRGBA< uint1 > > * GdiplusUtilities::LoadImageFromResource( const char * sName, const char * sType, HMODULE hInstance )
{
  Gdiplus::Bitmap * pBitmap = LoadBitmapFromResource( sName, sType, hInstance );
  if( pBitmap == NULL )
  {
    return NULL;
  }

   // create a vx image
  Image< PixelRGBA<uint1> > * pVXImage = new Image< PixelRGBA<uint1> >(Triple<uint4>(pBitmap->GetWidth(), pBitmap->GetHeight(), 0), PixelRGBA<uint1>(0,0,0,255));

  // copy the image
  for(uint4 y(0); y < pBitmap->GetHeight(); ++y) 
  {
    for(uint4 x(0); x < pBitmap->GetWidth(); ++x) 
    {
      // set the pixel
      Gdiplus::Color color;
      pBitmap->GetPixel( x, y, & color );
      PixelRGBA<uint1> pixel( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
      pVXImage->SetPixel( Point2D<uint2>( x, y ), pixel );
    } // for each column
  }  // for each scanline

  delete pBitmap;

  return pVXImage;
} // LoadImageFromResource( const char * sName, const char * sType, HMODULE hInstance )


/**
 * Creates an HBITMAP from the specified image file
 *
 * @param   sFilename   image filename
 * @return  HBITMAP on success; NULL otherwise
 */
HBITMAP GdiplusUtilities::LoadHBITMAPFromFile( const char * sFilename )
{
  USES_CONVERSION;

  // current gdi+ status
  Gdiplus::Status status( Gdiplus::Ok );
  
  // read in the image file
  Gdiplus::Bitmap bitmap( A2W( sFilename ) );
  if( ( status = bitmap.GetLastStatus() ) != Gdiplus::Ok )
  {
    LogErr( "Error reading image file. GetLastStatus: " + ToAscii( (int) status ), "GdiplusUtilities", "LoadHBITMAPFromFile" );
    return NULL;
  }

  // create an HBITMAP from the gdi+ bitmap
  HBITMAP hBitmap( NULL );
  bitmap.GetHBITMAP( Gdiplus::Color::White, & hBitmap );
  if( ( status = bitmap.GetLastStatus() ) != Gdiplus::Ok )
  {
    LogErr( "Error creating HBITMAP from gdi+ bitmap. GetLastStatus: " + ToAscii( (int) status ), "GdiplusUtilities", "LoadHBITMAPFromFile" );
    return NULL;
  }
  
  // return the HBITMAP handle
  return hBitmap;
} // LoadHBITMAPFromFile( const char * sFilename )


// $Log: GdiplusUtilities.C,v $
// Revision 1.9  2007/03/01 21:40:08  geconomos
// code review preparation
//
// Revision 1.8  2007/02/15 00:08:21  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.7  2006/06/01 20:20:14  frank
// updated to visual studio 2005
//
// Revision 1.6  2006/02/07 21:31:12  gdavidson
// Code Review
//
// Revision 1.5  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.4.2.4  2006/05/09 13:53:10  geconomos
// added comments
//
// Revision 1.4.2.3  2006/01/06 16:21:23  jmeade
// we'll have to worry about translucency later.
//
// Revision 1.4.2.2  2006/01/05 21:41:19  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.4.2.1  2006/01/05 19:15:00  jmeade
// transparency on draw-scaled-image fn.
//
// Revision 1.4  2005/10/04 13:44:09  geconomos
// added a method to return a vx image from a resource
//
// Revision 1.3  2005/10/03 15:00:39  geconomos
// Added LoadHBITMAPFromFile method
//
// Revision 1.2  2005/10/03 13:22:43  geconomos
// clean up
//
// Revision 1.1  2005/10/03 12:57:54  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GdiplusUtilities.C,v 1.9 2007/03/01 21:40:08 geconomos Exp $
// $Id: GdiplusUtilities.C,v 1.9 2007/03/01 21:40:08 geconomos Exp $
