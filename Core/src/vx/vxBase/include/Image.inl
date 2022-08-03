// $Id: Image.inl,v 1.4 2007/05/29 19:53:39 gdavidson Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// Defines
#define FILE_REVISION "$Revision: 1.4 $"




/**
 * Assignment operator
 *
 * @param other the other image
 */
template <class T> 
inline Image<T> & Image<T>::operator =(const Image<T> & other)
{
  if( &other != this )
  {
    m_dimensions = other.m_dimensions;
    m_clearValue = other.m_clearValue;
    
    // resize and copy the image
    if(m_bOwnData && m_pPixels != NULL)
      delete [] m_pPixels;
    
    const uint4 uNumPixels(m_dimensions.m_x * m_dimensions.m_y);
    m_pPixels = new T[uNumPixels];
    m_bOwnData = true;

    T * pSrc = other.m_pPixels;
    T * pDest = m_pPixels;
    T * pDestEnd = m_pPixels + uNumPixels;
    for (; pDest != pDestEnd; ++pDest, ++pSrc)
      *pDest = *pSrc;
  }
  return *this;
} // Image()


/**
 * Equality operator
 *
 * @param other the other image
 * @return      true if the same
 */
template <class T> 
inline bool Image<T>::operator ==(const Image<T> & other) const
{
  if (m_dimensions != other.m_dimensions)
    return false;

  if (m_clearValue != other.m_clearValue)
    return false;

  // resize and copy the image
  const uint4 uImageSize = m_dimensions.m_x * m_dimensions.m_y;
  for (uint4 i(0); i<uImageSize; ++i)
  {
    if (m_pPixels[i] != other.m_pPixels[i])
      return false;
  }

  return true;

} // operator ==()


/**
 * Inequality operator
 *
 * @param other the other image
 * @return      true if different
 */
template <class T> 
inline bool Image<T>::operator !=(const Image<T> & other) const
{

  return ! ( *this == other );

} // operator !=()

/**
 * Retrieve a pixel
 *
 * @param position the pixel position
 * @return         reference to the pixel
 */
template <class T>
inline const T & Image<T>::GetPixel(const Point2D<uint2> & position) const
{

  if ( position.m_x >= m_dimensions.m_x || position.m_y >= m_dimensions.m_y )
    throw ex::IndexOutOfBoundsException( LogRec( "Pixel access out of bounds.", "Image<T>", "GetPixel"  ));

  return m_pPixels[position.m_y * m_dimensions.m_x + position.m_x];

} // GetPixel()


/**
 * Set a pixel
 *
 * @param position the pixel position
 * @param newValue the new pixel value
 */
template <class T>
inline void Image<T>::SetPixel(const Point2D<uint2> & position, const T & newValue)
{

  if (position.X() >= m_dimensions.X() || position.Y() >= m_dimensions.Y())
    throw ex::IndexOutOfBoundsException( LogRec( "Pixel access out of bounds", "Image", "SetPixel" ) );

  m_pPixels[position.Y() * m_dimensions.X() + position.X()] = newValue;

} // SetPixel()


/**
 * Retrieve a pixel faster (no exception handling)
 *
 * @param position the pixel position
 * @return         the pixel value
 */
template <class T>
inline const T & Image<T>::GetPixelRaw(const Point2D<uint2> & position) const
{

  return m_pPixels[ position.m_y * m_dimensions.m_x + position.m_x ];

} // GetPixel()


/**
 * Set a pixel faster (no exception handling)
 *
 * @param position the pixel position
 * @param newValue the new pixel value
 */
template <class T>
inline void Image<T>::SetPixelRaw(const Point2D<uint2> & position, const T & newValue)
{

  m_pPixels[ position.m_y * m_dimensions.m_x + position.m_x ] = newValue;

} // SetPixel()


/**
 * Retrieve the number of bits per pixel in the image
 *
 * @return the bits per pixel
 */
template <class T>
inline uint4 Image<T>::GetBitsPerPixel() const
{
  return 8*sizeof(T);
} // GetBitsPerPixel()


/**
 * Clear all the pixels, set to the clear value, specifically tuned for the PixelRGBA class!!!
 */
template <>
void Image<PixelRGBA<unsigned char> >::Clear()
//*******************************************************************
{
  unsigned char * pDest = reinterpret_cast<unsigned char *>(m_pPixels);
  unsigned char * pDestEnd = reinterpret_cast<unsigned char *>(m_pPixels + m_dimensions.m_x * m_dimensions.m_y);
  unsigned char ucRed(m_clearValue.R());
  unsigned char ucGreen(m_clearValue.G());
  unsigned char ucBlue(m_clearValue.B());
  unsigned char ucAlpha(m_clearValue.A());

  while (pDest != pDestEnd)
  {
    *pDest++ = ucRed;
    *pDest++ = ucGreen;
    *pDest++ = ucBlue;
    *pDest++ = ucAlpha;
  }
} // Clear()


/**
 * Clear all the pixels, set to the clear value
 */
template <class T>
void Image<T>::Clear()
//*******************************************************************
{
  T * pDest = m_pPixels;
  T * pDestEnd = m_pPixels + m_dimensions.m_x * m_dimensions.m_y;

  while (pDest != pDestEnd)
    *pDest++ = m_clearValue;
} // Clear()


/**
 * Resize the image and clear all the pixels
 *
 * @param newDim the new dimensions
 */
template <class T>
void Image<T>::Resize(const Triple<uint4> & newDim)
{
  // resize image
  const int4 iNumImagePixels(newDim.m_x * newDim.m_y);
  m_dimensions = newDim;
  
  if (iNumImagePixels > m_iNumPixelsAllocated)
  {

    if (m_bOwnData && m_pPixels != NULL)
      delete [] m_pPixels;

    m_iNumPixelsAllocated = iNumImagePixels;
    m_pPixels = new T[m_iNumPixelsAllocated];
    m_bOwnData = true;

  }
  
  Clear();

} // Resize()


/**
 * Pad the image dimensions to the next power of two and return a new image
 *
 * @return a padded image
 */
template <class T> Image<T> Image<T>::GetPaddedImage() const
{

  // find next power of two
  Triple<uint4> newDimTriple(1,1,1);
  while (newDimTriple.m_x < m_dimensions.m_x) { newDimTriple.m_x *= 2; }
  while (newDimTriple.m_y < m_dimensions.m_y) { newDimTriple.m_y *= 2; }

  // copy the pixels over
  Image<T> newImage(newDimTriple, T());
  Point2D<uint2> posPoint;
  for (posPoint.m_y=0; posPoint.m_y<m_dimensions.m_y; ++posPoint.m_y)
  {
    for (posPoint.m_x=0; posPoint.m_x<m_dimensions.m_x; ++posPoint.m_x)
    {
      newImage.SetPixelRaw(posPoint, GetPixelRaw(posPoint));
    }
  }

  return newImage;

} // GetPaddedImage()


/**
 * flips the image about the horizontal
 */
template <class T> void Image<T>::FlipVertical()
{
  // flip the image
  for ( uint4 y(0); y<GetDim().m_y / 2; ++y )
  {
    for ( uint4 x(0); x<GetDim().m_x; ++x )
    {
      const Point2D< uint2 > pos1( x, y );
      const Point2D< uint2 > pos2( x, GetDim().m_y - 1 - y );
      const T tempPixel( GetPixel( pos1 ) );
      SetPixel( pos1, GetPixel( pos2 ) );
      SetPixel( pos2, tempPixel );
    } // for x
  } // for y
} // FlipVertical()


/**
 * Uses an external memory buffer instead of the internal one.
 * Will delete memory buffer at end of Image object scope.
 * Example:
 *   Image<uint1> image(Triple<uint2>(dimX,dimy,0), 0);
 *
 * @param  uint1 * pBuffer = new uint1[dimX*dimY];
 *   image.UseExternalBuffer(pBuffer);
 *   image.SetPixel(...
 */
template <class T>
void Image<T>::UseExternalBuffer(T * pBuffer)
{

  T * pSrc = m_pPixels;
  T * pDest = pBuffer;
  T * pDestEnd = pBuffer + m_dimensions.m_x * m_dimensions.m_y;
  for (; pDest != pDestEnd; ++pDest, ++pSrc)
    *pDest = *pSrc;
  if(m_bOwnData && m_pPixels != NULL)
    delete [] m_pPixels;
  
  m_pPixels = pBuffer;
  m_bOwnData = false;

} // GetDataPtr()


/** 
 * Convert a Pixel RGBA image to a Pixel RGB image
 *
 * @param inImage RGBA image
 * @return RGB image
 */
inline Image< PixelRGB<uint1> > ConvertToRGB(const Image<PixelRGBA<uint1> > & inImage)
{
  Image<PixelRGB<uint1> > outImage(inImage.GetDim(), PixelRGB<uint1>(0,0,0));
  Point2D<uint2> pos;
  for (pos.m_y=0; pos.m_y<inImage.GetDim().m_y; ++pos.m_y)
  {
    for (pos.m_x=0; pos.m_x<inImage.GetDim().m_x; ++pos.m_x)
    {
      const PixelRGBA<uint1> & inPixel = inImage.GetPixel(pos);
      outImage.SetPixelRaw(pos, PixelRGB<uint1>(inPixel.R(), inPixel.G(), inPixel.B()));
    }
  }
  return outImage;
} // ConvertToRGB()


/**
 * Clears the pixels in a scanline from start to end inclusive
 *
 * @param uY      the scanline
 * @param uStartX the min x position
 * @param uEndX   the max x position
 * @return 
 */
template<class T>
inline void Image<T>::ClearPixelsInScanLine( const uint2 & uY, const uint2 & uXstart, const uint2 & uXend)
{
  // calculate linear indexes
  T * uIndexStart(m_pPixels + uY * m_dimensions.m_x + uXstart);
  T * uIndexEnd(m_pPixels + uY * m_dimensions.m_x + uXend + 1);

  for( T * x = uIndexStart ; x != uIndexEnd ; ++x )
    *x = m_clearValue;

} // ClearPixelsInScanLine


/**
 * Clears the pixels in a scanline from start to end inclusive
 *
 * @param uMinPoint the min point
 * @param uMaxPoint the max point
 */
template<class T>
inline void Image<T>::ClearPixelsInRectangle( const Point2D<uint2> & uMinPoint, const Point2D<uint2> & uMaxPoint )
{

  // determine which scanlines to process
  uint2 yMin( Bound( uint2(0), uMinPoint.m_y, uint2( m_dimensions.m_y - 1 ) ) );
  uint2 yMax( Bound( uint2(0), uMaxPoint.m_y, uint2( m_dimensions.m_y - 1 ) ) );

  for( uint2 y(yMin) ; y<=yMax ; ++y )
    ClearPixelsInScanLine( y, uMinPoint.m_x, uMaxPoint.m_x );

} // ClearPixelsInRectangle


#if _MSC_VER < 1300


/**
 * File output
 * @param sFilePrefix the file prefix
 */
template < class T >
void Image< T >::Write(const std::string & sFilePrefix) const
{
  std::ofstream outStream(sFilePrefix.c_str(), std::ios::binary);
  outStream << "P6" << std::endl;
  outStream << "# no comment" << std::endl;
  outStream << m_dimensions.m_x << " " << m_dimensions.m_y << std::endl;
  outStream << "255" << std::endl;
  T * pPixelEnd = m_pPixels + m_dimensions.m_x * m_dimensions.m_y;
  for (T * pPixel=m_pPixels; pPixel != pPixelEnd; ++pPixel)
  {
    uint1 vRawValue[3];
    vRawValue[0] = pPixel->R();
    vRawValue[1] = pPixel->G();
    vRawValue[2] = pPixel->B();
    outStream.write((char *) vRawValue, 3*sizeof(uint1));
  }
} // Write()


/**
 * File output.
 * @param sFilePrefix the file prefix
 * @param uMaxValue   the max value to write into the PPM file
 */
template < class T >
void Image< T >::WritePPM3(const std::string & sFilePrefix, const uint4 uMaxValue) const
{
  std::ofstream outStream(sFilePrefix.c_str());
  outStream << "P3" << std::endl;
  outStream << "# no comment" << std::endl;
  outStream << m_dimensions.m_x << " " << m_dimensions.m_y << std::endl;
  outStream << uMaxValue << std::endl;
  T * pPixelEnd = m_pPixels + m_dimensions.m_x * m_dimensions.m_y;
  for (T * pPixel=m_pPixels; pPixel != pPixelEnd; ++pPixel)
  {
    for (uint1 j(0); j<3; ++j)
      outStream << uint4(pPixel->R()) << " ";
    outStream << std::endl;
  }
} // WritePPM3()


#endif


#undef FILE_REVISION




// $Log: Image.inl,v $
// Revision 1.4  2007/05/29 19:53:39  gdavidson
// Added a check against itself in the =operator
//
// Revision 1.3  2004/10/25 23:40:17  michael
// rename m_pixels to m_pPixels and implemented specialized Clear() funciton
// for PixelRGBA<uint1>
//
// Revision 1.2  2004/03/05 16:37:30  geconomos
// Updated to compile with 7.1 compiler.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.25  2003/06/10 12:06:40  frank
// Code review of pixel classes
//
// Revision 3.24  2003/05/30 17:56:09  michael
// fixed issue #3268
//
// Revision 3.23  2003/05/30 15:22:28  michael
// fixed issue #3254
//
// Revision 3.22  2003/05/16 16:44:29  frank
// code review
//
// Revision 3.21  2003/05/16 11:44:22  michael
// code review
//
// Revision 3.20  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.19  2003/01/22 21:19:25  ingmar
// 2D pixel Positions are now accesed using Point2D (and not through the 3D Point as before)
//
// Revision 3.18  2001/01/02 13:41:27  ingmar
// removed explicit throw on funtion inteface
// removed dependency on OpenGL by removing two unused functions
//
// Revision 3.17  2001/01/02 13:36:53  ingmar
// removed explicit throw on funtion inteface
// removed dependency on OpenGL by removing two unused functions
//
// Revision 3.16  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 3.15  2002/12/19 20:43:20  ingmar
// PixelRGBA member data is now private -> need to use access functions
//
// Revision 3.14  2002/11/27 16:06:06  ingmar
// removed call to Reset()  and
// made image resizing not reallocate memory if the existing memory chunk is still large enough
//
// Revision 3.13  2002/10/17 17:56:33  ingmar
// removed unused Scale() and Blur() functions that created compiler warnings and bloated the class interface
//
// Revision 3.12  2002/09/28 02:20:06  frank
// Cleaned up Clear for subimages.
//
// Revision 3.11  2002/09/19 18:34:05  geconomos
// I should have put the specialization for Blur() in the .C file.
//
// Revision 3.10  2002/09/19 18:22:10  geconomos
// Specialized Blur() function as NOOP for type bool.
//
// Revision 3.9  2002/09/10 20:35:06  michael
// Fixed compilation under 6.0 compiler (Dmitry)
//
// Revision 3.8  2002/09/09 21:05:37  dmitry
// Specialized some functions and moved them to the C file.
//
// Revision 3.7  2002/08/08 15:58:59  uday
// Added SetPixelsInScanLine(), SetPixelsInRectangle().
//
// Revision 3.6  2002/06/19 21:15:38  ingmar
// mow assuming that the image is made out of pixels
//
// Revision 3.5  2002/04/02 20:02:18  ingmar
// added SetClearColor(color)
//
// Revision 3.4  2002/03/12 22:25:45  frank
// Binli: added non-const data pointer access.
//
// Revision 3.3  2001/11/05 03:24:45  frank
// Added clear function.
//
// Revision 3.2  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.1  2001/10/23 22:15:45  frank
// Added image conversion function.
//
// Revision 3.0  2001/10/14 23:02:19  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 03 2001 16:59:32   frank
// Added GetBitsPerPixel function.
// 
//    Rev 2.1   Sep 20 2001 08:54:46   geconomos
// Updated an image class to keep track of memory buffers.
// 
//    Rev 2.0   Sep 16 2001 23:47:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:38   ingmar
// Initial revision.
// Revision 1.5  2001/09/13 17:42:48  soeren
// added quotes to function name in logger macro
//
// Revision 1.4  2001/09/10 09:05:00  ingmar
// fixed use of logger defines
//
// Revision 1.3  2001/09/07 21:21:16  soeren
// added #undef FILE_VERSION in the end of each file
//
// Revision 1.2  2001/09/07 17:59:08  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:11:45  soeren
// renamed image directory
//
// Revision 1.19  2001/08/21 22:19:22  sarang
// fixed syntax errors : now OK with linux/gcc
// Remember to keep declaration and definition of functionin sync. Exceptions should be written both in the .h and the .C files.
//
// Revision 1.18  2001/08/13 20:05:28  tushar
// removed errors due to the new logger
//
// Revision 1.17  2001/08/07 17:02:44  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.16  2001/07/25 00:29:04  tushar
// fixed some compiler errors (logger)
//
// Revision 1.15  2001/07/23 12:25:25  frank
// Modified Reset() to use m_clearValue.
// Check m_clearValue in equality operator.
//
// Revision 1.14  2001/07/19 21:33:52  ana
// Reset member function added.
// There are Image files over ReportLib, are they duplicates?
//
// Revision 1.13  2001/07/11 17:07:06  frank
// Added function to pad an image to the next power of two.
//
// Revision 1.12  2001/07/06 19:45:04  frank
// Fixed bugs in seldom-used functions.
//
// Revision 1.11  2001/07/02 16:28:39  frank
// Added inequality test.
//
// Revision 1.10  2001/06/20 15:15:28  frank
// Made dimensions uint4 like volumes.
//
// Revision 1.9  2001/06/18 20:39:04  huamin
// Fixed the bugs about the size of m_pPixels
//
// Revision 1.8  2001/06/14 16:37:21  frank
// Converted pixel buffer from std::vector to pointer in order to support using an external buffer (for efficiency purposes).
//
// Revision 1.7  2001/06/09 16:41:02  frank
// Made similar to volume class.
//
// Revision 1.6  2001/06/07 14:49:20  geconomos
// Added GetData() method, access to buffer in memory
//
// Revision 1.5  2001/06/06 21:12:00  huamin
// Add return value for "=" oeprator
//
// Revision 1.4  2001/05/14 14:45:44  frank
// Added PPM3 file format output.
//
// Revision 1.3  2001/05/10 08:27:51  frank
// Added raw set/get pixel.
//
// Revision 1.2  2001/05/09 18:16:25  frank
// Added some rendering functions.
//
// Revision 1.1  2001/05/07 11:03:21  frank
// Created file.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Image.inl,v 1.4 2007/05/29 19:53:39 gdavidson Exp $
// $Id: Image.inl,v 1.4 2007/05/29 19:53:39 gdavidson Exp $
