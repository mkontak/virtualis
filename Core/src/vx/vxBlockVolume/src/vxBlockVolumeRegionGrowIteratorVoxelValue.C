// $Id: vxBlockVolumeRegionGrowIteratorVoxelValue.C,v 1.1.1.1.34.1 2010/07/16 20:31:42 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Soeren & Ingmar {soeren|ingmar}@viatronix.com


// includes
#include "StdAfx.h"
#include "vxBlockVolumeRegionGrowIteratorVoxelValue.h" // header


// defines
#define FILE_REVISION "$Revision: 1.1.1.1.34.1 $"


// namespaces
using namespace std;


// partial template instantiations
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorVoxelValue<uint1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorVoxelValue<uint2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorVoxelValue<uint4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorVoxelValue< int1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorVoxelValue< int2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorVoxelValue< int4>;


/**
 * Constructor
 * @param data type of each voxel.
 * @param seedposition that must be part of the region.
 * @param volume to find the region in.
 * @param decides wether to inclue a voxel into the region.
 * @param mask of voxels that should NOT be touched (will be modified/extended by the region grow iterator).
 */
template <class VoxelType>
vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::vxBlockVolumeRegionGrowIteratorVoxelValue(Vector3D<int4> seedPos, 
                                          vxBlockVolume<VoxelType> & dataVolume, 
                                          const vxDeciderVoxelValue<VoxelType> & decider, 
                                          vxBlockVolume<bool> * pMaskVolume)
: 
m_rDataVolume(dataVolume),
m_dataIter(dataVolume, seedPos), 
m_rDecider(decider),
m_pMaskVol((pMaskVolume != NULL) ? NULL : new vxBlockVolume<bool>(dataVolume.GetHeader())),
m_rMaskVol((pMaskVolume != NULL) ? *pMaskVolume : *m_pMaskVol),
m_maskIter(m_rMaskVol, seedPos),
m_bGrowEnded(false)
{
  if (!m_dataIter.IsInsideVolume())
  {
    LogWrn("seed iterator not in volume","vxBlockVolumeRegionGrowIteratorVoxelValue","vxBlockVolumeRegionGrowIteratorVoxelValue");
  }

  // select the IsNewVoxelOfRegion according to the type of decider
  if (m_rDecider.IsPartOfRegion(m_rDataVolume.GetHeader().GetBackground()))
  {
    m_NextNewVoxelOfRegionPtr = &vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::NextNewVoxelOfRegionAndInsideVolume;
  }
  else
  {
    m_NextNewVoxelOfRegionPtr = &vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::NextNewVoxelOfRegion;
  }

  // make sure the seedpoint is not locked and is part of the region
  if (m_maskIter.GetVoxel() == false && m_rDecider.IsPartOfRegion(m_dataIter.GetVoxel()))
  {
    // initialize mask
    m_maskIter.SetVoxel(true);  

    // initialize queue
    m_posQueue.push(m_dataIter.GetPosAsPackedBlockOffsets());
  }
  else
  {
    LogWrn("seed iterator not in region","vxBlockVolumeRegionGrowIteratorVoxelValue","vxBlockVolumeRegionGrowIteratorVoxelValue");
  }

  // increment once to get the start voxel off the queue, but also add the neighbors on the queue 
  // (or to set region grow ended to true if no start voxel was placed)
  ++(*this);
} // constructor


/**
 * Destructor
 */
template <class VoxelType>
vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::~vxBlockVolumeRegionGrowIteratorVoxelValue()
{
  if (m_pMaskVol != NULL)
  {
    delete m_pMaskVol;
    m_pMaskVol = NULL;
  }
} // destructor


/**
 * Get as string
 * @return the current state of the iterator in a string
 */
template <class VoxelType>
std::string vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::GetAsString() const
{
  stringstream ss;

  ss << "vxBlockVolumeRegionGrowIteratorVoxelValue{Pos: " << m_dataIter.GetPos();

  if (IsInsideVolume() == true)
  {
    ss << ", inside, Vox: ";
  }
  else
  {
    ss << ", outside,Vox: ";
  }
  ss << m_dataIter.GetVoxel() << "}";

  return string(ss.str());
} // GetAsString()


/**
 * Does the book keeping to find out it the current voxel could be included in the region.
 * If the mechanickal analysis comes out positive, then the decider is asked to make the 
 * final decision. Does also make sure that we are not growing outside the volume
 */
template <class VoxelType>
void vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::NextNewVoxelOfRegionAndInsideVolume() 
{
  // set m_bGrowEnded if there are no more voxels to process
  if (m_posQueue.empty())
  {
    m_bGrowEnded = true;
    return;
  }

  do 
  {
    // get next voxel of region from queue
    const uint8 nextPos(static_cast<int4>(m_posQueue.front()));
    m_dataIter.SetPosFromPackedBlockOffsets( nextPos );
    m_posQueue.pop();    
  } while (!m_dataIter.IsInsideVolume() && !m_posQueue.empty());

  if(!m_dataIter.IsInsideVolume())
  {
    m_bGrowEnded = true;
    return;
  } // if

  m_maskIter.SetPos(m_dataIter);

  const Vector3D<int4> viDim( m_rDataVolume.GetHeader().GetVolDim() );
  const Vector3D<int4> viPos( m_maskIter.GetPos() );

  // peek towards next 6 neighboring voxels that are potentially part of the region and
  // test if these voxels are not yet processed and belong to the region
  if (!m_maskIter.Peek1nx0py0pz() && viPos.m_x-1 >= 0
       && m_rDecider.IsPartOfRegion(m_dataIter.Peek1nx0py0pz()))
  {
    m_posQueue.push( m_dataIter.GetPosAsPackedBlockOffsetsPrevX() );
    m_maskIter.SetVoxelPrevX(true);
  }

  if (!m_maskIter.Peek1px0py0pz() && viPos.m_x+1 < viDim.m_x
       && m_rDecider.IsPartOfRegion(m_dataIter.Peek1px0py0pz()))
  {
    m_posQueue.push( m_dataIter.GetPosAsPackedBlockOffsetsNextX() );
    m_maskIter.SetVoxelNextX(true);
  }

  if (!m_maskIter.Peek0px1ny0pz() && viPos.m_y-1 >= 0
       && m_rDecider.IsPartOfRegion(m_dataIter.Peek0px1ny0pz()))
  {
    m_posQueue.push( m_dataIter.GetPosAsPackedBlockOffsetsPrevY() );
    m_maskIter.SetVoxelPrevY(true);
  }

  if (!m_maskIter.Peek0px1py0pz() && viPos.m_y+1 < viDim.m_y
       && m_rDecider.IsPartOfRegion(m_dataIter.Peek0px1py0pz()))
  {
    m_posQueue.push( m_dataIter.GetPosAsPackedBlockOffsetsNextY() );
    m_maskIter.SetVoxelNextY(true);
  }

  if (!m_maskIter.Peek0px0py1nz() && viPos.m_z-1 >= 0
       && m_rDecider.IsPartOfRegion(m_dataIter.Peek0px0py1nz()))
  {
    m_posQueue.push( m_dataIter.GetPosAsPackedBlockOffsetsPrevZ() );
    m_maskIter.SetVoxelPrevZ(true);
  }

  if (!m_maskIter.Peek0px0py1pz() && viPos.m_z+1 < viDim.m_z
       && m_rDecider.IsPartOfRegion(m_dataIter.Peek0px0py1pz()))
  {
    m_posQueue.push( m_dataIter.GetPosAsPackedBlockOffsetsNextZ() );
    m_maskIter.SetVoxelNextZ(true);
  }
} // NextNewVoxelOfRegionAndInsideVolume()


/**
 * Does the book keeping to find out it the current voxel could be included in the region.
 * If the mechanickal analysis comes out positive, then the decider is asked to make the final
 * decision. Does NOT check for outside the volume, as it is doe implicitly through the decider
 */
template <class VoxelType>
void vxBlockVolumeRegionGrowIteratorVoxelValue<VoxelType>::NextNewVoxelOfRegion() 
{ 
  // set m_bGrowEnded if there are no more voxels to process
  if (m_posQueue.empty())
  {
    m_bGrowEnded = true;
    return;
  }

  // get next voxel of region from queue
  const uint8 nextPos(m_posQueue.front());
  m_dataIter.SetPosFromPackedBlockOffsets(nextPos);
  m_posQueue.pop();    
  m_maskIter.SetPos(m_dataIter);
  
  if(!m_dataIter.IsInsideVolume())
  {
    m_bGrowEnded = true;
    return;
  } // if

  const Vector3D<int4> viDim( m_rDataVolume.GetHeader().GetVolDim() );
  const Vector3D<int4> viPos( m_maskIter.GetPos() );

  // peek towards next 6 neighboring voxels that are potentially part of the region and
  // test if these voxels are not yet processed and belong to the region
  if (!m_maskIter.Peek1nx0py0pz() && viPos.m_x-1 >= 0
       && m_rDecider.IsPartOfRegion(m_dataIter.Peek1nx0py0pz()))
  {
    m_posQueue.push(m_dataIter.GetPosAsPackedBlockOffsetsPrevX());
    m_maskIter.SetVoxelPrevX(true);
  }

  if (!m_maskIter.Peek1px0py0pz() && viPos.m_x+1 < viDim.m_x
       && m_rDecider.IsPartOfRegion(m_dataIter.Peek1px0py0pz()))
  {
    m_posQueue.push(m_dataIter.GetPosAsPackedBlockOffsetsNextX());
    m_maskIter.SetVoxelNextX(true);
  }

  if (!m_maskIter.Peek0px1ny0pz() && viPos.m_y-1 >= 0
       && m_rDecider.IsPartOfRegion(m_dataIter.Peek0px1ny0pz()))
  {
    m_posQueue.push(m_dataIter.GetPosAsPackedBlockOffsetsPrevY());
    m_maskIter.SetVoxelPrevY(true);
  }

  if (!m_maskIter.Peek0px1py0pz() && viPos.m_y+1 < viDim.m_y
       && m_rDecider.IsPartOfRegion(m_dataIter.Peek0px1py0pz()))
  {
    m_posQueue.push(m_dataIter.GetPosAsPackedBlockOffsetsNextY());
    m_maskIter.SetVoxelNextY(true);
  }

  if (!m_maskIter.Peek0px0py1nz() && viPos.m_z-1 >= 0
       && m_rDecider.IsPartOfRegion(m_dataIter.Peek0px0py1nz()))
  {
    m_posQueue.push(m_dataIter.GetPosAsPackedBlockOffsetsPrevZ());
    m_maskIter.SetVoxelPrevZ(true);
  }

  if (!m_maskIter.Peek0px0py1pz() && viPos.m_z+1 < viDim.m_z
       && m_rDecider.IsPartOfRegion(m_dataIter.Peek0px0py1pz()))
  {
    m_posQueue.push(m_dataIter.GetPosAsPackedBlockOffsetsNextZ());
    m_maskIter.SetVoxelNextZ(true);
  }
} // NextNewVoxelOfRegion()


#undef FILE_REVISION


// $Log: vxBlockVolumeRegionGrowIteratorVoxelValue.C,v $
// Revision 1.1.1.1.34.1  2010/07/16 20:31:42  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/05/03 16:18:20  michael
// completed code review
//
// Revision 1.8  2003/05/02 14:36:57  dongqing
// code walk through
//
// Revision 1.7  2003/01/22 21:31:43  ingmar
// creating a new volume needs a vxVolumeHeader, not just a dimension
//
// Revision 1.6  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.5  2002/06/04 22:47:17  soeren
// new VolIterator Peek functions that allow peeking into a mix of positive and negative offsets
//
// Revision 1.4  2002/05/03 15:35:09  soeren
// fixed regiongrow for range which contains background
//
// Revision 1.3  2002/05/03 13:08:33  ingmar
// in constructor: make sure the seedpoint is not locked
//
// Revision 1.2  2002/04/30 13:47:58  ingmar
// completed renaming Intensity to VoxelValue
//
// Revision 1.1  2002/04/30 13:39:05  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeRegionGrowIteratorVoxelValue.C,v 1.1.1.1.34.1 2010/07/16 20:31:42 dongqing Exp $
// $Id: vxBlockVolumeRegionGrowIteratorVoxelValue.C,v 1.1.1.1.34.1 2010/07/16 20:31:42 dongqing Exp $
