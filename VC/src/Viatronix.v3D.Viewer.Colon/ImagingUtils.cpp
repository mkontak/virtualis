// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ImagingUtils.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "ImagingUtils.h"
#include "ReaderGlobal.h"


USING_UTILITIES_NS

/**
 * Writes the image to disk.
 *
 * @param     pBitmap           pointer to a Bitmap
 * @param     sFile             file name
 * @param     bInitGdiplus      true (default) initializes gdi+; otherwise false
 */
void Imaging::WriteImage( Gdiplus::Bitmap * pBitmap, const std::string & sFile, bool bInitGdiplus )
{
  if( pBitmap == NULL )
    return;

  ULONG_PTR pToken;
  if( bInitGdiplus )
  {
    Gdiplus::GdiplusStartupInput gsi;
    Gdiplus::GdiplusStartup( &pToken, &gsi, NULL );
  }

  SCOPE_EXIT
  {
    if( bInitGdiplus )
      Gdiplus::GdiplusShutdown( pToken );
  };

  CLSID imgCLSID;
  if( ReaderLib::ReaderGlobal::GetCodecClsid( L"image/png", imgCLSID ))
  {
    std::wstring sPath( ToWString( sFile ));
    pBitmap->Save( const_cast< wchar_t* >( sPath.c_str() ), &imgCLSID, NULL );
  }
} // WriteImage( pBitmap, sFile, bInitGdiplus )


/**
 * Writes the images to a directory
 *
 * @param     bitmaps           bitmap collection
 * @param     sDirectory        directory name
 * @param     sPrefix           file name prefix
 */
void Imaging::WriteImages( ImageList & bitmaps, const std::string & sDirectory, const std::string & sPrefix )
{
  ULONG_PTR pToken;
  Gdiplus::GdiplusStartupInput gsi;
  Gdiplus::GdiplusStartup( &pToken, &gsi, NULL );

  SCOPE_EXIT
  {
    Gdiplus::GdiplusShutdown( pToken );
  };

  // writes the report image to disk
  int4 iImageNumber( 0 );
  for( ImageList::iterator iter = bitmaps.begin(); iter != bitmaps.end(); ++iter )
  {     
    char sImagePath[ _MAX_PATH ];
    
    // Increment the image number.
    ++iImageNumber;

    // Pad out the image file to three digits.  1 becomes 001, 10 becomes 010 and 100 and up is unchanged.
    std::string sPadding = "";
    if(iImageNumber < 10)
    {
      sPadding = "00";
    }
    else if(iImageNumber > 9 && iImageNumber < 100)
    {
      sPadding = "0";
    }
    
    sprintf( sImagePath, "%s\\%s%s%d.png", sDirectory.c_str(), sPrefix.c_str(), sPadding.c_str(), iImageNumber );
    WriteImage( (*iter).get(), sImagePath, false );
  }
} // WriteImages( bitmaps, sDirectory, sPrefix )

