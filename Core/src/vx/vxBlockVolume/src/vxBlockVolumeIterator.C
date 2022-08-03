// $Id: vxBlockVolumeIterator.C,v 1.3.10.1 2010/07/16 20:31:42 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren (ingmar|soeren)@viatronix.com


// includes
#include "stdafx.h"
#include "vxBlockVolumeIterator.h"
#include "vxComplex.h"                                  

           
// defines
#define FILE_REVISION "$Revision: 1.3.10.1 $"

// partial template instantiations
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIterator<uint1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIterator<uint2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIterator<uint4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIterator< int1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIterator< int2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIterator< int4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIterator< bool>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIterator< float4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIterator< vxComplex >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeIterator< vxVoxelCost >;

using namespace std;


/**
 * Trilinear Interpolation (inlined by precompiler)
 */
#undef Trilin // just in case
#define Trilin(x,y,z,a,b,c,d,e,f,g,h,ab,cd,ef,gh,abcd,efgh,final)      \
                                 \
  ab = (b-a)*x + a;              \
  cd = (d-c)*x + c;              \
  ef = (f-e)*x + e;              \
  gh = (h-g)*x + g;              \
                                 \
  abcd = (cd-ab)*y + ab;         \
  efgh = (gh-ef)*y + ef;         \
  final = ((efgh-abcd)*z + abcd);



/**
 * Sets a complete block of voxels.
 * @param voxelBlock
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetBlock(const vxBlock<VoxelType> & voxelBlock)
{
  const uint4 uBlkOffset(GetOffsetL1Blk());
  const uint4 uVoxOffset(GetOffsetL0Vox());

  m_pL1->UnShare(); // make a copy of block of blocks at level L1
  m_pL1->At(uBlkOffset) = voxelBlock;

  m_pL0   = m_pL1->GetDataPtr() + uBlkOffset;
  m_pVox  = m_pL0->GetDataPtr() + uVoxOffset;
} // SetBlock()


/**
 * Sets the value at the current location.
 * May be garbage or segfault if the location is not valid.
 * @param value  The new voxel value
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetVoxelRawElse(const VoxelType & value)
{
  const uint4 uBlkOffset(GetOffsetL1Blk());
  const uint4 uVoxOffset(GetOffsetL0Vox());
  if ( m_pL1->IsUnShared() )
  {
    m_pL1->At(uBlkOffset).Set(value, uVoxOffset);
    m_pVox = m_pL0->GetDataPtr() + uVoxOffset;  
    return;
  }
  else
  {
    m_pL1->UnShare();// make a copy of block of blocks at level L1
    m_pL1->At(uBlkOffset).Set(value, uVoxOffset);

    m_pL0   = m_pL1->GetDataPtr() + uBlkOffset;
    m_pVox  = m_pL0->GetDataPtr() + uVoxOffset;
    return;
  }    
} // SetVoxelRawElse()


template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetVoxelAtXYZElse(const int4 iPosX, 
                                                         const int4 iPosY, 
                                                         const int4 iPosZ, 
                                                         VoxelType value)
{
  const int4 iL2VolOffset(GetOffsetL2VolFromPosXyz(iPosX, iPosY, iPosZ));
  const int4 iL1BlkOffset(GetOffsetL1BlkFromPosXyz(iPosX, iPosY, iPosZ));
  const int4 iL0VoxOffset(GetOffsetL0VoxFromPosXyz(iPosX, iPosY, iPosZ));

  vxBlockVolume<VoxelType>::BlockL1Blk * pL1(m_rVol.m_BlockL2Vol.GetDataPtr() + iL2VolOffset);
  vxBlockVolume<VoxelType>::BlockL0Vox * pL0(pL1->GetDataPtr() + iL1BlkOffset);

  const int4 iOldL2VolOffset( static_cast< int4 >( m_pL1  - m_rVol.m_BlockL2Vol.GetDataPtr() ));
  const int4 iOldL1BlkOffset( static_cast< int4 >(m_pL0  - m_pL1->GetDataPtr() ));
  const int4 iOldL0VoxOffset( static_cast< int4 >(m_pVox - m_pL0->GetDataPtr() ));

  // make a copy of block of blocks at level L1
  pL1->UnShare();
  pL1->At(iL1BlkOffset).Set(value, iL0VoxOffset);

  m_pL1  = m_rVol.m_BlockL2Vol.GetDataPtr() + iOldL2VolOffset;
  m_pL0  = m_pL1->GetDataPtr() + iOldL1BlkOffset;
  m_pVox = m_pL0->GetDataPtr() + iOldL0VoxOffset;

  return;
} // SetVoxelAtXYZElse()


/**
 * Set voxel at position of iterator
 * @param pointer to level 1 block
 * @param pointer to level 0 block
 * @param level 1 offset
 * @param level 0 offset
 * @param value
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetVoxelAtPosOfIteratorElse(typename vxBlockVolume<VoxelType>::BlockL1Blk * pL1,
                                                                   typename vxBlockVolume<VoxelType>::BlockL0Vox * pL0,
                                                                   const int4 iL1BlkOffset, 
                                                                   const int4 iL0VoxOffset, 
                                                                   VoxelType value)
{
  const int4  iOldL2VolOffset = static_cast< int4 >( m_pL1  - m_rVol.m_BlockL2Vol.GetDataPtr() );
  const int4  iOldL1BlkOffset = static_cast< int4 >( m_pL0  - m_pL1->GetDataPtr() );
  const int4  iOldL0VoxOffset = static_cast< int4 >( m_pVox - m_pL0->GetDataPtr() );  

  pL1->UnShare();// make a copy of block of blocks at level L1
  pL1->At(iL1BlkOffset).Set(value, iL0VoxOffset);

  m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + iOldL2VolOffset;
  m_pL0       = m_pL1->GetDataPtr() + iOldL1BlkOffset;
  m_pVox      = m_pL0->GetDataPtr() + iOldL0VoxOffset;
} // SetVoxelAtPosOfIteratorElse()


/**
 * Returns the value at the given floating point position.
  * Template specfialization for bool
* 
 * @return the sample value.
 */
template <>
const float4 & vxBlockVolumeIterator<bool>::GetSample(const Vector3D<float4> & vfPos)
{
  // Compute weights for tri-linear interpolation.
  const int4 iPosX((int4)vfPos.X());
  const int4 iPosY((int4)vfPos.Y());
  const int4 iPosZ((int4)vfPos.Z());

  const float4 fXDiff(vfPos.X() - iPosX);
  const float4 fYDiff(vfPos.Y() - iPosY);
  const float4 fZDiff(vfPos.Z() - iPosZ);

  const float4 vox000(Peek(iPosX  ,iPosY  ,iPosZ  )); 
  const float4 vox100(Peek(iPosX+1,iPosY  ,iPosZ  )); 
  const float4 vox010(Peek(iPosX  ,iPosY+1,iPosZ  )); 
  const float4 vox110(Peek(iPosX+1,iPosY+1,iPosZ  )); 
  const float4 vox001(Peek(iPosX  ,iPosY  ,iPosZ+1)); 
  const float4 vox101(Peek(iPosX+1,iPosY  ,iPosZ+1)); 
  const float4 vox011(Peek(iPosX  ,iPosY+1,iPosZ+1)); 
  const float4 vox111(Peek(iPosX+1,iPosY+1,iPosZ+1)); 

  float4 ab,cd,ef,gh,abcd,efgh;
  Trilin(
        fXDiff, fYDiff, fZDiff, vox000, 
        vox100, vox010, vox110, vox001, 
        vox101, vox011, vox111, ab,cd,ef,gh,abcd,efgh, m_fSample);
  return m_fSample;
} // GetSample


/**
 * Returns the value at the given floating point position.
  * Template specfialization for float4
* 
 * @return the sample value.
 */
template <>
const float4 & vxBlockVolumeIterator<float4>::GetSample(const Vector3D<float4> & vfPos)
{
  // Compute weights for tri-linear interpolation.
  const int4 iPosX((int4)vfPos.X());
  const int4 iPosY((int4)vfPos.Y());
  const int4 iPosZ((int4)vfPos.Z());

  const float4 fXDiff(vfPos.X() - iPosX);
  const float4 fYDiff(vfPos.Y() - iPosY);
  const float4 fZDiff(vfPos.Z() - iPosZ);

  const float4 vox000(Peek(iPosX  ,iPosY  ,iPosZ  )); 
  const float4 vox100(Peek(iPosX+1,iPosY  ,iPosZ  )); 
  const float4 vox010(Peek(iPosX  ,iPosY+1,iPosZ  )); 
  const float4 vox110(Peek(iPosX+1,iPosY+1,iPosZ  )); 
  const float4 vox001(Peek(iPosX  ,iPosY  ,iPosZ+1)); 
  const float4 vox101(Peek(iPosX+1,iPosY  ,iPosZ+1)); 
  const float4 vox011(Peek(iPosX  ,iPosY+1,iPosZ+1)); 
  const float4 vox111(Peek(iPosX+1,iPosY+1,iPosZ+1)); 

  float4 ab,cd,ef,gh,abcd,efgh;
  Trilin(
        fXDiff, fYDiff, fZDiff, vox000, 
        vox100, vox010, vox110, vox001, 
        vox101, vox011, vox111, ab,cd,ef,gh,abcd,efgh, m_fSample);
  return(m_fSample);
} // GetSample



/**
 * Returns the value at the given floating point position.
 * 
 * @return the sample value.
 */
template <class VoxelType>
const float4 & vxBlockVolumeIterator<VoxelType>::GetSample(const Vector3D<float4> & vfPos)
{
  // Compute weights for tri-linear interpolation.
  const int4 iPosX((int4)vfPos.X());
  const int4 iPosY((int4)vfPos.Y());
  const int4 iPosZ((int4)vfPos.Z());

  const float4 fXDiff(vfPos.X() - iPosX);
  const float4 fYDiff(vfPos.Y() - iPosY);
  const float4 fZDiff(vfPos.Z() - iPosZ);

  const VoxelType vox000(Peek(iPosX  ,iPosY  ,iPosZ  ));
  const VoxelType vox100(Peek(iPosX+1,iPosY  ,iPosZ  )); 
  const VoxelType vox010(Peek(iPosX  ,iPosY+1,iPosZ  )); 
  const VoxelType vox110(Peek(iPosX+1,iPosY+1,iPosZ  )); 
  const VoxelType vox001(Peek(iPosX  ,iPosY  ,iPosZ+1)); 
  const VoxelType vox101(Peek(iPosX+1,iPosY  ,iPosZ+1)); 
  const VoxelType vox011(Peek(iPosX  ,iPosY+1,iPosZ+1)); 
  const VoxelType vox111(Peek(iPosX+1,iPosY+1,iPosZ+1)); 

  float4 ab,cd,ef,gh,abcd,efgh;

  Trilin(
        fXDiff, fYDiff, fZDiff, vox000, 
        vox100, vox010, vox110, vox001, 
        vox101, vox011, vox111, ab,cd,ef,gh,abcd,efgh, m_fSample);
  return(m_fSample);
} // GetSample




/**
 * Returns the value at the given floating point position.
 * Template specfialization for bool
 * 
 * @return the sample value.
 */
template <>
const float4 & vxBlockVolumeIterator<bool>::GetSample(float4 fPosX, float4 fPosY, float4 fPosZ)
{
  // Compute weights for tri-linear interpolation.
  const int4 iPosX((int4)fPosX);
  const int4 iPosY((int4)fPosY);
  const int4 iPosZ((int4)fPosZ);

  const float4 fXDiff(fPosX - iPosX);
  const float4 fYDiff(fPosY - iPosY);
  const float4 fZDiff(fPosZ - iPosZ);

  const float4 vox000(Peek(iPosX  ,iPosY  ,iPosZ  )); 
  const float4 vox100(Peek(iPosX+1,iPosY  ,iPosZ  )); 
  const float4 vox010(Peek(iPosX  ,iPosY+1,iPosZ  )); 
  const float4 vox110(Peek(iPosX+1,iPosY+1,iPosZ  )); 
  const float4 vox001(Peek(iPosX  ,iPosY  ,iPosZ+1)); 
  const float4 vox101(Peek(iPosX+1,iPosY  ,iPosZ+1)); 
  const float4 vox011(Peek(iPosX  ,iPosY+1,iPosZ+1)); 
  const float4 vox111(Peek(iPosX+1,iPosY+1,iPosZ+1)); 

  // Compute the resampled value by tri-linear interpolation.
  float4 ab,cd,ef,gh,abcd,efgh;
  Trilin(
        fXDiff, fYDiff, fZDiff, vox000, 
        vox100, vox010, vox110, vox001, 
        vox101, vox011, vox111, ab,cd,ef,gh,abcd,efgh, m_fSample);
  return(m_fSample);
} // GetSample


/**
 * Returns the value at the given floating point position.
 * Template specfialization for float4
 *
 * @return the sample value.
 */
template <>
const float4 & vxBlockVolumeIterator<float4>::GetSample(float4 fPosX, float4 fPosY, float4 fPosZ)
{
  // Compute weights for tri-linear interpolation.
  const int4 iPosX((int4)fPosX);
  const int4 iPosY((int4)fPosY);
  const int4 iPosZ((int4)fPosZ);

  const float4 fXDiff(fPosX - iPosX);
  const float4 fYDiff(fPosY - iPosY);
  const float4 fZDiff(fPosZ - iPosZ);

  const float4 vox000(Peek(iPosX  ,iPosY  ,iPosZ  )); 
  const float4 vox100(Peek(iPosX+1,iPosY  ,iPosZ  )); 
  const float4 vox010(Peek(iPosX  ,iPosY+1,iPosZ  )); 
  const float4 vox110(Peek(iPosX+1,iPosY+1,iPosZ  )); 
  const float4 vox001(Peek(iPosX  ,iPosY  ,iPosZ+1)); 
  const float4 vox101(Peek(iPosX+1,iPosY  ,iPosZ+1)); 
  const float4 vox011(Peek(iPosX  ,iPosY+1,iPosZ+1)); 
  const float4 vox111(Peek(iPosX+1,iPosY+1,iPosZ+1)); 

  // Compute the resampled value by tri-linear interpolation.
  float4 ab,cd,ef,gh,abcd,efgh;
  Trilin(
        fXDiff, fYDiff, fZDiff, vox000, 
        vox100, vox010, vox110, vox001, 
        vox101, vox011, vox111, ab,cd,ef,gh,abcd,efgh, m_fSample);
  return(m_fSample);
} // GetSample


/**
 * Returns the value at the given floating point position.
 * 
 * @return the sample value.
 */
template <class VoxelType>
const float4 & vxBlockVolumeIterator<VoxelType>::GetSample(float4 fPosX, float4 fPosY, float4 fPosZ)
{
  // Compute weights for tri-linear interpolation.
  const int4 iPosX((int4)fPosX);
  const int4 iPosY((int4)fPosY);
  const int4 iPosZ((int4)fPosZ);

  const float4 fXDiff(fPosX - iPosX);
  const float4 fYDiff(fPosY - iPosY);
  const float4 fZDiff(fPosZ - iPosZ);

  const VoxelType vox000(Peek(iPosX  ,iPosY  ,iPosZ  )); 
  const VoxelType vox100(Peek(iPosX+1,iPosY  ,iPosZ  )); 
  const VoxelType vox010(Peek(iPosX  ,iPosY+1,iPosZ  )); 
  const VoxelType vox110(Peek(iPosX+1,iPosY+1,iPosZ  )); 
  const VoxelType vox001(Peek(iPosX  ,iPosY  ,iPosZ+1)); 
  const VoxelType vox101(Peek(iPosX+1,iPosY  ,iPosZ+1)); 
  const VoxelType vox011(Peek(iPosX  ,iPosY+1,iPosZ+1)); 
  const VoxelType vox111(Peek(iPosX+1,iPosY+1,iPosZ+1)); 

  // Compute the resampled value by tri-linear interpolation.
  float4 ab,cd,ef,gh,abcd,efgh;
  Trilin(
        fXDiff, fYDiff, fZDiff, vox000, 
        vox100, vox010, vox110, vox001, 
        vox101, vox011, vox111, ab,cd,ef,gh,abcd,efgh, m_fSample);
  return(m_fSample);
} // GetSample


/**
 * Peeks at the next voxel in the x direction w/out moving the iterator.
 * In case that the peek looks into the neighbor block.
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::PeekNextXElse() const
{
  if ( (GetPosX() & eL1BlkL0VoxPosMax) < eL1BlkL0VoxPosMax ) // exactly on the right edge of the L0 voxel block
  {
    return(*( ((m_pL0+1)->GetDataPtr()) + (m_pVox - m_pL0->GetDataPtr() - eL0VoxPosMax))); // at a position that is not on the right edge of the L1 block
  }
  else
  {
    return( GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX()+1, GetPosY(), GetPosZ())) );
  }    
} // PeekNextXElse


/**
 * Peeks at the next voxel in the y direction w/out moving the iterator.
 * In case that the peek looks into the neighbor block.
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::PeekNextYElse() const
{
  if ( (GetPosY() & eL1BlkL0VoxPosMax) < eL1BlkL0VoxPosMax )
  {
    return(*(((m_pL0+4)->GetDataPtr()) + ((m_pVox - m_pL0->GetDataPtr()) & 0xF0F)));
  }
  else
  {
    return( GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX(), GetPosY()+1, GetPosZ())) );
  }
} // PeekNextYElse


/**
 * Peeks at the next voxel in the z direction w/out moving the iterator.
 * In case that the peek looks into the neighbor block.
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::PeekNextZElse() const
{
  if ( (GetPosZ() & eL1BlkL0VoxPosMax) < eL1BlkL0VoxPosMax )
  { // 
    return(*(((m_pL0+16)->GetDataPtr()) + (m_pVox - m_pL0->GetDataPtr() & 0x0FF)));      
  }
  else
  {
    return( GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX(), GetPosY(), GetPosZ()+1)) );
  }
} // PeekNextZElse


/**
 * Peeks at the prev voxel in the x direction w/out moving the iterator.
 * In case that the peek looks into the neighbor block.
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::PeekPrevXElse() const
{
  if ( (GetPosX() & eL1BlkL0VoxPosMax) > 0 )
  {
    return(*(((m_pL0-1)->GetDataPtr()) + (m_pVox - m_pL0->GetDataPtr() + 15)));
  }
  else
  {
    return( GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX()-1, GetPosY(), GetPosZ())) );
  }
} // PeekPrevXElse

/**
 * Peeks at the prev voxel in the y direction w/out moving the iterator.
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::PeekPrevYElse() const
{
  if ( (GetPosY() & eL1BlkL0VoxPosMax) > 0 )
  {
    return(*(((m_pL0-4)->GetDataPtr()) + ((m_pVox - m_pL0->GetDataPtr()) | 0x0F0)));
  }
  else
  {
    return( GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX(), GetPosY()-1, GetPosZ())) );
  }
} // PeekPrevY


/**
 * Peeks at the prev voxel in the z direction w/out moving the iterator.
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::PeekPrevZElse() const
{
  if ( (GetPosZ() & eL1BlkL0VoxPosMax) > 0 )
  {
    return(*(((m_pL0-16)->GetDataPtr()) + ((m_pVox - m_pL0->GetDataPtr()) + 3840)));
  }
  else
  {
    return( GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX(), GetPosY(), GetPosZ()-1)) );
  }
} 

/**
 * Peeks at the next voxel in the xy direction w/out moving the iterator.
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::PeekNextXYElse() const
{
  return( GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX()+1, GetPosY()+1, GetPosZ())) );
} // PeekNextXY


/**
 * Peeks at the next voxel in the xz direction w/out moving the iterator.
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::PeekNextXZElse() const
{
  return( GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX()+1, GetPosY(), GetPosZ()+1)) );
} // PeekNextXZ


/**
 * Peeks at the next voxel in the xy direction w/out moving the iterator.
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::PeekNextYZElse() const
{
  return( GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX(), GetPosY()+1, GetPosZ()+1)) );
} // PeekNextYZ


/**
 * Peeks at the next voxel in the xyz direction w/out moving the iterator.
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::PeekNextXYZElse() const
{
  return( GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX()+1, GetPosY()+1, GetPosZ()+1)) );
} // PeekNextXYZ


/**
 * Increments the iterator location in the x direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::IncXElse()
{
  --m_L0VoxOffset;
  SetPosX(GetPosX()+1);
} // IncXElse


/**
 * Decrements the iterator location in the x direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::DecXElse()
{
  ++m_L0VoxOffset;
  SetPosX(GetPosX()-1);
} // DecX


/**
 * Increments the iterator location in the y direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::IncYElse()
{
  m_L0VoxOffset -= eL0VoxOffsetDeltaY;  
  SetPosY(GetPosY()+1);
} // IncY


/**
 * Decrements the iterator location in the y direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::DecYElse()
{
  m_L0VoxOffset += eL0VoxOffsetDeltaY;  
  SetPosY(GetPosY()-1);
} // DecY


/**
 * Increments the iterator location in the z direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::IncZElse()
{
  m_L0VoxOffset -= eL0VoxOffsetDeltaZ;  
  SetPosZ(GetPosZ()+1);
} // IncZ


/**
 * Decrements the iterator location in the z direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::DecZElse()
{
  m_L0VoxOffset += eL0VoxOffsetDeltaZ;  
  SetPosZ(GetPosZ()-1);
} // DecZ

#undef FILE_REVISION


// Revision History:
// $Log: vxBlockVolumeIterator.C,v $
// Revision 1.3.10.1  2010/07/16 20:31:42  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.3  2006/03/09 18:35:39  dongqing
// add vxVoxelCost voxel type to the vxBlockVolume
//
// Revision 1.2  2004/03/04 21:00:55  geconomos
// Modifications for 7.1 compiler including: use of typename for dependant types and resolving ambiguities.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.29  2003/05/17 16:22:37  michael
// reformatting
//
// Revision 1.28  2003/05/17 16:19:40  michael
// reformatting
//
// Revision 1.27  2003/05/16 11:54:10  michael
// code review
//
// Revision 1.26  2003/05/02 18:29:12  michael
// initial code review
//
// Revision 1.25  2002/08/28 15:04:53  ingmar
// added support for vxComplex
// removed masking out upper 4 bits inside GetSample()
//
// Revision 1.24  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.23  2002/05/22 21:13:42  ingmar
// adjusted voxel naming to XYZ order of indices -> hopefully fixes TriLin interpolation
//
// Revision 1.22  2002/05/20 21:02:57  ingmar
// removed all warnings caused by GetSample()
//
// Revision 1.21  2002/05/20 20:34:47  geconomos
// Fixed compilation errors for VC7.
//
// Revision 1.20  2002/05/15 20:08:34  tushar
// removed getsample from header and put it in c file
//
// Revision 1.19  2002/05/13 14:26:52  ingmar
// added and used GetVoxelAtPackedBlockOffsets() GetPosAsPackedBlockOffsetsFromPosXyz()
//
// Revision 1.18  2002/05/09 18:23:16  ingmar
// Restricted m_iPosX/Y/Z to onlt keep the position outside the current block scope. Inside an ofset is used instead.
// Resulted in the efficient traversal now being at the same speed as *poniter++ of a linear array
//
// Revision 1.17  2002/05/08 19:13:52  tushar
// made vxVolume complatible with float4
//
// Revision 1.16  2002/05/07 22:42:20  ingmar
// changed to coding standard compliant enums
//
// Revision 1.15  2002/04/29 21:09:50  soeren
// added SetVoxel from iterator
//
// Revision 1.14  2002/04/29 19:39:50  ingmar
// comment beautify
//
// Revision 1.13  2002/04/29 16:21:10  ingmar
// fixed offset computation
//
// Revision 1.12  2002/04/29 16:10:44  ingmar
// fixed offset computation
//
// Revision 1.11  2002/04/29 15:41:14  soeren
// added SetVoxel at neighbor pos
//
// Revision 1.10  2002/04/23 19:20:26  ingmar
// fixed operator++
//
// Revision 1.9  2002/04/23 17:25:59  ingmar
// alll inc and dec are now based on SetPos
//
// Revision 1.8  2002/04/19 18:43:19  jaddonisio
// added new type bool
//
// Revision 1.7  2002/04/18 19:55:38  ingmar
// removed obsolete GetVoxelRaw() from vxBlockVolumeIterator class
// and renamed GetVoxelRaw(offset) to GetVoxelAtLinOffset(offset)
//
// Revision 1.6  2002/04/17 00:58:14  ingmar
// added SetBlock()
//
// Revision 1.5  2002/04/09 15:44:17  michael
// fixed iterator compare from >0 to !=0
//
// Revision 1.4  2002/04/09 15:38:34  michael
// fixed iterator compare from >0 to !=0
//
// Revision 1.3  2002/04/04 22:09:23  ingmar
// fixed walking into negative positions
//
// Revision 1.2  2002/04/04 19:08:56  ingmar
// fixed some bugs in Peek and code beautifiy
//
// Revision 1.1  2002/04/04 14:02:38  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeIterator.C,v 1.3.10.1 2010/07/16 20:31:42 dongqing Exp $
// $Id: vxBlockVolumeIterator.C,v 1.3.10.1 2010/07/16 20:31:42 dongqing Exp $
