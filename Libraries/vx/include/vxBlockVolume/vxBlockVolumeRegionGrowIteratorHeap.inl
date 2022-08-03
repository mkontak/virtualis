// $Id: vxBlockVolumeRegionGrowIteratorHeap.inl,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.com


// defines
#define FILE_REVISION "$Revision: 1.1.1.1.34.1 $"


/**
 * Possibly add a value to queue
 * @param neighbour index
 */
template <class VoxelType>
void vxBlockVolumeRegionGrowIteratorHeap<VoxelType>::PossiblyAddToQueue(int4 iNeighborIndex)
{    
  if (m_peekMaskIter.IsInsideVolume() &&    // still inside valid volume area
       !m_peekMaskIter.GetVoxel() // not yet flaged as processed/discovered   
       && m_rDecider.IsPartOfRegion(m_peekDataIter))
  {
    vxComplex src(m_costIter.GetVoxel());
    vxComplex dst(m_peekCostIter.GetVoxel());
    
    if (m_inserter.NeedToUpdateHeap(src, dst, m_peekCostIter.GetPos(), iNeighborIndex))
    {
      m_peekCostIter.SetVoxel(dst);
      m_costPosHeap.push(HeapElement(m_peekCostIter.GetInternalVolIter().GetPosAsPackedBlockOffsets(), dst));
    }
  } // is candidate?
} // PossiblyAddToQueue()


/**
 * Pops the next position of the queue, moved the data+flag iterators to there 
 * and schedules the neighbors for addition in to the queue.
 */
template <class VoxelType>
void vxBlockVolumeRegionGrowIteratorHeap<VoxelType>::operator++()
{  
  // set m_bGrowEnded if there are no more voxels to process
  if (m_costPosHeap.empty())
  {
    m_bGrowEnded = true;
    return;
  }

  // get next voxel of region from queue
  uint8 iNextPosOffset( m_costPosHeap.top().m_iPosOffset );
  m_maskIter.SetPosFromPackedBlockOffsets(iNextPosOffset); 

  // purge the heap from entries that had already been processed before
  while (m_maskIter.GetVoxel())
  {                              
    m_costPosHeap.pop();
    if (m_costPosHeap.empty())
    {
      m_bGrowEnded = true;
      return;
    }
    iNextPosOffset = m_costPosHeap.top().m_iPosOffset;
    m_maskIter.SetPosFromPackedBlockOffsets(iNextPosOffset);  
  }

  // now mark this new position as processed
  m_maskIter.SetVoxel(true);
  m_costPosHeap.pop();

  // and set all other iterators to this position
  m_dataIter.SetPosFromPackedBlockOffsets(iNextPosOffset);
  m_costIter.SetPosFromPackedBlockOffsets(iNextPosOffset); 
  Vector3D<int4> posVec = m_dataIter.GetPos();  
  m_peekDataIter.CenterAt(posVec);
  m_peekMaskIter.CenterAt(posVec);
  m_peekCostIter.CenterAt(posVec);

  // peek towards next voxels that are potentially part of the region and
  // test if these voxels indeed belong to the region
  for (int4 k(0); !m_peekMaskIter.IsAtEnd();  ++m_peekMaskIter, ++m_peekDataIter, ++m_peekCostIter, ++k)
  { 
    PossiblyAddToQueue(k);
  }
} // operator++()


/**
 * Add a seed point to the heap
 * @para seed point
 * @param seed cost
 * @return success
 */
template <class VoxelType>
bool vxBlockVolumeRegionGrowIteratorHeap<VoxelType>::AddSeedToHeap(const Vector3D<int4> & seedPos, 
                                                                   const vxComplex & seedCost)
{
  // back up current pos
  uint8 iPosOffset(m_dataIter.GetPosAsPackedBlockOffsets());
  m_dataIter.SetPos(seedPos);

  if (!m_dataIter.IsInsideVolume())
  {
    LogWrn("<RegGrowHeapIterator::constructor> : seed iterator not in volume","RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>","AddSeedToHeap");
  }
  m_dataIter.SetPosFromPackedBlockOffsets(iPosOffset); // return to initial start position

  m_costIter.SetPos(seedPos);
  m_costIter.SetVoxel(seedCost);
  m_costPosHeap.push(HeapElement(m_costIter.GetPosAsPackedBlockOffsets(), seedCost));

  return true;
} // AddSeedToHeap


/**
 * Indicates when the region growing is finished
 * @return true when the traversal is finished
 */
template <class VoxelType>
bool vxBlockVolumeRegionGrowIteratorHeap<VoxelType>::IsAtEnd() const
{
  return m_bGrowEnded;
} // IsAtEnd()


/**
 * Indicates when the region growing is not finished
 * @return true when the traversal is not finished
 */
template <class VoxelType>
bool vxBlockVolumeRegionGrowIteratorHeap<VoxelType>::IsNotAtEnd() const
{
  return !m_bGrowEnded;
} // IsNotAtEnd()


/**
 * Returns the voxel value at the current position (safe version)
 * @return the voxel value at the position.
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeRegionGrowIteratorHeap<VoxelType>::GetVoxel() const
{
  return m_dataIter.GetVoxel();
} // GetVoxel()


/**
 * Sets the voxel value at the current position (safe version).
 * @param new voxel value
 */
template <class VoxelType>
void vxBlockVolumeRegionGrowIteratorHeap<VoxelType>::SetVoxel(const VoxelType & value)
{
  m_dataIter.SetVoxel(value);
} // SetVoxel()


/**
 * Returns the current position of the iterator within the volume.
 * @return the x,y,z position in the volume.
 */
template <class VoxelType>
const Vector3D<int4> vxBlockVolumeRegionGrowIteratorHeap<VoxelType>::GetPos() const
{
  return m_dataIter.GetPos();
} // GetPos()


/**
 * Indicated whether iterator is inside volume
 * @return true if the iterator is inside the volume
 */
template <class VoxelType>
bool vxBlockVolumeRegionGrowIteratorHeap<VoxelType>::IsInsideVolume() const
{
  return m_dataIter.IsInsideVolume();
} // IsInsideVolume()


/**
 * Overloaded operator<<
 * @param type of the voxels in the data volume
 */
template <class VoxelType>
std::ostream & operator<<(std::ostream & os, const vxBlockVolumeRegionGrowIteratorHeap<VoxelType> & volIter)
{
  os << volIter.GetAsString();
  return os;
} // operator<<()


#undef FILE_REVISION


// $Log: vxBlockVolumeRegionGrowIteratorHeap.inl,v $
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
// Revision 1.8  2003/05/03 16:18:20  michael
// completed code review
//
// Revision 1.7  2003/05/02 14:36:57  dongqing
// code walk through
//
// Revision 1.6  2003/03/28 17:17:08  wenli
// update the region growing
//
// Revision 1.5  2002/09/17 17:17:55  dongqing
// Tiny bug fixing and comments after testing on the Skeletonator Module
//
// Revision 1.4  2002/08/28 18:38:01  ingmar
// changed vector position to block offset  as the element to be queued up
//
// Revision 1.3  2002/08/28 16:48:46  ingmar
// replaced cxCost by vxComplex
//
// Revision 1.2  2002/08/28 15:07:22  ingmar
// first successfull test
//
// Revision 1.1  2002/08/27 13:59:13  ingmar
// initial version (copy of non-heap region grow iterator
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeRegionGrowIteratorHeap.inl,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
// $Id: vxBlockVolumeRegionGrowIteratorHeap.inl,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
