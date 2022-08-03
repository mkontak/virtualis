// $Id: VoxelCost.h,v 1.2 2005/04/27 17:37:17 geconomos Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: [Name] [e-mail]
//
// Complete History at bottom of file.


#pragma once
/**
 *  A node to store for each inside voxel.
 */
class VoxelCost 
{

// Helpful subclasses
public:
  class CmpCost {
  public:
    inline bool operator() (VoxelCost * a, VoxelCost * b) const;
  };
  typedef std::priority_queue<VoxelCost *, std::vector<VoxelCost *>, CmpCost> CostHeap;
  class CmpEuclidean {
  public:
    inline bool operator() (VoxelCost * a, VoxelCost * b) const;
  };
  typedef std::priority_queue<VoxelCost *, std::vector<VoxelCost *>, CmpEuclidean> EuclideanHeap;

  /// Default constructor.
  VoxelCost();

  /// Constructor for BlockVolume.
  VoxelCost(const uint4 & zeroForEmpty);

  /// Advanced Constructor.
  VoxelCost(const float4 & costOfVoxel, const float4 & piecewiseEuclideanDistance, const float4 & costToRoot, const Point<uint2> & location);

  /// Default deconstructor.
  ~VoxelCost();

  /// Equality operator.
  bool operator ==(const VoxelCost & other) const;

  /// UnEquality operator.
  bool operator !=(const VoxelCost & other) const;

  /// Returns the three data members concatenated in a single string object.
  std::string AsString() const;
  
  /// Assignment operator.
  inline VoxelCost & operator =(const int4 & other);

  /// just to make a block volume of these work
  operator int4()
  {
    return 5;
  }

// Data members
public:
  // cost of this voxel from DFB.
  float4 m_costOfVoxel;

  // distance from root voxel (either start or end).
  float4 m_piecewiseEuclideanDistance;

  // accumulated cost from root voxel (either start or end).
  float4 m_costToRoot;

  // the physical location of this voxel.
  Point<uint2> m_location;
}; // VoxelCost class


inline bool VoxelCost::CmpCost::operator () (VoxelCost * a, VoxelCost * b) const
{
  return(a->m_costToRoot > b->m_costToRoot);
} // operator ()


inline bool VoxelCost::CmpEuclidean::operator () (VoxelCost * a, VoxelCost * b) const
{
  return(a->m_piecewiseEuclideanDistance > b->m_piecewiseEuclideanDistance);
} // operator ()


inline bool VoxelCost::operator ==(const VoxelCost & other) const
{
  if (
    m_costOfVoxel == other.m_costOfVoxel &&
    m_piecewiseEuclideanDistance == other.m_piecewiseEuclideanDistance &&
    m_costToRoot == other.m_costToRoot &&
    m_location == other.m_location) {
    return true;
  } else {
    return false;
  }
}

inline bool VoxelCost::operator !=(const VoxelCost & other) const
{
  return !(*this == other);
}
inline VoxelCost & VoxelCost::operator =(const int4 & other)
{
  m_costOfVoxel                = 0;
  m_piecewiseEuclideanDistance = FLT_MAX;
  m_costToRoot                 = FLT_MAX;
  m_location                   = Point<uint2>(0,0,0);
  return * this;
}


/// Overloaded << Operator for VoxelCost.
inline std::ostream & operator <<(std::ostream & os, const VoxelCost & cost)
{
  os << "(" << cost.m_costOfVoxel << ",";
  os << cost.m_piecewiseEuclideanDistance << ",";
  os << cost.m_costToRoot << ",";
  os << cost.m_location << ")";
  return os;
}


// $Log: VoxelCost.h,v $
// Revision 1.2  2005/04/27 17:37:17  geconomos
// updated #pragmas
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.1  2002/03/06 01:24:53  ingmar
// added != operator, so that it compiles with the new Block Class
//
// Revision 3.0  2001/10/14 23:01:51  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:30   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:06   ingmar
// Initial revision.
// Revision 1.6  2001/06/03 17:02:32  dave
// removed some extra includes
//
// Revision 1.5  2001/04/08 22:05:28  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.4  2001/04/08 21:38:58  dave
// updated doc headers
//
// Revision 1.3  2001/04/06 18:20:36  dmitry
// updated datatypes
//
// Revision 1.2  2001/04/05 03:41:34  ingmar
// added documentation classes
//
// Revision 1.1  2001/01/26 17:17:27  dave
// changed Node.h to VoxelCost.h
//
// Revision 1.7  2001/01/26 04:21:37  dave
// removed extra constructor
//
// Revision 1.6  2000/11/13 12:59:20  frank
// Cleaned up code
//
// Revision 1.5  2000/10/31 13:53:33  frank
// Merged in new skeleton and segment structure
//
// Revision 1.4.10.1  2000/10/28 17:38:59  ingmar
// new ierators for new segments
//
// Revision 1.4.2.3  2000/10/19 14:28:26  frank
// changed short to ushort
//
// Revision 1.4.2.2  2000/10/17 18:05:49  frank
// Debugged skeleton grow
//
// Revision 1.4.2.1  2000/10/05 19:00:59  frank
// Revamped entire algorithm
//
// Revision 1.4  2000/09/27 23:29:43  ingmar
// merged TEASAR back into main trunc
//
// Revision 1.3.2.3  2000/09/27 22:21:07  ingmar
// teasar and Michael's algorithm now both work and peacefully coexist
//
// Revision 1.3.2.2  2000/09/27 13:01:37  ingmar
// m_costToRoot
//
// Revision 1.3.2.1  2000/09/27 02:34:12  ingmar
// m_piecewiseEuclideanDistance
//
// Revision 1.3  2000/09/26 01:17:50  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.2.4.1  2000/09/25 23:18:27  kevin
// shared -> static
//
// Revision 1.2  2000/09/12 17:58:18  ingmar
// moved initializations to constructor
//
// Revision 1.1.1.1  2000/08/11 22:15:55  ingmar
// unit tests complete
//
//
// 15    9/04/00 11:10p Ingmar
// now works with new naming sceme
//
// 14    8/28/00 2:30p Lihong
// include "Global.h" for disabling warnings of 4786 and 4251
//
// 13    8/21/00 5:04p Antonio
// updated header
//
// *****************  Version 12  *****************
// User: Michael      Date: 8/17/00    Time: 3:45p
// Updated in $/v2k/src/Skeleton
// add float distance from start
//
// *****************  Version 11  *****************
// User: Ingmar       Date: 8/15/00    Time: 6:41p
// Updated in $/v2k/src/Skeleton
// cleaned up debug prints and made Skeletonator use the ddb volume
//
// *****************  Version 10  *****************
// User: Antonio      Date: 8/11/00    Time: 3:30p
// Updated in $/v2k/src/Skeleton
// added header and history to end of file.
// $/v2k/src/Skeleton/Node.h
//
// **********************
// Label: last_void_*_volume_version
// User: Jay             Date:  8/11/00  Time:  1:19a
// Labeled 'last_void_*_volume_version'
// Label comment:
//   last void * volume version
// about to replace with template volume
//
// *****************  Version 9  *****************
// User: Mie             Date:  8/03/00  Time:  5:25p
// Checked in $/v2k/src/Skeleton
// Comment:
//   priority_queue & heap class for skeleton finding region growing
//
// *****************  Version 8  *****************
// User: Mie             Date:  8/02/00  Time:  1:29p
// Checked in $/v2k/src/Skeleton
// Comment:
//   ready to use stl priority_queue
//
// *****************  Version 7  *****************
// User: Mie             Date:  8/02/00  Time: 12:36p
// Checked in $/v2k/src/Skeleton
// Comment:
//   streamlined heap, removed heapPosition index variable
//
// *****************  Version 6  *****************
// User: Mie             Date:  8/01/00  Time: 10:29a
// Checked in $/v2k/src/Skeleton
//
// *****************  Version 5  *****************
// User: Mie             Date:  8/01/00  Time: 10:08a
// Checked in $/v2k/src/Skeleton
//
// *****************  Version 4  *****************
// User: Mie             Date:  7/31/00  Time:  3:21p
// Checked in $/v2k/src/Skeleton
// Comment:
//   added vx style comments
//
// *****************  Version 3  *****************
// User: Ingmar          Date:  7/24/00  Time:  9:25a
// Checked in $/v2k/src/Skeleton
// Comment:
//   code cleanup
//
// *****************  Version 2  *****************
// User: Ingmar          Date:  7/20/00  Time:  4:36p
// Checked in $/v2k/src/Skeleton
// Comment:
//   improved comments
//
// *****************  Version 1  *****************
// User: Michael         Date:  7/18/00  Time:  7:16p
// Created node.h
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/VoxelCost.h,v 1.2 2005/04/27 17:37:17 geconomos Exp $
// $Id: VoxelCost.h,v 1.2 2005/04/27 17:37:17 geconomos Exp $
