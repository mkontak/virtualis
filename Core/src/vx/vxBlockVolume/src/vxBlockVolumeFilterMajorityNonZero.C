// $Id: vxBlockVolumeFilterMajorityNonZero.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar ingmar@viatronix.com


// include declarationss
#include "stdafx.h"
#include "vxBlockVolumeFilterMajorityNonZero.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"


// define declarations
#define FILE_REVISION "$Revision: 1.1.1.1 $"


template class vxBlockVolumeFilterMajorityNonZero< int1>;
template class vxBlockVolumeFilterMajorityNonZero< int2>;
template class vxBlockVolumeFilterMajorityNonZero< int4>;
template class vxBlockVolumeFilterMajorityNonZero<uint1>;
template class vxBlockVolumeFilterMajorityNonZero<uint2>;
template class vxBlockVolumeFilterMajorityNonZero<uint4>;

using namespace std;


/**
 * Standard constructor.
 */
template <class VoxelType> 
vxBlockVolumeFilterMajorityNonZero<VoxelType>::vxBlockVolumeFilterMajorityNonZero(vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood)
:
m_eNeighborhood(eNeighborhood)
{
} // Standard constructor


/**
 * Copy constructor.
 */
template <class VoxelType> 
vxBlockVolumeFilterMajorityNonZero<VoxelType>::vxBlockVolumeFilterMajorityNonZero(vxBlockVolumeFilterMajorityNonZero<VoxelType> & src)
:
vxBlockVolumeFilter<VoxelType>(src),
m_eNeighborhood(src.m_eNeighborhood)
{
} // Copy constructor


/**
 * Run the filter.
 */
template <class VoxelType> 
void vxBlockVolumeFilterMajorityNonZero<VoxelType>::ApplyFilter()
{
  // create iterators for the src and dst volumes
  vxBlockVolumeIteratorNeighbors<VoxelType> srcIterNeighbors(*m_pSrcVolume, Vector3D<int4>(0,0,0), m_eNeighborhood);
  vxBlockVolumeIterator<VoxelType> srcIter(*m_pSrcVolume);
  vxBlockVolumeIterator<VoxelType> dstIter(*m_pDstVolume);

  // find the scaling fators to backproject a voxel from dst vol to src vol
  Vector3D<float4> fSrcUnits = Vector3D<float4>(m_pSrcVolume->GetHeader().GetVoxelUnits());
  Vector3D<float4> fDstUnits = Vector3D<float4>(m_pDstVolume->GetHeader().GetVoxelUnits());
  Vector3D<float4> fScaleVec = Vector3D<float4>(fDstUnits.GetDividedAnIso(Triple<float4>(fSrcUnits)));

 // run through all destination volume voxels and set their value to the median value taken from the src volume
  vector<VoxelType> vVoxels(1 + srcIterNeighbors.GetNumberOfNeighbors()); // one extra for center voxel
  for ( dstIter.SetPos(0,0,0); dstIter.IsNotAtEnd(); dstIter.NextBlockZYX() )
  {
    for ( ; dstIter.IsNotAtEndOfBlock(); dstIter.NextZYXinsideBlock() )
    {
      Vector3D<float4> fSamplePos(dstIter.GetPosX(), dstIter.GetPosY(), dstIter.GetPosZ());
      fSamplePos = fSamplePos.GetScaledAnIso( Triple<float4>(fScaleVec) );
      Vector3D<int4> iNeighborhoodCenterPos((int4)(fSamplePos.X()+0.5), (int4)(fSamplePos.Y()+0.5), (int4)(fSamplePos.Z()+0.5)); // rounded from vfSamplePos
      int2 iCount(0);
      for ( srcIterNeighbors.CenterAt(iNeighborhoodCenterPos); !srcIterNeighbors.IsAtEnd(); ++srcIterNeighbors )
      {
        vVoxels[iCount++] = srcIterNeighbors.GetVoxel();
      }
      vVoxels[iCount] = srcIterNeighbors.GetVoxel();
      dstIter.SetVoxel(SelectMajorityValueNonZero(vVoxels));
    }
  }
} // ApplyFilter()




#undef FILE_REVISION

// $Log: vxBlockVolumeFilterMajorityNonZero.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/05/16 11:48:14  michael
// code review
//
// Revision 1.2  2003/01/24 21:58:22  dongqing
// resolve conflicts coming from the modification of Triple, Normal, Point...
//
// Revision 1.1  2002/09/27 16:41:45  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeFilterMajorityNonZero.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxBlockVolumeFilterMajorityNonZero.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $