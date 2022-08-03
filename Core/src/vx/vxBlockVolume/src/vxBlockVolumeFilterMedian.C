// $Id: vxBlockVolumeFilterMedian.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Rekha {ingmar|rekha}@viatronix.com


// includes
#include "stdafx.h"
#include "vxBlockVolumeFilterMedian.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"


// explicit instantiations
template class vxBlockVolumeFilterMedian< int1>;
template class vxBlockVolumeFilterMedian< int2>;
template class vxBlockVolumeFilterMedian< int4>;
template class vxBlockVolumeFilterMedian<uint1>;
template class vxBlockVolumeFilterMedian<uint2>;
template class vxBlockVolumeFilterMedian<uint4>;


// namespaces
using namespace std;


/**
 * Standard constructor
 *
 * @param eNeighborhood the neighborhood
 */
template <class VoxelType> 
vxBlockVolumeFilterMedian<VoxelType>::vxBlockVolumeFilterMedian( vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood )
: m_eNeighborhood( eNeighborhood )
{
} // vxBlockVolumeFilterMedian()


/**
 * Copy constructor
 *
 * @param other the other filter
 */
template <class VoxelType> 
vxBlockVolumeFilterMedian<VoxelType>::vxBlockVolumeFilterMedian( vxBlockVolumeFilterMedian<VoxelType> & other )
: vxBlockVolumeFilter<VoxelType>( other ),
m_eNeighborhood( other.m_eNeighborhood )
{
} // Copy constructor


/**
 * Run the filter
 */
template <class VoxelType> 
void vxBlockVolumeFilterMedian<VoxelType>::ApplyFilter()
{
  // create iterators for the src and dst volumes
  vxBlockVolumeIteratorNeighbors<VoxelType> srcIterNeighbors( * m_pSrcVolume, Vector3D<int4>(0,0,0), m_eNeighborhood );
  vxBlockVolumeIterator<VoxelType> srcIter( * m_pSrcVolume );
  vxBlockVolumeIterator<VoxelType> dstIter( * m_pDstVolume );

  // find the scaling fators to backproject a voxel from dst vol to src vol
  Triplef fSrcUnits( m_pSrcVolume->GetHeader().GetVoxelUnits() );
  Triplef fDstUnits( m_pDstVolume->GetHeader().GetVoxelUnits() );
  Triplef fScaleVec( Triplef( Vector3Df( fDstUnits ).GetDividedAnIso( fSrcUnits ) ) );

  // run through all destination volume voxels and set their value to the median value taken from the src volume
  vector<VoxelType> vVoxels( 1 + srcIterNeighbors.GetNumberOfNeighbors() ); // one extra for center voxel
  for ( dstIter.SetPos(0,0,0); dstIter.IsNotAtEnd(); dstIter.NextBlockZYX() )
  {
    for ( ; dstIter.IsNotAtEndOfBlock(); dstIter.NextZYXinsideBlock() )
    {
      Vector3D<float4> fSamplePos(dstIter.GetPosX(), dstIter.GetPosY(), dstIter.GetPosZ());
      fSamplePos.ScaleAnIso(fScaleVec);
      Vector3D<int4> iNeighborhoodCenterPos((int4)(fSamplePos.X()+0.5), (int4)(fSamplePos.Y()+0.5), (int4)(fSamplePos.Z()+0.5)); // rounded from vfSamplePos
      VoxelType medianValue;
      int2 iCount(0);
      for ( srcIterNeighbors.CenterAt(iNeighborhoodCenterPos); !srcIterNeighbors.IsAtEnd(); ++srcIterNeighbors )
        vVoxels[iCount++] = srcIterNeighbors.GetVoxel();

      vVoxels[iCount] = srcIterNeighbors.GetVoxel();
      medianValue = SelectMedianValue(vVoxels);
      dstIter.SetVoxel(medianValue);
    }
  }
} // ApplyFilter()


// $Log: vxBlockVolumeFilterMedian.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2003/05/22 14:37:33  frank
// code review
//
// Revision 1.12  2003/05/16 11:54:11  michael
// code review
//
// Revision 1.11  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.10  2003/05/15 15:03:46  frank
// spelling
//
// Revision 1.9  2003/01/22 21:35:44  ingmar
// dimension and units are now Triples
//
// Revision 1.8  2002/05/20 15:49:45  soeren
// removed redundant arguments from SelectMedianValue()
//
// Revision 1.7  2002/05/13 21:37:30  ingmar
// median filetering test now working
//
// Revision 1.6  2002/05/13 20:13:20  ingmar
// renamed FindMedianValue() to SelectMedianValue()
//
// Revision 1.5  2002/05/09 22:22:59  ingmar
// removed bool instantiations to avoid compiler warnings
//
// Revision 1.4  2002/05/07 18:04:08  rekha
// changed findMedianValue() typo to FindMedianValue()
//
// Revision 1.3  2002/05/07 18:00:14  rekha
// completed method ApplyFilter()
//
// Revision 1.2  2002/04/25 17:59:07  ingmar
// skeleton for median filter using neighborhood iterators
//
// Revision 1.1  2002/04/25 17:22:08  ingmar
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeFilterMedian.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxBlockVolumeFilterMedian.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $