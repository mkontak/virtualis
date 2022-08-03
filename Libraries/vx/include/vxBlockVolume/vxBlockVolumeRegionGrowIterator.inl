// $Id: vxBlockVolumeRegionGrowIterator.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


/**
 * Does the book keeping to find out it the current voxel could be included in the region.
 * If the mechanickal analysis comes out positive, then the decider is asked to make the final decision.
 * @param type of the volume data iterator.
 * @param type of the voxels in the data volume.
 * @param type of the decider for wetther to include a voxel into the region.
 */
template <class VoxelType>
void vxBlockVolumeRegionGrowIterator<VoxelType>::PossiblyAddToQueue()
{
  if (  m_peekMaskIter.IsInsideVolume() &&    // still inside valid volume area
       !m_peekMaskIter.GetVoxel() // not yet flaged as processed/discovered   
       && m_rDecider.IsPartOfRegion(m_peekDataIter))
  {
    m_posQueue.push(m_peekDataIter.GetPos());
    m_peekMaskIter.SetVoxel(true);
  }
} // PossiblyAddToQueue()


/**
 * Pops the next position of the queue, moved the data+flag iterators to there 
 * and schedules the neighbors for adition in to the queue
 */
template <class VoxelType>
void vxBlockVolumeRegionGrowIterator<VoxelType>::operator++()
{
  // set m_bGrowEnded if there are no more voxels to process
  if ( m_posQueue.empty() )
  {
    m_bGrowEnded = true;
    return;
  }

  // get next voxel of region from queue
  const Vector3D<int4> nextPos = m_posQueue.front();
  m_dataIter.SetPos(nextPos);
  m_maskIter.SetPos(nextPos);
  m_posQueue.pop();
  m_peekDataIter.CenterAt(nextPos);
  m_peekMaskIter.CenterAt(nextPos);

  // peek towards next voxels that are potentially part of the region and
  // test if these voxels indeed belong to the region
  for ( ; !m_peekMaskIter.IsAtEnd(); ++m_peekMaskIter,++m_peekDataIter )
  {
    PossiblyAddToQueue();
  }
} // operator++()


/**
 * Indicates when the region growing is finished
 * @return true when the traversal is finished
 */
template <class VoxelType>
bool vxBlockVolumeRegionGrowIterator<VoxelType>::IsAtEnd() const
{
  return m_bGrowEnded;
} // IsAtEnd()


/**
 * Indicates when the region growing is not finished
 * @return true when the traversal is not finished
 */
template <class VoxelType>
bool vxBlockVolumeRegionGrowIterator<VoxelType>::IsNotAtEnd() const
{
  return !m_bGrowEnded;
} // IsNotAtEnd()


/**
 * Returns the voxel value at the current position (safe version)
 * @return the voxel value at the position
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeRegionGrowIterator<VoxelType>::GetVoxel() const
{
  return m_dataIter.GetVoxel();
} // GetVoxel()


/**
 * Sets the voxel value at the current position (safe version)
 * @param new voxel value.
 */
template <class VoxelType>
void vxBlockVolumeRegionGrowIterator<VoxelType>::SetVoxel(const VoxelType & value)
{
  m_dataIter.SetVoxel(value);
} // SetVoxel()

 
/**
 * Get position
 * @return the current position of the iterator within the volume
 */
template <class VoxelType>
const Vector3D<int4> vxBlockVolumeRegionGrowIterator<VoxelType>::GetPos() const
{
  return m_dataIter.GetPos();
} // GetPos()


/**
 * Indicated whether the iterator is inside the volume
 * @return true if the iterator is inside the volume
 */
template <class VoxelType>
bool vxBlockVolumeRegionGrowIterator<VoxelType>::IsInsideVolume() const
{
  return m_dataIter.IsInsideVolume();
} // InsideVolume()


#undef FILE_REVISION


// $Log: vxBlockVolumeRegionGrowIterator.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/05/03 16:18:20  michael
// completed code review
//
// Revision 1.5  2003/05/02 14:36:56  dongqing
// code walk through
//
// Revision 1.4  2002/09/25 21:01:15  ingmar
// removed duplicate operator <<
//
// Revision 1.3  2002/05/03 21:18:58  soeren
// added GetInternalVolIter
//
// Revision 1.2  2002/04/26 12:53:47  ingmar
// new deciders and region grow iterators based only on a single template parameter, the VoxelType
//
// Revision 1.1  2002/04/22 19:54:01  soeren
// intial version of Regiongrow iterator and decider
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeRegionGrowIterator.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeRegionGrowIterator.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
