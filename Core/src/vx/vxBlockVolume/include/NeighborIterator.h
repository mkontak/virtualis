// $Id: NeighborIterator.h,v 1.2 2007/04/09 17:03:28 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.net


#ifndef NeighborIterator_h
#define NeighborIterator_h

// includes
#include "Neighbor6Walker.h"
#include "Neighbor8Walker.h"
#include "Neighbor18Walker.h"
#include "Neighbor26Walker.h"

/**
 * An iterator that incrementally traverses the neighbors of a voxel.
 * $Id: NeighborIterator.h,v 1.2 2007/04/09 17:03:28 jmeade Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a>
*/
template <class Iter, class Vox, class NeighborWalker>
class NeighborIterator
{
public:

  /// CONSTRUCTOR & ASSIGNMENT

  /// Constructor called by clients
  NeighborIterator(const Iter & dataIter);

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
  /// gets raw value of the median voxel
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
  /// call as NeighborIterator<int,int,int>::Test(cout).
  static void Demo  (std::ostream & os = std::cout);
  static void Demo6 (std::ostream & os = std::cout);
  static void Demo18(std::ostream & os = std::cout);
  static void Demo26(std::ostream & os = std::cout);
  static bool Test  (std::ostream & os = std::cout);
  
  /// Test function to verify just one particular template instance combination
  /// this needs to work for any combinations of template arguments which breaks VC++, but are correct snd work in gcc
  static bool TestByTypeBlk(std::ostream & os = std::cout);

protected: /// data
  /// the index into the table of incremental difference vectors
  NeighborWalker m_neighborWalker;
}; // class NeighborIterator

/// Overloaded << Operator.
template <class Iter, class Vox, class NeighborWalker>
inline std::ostream & operator<<(std::ostream & os, const NeighborIterator<Iter, Vox, NeighborWalker> & iter);

/// helper iterator definitions
namespace it { 

  typedef  NeighborIterator< it::IterBlkVolBool,  bool  , Neighbor6Walker<   it::IterBlkVolBool> >  N6IterBlkVolBool;
  typedef  NeighborIterator< it::IterBlkVolUInt1, uint1 , Neighbor6Walker<   BlockVolume<uint1 >::VolIterator> >  N6IterBlkVolUInt1;
  typedef  NeighborIterator< it::IterBlkVolUInt2, uint2 , Neighbor6Walker<   BlockVolume<uint2 >::VolIterator> >  N6IterBlkVolUInt2;
  typedef  NeighborIterator< it::IterBlkVolUInt4, uint4 , Neighbor6Walker<   BlockVolume<uint4 >::VolIterator> >  N6IterBlkVolUInt4;
  typedef  NeighborIterator< it::IterBlkVolInt1,  int1  , Neighbor6Walker<   BlockVolume<int1  >::VolIterator> >  N6IterBlkVolInt1;
  typedef  NeighborIterator< it::IterBlkVolInt2,  int2  , Neighbor6Walker<   BlockVolume<int2  >::VolIterator> >  N6IterBlkVolInt2;
  typedef  NeighborIterator< it::IterBlkVolInt4,  int4  , Neighbor6Walker<   BlockVolume<int4  >::VolIterator> >  N6IterBlkVolInt4;
  typedef  NeighborIterator< it::IterBlkVolFloat4,float4, Neighbor6Walker<   BlockVolume<float4>::VolIterator> >  N6IterBlkVolFloat4;

  typedef  NeighborIterator< it::IterLinVolBool,  bool  , Neighbor6Walker<  LinearVolume<bool  >::VolIterator> >  N6IterLinVolBool;
  typedef  NeighborIterator< it::IterLinVolUInt1, uint1 , Neighbor6Walker<  LinearVolume<uint1 >::VolIterator> >  N6IterLinVolUInt1;
  typedef  NeighborIterator< it::IterLinVolUInt2, uint2 , Neighbor6Walker<  LinearVolume<uint2 >::VolIterator> >  N6IterLinVolUInt2;
  typedef  NeighborIterator< it::IterLinVolUInt4, uint4 , Neighbor6Walker<  LinearVolume<uint4 >::VolIterator> >  N6IterLinVolUInt4;
  typedef  NeighborIterator< it::IterLinVolInt1,  int1  , Neighbor6Walker<  LinearVolume<int1  >::VolIterator> >  N6IterLinVolInt1;
  typedef  NeighborIterator< it::IterLinVolInt2,  int2  , Neighbor6Walker<  LinearVolume<int2  >::VolIterator> >  N6IterLinVolInt2;
  typedef  NeighborIterator< it::IterLinVolInt4,  int4  , Neighbor6Walker<  LinearVolume<int4  >::VolIterator> >  N6IterLinVolInt4;
  typedef  NeighborIterator< it::IterLinVolFloat4,float4, Neighbor6Walker<  LinearVolume<float4>::VolIterator> >  N6IterLinVolFloat4;


  typedef  NeighborIterator< it::IterBlkVolBool,  bool  , Neighbor18Walker<   BlockVolume<bool  >::VolIterator> >  N18IterBlkVolBool;
  typedef  NeighborIterator< it::IterBlkVolUInt1, uint1 , Neighbor18Walker<   BlockVolume<uint1 >::VolIterator> >  N18IterBlkVolUInt1;
  typedef  NeighborIterator< it::IterBlkVolUInt2, uint2 , Neighbor18Walker<   BlockVolume<uint2 >::VolIterator> >  N18IterBlkVolUInt2;
  typedef  NeighborIterator< it::IterBlkVolUInt4, uint4 , Neighbor18Walker<   BlockVolume<uint4 >::VolIterator> >  N18IterBlkVolUInt4;
  typedef  NeighborIterator< it::IterBlkVolInt1,  int1  , Neighbor18Walker<   BlockVolume<int1  >::VolIterator> >  N18IterBlkVolInt1;
  typedef  NeighborIterator< it::IterBlkVolInt2,  int2  , Neighbor18Walker<   BlockVolume<int2  >::VolIterator> >  N18IterBlkVolInt2;
  typedef  NeighborIterator< it::IterBlkVolInt4,  int4  , Neighbor18Walker<   BlockVolume<int4  >::VolIterator> >  N18IterBlkVolInt4;
  typedef  NeighborIterator< it::IterBlkVolFloat4,float4, Neighbor18Walker<   BlockVolume<float4>::VolIterator> >  N18IterBlkVolFloat4;

  typedef  NeighborIterator< it::IterLinVolBool,  bool  , Neighbor18Walker<  LinearVolume<bool  >::VolIterator> >  N18IterLinVolBool;
  typedef  NeighborIterator< it::IterLinVolUInt1, uint1 , Neighbor18Walker<  LinearVolume<uint1 >::VolIterator> >  N18IterLinVolUInt1;
  typedef  NeighborIterator< it::IterLinVolUInt2, uint2 , Neighbor18Walker<  LinearVolume<uint2 >::VolIterator> >  N18IterLinVolUInt2;
  typedef  NeighborIterator< it::IterLinVolUInt4, uint4 , Neighbor18Walker<  LinearVolume<uint4 >::VolIterator> >  N18IterLinVolUInt4;
  typedef  NeighborIterator< it::IterLinVolInt1,  int1  , Neighbor18Walker<  LinearVolume<int1  >::VolIterator> >  N18IterLinVolInt1;
  typedef  NeighborIterator< it::IterLinVolInt2,  int2  , Neighbor18Walker<  LinearVolume<int2  >::VolIterator> >  N18IterLinVolInt2;
  typedef  NeighborIterator< it::IterLinVolInt4,  int4  , Neighbor18Walker<  LinearVolume<int4  >::VolIterator> >  N18IterLinVolInt4;
  typedef  NeighborIterator< it::IterLinVolFloat4,float4, Neighbor18Walker<  LinearVolume<float4>::VolIterator> >  N18IterLinVolFloat4;


  typedef  NeighborIterator< it::IterBlkVolBool,  bool  , Neighbor26Walker<   BlockVolume<bool  >::VolIterator> >  N26IterBlkVolBool;
  typedef  NeighborIterator< it::IterBlkVolUInt1, uint1 , Neighbor26Walker<   BlockVolume<uint1 >::VolIterator> >  N26IterBlkVolUInt1;
  typedef  NeighborIterator< it::IterBlkVolUInt2, uint2 , Neighbor26Walker<   BlockVolume<uint2 >::VolIterator> >  N26IterBlkVolUInt2;
  typedef  NeighborIterator< it::IterBlkVolUInt4, uint4 , Neighbor26Walker<   BlockVolume<uint4 >::VolIterator> >  N26IterBlkVolUInt4;
  typedef  NeighborIterator< it::IterBlkVolInt1,  int1  , Neighbor26Walker<   BlockVolume<int1  >::VolIterator> >  N26IterBlkVolInt1;
  typedef  NeighborIterator< it::IterBlkVolInt2,  int2  , Neighbor26Walker<   BlockVolume<int2  >::VolIterator> >  N26IterBlkVolInt2;
  typedef  NeighborIterator< it::IterBlkVolInt4,  int4  , Neighbor26Walker<   BlockVolume<int4  >::VolIterator> >  N26IterBlkVolInt4;
  typedef  NeighborIterator< it::IterBlkVolFloat4,float4, Neighbor26Walker<   BlockVolume<float4>::VolIterator> >  N26IterBlkVolFloat4;

  typedef  NeighborIterator< it::IterLinVolBool,  bool  , Neighbor26Walker<  LinearVolume<bool  >::VolIterator> >  N26IterLinVolBool;
  typedef  NeighborIterator< it::IterLinVolUInt1, uint1 , Neighbor26Walker<  LinearVolume<uint1 >::VolIterator> >  N26IterLinVolUInt1;
  typedef  NeighborIterator< it::IterLinVolUInt2, uint2 , Neighbor26Walker<  LinearVolume<uint2 >::VolIterator> >  N26IterLinVolUInt2;
  typedef  NeighborIterator< it::IterLinVolUInt4, uint4 , Neighbor26Walker<  LinearVolume<uint1 >::VolIterator> >  N26IterLinVolUInt4;
  typedef  NeighborIterator< it::IterLinVolInt1,  int1  , Neighbor26Walker<  LinearVolume<int1  >::VolIterator> >  N26IterLinVolInt1;
  typedef  NeighborIterator< it::IterLinVolInt2,  int2  , Neighbor26Walker<  LinearVolume<int2  >::VolIterator> >  N26IterLinVolInt2;
  typedef  NeighborIterator< it::IterLinVolInt4,  int4  , Neighbor26Walker<  LinearVolume<int4  >::VolIterator> >  N26IterLinVolInt4;
  typedef  NeighborIterator< it::IterLinVolFloat4,float4, Neighbor26Walker<  LinearVolume<float4>::VolIterator> >  N26IterLinVolFloat4;

  // 2D traversal
  typedef  NeighborIterator< it::IterBlkVolBool,  bool  , Neighbor8Walker<    BlockVolume<bool  >::VolIterator> >  N8IterBlkVolBool;
  typedef  NeighborIterator< it::IterBlkVolUInt1, uint1 , Neighbor8Walker<    BlockVolume<uint1 >::VolIterator> >  N8IterBlkVolUInt1;
  typedef  NeighborIterator< it::IterBlkVolUInt2, uint2 , Neighbor8Walker<    BlockVolume<uint2 >::VolIterator> >  N8IterBlkVolUInt2;
  typedef  NeighborIterator< it::IterBlkVolUInt4, uint4 , Neighbor8Walker<    BlockVolume<uint4 >::VolIterator> >  N8IterBlkVolUInt4;
  typedef  NeighborIterator< it::IterBlkVolInt1,  int1  , Neighbor8Walker<    BlockVolume<int1  >::VolIterator> >  N8IterBlkVolInt1;
  typedef  NeighborIterator< it::IterBlkVolInt2,  int2  , Neighbor8Walker<    BlockVolume<int2  >::VolIterator> >  N8IterBlkVolInt2;
  typedef  NeighborIterator< it::IterBlkVolInt4,  int4  , Neighbor8Walker<    BlockVolume<int4  >::VolIterator> >  N8IterBlkVolInt4;
  typedef  NeighborIterator< it::IterBlkVolFloat4,float4, Neighbor8Walker<    BlockVolume<float4>::VolIterator> >  N8IterBlkVolFloat4;
  
  typedef  NeighborIterator< it::IterLinVolBool,  bool  , Neighbor8Walker<   LinearVolume<bool  >::VolIterator> >  N8IterLinVolBool;
  typedef  NeighborIterator< it::IterLinVolUInt1, uint1 , Neighbor8Walker<   LinearVolume<uint1 >::VolIterator> >  N8IterLinVolUInt1;
  typedef  NeighborIterator< it::IterLinVolUInt2, uint2 , Neighbor8Walker<   LinearVolume<uint2 >::VolIterator> >  N8IterLinVolUInt2;
  typedef  NeighborIterator< it::IterLinVolUInt4, uint4 , Neighbor8Walker<   LinearVolume<uint4 >::VolIterator> >  N8IterLinVolUInt4;
  typedef  NeighborIterator< it::IterLinVolInt1,  int1  , Neighbor8Walker<   LinearVolume<int1  >::VolIterator> >  N8IterLinVolInt1;
  typedef  NeighborIterator< it::IterLinVolInt2,  int2  , Neighbor8Walker<   LinearVolume<int2  >::VolIterator> >  N8IterLinVolInt2;
  typedef  NeighborIterator< it::IterLinVolInt4,  int4  , Neighbor8Walker<   LinearVolume<int4  >::VolIterator> >  N8IterLinVolInt4;
  typedef  NeighborIterator< it::IterLinVolFloat4,float4, Neighbor8Walker<   LinearVolume<float4>::VolIterator> >  N8IterLinVolFloat4;

} // namespace it


#include "NeighborIterator.inl"

#endif // NeighborIterator_h



// $Log: NeighborIterator.h,v $
// Revision 1.2  2007/04/09 17:03:28  jmeade
// code review.
//
// Revision 1.1  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.3  2003/01/22 23:19:14  ingmar
// added missing includes
//
// Revision 3.2  2001/12/26 16:15:54  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.1  2001/12/24 17:56:21  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:23  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:48   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:44  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:20  soeren
// renamed volume  directory
//
// Revision 1.7  2001/08/16 19:22:53  jaddonisio
// New Neighbor8Walker for 2D processing.
//
// Revision 1.6  2001/08/15 22:34:16  jaddonisio
// Cosmetic change: alignment of code.
//
// Revision 1.5  2001/08/14 05:23:04  ingmar
// removed basic volIter from NeighborIter, it is now only in NeighborWalker
//
// Revision 1.4  2001/08/09 18:58:47  frank
// fixed typedefs of linear volume iterators - ingmar
//
// Revision 1.3  2001/05/22 22:31:42  lihong
// fixed typo in iterator typedefs
//
// Revision 1.2  2001/05/14 15:20:52  ingmar
// better testByType
//
// Revision 1.1  2001/05/01 13:54:43  ingmar
// reset version
//
// Revision 1.32  2001/04/30 16:05:34  ingmar
// beautify
//
// Revision 1.31  2001/04/30 14:57:45  ingmar
// beautify
//
// Revision 1.30  2001/04/30 14:47:50  ingmar
// beautify
//
// Revision 1.5 to 1.29 2001/04/28 01:23:21  ingmar
// code beautify
//
// Revision 1.4  2001/04/19 15:11:30  ingmar
// seperated Test and Demo
//
// Revision 1.3  2001/04/19 03:01:02  ingmar
// made namespace it hirerarchical
//
// Revision 1.2  2001/04/18 03:02:43  ingmar
// updated header info
//
// Revision 1.1  2001/04/14 04:36:22  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/NeighborIterator.h,v 1.2 2007/04/09 17:03:28 jmeade Exp $
// $Id: NeighborIterator.h,v 1.2 2007/04/09 17:03:28 jmeade Exp $
