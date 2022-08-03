// $Id: vxBlockVolumeFilterLinear.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// includes
#include "stdafx.h"
#include "vxBlockVolumeFilterLinear.h"
#include "vxBlockVolumeIterator.h"


// explicit instantiations
template class vxBlockVolumeFilterLinear< int1>;
template class vxBlockVolumeFilterLinear< int2>;
template class vxBlockVolumeFilterLinear< int4>;
template class vxBlockVolumeFilterLinear<uint1>;
template class vxBlockVolumeFilterLinear<uint2>;
template class vxBlockVolumeFilterLinear<uint4>;


// namespaces
using namespace std;


/**
 * Apply Tri-linear filtering to create the destination volume.
 */
template <class VoxelType> 
void vxBlockVolumeFilterLinear<VoxelType>::ApplyFilter()
{
  // create iterators for the src and dst volumes
  vxBlockVolumeIterator<VoxelType> srcIter( * m_pSrcVolume );
  vxBlockVolumeIterator<VoxelType> dstIter( * m_pDstVolume );

  // find the scaling fators to backproject a voxel from dst vol to src vol
  Triplef fSrcUnits( m_pSrcVolume->GetHeader().GetVoxelUnits() );
  Triplef fDstUnits( m_pDstVolume->GetHeader().GetVoxelUnits() );
  Triplef fScaleVec( Triplef( Vector3Df( fDstUnits ).GetDividedAnIso( fSrcUnits ) ) );

  // run through all destination vloume voxels and set their value to the sample value takes from the src volume
  for ( dstIter.SetPos(0,0,0); dstIter.IsNotAtEnd(); dstIter.NextBlockZYX() )
  {
    for ( ; dstIter.IsNotAtEndOfBlock(); dstIter.NextZYXinsideBlock())
    {
      Vector3D<float4> fSamplePos(dstIter.GetPosX(), dstIter.GetPosY(), dstIter.GetPosZ());
      fSamplePos.ScaleAnIso(fScaleVec);
      dstIter.SetVoxel( srcIter.GetSample(fSamplePos) );
    }
  }

} // ApplyFilter()


// $Log: vxBlockVolumeFilterLinear.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.14  2003/05/22 14:37:33  frank
// code review
//
// Revision 1.13  2003/05/16 11:48:14  michael
// code review
//
// Revision 1.12  2003/05/15 15:03:46  frank
// spelling
//
// Revision 1.11  2003/01/22 21:35:44  ingmar
// dimension and units are now Triples
//
// Revision 1.10  2002/05/17 13:46:48  ingmar
// removed inefficient operator ++
//
// Revision 1.9  2002/05/13 22:13:30  ingmar
// fixed comments
//
// Revision 1.8  2002/05/13 21:45:18  ingmar
// Tri-Linear filtering now working
//
// Revision 1.7  2002/05/13 20:23:07  ingmar
// renamed variables according to coding conventions
//
// Revision 1.6  2002/05/09 22:22:59  ingmar
// removed bool instantiations to avoid compiler warnings
//
// Revision 1.5  2002/05/07 17:57:40  rekha
// changed variable names as per convention
//
// Revision 1.4  2002/04/25 17:17:24  ingmar
// fixed typo
//
// Revision 1.3  2002/04/25 17:17:01  ingmar
// added comments
//
// Revision 1.2  2002/04/25 17:12:36  ingmar
// implemented ApplyFilter()
//
// Revision 1.1  2002/04/24 19:52:12  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeFilterLinear.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxBlockVolumeFilterLinear.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
