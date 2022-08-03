// $Id: vxDeciderVoxelValue.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

#ifndef vxDeciderVoxelValue_h
#define vxDeciderVoxelValue_h

// class definition
template <class VoxelType>
class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValue
{
// functions
public:

  /// constructor
  vxDeciderVoxelValue() {}

  /// desctructor
  virtual ~vxDeciderVoxelValue() {}

  /// is part of region?
  virtual bool IsPartOfRegion ( const VoxelType & voxel) const = 0;
}; // vxDeciderVoxelValue


// class definition
/////////////////////
template <class VoxelType> 
class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueLess : public vxDeciderVoxelValue<VoxelType> 
{
// functions
public:

  vxDeciderVoxelValueLess(const VoxelType & threshold = 1)
    : m_threshold(threshold) {};

  /// is part of region?
  bool IsPartOfRegion(const VoxelType & voxel) const;

// data
private:

  /// threshold
  VoxelType m_threshold;
}; // vxDeciderVoxelValueLess


// class definition
/////////////////////
template <class VoxelType> 
class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueLessEqual : public vxDeciderVoxelValue<VoxelType> 
{
// functions
public:

  /// constructor
  vxDeciderVoxelValueLessEqual(const VoxelType & threshold = 1)
    : m_threshold(threshold) {};

  /// is part of region?
  bool IsPartOfRegion(const VoxelType & voxel) const;

// data
private:

  /// threshold
  VoxelType m_threshold;
}; // vxDeciderVoxelValueLessEqual


// class definition
/////////////////////
template <class VoxelType> 
class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreater : public vxDeciderVoxelValue<VoxelType> 
{
// functions
public:

  /// constructor
  vxDeciderVoxelValueGreater(const VoxelType & threshold = 1)
    : m_threshold(threshold) {};

  /// is part of region?
  bool IsPartOfRegion(const VoxelType & voxel) const;

// data
private:

  /// threshold
  VoxelType m_threshold;
}; // vxDeciderVoxelValueGreater


// class definition
/////////////////////
template <class VoxelType> 
class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueGreaterEqual : public vxDeciderVoxelValue<VoxelType> 
{
// functions
public:

  /// constructor
  vxDeciderVoxelValueGreaterEqual(const VoxelType & threshold = 1)
    : m_threshold(threshold) {};

  /// is part of region?
  bool IsPartOfRegion(const VoxelType & voxel) const;

// data
private:

  /// threshold
  VoxelType m_threshold;
}; // vxDeciderVoxelValueGreaterEqual


// class definition
/////////////////////
template <class VoxelType> 
class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueEqual : public vxDeciderVoxelValue<VoxelType> 
{
// functions
public:

  /// constructor
  vxDeciderVoxelValueEqual(const VoxelType & value = 1)
    : m_value(value) {};

  /// is part of region?
  bool IsPartOfRegion(const VoxelType & voxel) const;

// data
private:

  /// threshold
  VoxelType m_value;
}; // vxDeciderVoxelValueEqual


// class definition
/////////////////////
template <class VoxelType> 
class VX_BLOCK_VOLUME_DLL vxDeciderVoxelValueRange : public vxDeciderVoxelValue<VoxelType> 
{
// functions
public:
  
  /// constructor
  vxDeciderVoxelValueRange(const VoxelType & low, const VoxelType & high)
    : m_valueLow(low),
      m_valueHigh(high) {};

  /// is part of region?
  bool IsPartOfRegion(const VoxelType & voxel) const;

// data
private:

  /// low value
  VoxelType m_valueLow;

  // high value
  VoxelType m_valueHigh;
}; // vxDeciderVoxelValueRange


#endif // vxDeciderVoxelValue_h


// $Log: vxDeciderVoxelValue.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/03 16:18:21  michael
// completed code review
//
// Revision 1.4  2002/08/21 15:43:08  frank
// Exported classes to DLL.
//
// Revision 1.3  2002/05/22 20:50:23  ingmar
// added sectioning for doxygen documentation
//
// Revision 1.2  2002/05/09 22:23:29  ingmar
// removed =0 to avoid compiler warnings
//
// Revision 1.1  2002/04/30 13:39:16  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxDeciderVoxelValue.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxDeciderVoxelValue.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
