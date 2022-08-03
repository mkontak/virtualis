// $Id: vxVoxelCost.C,v 1.2 2006/04/26 16:40:48 dongqing Exp $
//
// Copyright(c) 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen   (dongqing@viatronix.com)
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "vxVoxelCost.h"

/**
 * Constructor for node.
 */
vxVoxelCost::vxVoxelCost()
{
  m_costOfVoxel                = -1;
  m_piecewiseEuclideanDistance = FLT_MAX; // originally set to infinite
  m_costToRoot                 = FLT_MAX; // originally set to infinite
  m_location                   = Vector3D<int2>(0,0,0);
} // Constructor


/**
 * Constructor for node used in a vxBlockVolume.
 */
vxVoxelCost::vxVoxelCost(const uint4 & zeroForEmpty)
{
  vxVoxelCost();
} // Constructor


/**
 * Advanced constructor.
 */
vxVoxelCost::vxVoxelCost(const float4 & costOfVoxel, const float4 & piecewiseEuclideanDistance, 
						 const float4 & costToRoot,  const Vector3D<int2> & location)
{
  m_costOfVoxel                = costOfVoxel;
  m_piecewiseEuclideanDistance = piecewiseEuclideanDistance;
  m_costToRoot                 = costToRoot;
  m_location                   = location;
} // vxVoxelCost

/**
 * Deconstructor for node structure.
 */
vxVoxelCost::~vxVoxelCost()
{
} // Deconstructor


// $Log: vxVoxelCost.C,v $
// Revision 1.2  2006/04/26 16:40:48  dongqing
// fix the CSI bug
//
// Revision 1.1  2006/03/09 15:16:15  dongqing
// move from Colon Processor to vxBlockVolume
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxVoxelCost.C,v 1.2 2006/04/26 16:40:48 dongqing Exp $
// $Id: vxVoxelCost.C,v 1.2 2006/04/26 16:40:48 dongqing Exp $
