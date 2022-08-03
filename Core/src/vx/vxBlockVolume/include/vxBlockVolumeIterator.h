// $Id: vxBlockVolumeIterator.h,v 1.6.10.1 2010/07/16 20:31:28 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren (ingmar|soeren)@viatronix.com

/**
 * Optimized Block Volume Iterator
 */

#ifndef vxBlockVolumeIterator_h
#define vxBlockVolumeIterator_h


// includes
#include "vxBlockVolume.h"
#include "vxBlockVolumeIteratorConsts.h"
#include "vxComplex.h"
#include <vector>
#include "vxVoxelCost.h"

/*******************************************************************

uint8 PackedVolumeOffset

L2 Offset = Z Y X = 5 5 5 = total 15 bits
L1 Offset = Z Y X = 2 2 2 = total  6 bits
L0 Offset = Z Y X = 4 4 4 = total 12 bits

uint8 = L2 L1 L0 = 15 6 12 = total 33 bits

********************************************************************/


// class definition
template <class VoxelType>
class vxBlockVolumeIterator : vxBlockVolumeIteratorConsts
{
// functions
public:

  /// constructor
  vxBlockVolumeIterator(vxBlockVolume<VoxelType> & vol, const Vector3D<int4> & point = Vector3D<int4>(0,0,0));

  /// desctructor
  ~vxBlockVolumeIterator();

  /// moves the iterator to the next voxel according to a ZYX traversal
  inline void NextZYX();

  /// moves the iterator to the next voxel according to a XZY traversal
  inline void NextXZY();

  /// moves the iterator to the next voxel according to a YXZ traversal
  inline void NextYXZ();

  /// moves the iterator to the previous voxel according to a ZYX traversal
  inline void PreviousZYX();

  /// moves the position to the next non-background voxel in ZYX order
  inline void FirstNonBackgroundZYX();

  /// moves the position to the next non-background voxel in ZYX order
  inline void FirstNonBackgroundZYXinsideBlock();

  /// moves the position to the next non-background block in ZYX order
  inline void FirstNonBackgroundBlockZYX();

  /// moves the position to the next non-background voxel in ZYX order
  inline void NextNonBackgroundZYX();

  /// moves the iterator to the next non background voxel according to a ZYX traversal only inside the current block
  inline void NextNonBackgroundZYXinsideBlock();

  /// moves the position to the next non-background block in ZYX order
  inline void NextNonBackgroundBlockZYX();

  /// moves the iterator to the beginning of the next block
  inline void NextBlockXsameOffset();

  /// moves the iterator to the beginning of the next block
  inline void NextBlockYsameOffset();

  /// moves the iterator to the beginning of the next block
  inline void NextBlockZsameOffset();

  /// moves the iterator to the beginning of the next block
  inline void NextBlockZYX();

  /// traversal has reached end
  inline bool IsAtEnd() const;

  /// traversal has reached end
  inline bool IsNotAtEnd() const;

  /// moves the iterator to the next voxel according to a ZYX traversal only inside the current block
  inline void NextZYXinsideBlock();

  /// moves the iterator to the next voxel according to a XZY traversal only inside the current block
  inline void NextXZYinsideBlock();

  /// moves the iterator to the next voxel according to a YXZ traversal only inside the current block
  inline void NextYXZinsideBlock();

  /// traversal has reached end
  inline bool IsInsideOfBlock() const;

  /// traversal has reached end
  inline bool IsAtEndOfBlock() const;

  /// traversal has reached end
  inline bool IsNotAtEndOfBlock() const;

  /// set clear block
  inline void SetClearBlock(VoxelType clearValue);

  /// clear the current block
  inline void ClearCurrentBlock();

  /// returns the current block of voxels corresponding to the current location.
  inline const vxBlock<VoxelType> & GetCurrentBlock() const;

  /// returns the current block of voxels corresponding to the current location.
  inline void SetCurrentBlock(const vxBlock<VoxelType> & block);
  
  ///////////////////////////////////////////////////////////////////////
  //
  // get & set voxels
  //

  /// returns the value at the current location.
  inline const VoxelType & GetVoxel() const;

  template <class OtherVoxelType>
  inline const VoxelType & GetVoxel(const vxBlockVolumeIterator<OtherVoxelType> & iter)
  {
    const int4 iL2VolOffset = (iter.m_pL1  - iter.m_rVol.m_BlockL2Vol.GetDataPtr());
    const int4 iL1BlkOffset = (iter.m_pL0  - iter.m_pL1->GetDataPtr());
    const int4 iL0VoxOffset = (iter.m_pVox - iter.m_pL0->GetDataPtr());

    vxBlockVolume<VoxelType>::BlockL1Blk * pL1(m_rVol.m_BlockL2Vol.GetDataPtr() + iL2VolOffset);
    vxBlockVolume<VoxelType>::BlockL0Vox * pL0(pL1->GetDataPtr() + iL1BlkOffset);
    VoxelType                            * pVox(pL0->GetDataPtr() + iL0VoxOffset);

    return *pVox;
  }

  /// returns the value at the equivalent linear volume offset location.
  inline const VoxelType & GetVoxelAtPackedBlockOffsets(const uint8 iOffsets) const;

  /// returns the value at the equivalent linear volume offset location.
  inline const VoxelType & GetVoxelAtLinOffset(const uint8 iOffset) const;

  /// sets the value at the current location.
  __forceinline void SetVoxel(const VoxelType & value);

  /// sets the value at the current location; Safe version.
  inline void SetVoxelNextX(const VoxelType & value);

  /// sets the value at the current location; Safe version.
  inline void SetVoxelNextY(const VoxelType & value);

  /// sets the value at the current location; Safe version.
  inline void SetVoxelNextZ(const VoxelType & value);

  /// sets the value at the current location; Safe version.
  inline void SetVoxelPrevX(const VoxelType & value);

  /// sets the value at the current location; Safe version.
  inline void SetVoxelPrevY(const VoxelType & value);

  /// sets the value at the current location; Safe version.
  inline void SetVoxelPrevZ(const VoxelType & value);

  /// sets a complete block of voxels.
  void SetBlock(const vxBlock<VoxelType> & voxelBlock);

private:

  /// set voxel at iterator location
  inline void  SetVoxelAtPosOfIteratorElse(typename vxBlockVolume<VoxelType>::BlockL1Blk * pL1, typename vxBlockVolume<VoxelType>::BlockL0Vox * pL0, const int4 iL1BlkOffset, const int4 iL0VoxOffset, VoxelType value);

public:
  /// sets the value at the location of another iterator
  template <class OtherVoxelType>
  inline void SetVoxel(const VoxelType & voxelValue, const vxBlockVolumeIterator<OtherVoxelType> & rhsIterator)
  {
    const int4 iL2VolOffset = (rhsIterator.m_pL1 - rhsIterator.m_rVol.m_BlockL2Vol.GetDataPtr());
    const int4 iL1BlkOffset = (rhsIterator.m_pL0 - rhsIterator.m_pL1->GetDataPtr());
    const int4 iL0VoxOffset = (rhsIterator.m_pVox - rhsIterator.m_pL0->GetDataPtr());

    vxBlockVolume<VoxelType>::BlockL1Blk * pL1  = m_rVol.m_BlockL2Vol.GetDataPtr() + iL2VolOffset;
    vxBlockVolume<VoxelType>::BlockL0Vox * pL0  = pL1->GetDataPtr() + iL1BlkOffset;
    VoxelType                            * pVox = pL0->GetDataPtr() + iL0VoxOffset;

    if ( pL1->IsUnShared() && pL0->IsUnShared() )
    {
      *pVox = voxelValue;

      // this is inefficient but needed because there is either
      // a bug or it really is needed. It appears as if a block
      // becomes unshared but then succesively performed SetVoxel()
      // results in it becoming homogeneous. Hence, it is not 
      // shared and the transition from homogenous but unshared!!
      // to not homogeneous is lost.
      //////////////////////////////////////////////////////////////
      m_pL0->m_pBlockInfo->m_bIsHomogenous = false;

      return;
    } // if
    else
    {
      SetVoxelAtPosOfIteratorElse(pL1, pL0, iL1BlkOffset, iL0VoxOffset, voxelValue);
    }  // else 
  } // SetVoxel()


  /// copies the voxel from another iterator
  template <class OtherVoxelType>
  inline void SetVoxel(const vxBlockVolumeIterator<OtherVoxelType> & rhsIterator)
  {
    const int4 iL2VolOffset = (rhsIterator.m_pL1 - rhsIterator.m_rVol.m_BlockL2Vol.GetDataPtr());
    const int4 iL1BlkOffset = (rhsIterator.m_pL0 - rhsIterator.m_pL1->GetDataPtr());
    const int4 iL0VoxOffset = (rhsIterator.m_pVox - rhsIterator.m_pL0->GetDataPtr());

    vxBlockVolume<VoxelType>::BlockL1Blk * pL1  = m_rVol.m_BlockL2Vol.GetDataPtr() + iL2VolOffset;
    vxBlockVolume<VoxelType>::BlockL0Vox * pL0  = pL1->GetDataPtr() + iL1BlkOffset;
    VoxelType                            * pVox = pL0->GetDataPtr() + iL0VoxOffset;

    if ( pL1->IsUnShared() && pL0->IsUnShared() )
    {
      *pVox = rhsIterator.GetVoxel();
      return;
    }
    else
    {
      SetVoxelAtPosOfIteratorElse(pL1, pL0, iL1BlkOffset, iL0VoxOffset, rhsIterator.GetVoxel());
    }   // else
  }  // SetVoxel(

  /// MAY RELOCATE ITERATOR and does set the voxel at the offset position of all three blocks in the hierarchy packed into one int.
  inline void SetVoxelAtPackedBlockOffsetsPos(uint8 iPackedBlockOffsetsPos, const VoxelType & voxelValue);

  /// get sample
  const float4 & GetSample(float4 fPosX, float4 fPosY, float4 fPosZ);

  /// get sample
  const float4 & GetSample(const Vector3D<float4> & position);

private:

  /// returns the pointer to the block of voxels corresponding to the current location.
  inline const VoxelType * GetDataPointer() const;

  void SetVoxelAtXYZElse(const int4 iPosX, const int4 iPosY, const int4 iPosZ, VoxelType value);

public:
  ///////////////////////////////////////////////////////////////////////
  // incremental peek at some voxel

  const VoxelType & Peek(const int4 iPosX, const int4 iPosY, const int4 iPosZ) const;

  /// peeks at the value at the location given relative to current location.
  inline const VoxelType & PeekRel(const Vector3D<int4> & delta) const;

  /// peeks at the value at the location given relative to current location.
  inline const VoxelType & PeekRel(const int4 iDeltaX, const int4 iDeltaY, const int4 iDeltaZ ) const;

  /// peeks at the next voxel in the x direction w/out moving the iterator.
  inline const VoxelType & Peek1px0py0pz() const;

  /// peeks at the next voxel in the y direction w/out moving the iterator.
  inline const VoxelType & Peek0px1py0pz() const;

  /// peeks at the next voxel in the z direction w/out moving the iterator.
  inline const VoxelType & Peek0px0py1pz() const;

  /// peeks at the prev voxel in the xy direction w/out moving the iterator.
  inline const VoxelType & Peek1px1py0pz() const;

  /// peeks at the prev voxel in the xz direction w/out moving the iterator.
  inline const VoxelType & Peek1px0py1pz() const;

  /// peeks at the prev voxel in the yz direction w/out moving the iterator.
  inline const VoxelType & Peek0px1py1pz() const;

  /// peeks at the prev voxel in the xyz direction w/out moving the iterator.
  inline const VoxelType & Peek1px1py1pz() const;

  /// peeks at the prev voxel in the x direction w/out moving the iterator.
  inline const VoxelType & Peek1nx0py0pz() const;

  /// peeks at the prev voxel in the y direction w/out moving the iterator.
  inline const VoxelType & Peek0px1ny0pz() const;

  /// peeks at the prev voxel in the z direction w/out moving the iterator.
  inline const VoxelType & Peek0px0py1nz() const;

  /// peeks at the prev voxel in the xy direction w/out moving the iterator.
  inline const VoxelType & Peek1nx1ny0pz() const;

  /// peeks at the prev voxel in the xz direction w/out moving the iterator.
  inline const VoxelType & Peek1nx0py1nz() const;

  /// peeks at the prev voxel in the yz direction w/out moving the iterator.
  inline const VoxelType & Peek0px1ny1nz() const;

  /// peeks at the prev voxel in the xyz direction w/out moving the iterator.
  inline const VoxelType & Peek1nx1ny1nz() const;

  // this function will determine if we can call following 3 simple & safety peek functions.
  inline const bool PeekXYSafety() const;

  /// determine if the pointer is right at the corner (0,0) of a block
  inline const bool IsAtBlockCorner() const;

  /// next x in z-slice.
  inline const VoxelType & QuickPeek1px0py0pz() const;

  /// next y in z-slice.
  inline const VoxelType & QuickPeek0px1py0pz() const;

  /// next xy in z-slice.
  inline const VoxelType & QuickPeek1px1py0pz() const;

  /// peek voxel inside a block
  inline const VoxelType & QuickPeekInsideBlock(const int4 x, const int4 y) const;

  /// peek voxel inside a block
  inline const VoxelType & QuickPeekInsideBlock(const int4 x, const int4 y, const int4 z) const;

  /// peek voxel inside a block
  inline const VoxelType & QuickPeekInsideBlock(const int4 iOffset) const;

  /// returns the current x,y,z position.
  inline const Vector3D<int4> GetPos() const;

  /// returns the offset that corresponds to the current position.
  inline const int4 GetPosX() const;

  /// returns the offset that corresponds to the current position.
  inline const int4 GetPosY() const;

  /// returns the offset that corresponds to the current position.
  inline const int4 GetPosZ() const;

  /// returns the current x,y,z position.
  inline const Vector3D<int4> GetPosInsideBlock() const;

  /// returns the offset that corresponds to the current position.
  inline const int4 GetPosXInsideBlock() const;

  /// returns the offset that corresponds to the current position.
  inline const int4 GetPosYInsideBlock() const;

  /// returns the offset that corresponds to the current position.
  inline const int4 GetPosZInsideBlock() const;

  /// sets the iterator to the given absolute position.
  inline void SetPos(const Vector3D<int4> & pos);

  /// sets the iterator to the given absolute position.
  template <class OtherVoxelType>
  inline void SetPos(const vxBlockVolumeIterator<OtherVoxelType> & rhsIterator)
  {
    m_iBlockPosX  = rhsIterator.m_iBlockPosX;
    m_iBlockPosY  = rhsIterator.m_iBlockPosY;
    m_iBlockPosZ  = rhsIterator.m_iBlockPosZ;
    m_L0VoxOffset = rhsIterator.m_L0VoxOffset;
    m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + (rhsIterator.m_pL1 - rhsIterator.m_rVol.m_BlockL2Vol.GetDataPtr());
    m_pL0       = m_pL1->GetDataPtr() + (rhsIterator.m_pL0 - rhsIterator.m_pL1->GetDataPtr());
    m_pVox      = m_pL0->GetDataPtr() + rhsIterator.m_L0VoxOffset;
  } // SetPos

  /// sets the iterator to the given absolute position.
  inline void SetPos(const int4 iX, const int4 iY, const int4 iZ);

  /// sets just the X position of the iterator to the given absolute position.
  inline void SetPosX(const int4 iX);

  /// sets just the Y position of the iterator to the given absolute position.
  inline void SetPosY(const int4 iY);

  /// sets just the Z position of the iterator to the given absolute position.
  inline void SetPosZ(const int4 iZ);

  /// sets just the X and Y position of the iterator to the given absolute position.
  inline void SetPosXY(const int4 iX, const int4 iY);

  /// sets just the Y and Z position of the iterator to the given absolute position.
  inline void SetPosYZ(const int4 iY, const int4 iZ);

  /// sets just the X and Z position of the iterator to the given absolute position.
  inline void SetPosXZ(const int4 iX, const int4 iZ);

  /// moves the iterator relative to its current position.
  inline void SetPosRel(const Vector3D<int4> & ivDelta);

  /// moves the iterator relative to its current position.
  inline void SetPosRel(const int4 iDeltaX, const int4 iDeltaY, const int4 iDeltaZ);

  /// returns the current offset position of all three blocks in the hierarchy packed into one int.
  inline const uint8 GetPosAsPackedBlockOffsets() const;

  /// returns the offset position of the position given in all three blocks in the hierarchy packed into one int.
  inline const uint8 GetPosAsPackedBlockOffsetsFromPosXyz(const int4 iPosX, const int4 iPosY, const int4 iPosZ) const;

  /// returns the next X position corresponding offset in all three blocks in the hierarchy packed into one int.
  inline const uint8 GetPosAsPackedBlockOffsetsNextX() const;

  /// returns the next Y position corresponding offset in all three blocks in the hierarchy packed into one int.
  inline const uint8 GetPosAsPackedBlockOffsetsNextY() const;

  /// returns the next Z position corresponding offset in all three blocks in the hierarchy packed into one int.
  inline const uint8 GetPosAsPackedBlockOffsetsNextZ() const;

  /// returns the previous X position corresponding offset in all three blocks in the hierarchy packed into one int.
  inline const uint8 GetPosAsPackedBlockOffsetsPrevX() const;

  /// returns the previous Y position corresponding offset in all three blocks in the hierarchy packed into one int.
  inline const uint8 GetPosAsPackedBlockOffsetsPrevY() const;

  /// returns the previous Z position corresponding offset in all three blocks in the hierarchy packed into one int.
  inline const uint8 GetPosAsPackedBlockOffsetsPrevZ() const;

  /// sets the current offset position of all three blocks in the hierarchy packed into one int.
  inline void SetPosFromPackedBlockOffsets(uint8 iPackedBlockOffsets);

  /// Increments the iterator location in the x direction.
  inline void IncX();

  /// Increments the iterator location in the x direction.
  inline void IncXbyDelta(const int4 & iDelta);

  /// Increments the iterator location in the y direction.
  inline void IncY();

  /// Increments the iterator location in the y direction.
  inline void IncYbyDelta(const int4 & iDelta);

  /// Increments the iterator location in the z direction.
  inline void IncZ();

  /// Increments the iterator location in the z direction.
  inline void IncZbyDelta(const int4 & iDelta);

  /// Decrements the iterator location in the x direction.
  inline void DecX();

  /// Decrements the iterator location in the x direction.
  inline void DecXbyDelta(const int4 & iDelta);

  /// Decrements the iterator location in the y direction.
  inline void DecY();

  /// Decrements the iterator location in the y direction.
  inline void DecYbyDelta(const int4 & iDelta);

  /// Decrements the iterator location in the z direction.
  inline void DecZ();

  /// Decrements the iterator location in the z direction.
  inline void DecZbyDelta(const int4 & iDelta);

  /// returns true if the iterator is inside the volume.
  inline bool IsInsideVolume() const;

  /// returns a const reference to the volume this iterator is attached to.
  inline const vxBlockVolume<VoxelType> & GetVolume() const { return m_rVol; }

private:

  ///////////////////////////////////////////////////////////////////////
  // private helper functions

  /// returns the offset that corresponds to the current position.
  inline const int4 GetOffsetL2Vol() const;

  /// returns the offset that corresponds to the current position.
  inline const int4 GetOffsetL1Blk() const;

  /// returns the offset that corresponds to the current position.
  inline const int4 GetOffsetL0Vox() const { return m_L0VoxOffset; }

  /// returns the offset that corresponds to the current position.
  inline const int4 GetOffsetL2VolFromPosXyz(const int4 iPosX, const int4 iPosY, const int4 iPosZ) const;

  /// returns the offset that corresponds to the current position.
  inline const int4 GetOffsetL1BlkFromPosXyz(const int4 iPosX, const int4 iPosY, const int4 iPosZ) const;

  /// returns the offset that corresponds to the current position.
  inline const int4 GetOffsetL0VoxFromPosXyz(const int4 iPosX, const int4 iPosY, const int4 iPosZ) const;

  /// returns the offset that corresponds to the current position.
  const int4 GetOffsetL2VolFromPos(const Vector3D<int4> & iPos) const;

  /// returns the offset that corresponds to the current position.
  const int4 GetOffsetL1BlkFromPos(const Vector3D<int4> & iPos) const;

  /// returns the offset that corresponds to the current position.
  const int4 GetOffsetL0VoxFromPos(const Vector3D<int4> & iPos) const;


  /// returns the offset that corresponds to the current position.
  inline const uint8 GetLinVolOffset() const;

  /// returns the offset that corresponds to the given position.
  inline const uint8 GetLinVolOffset(const Vector3D<int4> & ivPos) const;

  /// returns the offset that corresponds to the given position.
  inline const uint8 GetLinVolOffset(const int4 iPosX, const int4 iPosY, const int4 iPosZ) const;

  /// Decrements the iterator location in the z direction.
  void DecZElse();

  /// Decrements the iterator location in the y direction.
  void DecYElse();

  /// Decrements the iterator location in the x direction.
  void DecXElse();

  /// Increments the iterator location in the z direction.
  void IncZElse();

  /// Increments the iterator location in the y direction.
  void IncYElse();

  /// Increments the iterator location in the x direction.
  void IncXElse();

  /// peeks at the prev voxel in the z direction w/out moving the iterator.
  const VoxelType & PeekPrevZElse() const;

  /// peeks at the prev voxel in the y direction w/out moving the iterator.
  const VoxelType & PeekPrevYElse() const;

  /// peeks at the prev voxel in in the x direction in the next block w/out moving the iterator.
  const VoxelType & PeekPrevXElse() const;

  /// peeks at the prev voxel in the xyz direction w/out moving the iterator.
  typename const VoxelType & PeekNextXYZElse() const;

  /// peeks at the prev voxel in the yz direction w/out moving the iterator.
  const VoxelType & PeekNextYZElse() const;

  /// peeks at the next voxel in in the y direction in the next block w/out moving the iterator.
  const VoxelType & PeekNextYElse() const;

  /// peeks at the prev voxel in the xy direction w/out moving the iterator.
  const VoxelType & PeekNextXYElse() const;

  /// peeks at the next voxel in in the z direction in the next block w/out moving the iterator.
  const VoxelType & PeekNextZElse() const;

  /// peeks at the next voxel in in the x direction in the next block w/out moving the iterator.
  const VoxelType & PeekNextXElse() const;

  /// peeks at the prev voxel in the xz direction w/out moving the iterator.
  const VoxelType & PeekNextXZElse() const;

  /// sets the value at the current location.
  void SetVoxelRawElse(const VoxelType & value);

// data
protected:

  /// current voxel in the level 0 voxel block
  VoxelType * m_pVox;

  /// current level 0 block
  typename vxBlockVolume<VoxelType>::BlockL0Vox * m_pL0;

  /// current level 1 block
  typename vxBlockVolume<VoxelType>::BlockL1Blk * m_pL1;

  /// the offset from the beginnig of the current level 0 block of voxels
  int4 m_L0VoxOffset;

  /// the x coordinate of the voxel position  
  int4 m_iBlockPosX;

  /// the y coordinate of the voxel position  
  int4 m_iBlockPosY;

  /// the z coordinate of the voxel position  
  int4 m_iBlockPosZ;

  /// reference to the volume on which this iterator operates
  vxBlockVolume<VoxelType> & m_rVol;

  /// background voxel
  const VoxelType m_Background;

  /// quick reference to volume dimension X
  const int4 & m_iVolumeDimensionX;

  /// quick reference to volume dimension Y
  const int4 & m_iVolumeDimensionY;

  /// quick reference to volume dimension Z
  const int4 & m_iVolumeDimensionZ;

  /// maximum level 2 offset
  const int4 m_L2VolOffsetVolMax;

  /// maximum level 1 offset
  const int4 m_L1BlkOffsetVolMax;

  /// maximum level 0 offset
  const int4 m_L0VoxOffsetVolMax;

  /// temporary variable used to quickly cache the result of #define Trilin
  float4 m_fSample;

  /// clear block
  vxBlock<VoxelType> m_ClearBlockVox;

  /// vector of clear blocks
  std::vector<vxBlock<VoxelType> *> m_vpClearBlocks;

  friend class vxBlockVolume<VoxelType>;
  friend class vxBlockVolumeIterator<int1>;
  friend class vxBlockVolumeIterator<int2>;
  friend class vxBlockVolumeIterator<int4>;
  friend class vxBlockVolumeIterator<uint1>;
  friend class vxBlockVolumeIterator<uint2>;
  friend class vxBlockVolumeIterator<uint4>;
  friend class vxBlockVolumeIterator<bool>;
  friend class vxBlockVolumeIterator<float4>;
  // this is only used in VC Processor on March 2006
  friend class vxBlockVolumeIterator<vxVoxelCost>;
  //friend class vxBlockVolumeFileReader< VoxelType >;
}; // vxBlockVolumeIterator


/// Overloaded << Operator.
template <class VoxelType>
std::ostream & operator<<(std::ostream & os, const vxBlockVolumeIterator<VoxelType> & iter);


#undef FILE_REVISION


#include "vxBlockVolumeIterator.inl"


#endif


// Revision History:
// $Log: vxBlockVolumeIterator.h,v $
// Revision 1.6.10.1  2010/07/16 20:31:28  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.6  2006/04/03 16:45:08  geconomos
// + promoted SetBlock to public
// + removed friend decaration for vxBlockVolumeFileReader
//
// Revision 1.5  2006/03/29 19:56:36  geconomos
// made a friend to class vxBlockVolumeFileReader
//
// Revision 1.4  2006/03/09 15:17:04  dongqing
// move vxVoxelCost from Colon Processor to vxBlockVolume
//
// Revision 1.3  2004/03/22 20:58:23  frank
// fixed some typos
//
// Revision 1.2  2004/03/04 21:00:55  geconomos
// Modifications for 7.1 compiler including: use of typename for dependant types and resolving ambiguities.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.61  2003/07/08 19:22:13  michael
// added fix for correct handling of homogeneous blocks. So far, it was only
// tested for being shared() on SetVoxel() but even an unshared block can later
// on become homogeneous which needs to be overwritten with subsequent
// SetVoxel() operations.
//
// Revision 1.60  2003/07/02 15:57:37  michael
// renamed member to reflect in name that it is the block position, not the actual
// iterator position.
//
// Revision 1.59  2003/06/18 15:19:47  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.58  2003/05/17 16:19:40  michael
// reformatting
//
// Revision 1.57  2003/05/16 11:54:10  michael
// code review
//
// Revision 1.56  2003/05/02 18:29:12  michael
// initial code review
//
// Revision 1.55  2002/08/28 16:48:13  ingmar
// added support for vxComplex
//
// Revision 1.54  2002/08/15 14:25:32  jaddonisio
// Changed Get functions to pass iPos by reference and not by value.
//
// Revision 1.53  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.52  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.51  2002/07/02 01:22:38  soeren
// added clearblock array
//
// Revision 1.50  2002/06/27 19:01:17  soeren
// added GetPos functions inside a block
//
// Revision 1.49.2.2  2002/07/08 21:11:23  binli
// new quick peek function: pointer
//
// Revision 1.49.2.1  2002/07/02 20:28:40  jaddonisio
// Merge from main trunk, per Soeren changes.
//
// Revision 1.49  2002/06/20 19:18:55  binli
// quick peek function.
//
// Revision 1.48  2002/06/17 15:34:12  soeren
// added UpdateLabelRenderVolume
//
// Revision 1.47  2002/06/04 22:47:51  soeren
// new VolIterator Peek functions that allow peeking into a mix of positive and negative offsets
//
// Revision 1.46  2002/05/17 20:38:55  frank
// Publically exposed GetCurrentBlock().
//
// Revision 1.45  2002/05/17 13:46:48  ingmar
// removed inefficient operator ++
//
// Revision 1.44  2002/05/15 20:08:34  tushar
// removed getsample from header and put it in c file
//
// Revision 1.43  2002/05/15 18:15:42  soeren
// added isClear function in volume
//
// Revision 1.42  2002/05/15 15:48:52  ingmar
// NextBlockZ() ->NextBlockZsameOffset()
//
// Revision 1.41  2002/05/14 18:05:35  ingmar
// comments
//
// Revision 1.40  2002/05/13 14:26:03  ingmar
// added GetVoxelAtPackedBlockOffsets() GetPosAsPackedBlockOffsetsFromPosXyz()
// and fixed comments
//
// Revision 1.39  2002/05/11 00:13:35  ingmar
// added NextBlockX() Y() Z()
//
// Revision 1.38  2002/05/09 22:17:27  ingmar
// towards improving the speed of IsNotAtEnd() throughthe use of 3 offset max values
//
// Revision 1.37  2002/05/09 18:23:16  ingmar
// Restricted m_iPosX/Y/Z to onlt keep the position outside the current block scope. Inside an ofset is used instead.
// Resulted in the efficient traversal now being at the same speed as *poniter++ of a linear array
//
// Revision 1.36  2002/05/08 19:13:52  tushar
// made vxVolume complatible with float4
//
// Revision 1.35  2002/04/30 21:29:04  soeren
// added GetVoxel at iterator pos
//
// Revision 1.34  2002/04/30 15:30:17  soeren
// added firstnonbackgroundzyxinsideblock
//
// Revision 1.33  2002/04/30 14:38:57  soeren
// removed two functions
//
// Revision 1.32  2002/04/29 21:09:50  soeren
// added SetVoxel from iterator
//
// Revision 1.31  2002/04/29 16:02:23  soeren
// added get packed offsets of neighbor
//
// Revision 1.30  2002/04/29 15:41:14  soeren
// added SetVoxel at neighbor pos
//
// Revision 1.29  2002/04/28 00:42:57  soeren
// added SeVoxelfroIterator
//
// Revision 1.28  2002/04/27 23:18:38  soeren
// added SetPos from Iterator
//
// Revision 1.27  2002/04/27 18:19:36  ingmar
// added SetVoxelAtPackedBlockOffsetsPos
// and GetVoxelAtPackedBlockOffsetsPos
//
// Revision 1.26  2002/04/26 17:31:13  jaddonisio
// added next non background -
//
// Revision 1.25  2002/04/24 18:54:05  soeren
// added set clear block function
//
// Revision 1.24  2002/04/24 18:40:40  soeren
// fixed background bug and added clear of the whole volume by value function
//
// Revision 1.23  2002/04/24 13:14:23  michael
// made GetPos{X,Y,Z} public instead of private
//
// Revision 1.22  2002/04/24 03:04:02  michael
// added method to clear the current block of the iterator's location
//
// Revision 1.21  2002/04/24 01:42:57  soeren
// optimized NextBlockZyx
//
// Revision 1.20  2002/04/23 19:20:26  ingmar
// fixed operator++
//
// Revision 1.19  2002/04/23 17:37:50  ingmar
// added m_pCurrentBlockStart andm_pCurrentBlockEnd
//
// Revision 1.18  2002/04/23 16:43:46  ingmar
// added FirstNonBackgroundBlockZYX() and FirstNonBackgroundZYX()
//
// Revision 1.17  2002/04/23 16:23:32  ingmar
// added traversal inside a block
//
// Revision 1.16  2002/04/18 19:55:38  ingmar
// removed obsolete GetVoxelRaw() from vxBlockVolumeIterator class
// and renamed GetVoxelRaw(offset) to GetVoxelAtLinOffset(offset)
//
// Revision 1.15  2002/04/17 20:39:32  ingmar
// changed parameters GetSample
//
// Revision 1.14  2002/04/17 20:32:28  ingmar
// added GetSample
//
// Revision 1.13  2002/04/17 14:14:29  ingmar
// added nextnonBackgroundBlockZYX
//
// Revision 1.12  2002/04/17 13:58:35  ingmar
// renamed NextBlock() to NextBlockZYX()
//
// Revision 1.11  2002/04/17 00:58:36  ingmar
// added SetBlock()
//
// Revision 1.10  2002/04/16 20:25:18  ingmar
// added BlockVolume as a friend
//
// Revision 1.9  2002/04/16 18:53:53  ingmar
// added GetDataBlockPointer() and NextBlock()
//
// Revision 1.8  2002/04/16 16:47:20  ingmar
// added SetPosRel() with new parameters
//
// Revision 1.7  2002/04/11 18:45:15  ingmar
// added IsNotAtEnd
//
// Revision 1.6  2002/04/08 23:23:37  michael
// added two more iterators NextXZY(), NextYXZ()
//
// Revision 1.5  2002/04/08 15:26:44  ingmar
// added fast constants for volume dimensions
//
// Revision 1.4  2002/04/05 19:13:53  ingmar
// fixed PeekPrevXYZ() and started to use inheritance from enum class
//
// Revision 1.3  2002/04/05 15:27:06  ingmar
// added new Peek function
//
// Revision 1.2  2002/04/04 19:08:56  ingmar
// fixed some bugs in Peek and code beautifiy
//
// Revision 1.1  2002/04/04 14:02:38  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeIterator.h,v 1.6.10.1 2010/07/16 20:31:28 dongqing Exp $
// $Id: vxBlockVolumeIterator.h,v 1.6.10.1 2010/07/16 20:31:28 dongqing Exp $
