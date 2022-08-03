// $Id: MemoryBuffer.h,v 1.2 2004/03/30 21:50:34 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


#ifndef MemoryBuffer_h
#define MemoryBuffer_h

// includes
#include "vxInflate.h"


/**
 * A memory buffer that can be passed around as a structure.  The memory can
 * be expanded and contracted as needed (like a vector) or set from an external
 * pointer and fixed in size.
 */
class VX_BASE_DLL MemoryBuffer 
{
// Member Functions
public:

  /// Constructor
  MemoryBuffer( const uint4 & uInitialSize=0 );

  /// Copy constructor
  MemoryBuffer( const MemoryBuffer & other );
  
  /// Constructor from an external buffer
  MemoryBuffer( uint1 * const pBuffer, const uint4 & uBufferSize );
  
  /// Destructor
  ~MemoryBuffer();

  /// Assignment operator
  MemoryBuffer & operator =( const MemoryBuffer & other );

  /// Set the current size keeping whatever data is possible
  void Resize( const uint4 & uNewSize );

  /// Ensure there is enough capacity keeping whatever data is possible
  void Reserve( const uint4 & uNewCapacity );
  
  /// Return a pointer to the memory
  uint1 * GetDataPtr();

  /// Access into the array
  uint1 & operator []( const uint4 uIndex ) const { return m_pBuffer[uIndex]; }

  /// Fill the buffer with a specificed value
  void Clear( const uint1 uClearValue );

  /// Retrieve the number of active bytes in the array
  const uint4 & GetSize() const;

  /// Retrieves the maximum possible number of bytes before reallocation occurs
  const uint4 & GetCapacity() const;

  /// note: must reserve sufficient space in the destination buffer before calling
  void InflateInto( const hdr::FileCompressionEnum eCompression, MemoryBuffer & destinationBuffer );
  
  /// note: must reserve sufficient space in the destination buffer before calling
  void DeflateInto( const hdr::FileCompressionEnum eCompression, MemoryBuffer & destinationBuffer );

  /// Copy from an external buffer into this
  void CopyIn( const uint1 * pBuffer, const uint4 uSize );
  
  /// Copy from this to an external buffer (make sure there is enough space allocated externally)
  void CopyOut( uint1 * pBuffer, const uint4 uSize ) const;

  /// Write out to a stream
  void Write( std::ofstream & os ) const;

  /// Equality operator
  bool operator ==( const MemoryBuffer & other ) const;
  
  /// Inequality operator
  bool operator !=( const MemoryBuffer & other ) const;
  
  /// Encode into a base 16-encoded string of characters
  MemoryBuffer GetEncodedBase16() const;

  /// Decode from a base16-encoded string of characters
  MemoryBuffer GetDecodedBase16() const;
  
  /// Convert to a string
  std::string ToString() const;

  /// Convert from a string
  static MemoryBuffer FromString( const std::string & sInput );

// Data Members
private:

  /// The pointer to the memory buffer
  uint1 * m_pBuffer;
  
  /// The current size of the active portion of the buffer
  uint4 m_uSize;

  /// The capacity of the buffer which is always at least as big as the size
  uint4 m_uCapacity;

  /// If this buffer is external and therefore protected from reallocation and deletion
  bool m_bProtected;
  
}; // class MemoryBuffer


#endif // MemoryBuffer_h


// $Log: MemoryBuffer.h,v $
// Revision 1.2  2004/03/30 21:50:34  frank
// added base 16 encoding
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
// Revision 3.3  2002/08/14 16:12:09  wenli
// initial version - Vascular
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
//    Rev 2.0   Sep 16 2001 23:47:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:40   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:45  soeren
// renamed image directory
//
// Revision 1.7  2001/08/21 22:22:40  sarang
// fixed errors; removed duplicate const; now OK with linux/gcc
//
// Revision 1.6  2001/07/12 15:05:46  frank
// Comments.
//
// Revision 1.5  2001/07/06 19:43:32  frank
// Protected the stream from deletion if external.
//
// Revision 1.4  2001/07/02 16:28:02  frank
// Added equality test.
//
// Revision 1.3  2001/06/29 19:45:56  frank
// Removed dead code.
//
// Revision 1.2  2001/06/27 04:30:58  frank
// Moved code to implementation file.
//
// Revision 1.1  2001/06/27 03:14:19  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/MemoryBuffer.h,v 1.2 2004/03/30 21:50:34 frank Exp $
// $Id: MemoryBuffer.h,v 1.2 2004/03/30 21:50:34 frank Exp $
