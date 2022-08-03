// $Id: vxBlockVolumeIteratorNeighbors.h,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Rekha (ingmar|rekha)@viatronix.com

/**
 * Optimized Block Volume Neighbor Iterator.
 */

#ifndef vxBlockVolumeIteratorNeighbors_h
#define vxBlockVolumeIteratorNeighbors_h


// includes
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"


// class definition
template <class VoxelType>
class vxBlockVolumeIteratorNeighbors : vxBlockVolumeIteratorNeighborsConsts
{
// functions
public:

  /// constructor
  vxBlockVolumeIteratorNeighbors(vxBlockVolume<VoxelType> & vol, 
                                 const Vector3D<int4> & point = Vector3D<int4>(0,0,0),
                                 NeigborhoodsEnum eNeighborhood = e3Dneighbors06);

  /// returns the next voxel in neighborhood order.
  inline void operator++();

  /// traversal has reached end.
  inline bool IsAtEnd() const;

  /// returns the value at the current location.
  inline const VoxelType & GetVoxel() const;

  /// sets the value at the current location.
  inline void SetVoxel(const VoxelType & value);

  /// returns the current x,y,z position.
  inline const Vector3D<int4> GetPos() const;

  /// get internal volume iterator
  inline const vxBlockVolumeIterator<VoxelType> & GetInternalVolIter() const;
  
  /// sets the iterator to the given absolute position.
  inline void CenterAt(const Vector3D<int4> & pos);

  /// sets the iterator to the given absolute position.
  inline void CenterAt(const int4 iX, const int4 iY, const int4 iZ);

  /// sets the iterator to the given absolute position.
  inline void CenterFromPackedBlockOffsets(uint8 iPackedBlockOffsets);

  /// returns true if the iterator is inside the volume.
  inline bool IsInsideVolume() const;

  /// returns the number of voxels in the neighborhood.
  inline int4 GetNumberOfNeighbors() const;
  
  /// returns a const reference to the volume this iterator is attached to.
  inline const vxBlockVolume<VoxelType> & GetVolume() const;

  /// get neighbourhood type
  NeigborhoodsEnum GetNeighbourhoodType() { return m_eNeighborhood; }

  /// convert to string
  std::string ToString() const;

  /// overloaded operator<<
  inline friend std::ostream & operator<<(std::ostream & os, const vxBlockVolumeIteratorNeighbors<VoxelType> & volIter)
  {
    os << volIter.ToString();
    return( os );
  }

private:

  /// returns the next voxel in 2D  4 neighborhood xy order
  void Next2Dneighbor04xy();

  /// returns the next voxel in 2D  4 neighborhood xz order
  void Next2Dneighbor04xz();

  /// returns the next voxel in 2D  4 neighborhood yz order
  void Next2Dneighbor04yz();

  /// returns the next voxel in 2D  8 neighborhood xy order
  void Next2Dneighbor08xy();

  /// returns the next voxel in 2D  8 neighborhood xz order
  void Next2Dneighbor08xz();

  /// returns the next voxel in 2D  8 neighborhood yz order
  void Next2Dneighbor08yz();

  /// returns the next voxel in 3D  7 neighborhood order
  void Next3Dneighbor07();

  /// returns the next voxel in 3D  6 neighborhood order
  void Next3Dneighbor06();

  /// returns the next voxel in 3D 18 neighborhood order
  void Next3Dneighbor18();

  /// returns the next voxel in 3D 26 neighborhood order
  void Next3Dneighbor26();

  /// returns the next voxel in 3D 64 neighborhood order
  void Next3Dneighbor63();

private:

  /// the volume iterator doing the actual volume traversal
  vxBlockVolumeIterator<VoxelType> m_volIter;

  /// typedef for neighbour function pointer
  typedef void (vxBlockVolumeIteratorNeighbors<VoxelType>::* FunctionPtrType)();

  /// the pointer to the type of neighborhood traversing function desired
  FunctionPtrType m_NextNeighborFunctionPtr;

  /// neighbourhood function type
  NeigborhoodsEnum m_eNeighborhood;

  /// current index of neighbor voxel 
  int4 m_iNeighborIndex;

  /// current index of neighbor voxel 
  int4 m_iNumberOfNeighbors;

}; // vxBlockVolumeIteratorNeighbors


#include "vxBlockVolumeIteratorNeighbors.inl"


#endif // vxBlockVolumeIteratorNeighbors_h


// $Log: vxBlockVolumeIteratorNeighbors.h,v $
// Revision 1.1.1.1.34.1  2010/07/16 20:31:28  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.15  2003/07/08 19:20:57  michael
// added Get() for chosen neighbourhood iteration scheme
//
// Revision 1.14  2003/05/03 16:18:19  michael
// completed code review
//
// Revision 1.13  2003/05/02 14:36:56  dongqing
// code walk through
//
// Revision 1.12  2002/09/25 21:20:25  ingmar
// implemented operator<< inside the class declaration
//
// Revision 1.11  2002/05/14 21:52:09  ingmar
// added e3Dneighbors07 for traversal of 2x2x2 neighborhood
//
// Revision 1.10  2002/05/13 20:11:07  ingmar
// added GetNumberOfNeighbors()
//
// Revision 1.9  2002/05/03 21:10:50  soeren
// added GetInternalVolIter
//
// Revision 1.8  2002/04/30 13:51:06  ingmar
// added Get/SetPos From PackedBlockOffsets
//
// Revision 1.7  2002/04/25 16:35:34  ingmar
// added e2Dneighbors08 xy, xz and zy neighbor traversal
//
// Revision 1.6  2002/04/25 16:13:19  ingmar
// added e2Dneighbors04xz and zy neighbor traversal
//
// Revision 1.5  2002/04/25 16:02:28  ingmar
// added e2Dneighbors04xy neighbor traversal
//
// Revision 1.4  2002/04/18 19:54:32  ingmar
// removed obsolete GetVoxelRaw() from vxBlockVolumeIterator class
//
// Revision 1.3  2002/04/12 14:59:36  rekha
// extended to 6 & 18 & 26 & 63 neighborhoods
//
// Revision 1.2  2002/04/09 21:20:41  ingmar
// first working test
//
// Revision 1.1  2002/04/09 18:46:48  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeIteratorNeighbors.h,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
// $Id: vxBlockVolumeIteratorNeighbors.h,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
