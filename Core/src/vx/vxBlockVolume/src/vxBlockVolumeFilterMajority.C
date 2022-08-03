// $Id: vxBlockVolumeFilterMajority.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar ingmar@viatronix.com


// includes
#include "stdafx.h"
#include "vxBlockVolumeFilterMajority.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"


// explicit instantiations
template class vxBlockVolumeFilterMajority< int1>;
template class vxBlockVolumeFilterMajority< int2>;
template class vxBlockVolumeFilterMajority< int4>;
template class vxBlockVolumeFilterMajority<uint1>;
template class vxBlockVolumeFilterMajority<uint2>;
template class vxBlockVolumeFilterMajority<uint4>;


// namespaces
using namespace std;


/**
 * Standard constructor
 *
 * @param eNeighborhood the neighborhood under surveillance
 */
template <class VoxelType> 
vxBlockVolumeFilterMajority<VoxelType>::vxBlockVolumeFilterMajority( vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood )
: m_eNeighborhood( eNeighborhood )
{
} // vxBlockVolumeFilterMajority()


/**
 * Copy constructor
 *
 * @param other the other filter
 */
template <class VoxelType> 
vxBlockVolumeFilterMajority<VoxelType>::vxBlockVolumeFilterMajority( vxBlockVolumeFilterMajority<VoxelType> & other )
: vxBlockVolumeFilter<VoxelType>( other ),
m_eNeighborhood( other.m_eNeighborhood )
{
} // Copy constructor


/**
 * Run the filter
 */
template <class VoxelType> 
void vxBlockVolumeFilterMajority<VoxelType>::ApplyFilter()
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
      int2 iCount(0);
      for ( srcIterNeighbors.CenterAt(iNeighborhoodCenterPos); !srcIterNeighbors.IsAtEnd(); ++srcIterNeighbors )
      {
        vVoxels[iCount++] = srcIterNeighbors.GetVoxel();
      }
      vVoxels[iCount] = srcIterNeighbors.GetVoxel();
      dstIter.SetVoxel( SelectMajorityValue( vVoxels ) );
    }
  }

} // ApplyFilter()


// $Log: vxBlockVolumeFilterMajority.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/05/22 14:37:33  frank
// code review
//
// Revision 1.5  2003/05/16 11:48:14  michael
// code review
//
// Revision 1.4  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.3  2003/05/15 15:03:46  frank
// spelling
//
// Revision 1.2  2003/01/22 21:35:44  ingmar
// dimension and units are now Triples
//
// Revision 1.1  2002/05/14 23:18:07  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeFilterMajority.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxBlockVolumeFilterMajority.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
