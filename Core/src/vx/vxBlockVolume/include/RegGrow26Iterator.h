// $Id: RegGrow26Iterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: 8-23-2000
// Author: Ingmar Bitter ingmar@viatronix.net


#ifndef RegGrow26Iterator_h
#define RegGrow26Iterator_h

// includes
#include "Decider.h"


/**
 * An iterator that linearizes 26-connected region growing.
 * $Id: RegGrow26Iterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a> 
*/
template <class Iter, class Vox, class Decider >
class RegGrow26Iterator
 {
public:

  /// Constructor called by clients
  RegGrow26Iterator(const Iter & dataIter, const Decider & decider);

  /// Increments the iterator position in growing direction.
  inline void operator ++();
  /// is true when the traversal is finished.
  inline bool End() const;

  /// Returns the value at the current location: Safe version.
  inline const Vox & GetVoxel() const;
  /// Returns the value at the current location: Unsafe version.
  inline const Vox & GetVoxelRaw() const;
  /// Sets the value at the current location: Safe version.
  inline void SetVoxel(const Vox & value);
  /// Sets the value at the current location: Unsafe version.
  inline void SetVoxelRaw(const Vox & value);

  /// Returns the current x,y,z position.
  inline const Point<uint2> & GetPos() const;
  /// Returns true if the iterator is inside the volume.
  inline bool InsideVolume() const;

  /// Test function to verify and demonstrate the class functionality.
  /// call as RegGrow26Iterator<int,int,int>::Test(cout).
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
  BlockVolume<bool>::VolIterator  m_flagIter;
  /// queue keeping track of discovered but not processed voxels
  std::queue< Point<uint2> > m_posQ;
  /// indicated state of region grow (ended or not)
  bool  m_bGrowEnded;
  /// temporary flag iterator used in ++() and PossiblyAddToQueue()
  BlockVolume<bool>::VolIterator m_peekFlagIter;
  /// temporary data iterator used in ++() and PossiblyAddToQueue()
  Iter m_peekDataIter;

}; // class RegGrow26Iterator


/// Overloaded << Operator.
template <class Iter, class Vox, class Decider >
inline std::ostream & operator<<(std::ostream & os, const RegGrow26Iterator<Iter, Vox, Decider> & iter);

#include "RegGrow26Iterator.inl"

#endif



// $Log: RegGrow26Iterator.h,v $
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
//    Rev 2.0   Sep 16 2001 23:48:18   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:52   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:44  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:20  soeren
// renamed volume  directory
//
// Revision 1.12  2001/05/15 13:42:43  ingmar
// now obsolete
//
// Revision 1.11  2001/04/08 22:04:17  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.10  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.9  2001/04/05 03:41:35  ingmar
// added documentation classes
//
// Revision 1.8  2001/04/03 05:46:59  soeren
// added documentation
//
// Revision 1.7  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.6  2000/11/16 17:19:49  dave
// added native BlockVolume VolIterator
//
// Revision 1.5  2000/11/03 17:02:26  dave
// converted back
//
// Revision 1.4  2000/10/31 22:35:28  ingmar
// added tests and operator <<
//
// Revision 1.3  2000/10/31 19:40:44  dave
// compiles in gcc
//
// Revision 1.2  2000/10/30 21:34:32  ingmar
// added dilate region iterator test (still needs debugging)
//
// Revision 1.1  2000/10/30 18:08:02  ingmar
// added 26 connected region grow iterator to volume module
//
//
// $Id: RegGrow26Iterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $
