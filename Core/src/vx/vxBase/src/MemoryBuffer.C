// $Id: MemoryBuffer.C,v 1.3 2006/03/29 19:37:04 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


// include declarations
#include "stdafx.h"
#include "MemoryBuffer.h"
#include "vxInflate.h"
#include "Deflate.h"
#include "Exception.h"


// Namespaces
using namespace std ;


// Defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Copy constructor
 *
 * @param other the other memory buffer
 */
MemoryBuffer::MemoryBuffer(const MemoryBuffer & other)
{
  m_uCapacity = m_uSize = other.m_uSize;
  if (m_uCapacity == 0)
  {
    m_pBuffer = NULL;
  }
  else
  {
    m_pBuffer = new uint1[m_uCapacity];
    for (uint4 i(0); i<m_uSize; ++i)
    {
      m_pBuffer[i] = other.m_pBuffer[i];
    }
  }
  m_bProtected = false;
} // MemoryBuffer( other )


/**
 * Constructor
 *
 * @param uInitialSize the initial buffer size
 */
MemoryBuffer::MemoryBuffer(const uint4 & uInitialSize)
{
  m_uCapacity = m_uSize = uInitialSize;
  if (m_uCapacity == 0)
  {
    m_pBuffer = NULL;
  }
  else
  {
    m_pBuffer = new uint1[m_uCapacity];
  }
  m_bProtected = false;
} // MemoryBuffer(const uint4 & uInitialSize)


/**
 * Constructor from an externally supplied buffer.
 * It will never resize beyond the original size, but it may contract
 * and not use some of the space
 *
 * @param puBuffer    the pointer to the buffer
 * @param uBufferSize the buffer size
 */
MemoryBuffer::MemoryBuffer(uint1 * const puBuffer, const uint4 & uBufferSize)
{
  m_pBuffer = puBuffer;
  m_uCapacity = uBufferSize;
  m_uSize = uBufferSize;
  m_bProtected = true;
} // MemoryBuffer( puBuffer, uBufferSize )


/**
 * Desturctor
 */
MemoryBuffer::~MemoryBuffer()
{

  if ( m_uCapacity > 0 && m_bProtected == false )
  {
    delete [] m_pBuffer;
    m_pBuffer = NULL;
  }

} // ~MemoryBuffer()


/**
 * Assignment operator
 *
 * @param other the other buffer
 */
MemoryBuffer & MemoryBuffer::operator =(const MemoryBuffer & other)
{
  if ( & other != this )
  {

    Resize( other.GetSize() );

    for ( uint4 i(0); i<m_uSize; ++i )
      m_pBuffer[i] = other.m_pBuffer[i];

  }

  return * this;

} // operator =()


/**
 * Set the current size.
 * Efficient if the size is within the current capacity.
 * Inefficient if the size is larger than the current capacity
 * so avoid calling repeatedly with larger and larger sizes.
 *
 * @param uNewSize the new buffer size
 */
void MemoryBuffer::Resize( const uint4 & uNewSize )
{

  Reserve(uNewSize);

  m_uSize = uNewSize;

} // Resize()


/**
 * Ensure there is enough capacity.
 *
 * @param uNewCapacity the capacity to reserve
 */
void MemoryBuffer::Reserve( const uint4 & uNewCapacity )
{
  if ( uNewCapacity <= m_uCapacity )
  {
    // plenty of space already
    return;
  } // if enough space already
  else
  {
    if ( m_bProtected )
      throw ex::OutOfMemoryException( LogRec( "Not allowed to reallocate memory.", "MemoryBuffer", "Reserve " ) );

    // not enough space yet
    uint1 * pTempBuffer = new uint1[ uNewCapacity ];

    // copy from the old to the new
    for ( uint4 i(0); i<m_uCapacity; ++i )
      pTempBuffer[i] = m_pBuffer[i];

    if ( m_uCapacity != 0 )
    {
      // there is nothing allocated yet, so nothing to delete
      delete [] m_pBuffer;
    }

    m_pBuffer = pTempBuffer;
    m_uCapacity = uNewCapacity;

  } // not enough space - had to reserve more

} // Reserve()


/**
 * Get the data pointer
 *
 * @return the data pointer
 */
uint1 * MemoryBuffer::GetDataPtr()
{
  return m_pBuffer;
} // GetDataPtr()


/**
 * Clear the buffer
 *
 * @param uClearValue the value to clear with
 */
void MemoryBuffer::Clear( const uint1 uClearValue )
{

  for ( uint4 i(0); i<m_uSize; i++ )
    m_pBuffer[i] = uClearValue;

} // Clear()


/**
 * Get the used size of the buffer
 *
 * @return the used size of the buffer
 */
const uint4 & MemoryBuffer::GetSize() const
{
  return m_uSize;
} // GetSize()


/** 
 * The capacity of the buffer
 *
 * @return the capacity of the buffer
 */
const uint4 & MemoryBuffer::GetCapacity() const
{
  return m_uCapacity;
} // GetCapacity()


/**
 * note: must reserve sufficient space in the destination buffer before calling
 *
 * @param eCompression      the type of compression
 * @param destinationBuffer the destination buffer
 */
void MemoryBuffer::InflateInto( const hdr::FileCompressionEnum eCompression, MemoryBuffer & destinationBuffer )
{

  // read the first four bytes to discover the uncompressed size
  const uint4 uUncompressedSize = *((uint4 *) m_pBuffer);
  
  // make sure there is enough space in destination
  if (destinationBuffer.m_uCapacity < uUncompressedSize)
    LogWrn( "Insufficient buffer.", "MemoryBuffer", "InflateInto" );

  destinationBuffer.Resize(uUncompressedSize);
  
  // inflate
  uint8 uNewSize(uUncompressedSize);
  int iResult = Inflate::InflateIt(eCompression, m_pBuffer+4, m_uSize-4, destinationBuffer.m_pBuffer, uNewSize);

  if (iResult < 0)
    throw ex::RuntimeException( LogRec( "Inflation failed.", "MemoryBuffer", "InflateInto" ) );

} // InflateInto()


/**
 * note: must reserve sufficient space in the destination buffer before calling
 *
 * @param eCompression      the compression type
 * @param destinationBuffer the destintation buffer
 */
void MemoryBuffer::DeflateInto( const hdr::FileCompressionEnum eCompression, MemoryBuffer & destinationBuffer )
{
  // copy the original size into the front of the buffer
  *((uint4 *) destinationBuffer.m_pBuffer) = m_uSize;
  
  // deflate and find out the new size
  uint4 uNewSize(m_uSize);
  Deflate i;
  int iResult = i.DeflateIt(eCompression, m_pBuffer, m_uSize, destinationBuffer.m_pBuffer+4, uNewSize);

  if ( iResult < 0 )
    throw ex::RuntimeException( LogRec( "Deflation failed.", "MemoryBuffer", "DeflateInto" ) );

  uNewSize += 4;

  if (uNewSize > destinationBuffer.m_uCapacity)
    throw ex::RuntimeException( LogRec( "Insufficient space.", "MemoryBuffer", "DeflateInto" ) );

  destinationBuffer.m_uSize = uNewSize;

} // DeflateInto()


/**
 * Copy the data in from another buffer
 *
 * @param pBuffer the other buffer
 * @param uSize   the other buffer size
 */
void MemoryBuffer::CopyIn( const uint1 * pBuffer, const uint4 uSize )
{
  Resize( uSize );
  try
  {
    for ( uint4 i(0); i<uSize; ++i )
      m_pBuffer[i] = pBuffer[i];
  }
  catch (...)
  {
    throw ex::RuntimeException( LogRec( "Buffer overrun during copy.", "MemoryBuffer", "CopyIn" ) );
  }
} // CopyIn()


/**
 * Copy the data out into another buffer
 *
 * @param pBuffer the other buffer
 * @param uSize   the other buffer size
 */
void MemoryBuffer::CopyOut( uint1 * pBuffer, const uint4 uSize ) const
{
  if ( uSize < m_uSize )
    throw ex::OutOfMemoryException( LogRec( "Insufficient memory to perform copy.", "MemoryBuffer", "CopyOut" ) );

  try
  {
    for ( uint4 i(0); i<m_uSize; ++i )
      pBuffer[i] = m_pBuffer[i];
  }
  catch (...)
  {
    throw ex::RuntimeException( LogRec( "Buffer overrun during copy.", "MemoryBuffer", "CopyOut" ) );
  }
} // CopyOut()


/**
 * Write out to a stream
 *
 * @param os the output stream
 */
void MemoryBuffer::Write(std::ofstream & os) const
{
  os.write((char *) m_pBuffer, m_uSize);
} // Write()


/**
 * Equality operator
 *
 * @param other the other buffer
 */
bool MemoryBuffer::operator ==(const MemoryBuffer & other) const
{
  if (m_uSize != other.m_uSize)
  {
    return false;
  }

  for (uint4 i(0); i<m_uSize; ++i)
  {
    if (m_pBuffer[i] != other.m_pBuffer[i])
    {
      return false;
    }
  }

  return true;
} // operator ==()


/**
 * Inequality operator
 *
 * @param other the other buffer
 */
bool MemoryBuffer::operator !=( const MemoryBuffer & other ) const
{
  return ! ( * this == other );
} // operator !=()


/**
 * Encode into a base 16-encoded string of characters
 *
 * @return the encoded buffer
 */
MemoryBuffer MemoryBuffer::GetEncodedBase16() const
{

  // will use soon...
  const uint1 base64[64] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                             'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                             'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                             'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

  // split each byte into two 4-bit pieces
  MemoryBuffer outputBuffer( unsigned int( m_uSize * 2 ) );
  for ( unsigned int i(0); i<m_uSize; ++i )
  {

    // split into two 16-bit characters
    unsigned char char0(   m_pBuffer[i]        & 0xF );
    unsigned char char1( ( m_pBuffer[i] >> 4 ) & 0xF );

    // convert each piece into ASCII hexadecimal format
    const unsigned char digits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    outputBuffer[ 2 * i + 0 ] = digits[ char0 ];
    outputBuffer[ 2 * i + 1 ] = digits[ char1 ];

  }

  return outputBuffer;

} // GetEncodedBase16()


/**
 * Decode from a base 16-encoded string of characters
 *
 * @return the decoded memory buffer
 */
MemoryBuffer MemoryBuffer::GetDecodedBase16() const
{

  const uint1 digits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  MemoryBuffer outputBuffer( m_uSize / 2 );

  for ( unsigned int i(0); i<m_uSize; ++i )
  {

    // first half digit
    uint1 inChar0( 0 );
    if ( m_pBuffer[i] >= '0' && m_pBuffer[i] <= '9' ) inChar0 = m_pBuffer[i] - '0';
    if ( m_pBuffer[i] >= 'A' && m_pBuffer[i] <= 'F' ) inChar0 = m_pBuffer[i] - 'A' + 10;

    ++i;

    // second half digit
    uint1 inChar1( 0 );
    if ( m_pBuffer[i] >= '0' && m_pBuffer[i] <= '9' ) inChar1 = m_pBuffer[i] - '0';
    if ( m_pBuffer[i] >= 'A' && m_pBuffer[i] <= 'F' ) inChar1 = m_pBuffer[i] - 'A' + 10;

    // combine into single value
    uint1 finalValue( inChar0 | ( inChar1 << 4 ) );
    outputBuffer.m_pBuffer[ ( i - 1 ) / 2 ] = finalValue;

  }
  
  return outputBuffer;

} // GetDecodedBase16()


/**
 * Convert to a string
 *
 * @return a standard string
 */
std::string MemoryBuffer::ToString() const
{

  // check for illegal length strings
  if ( m_uSize <= 0 ) return "";
  
  // create the string
  std::string output( m_uSize, ' ' );
  for ( int4 i(0); i<m_uSize; ++i )
  {
    output[i] = m_pBuffer[i];
  }
  return output;

} // ToString()


/**
 * Convert from a string
 *
 * @param sInput the input string
 *
 * @return converted memory buffer
 */
MemoryBuffer MemoryBuffer::FromString( const std::string & sInput )
{

  MemoryBuffer output;
  output.CopyIn( reinterpret_cast< const uint1 * const >( & sInput[0] ), sInput.size() );
  return output;

} // FromString()


// undefines
#undef FILE_REVISION


// $Log: MemoryBuffer.C,v $
// Revision 1.3  2006/03/29 19:37:04  geconomos
// changed Inflate methods to static
//
// Revision 1.2  2004/03/30 21:50:34  frank
// added base 16 encoding
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/05/16 16:44:29  frank
// code review
//
// Revision 3.4  2003/05/16 11:44:22  michael
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
// Revision 3.0  2001/10/14 23:02:19  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:40   ingmar
// Initial revision.
// Revision 1.6  2001/09/14 23:30:19  soeren
// changed LogFyi to LogDbg
//
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
// Revision 1.12  2001/08/22 23:31:20  soeren
// made some changes to make it to compile with the intel compiler
//
// Revision 1.11  2001/08/21 22:22:40  sarang
// fixed errors; removed duplicate const; now OK with linux/gcc
//
// Revision 1.10  2001/08/13 20:05:28  tushar
// removed errors due to the new logger
//
// Revision 1.9  2001/08/13 00:24:27  ingmar
// GetMessageString() -> GetLogRecord().GetTestMessage()
//
// Revision 1.8  2001/08/07 17:02:44  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.7  2001/07/24 23:56:20  tushar
// fixed some compiler errors (logger)
//
// Revision 1.6  2001/07/16 23:29:48  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.5  2001/07/12 15:05:46  frank
// Comments.
//
// Revision 1.4  2001/07/06 19:43:42  frank
// Protected the stream from deletion if external.
//
// Revision 1.3  2001/07/02 16:28:02  frank
// Added equality test.
//
// Revision 1.2  2001/06/29 19:45:56  frank
// Removed dead code.
//
// Revision 1.1  2001/06/27 04:30:41  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/MemoryBuffer.C,v 1.3 2006/03/29 19:37:04 geconomos Exp $
// $Id: MemoryBuffer.C,v 1.3 2006/03/29 19:37:04 geconomos Exp $
