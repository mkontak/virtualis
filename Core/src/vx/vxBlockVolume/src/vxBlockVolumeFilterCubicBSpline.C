// $Id: vxBlockVolumeFilterCubicBSpline.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Wenli {ingmar|wenli}@viatronix.com


// includes
#include "stdafx.h"
#include "vxBlockVolumeFilterCubicBSpline.h"
#include "vxBlockVolumeFilterKernel.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


// exlicit instantiations
template class vxBlockVolumeFilterCubicBSpline< int1>;
template class vxBlockVolumeFilterCubicBSpline< int2>;
template class vxBlockVolumeFilterCubicBSpline< int4>;
template class vxBlockVolumeFilterCubicBSpline<uint1>;
template class vxBlockVolumeFilterCubicBSpline<uint2>;
template class vxBlockVolumeFilterCubicBSpline<uint4>;


// namespaces
using namespace std;


/**
 * Standard constructor
 *
 * @param eNeighborhood the neighborhood
 */
template <class VoxelType> 
vxBlockVolumeFilterCubicBSpline<VoxelType>::vxBlockVolumeFilterCubicBSpline( vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood )
: m_eNeighborhood( eNeighborhood )
{
} // vxBlockVolumeFilterCubicBSpline()


/**
 * Copy constructor
 *
 * @param other the other filter
 */
template <class VoxelType> 
vxBlockVolumeFilterCubicBSpline<VoxelType>::vxBlockVolumeFilterCubicBSpline( vxBlockVolumeFilterCubicBSpline<VoxelType> & other )
: vxBlockVolumeFilter<VoxelType>( other ),
m_eNeighborhood( other.m_eNeighborhood )
{
} // Copy constructor



/**
 * Run the filter
 */
template <class VoxelType> 
void vxBlockVolumeFilterCubicBSpline<VoxelType>::ApplyFilter()
{
  // create iterators for the src and dst volumes
  vxBlockVolumeIteratorNeighbors<VoxelType> srcIterNeighbors(*m_pSrcVolume, Vector3D<int4>(0,0,0), m_eNeighborhood);
  vxBlockVolumeIterator<VoxelType> srcIter(*m_pSrcVolume);
  vxBlockVolumeIterator<VoxelType> dstIter(*m_pDstVolume);

  // make two intermediate volumes
  vxBlockVolume<float4> xVol(m_pSrcVolume->GetHeader());
  vxBlockVolume<float4> yVol(m_pSrcVolume->GetHeader());
  
  Triplei srcVolDim = m_pSrcVolume->GetHeader().GetVolDim();
  xVol.GetHeader().SetVolDim(m_pDstVolume->GetHeader().GetVolDim().X(),srcVolDim.Y(),srcVolDim.Z());
  yVol.GetHeader().SetVolDim(m_pDstVolume->GetHeader().GetVolDim().X(),m_pDstVolume->GetHeader().GetVolDim().Y(),srcVolDim.Z());

  vxBlockVolumeIterator<float4> xVolIter(xVol);
  vxBlockVolumeIterator<float4> yVolIter(yVol);


  // find the scaling fators to backproject a voxel from dst vol to src vol
  Triplef fSrcUnits = m_pSrcVolume->GetHeader().GetVoxelUnits();
  Triplef fDstUnits = m_pDstVolume->GetHeader().GetVoxelUnits();
  Triplef fScaleVec = fDstUnits.GetDividedAnIso(fSrcUnits);

  // filter kernels
  vector< vxBlockVolumeFilterKernel > xKernel; 
  vxBlockVolumeFilterKernel::GetKernelCubicBSpline(xKernel, fScaleVec.X(), m_pSrcVolume->GetHeader().GetVolDim().X());
  vector< vxBlockVolumeFilterKernel > yKernel; 
  vxBlockVolumeFilterKernel::GetKernelCubicBSpline(yKernel, fScaleVec.Y(), m_pSrcVolume->GetHeader().GetVolDim().Y());
  vector< vxBlockVolumeFilterKernel > zKernel; 
  vxBlockVolumeFilterKernel::GetKernelCubicBSpline(zKernel, fScaleVec.Z(), m_pSrcVolume->GetHeader().GetVolDim().Z());

  //// three passes, one for each kernel

  // xKernel: run through all xVol volume voxels and set their value to the xKernel convolution result
  for ( xVolIter.SetPos(0,0,0); xVolIter.IsNotAtEnd(); xVolIter.NextBlockZYX() )
  {
    for ( ; xVolIter.IsNotAtEndOfBlock(); xVolIter.NextZYXinsideBlock() )
    {
      float4 fSum = 0;
      vxBlockVolumeFilterKernel & thisKernel = xKernel[xVolIter.GetPosX()];
      int4 iFilterSizeX = thisKernel.m_vfWeight.size();

      srcIter.SetPos(thisKernel.m_ifirstPos, xVolIter.GetPos().Y(), xVolIter.GetPos().Z() );
      for ( int4 iCount(0); iCount < iFilterSizeX; iCount++, srcIter.IncX() )
      {
        fSum += thisKernel.m_vfWeight[iCount] * srcIter.GetVoxel();
      }
      xVolIter.SetVoxel(fSum);
    }
  }

  // yKernel: run through all yVol volume voxels and set their value to the yKernel convolution result
  for ( yVolIter.SetPos(0,0,0); yVolIter.IsNotAtEnd(); yVolIter.NextBlockZYX() )
  {
    for ( ; yVolIter.IsNotAtEndOfBlock(); yVolIter.NextZYXinsideBlock() )
    {
      float4 fSum = 0;
      vxBlockVolumeFilterKernel & thisKernel = yKernel[yVolIter.GetPosY()];
      int4 iFilterSizeY = thisKernel.m_vfWeight.size();

      xVolIter.SetPos(yVolIter.GetPos().X(), thisKernel.m_ifirstPos, yVolIter.GetPos().Z() );
      for ( int4 iCount(0); iCount < iFilterSizeY; iCount++, xVolIter.IncY() )
      {
        fSum += thisKernel.m_vfWeight[iCount] * xVolIter.GetVoxel();
      }
      yVolIter.SetVoxel(fSum);
    }
  }

  // zKernel: run through all dstVol volume voxels and set their value to the zKernel convolution result
  for ( dstIter.SetPos(0,0,0); dstIter.IsNotAtEnd(); dstIter.NextBlockZYX() )
  {
    for ( ; dstIter.IsNotAtEndOfBlock(); dstIter.NextZYXinsideBlock() )
    {
      float4 fSum = 0;
      vxBlockVolumeFilterKernel & thisKernel = zKernel[dstIter.GetPosZ()];
      int4 iFilterSizeZ = thisKernel.m_vfWeight.size();

      yVolIter.SetPos(dstIter.GetPos().X(), dstIter.GetPos().Y(), thisKernel.m_ifirstPos);
      for ( int4 iCount(0); iCount < iFilterSizeZ; iCount++, yVolIter.IncZ() )
      {
        fSum += thisKernel.m_vfWeight[iCount] * yVolIter.GetVoxel();
      }
      dstIter.SetVoxel(static_cast<VoxelType>(fSum));
    }
  }
} // ApplyFilter()


#undef FILE_REVISION


// $Log: vxBlockVolumeFilterCubicBSpline.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/05/22 14:37:33  frank
// code review
//
// Revision 1.7  2003/05/16 11:48:14  michael
// code review
//
// Revision 1.6  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.5  2003/05/15 15:03:46  frank
// spelling
//
// Revision 1.4  2003/01/22 21:35:44  ingmar
// dimension and units are now Triples
//
// Revision 1.3  2002/06/05 11:37:40  soeren
// removed non const GetVolDim
//
// Revision 1.2  2002/05/24 20:37:49  wenli
// Cubic BSpline Filter to resample the volume
//
// Revision 1.1  2002/05/22 23:39:15  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeFilterCubicBSpline.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxBlockVolumeFilterCubicBSpline.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
