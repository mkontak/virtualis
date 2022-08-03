// $Id: ImageUtilities.C,v 1.2 2005/10/05 17:42:27 vxconfig Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

// Includes
#include "stdafx.h"
#include "ImageUtilities.h"
#include "v3DFile.h"


/**
 * Convert an image from RGBA to RGB
 *
 * @param outImage the output RGB image
 * @param inImage  the input RGBA image
 */
void ImageUtilities::ConvertImage( Image< PixelRGB< uint1 > > & outImage, const Image< PixelRGBA< uint1 > > & inImage )
{

  outImage.Resize( inImage.GetDim() );

  // loop over all images pixels
  for ( int4 y(0); y<outImage.GetDim().m_y; ++y )
  {
    for ( int4 x(0); x<outImage.GetDim().m_x; ++x )
    {
      const Point2D< uint2 > pos( x, y );
      const PixelRGBA< uint1 > & srcPixel( inImage.GetPixelRaw( pos ) );
      const PixelRGB< uint1 > destPixel( srcPixel.R(), srcPixel.G(), srcPixel.B() );
      outImage.SetPixelRaw( pos, destPixel );
    } // for x
  } // for y

} // ConvertImage()


/**
 * Rotate an RGB image counter-clockwise 90 degrees
 *
 * @param outImage the output RGB image
 * @param inImage  the input RGB image
 */
void ImageUtilities::RotateImageCCW90( Image< PixelRGB< uint1 > > & outImage, const Image< PixelRGB< uint1 > > inImage )
{

  outImage.Resize( Triple< uint4 >( inImage.GetDim().m_y, inImage.GetDim().m_x, 0 ) );

  // loop over all images pixels
  for ( int4 y(0); y<outImage.GetDim().m_y; ++y )
  {
    for ( int4 x(0); x<outImage.GetDim().m_x; ++x )
    {
      const Point2D< uint2 > pos( y, x );
      const PixelRGB< uint1 > & srcPixel(inImage.GetPixelRaw( pos ));
      const PixelRGB< uint1 > destPixel( srcPixel.R(), srcPixel.G(), srcPixel.B() );
      outImage.SetPixelRaw( pos, destPixel );
    } // for x
  } // for y

} // RotateImageCCW90()


/**
 * Rotate an RGB image counter-clockwise 90 degrees
 *
 * @param outImage the output RGBA image
 * @param inImage  the input RGBA image
 */
void ImageUtilities::RotateImageCCW90( Image< PixelRGBA< uint1 > > & outImage, const Image< PixelRGBA< uint1 > > inImage )
{

  outImage.Resize( Triple< uint4 >( inImage.GetDim().m_y, inImage.GetDim().m_x, 0 ) );

  // loop over all images pixels
  for ( int4 y(0); y<outImage.GetDim().m_y; ++y )
  {
    for ( int4 x(0); x<outImage.GetDim().m_x; ++x )
    {
      const Point2D< uint2 > pos( y, x );
      const PixelRGBA< uint1 > destPixel( inImage.GetPixelRaw( pos ) );
      outImage.SetPixelRaw( pos, destPixel );
    } // for x
  } // for y

} // RotateImageCCW90()



/**
 * Flip an image vertically
 *
 * @param image  the input and output RGBA image
 */
void ImageUtilities::FlipVertically( Image< PixelRGBA< uint1 > > & image )
{

  // loop over all images pixels
  for ( int4 y(0); y<image.GetDim().m_y / 2; ++y )
  {
    for ( int4 x(0); x<image.GetDim().m_x; ++x )
    {
      const Point2D< uint2 > pos1( y, x );
      const Point2D< uint2 > pos2( image.GetDim().m_y - 1 - y, x );
      const PixelRGBA< uint1 > tempPixel( image.GetPixelRaw( pos1 ) );
      image.SetPixelRaw( pos1, image.GetPixelRaw( pos2 ) );
      image.SetPixelRaw( pos2, tempPixel );
    } // for x
  } // for y

} // FlipVertically()


/**
 * Write an RGB image to a PNG file
 *
 * @param image     the input RGB image
 * @param sFilename the output filename
 */
void ImageUtilities::WritePngImage( const Image< PixelRGB< uint1 > > & image, const std::string & sFilename )
{

  V3DFile img;
  img.SetImage( image );
  img.Write( sFilename );

} // WritePngImage()


/**
 * Write an RGBA image to a PNG file.  It does not yet write the transparency
 * portion of the image.
 *
 * @param image     the input RGBA image
 * @param sFilename the output filename
 */
void ImageUtilities::WritePngImage( const Image< PixelRGBA< uint1 > > & image, const std::string & sFilename )
{

  Image< PixelRGB< uint1 > > rgbImage( PixelRGB< uint1 >( 0, 0, 0 ) );
  ConvertImage( rgbImage, image );
  WritePngImage( rgbImage, sFilename );

} // WritePngImage()


// $Log: ImageUtilities.C,v $
// Revision 1.2  2005/10/05 17:42:27  vxconfig
// added flip utility
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/12/15 16:07:12  frank
// Exported the functions so they can be used outside of this DLL
//
// Revision 1.5  2003/06/10 12:06:40  frank
// Code review of pixel classes
//
// Revision 1.4  2003/05/08 20:41:22  geconomos
// Fixed complie error.
//
// Revision 1.3  2003/05/08 20:14:04  michael
// code reviews
//
// Revision 1.2  2003/05/08 17:50:37  frank
// code review
//
// Revision 1.1  2003/03/06 16:14:51  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/ImageUtilities.C,v 1.2 2005/10/05 17:42:27 vxconfig Exp $
// $Id: ImageUtilities.C,v 1.2 2005/10/05 17:42:27 vxconfig Exp $
