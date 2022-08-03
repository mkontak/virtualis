// $Id: vxDecider.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

/**
 *  Decider is to define the rule for growing a voxel
 */

#ifndef vxDecider_h
#define vxDecider_h


// includes
#include "vxBlockVolumeIteratorNeighbors.h"


// class definition
template <class VoxelType>
class vxDecider 
{
// functions
public:

  /// constructor
  vxDecider() {}

  /// destructor
  virtual ~vxDecider() {}

  /// is part of region
  virtual bool IsPartOfRegion ( const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const = 0;
}; // vxDecider


// class definition
/////////////////////
template <class VoxelType> 
class vxDeciderThreshLess : public vxDecider<VoxelType> 
{
// functions
public:

  /// constructor
  vxDeciderThreshLess(const VoxelType & threshold = 1)
    : m_threshold(threshold) {}

  /// is part of region
  bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const;

// data
private:

  // voxel threshold
  VoxelType m_threshold;
}; // vxDeciderThreshLess()


// class definition
/////////////////////
template <class VoxelType> 
class vxDeciderThreshLessEqual : public vxDecider<VoxelType> 
{
// functions
public:

  /// constructor
  vxDeciderThreshLessEqual(const VoxelType & threshold = 1)
    : m_threshold(threshold) {}

  /// is part of region
  bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const;

// data
private:

  // voxel threshold
  VoxelType m_threshold;
}; // vxDeciderThreshLessEqual()


// class definition
/////////////////////
template <class VoxelType> 
class vxDeciderThreshGreater : public vxDecider<VoxelType> 
{
// functions
public:

  /// constructor
  vxDeciderThreshGreater(const VoxelType & threshold = 1)
    : m_threshold(threshold) {}

  /// is part of region
  bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const;

// data
private:

  // voxel threshold
  VoxelType m_threshold;
}; // vxDeciderThreshGreater()


// class definition
/////////////////////
template <class VoxelType> 
class vxDeciderThreshGreaterEqual : public vxDecider<VoxelType> 
{
// functions
public:

  /// constructor
  vxDeciderThreshGreaterEqual(const VoxelType & threshold = 1)
    : m_threshold(threshold) {}

  /// is part of region
  bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const;

// data
private:

  // voxel threshold
  VoxelType m_threshold;
}; // vxDeciderThreshGreaterEqual()


// class definition
/////////////////////
template <class VoxelType> 
class vxDeciderEqual : public vxDecider<VoxelType> 
{
// functions
public:

  /// constructor
  vxDeciderEqual(const VoxelType & value = 1)
    : m_value(value) {}

  /// is part of region
  bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const;

// data
private:

  // voxel value
  VoxelType m_value;
}; // vxDeciderEqual()


// class definition
/////////////////////
template <class VoxelType> 
class vxDeciderRange : public vxDecider<VoxelType> 
{
// functions
public:

  /// constructor
  vxDeciderRange(const VoxelType & low, const VoxelType & high)
    : m_valueLow(low),
      m_valueHigh(high) {}

  /// is part of region
  bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const;

// data
private:

  /// lower bound
  VoxelType m_valueLow;

  // upper bound
  VoxelType m_valueHigh;
}; // vxDeciderRange()


// class definition
/////////////////////
template <class VoxelType> 
class vxDeciderSphere : public vxDecider<VoxelType> 
{
// functions
public:

  /// constructor
  vxDeciderSphere(const Vector3D<int4> & startPos, const float4 & fRadius = 1, const Triple<float4> & scaleUnisoVec = Triple<float4>(1,1,1))
    : m_startPos(startPos),
      m_fRadiusSquare(fRadius*fRadius),
      m_scaleUnisoVec(scaleUnisoVec) {}

  /// is part of region
  bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & iter) const;

// data
private:

  /// start position
  Vector3D<int4> m_startPos;

  /// sphere radius
  float4 m_fRadiusSquare;

  /// scale unisotropic vector
  Triple<float4> m_scaleUnisoVec;
}; // vxDeciderSphere()


/*         LEAVE THE FOLLOWING IN SINCE WE WILL NEED TO INCLUDE IT AGAIN AND FIND THE BUG


template <class Iter, class Vox, class InRegionvxDecider > class vxDeciderOutsideSurface6 : public vxDecider<VoxelType> {
  InRegionvxDecider m_inRegionvxDecider;
public:
  vxDeciderOutsideSurface6(const InRegionvxDecider & inRegionvxDecider)
    :m_inRegionvxDecider(inRegionvxDecider) {}
  inline bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & dataIter) const;
  static bool Test (std::ostream & os = std::cout);
}; // vxDeciderOutsideSurface6


template <class Iter,  class Vox, class InRegionvxDecider > class vxDeciderOutsideSurface18 : public vxDecider<VoxelType> {
  InRegionvxDecider m_inRegionvxDecider;
public:
  vxDeciderOutsideSurface18(const InRegionvxDecider & inRegionvxDecider)
    :m_inRegionvxDecider(inRegionvxDecider) {}
  inline bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & dataIter) const;
  static bool Test (std::ostream & os = std::cout);
}; // vxDeciderOutsideSurface18


template <class Iter,  class Vox, class InRegionvxDecider > class vxDeciderOutsideSurface26 : public vxDecider<VoxelType> {
  InRegionvxDecider m_inRegionvxDecider;
public:
  vxDeciderOutsideSurface26(const InRegionvxDecider & inRegionvxDecider)
    :m_inRegionvxDecider(inRegionvxDecider) {}
  inline bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & dataIter) const;
  static bool Test (std::ostream & os = std::cout);
}; // vxDeciderOutsideSurface26


template <class Iter, class Vox, class InRegionvxDecider > class vxDeciderInsideSurface6 : public vxDecider<VoxelType> {
  InRegionvxDecider m_inRegionvxDecider;
public:
  vxDeciderInsideSurface6(const InRegionvxDecider & inRegionvxDecider)
    :m_inRegionvxDecider(inRegionvxDecider) {}
  inline bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & dataIter) const;
  static bool Test (std::ostream & os = std::cout);
}; // vxDeciderInsideSurface6


template <class Iter, class Vox, class InRegionvxDecider > class vxDeciderInsideSurface18 : public vxDecider<VoxelType> {
  InRegionvxDecider m_inRegionvxDecider;
public:
  vxDeciderInsideSurface18(const InRegionvxDecider & inRegionvxDecider)
    :m_inRegionvxDecider(inRegionvxDecider) {}
  inline bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & dataIter) const;
  static bool Test (std::ostream & os = std::cout);
}; // vxDeciderInsideSurface18


template <class Iter, class Vox, class InRegionvxDecider > class vxDeciderInsideSurface26 : public vxDecider<VoxelType> {
  InRegionvxDecider m_inRegionvxDecider;
public:
  vxDeciderInsideSurface26(const InRegionvxDecider & inRegionvxDecider)
    :m_inRegionvxDecider(inRegionvxDecider) {}
  inline bool IsPartOfRegion(const vxBlockVolumeIteratorNeighbors<VoxelType> & dataIter) const;
  static bool Test (std::ostream & os = std::cout);
}; // vxDeciderInsideSurface26


#include "vxDecider.inl"
*/


#endif // vxDecider_h


// $Log: vxDecider.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/12/22 16:20:58  michael
// worked on the DeciderSphere to take a radius that is float since it needs to
// be scaled anisotropically with the spacing of the volume which cannot be
// handled properlin on an integer basis.
//
// Revision 1.9  2003/05/03 16:18:21  michael
// completed code review
//
// Revision 1.8  2003/05/02 14:36:57  dongqing
// code walk through
//
// Revision 1.7  2003/01/22 21:30:48  ingmar
// dimension and units are now Triples
//
// Revision 1.6  2001/01/01 06:07:52  ingmar
// made the decider sphere consider unisotropic scaling of the volume data
//
// Revision 1.5  2002/05/22 23:40:04  ingmar
// added comments
//
// Revision 1.4  2002/05/22 20:50:23  ingmar
// added sectioning for doxygen documentation
//
// Revision 1.3  2002/04/26 12:53:13  ingmar
// new deciders and region grow iterators based only on a single template parameter, the VoxelType
//
// Revision 1.2  2002/04/23 03:19:38  soeren
// enabled DeciderSphere
//
// Revision 1.1  2002/04/22 19:54:01  soeren
// intial version of Regiongrow iterator and decider
//
// Revision 3.3  2001/12/24 18:13:00  ingmar
// $Id: vxDecider.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
