// $Id: vxDeciderVoxelValue.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
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
#include "vxDeciderVoxelValue.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"


// partial template instatiations
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValue<uint1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValue<uint2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValue<uint4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValue< int1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValue< int2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValue< int4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValue< bool>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueEqual<uint1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueEqual<uint2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueEqual<uint4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueEqual< int1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueEqual< int2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueEqual< int4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueEqual< bool>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueLess<uint1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueLess<uint2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueLess<uint4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueLess< int1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueLess< int2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueLess< int4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueLess< bool>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueRange<uint1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueRange<uint2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueRange<uint4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueRange< int1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueRange< int2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueRange< int4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueRange< bool>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreater<uint1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreater<uint2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreater<uint4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreater< int1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreater< int2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreater< int4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreater< bool>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreaterEqual<uint1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreaterEqual<uint2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreaterEqual<uint4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreaterEqual< int1>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreaterEqual< int2>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreaterEqual< int4>;
template class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreaterEqual< bool>;


// namespaces
using namespace std;


/**
 * Is part of region
 * @param voxel
 */
template <class VoxelType> 
bool vxDeciderVoxelValueLess<VoxelType>::IsPartOfRegion(const VoxelType & voxel) const 
{
  return voxel < m_threshold;
}



/**
 * Is part of region
 * @param voxel
 */
template <class VoxelType> 
bool vxDeciderVoxelValueLessEqual<VoxelType>::IsPartOfRegion(const VoxelType & voxel) const 
{
  return voxel <= m_threshold;
}



/**
 * Is part of region
 * @param voxel
 */
template <class VoxelType> 
bool vxDeciderVoxelValueGreater<VoxelType>::IsPartOfRegion(const VoxelType & voxel) const 
{
  return voxel > m_threshold;
}



/**
 * Is part of region
 * @param voxel
 */
template <class VoxelType> 
bool vxDeciderVoxelValueGreaterEqual<VoxelType>::IsPartOfRegion(const VoxelType & voxel) const 
{
  return voxel >= m_threshold; 
}



/**
 * Is part of region
 * @param voxel
 */
template <class VoxelType> 
bool vxDeciderVoxelValueEqual<VoxelType>::IsPartOfRegion(const VoxelType & voxel) const 
{
  return voxel == m_value;
}



/**
 * Is part of region
 * @param voxel
 */
template <class VoxelType> 
bool vxDeciderVoxelValueRange<VoxelType>::IsPartOfRegion(const VoxelType & voxel) const 
{
  return m_valueLow <= voxel && voxel <= m_valueHigh;
}


// Revision History:
// $Log: vxDeciderVoxelValue.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.7  2003/05/03 16:18:21  michael
// completed code review
//
// Revision 1.6  2002/08/21 15:43:08  frank
// Exported classes to DLL.
//
// Revision 1.5  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.4  2002/05/22 23:38:29  ingmar
// code beautify
//
// Revision 1.3  2002/04/30 14:19:28  ingmar
// added explicit instantiations of vxDeciderVoxelValue
//
// Revision 1.2  2002/04/30 13:47:58  ingmar
// completed renaming Intensity to VoxelValue
//
// Revision 1.1  2002/04/30 13:39:13  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxDeciderVoxelValue.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxDeciderVoxelValue.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
