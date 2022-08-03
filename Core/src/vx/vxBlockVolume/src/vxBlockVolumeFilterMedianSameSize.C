// $Id: vxBlockVolumeFilterMedianSameSize.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dongqing Chen dongqing@viatronix.com


// includes
#include "stdafx.h"
#include "vxBlockVolumeFilterMedianSameSize.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"

// explicit instantiations
template class vxBlockVolumeFilterMedianSameSize< int1>;
template class vxBlockVolumeFilterMedianSameSize< int2>;
template class vxBlockVolumeFilterMedianSameSize< int4>;
template class vxBlockVolumeFilterMedianSameSize<uint1>;
template class vxBlockVolumeFilterMedianSameSize<uint2>;
template class vxBlockVolumeFilterMedianSameSize<uint4>;


// namespaces
using namespace std;

// defines
#define FILE_REVISION  "$Revision: 1.1.1.1 $"

/**
 * Standard constructor
 *
 * @param eNeighborhood the neighborhood
 */
template <class VoxelType> 
vxBlockVolumeFilterMedianSameSize<VoxelType>::vxBlockVolumeFilterMedianSameSize( vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood )
: m_eNeighborhood( eNeighborhood )
{

  vxBlockVolume<uint1> labelVol( vxVolumeHeader( Triple<int4>(12,12,12) ) );
  m_pLabelVolume = & labelVol;
  m_uLabelIndexOfROI = 0x00;

} // vxBlockVolumeFilterMedian()


/**
 * Copy constructor
 *
 * @param other the other filter
 */
template <class VoxelType> 
vxBlockVolumeFilterMedianSameSize<VoxelType>::vxBlockVolumeFilterMedianSameSize( vxBlockVolumeFilterMedianSameSize<VoxelType> & other )
: vxBlockVolumeFilter<VoxelType>( other ),
m_eNeighborhood( other.m_eNeighborhood ),
m_pLabelVolume( other.m_pLabelVolume )
{
} // Copy constructor


/**
 * Run the filter
 */
template <class VoxelType> 
void vxBlockVolumeFilterMedianSameSize<VoxelType>::ApplyFilter()
{
  if( m_pSrcVolume->GetHeader().GetVolDim() != m_pLabelVolume->GetHeader().GetVolDim() )
  {
    throw ex::VException(LogRec("The sizes of source and label volumes are not match", "vxBlockVolumeFilterMedianSameSize", "ApplyFilter"));
  }

  // create iterators for the src and dst volumes
  vxBlockVolumeIteratorNeighbors<VoxelType> srcIterNeighbors( * m_pSrcVolume, Vector3D<int4>(0,0,0), m_eNeighborhood );
  vxBlockVolumeIterator<VoxelType> srcIter( * m_pSrcVolume );
  vxBlockVolumeIterator<VoxelType> dstIter( * m_pDstVolume );
  vxBlockVolumeIterator<uint1>     labIter( * m_pLabelVolume );

  // run through all destination volume voxels and set their value to the median value taken from the src volume
  vector<VoxelType> vVoxels( 1 + srcIterNeighbors.GetNumberOfNeighbors() ); // one extra for center voxel
  int4 iMedianIndex;

  for ( dstIter.SetPos(0,0,0); dstIter.IsNotAtEnd(); dstIter.NextBlockZYX() )
  {
    for ( ; dstIter.IsNotAtEndOfBlock(); dstIter.NextZYXinsideBlock() )
    {

      Vector3D<int4> iNeighborhoodCenterPos( dstIter.GetPos() );
      labIter.SetPos( iNeighborhoodCenterPos );
      if( labIter.GetVoxel() == m_uLabelIndexOfROI )
      {
        int2 iCount( 0 );

        for ( srcIterNeighbors.CenterAt(iNeighborhoodCenterPos); !srcIterNeighbors.IsAtEnd(); ++srcIterNeighbors )
          vVoxels[iCount++] = srcIterNeighbors.GetVoxel();

        vVoxels[iCount] = srcIterNeighbors.GetVoxel();
        iMedianIndex = (iCount + 1)/2;
        nth_element(vVoxels.begin(), vVoxels.begin()+iMedianIndex , vVoxels.end());
        dstIter.SetVoxel( vVoxels[iMedianIndex] );
      }

    } // for( inside block )
  } // for( block-wise )
} // ApplyFilter()

#undef FILE_REVISION

// $Log: vxBlockVolumeFilterMedianSameSize.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/07/24 18:23:29  dongqing
// add label volume
//
// Revision 1.1  2003/07/23 19:54:39  dongqing
// new
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeFilterMedianSameSize.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxBlockVolumeFilterMedianSameSize.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $