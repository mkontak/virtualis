// $Id: Jitter.h,v 1.2 2004/10/20 21:50:23 michael Exp $
//
// Copyright© 2000,2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.com)

#ifndef Jitter_h
#define Jitter_h

/** 
 * This class precomputes a square set of ray offsets
 * used to jitter the sampling along rays during volume
 * rendering.  A fast lookup per pixel allows you to
 * adjust the initial sample position for each ray
 * independently.
 */
class VX_VIEWER_DLL Jitter
{

// Member functions
public:

  /// Constructor.
  Jitter();

  /// Retrive a jitter value for a pixel.
  inline float4 GetValue(const uint4 uPositionX, const uint4 uPositionY) const;
    
  /// Call this once per frame to randomize the values.
  inline void SwapBuffers();

protected:
private:

// Data members
public:
protected:
private:

  // some constants
  enum {
    /// The size of the array of jitter values.
    m_eSize = 32,
    
    /// The number of bits to shift to compute the offset into the array.
    m_eBitShift = 5,
    
    /// The lower bits to use to index into the array.
    m_eBitMask = 31,
    
    /// The number of times bigger than the t.
    m_eNumBuffers = 3
  };

  /// A pointer to all the jitter values.
  float4 m_vfAllBuffers[m_eSize*m_eSize*m_eNumBuffers];
  
  ///  A pointer to the current jitter values for this frame.
  float4 * m_pfCurrentBuffer;
};

#include "Jitter.inl"

#endif // ifndef Jitter_h


// $Log: Jitter.h,v $
// Revision 1.2  2004/10/20 21:50:23  michael
// added VX_VIEWER_DLL to export classes
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/05/16 11:45:25  michael
// code review
//
// Revision 1.1  2002/07/17 15:30:52  ingmar
// moved Jitter class from v3DColon/Readlib to 75_renderUtils
//
// Revision 3.1  2001/12/28 19:51:54  ingmar
// moved Sec_ReaderLib to ReaderLib.h
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:00   ingmar
// Initial revision.
// Revision 1.4  2001/06/26 16:21:28  ana
// Results of code review for Fields and Jitter classes
//
// Revision 1.3  2001/04/10 13:13:39  frank
// Code walkthrough results.
//
// Revision 1.2  2001/03/19 12:18:43  frank
// Updated to reflect new coding standards.
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
// 
// 4     8/28/00 12:21p Antonio
// updated header and footer
//
//*****************  Version 3  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 3
//Comment:
//  
//
//*****************  Version 2  *****************
//User: Frank           Date:  7/13/00  Time: 11:22p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Changed .i to .inl and .cpp to .C
//Removed precompiled headers
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created Jitter.h
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/Jitter.h,v 1.2 2004/10/20 21:50:23 michael Exp $
// $Id: Jitter.h,v 1.2 2004/10/20 21:50:23 michael Exp $
