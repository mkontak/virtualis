// $Id: Deflate.h,v 1.2 2006/02/09 13:57:57 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Dave McMananamon (dave@viatronix.com)

#ifndef Deflate_h
#define Deflate_h


// includes
#include "vxInflate.h"

/**
 * Compression library - decompression routines
 */
class VX_BASE_DLL Deflate 
{
// Member Functions
public:

  /// Constructor
  Deflate();

  /// Destructor
  ~Deflate();

  /// Decompress the input data, returning n for success, -n for failure
  int4 DeflateIt( const hdr::FileCompressionEnum  & fc, uint1 * puSource, size_t iSourceLength, uint1 * puDest, uint4 & uDestLength );

private:

  /// Deflate with LZO compression
  int4 DeflateLZO( uint1 * puSource, size_t iSourceLength, uint1 * puDest, uint4 & uDestLength );

  /// Deflate with Zlib compression
  int4 DeflateZLIB( uint1 * puSource, size_t iSourceLength, uint1 * puDest, uint4 & uDestLength );

// Data Members
protected:

  /// the working memory
  uint1 * m_lzoWrkmem;

}; // class Deflate


#endif // Deflate_h


// $Log: Deflate.h,v $
// Revision 1.2  2006/02/09 13:57:57  geconomos
// removal of libpng
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.6  2003/05/16 16:44:29  frank
// code review
//
// Revision 3.5  2003/05/16 11:44:22  michael
// code review
//
// Revision 3.4  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.3  2002/05/06 02:47:30  michael
// removed commented out code
//
// Revision 3.2  2002/03/08 22:38:39  michael
// include cleanup
//
// Revision 3.1  2001/12/24 15:36:55  ingmar
// added class Sec_20_image to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:19  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:34   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:44  soeren
// renamed image directory
//
// Revision 1.6  2001/06/29 19:45:27  frank
// Added ZLIB compression.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Deflate.h,v 1.2 2006/02/09 13:57:57 geconomos Exp $
// $Id: Deflate.h,v 1.2 2006/02/09 13:57:57 geconomos Exp $
