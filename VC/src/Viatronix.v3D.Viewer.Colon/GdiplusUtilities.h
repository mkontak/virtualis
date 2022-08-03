// $Id: GdiplusUtilities.h,v 1.4 2006/01/27 20:31:37 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (george@viatronix.com)

// pragma declarations
#pragma once

// includes
#include "Image.h"

OPEN_WINUTILS_NS

// class delcaration
class GdiplusUtilities
{
// member functions
public:

  /// draws an image using the specified opacity
  static void DrawImageTransparent( Gdiplus::Graphics & gfx, Gdiplus::Image * pImage, int4 x, int4 y, float4 fOpacity );

  /// fits an image to the specfied rectangle preserving the aspect ratio
  static void DrawImageScaled( Gdiplus::Graphics & gfx, Gdiplus::Image * pImage, Gdiplus::Rect bounds, float4 fOpacity = 1.0F );

  /// draws a rectangle with rounded edges
  static void DrawRoundedRectangle( Gdiplus::Graphics & gfx,  Gdiplus::Pen & pen, Gdiplus::RectF & rect, int4 iBevel );

  // creates a gdi+ Bitmap instance from the specified resource image
  static Gdiplus::Bitmap * LoadBitmapFromResource( const char * sName, const char * sType, HMODULE hInstance = AfxGetResourceHandle() );

  // creates a viatronix image from the specified resource image
  static Image< PixelRGBA< uint1 > > * LoadImageFromResource( const char * sName, const char * sType, HMODULE hInstance = AfxGetResourceHandle() );

  // creates an HBITMAP from the specified image file
  static HBITMAP LoadHBITMAPFromFile( const char * sFilename);

  ///  get the next multiple of two numbers
  inline static int4 GetNextMultiple( int4 n, int4 m ) { return ( ( n + m ) / m )* m; } 

}; // class GdiplusUtils

CLOSE_WINUTILS_NS

// $Log: GdiplusUtilities.h,v $
// Revision 1.4  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.3.2.1  2006/01/05 19:15:00  jmeade
// transparency on draw-scaled-image fn.
//
// Revision 1.3  2005/10/04 13:44:09  geconomos
// added a method to return a vx image from a resource
//
// Revision 1.2  2005/10/03 15:00:39  geconomos
// Added LoadHBITMAPFromFile method
//
// Revision 1.1  2005/10/03 12:57:54  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GdiplusUtilities.h,v 1.4 2006/01/27 20:31:37 jmeade Exp $
// $Id: GdiplusUtilities.h,v 1.4 2006/01/27 20:31:37 jmeade Exp $
