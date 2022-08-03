// $Id: Image.h,v 1.3 2004/10/25 23:36:16 michael Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.net)

#ifndef Image_h
#define Image_h


// Includes
#include "PixelRGB.h"
#include "PixelRGBA.h"
#include "Point2D.h"
#include "Triple.h"


/**
 * Contains a 2D image of templated pixels.
 */
template <class T> 
class VX_BASE_DLL Image 
{
// Member Functions
public:

  // Construction & Destruction
  // --------------------------

  /// Default constructor.
  Image<T>(const T & clearValue);
  
  /// Constructor.
  Image<T>(const Triple<uint4> & dimensions, const T & clearValue);
  
  /// Copy constructor.
  Image<T>(const Image<T> & other);
  
  /// Destructor.
  ~Image<T>();

  /// Assignment operator.
  inline Image<T> & operator =(const Image<T> & other);
  
  /// Equality operator.
  inline bool operator ==(const Image<T> & other) const;
  
  /// Inequality operator.
  inline bool operator !=(const Image<T> & other) const;
  
  
  // Input & Output
  // --------------
  
  /// File output.
  void Write(const std::string & sFilePrefix) const;
  
  /// File output.
  void WritePPM3(const std::string & sFilePrefix, const uint4 uMaxValue) const;

  // Access Functions
  // ----------------

  /// Get the width and height in the form of a triple.
  inline const Triple<uint4> & GetDim() const { return m_dimensions; }

  /// Retrieve a pixel.
  inline const T & GetPixel(const Point2D<uint2> & position) const;
  
  /// Set a pixel.
  inline void SetPixel(const Point2D<uint2> & position, const T & newValue);

  /// Retrieve a pixel faster.
  inline const T & GetPixelRaw(const Point2D<uint2> & position) const;
  
  /// Set a pixel faster.
  inline void SetPixelRaw(const Point2D<uint2> & position, const T & newValue);
  
  /// Returns the memory address of the image buffer.
  inline const T * GetDataPtrConst() const { return m_pPixels; }
  
  /// Returns the memory address of the image buffer.
  inline T * GetDataPtr() const { return m_pPixels; }

  /// Uses an external memory buffer instead of the internal one.
  inline void UseExternalBuffer(T * pBuffer);

  /// Retrieve the number of bits per pixel in the image.
  inline uint4 GetBitsPerPixel() const;

  /// Set the clear color/value.
  inline void SetClearColor(const T & clearColor) { m_clearValue = clearColor; }

  
  // Image operations
  // ----------------

  /// Clear all the pixels, set to the clear value.
  inline void Clear();

  /// Clears the pixels in a scanline from start to end inclusive
  inline void ClearPixelsInScanLine( const uint2 & uY, const uint2 & uStartX, const uint2 & uEndX );
  
  /// Clears the pixels in a scanline from start to end inclusive
  inline void ClearPixelsInRectangle( const Point2D<uint2> & uMinPoint, const Point2D<uint2> & uMaxPoint );

  /// Resize the image and clear all the pixels.
  inline void Resize(const Triple<uint4> & newDim);

  /// Pad the image dimensions to the next power of two and return a new image.
  inline Image<T> GetPaddedImage() const;

  /// Flip the image vertically (around the horizontal median)
  inline void FlipVertical();

// Data Members
private:

  /// The image data.
  T * m_pPixels;

  /// The image size.
  Triple<uint4> m_dimensions;

  /// The clear value.
  T m_clearValue;
  
  /// Indicates whether ot not pixels should be freed.
  bool m_bOwnData;

  // the number of pixels an image can be resized to without the need to call nwe()
  int4 m_iNumPixelsAllocated;

}; // class Image<T>

#include "Image.inl"

#endif // Image_h


// $Log: Image.h,v $
// Revision 1.3  2004/10/25 23:36:16  michael
// rename m_pixels to m_pPixels
//
// Revision 1.2  2004/03/05 16:37:30  geconomos
// Updated to compile with 7.1 compiler.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.19  2003/06/10 12:06:40  frank
// Code review of pixel classes
//
// Revision 3.18  2003/05/16 16:44:29  frank
// code review
//
// Revision 3.17  2003/05/16 11:44:22  michael
// code review
//
// Revision 3.16  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.15  2003/01/22 21:17:11  ingmar
// 2D pixel Positions are now accesed using Point2D (and not through the 3D Point as before)
//
// Revision 3.14  2001/01/02 13:36:53  ingmar
// removed explicit throw on funtion inteface
// removed dependency on OpenGL by removing two unused functions
//
// Revision 3.13  2002/12/20 19:01:15  ingmar
// removed single scalar constructor from PixelRGBA
//
// Revision 3.12  2002/11/27 16:06:06  ingmar
// removed call to Reset()  and
// made image resizing not reallocate memory if the existing memory chunk is still large enough
//
// Revision 3.11  2002/10/17 17:56:33  ingmar
// removed unused Scale() and Blur() functions that created compiler warnings and bloated the class interface
//
// Revision 3.10  2002/09/28 02:20:06  frank
// Cleaned up Clear for subimages.
//
// Revision 3.9  2002/09/10 20:35:06  michael
// Fixed compilation under 6.0 compiler (Dmitry)
//
// Revision 3.8  2002/09/10 15:16:34  dmitry
// Compilation conditional statements added for the specialized template functions.
//
// Revision 3.7  2002/09/09 21:05:38  dmitry
// Specialized some functions and moved them to the C file.
//
// Revision 3.6  2002/08/08 15:58:59  uday
// Added SetPixelsInScanLine(), SetPixelsInRectangle().
//
// Revision 3.5  2002/04/02 20:02:18  ingmar
// added SetClearColor(color)
//
// Revision 3.4  2002/03/12 22:25:43  frank
// Binli: added non-const data pointer access.
//
// Revision 3.3  2002/03/08 17:15:18  michael
// further removed include dependencies
//
// Revision 3.2  2001/12/24 15:36:55  ingmar
// added class Sec_20_image to structure the doxygen documentation
//
// Revision 3.1  2001/11/05 03:24:45  frank
// Added clear function.
//
// Revision 3.0  2001/10/14 23:02:19  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 03 2001 16:59:30   frank
// Added GetBitsPerPixel function.
// 
//    Rev 2.1   Sep 20 2001 08:54:38   geconomos
// Updated an image class to keep track of memory buffers.
// 
//    Rev 2.0   Sep 16 2001 23:47:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:38   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:44  soeren
// renamed image directory
//
// Revision 1.17  2001/07/23 12:25:25  frank
// Modified Reset() to use m_clearValue.
// Check m_clearValue in equality operator.
//
// Revision 1.16  2001/07/19 21:33:52  ana
// Reset member function added.
// There are Image files over ReportLib, are they duplicates?
//
// Revision 1.15  2001/07/11 17:07:06  frank
// Added function to pad an image to the next power of two.
//
// Revision 1.14  2001/07/02 17:12:34  liwei
// names of namespaces in enums.h changed
//
// Revision 1.13  2001/07/02 16:28:30  frank
// Added inequality test.
//
// Revision 1.12  2001/06/20 15:15:24  frank
// Made dimensions uint4 like volumes.
//
// Revision 1.11  2001/06/18 16:54:22  liwei
// no message
//
// Revision 1.10  2001/06/14 16:37:21  frank
// Converted pixel buffer from std::vector to pointer in order to support using an external buffer (for efficiency purposes).
//
// Revision 1.9  2001/06/09 16:41:02  frank
// Made similar to volume class.
//
// Revision 1.8  2001/06/07 14:49:20  geconomos
// Added GetData() method, access to buffer in memory
//
// Revision 1.7  2001/05/14 14:45:44  frank
// Added PPM3 file format output.
//
// Revision 1.6  2001/05/10 08:27:51  frank
// Added raw set/get pixel.
//
// Revision 1.5  2001/05/09 18:16:25  frank
// Added some rendering functions.
//
// Revision 1.4  2001/05/07 11:02:55  frank
// Added more functions.
//
// Revision 1.3  2001/05/04 11:12:01  frank
// Added additional functionality.
//
// Revision 1.2  2001/05/03 14:06:19  frank
// Added basic functionality.
//
// Revision 1.1  2001/05/03 12:40:08  frank
// Added empty Image class.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Image.h,v 1.3 2004/10/25 23:36:16 michael Exp $
// $Id: Image.h,v 1.3 2004/10/25 23:36:16 michael Exp $
