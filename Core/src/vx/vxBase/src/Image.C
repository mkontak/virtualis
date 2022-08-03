// $Id: Image.C,v 1.6 2007/05/31 16:05:53 gdavidson Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Image.C,v 1.6 2007/05/31 16:05:53 gdavidson Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.net)


// Includes
#include "stdafx.h"
#include "Image.h"

template class VX_BASE_DLL Image< bool >;
template class VX_BASE_DLL Image< float4 >;
template class VX_BASE_DLL Image< uint2 >;
template class VX_BASE_DLL Image< PixelRGB< uint1 > >;
template class VX_BASE_DLL Image< PixelRGBA< uint1 > >;
template class VX_BASE_DLL Image< PixelRGB< uint2 > >;
template class VX_BASE_DLL Image< PixelRGBA< uint2 > >;


/**
 * Default constructor
 *
 * @param clearValue the value with which to clear the image
 */
template <class T> 
Image<T>::Image(const T & clearValue)
{
  m_iNumPixelsAllocated = 0;
  m_dimensions = Triple<uint4>(0,0,0);
  m_pPixels = new T[1];
  m_clearValue = clearValue;
  m_bOwnData = true;
} // Image()


/**
 * Constructor
 *
 * @param diemsions  the dimensions of the image (z is ignored)
 * @param clearValue the value with which to clear the image
 */
template <class T> 
Image<T>::Image(const Triple<uint4> & dimensions, const T & clearValue)
{
  m_iNumPixelsAllocated = 0;
  m_bOwnData = true;
  m_dimensions = dimensions;
  m_clearValue = clearValue;

  // resize and clear the image
  const uint4 uNumPixels(m_dimensions.m_x * m_dimensions.m_y);
  m_pPixels = new T[uNumPixels];

  T * pDest = m_pPixels;
  T * pDestEnd = m_pPixels + uNumPixels;
  while (pDest != pDestEnd)
    *pDest++ = clearValue;
       
} // Image()


/**
 * Copy constructor
 *
 * @param other the other image
 */
template <class T> 
Image<T>::Image(const Image<T> & other)
{
  m_iNumPixelsAllocated = 0;
  m_bOwnData = true;
  m_dimensions = other.m_dimensions;
  m_clearValue = other.m_clearValue;

  // resize and copy the image
  const uint4 uNumPixels(m_dimensions.m_x * m_dimensions.m_y);
  m_pPixels = new T[uNumPixels];
  T * pSrc = other.m_pPixels;
  T * pDest = m_pPixels;
  T * pDestEnd = m_pPixels + uNumPixels;
  for (; pDest != pDestEnd; ++pDest, ++pSrc)
    *pDest = *pSrc;

} // Image()

/**
 * Destructor
 */
template <class T> 
inline Image<T>::~Image()
{
  if ( m_bOwnData && m_pPixels != NULL )
  {
    delete [] m_pPixels;
    m_pPixels = NULL;
  }
} // ~Image()



/**
 * File output
 *
 * @param sFilePrefix the file prefix
 */
template <>
void Image< class PixelRGBA< uint1 > >::Write( const std::string & sFilePrefix ) const
{
  std::ofstream outStream(sFilePrefix.c_str(), std::ios::binary);
  outStream << "P6" << std::endl;
  outStream << "# no comment" << std::endl;
  outStream << m_dimensions.m_x << " " << m_dimensions.m_y << std::endl;
  outStream << "255" << std::endl;
  PixelRGBA< uint1 > * pPixelEnd = m_pPixels + m_dimensions.m_x * m_dimensions.m_y;
  for (PixelRGBA< uint1 > * pPixel=m_pPixels; pPixel != pPixelEnd; ++pPixel)
  {
    uint1 vRawValue[3];
    vRawValue[0] = pPixel->R();
    vRawValue[1] = pPixel->G();
    vRawValue[2] = pPixel->B();
    outStream.write((char *) vRawValue, 3*sizeof(uint1));
  }
} // Write()


/**
 * File output
 *
 * @param sFilePrefix the file prefix
 */
template <>
void Image< class PixelRGB< uint1 > >::Write( const std::string & sFilePrefix ) const
{
  std::ofstream outStream(sFilePrefix.c_str(), std::ios::binary);
  outStream << "P6" << std::endl;
  outStream << "# no comment" << std::endl;
  outStream << m_dimensions.m_x << " " << m_dimensions.m_y << std::endl;
  outStream << "255" << std::endl;
  PixelRGB< uint1 > * pPixelEnd = m_pPixels + m_dimensions.m_x * m_dimensions.m_y;
  for (PixelRGB< uint1 > * pPixel=m_pPixels; pPixel != pPixelEnd; ++pPixel)
  {
    uint1 vRawValue[3];
    vRawValue[0] = pPixel->R();
    vRawValue[1] = pPixel->G();
    vRawValue[2] = pPixel->B();
    outStream.write((char *) vRawValue, 3*sizeof(uint1));
  }
} // Write()


/**
 * File output
 *
 * @param sFilePrefix the file prefix
 */
template <>
void Image< float4 >::Write(const std::string & sFilePrefix) const
{
  std::ofstream outStream(sFilePrefix.c_str(), std::ios::binary);
  outStream << "P6" << std::endl;
  outStream << "# no comment" << std::endl;
  outStream << m_dimensions.m_x << " " << m_dimensions.m_y << std::endl;
  outStream << "255" << std::endl;
  float4 * pPixelEnd = m_pPixels + m_dimensions.m_x * m_dimensions.m_y;
  for (float4 * pPixel=m_pPixels; pPixel != pPixelEnd; ++pPixel)
  {
    uint1 vRawValue[3];
    vRawValue[0] = static_cast< uint1 >( 255 * (1-exp(-(*pPixel)/10000.0F)));  
    vRawValue[1] = static_cast< uint1 >( 255 * (1-exp(-(*pPixel)/100.0F))); 
    vRawValue[2] = static_cast< uint1 >( 255 * (1-exp(-(*pPixel)))); 
    outStream.write((char *) vRawValue, 3*sizeof(uint1));
  }
} // Write()


/**
 * File output
 *
 * @param sFilePrefix the file prefix
 * @param uMaxValue   the maximum value to be written into the PPM image
 */
template <>
void Image< PixelRGBA< uint1 > >::WritePPM3(const std::string & sFilePrefix, const uint4 uMaxValue) const
{
  std::ofstream outStream(sFilePrefix.c_str());
  outStream << "P3" << std::endl;
  outStream << "# no comment" << std::endl;
  outStream << m_dimensions.m_x << " " << m_dimensions.m_y << std::endl;
  outStream << uMaxValue << std::endl;
  PixelRGBA< uint1 > * pPixelEnd = m_pPixels + m_dimensions.m_x * m_dimensions.m_y;
  for (PixelRGBA< uint1 > * pPixel=m_pPixels; pPixel != pPixelEnd; ++pPixel)
  {
    for (uint1 j(0); j<3; ++j)
      outStream << uint4(pPixel->R()) << " ";
    outStream << std::endl;
  }
} // WritePPM3()


// $Log: Image.C,v $
// Revision 1.6  2007/05/31 16:05:53  gdavidson
// Specialized for unit2
//
// Revision 1.5  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.4  2004/10/25 23:36:16  michael
// rename m_pixels to m_pPixels
//
// Revision 1.3  2004/08/31 19:51:41  frank
// pointer safety
//
// Revision 1.2  2004/03/05 16:37:30  geconomos
// Updated to compile with 7.1 compiler.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.14  2003/06/10 12:06:40  frank
// Code review of pixel classes
//
// Revision 3.13  2003/05/16 16:44:29  frank
// code review
//
// Revision 3.12  2003/05/16 11:44:22  michael
// code review
//
// Revision 3.11  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.10  2003/01/22 21:16:11  ingmar
// added missing include
//
// Revision 3.9  2001/01/02 13:36:53  ingmar
// removed explicit throw on funtion inteface
// removed dependency on OpenGL by removing two unused functions
//
// Revision 3.8  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 3.7  2002/11/19 19:05:16  ingmar
// added Write() for Image<float4>
//
// Revision 3.6  2002/10/17 17:56:33  ingmar
// removed unused Scale() and Blur() functions that created compiler warnings and bloated the class interface
//
// Revision 3.5  2002/09/19 19:17:12  geconomos
// Forget the specialization of Blur().
//
// Revision 3.4  2002/09/19 18:34:05  geconomos
// I should have put the specialization for Blur() in the .C file.
//
// Revision 3.3  2002/09/10 20:35:06  michael
// Fixed compilation under 6.0 compiler (Dmitry)
//
// Revision 3.2  2002/09/10 15:16:34  dmitry
// Compilation conditional statements added for the specialized template functions.
//
// Revision 3.1  2002/09/09 21:05:38  dmitry
// Specialized some functions and moved them to the C file.
//
// Revision 3.0  2001/10/14 23:02:19  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:36   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:44  soeren
// renamed image directory
//
// Revision 1.5  2001/06/20 22:43:23  liwei
// member function SetData() removed
//
// Revision 1.4  2001/06/18 16:54:22  liwei
// no message
//
// Revision 1.3  2001/05/07 11:02:55  frank
// Added more functions.
//
// Revision 1.2  2001/05/03 14:06:19  frank
// Added basic functionality.
//
// Revision 1.1  2001/05/03 12:40:09  frank
// Added empty Image class.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Image.C,v 1.6 2007/05/31 16:05:53 gdavidson Exp $
// $Id: Image.C,v 1.6 2007/05/31 16:05:53 gdavidson Exp $
