// $Id: vxBlockVolumeRegionGrowIteratorHist.h,v 1.3.2.1 2010/07/16 20:31:28 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

#ifndef vxBlockVolumeRegionGrowIteratorHist_h
#define vxBlockVolumeRegionGrowIteratorHist_h


// includes
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxDeciderVoxelValue.h"


/**
 * An iterator for linearized region growing.
 */
class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorHist : public vxBlockVolumeIteratorNeighborsConsts
{
// functions
public:

  /// constructor
  vxBlockVolumeRegionGrowIteratorHist(Vector3D<int4> seedPos, 
                                      vxBlockVolume<uint2> & dataVolume, 
                                      const vxDeciderVoxelValue<int4> & deciderIntensity,
                                      vxBlockVolume<bool> * pMaskVolume = NULL,
                                      vxBlockVolume<bool> * pRegionTagVolume = NULL);

  /// destructor
  ~vxBlockVolumeRegionGrowIteratorHist();

  /// run iterator to end
  inline int4 Run();

  /// increments the iterator position in growing direction
  inline void operator ++ ();

  /// traversal has reached end
  inline bool IsAtEnd() const { return m_bGrowEnded; }

  /// traversal has reached end
  inline bool IsNotAtEnd() const { return !m_bGrowEnded; }

  /// returns the value at the current location.
  inline const uint2 & GetVoxel() const;

  /// sets the value at the current location.
  inline void SetVoxel(const uint2 & value);

  /// get internal volume iterator
  inline const vxBlockVolumeIterator<uint2> & GetInternalVolIter() const { return m_iterData; }

  /// Returns true if the iterator is inside the volume.
  inline bool IsInsideVolume() const;

  /// convert to string.
  std::string GetAsString() const;

  /// overloaded operator<<
  friend inline std::ostream & operator<<(std::ostream & os, const vxBlockVolumeRegionGrowIteratorHist & iter)
  {
    iter.GetAsString();
  }

  // get position
  inline const Vector3D<int4> GetPos() const;

private:

  /// what to do after moving the iterators onto a potentially next region voxel
  void NextNewBlock16OfRegion();

  /// what to do after moving the iterators onto a potentially next region voxel
  void NextNewVoxelOfRegionAndInsideVolume();

  /// what to do after moving the iterators onto a potentially next region voxel
  void NextNewVoxelOfRegion();

  /// next block by traversing along YZ
  void NextVoxelsByYZ();

  /// next block by traversing along YZ
  void NextVoxelsByXZ();

  /// next block by traversing along YZ
  void NextVoxelsByXY();

  /// next voxels in neighborhood
  void NextVoxelsInNeighborhood();
  
  /// test if in block16 region
  bool IsEntireBlockInRegion(const int4 iHistVoxel, const vxBlockVolumeIterator<bool> & rMaskIter);

  /// update mask, region, number of voxels after finding a fully contained block
  void UpdateForFullyContainedBlock();

// data
private:

  /// the volume iterator for the data
  vxBlockVolumeIterator<uint2> m_iterData;

  /// volume of region tags
  vxBlockVolume<bool> * m_pRegionTagVol;

  /// volume of region tags
  vxBlockVolume<bool> & m_rRegionTagVol;

  /// volume iterator for the region volume
  vxBlockVolumeIterator<bool> m_iterRegion;

  /// volume of already processed flags
  vxBlockVolume<bool> * m_pMaskVol;

  /// volume of already processed flags
  vxBlockVolume<bool> & m_rMaskVol;

  /// volume iterator for the mask volume
  vxBlockVolumeIterator<bool> m_iterMask;

  /// copy of maskVol that is passed in
  vxBlockVolume<bool> m_maskOrgVol;
  
  /// iterator for maskOrg
  vxBlockVolumeIterator<bool> m_iterMaskOrg;
  
  /// the queue of discovered but not processed inside region voxels
  std::queue<uint8> m_queueOfVoxels;       

  /// the queue of discovered but not processed inside region 16x16x16 blocks
  std::queue<uint8> m_queueOfBlock16;

  /// status of region grow completion (ended or not)
  bool m_bGrowEnded;                 

  /// the real data
  vxBlockVolume<uint2> & m_rDataVolume;

  /// iterator for 16x16x16 histogram volume
  vxBlockVolumeIterator<uint2> m_iterHist;

  /// mask for blocks entirely inside region
  uint4 m_uMaskBlockIsEntirelyInsideRegion;

  /// mask for blocks entirely outside region
  uint4 m_uMaskBlockIsEntirelyOutsideRegion;

  /// mask for blocks that have some outside
  uint4 m_uMaskBlockHasSomeOutsideRegionVoxels;

  /// decider to determine a voxel's membership in the region
  const vxDeciderVoxelValue<int4> & m_decider;

  /// typedefs
  typedef void (vxBlockVolumeRegionGrowIteratorHist::* FunctionPtrType)();

  /// pointer to the type of neighborhood traversing function desired
  FunctionPtrType m_NextVoxelOrQueuePtr;

  /// pointer to last block taken from queue
  const vxBlock<bool> * m_pLastBlock;

  /// number of voxels in region grow
  uint4 m_uNumOfVoxels;

}; // vxBlockVolumeRegionGrowIteratorHist


#include "vxBlockVolumeRegionGrowIteratorHist.inl"


#endif // vxBlockVolumeRegionGrowIteratorHist_h


// Revision History:
// $Log: vxBlockVolumeRegionGrowIteratorHist.h,v $
// Revision 1.3.2.1  2010/07/16 20:31:28  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.3  2007/06/28 19:41:47  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.2.4.1  2007/04/25 19:47:08  romy
// converted decider type to int4. Issue#5611
//
// Revision 1.2  2006/03/29 19:54:29  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2003/07/02 15:58:53  michael
// fixed block based region grow iterator
//
// Revision 1.15  2003/06/19 20:30:15  michael
// coding standard ...
//
// Revision 1.14  2003/06/18 15:19:48  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.13  2003/05/03 16:18:20  michael
// completed code review
//
// Revision 1.12  2002/12/12 17:07:39  ingmar
// removed binary histogram 04 ( 3x better histogram creation time, only slightly slower raw region growing, faster overall region grow time when also counting volume updates)
//
// Revision 1.11  2002/10/07 22:22:07  jaddonisio
// Count the number of voxels correctly.
//
// Revision 1.10  2002/08/21 15:43:08  frank
// Exported classes to DLL.
//
// Revision 1.9  2002/08/20 15:46:49  jaddonisio
// Clean up.
//
// Revision 1.8  2002/08/20 15:11:34  jaddonisio
// Use new mask based on block04.
//
// Revision 1.7  2002/08/16 21:55:10  jaddonisio
// Spacing and comments.
//
// Revision 1.6  2002/08/16 18:19:03  jaddonisio
// Moved some code into functions for clarity.
//
// Revision 1.5  2002/08/15 14:29:03  jaddonisio
// Fixed logic to account for locking but now a little bit slower.
//
// Revision 1.4  2002/08/13 19:19:35  jaddonisio
// Bidirectional traversal between 16, 04, & 01 regions.
//
// Revision 1.3  2002/08/12 19:25:45  jaddonisio
// New 16x16x16 -> 4x4x4 -> voxel iteration works. Faster by a factor of 3.7, approximately.
//
// Revision 1.2  2002/08/06 22:31:55  jaddonisio
// Finished finding consecutive blocks of 16x16x16 that are in the region. More to come...
//
// Revision 1.1  2002/08/05 22:09:31  jaddonisio
// Initial version.
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
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeRegionGrowIteratorHist.h,v 1.3.2.1 2010/07/16 20:31:28 dongqing Exp $
// $Id: vxBlockVolumeRegionGrowIteratorHist.h,v 1.3.2.1 2010/07/16 20:31:28 dongqing Exp $
