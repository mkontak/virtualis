// $Id: vxBlockVolumeRegionGrowIteratorVoxelValue.h,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

/*
  An iterator for linearized region growing
*/

#ifndef vxBlockVolumeRegionGrowIteratorVoxelValue_h
#define vxBlockVolumeRegionGrowIteratorVoxelValue_h


// includes
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxDeciderVoxelValue.h"


// class definition
template <class VoxelType>
class vxBlockVolumeRegionGrowIteratorVoxelValue : public vxBlockVolumeIteratorNeighborsConsts
{
// functions
public:

  /// constructor
  vxBlockVolumeRegionGrowIteratorVoxelValue(Vector3D<int4> seedPos, 
                                            vxBlockVolume<VoxelType> & dataVolume, 
                                            const vxDeciderVoxelValue<VoxelType> & deciderIntensity, 
                                            vxBlockVolume<bool> * pMaskVolume = NULL);

  /// destructor
  ~vxBlockVolumeRegionGrowIteratorVoxelValue();

  /// increments the iterator position in growing direction
  inline void operator ++ ();

  /// traversal has reached end
  inline bool IsAtEnd() const;

  /// traversal has reached end
  inline bool IsNotAtEnd() const;

  /// returns the value at the current location
  inline const VoxelType & GetVoxel() const;

  /// sets the value at the current location
  inline void SetVoxel(const VoxelType & value);

  /// get internal volume iterator
  inline const vxBlockVolumeIterator<VoxelType> & GetInternalVolIter() const { return m_dataIter; }

  /// returns true if the iterator is inside the volume
  inline bool IsInsideVolume() const;

  /// convert to string
  std::string GetAsString() const;

  /// overloaded operator<<
  friend inline std::ostream & operator<<(std::ostream & os, const vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType> & iter)
  {
    iter.GetAsString();
  }

  /// get position
  inline const Vector3D<int4> GetPos() const;

private:

  /// what to do after moving the iterators onto a potentially next region voxel
  void NextNewVoxelOfRegionAndInsideVolume();

  /// what to do after moving the iterators onto a potentially next region voxel
  void NextNewVoxelOfRegion();

// data
private:

  /// the volume iterator for the data
  vxBlockVolumeIterator<VoxelType> m_dataIter;

  /// the volume of already processed flags
  vxBlockVolume<bool> * m_pMaskVol;

  /// the volume of already processed flags
  vxBlockVolume<bool> & m_rMaskVol;

  /// the volume iterator for the mask volume
  vxBlockVolumeIterator<bool> m_maskIter;

  /// the queue of discovered but not processed voxels
  std::queue<uint8> m_posQueue;       

  /// status of region grow completion (ended or not)
  bool m_bGrowEnded;                 

  /// the real data
  vxBlockVolume<VoxelType> & m_rDataVolume;

  /// the decider to determine a voxel's membership in the region
  const vxDeciderVoxelValue<VoxelType> & m_rDecider;

  ///// typedef 
  typedef void (vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::* FunctionPtrType)();

  ///// the pointer to the type of neighborhood traversing function desired
  FunctionPtrType m_NextNewVoxelOfRegionPtr;
}; // vxBlockVolumeRegionGrowIteratorVoxelValue


#include "vxBlockVolumeRegionGrowIteratorVoxelValue.inl"


#endif // vxBlockVolumeRegionGrowIteratorVoxelValue_h


// $Log: vxBlockVolumeRegionGrowIteratorVoxelValue.h,v $
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
// Revision 1.6  2003/05/03 16:18:21  michael
// completed code review
//
// Revision 1.5  2002/05/22 20:50:23  ingmar
// added sectioning for doxygen documentation
//
// Revision 1.4  2002/05/03 21:18:58  soeren
// added GetInternalVolIter
//
// Revision 1.3  2002/05/03 15:35:09  soeren
// fixed regiongrow for range which contains background
//
// Revision 1.2  2002/04/30 13:47:58  ingmar
// completed renaming Intensity to VoxelValue
//
// Revision 1.1  2002/04/30 13:39:08  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeRegionGrowIteratorVoxelValue.h,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
// $Id: vxBlockVolumeRegionGrowIteratorVoxelValue.h,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
