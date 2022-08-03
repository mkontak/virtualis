// $Id: RegGrow6Iterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: 8-23-2000
// Author: Ingmar Bitter ingmar@viatronix.net


#ifndef RegGrow6Iterator_h
#define RegGrow6Iterator_h

// includes
#include "Decider.h"
#include "Neighbor6Iterator.h"


/**
 * An iterator that linearizes 6-connected region growing.
 * $Id: RegGrow6Iterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a> 
*/
template <class Iter, class Vox, class Decider >
class RegGrow6Iterator
{
public:

  /// CONSTRUCTOR & ASSIGNMENT

  /// Constructor called by clients
  RegGrow6Iterator(const Iter & dataIter, Decider decider);

  /// VOLUME TRAVERSAL

  /// Increments the iterator position in growing direction.
  inline void operator ++();
  /// is true when the traversal is finished.
  inline bool End() const;

  /// GET & SET VOXELS

  /// Returns the value at the current location: Safe version.
  inline const Vox & GetVoxel() const;
  /// Returns the value at the current location: Unsafe version.
  inline const Vox & GetVoxelRaw() const;
  /// Sets the value at the current location: Safe version.
  inline void SetVoxel(const Vox & value);
  /// Sets the value at the current location: Unsafe version.
  inline void SetVoxelRaw(const Vox & value);

  /// STATUS REPORTS

  /// Returns the current x,y,z position.
  inline const Point<uint2> & GetPos() const;
  /// Returns true if the iterator is inside the volume.
  inline bool InsideVolume() const;


  /// VERIFICATION

  /// Test function to verify and demonstrate the class functionality.
  /// call as RegGrow6Iterator<int,int,int>::Test(cout).
  static bool Test (std::ostream & os = std::cout);
  /// Test function to verify just one particular template instance
  static bool TestByType(std::ostream & os = std::cout);

protected:
  /// what to do after moving the iterators onto a potentially next region voxel
  inline  void PossiblyAddToQueue();

protected: // data
  /// data iterator
  Iter  m_dataIter;
  /// decides whether to include a voxel
  Decider  m_decider;
  /// bitvolume flagging already processed voxels
  BlockVolB  m_flagVol;
  /// iterator for the flag volume
  BlockVolIterB  m_flagIter;
  /// queue keeping track of discovered but not processed voxels
  std::queue< Point<uint2> > m_posQ;
  /// indicated state of region grow (ended or not)
  bool  m_bGrowEnded;
  /// temporary flag iterator used in ++() and PossiblyAddToQueue()
  BlockVolN6IterB m_peekFlagIter;
  /// temporary data iterator used in ++() and PossiblyAddToQueue()
  Neighbor6Iterator<Iter, Vox>    m_peekDataIter;

}; // class RegGrow6Iterator


/// Overloaded << Operator.
template <class Iter, class Vox, class Decider >
inline std::ostream & operator<<(std::ostream & os, const RegGrow6Iterator<Iter, Vox, Decider> & iter);

// typedef is more difficult due to decider, may need macro ??


#include "RegGrow6Iterator.inl"

#endif



// $Log: RegGrow6Iterator.h,v $
// Revision 1.3  2007/04/10 01:23:32  jmeade
// code review.
//
// Revision 1.2  2007/03/06 18:58:23  dongqing
// code review
//
// Revision 1.1  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.1  2001/12/24 17:51:15  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:23  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:20   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:54   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:44  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.23  2001/05/15 13:42:34  ingmar
// now obsolete
//
// Revision 1.22  2001/04/08 22:04:17  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.21  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.20  2001/04/05 03:41:35  ingmar
// added documentation classes
//
// Revision 1.19  2001/04/03 05:46:59  soeren
// added documentation
//
// Revision 1.18  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.17  2000/11/16 17:19:49  dave
// added native BlockVolume VolIterator
//
// Revision 1.16  2000/11/15 03:09:49  ingmar
// based the region growing on neighbor6Iterator
//
// Revision 1.15  2000/11/07 04:47:04  dave
// didn't add typedefs
//
// Revision 1.14  2000/11/04 20:55:41  ingmar
// code beautify
//
// Revision 1.13  2000/11/03 17:02:26  dave
// converted back
//
// Revision 1.12  2000/11/01 17:18:33  ingmar
// added operator <<
//
// Revision 1.11  2000/10/31 17:16:02  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.10  2000/10/30 18:08:02  ingmar
// added 26 connected region grow iterator to volume module
//
// Revision 1.9  2000/10/26 17:06:27  ingmar
// code cleanup and better documentaion
//
// Revision 1.8  2000/10/26 03:19:41  ingmar
// code cleanup and addition of test function skeleton
//
// Revision 1.7  2000/10/25 01:35:25  ingmar
// removed VxIterator and improved VxVolIterator and RegGrow6Iterator
//
// Revision 1.6  2000/10/23 20:18:08  ingmar
// removed VxDilator and added SpiralIterator
//
// Revision 1.5  2000/10/22 00:24:23  dave
// better iterators
//
// Revision 1.4  2000/10/19 19:59:18  ingmar
// moved Global.h up to get rid of VC++ compile warnings
//
// Revision 1.3  2000/10/19 04:01:04  ingmar
// fixed 6-connected region grow and related VolIterator bugs
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
// 6     9/08/00 11:29a Ingmar
// code beautify
//
// 5     8/25/00 5:18p Jay
// towards a wroking version of a region grow iterator
//
// 4     8/24/00 5:00p Ingmar
// region grow now with generic template decider
//
// 3     8/24/00 3:13p Ingmar
// now first procedual end condition working
//
// 2     8/24/00 12:53a Ingmar
// new 6 connected region grow iterator
//
// 1     8/23/00 2:28p Ingmar
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/RegGrow6Iterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $
// $Id: RegGrow6Iterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $
