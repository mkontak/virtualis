// $Id: VxVolIterator.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created:  summer 00
// Author: David & Jayanta  dave@viatronix.com
//
// Complete History at bottom of file.


#ifndef VxVolIterator_h
#define VxVolIterator_h


/**
 * Generic volume iterator implementation that can be used for any volume layout.
 * Possibly not the most efficient implementation for a particular layout.
 * Templatized for any Layout L or Type T.
 * $Id: VxVolIterator.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a> 
 */
template <class L, class T>
class VX_BLOCK_VOLUME_DLL VxVolIterator
 {
public:
  friend class hdr::VolAPI<uint1>;
  friend hdr::VolAPI<T>;

  /// CONSTRUCTOR & ASSIGNMENT
  /// Constructor called by Vol3DIterator, not by clients.
  inline VxVolIterator(L & container, const Point<uint2> & point);

  /// COMPLETE VOLUME TRAVERSAL
  /// Moves the position to the next non-background voxel in a non-specified order.
  inline void NextNonZero();

  /// Returns the next voxel in an order optimized for a particular layout
  inline void operator++();

  /// Returns the next voxel according to a ZYX traversal.
  inline void NextZYX();

  /// Traversal has reached end.
  inline bool End();

  /// SET & GET VOXELS
  /// Sets the value at the current location.
  inline void SetVoxel(const T & value);
  /// Sets the value at the current location.
  inline void SetVoxelRaw(const T & value);
  /// Returns the value at the current location: Safe version.
  inline const T & GetVoxel() const;
  /// Returns the value at the current location: Unsafe version.
  inline const T & GetVoxelRaw() const;

  /// deprecated operator * , remains in API to support STL algorithms - use GetVoxel instead.
  inline const T & operator*() const;

  /// COMPARATORS
  /// iterator inequality test by location & size.
  inline bool operator!=(const VxVolIterator & rhs);
  /// Iterator equality test by location & size.
  inline bool operator==(const VxVolIterator & rhs);

  /// ABSOLUTE LOCATION FUNCTIONS
  /// Peeks at the absolute location.
  // PeekAbs() -> Not implemented, just call GetVoxel() or GetVoxelRaw()
  /// Moves the iterator to the given absolute position.
  inline void MoveAbs(const Point<uint2> & pos);

  /// Peeks at the value at the location given relative to current location.
  inline const T & PeekRel(const Vector<int2> & delta) const;
  /// Moves the iterator relative to its current position.
  inline void MoveRel(const Vector<int2> & delta);

  /// INCREMENTAL PEEK AT SOME VOXEL
  /// Peeks at the next voxel in the x direction w/out moving the iterator.
  inline const T & PeekNextX(const uint2 & delta = 1) const;
  /// Peeks at the next voxel in the y direction w/out moving the iterator.
  inline const T & PeekNextY(const uint2 & delta = 1) const;
  /// Peeks at the next voxel in the z direction w/out moving the iterator.
  inline const T & PeekNextZ(const uint2 & delta = 1) const;

  /// Peeks at the prev voxel in the x direction w/out moving the iterator.
  inline const T & PeekPrevX(const uint2 & delta = 1) const;
  /// Peeks at the prev voxel in the y direction w/out moving the iterator.
  inline const T & PeekPrevY(const uint2 & delta = 1) const;
  /// Peeks at the prev voxel in the z direction w/out moving the iterator.
  inline const T & PeekPrevZ(const uint2 & delta = 1) const;

  /// INCREMENTAL WALK TO NEAR BY VOXELS
  /// Increments the iterator location in the x direction.
  inline void IncX(const uint2 & delta = 1);
  /// Increments the iterator location in the y direction.
  inline void IncY(const uint2 & delta = 1);
  /// Increments the iterator location in the z direction.
  inline void IncZ(const uint2 & delta = 1);

  /// Decrements the iterator location in the x direction.
  inline void DecX(const uint2 & delta = 1);
  /// Decrements the iterator location in the y direction.
  inline void DecY(const uint2 & delta = 1);
  /// Decrements the iterator location in the z direction.
  inline void DecZ(const uint2 & delta = 1);

  /// STATUS REPORTS
  /// Returns true if the iterator is inside the volume.
  inline bool InsideVolume() const;
  /// Returns the current x,y,z position.
  inline const Point<uint2> & GetPos() const;
  /// Get Volume Dimension included in API for Convenience for other iterators.
  inline const Triple<uint4> & GetDim() const;

protected:  //data
  L & m_vol;          // reference to container volume
  Point<uint2> m_pos; // current position
  bool m_inside;      // if currently inside the volume
}; // VxVolIterator

/// Overloaded << Operator.
template <class Layout, class Vox>
std::ostream & operator<<(std::ostream & os, const VxVolIterator<Layout, Vox> & iter);

#include "VxVolIterator.inl"
#endif

// $Log: VxVolIterator.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.3  2003/05/16 11:48:13  michael
// code review
//
// Revision 3.2  2001/12/26 16:15:55  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.1  2001/12/24 17:49:46  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:24  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:26   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:02   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:43  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:22  soeren
// renamed volume  directory
//
// Revision 1.20  2001/04/09 16:25:35  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.19  2001/04/08 22:04:16  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.18  2001/04/08 21:38:23  dave
// updated doc headers
//
// Revision 1.17  2001/04/05 03:41:35  ingmar
// added documentation classes
//
// Revision 1.16  2001/04/03 05:46:59  soeren
// added documentation
//
// Revision 1.15  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.14  2000/11/16 17:19:49  dave
// added native BlockVolume VolIterator
//
// Revision 1.13  2000/11/10 17:34:06  dave
// impl. API changes for VolIterator
//
// Revision 1.12  2000/11/04 03:08:56  ingmar
// Streamlined VolIterator
//
// Revision 1.11  2000/11/03 20:34:02  ingmar
// moved volume dimension back to unit and made positions inside a volume ushort
//
// Revision 1.10  2000/11/03 18:20:15  ingmar
// moved volume dimension back to unit and made positions inside a volume ushort
//
// Revision 1.9  2000/11/03 17:02:27  dave
// converted back
//
// Revision 1.8  2000/11/02 19:48:10  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.7  2000/10/31 22:35:28  ingmar
// added tests and operator <<
//
// Revision 1.6  2000/10/24 02:56:47  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.5  2000/10/23 20:18:08  ingmar
// removed VxDilator and added SpiralIterator
//
// Revision 1.4  2000/10/22 23:48:21  dave
// added RecBlockVolume
//
// Revision 1.3  2000/10/22 00:24:24  dave
// better iterators
//
// Revision 1.2  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.1.1.1.2.1  2000/09/25 18:56:37  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:52  ingmar
// unit tests complete
//
//
// 13    9/08/00 11:29a Ingmar
// code beautify
//
// 12    9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
//
// 11    8/25/00 5:18p Jay
// towards a wroking version of a region grow iterator
//
// 10    8/24/00 5:00p Ingmar
// region grow now with generic template decider
//
// 9     8/24/00 3:13p Ingmar
// now first procedual end condition working
//
// 8     8/24/00 12:48a Ingmar
// started to change m_location from 3uint to 3int
//
// 7     8/23/00 3:23p Ingmar
// made data mempers protected
//
// 6     8/23/00 3:09p Ingmar
// fixed wrong const functions
//
// 5     8/21/00 2:12a Sarang
// added header and footer
//
//*****************  Version 4  *****************
//User: Sarang          Date:  8/20/00  Time:  6:49p
//Checked in $/v2k/src/Volume
//Comment:
//  The whole pipeline now works under linux+gcc. Removed tons of bugs
//and syntax errors found by gcc.
//
//*****************  Version 3  *****************
//User: Jay             Date:  8/09/00  Time: 11:54p
//Checked in $/TemplateVol/src/Volume
//Comment:
//  Template Volumes working.
//
//*****************  Version 2  *****************
//User: Jay             Date:  8/09/00  Time: 10:41p
//Checked in $/TemplateVol/src/Volume
//Comment:
//  added iterator template classes
//
//*****************  Version 1  *****************
//User: Jay             Date:  8/09/00  Time:  3:56p
//Created vxVolIterator.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/VxVolIterator.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: VxVolIterator.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
