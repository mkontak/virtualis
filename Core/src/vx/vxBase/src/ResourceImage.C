// $Id: ResourceImage.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)


// includes
#include "stdafx.h"
#include "ResourceImage.h"
#include "Exception.h"


/**
 * Class to manage the GDI plus engine
 */
class GdiplusEngine
{
// member functions
public:
  
  /// default constructor
  GdiplusEngine()
  {
    Gdiplus::GdiplusStartupInput gsi;
    Gdiplus::GdiplusStartup( & m_pToken, & gsi, NULL );
  } 
  
  /// destructor
  ~GdiplusEngine()
  {
    Gdiplus::GdiplusShutdown(m_pToken);
  }

// data members
private:
  
  /// Gdiplus initialization token
  ULONG_PTR m_pToken;

}; // class GdiplusEngine


HMODULE ResourceImage::m_hModuleHandle = 0;


/**
 * Loads the specified image
 *
 * @param sResource     the desired resource
 * @param hModuleHandle the module handle
 * @return              the desired image
 */
Image< PixelRGBA<uint1> > * ResourceImage::FromResource( const std::string & sResource, HMODULE hModuleHandle )
{
  GdiplusEngine engine;

  HMODULE hModule = hModuleHandle == NULL? m_hModuleHandle : hModuleHandle;
  
  if( hModule == NULL )
    return NULL;

  HRSRC hrsrc = FindResource( hModule, sResource.c_str(), "PNG" );  
  if (hrsrc == NULL)
    return NULL;
  
  // size of the resource in bytes
  DWORD dwSize = SizeofResource( hModule, hrsrc );
  
  // according to the documentation you do not have to release the loaded resource
  HGLOBAL hResData = LoadResource( hModule, hrsrc );
  if( hResData == NULL )
    return NULL;
  
  // allocate global memory to hold the resource
  HGLOBAL hGlobal = GlobalAlloc( GHND, dwSize );

  if ( hGlobal == NULL )
    throw ex::OutOfMemoryException( LogRec( "Unable allocate global memeory for resource (" + ToAscii( GetLastError() ) + ")", "ResourceImage", "FromResource" ) );
  
  // copy to global memory
  uint1* puDest = reinterpret_cast<uint1*>( GlobalLock( hGlobal ) );
  uint1* puSource = reinterpret_cast<uint1*>(LockResource(hResData));
  
  memcpy(puDest, puSource, dwSize);
  
  GlobalUnlock(hGlobal);
  
  // The following will release the HGLOBAL when the stream interface is released
  IStream* pStream;
  CreateStreamOnHGlobal( hGlobal, TRUE, &pStream );
  
  // create the gdiplus image
  Gdiplus::Bitmap * pImage = Gdiplus::Bitmap::FromStream( pStream );
  if(pImage == NULL)
    return NULL;
  
  uint4 uWidth( pImage->GetWidth() );
  uint4 uHeight( pImage->GetHeight() );
  
  // create a vx image
  Image< PixelRGBA<uint1> > * pVXImage = new Image< PixelRGBA<uint1> >(Triple<uint4>(uWidth, uHeight, 0), PixelRGBA<uint1>(0,0,0,255));

  // copy the image
  for(uint4 y(0); y < uHeight; y++) 
  {

    for(uint4 x(0); x < uWidth; x++) 
    {

      // set the pixel
      Gdiplus::Color color;
      pImage->GetPixel( x, y, & color );
      PixelRGBA<uint1> pixel( color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
      pVXImage->SetPixel( Point2D<uint2>( x, y ), pixel );
      
    } // for each column

  }  // for each scanline

  // free the gdiplus image
  delete pImage;

  // return the vx image 
  return pVXImage;

} // FromResourceString()


// $Log: ResourceImage.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2003/05/16 16:47:54  frank
// code review
//
// Revision 1.12  2003/05/16 11:44:23  michael
// code review
//
// Revision 1.11  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 1.10  2003/03/12 15:29:00  geconomos
// Added inclusion of ComDef.h
//
// Revision 1.9  2003/02/26 19:26:40  geconomos
// Added HMODULE parameter to FromResource
//
// Revision 1.8  2003/01/22 21:20:59  ingmar
// synchronized to new Point2D access to Image pixels
//
// Revision 1.7  2002/09/19 18:21:24  geconomos
// Changed module handle type from uint4 to HMODULE.
//
// Revision 1.6  2002/03/21 16:22:11  michael
// added author and comments
//
// Revision 1.5  2002/03/21 16:00:58  geconomos
// Added Gdiplus initialization.
//
// Revision 1.4  2002/03/20 14:49:22  geconomos
// Changed class to contain single static method for loading images from the resource.
//
// Revision 1.3  2002/03/18 16:17:43  jenny
// GetModuleHandle("v3D.Visualization.Debug.dll")
//
// Revision 1.2  2002/01/22 05:24:40  michael
// added orientation cube images (png)
//
// Revision 1.1  2002/01/21 22:01:25  dmitry
// Initial Version
//
// $Id: ResourceImage.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/ResourceImage.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
