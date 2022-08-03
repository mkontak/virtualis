// $Id: vxVoxelCost.h,v 1.3 2006/04/26 16:40:57 dongqing Exp $
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

#pragma once

#include "Vector3D.h"

/**
 *  A node to store for each inside voxel.
 *
 *  As on March 2006, this class is only employed in the VC Processor codes.
 */
class VX_BLOCK_VOLUME_DLL vxVoxelCost 
{
// Helpful subclasses
public:
  class vxCmpCost 
  {
  public:
    inline bool operator() (vxVoxelCost * a, vxVoxelCost * b) const;
  };

  typedef std::priority_queue<vxVoxelCost *, std::vector<vxVoxelCost *>, vxCmpCost> vxCostHeap;

  class vxCmpEuclidean 
  {
  public:
    inline bool operator() (vxVoxelCost * a, vxVoxelCost * b) const;
  };

  typedef std::priority_queue<vxVoxelCost *, std::vector<vxVoxelCost *>, vxCmpEuclidean> vxEuclideanHeap;

  /// Default constructor.
  vxVoxelCost();

  /// Constructor for vxBlockVolume.
  vxVoxelCost(const uint4 & zeroForEmpty);

  /// Advanced Constructor.
  vxVoxelCost(const float4 & costOfVoxel, const float4 & piecewiseEuclideanDistance, 
	          const float4 & costToRoot,  const Vector3D<int2> & location);

  /// Default deconstructor.
  ~vxVoxelCost();

  /// Equality operator.
  bool operator ==(const vxVoxelCost & other) const;

  /// UnEquality operator.
  bool operator !=(const vxVoxelCost & other) const;

  /// + this is a dummy one doing nothing
  vxVoxelCost operator + (const vxVoxelCost & other) const { return *this; }
  friend vxVoxelCost operator + (const float4    & lhs, const vxVoxelCost & rhs) { return vxVoxelCost(); }
  friend vxVoxelCost operator + (const vxVoxelCost & lhs, const float4    & rhs) { return vxVoxelCost(); }

  /// - this is a dummy one doing nothing
  vxVoxelCost operator - (const vxVoxelCost & other) const { return *this; }

  /// Returns the three data members concatenated in a single string object.
  std::string AsString() const;
  
  /// Assignment operator.
  inline vxVoxelCost & operator =(const int4 & other);

  /// just to make a block volume of these work
  operator int4()
  {
    return 5;
  }

// Data members
public:
  // cost of this voxel from DFB.
  float4 m_costOfVoxel;

  // distance from root voxel (either start or end).
  float4 m_piecewiseEuclideanDistance;

  // accumulated cost from root voxel (either start or end).
  float4 m_costToRoot;

  // the physical location of this voxel.
  Vector3D<int4> m_location;
}; // vxVoxelCost class


/**
 *  @param a
 *  @param b
 *  @return true if cost a bigger than cost b
 */
inline bool vxVoxelCost::vxCmpCost::operator () (vxVoxelCost * a, vxVoxelCost * b) const
{
  return(a->m_costToRoot > b->m_costToRoot);
} // operator ()

/**
 *  @param a
 *  @param b
 *  @return true if distance a bigger than distance b
 */
inline bool vxVoxelCost::vxCmpEuclidean::operator () (vxVoxelCost * a, vxVoxelCost * b) const
{
  return(a->m_piecewiseEuclideanDistance > b->m_piecewiseEuclideanDistance);
} // operator ()


/**
 *  @param other
 *  @return true if copy success
 */
inline bool vxVoxelCost::operator ==(const vxVoxelCost & other) const
{
  if 
  ( m_costOfVoxel                == other.m_costOfVoxel                &&
    m_piecewiseEuclideanDistance == other.m_piecewiseEuclideanDistance &&
    m_costToRoot                 == other.m_costToRoot                 &&
    m_location                   == other.m_location ) 
  {
    return true;
  } 
  else 
  {
    return false;
  }
} // operator () ==


/**
 *  @param other
 *  @return true if comparison success
 */
inline bool vxVoxelCost::operator !=(const vxVoxelCost & other) const
{
  return !(*this == other);
} // operator () !=


/**
 *  @param other
 *  @return true if copy success
 */
inline vxVoxelCost & vxVoxelCost::operator =(const int4 & other)
{
  m_costOfVoxel                = 0;
  m_piecewiseEuclideanDistance = FLT_MAX;
  m_costToRoot                 = FLT_MAX;
  m_location                   = Vector3D<int2>(0,0,0);
  return * this;
} // operator () ==


/**
 *  Overloaded << Operator for vxVoxelCost.
 * 
 *  @param os
 *  @param cost
 *  @return os
 */
inline std::ostream & operator <<(std::ostream & os, const vxVoxelCost & cost)
{
  os << "(" << cost.m_costOfVoxel << ",";
  os << cost.m_piecewiseEuclideanDistance << ",";
  os << cost.m_costToRoot << ",";
  os << cost.m_location << ")";
  return os;
} // operator () <<


// $Log: vxVoxelCost.h,v $
// Revision 1.3  2006/04/26 16:40:57  dongqing
// fix the CSI bug
//
// Revision 1.2  2006/03/09 18:36:32  dongqing
// add vxVoxelCost voxel type to the vxBlockVolume
//
// Revision 1.1  2006/03/09 15:17:04  dongqing
// move vxVoxelCost from Colon Processor to vxBlockVolume
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxVoxelCost.h,v 1.3 2006/04/26 16:40:57 dongqing Exp $
// $Id: vxVoxelCost.h,v 1.3 2006/04/26 16:40:57 dongqing Exp $
