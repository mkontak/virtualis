// $Id: CRC.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "CRC.h"


// Namespaces
using namespace std;


/**
 * Constructor
 */
CRC::CRC()
{

  for (unsigned long n(0); n<256; n++)
  {
    unsigned long c(n);
    for (int k(0); k<8; k++)
    {
      if (c & 1)
      {
        c = 0xedb88320L ^ (c >> 1);
      }
      else
      {
        c = c >> 1;
      }
    }
    m_vTable[n] = c;
  }

} // CRC()


/**
 * Generate the new complement
 *
 * @param uComplement the old complement
 * @return            the new complement
 */
unsigned long CRC::CrcComplement( const unsigned long uComplement )
{

  return uComplement ^ 0xffffffff;

} // CrcComplement


/**
 * Initialize
 *
 * @return the initial complement
 */
unsigned long CRC::CrcInit()
{

  return CrcComplement( 0 );

} // CrcInit()


/**
 * Computes the CRC checksum for a buffer of bytes
 *
 * @param sChunkType the chunk type
 * @param pBuffer    the buffer
 * @uBuffersize      the size of the buffer
 * @return           the checksum
 */
uint4 CRC::ComputeChecksum(const std::string & sChunkType, const uint1 * pBuffer, const uint4 uBufferSize) 
{

  unsigned long crc(UpdateCrc(CrcInit(),(unsigned char *)sChunkType.c_str(), 4));
  crc = UpdateCrc(crc, pBuffer, uBufferSize);
  crc = CrcComplement(crc);
  return crc;
  //ccrc = UpdateCrc(0xffffffffL, pBuffer, iBufferSize) ^ 0xffffffffL;

} // ComputeChecksum()


/**
 * Update a running CRC with the bytes buf[0..len-1]--the CRC
 * should be initialized to all 1's, and the transmitted value
 * is the 1's complement of the final running CRC (see the
 * crc() routine below)). 
 *
 * @param uCRc    the old CRC
 * @param puBuf   the buffer 
 * @param iLength the buffer length
 * @return        the final crc
 */
unsigned long CRC::UpdateCrc( unsigned long uCrc, const unsigned char * puBuf, const int iLength )
{
  unsigned long c( uCrc );
  unsigned char const * p =  puBuf;

  for ( int4 n(0); n<iLength; n++ )
    c = m_vTable[(c ^ (*p++)) & 0xff] ^ (c >> 8);

  return c;
} // UpdateCrc()


// $Log: CRC.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/05/16 18:02:33  mkontak
// Compiler issue
//
// Revision 3.4  2003/05/16 16:44:29  frank
// code review
//
// Revision 3.3  2003/05/16 11:44:22  michael
// code review
//
// Revision 3.2  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.1  2002/04/26 12:38:54  soeren
// added checksum test for reading and writing vxBlockVolumes
//
// Revision 3.0  2001/10/14 23:02:18  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:32   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:44  soeren
// renamed image directory
//
// Revision 1.4  2001/06/24 21:01:21  frank
// Used actual CRC algorithm for PNG files.
//
// Revision 1.3  2001/06/19 19:08:35  frank
// Fixed checksum.
//
// Revision 1.2  2001/06/19 16:29:52  frank
// Integrated V3DFile class into system.
//
// Revision 1.1  2001/06/19 15:53:06  frank
// Initial version.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/CRC.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: CRC.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
