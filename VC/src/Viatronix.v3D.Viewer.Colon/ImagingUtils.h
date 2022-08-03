// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ImagingUtils.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include <vector>
#include <memory>
#include "atlcomtime.h"


OPEN_UTILITIES_NS


namespace Imaging
{

  /// collection of bitmaps
  typedef std::shared_ptr< Gdiplus::Bitmap > Image;
  typedef std::vector< Image > ImageList;

  /// writes out the image to disk
  void WriteImage( Gdiplus::Bitmap * pBitmap, const std::string & sFile, bool bInitGdiplus = true );

  /// writes out the collection of images to disk
  void WriteImages( ImageList & bitmaps, const std::string & sDirectory, const std::string & sPrefix = "image" );

}; // namespace Imaging


CLOSE_UTILITIES_NS