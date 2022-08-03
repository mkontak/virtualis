// $Id: Inflate.C,v 1.2 2006/02/09 13:57:57 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: David dave@viatronix.com


// Includes
#include "stdafx.h"
#include <zlib.h>
#include <minilzo.h>
#include "vxInflate.h"
#include "Exception.h"


// Namespaces
using namespace hdr;
using namespace std;


// Defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Decompress the input data
 *
 * @param fc            the type of compression used in the data chunk
 * @param pSource       the compressed data
 * @param iSourceLength the size of the compressed data
 * @param puDest        the uncompressed data chunk
 * @param uDestLength   the size of the uncompressed chunk
 * @return              n for success, -n for failure
 */
int4 Inflate::InflateIt( const hdr::FileCompressionEnum  & fc, uint1 * pSource, size_t iSourceLength, uint1 * puDest, uint8 uDestLength )
{
  int4 result( -1 );
  switch ( fc )
  {
  case LZO:
    result = InflateLZO( pSource, iSourceLength, puDest, static_cast< uint4 >( uDestLength ));
    break;
  case ZLIB:
    result = InflateZLIB( pSource, iSourceLength, puDest, uDestLength );
    break;
  case NONE:
    // just copy data to new location
    if ( memmove( puDest, pSource, iSourceLength ) != NULL ) return 2;
    break;
  }

  return result;

} // InflateIt()


/**
 * Inflate zlib compressed data
 *
 * @param pSource       the compressed data
 * @param iSourceLength the size of the compressed data
 * @param puDest        the uncompressed data chunk
 * @param uDestLength   the size of the uncompressed chunk
 * @return              n for success, -n for failure
 */
int4 Inflate::InflateZLIB(uint1 * pSource, size_t iSourceLength, uint1 * puDest, uint8 uDestLength )
{
  int4 result( uncompress( (Bytef *) puDest, reinterpret_cast<unsigned long *>( & uDestLength), pSource, static_cast< uLong >( iSourceLength ) ));

  if ( result != Z_OK )
    throw ex::RuntimeException( LogRec( "Unable to uncompress.", "Inflate", "m_InflateZLIB" ) );

  return 1;

} // InflateZLIB()


/**
 * Inflate LZO compressed data
 *
 * @param pSource       the compressed data
 * @param iSourceLength the size of the compressed data
 * @param puDest        the uncompressed data chunk
 * @param uDestLength   the size of the uncompressed chunk
 * @return              n for success, -n for failure
 */
int4 Inflate::InflateLZO( uint1 * pSource, size_t iSourceLength, uint1 * puDest, uint4 uDestLength )
{
  if ( lzo_init() != LZO_E_OK )
    throw ex::RuntimeException( LogRec( "Unable to initialize.", "Inflate", "m_InflateLZO" ) );

  lzo_uint newLen( uDestLength );
  int4 rc( lzo1x_decompress_safe( (lzo_byte *) pSource, (lzo_uint) iSourceLength, (lzo_byte *) puDest, (lzo_uint *) & uDestLength, NULL ) );
  if ( rc == LZO_E_OK  && uDestLength == newLen )
  {
    // success
    return 1;
  }
  else
  {
    switch ( rc )
    {
    case LZO_E_OK                 : throw ex::VException( LogRec( "Unable to inflate: destLen != newLen" , "Inflate", "m_InflateLZO" ) ); 
    case LZO_E_ERROR              : throw ex::VException( LogRec( "Unable to inflate: ERROR"             , "Inflate", "m_InflateLZO" ) );
    case LZO_E_OUT_OF_MEMORY      : throw ex::VException( LogRec( "Unable to inflate: OUT_OF_MEMORY"     , "Inflate", "m_InflateLZO" ) );
    case LZO_E_NOT_COMPRESSIBLE   : throw ex::VException( LogRec( "Unable to inflate: NOT_COMPRESSIBLE"  , "Inflate", "m_InflateLZO" ) );
    case LZO_E_INPUT_OVERRUN      : throw ex::VException( LogRec( "Unable to inflate: INPUT_OVERRUN"     , "Inflate", "m_InflateLZO" ) );
    case LZO_E_OUTPUT_OVERRUN     : throw ex::VException( LogRec( "Unable to inflate: OUTPUT_OVERRUN"    , "Inflate", "m_InflateLZO" ) );
    case LZO_E_LOOKBEHIND_OVERRUN : throw ex::VException( LogRec( "Unable to inflate: LOOKBEHIND_OVERRUN", "Inflate", "m_InflateLZO" ) );
    case LZO_E_EOF_NOT_FOUND      : throw ex::VException( LogRec( "Unable to inflate: OF_NOT_FOUND"      , "Inflate", "m_InflateLZO" ) );
    case LZO_E_INPUT_NOT_CONSUMED : throw ex::VException( LogRec( "Unable to inflate: INPUT_NOT_CONSUMED", "Inflate", "m_InflateLZO" ) );
    default                       : throw ex::VException( LogRec( "Unable to inflate. "                  , "Inflate", "m_InflateLZO" ) );
    }
  }
} // InflateLZO()


#undef FILE_REVISION


// $Log: Inflate.C,v $
// Revision 1.2  2006/02/09 13:57:57  geconomos
// removal of libpng
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.8  2003/05/16 16:44:29  frank
// code review
//
// Revision 3.7  2003/05/16 13:15:39  frank
// formatting
//
// Revision 3.6  2003/05/16 11:44:22  michael
// code review
//
// Revision 3.5  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.4  2002/05/01 17:24:38  soeren
// code beautify
//
// Revision 3.3  2002/05/01 17:15:33  soeren
// added more precise messages of what went wrong if inflate fails
//
// Revision 3.2  2002/04/18 15:56:10  ingmar
// enabled optmized read and write of new vxBlockVolume
//
// Revision 3.1  2002/03/08 22:38:39  michael
// include cleanup
//
// Revision 3.0  2001/10/14 23:02:19  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 01 2001 16:10:14   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:47:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:38   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:45  soeren
// renamed image directory
//
// Revision 1.6  2001/04/09 22:25:43  dave
// changing from Abort to use exceptions
//
// Revision 1.5  2001/04/09 16:25:34  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.4  2001/04/08 22:38:44  dave
// added set_terminate() handler
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
// Revision 1.5  2001/02/17 23:15:11  ingmar
// moved deflate and Inflate to 8_volume
//
// Revision 1.1  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.3  2000/11/03 03:14:07  ingmar
// fixed some gcc syntax errors and warnings
//
// Revision 1.2  2000/10/31 17:16:02  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
// 
// 9     9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
// 
// 8     9/04/00 12:58p Dave
// 
// 7     8/29/00 2:50p Ingmar
// code beautify
// 
// 6     8/29/00 12:17p Antonio
// updated header
// 
// 5     8/16/00 1:56p Antonio
// modified header
// 
// *****************  Version 4  *****************
// User: Antonio      Date: 8/11/00    Time: 12:20p
// Updated in $/vx/src/Volume
// added header and history to end of file
// 
// *****************  Version 3  *****************
// User: Jay             Date:  7/19/00  Time:  2:15p
// Checked in $/vx/src/Volume
// Comment:
//   Synchronized with CVS
// 
// *****************  Version 2  *****************
// User: Frank           Date:  7/18/00  Time:  3:57p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Removed "using namespace *" from header files to prevent ambiguities
// 
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:03a
// Created Inflate.C
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Inflate.C,v 1.2 2006/02/09 13:57:57 geconomos Exp $
// $Id: Inflate.C,v 1.2 2006/02/09 13:57:57 geconomos Exp $
