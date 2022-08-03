// $Id: vxBlockVolumeRayIterator.h,v 1.3.10.1 2010/07/16 20:31:28 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

#ifndef vxBlockVolumeRayIterator_h
#define vxBlockVolumeRayIterator_h


// includes
#include "vxBlockVolume.h"
#include "vxBlockVolumeIteratorConsts.h"
#include "Vector4d.h"
#include "RaySegment.h"


/********************************************************************

uint8 PackedVolumeOffset

Z = L2 L1 L0 = 5 2 4 = total 11 bits
Y = L2 L1 L0 = 5 2 4 = total 11 bits
X = L2 L1 L0 = 5 2 4 = total 11 bits

uint8 = Z Y X = 11 11 11 = total 33 bits

*********************************************************************/



/**
 * An iterator that incrementally traverses  voxels along a ray.
 */
template <class VoxelType> 
class vxBlockVolumeRayIterator : public vxBlockVolumeIteratorConsts
{
// functions
public:

  /// constructor 
  vxBlockVolumeRayIterator(vxBlockVolume<VoxelType> & vol);

  /// constructor 
  vxBlockVolumeRayIterator(vxBlockVolume<VoxelType> & vol, 
                           const RaySegment & raySegment, 
                           const Vector4Df  & sampleIncrementVec);

  /// advance to the next sample position along the ray
  void operator++();

  /// space leap ones along the ray
  inline void SpaceLeap(float4 fSpaceLeapDist);

  /// space leap ones along the ray
  inline void SpaceLeapInt(int4 iSpaceLeapDist);

  /// space leap in world space distance (millimeters) along the ray
  inline void SpaceLeapMM(float4 fSpaceLeapDist);

  /// space leap ones along the ray
  inline void SpaceLeapMinusHalf();

  /// space leap ones along the ray
  inline void SpaceLeapOne();

  /// space leap ones along the ray
  inline void SpaceLeapTwo();

  /// is true when the traversal is finished
  inline bool IsAtEnd() const { return (m_pos.T() >= m_fRaySegmentEndT); }

  /// set sample distance
  inline void SetRaySegment(const RaySegment & raySegment);

  /// set sample distance
  inline void SetSampleIncrementVector(const Vector4Df & sampleIncrementVec);

  /// compute how far along the ray intersects with the current block
  inline float4 GetDistanceToEndOfBlock() const;

  /// compute how far along the ray intersects with the current small block
  inline float4 GetDistanceToEndOfSmallBlock() const;

  /// returns the voxel the floor of the current position: Unsafe, fast case.
  inline const VoxelType & GetVoxel() const;

  /// returns the value at the current position: Fast version
  inline const int4 GetSampleInt();

  /// returns the value at the current position: Fast version
  inline const float4 & GetSample();

  /// returns the value at the current position: Fast version
  inline const float4 & GetSampleSIMD();

  /// returns the value at the current position: Interpolates the label based on trilinear distance-weighted majority
  inline int4 GetSampleBestLabel();

  /// returns the value at the current position: Chooses the nearest voxel based on trilinear distance
  inline VoxelType GetSampleNearest();

  /// returns true if the iterator is inside the volume.
  inline bool IsInsideVolume() const;

private:

  /// returns the value at the current position: Fast version.
  const float4 & GetSampleElse();

  /// returns the value at the current position: Slow version. Interpolates the label based on trilinear distance-weighted majority.
  int4 GetSampleBestLabelElse();

  /// returns the value at the current position: Slow version.  Chooses the nearest voxel based on trilinear distance.
  VoxelType GetSampleNearestElse();

public:

  /// returns the resampled central difference gradient (fast case)
  inline const Vector3D<float4> & GetSampleGradientCentralDifference();

  /// returns the resampled intermediate difference gradient (fast case)
  inline const Vector3D<float4> & GetSampleGradientImmediateDifference();

  /// returns the resampled Sobel gradient (fast case)
  inline const Vector3D<float4> & GetSampleGradientSobel();

private:

  /// returns the resampled central difference gradient (slow case)
  const Vector3D<float4> & GetSampleGradientCentralDifferenceElse();

  /// returns the resampled intermediate difference gradient (slow case)
  const Vector3D<float4> & GetSampleGradientImmediateDifferenceElse();

  /// returns the resampled Sobel gradient (slow case)
  const Vector3D<float4> & GetSampleGradientSobelElse();

public:

  /// centers the iterator at the location
  __forceinline void SetPos(const Vector4d<float4> & pos);
  
  template <class OtherVoxelType>
  inline void SetPosFromRayIter(const vxBlockVolumeRayIterator<OtherVoxelType> & rayIter)
  {
    m_pos  = rayIter.m_pos;
    m_iPosX = rayIter.m_iPosX;
    m_iPosY = rayIter.m_iPosY;
    m_iPosZ = rayIter.m_iPosZ;
    m_pL1   = m_rVol.m_BlockL2Vol.GetDataPtr() + (rayIter.m_pL1  - rayIter.m_rVol.m_BlockL2Vol.GetDataPtr());
    m_pL0   = m_pL1->GetDataPtr()              + (rayIter.m_pL0  - rayIter.m_pL1->GetDataPtr());
    m_pVox  = m_pL0->GetDataPtr()              + (rayIter.m_pVox - rayIter.m_pL0->GetDataPtr());
  } // SetPosFromRayIter()

  /// returns the current x,y,z position
  inline const Vector4d<float4> & GetPosFloat() const;

  /// returns the current x,y,z position
  inline const Vector3D<int4> GetPosInt() const;

  /// returns the space leap delta vector
  inline const Vector4d<float4> GetSldDeltaVector() const { return m_sldDeltaVector; }

protected:

  /// set direction vector
  inline void SetDirectionVector(const Vector3D<float> & directionVector);

private:

  /// set the space leap increment vector
  inline void SetSldDeltaVector();

  /// update internal integer position (fast case)
  inline void UpdateInternalIntegerPosition();

  /// update internal integer postion (slow case)
  void UpdateInternalIntegerPositionElse();

  /// the trilinear interpolated value
  inline void Trilinear(int4 x, int4 y, int4 z, 
                        const int4 a, const int4 b, const int4 c, const int4 d, 
                        const int4 e, const int4 f, const int4 g, const int4 h, int4 & iFinal) const;

public:

  /// returns the offset that corresponds to the current position
  inline const int4 GetOffsetL2Vol() const;

  /// returns the offset that corresponds to the current position
  inline const int4 GetOffsetL1Blk() const;

  /// returns the offset that corresponds to the current position
  inline const int4 GetOffsetL0Vox() const;

  /// returns the offset that corresponds to the current position
  inline const uint8 GetOffset() const;

  /// returns the offset that corresponds to the given position
  inline const uint8 GetOffset(const int4 iPosX, const int4 iPosY, const int4 iPosZ) const;

public:  

  /// peek relative to current position
  const VoxelType & PeekRelative(int4 idX, int4 idY, int4 idZ) const;

  /// returns the value at the given offset position; Unsafe, fast case
  inline const VoxelType & GetVoxel(const uint8 uOffset) const;

  /// peek x-1, y-1, and z-1
  inline const VoxelType & Peek1nx1ny1nz() const;

  /// peek x, y-1, and z-1
  inline const VoxelType & Peek0px1ny1nz() const;

  /// peek x+1, y-1, and z-1
  inline const VoxelType & Peek1px1ny1nz() const;

  /// peek x+2, y-1, and z-1
  inline const VoxelType & Peek2px1ny1nz() const;


  /// peek x-1, y, and z-1
  inline const VoxelType & Peek1nx0py1nz() const;

  /// peek x, y, and z-1
  inline const VoxelType & Peek0px0py1nz() const;

  /// peek x+1, y, and z-1
  inline const VoxelType & Peek1px0py1nz() const;

  /// peek x+2, y, and z-1
  inline const VoxelType & Peek2px0py1nz() const;

  /// peek x-1, y+1, and z-1
  inline const VoxelType & Peek1nx1py1nz() const;

  /// peek x, y+1, and z-1
  inline const VoxelType & Peek0px1py1nz() const;

  /// peek x+1, y+1, and z-1
  inline const VoxelType & Peek1px1py1nz() const;

  /// peek x+2, y+1, and z-1
  inline const VoxelType & Peek2px1py1nz() const;


  /// peek x-1, y+2, and z-1
  inline const VoxelType & Peek1nx2py1nz() const;

  /// peek x, y+2, and z-1
  inline const VoxelType & Peek0px2py1nz() const;

  /// peek x+1, y+2, and z-1
  inline const VoxelType & Peek1px2py1nz() const;

  /// peek x+2, y+2, and z-1
  inline const VoxelType & Peek2px2py1nz() const;


  /// peek x-1, y-1, and z
  inline const VoxelType & Peek1nx1ny0pz() const;

  /// peek x, y-1, and z
  inline const VoxelType & Peek0px1ny0pz() const;

  /// peek x+1, y-1, and z
  inline const VoxelType & Peek1px1ny0pz() const;

  /// peek x+2, y-1, and z
  inline const VoxelType & Peek2px1ny0pz() const;

  /// peek x-1, y, and z
  inline const VoxelType & Peek1nx0py0pz() const;

  /// peek x, y, and z
  inline const VoxelType & Peek0px0py0pz() const;

  /// peek x+1, y, and z
  inline const VoxelType & Peek1px0py0pz() const;

  /// peek x+2, y, and z
  inline const VoxelType & Peek2px0py0pz() const;

  /// peek x-0, y+1, and z
  inline const VoxelType & Peek1nx1py0pz() const;

  /// peek x, y+1, and z
  inline const VoxelType & Peek0px1py0pz() const;

  /// peek x+1, y+1, and z
  inline const VoxelType & Peek1px1py0pz() const;

  /// peek x+2, y+1, and z
  inline const VoxelType & Peek2px1py0pz() const;

  /// peek x-1, y+2, and z
  inline const VoxelType & Peek1nx2py0pz() const;

  /// peek x, y+2, and z
  inline const VoxelType & Peek0px2py0pz() const;

  /// peek x+1, y+2, and z
  inline const VoxelType & Peek1px2py0pz() const;

  /// peek x+2, y+2, and z
  inline const VoxelType & Peek2px2py0pz() const;

  /// peek x-1, y-1, and z+1
  inline const VoxelType & Peek1nx1ny1pz() const;

  /// peek x, y-1, and z+1
  inline const VoxelType & Peek0px1ny1pz() const;

  /// peek x+1, y-1, and z+1
  inline const VoxelType & Peek1px1ny1pz() const;

  /// peek x+2, y-1, and z+1
  inline const VoxelType & Peek2px1ny1pz() const;

  /// peek x-1, y, and z+1
  inline const VoxelType & Peek1nx0py1pz() const;

  /// peek x, y, and z+1
  inline const VoxelType & Peek0px0py1pz() const;

  /// peek x+1, y, and z+1
  inline const VoxelType & Peek1px0py1pz() const;

  /// peek x+2, y, and z+1
  inline const VoxelType & Peek2px0py1pz() const;

  /// peek x-1, y+1, and z+1
  inline const VoxelType & Peek1nx1py1pz() const;

  /// peek x, y+1, and z+1
  inline const VoxelType & Peek0px1py1pz() const;

  /// peek x+1, y+1, and z+1
  inline const VoxelType & Peek1px1py1pz() const;

  /// peek x+2, y+1, and z+1
  inline const VoxelType & Peek2px1py1pz() const;

  /// peek x-1, y+2, and z+1
  inline const VoxelType & Peek1nx2py1pz() const;

  /// peek x, y+2, and z+1
  inline const VoxelType & Peek0px2py1pz() const;

  /// peek x+1, y+2, and z+1
  inline const VoxelType & Peek1px2py1pz() const;

  /// peek x+2, y+2, and z+1
  inline const VoxelType & Peek2px2py1pz() const;

  /// peek x-1, y-1, and z+2
  inline const VoxelType & Peek1nx1ny2pz() const;

  /// peek x, y-1, and z+2
  inline const VoxelType & Peek0px1ny2pz() const;

  /// peek x+1, y-1, and z+2
  inline const VoxelType & Peek1px1ny2pz() const;

  /// peek x+2, y-1, and z+2
  inline const VoxelType & Peek2px1ny2pz() const;

  /// peek x-1, y, and z+2
  inline const VoxelType & Peek1nx0py2pz() const;

  /// peek x, y, and z+2
  inline const VoxelType & Peek0px0py2pz() const;

  /// peek x+1, y, and z+2
  inline const VoxelType & Peek1px0py2pz() const;

  /// peek x+2, y, and z+2
  inline const VoxelType & Peek2px0py2pz() const;

  /// peek x-1, y+1, and z+2
  inline const VoxelType & Peek1nx1py2pz() const;

  /// peek x, y+1, and z+2
  inline const VoxelType & Peek0px1py2pz() const;

  /// peek x+1, y+1, and z+2
  inline const VoxelType & Peek1px1py2pz() const;

  /// peek x+2, y+1, and z+2
  inline const VoxelType & Peek2px1py2pz() const;

  /// peek x-1, y+2, and z+2
  inline const VoxelType & Peek1nx2py2pz() const;

  /// peek x0, y+2, and z+2
  inline const VoxelType & Peek0px2py2pz() const;

  /// peek x+1, y+2, and z+2
  inline const VoxelType & Peek1px2py2pz() const;

  /// peek x+2, y+2, and z+2
  inline const VoxelType & Peek2px2py2pz() const;

// data
protected:

  /// exact sampling position
  Vector4d<float4> m_pos;

  /// the depth value at the end of the ray
  float4 m_fRaySegmentEndT;

  /// floor of the x coordinate of the sampling position  
  int4 m_iPosX;

  /// floor of the y coordinate of the sampling position  
  int4 m_iPosY;

  /// floor of the z coordinate of the sampling position  
  int4 m_iPosZ;

  /// unit sample distance increment vector
  Vector4Df m_sampleIncVector;

  /// space leap jump vector
  Vector4d<float4> m_sldDeltaVector;

  /// space leap jump vector
  Vector4d<float4> m_sldDeltaVectorMinusHalf; 

  /// space leap jump vector
  Vector4d<float4> m_sldDeltaVectorTwo; 

  /// gradient vector
  Vector3D<float4> m_gradientVector;

  /// resampled voxel value
  float4 m_fResampledVoxel;

  /// current voxel in the level 0 voxel block
  VoxelType * m_pVox;

  /// current level 0 block
  typename vxBlockVolume<VoxelType>::BlockL0Vox * m_pL0;

  /// current level 1 block
  typename vxBlockVolume<VoxelType>::BlockL1Blk * m_pL1;

  /// reference to the volume on which this iterator operates
  vxBlockVolume<VoxelType> & m_rVol;

// friends
protected:

  friend vxBlockVolumeRayIterator<int2>;
  friend vxBlockVolumeRayIterator<uint2>;

}; // vxBlockVolumeRayIterator


#include "vxBlockVolumeRayIterator.inl"


#endif // vxBlockVolumeRayIterator_h


// $Log: vxBlockVolumeRayIterator.h,v $
// Revision 1.3.10.1  2010/07/16 20:31:28  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.3  2006/03/29 19:57:21  geconomos
// changed visibilty of GetOffsetXXX methods to public
//
// Revision 1.2  2004/03/04 21:00:55  geconomos
// Modifications for 7.1 compiler including: use of typename for dependant types and resolving ambiguities.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.38  2003/09/08 16:32:47  frank
// Added functions to better control space leaping
//
// Revision 1.37  2003/06/25 18:35:39  frank
// Added IsInsideVolume
//
// Revision 1.36  2003/05/20 19:55:05  michael
// non conform variable name (hungarian notation)
//
// Revision 1.35  2003/05/03 16:18:20  michael
// completed code review
//
// Revision 1.34  2003/01/22 21:34:59  ingmar
// this iterator is now only using a RaySegment and a Vector4Df increment to determine how to traverse the ray
//
// Revision 1.33  2002/11/26 18:08:21  ingmar
// now using Vector4d<float4> to keep the current position as well as offset
//
// Revision 1.32  2002/11/20 23:30:54  frank
// Added some new ray sampling options for label volumes
//
// Revision 1.31  2002/09/25 23:21:57  ingmar
// changes from intel  tuning visit
//
// Revision 1.30  2002/09/25 23:12:35  ingmar
// changes from intel  tuning visit
//
// Revision 1.29  2002/08/02 21:09:53  ingmar
// coding standard compliant variable naming
//
// Revision 1.28  2002/07/12 23:18:15  ingmar
// avoided calling ftoa() for samples -> 3.8fps
//
// Revision 1.27  2002/07/09 17:12:04  ingmar
// changed Sld block width to 16
// added GetDistanceToEndOfSmallBlock()  for blocksize 4^3
//
// Revision 1.26  2002/06/14 22:20:50  ingmar
// finished Sobel Gradient Filter implementation
// added several Peek functions needed for 64 neighborhood of sobel filter
//
// Revision 1.25  2002/06/13 21:02:45  ingmar
// added GetDistanceToEndOfBlock()
//
// Revision 1.24  2002/06/10 20:59:19  soeren
// changed constructor
//
// Revision 1.23  2002/06/06 20:49:16  soeren
// added inital version of Sobel gradient
//
// Revision 1.22  2002/06/06 15:29:18  soeren
// implemented GetSampleGradientImmediateDifference
//
// Revision 1.21  2002/06/05 14:35:03  soeren
// renamed peeks
//
// Revision 1.20  2002/06/04 15:51:40  soeren
// improved GetSampleGradientCentralDifference
//
// Revision 1.19  2002/06/04 15:01:56  soeren
// renamed GetSampleGradient to GetSampleCentraldifferencegradient
//
// Revision 1.18  2002/06/04 13:53:36  soeren
// beautified
//
// Revision 1.17  2002/06/04 13:32:57  soeren
// optimized GetSample
//
// Revision 1.16  2002/06/04 13:08:36  soeren
// removed old gradient computation
//
// Revision 1.15  2002/06/03 16:46:27  soeren
// inlined updatedinternal....
//
// Revision 1.14  2002/06/03 14:04:48  soeren
// removed obsolute Next and prev else
//
// Revision 1.13  2002/06/02 18:59:54  soeren
// fixed new gradientTrilinear function
//
// Revision 1.12  2002/06/02 18:20:26  soeren
// removed obsolute PeekNext and PeekPrev
//
// Revision 1.11  2002/06/02 17:50:58  soeren
// added more accurate GetGradientTrilinear function and the 30 Peek functions for that function
//
// Revision 1.10  2002/06/02 17:07:19  soeren
// initial version of GetGradientTrilinear()
//
// Revision 1.9  2002/06/02 15:21:55  soeren
// Speed up, through moving else of UpdateInternalIntegerPosition to C file:-)
//
// Revision 1.8  2002/06/02 15:15:57  soeren
// Worked on CleanUp.
//
// Revision 1.7  2002/06/02 14:38:21  soeren
// removed non raw functions and renamed raw functions to non raw,
// since there is nothing unsave now
//
// Revision 1.6  2002/05/24 20:08:24  ingmar
// towards coding standard compliance
//
// Revision 1.5  2002/05/07 22:41:10  ingmar
// added derivation from vxBlockVolumeIteratorConsts to shorten access to the constants
//
// Revision 1.4  2002/04/09 23:46:40  ingmar
// removed obsulete member direction vector
//
// Revision 1.3  2002/04/08 21:48:19  ingmar
// new spaceleap computation
//
// Revision 1.2  2002/04/04 19:08:56  ingmar
// fixed some bugs in Peek and code beautifiy
//
// Revision 1.1  2002/04/04 14:42:52  ingmar
// initial version
//
// Revision 1.6  2002/04/02 20:05:07  ingmar
// now using bounding box of contributing voxels to test for end of ray
//
// Revision 1.5  2002/03/20 16:14:35  ingmar
// Instead of fast access pointer to the beginning of the data of a block, we are using now the direct
// access function of the block.
//
// Revision 1.4  2002/03/19 20:11:50  ingmar
// faster SetPos() through checking if we are inside the current block
//
// Revision 1.3  2002/03/19 17:01:02  ingmar
// included all voliter functionality to allow compiler to make better optimaizations
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeRayIterator.h,v 1.3.10.1 2010/07/16 20:31:28 dongqing Exp $
// $Id: vxBlockVolumeRayIterator.h,v 1.3.10.1 2010/07/16 20:31:28 dongqing Exp $
