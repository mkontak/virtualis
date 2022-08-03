// $Id: Movies.h,v 1.2 2007/03/09 21:07:03 mkontak Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Make Kontak] (mailto:mkontak@viatronix.com)

///pragmas
#pragma once

// namespaces
using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System::Collections;

OPEN_UTILITIES_NS

// Conversion delagate used in .NET type call backs
public delegate void MoviesExtractionCallback(int iTotalFrames, int iFramesExtracted);


/**
 * Implements a movies object to extract the frames
 */
public ref class Movies
{

private:

  /// Construxtor
  Movies() { }

public:

  /// Returns all the movie files in the path
  static array<System::String ^> ^ GetFiles(System::String ^ sPath);

  /// Returns true if the file is supported
  static bool Supports(System::String ^ sMovieFile);

  /// Gets all images from the movie file
  static ArrayList ^ GetImages(System::String ^ sMovieFile, MoviesExtractionCallback ^ pCallback);

  /// Gets all images from the movie file
  static inline ArrayList ^ GetImages(System::String ^ sMovieFile)
  { return GetImages(sMovieFile, __nullptr); }

  /// Gets a preview image from the movie file
  //static Bitmap ^ GetPreviewImage(System::String ^ sMovieFile);

private:

  /// Gets all images from a WMV file
  static ArrayList ^ GetWmvImages(System::String ^ sMovieFile, MoviesExtractionCallback ^ pCallback);

  /// Gets all limages from an AVI file
  static ArrayList ^ GetAviImages(System::String ^ sMovieFile, MoviesExtractionCallback ^ pCallback);

  /// Gets a preview image from a WMV file
  //static ArrayList ^ GetWmvPreviewImage(System::String ^ sMovieFile);

  /// Gets a preview images from an AVI file
  //static ArrayList ^ GetAviPreviewImage(System::String ^ sMovieFile);


};  // class Movies


CLOSE_UTILITIES_NS


// $Log: Movies.h,v $
// Revision 1.2  2007/03/09 21:07:03  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Movies.h,v 1.2 2007/03/09 21:07:03 mkontak Exp $
// $Id: Movies.h,v 1.2 2007/03/09 21:07:03 mkontak Exp $
