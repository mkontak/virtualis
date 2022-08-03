// $Id: vxBlockVolumeRegionGrowIteratorHist.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
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
 * Runs the iterator through to the end
 * @return Number of iterations
 */
int4 vxBlockVolumeRegionGrowIteratorHist::Run()
//*******************************************************************
{
  while (this->IsNotAtEnd())
  {
    ++(*this);
  }
  return m_uNumOfVoxels;
} // Run()


/**
 * Pops the next position of the queue, moved the data+flag iterators to 
 * there and schedules the neighbors for adition in to the queue
 */
void vxBlockVolumeRegionGrowIteratorHist::operator++()
//*******************************************************************
{
  (this->*m_NextVoxelOrQueuePtr)();
} // operator++()


/**
 * Returns the voxel value at the current position (safe version)
 * @return the voxel value at the position
 */
const uint2 & vxBlockVolumeRegionGrowIteratorHist::GetVoxel() const
//*******************************************************************
{
  return m_iterData.GetVoxel();
} // GetVoxel()


/**
 * Sets the voxel value at the current position (safe version)
 * @param new voxel value
 */
void vxBlockVolumeRegionGrowIteratorHist::SetVoxel(const uint2 & value)
//*******************************************************************
{
  m_iterData.SetVoxel(value);
} // SetVoxel()


/**
 * Returns the current position of the iterator within the volume
 * @return the x,y,z position in the volume
 */
const Vector3D<int4> vxBlockVolumeRegionGrowIteratorHist::GetPos() const
//*******************************************************************
{
  return m_iterData.GetPos();
} // GetPos()


/**
 * Indicates whether the iterator is inside the volume
 * @return true if the iterator is inside the volume
 */
bool vxBlockVolumeRegionGrowIteratorHist::IsInsideVolume() const
//*******************************************************************
{
  return m_iterData.IsInsideVolume();
} // IsInsideVolume()


#undef FILE_REVISION


// Revision History:
// $Log: vxBlockVolumeRegionGrowIteratorHist.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/07/02 15:58:53  michael
// fixed block based region grow iterator
//
// Revision 1.9  2003/06/18 15:19:48  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.8  2003/05/03 16:18:20  michael
// completed code review
//
// Revision 1.7  2003/05/02 14:36:57  dongqing
// code walk through
//
// Revision 1.6  2002/12/12 17:07:39  ingmar
// removed binary histogram 04 (3x better histogram creation time, only slightly slower raw region growing, faster overall region grow time when also counting volume updates)
//
// Revision 1.5  2002/10/07 22:22:07  jaddonisio
// Count the number of voxels correctly.
//
// Revision 1.4  2002/08/13 19:19:35  jaddonisio
// Bidirectional traversal between 16, 04, & 01 regions.
//
// Revision 1.3  2002/08/12 19:25:45  jaddonisio
// New 16x16x16 -> 4x4x4 -> voxel iteration works. Faster by a factor of 3.7, approximately.
//
// Revision 1.2  2002/08/06 22:31:54  jaddonisio
// Finished finding consecutive blocks of 16x16x16 that are in the region. More to come...
//
// Revision 1.1  2002/08/05 22:09:31  jaddonisio
// Initial version.
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
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeRegionGrowIteratorHist.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeRegionGrowIteratorHist.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $

