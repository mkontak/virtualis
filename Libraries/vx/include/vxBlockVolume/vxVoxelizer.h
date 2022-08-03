// $Id: vxVoxelizer.h,v 1.1 2006/02/17 15:10:48 frank Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "Triangle.h"
#include "Point3D.h"
#include "Matrix.h"


/**
 * Utility to voxelize a set of triangles into a volume
 */
class VX_BASE_DLL vxVoxelizer
{
// member functions
public:

  /// constructor
  vxVoxelizer();

  /// destructor
  ~vxVoxelizer();

  /// read a set of triangles from disk
  void ReadFileOFF( const std::string & sFilename );

  /// primary voxelization function
  void VoxelizeTriangles( vxBlockVolume<uint2> & volume );

  /// transform the triangles
  void Transform( const Matrix<float4> & transformation );

private:

  /// voxelize a single triangle
  void VoxelizeTriangle( vxBlockVolumeIterator<uint2> & volIter, const Triple<int4> & volumeDimensions, float4 * points );

// data members
private:

  /// a list of triangles that will be voxelized
  std::vector< Point3D< float4 > > m_triangles;

}; // class vxVoxelizer



// $Log: vxVoxelizer.h,v $
// Revision 1.1  2006/02/17 15:10:48  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxVoxelizer.h,v 1.1 2006/02/17 15:10:48 frank Exp $
// $Id: vxVoxelizer.h,v 1.1 2006/02/17 15:10:48 frank Exp $
