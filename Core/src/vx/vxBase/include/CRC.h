// $Id: CRC.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.com)

#ifndef CRC_h
#define CRC_h


/**
 * Cyclic Redundancy Check
 */
class VX_BASE_DLL CRC 
{
// Member Functions
public:

  /// Constructor.
  CRC();

  /// Compute the checksum.
  uint4 ComputeChecksum( const std::string & sChunkType, const uint1 * pBuffer, const uint4 uBufferSize );

  /// Compute the complement.
  unsigned long CrcComplement( const unsigned long uComplement );

  /// Initialize the data structure.
  unsigned long CrcInit();

  /// Update the checksum.
  unsigned long UpdateCrc( const unsigned long uCrc, const unsigned char * puBuf, const int iLength );

private:

  /// Copy constructor.
  CRC( const CRC & other );

  /// Assignment operator.
  CRC & operator =( const CRC & other );

  /// Equality operator.
  bool operator ==( const CRC & other );

// Data Members
private:
  
  // Table of CRCs of all 8-bit messages.
  unsigned long m_vTable[256];
  
}; // class CRC


#endif // CRC_h


// $Log: CRC.h,v $
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
// Revision 3.3  2002/05/07 18:25:13  frank
// Added comments.
//
// Revision 3.2  2002/04/26 12:38:55  soeren
// added checksum test for reading and writing vxBlockVolumes
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
//    Rev 2.0   Sep 16 2001 23:47:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:32   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:44  soeren
// renamed image directory
//
// Revision 1.4  2001/08/21 22:07:57  sarang
// added #include<string>.. now OK with linux/gcc
//
// Revision 1.3  2001/06/24 21:01:20  frank
// Used actual CRC algorithm for PNG files.
//
// Revision 1.2  2001/06/19 16:29:52  frank
// Integrated V3DFile class into system.
//
// Revision 1.1  2001/06/19 15:53:06  frank
// Initial version.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/CRC.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: CRC.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
