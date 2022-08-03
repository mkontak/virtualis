// $Id: ScanlineFilter.C,v 1.2 2005/05/23 12:47:48 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "ScanlineFilter.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor.
 * The scanline buffers should contain the filter byte at the beginning followed by the pixel bytes.
 *
 * @param scanlineBuffer     the current scanline buffer
 * @param prevScanlineBuffer the previous scanline buffer
 * @param uBytePerPixel      the number of bytes per pixel
 */
ScanlineFilter::ScanlineFilter( MemoryBuffer & scanlineBuffer, MemoryBuffer & prevScanlineBuffer, const uint4 uBytesPerPixel )
: m_uBytesPerPixel( uBytesPerPixel )
{
  // get a hold of the filter byte
  m_puFilterByte = scanlineBuffer.GetDataPtr();

  // set up the two scanlines to start at the first pixels, not at the filter bytes
  m_pScanlineBuffer = new MemoryBuffer( scanlineBuffer.GetDataPtr()+1, scanlineBuffer.GetSize()-1 );
  m_pPrevScanlineBuffer = new MemoryBuffer( prevScanlineBuffer.GetDataPtr()+1, prevScanlineBuffer.GetSize()-1 );

} // ScanlineFilter()


/**
 * Destructor
 */
ScanlineFilter::~ScanlineFilter()
{
  delete m_pScanlineBuffer;
  delete m_pPrevScanlineBuffer;
} // ~ScanlineFilter()


/**
 * Encode (filter) the data using the "best" filter.
 */
void ScanlineFilter::EncodeScanline()
{
  return; // not yet working

  MemoryBuffer subBuffer     = * m_pScanlineBuffer;
  MemoryBuffer upBuffer      = * m_pScanlineBuffer;
  MemoryBuffer averageBuffer = * m_pScanlineBuffer;
  MemoryBuffer paethBuffer   = * m_pScanlineBuffer;

  uint4 iNoneNorm   ( EncodeNone   (           ) );
  uint4 iSubNorm    ( EncodeSub    ( subBuffer ) );
  uint4 iUpNorm     ( EncodeUp     ( subBuffer ) );
  uint4 iAverageNorm( EncodeAverage( subBuffer ) );
  uint4 iPaethNorm  ( EncodePaeth  ( subBuffer ) );

  // chose the best and simplest method in preference of simplicity
  if (iNoneNorm <= iSubNorm &&
      iNoneNorm <= iUpNorm &&
      iNoneNorm <= iAverageNorm &&
      iNoneNorm <= iPaethNorm)
  {
    *m_puFilterByte = NONE;
  }
  else if (iSubNorm <= iUpNorm &&
           iSubNorm <= iAverageNorm &&
           iSubNorm <= iPaethNorm)
  {
    *m_puFilterByte = SUB;
    *m_pScanlineBuffer = subBuffer;
  }
  else if (iUpNorm <= iAverageNorm &&
           iUpNorm <= iPaethNorm)
  {
    *m_puFilterByte = UP;
    *m_pScanlineBuffer = upBuffer;
  }
  else if (iAverageNorm <= iPaethNorm)
  {
    *m_puFilterByte = AVERAGE;
    *m_pScanlineBuffer = averageBuffer;
  }
  else
  {
    *m_puFilterByte = PAETH;
    *m_pScanlineBuffer = paethBuffer;
  }

} // EncodeScanline()


/**
 * Decode (unfilter) the data using the specified filter.
 */
void ScanlineFilter::DecodeScanline()
{
  switch ( * m_puFilterByte )
  {

  case NONE:
    break;

  case SUB:
    DecodeSub();
    break;

  case UP:
    DecodeUp();
    break;

  case AVERAGE:
    DecodeAverage();
    break;

  case PAETH:
    DecodePaeth();
    break;

  default:
    throw ex::DataCorruptionException( LogRec( "Undefined filter type.", "ScanlineFilter", "DecodeScanline" ) );
    break;

  } // switch filter byte

  * m_puFilterByte = NONE;

} // DecodeScanline()


/**
 * Decode the sub filter.
 */
void ScanlineFilter::DecodeSub()
{
  // sub filter: Sub(x) + Raw(x-bpp)
  for (uint4 i(0); i<m_pScanlineBuffer->GetSize(); ++i)
  {
    const uint4 uThisPixel( ( * m_pScanlineBuffer )[ i ] );
    const uint4 uLeftPixel( ( i > 2 ) ? ( * m_pScanlineBuffer )[ i - m_uBytesPerPixel ] : 0 );
    ( * m_pScanlineBuffer )[ i ] = uint1((uThisPixel + uLeftPixel) % 256);
  }
} // DecodeSub()


/**
 * Decode the up filter.
 */
void ScanlineFilter::DecodeUp()
{
  for (uint4 i(0); i<m_pScanlineBuffer->GetSize(); ++i)
  {
    const uint4 uThisPixel((*m_pScanlineBuffer)[i]);
    const uint4 uUpPixel((*m_pPrevScanlineBuffer)[i]);
    (*m_pScanlineBuffer)[i] = uint1((uThisPixel + uUpPixel) % 256);
  }
} // DecodeUp()


/**
 * Decode the average filter.
 */
void ScanlineFilter::DecodeAverage()
{
  for (uint4 i(0); i<m_pScanlineBuffer->GetSize(); ++i)
  {
    const uint4 uThisPixel((*m_pScanlineBuffer)[i]);
    const uint4 uUpPixel((*m_pPrevScanlineBuffer)[i]);
    const uint4 uLeftPixel((i>2) ? (*m_pScanlineBuffer)[i-m_uBytesPerPixel] : 0);
    uint4 uAverage((uLeftPixel + uUpPixel) / 2);
    (*m_pScanlineBuffer)[i] = uint1((uThisPixel + uAverage) % 256);
  }
} // DecodeAverage()


/**
 * Decode the Paeth filter.
 */
void ScanlineFilter::DecodePaeth()
{
  for (uint4 i(0); i<m_pScanlineBuffer->GetSize(); ++i)
  {
    const uint4 uThisPixel((*m_pScanlineBuffer)[i]);
    const uint4 uUpPixel((*m_pPrevScanlineBuffer)[i]);
    const uint4 uLeftPixel((i>2) ? (*m_pScanlineBuffer)[i-m_uBytesPerPixel] : 0);
    const uint4 uUpperLeftPixel((i>2) ? (*m_pPrevScanlineBuffer)[i-m_uBytesPerPixel] : 0);
    const uint4 uPaethPredictedPixel(PaethPredictor(uLeftPixel, uUpPixel, uUpperLeftPixel));
    (*m_pScanlineBuffer)[i] = uint1((uThisPixel + uPaethPredictedPixel) % 256);
  }
} // DecodePaeth()


/**
 * Predict the next pixel using the Paeth predictor.
 * @param iLeft
 * @param iAbove
 * @param iUpLeft
 * @return
 */
int4 ScanlineFilter::PaethPredictor(const int4 iLeft, const int4 iAbove, const int4 iUpLeft) const
{
  // initial estimate
  int4 iEstimate(iLeft + iAbove - iUpLeft);
  
  // compute distances to three pixels
  int4 iDistLeft  (iEstimate - iLeft  );
  int4 iDistAbove (iEstimate - iAbove );
  int4 iDistUpLeft(iEstimate - iUpLeft);
  
  // compute absolute values
  iDistLeft   *= (iDistLeft  <0) ? -1 : 1;
  iDistAbove  *= (iDistAbove <0) ? -1 : 1;
  iDistUpLeft *= (iDistUpLeft<0) ? -1 : 1;
  
  // return nearest of left,above,upLeft, breaking ties in order left,above,upLeft.
  if (iDistLeft <= iDistAbove && iDistLeft <= iDistUpLeft)
    return iLeft;
  else if (iDistAbove <= iDistUpLeft)
    return iAbove;
  else
    return iUpLeft;

} // PaethPredictor()


/**
 * Encode the scanline using the none filter into the test buffer and return the error norm.
 * @return
 */
uint4 ScanlineFilter::EncodeNone() const
{
  uint4 uNorm(0);
  for (uint4 i(0); i<m_pScanlineBuffer->GetSize(); ++i)
  {
    const int4 iThisPixel((*m_pScanlineBuffer)[i]);
    uNorm += (iThisPixel> 0) ? iThisPixel: -iThisPixel;
  }
  return uNorm;
} // EncodeNone()


/**
 * Encode the scanline using the sub filter into the test buffer and return the error norm.
 * @param testScanlineBuffer
 * @return
 */
uint4 ScanlineFilter::EncodeSub(MemoryBuffer & testScanlineBuffer) const
{
  uint4 uNorm(0);
  for (uint4 i(0); i<m_pScanlineBuffer->GetSize(); ++i)
  {
    const uint4 uThisPixel((*m_pScanlineBuffer)[i]);
    const uint4 uLeftPixel((i>2) ? (*m_pScanlineBuffer)[i-m_uBytesPerPixel] : 0);
    const int4 iError(int4(uThisPixel) - int4(uLeftPixel));
    uNorm += (iError > 0) ? iError : -iError;
    testScanlineBuffer[i] = uint1((iError + 256) % 256);
  }
  return uNorm;
} // EncodeSub()


/**
 * Encode the scanline using the up filter into the test buffer and return the error norm.
 * @param testScanlineBuffer
 * @return
 */
uint4 ScanlineFilter::EncodeUp(MemoryBuffer & testScanlineBuffer) const
{
  uint4 uNorm(0);
  for (uint4 i(0); i<m_pScanlineBuffer->GetSize(); ++i)
  {
    const uint4 uThisPixel((*m_pScanlineBuffer)[i]);
    const uint4 uUpPixel((*m_pPrevScanlineBuffer)[i]);
    const int4 iError(int4(uThisPixel) - int4(uUpPixel));
    uNorm += (iError > 0) ? iError : -iError;
    testScanlineBuffer[i] = uint1((iError + 256) % 256);
  }
  return uNorm;
} // EncodeUp()


/**
 * Encode the scanline using the average filter into the test buffer and return the error norm.
 * @param testScanlineBuffer
 * @return
 */
uint4 ScanlineFilter::EncodeAverage(MemoryBuffer & testScanlineBuffer) const
{
  uint4 uNorm(0);
  for (uint4 i(0); i<m_pScanlineBuffer->GetSize(); ++i)
  {
    const uint4 uThisPixel((*m_pScanlineBuffer)[i]);
    const uint4 uLeftPixel((i>2) ? (*m_pScanlineBuffer)[i-m_uBytesPerPixel] : 0);
    const uint4 uUpPixel((*m_pPrevScanlineBuffer)[i]);
    const uint4 uAverage((uLeftPixel + uUpPixel)/2);
    const int4 iError(int4(uThisPixel) - int4(uAverage));
    uNorm += (iError > 0) ? iError : -iError;
    testScanlineBuffer[i] = uint1((iError + 256) % 256);
  }
  return uNorm;
} // EncodeAverage()


/**
 * Encode the scanline using the Paeth filter into the test buffer and return the error norm.
 * @param testScanlineBuffer
 * @return
 */
uint4 ScanlineFilter::EncodePaeth(MemoryBuffer & testScanlineBuffer) const
{
  uint4 uNorm(0);
  for (uint4 i(0); i<m_pScanlineBuffer->GetSize(); ++i)
  {
    const uint4 uThisPixel((*m_pScanlineBuffer)[i]);
    const uint4 uUpPixel((*m_pPrevScanlineBuffer)[i]);
    const uint4 uLeftPixel((i>2) ? (*m_pScanlineBuffer)[i-m_uBytesPerPixel] : 0);
    const uint4 uUpperLeftPixel((i>2) ? (*m_pPrevScanlineBuffer)[i-m_uBytesPerPixel] : 0);
    const uint4 uPaethPredictedPixel(PaethPredictor(uLeftPixel, uUpPixel, uUpperLeftPixel));
    const int4 iError(int4(uThisPixel) - int4(uPaethPredictedPixel));
    uNorm += (iError > 0) ? iError : -iError;
    testScanlineBuffer[i] = uint1((iError + 256) % 256);
  }
  return uNorm;
} // EncodePaeth()


#undef FILE_REVISION


// $Log: ScanlineFilter.C,v $
// Revision 1.2  2005/05/23 12:47:48  frank
// fixed file revision definition so that we get correct information in log files
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/05/16 16:48:02  frank
// code review
//
// Revision 3.4  2003/05/16 11:44:23  michael
// code review
//
// Revision 3.3  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.2  2002/03/08 22:38:39  michael
// include cleanup
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:20  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:20   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:46   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:45  soeren
// renamed image directory
//
// Revision 1.6  2001/08/13 20:05:28  tushar
// removed errors due to the new logger
//
// Revision 1.5  2001/07/25 00:29:15  tushar
// fixed some compiler errors (logger)
//
// Revision 1.4  2001/07/20 11:53:42  frank
// Debugging EncodeScanline()
//
// Revision 1.3  2001/07/13 11:51:42  frank
// Added bytes per pixel.
//
// Revision 1.2  2001/07/13 03:40:34  frank
// Implemented encoding and optimization.
//
// Revision 1.1  2001/07/13 03:09:15  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/ScanlineFilter.C,v 1.2 2005/05/23 12:47:48 frank Exp $
// $Id: ScanlineFilter.C,v 1.2 2005/05/23 12:47:48 frank Exp $
