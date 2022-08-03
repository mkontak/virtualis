// $Id: Neighbor18Iterator.h,v 1.2 2007/04/09 20:24:23 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.net

#ifndef Neighbor18Iterator_h
#define Neighbor18Iterator_h


/**
 * An iterator that incrementally traverses the 18 neighbors of a voxel.
 * $Id: Neighbor18Iterator.h,v 1.2 2007/04/09 20:24:23 jmeade Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a>
*/
template <class Iter, class Vox>
class Neighbor18Iterator
{
public:

  /// CONSTRUCTOR & ASSIGNMENT

  /// Constructor called by clients
  Neighbor18Iterator(const Iter & dataIter);

  /// VOLUME TRAVERSAL

  /// Increments the iterator along all 18 neighbors of a voxel.
  inline void operator ++();
  /// is true when the traversal is finished.
  inline bool End() const;

  /// SET & GET VOXELS

  /// Returns the value at the current location: Safe version.
  inline const Vox & GetVoxel() const;
  /// Returns the value at the current location: Unsafe version.
  inline const Vox & GetVoxelRaw() const;
  /// Sets the value at the current location: Safe version.
  inline void SetVoxel(const Vox & value);
  /// Sets the value at the current location: Unsafe version.
  inline void SetVoxelRaw(const Vox & value);
  /// get raw value of median voxel
  inline Vox GetMedianVoxelRaw() const;


  /// return a reference to the underlying VolIterator
  inline const Iter & GetDataVolIter() const;

  /// Centers the iterator at the location.
  inline void CenterAt(const Point<uint2> & pos);

  /// STATUS REPORTS

  /// Returns the current x,y,z position.
  inline const Point<uint2> & GetPos() const;
  /// Returns true if the iterator is inside the volume.
  inline bool InsideVolume() const;


  /// VERIFICATION

  /// Test function to verify and demonstrate the class functionality.
  /// call as Neighbor18Iterator<int,int,int>::Test(cout).
  static bool Test (std::ostream & os = std::cout);

#ifdef _gcc
  /// Test function to verify just one particular template instance combination
  /// this needs to work for any combinations of template arguments which breaks VC++, but are correct snd work in gcc
  template < class Vol >
  static bool TestByType(std::ostream & os = std::cout, Vol dummy = 0);
#endif // _gcc

protected: /// data
  /// the low level volume data iterator
  Iter  m_dataIter;
  /// the index into the table of incremental difference vectors
  uint1 m_uNeighborIndex;
}; // class Neighbor18Iterator


/// Overloaded << Operator.
template <class Iter, class Vox>
inline std::ostream & operator<<(std::ostream & os, const Neighbor18Iterator<Iter, Vox> & iter);

/// iterator typedefs
typedef  Neighbor18Iterator<BlockVolume<bool>   ::VolIterator, bool  >  BlockVolN18IterB;
typedef  Neighbor18Iterator<BlockVolume<uint1>  ::VolIterator, uint1 >  BlockVolN18IterUC;
typedef  Neighbor18Iterator<BlockVolume<uint2>  ::VolIterator, uint2 >  BlockVolN18IterUS;
typedef  Neighbor18Iterator<BlockVolume<uint4>  ::VolIterator, uint4 >  BlockVolN18IterUI;
typedef  Neighbor18Iterator<BlockVolume<int1>   ::VolIterator, int1  >  BlockVolN18IterC;
typedef  Neighbor18Iterator<BlockVolume<int2>   ::VolIterator, int2  >  BlockVolN18IterS;
typedef  Neighbor18Iterator<BlockVolume<int4>   ::VolIterator, int4  >  BlockVolN18IterI;
typedef  Neighbor18Iterator<BlockVolume<float4> ::VolIterator, float4>  BlockVolN18IterF;

typedef  Neighbor18Iterator<LinearVolume<bool>  ::VolIterator, bool  >  LinVolN18IterB;
typedef  Neighbor18Iterator<LinearVolume<uint1> ::VolIterator, uint1 >  LinVolN18IterUC;
typedef  Neighbor18Iterator<LinearVolume<uint2> ::VolIterator, uint2 >  LinVolN18IterUS;
typedef  Neighbor18Iterator<LinearVolume<uint4> ::VolIterator, uint4 >  LinVolN18IterUI;
typedef  Neighbor18Iterator<LinearVolume<int1>  ::VolIterator, int1  >  LinVolN18IterC;
typedef  Neighbor18Iterator<LinearVolume<int2>  ::VolIterator, int2  >  LinVolN18IterS;
typedef  Neighbor18Iterator<LinearVolume<int4>  ::VolIterator, int4  >  LinVolN18IterI;
typedef  Neighbor18Iterator<LinearVolume<float4>::VolIterator, float4>  LinVolN18IterF;

#include "Neighbor18Iterator.inl"

#endif



// $Log: Neighbor18Iterator.h,v $
// Revision 1.2  2007/04/09 20:24:23  jmeade
// code review.
//
// Revision 1.1  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.2  2002/04/04 22:40:17  ingmar
// removed obsolete recursive block volume related classes
//
// Revision 3.1  2001/12/24 17:56:21  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:22  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:36   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:45  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:19  soeren
// renamed volume  directory
//
// Revision 1.9  2001/05/15 13:43:25  ingmar
// now obsolete
//
// Revision 1.8  2001/04/18 03:02:43  ingmar
// updated header info
//
// Revision 1.7  2001/04/08 22:04:17  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.6  2001/04/08 21:38:21  dave
// updated doc headers
//
// Revision 1.5  2001/04/05 03:41:35  ingmar
// added documentation classes
//
// Revision 1.4  2001/04/03 05:46:59  soeren
// added documentation
//
// Revision 1.3  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.2  2000/11/22 19:34:45  dave
// added Median for Sarang
//
// Revision 1.1  2000/11/21 02:09:44  ingmar
// first version of Neighbor18Iterator
//
//
// $Id: Neighbor18Iterator.h,v 1.2 2007/04/09 20:24:23 jmeade Exp $
