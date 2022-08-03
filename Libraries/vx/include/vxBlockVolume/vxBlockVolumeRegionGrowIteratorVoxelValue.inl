// $Id: vxBlockVolumeRegionGrowIteratorVoxelValue.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
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
 * Pops the next position of the queue, moved the data+flag iterators 
 * to there and schedules the neighbors for adition in to the queue
 */
template <class VoxelType>
void vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::operator++()
{
  (this->*m_NextNewVoxelOfRegionPtr)();
} // operator++()


/**
 * Indicates when the region growing is finished
 * @return true when the traversal is finished
 */
template <class VoxelType>
bool vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::IsAtEnd() const
{
  return m_bGrowEnded;
} // IsAtEnd()


/**
 * Indicates when the region growing is not finished
 * @return true when the traversal is not finished
 */
template <class VoxelType>
bool vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::IsNotAtEnd() const
{
  return !m_bGrowEnded;
} // IsNotAtEnd()


/**
 * Returns the voxel value at the current position (safe version)
 * @return the voxel value at the position
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::GetVoxel() const
{
  return m_dataIter.GetVoxel();
} // GetVoxel()


/**
 * Sets the voxel value at the current position (safe version)
 * @param new voxel value.
 */
template <class VoxelType>
void vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::SetVoxel(const VoxelType & value)
{
  m_dataIter.SetVoxel(value);
} // SetVoxel()


/**
 * Returns the current position of the iterator within the volume
 * @return the x,y,z position in the volume
 */
template <class VoxelType>
const Vector3D<int4> vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::GetPos() const
{
  return m_dataIter.GetPos();
} // GetPos()


/**
* Indicates whether iterator is currently within volume
* @return true if the iterator is inside the volume
*/
template <class VoxelType>
bool vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::IsInsideVolume() const
{
  return m_dataIter.IsInsideVolume();
} // InsideVolume()


/**
 * Overloaded operator<< Operator()
 * @param type of the voxels in the data volume.
 */
template <class VoxelType>
std::ostream & operator<<(std::ostream & os, const vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType> & volIter)
{
  os << volIter.GetAsString();
  return os;
} // operator<<()


#undef FILE_REVISION


// $Log: vxBlockVolumeRegionGrowIteratorVoxelValue.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/03 16:18:21  michael
// completed code review
//
// Revision 1.4  2003/05/02 14:36:57  dongqing
// code walk through
//
// Revision 1.3  2002/05/03 21:18:58  soeren
// added GetInternalVolIter
//
// Revision 1.2  2002/05/03 15:35:09  soeren
// fixed regiongrow for range which contains background
//
// Revision 1.1  2002/04/30 13:39:10  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeRegionGrowIteratorVoxelValue.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeRegionGrowIteratorVoxelValue.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
