// $Id: ScanlineFilter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef ScanlineFilter_h
#define ScanlineFilter_h


// includes
#include "Typedef.h"
#include "MemoryBuffer.h"


/**
 * Implements a scanline filter used to compress and decompress scanlines for PNG images.
 */
class ScanlineFilter
{
// Enumerations
public:

  /// the types of scanline filtering
  enum FilterTypeEnum { NONE, SUB, UP, AVERAGE, PAETH };

// Member Functions
public:

  /// Constructor.
  ScanlineFilter( MemoryBuffer & scanlineBuffer, MemoryBuffer & prevScanlineBuffer, const uint4 uBytesPerPixel );

  /// Destructor.
  ~ScanlineFilter();

  /// Encode (filter) the data using the "best" filter.
  void EncodeScanline();

  /// Decode (unfilter) the data using the specified filter.
  void DecodeScanline();

  /// Decode the sub filter.
  void DecodeSub();

  /// Decode the up filter.
  void DecodeUp();

  /// Decode the average filter.
  void DecodeAverage();

  /// Decode the Paeth filter.
  void DecodePaeth();

  /// Predict the next pixel using the Paeth predictor.
  int4 PaethPredictor( const int4 iLeft, const int4 iAbove, const int4 iUpLeft ) const;

  /// Encode the scanline using the none filter into the test buffer and return the error norm.
  uint4 EncodeNone() const;
  
  /// Encode the scanline using the sub filter into the test buffer and return the error norm.
  uint4 EncodeSub( MemoryBuffer & testScanlineBuffer ) const;
  
  /// Encode the scanline using the up filter into the test buffer and return the error norm.
  uint4 EncodeUp( MemoryBuffer & testScanlineBuffer ) const;
  
  /// Encode the scanline using the average filter into the test buffer and return the error norm.
  uint4 EncodeAverage( MemoryBuffer & testScanlineBuffer ) const;
  
  /// Encode the scanline using the Paeth filter into the test buffer and return the error norm.
  uint4 EncodePaeth( MemoryBuffer & testScanlineBuffer ) const;
  
// Data Members
private:

  /// The current scanline.
  MemoryBuffer * m_pScanlineBuffer;

  /// The previous scanline.
  MemoryBuffer * m_pPrevScanlineBuffer;

  /// The filter byte which indicates the current filter method.
  uint1 * m_puFilterByte;

  /// The number of bytes per pixel.
  uint4 m_uBytesPerPixel;

}; // class ScanlineFilter

#endif // ScanlineFilter_h

// $Log: ScanlineFilter.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.4  2003/05/16 16:48:02  frank
// code review
//
// Revision 3.3  2003/05/16 11:44:23  michael
// code review
//
// Revision 3.2  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.1  2001/12/24 15:36:55  ingmar
// added class Sec_20_image to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:20  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:22   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:46   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:45  soeren
// renamed image directory
//
// Revision 1.3  2001/07/13 11:51:28  frank
// Added bytes per pixel.
//
// Revision 1.2  2001/07/13 03:40:34  frank
// Implemented encoding and optimization.
//
// Revision 1.1  2001/07/13 03:09:15  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ScanlineFilter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: ScanlineFilter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
