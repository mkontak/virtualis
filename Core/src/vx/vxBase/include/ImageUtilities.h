// $Id: ImageUtilities.h,v 1.2 2005/10/05 17:42:27 vxconfig Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * define namespace ImageUtilities
 */

#ifndef ImageUtilities_h
#define ImageUtilities_h


// Includes
#include "Image.h"


// Namespaces
namespace ImageUtilities
{

  /// Convert an image from RGBA to RGB
  VX_BASE_DLL void ConvertImage( Image< PixelRGB< uint1 > > & outImage, const Image< PixelRGBA< uint1 > > & inImage );

  /// Rotate an RGB image counter-clockwise 90 degrees
  VX_BASE_DLL void RotateImageCCW90( Image< PixelRGB< uint1 > > & outImage, const Image< PixelRGB< uint1 > > inImage );

  /// Rotate an RGBA image counter-clockwise 90 degrees
  VX_BASE_DLL void RotateImageCCW90( Image< PixelRGBA< uint1 > > & outImage, const Image< PixelRGBA< uint1 > > inImage );

  /// Flip an image vertically
  VX_BASE_DLL void FlipVertically( Image< PixelRGBA< uint1 > > & image );

  /// Write an RGB image to a PNG file
  VX_BASE_DLL void WritePngImage( const Image< PixelRGB< uint1 > > & image, const std::string & sFilename );

  /// Write an RGBA image to a PNG file
  VX_BASE_DLL void WritePngImage( const Image< PixelRGBA< uint1 > > & image, const std::string & sFilename );

}; // namespace ImageUtilities


#endif // ImageUtilities_h


// $Log: ImageUtilities.h,v $
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
// Revision 1.5  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 1.4  2003/05/08 17:50:37  frank
// code review
//
// Revision 1.3  2003/05/08 17:33:33  michael
// code review
//
// Revision 1.2  2003/03/06 17:55:25  frank
// comments
//
// Revision 1.1  2003/03/06 16:14:52  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ImageUtilities.h,v 1.2 2005/10/05 17:42:27 vxconfig Exp $
// $Id: ImageUtilities.h,v 1.2 2005/10/05 17:42:27 vxconfig Exp $
