// $Id: Convert.h,v 1.2 2005/11/08 17:46:49 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Convert.h,v 1.2 2005/11/08 17:46:49 mkontak Exp $
//
// Copyright© (current year), Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: [Mark Kontak] [mkontak@viatronix.com]
//
// Complete history on bottom of file


// pragmas
#pragma once


OPEN_UTILITIES_NS

using namespace System::Drawing;
using namespace System::Collections;


/** 
 * Class used for converting enumerators to strings
 */
public ref class Convert
{

public:
  Convert() { }

  /// Converts the bytes to an int
  static System::Int32 ToInt32(array<byte> ^ buffer, int startIndex, int length);

  /// Converts the bytes to an int (assumes the length is 8)
  __inline static System::Int32 ToInt32(array<byte> ^ buffer, int startIndex)
  { return ToInt32(buffer, startIndex, 8); }


  /// Converts the bytes to a long or Int64
  static System::Int64 ToInt64(array<byte> ^ buffer, int startIndex, int length);

  /// Converts the bytes to a long or Int64 (Assumes length to be 16)
  static System::Int64 ToInt64(array<byte> ^ buffer, int startIndex)
  { return ToInt64(buffer, startIndex, 16); }


  /// Converts the Int64 to a bytes array
  static array<byte> ^ ToByteArray(System::Int64 intValue) ;

  /// Converts the Int32w to a bytes array
  static array<byte> ^ ToByteArray(System::Int32 intValue);

  /// Converts the movie file to a list of images each frame being a single image
  static ArrayList ^ ToImageList(System::String ^ pMovieFile);

};  // Descriptions


CLOSE_UTILITIES_NS


// $Log: Convert.h,v $
// Revision 1.2  2005/11/08 17:46:49  mkontak
// Added features
//
// Revision 1.1  2005/04/28 13:24:45  mkontak
// Added byte converters
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Convert.h,v 1.2 2005/11/08 17:46:49 mkontak Exp $
// $Id: Convert.h,v 1.2 2005/11/08 17:46:49 mkontak Exp $
