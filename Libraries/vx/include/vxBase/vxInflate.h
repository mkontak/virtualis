// $Id: Inflate.h,v 1.3 2006/03/29 19:38:38 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: David dave@viatronix.com

#ifndef Inflate_h
#define Inflate_h


// Namespaces
namespace hdr 
{
  enum FileCompressionEnum 
  {
    NONE, LZO, ZLIB // could add BZIP2 for better,slower comp.
  };
} // namespace hdr

/**
 * Compression library - inflation routines
 */
class VX_BASE_DLL Inflate 
{
// functions
public:

  /// Decompress the input data, returning n for success, -n for failure
  static int InflateIt( const hdr::FileCompressionEnum  & fc, uint1 * pSource, size_t iSourceLength, uint1 * puDest, uint8 uDestLength );

private:

  /// Inflate zlib compressed data
  static int InflateZLIB(uint1 * pSource, size_t iSourceLength, uint1 * puDest, uint8 uDestLength);

  /// Inflate LZO compressed data
  static int InflateLZO(uint1 * pSource, size_t iSourceLength, uint1 * puDest, uint4 uDestLength);

}; // Inflate


#endif // Inflate_h


// Revision History:
// $Log: Inflate.h,v $
// Revision 1.3  2006/03/29 19:38:38  geconomos
// changed Inflate methods to static
//
// Revision 1.2  2006/02/09 13:57:57  geconomos
// removal of libpng
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.7  2003/05/17 15:49:14  michael
// code review
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
// Revision 3.3  2002/08/14 16:11:36  wenli
// initial version - Vascular
//
// Revision 3.2  2002/03/08 17:15:18  michael
// further removed include dependencies
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
//    Rev 2.0   Sep 16 2001 23:47:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:38   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:45  soeren
// renamed image directory
//
// Revision 1.5  2001/04/12 14:37:52  dmitry
// Sec_5_image base class added
//
// Revision 1.4  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.3  2001/04/05 18:00:14  dmitry
// updated datatypes
//
// Revision 1.2  2001/03/28 20:06:54  dmitry
// Update typedef, implemented proper casting
//
// Revision 1.1  2001/02/18 21:44:44  ingmar
// moved Deflate and Inflate to 5_image
//
// Revision 1.5  2001/02/17 23:15:12  ingmar
// moved deflate and Inflate to 8_volume
//
// Revision 1.1  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.3  2000/10/31 17:16:02  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.2  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.1.1.1.2.1  2000/09/25 18:56:36  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
// 
// 9     9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
// 
// 8     9/01/00 5:56p Dave
// initial code walkthrough changes
// 
// 7     8/21/00 1:07a Sarang
// modified header
// 
// *****************  Version 6  *****************
// User: Antonio      Date: 8/11/00    Time: 1:04p
// Updated in $/vx/src/Volume
// added header and history to end of file.
// 
// *****************  Version 5  *****************
// User: Jay             Date:  8/11/00  Time:  1:29a
// Checked in $/v2k/src/Volume
// Comment:
//   
// 
// *****************  Version 4  *****************
// User: Jay             Date:  8/08/00  Time: 10:45a
// Checked in $/vx/src/Volume
// Comment:
//   got old volume library back into working state
// 
// *****************  Version 3  *****************
// User: Jay             Date:  8/07/00  Time:  9:34p
// Checked in $/vx/src/Volume
// Comment:
//   Made a template version of volume
// 
// *****************  Version 2  *****************
// User: Frank           Date:  7/18/00  Time:  3:57p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Removed "using namespace *" from header files to prevent ambiguities
// 
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:03a
// Created Inflate.h
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Inflate.h,v 1.3 2006/03/29 19:38:38 geconomos Exp $
// $Id: Inflate.h,v 1.3 2006/03/29 19:38:38 geconomos Exp $
