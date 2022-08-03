// $Id: vxDecider.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// includes
#include "StdAfx.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxDecider.h"


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


// partial template instatiations
template class VX_BLOCK_VOLUME_DLL vxDeciderEqual<uint1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderEqual<uint2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderEqual<uint4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderEqual< int1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderEqual< int2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderEqual< int4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderEqual< bool>;
template class VX_BLOCK_VOLUME_DLL vxDeciderEqual<float4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderThreshLess<uint1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderThreshLess<uint2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderThreshLess<uint4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderThreshLess< int1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderThreshLess< int2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderThreshLess< int4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderThreshLess< bool>;
template class VX_BLOCK_VOLUME_DLL vxDeciderThreshLess<float4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderRange<uint1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderRange<uint2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderRange<uint4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderRange< int1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderRange< int2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderRange< int4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderRange< bool>;
template class VX_BLOCK_VOLUME_DLL vxDeciderRange< float4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderSphere<uint1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderSphere<uint2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderSphere<uint4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderSphere< int1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderSphere< int2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderSphere< int4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderSphere< bool>;
template class VX_BLOCK_VOLUME_DLL vxDeciderSphere< float4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderThreshGreaterEqual<int2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderThreshGreaterEqual<uint2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderThreshGreater<int2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderThreshGreater<uint2>;


// namespaces
using namespace std;


/**
 * Is part of region
 * @param iter
 * @return true if intensity less than threshold
 */
template <class VoxelType> 
bool vxDeciderThreshLess<VoxelType>::IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const 
{
  return iter.GetVoxel() < m_threshold;
}


/**
 * Is part of region
 * @param iter
 * @return true if intensity less than threshold
 */
template <class VoxelType> 
bool vxDeciderThreshLessEqual<VoxelType>::IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const 
{
  return iter.GetVoxel() <= m_threshold;
}


/**
 * Is part of region
 * @param iter
 * @return true if intensity less than threshold
 */
template <class VoxelType> 
bool vxDeciderThreshGreater<VoxelType>::IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const 
{
  return iter.GetVoxel() > m_threshold;
}


/**
 * Is part of region
 * @param iter
 * @return true if intensity less than threshold
 */
template <class VoxelType> 
bool vxDeciderThreshGreaterEqual<VoxelType>::IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const 
{
  if (m_threshold < 1)
  {
    LogDbg("m_threshold = " + ToAscii( m_threshold ), "vxDeciderThreshGreaterEqual",  "IsPartOfRegion" );
  }

  return iter.GetVoxel() >= m_threshold;
}


/**
 * Is part of region
 * @param iter
 * @return true if intensity less than threshold
 */
template <class VoxelType> 
bool vxDeciderEqual<VoxelType>::IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const 
{
  return iter.GetVoxel() == m_value;
}


/**
 * Is part of region
 * @param iter
 * @return true if intensity less than threshold
 */
template <class VoxelType> 
bool vxDeciderRange<VoxelType>::IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const 
{
  if (m_valueLow < 1)
  {
    LogDbg("m_valueHigh = " + ToAscii( m_valueHigh ), "vxDeciderRange",  "IsPartOfRegion");
  }
  return m_valueLow <= iter.GetVoxel() && iter.GetVoxel() <= m_valueHigh;
}


/**
 * Is part of region
 * @param iter
 * @return true if intensity less than threshold
 */
template <class VoxelType> 
bool vxDeciderSphere<VoxelType>::IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const 
{
  Vector3D<float4> diffVec(m_startPos-Vector3D<int4>(iter.GetPos().X(),
                                                     iter.GetPos().Y(),
                                                     iter.GetPos().Z()));
  diffVec.ScaleAnIso(m_scaleUnisoVec);
  return diffVec.GetLengthSquared() < m_fRadiusSquare;
}


#undef FILE_REVISION


// Revision History:
// $Log: vxDecider.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.17  2003/12/22 16:20:58  michael
// worked on the DeciderSphere to take a radius that is float since it needs to
// be scaled anisotropically with the spacing of the volume which cannot be
// handled properlin on an integer basis.
//
// Revision 1.16  2003/06/18 15:19:48  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.15  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.14  2003/05/03 16:18:21  michael
// completed code review
//
// Revision 1.13  2003/05/02 14:36:57  dongqing
// code walk through
//
// Revision 1.12  2003/03/28 17:17:08  wenli
// update the region growing
//

// Revision 1.11  2003/01/22 21:30:47  ingmar
// dimension and units are now Triples
//
// Revision 1.10  2001/01/01 06:07:52  ingmar

// made the decider sphere consider unisotropic scaling of the volume data
//
// Revision 1.9  2002/09/17 17:17:55  dongqing
// Tiny bug fixing and comments after testing on the Skeletonator Module
//
// Revision 1.8  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.7  2002/06/07 14:58:56  geconomos
// Added instaniation for vxDeciderThreshGreaterEqual<int2>.
//
// Revision 1.6  2002/05/22 23:40:30  ingmar
// code beautify
//
// Revision 1.5  2002/04/26 13:53:35  ingmar
// activated vxDeciderEqual
//
// Revision 1.4  2002/04/26 12:53:13  ingmar
// new deciders and region grow iterators based only on a single template parameter, the VoxelType
//
// Revision 1.3  2002/04/25 22:03:49  jaddonisio
// Instantiate deciders.
//
// Revision 1.2  2002/04/23 03:19:38  soeren
// enabled DeciderSphere
//
// Revision 1.1  2002/04/22 19:54:01  soeren
// intial version of Regiongrow iterator and decider
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxDecider.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxDecider.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
