// $Id: vxBlockVolumeRegionGrowIterator.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
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
#include "vxBlockVolumeRegionGrowIterator.h" // header


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


// namespaces
using namespace std;


// partial template instatiations
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIterator<uint1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIterator<uint2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIterator<uint4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIterator< int1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIterator< int2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRegionGrowIterator< int4>;


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
vxBlockVolumeRegionGrowIterator<VoxelType>::vxBlockVolumeRegionGrowIterator(Vector3D<int4> seedPos, 
                                                                            vxBlockVolume<VoxelType> & dataVolume, 
                                                                            const vxDecider<VoxelType> & decider, 
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
m_bGrowEnded(false)
{
  if (!m_dataIter.IsInsideVolume())
  {
    LogWrn("seed iterator not in volume","vxBlockVolumeRegionGrowIterator","vxBlockVolumeRegionGrowIterator");
  }

  for (; !m_peekDataIter.IsAtEnd(); ++m_peekDataIter)
  {
    // just advance to end which is setting it back to the center voxel
  }

  // make sure the seedpoint is not locked and is part of the region
  if (m_maskIter.GetVoxel() == false && m_rDecider.IsPartOfRegion(m_peekDataIter))
  {
    // reset peek data iter to first neighbor next to seed point
    m_peekDataIter.CenterAt(seedPos);

    // initialize mask
    m_maskIter.SetVoxel(true);  

    // initialize queue
    m_posQueue.push(m_dataIter.GetPos());
  }
  else
  {
    LogWrn("seed iterator not in region","vxBlockVolumeRegionGrowIterator","vxBlockVolumeRegionGrowIterator");
  }
  // increment once to get the start voxel off the queue, but also add the neighbors on the queue 
  // (or to set region grow ended to true if no start voxel was placed)
  ++(*this);
} // constructor


/**
 * Destructor
 */
template <class VoxelType>
vxBlockVolumeRegionGrowIterator<VoxelType>::~vxBlockVolumeRegionGrowIterator()
{
  if (m_pMaskVol != NULL)
  {
    delete m_pMaskVol;
    m_pMaskVol = NULL;
  }
} // destructor


/**
 * Returns the current state of the iterator in a string.
 */
template <class VoxelType>
std::string vxBlockVolumeRegionGrowIterator<VoxelType>::GetAsString() const
{
  stringstream ss;

  ss << "vxBlockVolumeRegionGrowIterator{Pos: " << m_dataIter.GetPos();

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
// $Log: vxBlockVolumeRegionGrowIterator.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/05/03 16:18:20  michael
// completed code review
//
// Revision 1.9  2003/01/22 21:33:08  ingmar
// creating a new volume needs a vxVolumeHeader, not just a dimension
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
// $Id: vxBlockVolumeRegionGrowIterator.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
