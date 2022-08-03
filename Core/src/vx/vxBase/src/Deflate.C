// $Id: Deflate.C,v 1.2 2006/02/09 13:57:57 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Dave McMananamon (dave@viatronix.com)


// Includes
#include "stdafx.h"
#include <minilzo.h>
#include <zlib.h>
#include "Exception.h"
#include "Deflate.h"
#include "logger.h"

// Namespaces
using namespace hdr;
using namespace std ;


// Defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 */
Deflate::Deflate()
{
  m_lzoWrkmem = new uint1 [LZO1X_1_MEM_COMPRESS];
} // Deflate()



/**
 * Destructor
 */
Deflate::~Deflate()
{
  delete [] m_lzoWrkmem;
} // ~Deflate()


/**
 * Decompress the input data, returning n for success, -n for failure.
 *
 * @param fc            the type of compression used in the data chunk.
 * @param puSource      the compressed data.
 * @param iSourceLength the size of the compressed data.
 * @param puDest        the uncompressed data chunk.
 * @param uDestLength   the size of the uncompressed chunk.
 * @return unknown
 */
int4 Deflate::DeflateIt( const hdr::FileCompressionEnum & fc, uint1 * puSource, size_t iSourceLength, uint1 * puDest, uint4 & uDestLength )
{
  int4 result( -1 );

  switch ( fc ) 
  {
  case LZO:
    result = DeflateLZO( puSource, iSourceLength, puDest, uDestLength );
    break;
  case ZLIB:
    result = DeflateZLIB( puSource, iSourceLength, puDest, uDestLength );
    break;
  case NONE:
    LogWrn( "Compression Type not supported ", "Deflate", "DeflateIt" );
    throw ex::UnsupportedOperationException( LogRec( "Compression type not supported", "Deflater", "DeflateIt" ) );
    break;
  }

  return result;

} // DeflateIt()


/**
 * Decompress with LZO compression library
 *
 * @param puSource      the compressed data.
 * @param iSourceLength the size of the compressed data.
 * @param puDest        the uncompressed data chunk.
 * @param uDestLength   the size of the uncompressed chunk.
 * @return unknown
 */
int4 Deflate::DeflateLZO( uint1 * puSource, size_t iSourceLength, uint1 * puDest, uint4 & uDestLength )
{
  lzo_uint tmp( uDestLength );
  int4 rc( lzo1x_1_compress( puSource, static_cast< lzo_uint >( iSourceLength ), puDest, & tmp, m_lzoWrkmem ) );
  uDestLength = tmp;
  if ( rc == LZO_E_OK )
    return 1;
	else 
		throw ex::RuntimeException( LogRec( "Unable to inflate.", "Inflate", "InflateLZO" ) );

} // DeflateLZO()


/**
 * ZEXTERN int ZEXPORT compress OF((Bytef *dest,   uLongf *destLen,
 *                                  const Bytef *source, uLong sourceLen));
 * Compresses the source buffer into the destination buffer.  sourceLen is
 * the byte length of the source buffer. Upon entry, destLen is the total
 * size of the destination buffer, which must be at least 0.1% larger than
 * sourceLen plus 12 bytes. Upon exit, destLen is the actual size of the
 * compressed buffer.
 * This function can be used to compress a whole file at once if the
 * input file is mmap'ed.
 * compress returns Z_OK if success, Z_MEM_ERROR if there was not
 * enough memory, Z_BUF_ERROR if there was not enough room in the output
 * buffer.
 *
 * @param puSource      the compressed data.
 * @param iSourceLength the size of the compressed data.
 * @param puDest        the uncompressed data chunk.
 * @param uDestLength   the size of the uncompressed chunk.
 * @return Z_OK, Z_MEM_ERROR, or Z_BUF_ERROR (see docs for details)
 */
int4 Deflate::DeflateZLIB( uint1 * puSource, size_t iSourceLength, uint1 * puDest, uint4 & uDestLength )
{

  unsigned long uLongDestLen( uDestLength );
  int4 iCompResult( compress( puDest, & uLongDestLen, (Bytef *) puSource, static_cast< uLong >( iSourceLength )));
  uDestLength = uLongDestLen;
  
  if ( iCompResult == Z_OK )
  {
    return 1;
  }
  else
  {
    LogWrn( "ZLIB compression failed.", "Deflate", "DeflateZLIB" );
    return -1;
  }

} // DeflateZLIB()


#undef FILE_REVISION


// $Log: Deflate.C,v $
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
// Revision 3.3  2002/05/01 04:42:23  soeren
// work on write of block vol ver 1.0
//
// Revision 3.2  2002/03/08 22:38:39  michael
// include cleanup
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
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
//    Rev 2.0   Sep 16 2001 23:47:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:34   ingmar
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
// Revision 1.1  2001/09/01 00:11:44  soeren
// renamed image directory
//
// Revision 1.10  2001/08/13 20:05:28  tushar
// removed errors due to the new logger
//
// Revision 1.9  2001/08/07 17:02:44  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.8  2001/07/24 23:56:09  tushar
// fixed some compiler errors (logger)
//
// Revision 1.7  2001/07/16 23:21:05  jmeade
// use detailed exception classes whenever possible
//
// Revision 1.6  2001/06/29 19:45:27  frank
// Added ZLIB compression.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Deflate.C,v 1.2 2006/02/09 13:57:57 geconomos Exp $
// $Id: Deflate.C,v 1.2 2006/02/09 13:57:57 geconomos Exp $
