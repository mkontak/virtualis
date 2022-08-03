// $Id: vxBlockVolumeRegionGrowIteratorHeap.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.com

#include "StdAfx.h"
#include "vxBlockVolumeRegionGrowIteratorHeap.h" // header

#define FILE_REVISION "$Revision: 1.1.1.1 $"

using namespace std;

template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorHeap<uint1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorHeap<uint2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorHeap<uint4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorHeap< int1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorHeap< int2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorHeap< int4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIteratorHeap<float4>;


/**
 * Constructor
 * @param data type of each voxel
 * @param seedposition that must be part of the region
 * @param volume to find the region in
 * @param decides wether to inclue a voxel into the region
 * @param e3DNeighborhood, type of neighborhood connectivity desired for this region grow
 * @param mask of voxels that should NOT be touched (will be modified/extended by the region grow iterator)
 */
template <class VoxelType>
vxBlockVolumeRegionGrowIteratorHeap<VoxelType>::
vxBlockVolumeRegionGrowIteratorHeap(Vector3D<int4> seedPos, 
                                    vxBlockVolume<VoxelType> & dataVolume, 
                                    const vxDecider<VoxelType> & decider,
                                    vxInserter & inserter,
                                    const int4 backgroundCost,
                                    const vxComplex & seedCost, 
                                    vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum e3DNeighborhood, 
                                    vxBlockVolume<bool> * pMaskVolume)
: 
m_rDataVolume(dataVolume),
m_dataIter(dataVolume, seedPos), 
m_peekDataIter(dataVolume, seedPos, e3DNeighborhood),
m_rDecider(decider),
m_pMaskVol((pMaskVolume != NULL) ? NULL : new vxBlockVolume<bool>(dataVolume.GetHeader())),
m_rMaskVol((pMaskVolume != NULL) ? *pMaskVolume : *m_pMaskVol),
m_maskIter(m_rMaskVol, seedPos),
m_peekMaskIter(m_rMaskVol, seedPos, e3DNeighborhood),
m_bGrowEnded(false),
m_inserter(inserter),
m_costVol(vxVolumeHeader(dataVolume.GetHeader().GetVolDim(), Triple<float4>(1.0,1.0,1.0), 32, backgroundCost)),
m_costIter(m_costVol, seedPos),
m_peekCostIter(m_costVol, seedPos, e3DNeighborhood)
{
  // check if m_volIterator is inside
  if (!m_dataIter.IsInsideVolume())
  {
    LogWrn("heap region grow iterator with seed at "+m_dataIter.GetPos()+" not in volume of dim "+m_dataIter.GetVolume().GetHeader().GetVolDim(),"vxBlockVolumeRegionGrowIteratorHeap","vxBlockVolumeRegionGrowIteratorHeap");
  }
  
  for (; !m_peekDataIter.IsAtEnd(); ++m_peekDataIter)
  {
    // just advance to end which is setting it back to the center voxel
  } 

  m_costIter.SetVoxel(seedCost);
  vxComplex vcBGCost(static_cast<float4>(backgroundCost), static_cast<float4>(backgroundCost));
  if (!m_inserter.NeedToUpdateHeap(seedCost, vcBGCost, m_dataIter.GetPos(), m_peekCostIter.GetNumberOfNeighbors()))
  { // updates cost
    LogWrn("<RegGrowHeapIterator::constructor> : no need to insert seed","RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>","RegGrowHeapIterator");
  }
  
  // make sure the seedpoint is not locked and is part of the region
  if (m_maskIter.GetVoxel() == false  &&  m_rDecider.IsPartOfRegion(m_peekDataIter))
  {
    // reset peek data iter to first neighbor next to seed point
    m_peekDataIter.CenterAt(seedPos);

    // initialize queue
    m_costPosHeap.push(HeapElement(m_dataIter.GetPosAsPackedBlockOffsets(), seedCost));
  }
  else
  {
    if (m_maskIter.GetVoxel() == true)
    {
      LogWrn("heap region grow iterator with seed at "+m_dataIter.GetPos()+" is blocked by mask","vxBlockVolumeRegionGrowIteratorHeap","vxBlockVolumeRegionGrowIteratorHeap");
    }
    else
    {
      LogWrn("heap region grow iterator with seed at "+m_dataIter.GetPos()+" and value "+ToAscii(m_peekDataIter.GetVoxel())+" not in region","vxBlockVolumeRegionGrowIteratorHeap","vxBlockVolumeRegionGrowIteratorHeap");
    }
  }
  // increment once to get the start voxel off the queue, but also add the neighbors on the queue 
  // (or to set region grow ended to true if no start voxel was placed)
  ++(*this); 
} // constructor


/**
 * Destructor
 */
template <class VoxelType>
vxBlockVolumeRegionGrowIteratorHeap<VoxelType>::~vxBlockVolumeRegionGrowIteratorHeap()
{
  if (m_pMaskVol != NULL)
  {
    delete m_pMaskVol;
    m_pMaskVol = NULL;
  }
} // destructor


/**
 * Returns the current state of the iterator in a string
 * @return string
 */
template <class VoxelType>
std::string vxBlockVolumeRegionGrowIteratorHeap<VoxelType>::GetAsString() const
{
  stringstream ss;

  ss << "vxBlockVolumeRegionGrowIteratorHeap{Pos: " << m_dataIter.GetPos();

  if (IsInsideVolume() == true)
  {
    ss << ", inside, Vox: ";
  }
  else
  {
    ss << ", outside,Vox: ";
  }
  ss << m_dataIter.GetVoxel() << "}";

  return(string(ss.str()));
} // GetAsString()


#undef FILE_REVISION


// complete revision history:
// $Log: vxBlockVolumeRegionGrowIteratorHeap.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/05/16 11:48:13  michael
// code review
//
// Revision 1.7  2003/05/03 16:18:20  michael
// completed code review
//
// Revision 1.6  2003/01/22 21:32:47  ingmar
// creating a new volume needs a vxVolumeHeader, not just a dimension
// also added more detailed debug info
//
// Revision 1.5  2002/09/17 17:17:55  dongqing
// Tiny bug fixing and comments after testing on the Skeletonator Module
//
// Revision 1.4  2002/08/28 18:37:40  ingmar
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
// Revision 1.8  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.7  2002/05/03 13:07:53  ingmar
// added descriptive comment
//
// Revision 1.6  2002/05/02 19:08:09  soeren
// added one more check in construco
//
// Revision 1.5  2002/04/27 22:31:52  ingmar
// added comments; code beautify
//
// Revision 1.4  2002/04/26 12:53:13  ingmar
// new deciders and region grow iterators based only on a single template parameter, the VoxelType
//
// Revision 1.3  2002/04/25 22:04:38  jaddonisio
// i. Instantiate deciders.
// ii. Correct logic on 'IsPartOfRegion'.
//
// Revision 1.2  2002/04/22 20:44:36  soeren
// added new constructor to regiongrowIterator
//
// Revision 1.1  2002/04/22 19:54:00  soeren
// intial version of Regiongrow iterator and decider
//
// $Id: vxBlockVolumeRegionGrowIteratorHeap.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
