// $Id: vxBlockVolumeIterator.inl,v 1.6.2.1 2010/07/16 20:31:28 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren (ingmar|soeren)@viatronix.com


#undef Trilin
#define Trilin(x,y,z,a,b,c,d,e,f,g,h,ab,cd,ef,gh,abcd,efgh,final)      \
                                 \
  ab = x*(b-a) + a;              \
  cd = x*(d-c) + c;              \
  ef = x*(f-e) + e;              \
  gh = x*(h-g) + g;              \
                                 \
  abcd = y*(cd-ab) + ab;         \
  efgh = y*(gh-ef) + ef;         \
  final = (z*(efgh-abcd) + abcd);


/**
 * Constructor
 * @param volume
 * @param position
 */
template <class VoxelType>
vxBlockVolumeIterator<VoxelType>::vxBlockVolumeIterator( vxBlockVolume<VoxelType> & vol,  const Vector3D<int4> & position ) : 
  m_rVol(vol), 
  m_Background(0),
  m_iVolumeDimensionX(m_rVol.GetHeader().GetVolDim().X()),
  m_iVolumeDimensionY(m_rVol.GetHeader().GetVolDim().Y()),
  m_iVolumeDimensionZ(m_rVol.GetHeader().GetVolDim().Z()),
  m_L2VolOffsetVolMax(GetOffsetL2VolFromPosXyz(m_iVolumeDimensionX-1, m_iVolumeDimensionY-1, m_iVolumeDimensionZ-1)),
  m_L1BlkOffsetVolMax(GetOffsetL1BlkFromPosXyz(m_iVolumeDimensionX-1, m_iVolumeDimensionY-1, m_iVolumeDimensionZ-1)),
  m_L0VoxOffsetVolMax(GetOffsetL0VoxFromPosXyz(m_iVolumeDimensionX-1, m_iVolumeDimensionY-1, m_iVolumeDimensionZ-1)) 
//*******************************************************************
{
  m_ClearBlockVox = m_rVol.m_bgBlockL0Vox;
  m_vpClearBlocks.push_back(&m_rVol.m_bgBlockL0Vox);
  
  // All the members are not initialized in the C++ Style, because of strange behavior;
  m_iBlockPosX     = position.X() & 0x1FFFFFFF0;
  m_iBlockPosY     = position.Y() & 0x1FFFFFFF0;
  m_iBlockPosZ     = position.Z() & 0x1FFFFFFF0;
  m_L0VoxOffset = GetOffsetL0VoxFromPos(position);

  m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
  m_pL0       = m_pL1->GetDataPtr() + GetOffsetL1Blk();
  m_pVox      = m_pL0->GetDataPtr() + GetOffsetL0Vox();
} // constructor


/**
 * Destructor	
 */
template <class VoxelType>
vxBlockVolumeIterator<VoxelType>::~vxBlockVolumeIterator()
//*******************************************************************
{
  std::vector< vxBlock < VoxelType > * >::iterator  iter;
  for (iter = m_vpClearBlocks.begin() + 1;iter != m_vpClearBlocks.end() ; iter++)
  {
    delete (*iter);
  }
}

  
/**
 * Returns the offset that corresponds to the given position.
 * @return the offset.
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetOffsetL2Vol() const
//*******************************************************************
{                                 
  return ((m_iBlockPosX & eL2VolPosMaskX) >> 6)+   // 11111 +
         ((m_iBlockPosY & eL2VolPosMaskY) >> 1 )+  // 11111 00000 +
         ((m_iBlockPosZ & eL2VolPosMaskZ) << 4 );  // 11111 00000 00000
} // GetOffsetL2Vol	()																																																																																																																																			


/**
 * Returns the offset that corresponds to the given position
 * @return the offset
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetOffsetL1Blk() const
//*******************************************************************
{
  return ((m_iBlockPosX & eL1BlkPosMask) >> eL0VoxPosBits  )+  // 11 +
         ((m_iBlockPosY & eL1BlkPosMask) >> 2 )+               // 11 00 +
         ((m_iBlockPosZ & eL1BlkPosMask) );                    // 11 00 00
} // GetOffsetL1Blk()


/**
 * Returns the offset that corresponds to the given position
 * @param position
 * @return the offset
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetOffsetL0VoxFromPos(const Vector3D<int4> & position) const
//*******************************************************************
{
  return (position.X() & eL0VoxPosMask) +
         ((position.Y() & eL0VoxPosMask) << eL0VoxPosBits )+ 
         ((position.Z() & eL0VoxPosMask) << 8); // 8 = (2*eL0VoxPosBits)
} // GetOffsetL0VoxFromPos()



/**
 * Returns the offset that corresponds to the given position.
 * @param position
 * @return the offset
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetOffsetL1BlkFromPos(const Vector3D<int4> & position) const
//*******************************************************************
{
  return ((position.X() & eL1BlkPosMask) >> eL0VoxPosBits )+
         ((position.Y() & eL1BlkPosMask) >> 2)+      // 2 = (eL0VoxPosBits - eL1BlkPosBits)
         ((position.Z() & eL1BlkPosMask) );
} // GetOffsetL1BlkFromPos()


/**
 * Returns the offset that corresponds to the given position
 * @param position
 * @return the offset
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetOffsetL2VolFromPos(const Vector3D<int4> & position) const
//*******************************************************************
{
  return ((position.X() & eL2VolPosMaskX) >> 6) +    // 6 = eL0VoxPosBits + eL1BlkPosBits
         ((position.Y() & eL2VolPosMaskY) >> 1) +    // 1 = eL0VoxPosBits + eL1BlkPosBits - eL2VolPosBitsX
         ((position.Z() & eL2VolPosMaskZ) << 4);
} // GetOffsetL2VolFromPos	()																																																																																																																																			



/**
 * Returns the offset that corresponds to the given position
 * @param position in X
 * @param position in Y
 * @param position in Z
 * @return the offset
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetOffsetL0VoxFromPosXyz(const int4 iPosX, 
                                                                      const int4 iPosY, 
                                                                      const int4 iPosZ) const
//*******************************************************************
{
  return ((iPosX & eL0VoxPosMask) +
         ((iPosY & eL0VoxPosMask) << eL0VoxPosBits )+
         ((iPosZ & eL0VoxPosMask) << 8));     // 8 = (2*eL0VoxPosBits)
} // GetOffsetL0Vox


/**
 * Returns the offset that corresponds to the given position
 * @param position in X
 * @param position in Y
 * @param position in Z
 * @return the offset
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetOffsetL1BlkFromPosXyz(const int4 iPosX, 
                                                                      const int4 iPosY, 
                                                                      const int4 iPosZ) const
//*******************************************************************
{
  return ((iPosX & eL1BlkPosMask) >> eL0VoxPosBits )+
         ((iPosY & eL1BlkPosMask) >> 2 )+     // 2 = (eL0VoxPosBits - eL1BlkPosBits)
         ((iPosZ & eL1BlkPosMask) );
} // GetOffsetL0Vox()


/**
 * Returns the offset that corresponds to the given position
 * @param position in X
 * @param position in Y
 * @param position in Z
 * @return the offset
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetOffsetL2VolFromPosXyz(const int4 iPosX, const int4 iPosY, const int4 iPosZ) const
//*******************************************************************
{
  return ((iPosX & eL2VolPosMaskX) >> 6 )+   // 6 = (eL0VoxPosBits + eL1BlkPosBits)
         ((iPosY & eL2VolPosMaskY) >> 1 )+   // 1 = (eL0VoxPosBits + eL1BlkPosBits - eL2VolPosBitsX)
         ((iPosZ & eL2VolPosMaskZ) << 4 ); 
} // GetOffsetL0Vox()



/**
 * Returns the offset that corresponds to the given position
 * @param position
 * @return the offset
 */
template <class VoxelType>
const uint8 vxBlockVolumeIterator<VoxelType>::GetLinVolOffset(const Vector3D<int4> & position) const
//*******************************************************************
{
  return (uint8(position.X() & eTotalPosMaskX) + 
         (uint8(position.Y() & eTotalPosMaskY) << eTotalPosBitsX) + 
         (uint8(position.Z() & eTotalPosMaskZ) << 22) );   // 22 = (eTotalPosBitsX + eTotalPosBitsY)
  //return (uint8(position.X() & eTotalPosMaskX) + 
  //       (uint8(position.Y() & eTotalPosMaskY) << eTotalPosBitsX) + 
  //       (uint8(position.Z() & eTotalPosMaskZ) << 22) ) & 0x1FFFFFFFF;   // 22 = (eTotalPosBitsX + eTotalPosBitsY)
} // GetLinVolOffset()


/**
 * Returns the offset that corresponds to the given position
 * @param position in X
 * @param position in Y
 * @param position in Z
 * @return the offset
 */
template <class VoxelType>
const uint8 vxBlockVolumeIterator<VoxelType>::GetLinVolOffset(const int4 iPosX, 
                                                             const int4 iPosY, 
                                                             const int4 iPosZ) const
//*******************************************************************
{
  return (uint8(iPosX & eTotalPosMaskX) + 
         (uint8(iPosY & eTotalPosMaskY) << eTotalPosBitsX)+ 
         (uint8(iPosZ & eTotalPosMaskZ) << 22) );    // 22 = (eTotalPosBitsX + eTotalPosBitsY)
  //return (uint8(iPosX & eTotalPosMaskX) + 
  //       (uint8(iPosY & eTotalPosMaskY) << eTotalPosBitsX)+ 
  //       (uint8(iPosZ & eTotalPosMaskZ) << 22) ) & 0x1FFFFFFFF;    // 22 = (eTotalPosBitsX + eTotalPosBitsY)
} // GetLinVolOffset()


/**
 * Returns the offset that corresponds to the given position
 * @return the offset
 */
template <class VoxelType>
const uint8 vxBlockVolumeIterator<VoxelType>::GetLinVolOffset() const
//*******************************************************************
{
  return (uint8(GetPosX() & eTotalPosMaskX) + 
         (uint8(GetPosY() & eTotalPosMaskY) << eTotalPosBitsX)+ 
         (uint8(GetPosZ() & eTotalPosMaskZ) << 22) );    // 22 = (eTotalPosBitsX + eTotalPosBitsY)
  //return (uint8(GetPosX() & eTotalPosMaskX) + 
  //       (uint8(GetPosY() & eTotalPosMaskY) << eTotalPosBitsX)+ 
  //       (uint8(GetPosZ() & eTotalPosMaskZ) << 22) ) & 0x1FFFFFFFF;    // 22 = (eTotalPosBitsX + eTotalPosBitsY)
} // GetLinVolOffset()		


/**
 * Moves the position to the next non-background voxel in a non-specified order.
 * This implementation is the default, but may be overwritten in a more 
 * optimized way for a particular volume layout.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::FirstNonBackgroundZYX()
//*******************************************************************
{
  SetPos(0,0,0);
  while (IsNotAtEnd() && (GetVoxel() == m_Background))
  {
    NextZYX();
  }
} // FirstNonBackgroundZYX()


/**
 * Moves the position to the next non-background voxel in a non-specified order.
 * This implementation is the default, but may be overwritten in a more 
 * optimized way for a particular volume layout.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::FirstNonBackgroundZYXinsideBlock()
//*******************************************************************
{
  SetPos(m_iBlockPosX, m_iBlockPosY, m_iBlockPosZ);
  while (IsNotAtEndOfBlock() && (GetVoxel() == m_Background))
  {
    NextZYXinsideBlock();
  }
}  // FirstNonBackgroundZYXinsideBlock()


/**
 * Moves the position to the next non-background block in ZYX order.  
 * 
 * @param VoxelType
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::FirstNonBackgroundBlockZYX()
//*******************************************************************
{
  SetPos(0,0,0);
  while (!m_pL0->IsDirty() && IsNotAtEnd())
  {
    NextBlockZYX();
  }
}

/**
 * Moves the position to the next non-background voxel in a non-specified order.
 * This implementation is the default, but may be overwritten in a more 
 * optimized way for a particular volume layout.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextNonBackgroundZYX()
//*******************************************************************
{
  do
  {
    NextZYX();
  } while (GetVoxel() == m_Background);
}  // NextNonBackgroundZYX()


/**
 * Moves the position to the next non-background voxel according to 
 * a ZYX traversal only inside the current block.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextNonBackgroundZYXinsideBlock()
//*******************************************************************
{
  do
  {
    NextZYXinsideBlock();
  } while (IsNotAtEndOfBlock() && GetVoxel() == m_Background);
}  // NextNonBackgroundZYXinsideBlock()


/**
 * Moves the position to the next non-background block in ZYX order
 * @param VoxelType
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextNonBackgroundBlockZYX()
//*******************************************************************
{
  do
  {
    NextBlockZYX();
  }while (!m_pL0->IsDirty() && IsNotAtEnd());
} // NextNonBackgroundBlockZYX()


/**
 * Moves the iterator to the next voxel according to a ZYX traversal.
 * imitates triple for () loop behavior
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::PreviousZYX()
//*******************************************************************
{
  if (((m_L0VoxOffset & eL0VoxOffsetMaskX) > 0) || GetPosX() > 0)
  {
    DecX(); return;
  }
  else
  {
    SetPosX(m_iVolumeDimensionX - 1);
  }
  if ((m_L0VoxOffset & eL0VoxOffsetMaskY) > 0 || GetPosY() > 0)
  {
    DecY(); return;
  }
  else
  {
    SetPosY(m_iVolumeDimensionY - 1);
  }
  if ((m_L0VoxOffset & eL0VoxOffsetMaskZ) > 0 || GetPosZ() > 0)
  {
    DecZ(); 
    return; // may inc outside the volume
  }
  else
  {
    SetPos(0,0,-1);
  }
} // PreviousZYX()


/**
 * Moves the iterator to the beginning of the next block.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextBlockXsameOffset()
//*******************************************************************
{
  SetPosX(GetPosX() + eL0VoxPosNum);
}


/**
 * Moves the iterator to the next block in Y-direction AND maintains the offset inside the block
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextBlockYsameOffset()
//*******************************************************************
{
  SetPosY(GetPosY() + eL0VoxPosNum);
}


/**
 * Moves the iterator to the beginning of the next block.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextBlockZsameOffset()
//*******************************************************************
{
  SetPosZ(GetPosZ() + eL0VoxPosNum);
}


/**
 * Moves the iterator to the beginning of the next block.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextBlockZYX()
//*******************************************************************
{
  // then update position and pointers
  if (m_iBlockPosX < (m_iVolumeDimensionX-eL0VoxPosNum))
  {
    SetPos(m_iBlockPosX + eL0VoxPosNum, m_iBlockPosY, m_iBlockPosZ);
    return;
  }

  if (m_iBlockPosY < (m_iVolumeDimensionY-eL0VoxPosNum))
  {
    SetPos(0, m_iBlockPosY + eL0VoxPosNum, m_iBlockPosZ);
    return;
  }

  if (m_iBlockPosZ < (m_iVolumeDimensionZ-eL0VoxPosNum))
  {
    SetPos(0, 0, m_iBlockPosZ + eL0VoxPosNum);
    return;
  }
  else
  {
    SetPos(m_iVolumeDimensionX, m_iVolumeDimensionY, m_iVolumeDimensionZ);
  }
} // NextBlockZYX()


/**
 * Moves the iterator to the next voxel according to a ZYX traversal.
 * imitates triple for () loop behavior
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextZYX()
//*******************************************************************
{
  if (GetPosX() < (m_iVolumeDimensionX-1))
  {
    IncX(); return;
  }
  else
  {
    SetPosX(0);
  }
  if (GetPosY() < (m_iVolumeDimensionY-1))
  {
    IncY(); return;
  }
  else
  {
    SetPosY(0);
  }
  if (GetPosZ() < (m_iVolumeDimensionZ-1))
  {
    IncZ(); return;
  }
  else
  {
    SetPos(m_iVolumeDimensionX, m_iVolumeDimensionY, m_iVolumeDimensionZ);
  }
} // NextZYX()


/**
 * Moves the iterator to the next voxel according to a XZY traversal.
 * imitates triple for () loop behavior
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextXZY()
//*******************************************************************
{
  if (GetPosY() < (m_iVolumeDimensionY-1))
  {
    IncY(); return;
  }
  else
  {
    SetPosY(0);
  }
  if (GetPosZ() < (m_iVolumeDimensionZ-1))
  {
    IncZ(); return; // may inc outside the volume
  }
  else
  {
    SetPosZ(0);
  }
  if (GetPosX() < (m_iVolumeDimensionX-1))
  {
    IncX(); return;
  }
  else
  {
    SetPos(m_iVolumeDimensionX, m_iVolumeDimensionY, m_iVolumeDimensionZ);
  }
} // NextXZY()


/**
 * Moves the iterator to the next voxel according to a ZYX traversal.
 * imitates triple for () loop behavior
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextYXZ()
//*******************************************************************
{
  if (GetPosZ() < (m_iVolumeDimensionZ-1))
  {
    IncZ(); return; // may inc outside the volume
  }
  else
  {
    SetPosZ(0);
  }
  if (GetPosX() < (m_iVolumeDimensionX-1))
  {
    IncX(); return;
  }
  else
  {
    SetPosX(0);
  }
  if (GetPosY() < (m_iVolumeDimensionY-1))
  {
    IncY(); return;
  }
  else
  {
    SetPos(m_iVolumeDimensionX, m_iVolumeDimensionY, m_iVolumeDimensionZ);
  }
} // NextYXZ()


/**
 * Status of the volume traversal.
 * @return true if the iterator has been advanced 
 *         bejond the volume boundaries.
 */
template <class VoxelType>
bool vxBlockVolumeIterator<VoxelType>::IsAtEnd() const
//*******************************************************************
{
  return !((GetPosX() < m_iVolumeDimensionX) && (GetPosY() < m_iVolumeDimensionY) && (GetPosZ() < m_iVolumeDimensionZ));
} // IsAtEnd


/**
 * Status of the volume traversal.
 * @return false if the iterator has been advanced 
 *         bejond the volume boundaries.
 */
template <class VoxelType>
bool vxBlockVolumeIterator<VoxelType>::IsNotAtEnd() const
//*******************************************************************
{
  return (GetPosX() < m_iVolumeDimensionX && 
          GetPosY() < m_iVolumeDimensionY && 
          GetPosZ() < m_iVolumeDimensionZ);
} // IsNotAtEnd()


/**
 * Moves the iterator to the next voxel according to a ZYX traversal only inside the current block.
 * imitates triple for () loop behavior
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextZYXinsideBlock()
//*******************************************************************
{
  ++m_pVox;
  ++m_L0VoxOffset;
} // NextZYXinsideBlock()


/**
 * Moves the iterator to the next voxel according to a XZY traversal only inside the current block.
 * imitates triple for () loop behavior
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextXZYinsideBlock()
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskY) < eL0VoxOffsetMaskY)
  {
    m_pVox += eL0VoxOffsetDeltaY;
    m_L0VoxOffset += eL0VoxOffsetDeltaY;
    return; 
  }
  else
  {
    SetPosY(m_iBlockPosY);
  }

  if ((m_L0VoxOffset & eL0VoxOffsetMaskZ) < eL0VoxOffsetMaskZ)
  {
    m_pVox += eL0VoxOffsetDeltaZ;
    m_L0VoxOffset += eL0VoxOffsetDeltaZ;
    return;
  }
  else
  {
    SetPosZ(m_iBlockPosZ);
  }

  if ((GetPosX() & eL0VoxPosMax) < eL0VoxPosMax)
  {
    m_pVox += eL0VoxOffsetDeltaX;
    m_L0VoxOffset += eL0VoxOffsetDeltaX;
    return;
  }
  else
  {
    m_L0VoxOffset = eL0VoxOffsetNum;
  }
} // NextXZYinsideBlock()


/**
 * Moves the iterator to the next voxel according to a YXZ traversal only inside the current block.
 * imitates triple for () loop behavior
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::NextYXZinsideBlock()
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskZ) < eL0VoxOffsetMaskZ)
  {
    m_pVox += eL0VoxOffsetDeltaZ;
    m_L0VoxOffset += eL0VoxOffsetDeltaZ;
    return; 
  }
  else
  {
    SetPosZ(m_iBlockPosZ);
  }

  if ((m_L0VoxOffset & eL0VoxOffsetMaskX) < eL0VoxOffsetMaskX)
  {
    m_pVox += eL0VoxOffsetDeltaX;
    m_L0VoxOffset += eL0VoxOffsetDeltaX;
    return;
  }
  else
  {
    SetPosX(m_iBlockPosX);
  }

  if ((m_L0VoxOffset & eL0VoxOffsetMaskY) < eL0VoxOffsetMaskY)
  {
    m_pVox += eL0VoxOffsetDeltaY;
    m_L0VoxOffset += eL0VoxOffsetDeltaY;
  }
  else
  {
    m_L0VoxOffset = eL0VoxOffsetNum;
  }
} // NextYXZinsideBlock()


/**
 * Status of the volume traversal
 * @return false if the iterator has been advanced 
 *         bejond the volume boundaries.
 */
template <class VoxelType>
bool vxBlockVolumeIterator<VoxelType>::IsNotAtEndOfBlock() const
//*******************************************************************
{
  return m_L0VoxOffset < eL0VoxOffsetNum;
}


/**
 * Status of the volume traversal
 * @return false if the iterator has been advanced 
 *         bejond the volume boundaries.
 */
template <class VoxelType>
bool vxBlockVolumeIterator<VoxelType>::IsAtEndOfBlock() const
//*******************************************************************
{
  return (m_L0VoxOffset >= eL0VoxOffsetNum);
}


/**
 * Status of the volume traversal
 * @return false if the iterator has been advanced 
 *         bejond the volume boundaries
 */
template <class VoxelType>
bool vxBlockVolumeIterator<VoxelType>::IsInsideOfBlock() const
//*******************************************************************
{
  return (0 <= m_L0VoxOffset && m_L0VoxOffset < eL0VoxOffsetNum);
}


/**
 * Returns the value at the current location.
 * Is background if the location is outside the volume.
 * @return const reference to the voxel.
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::GetVoxel() const
//*******************************************************************
{
  return *m_pVox;
}


/**
 * Returns the value at the equivalent linear volume offset location
 * @param offset
 * @return const reference to the voxel
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::GetVoxelAtPackedBlockOffsets(const uint8 uOffsets) const     
//*******************************************************************
{
  return m_rVol.m_BlockL2Vol.At( uint4(uOffsets >> 18) & eL2VolOffsetMaskAll ).
                             At( uint4(uOffsets >> eL0VoxOffsetBits) & eL1BlkOffsetMaskAll ).
                             At( uint4(uOffsets) & eL0VoxOffsetMaskAll );
} // GetVoxelAtPackedBlockOffsets()


/**
 * Returns the value at the equivalent linear volume offset location
 * @param offset
 * @return const reference to the voxel
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::GetVoxelAtLinOffset(const uint8 uOffset) const
//*******************************************************************
{
  return m_rVol.m_BlockL2Vol.At(uint4((uOffset & 0x1F0000000) >> 18) |
                                uint4((uOffset & 0x00F800000) >> 18) |
                                uint4((uOffset & 0x0007C0000) >>  18)).
                             At(uint4((uOffset & 0x30000) >> 12) |
                                uint4((uOffset & 0x0C000) >> 12) |
                                uint4((uOffset & 0x03000) >> 12)).
                             At(uint4(uOffset & 0xF00) |
                                uint4(uOffset & 0x0F0) |
                                uint4(uOffset & 0x00F));
} // GetVoxelAtLinOffset()


/**
 * Returns the pointer to the block of voxels corresponding to the current location
 * @return pointer to the data block
 */
template <class VoxelType>
const VoxelType * vxBlockVolumeIterator<VoxelType>::GetDataPointer() const
//*******************************************************************
{
  return m_pL0->GetDataPtr();
}


/**
 * Returns the current block of voxels corresponding to the current location
 * @return pointer to the data block
 */
template <class VoxelType>
const vxBlock<VoxelType> & vxBlockVolumeIterator<VoxelType>::GetCurrentBlock() const
//*******************************************************************
{
  return *m_pL0;
}


/**
 * Sets the current block of voxels corresponding to the current location
 * @return block
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetCurrentBlock(const vxBlock<VoxelType> & block)
//*******************************************************************
{
  if (m_pL1->IsUnShared() && m_pL0->IsUnShared())
  {
    *m_pL0 = block;  
    m_pVox  = m_pL0->GetDataPtr() + GetOffsetL0Vox();
  }
  else
  {
    m_pL1->UnShare();
    m_pL0   = m_pL1->GetDataPtr() + GetOffsetL1Blk();    
    *m_pL0 = block;  
    m_pVox  = m_pL0->GetDataPtr() + GetOffsetL0Vox();
  }
} // SteCurrentBlock()


/**
 * Sets the value at the current location.
 * May be garbage or segfault if the location is not valid
 * @param value
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetVoxel(const VoxelType & value)
//*******************************************************************
{
  if (m_pL1->IsUnShared() && m_pL0->IsUnShared())
  {
    *m_pVox = value;

    // this is inefficient but needed because there is either
    // a bug or it really is needed. It appears as if a block
    // becomes unshared but then succesively performed SetVoxel()
    // results in it becoming homogeneous. Hence, it is not 
    // shared and the transition from homogenous but unshared!!
    // to not homogeneous is lost.
    //////////////////////////////////////////////////////////////
    m_pL0->m_pBlockInfo->m_bIsHomogenous = false;

    return;
  }
  else
  {
    SetVoxelRawElse(value);
  }
} // SetVoxel()


/** 
 * Sets the value at the current location; Safe version
 * @param vlaue
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetVoxelNextX(const VoxelType & value)
//*******************************************************************
{
  if ((((GetPosX()+1) & eL0VoxPosMax) != 0) && (m_pL0->IsUnShared()))
  {
    *(m_pVox + 1) = value;
  }
  else
  {
    SetVoxelAtXYZElse(GetPosX()+1,GetPosY(),GetPosZ(), value);
  }   
} // SetVoxelNextX()


/** 
 * Sets the value at the current location; Safe version
 * @param vlaue
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetVoxelNextY(const VoxelType & value)
//*******************************************************************
{
  if ((((GetPosY()+1) & eL0VoxPosMax) != 0) && (m_pL0->IsUnShared()))
  {
    *(m_pVox + 16) = value;
  }
  else
  {
    SetVoxelAtXYZElse(GetPosX(),GetPosY()+1,GetPosZ(), value);
  }   
} // SetVoxelNextY()


/** 
 * Sets the value at the current location; Safe version.
 * @param vlaue
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetVoxelNextZ(const VoxelType & value)
//*******************************************************************
{
  if ((((GetPosZ()+1) & eL0VoxPosMax) != 0) && (m_pL0->IsUnShared()))
  {
    *(m_pVox + 256) = value;
  }
  else
  {
    SetVoxelAtXYZElse(GetPosX(),GetPosY(),GetPosZ()+1, value);
  }   
} // SetVoxelNextZ()


/** 
 * Sets the value at the current location; Safe version.
 * @param value
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetVoxelPrevX(const VoxelType & value)
//*******************************************************************
{
  if (((GetPosX()-1 & eL0VoxPosMax) < eL0VoxPosMax) && (m_pL0->IsUnShared()))
  {
    *(m_pVox - 1) = value;
  }
  else
  {
    SetVoxelAtXYZElse(GetPosX() - 1,GetPosY(),GetPosZ(), value);
  }   
} // SetVoxelPrevX()


/**
 * Sets the value at the current location; Safe version.
 * @param value
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetVoxelPrevY(const VoxelType & value)
//*******************************************************************
{
  if (((GetPosY()-1 & eL0VoxPosMax) < eL0VoxPosMax) && (m_pL0->IsUnShared()))
  {
    *(m_pVox - 16) = value;
  }
  else
  {
    SetVoxelAtXYZElse(GetPosX(),GetPosY() - 1,GetPosZ(), value);
  }
} // SetVoxelPrevY()


/** 
 * Sets the value at the current location; Safe version.
 * @param value
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetVoxelPrevZ(const VoxelType & value)
//*******************************************************************
{
  if (((GetPosZ()-1 & eL0VoxPosMax) < eL0VoxPosMax) && (m_pL0->IsUnShared()))
  {
    *(m_pVox - 256) = value;
  }
  else
  {
    SetVoxelAtXYZElse(GetPosX(),GetPosY(),GetPosZ() - 1, value);
  }   
} // SetVoxelPrevZ()


/**
 * Sets the voxel at the offset position of all three blocks in the hierarchy packed into one int
 * @param offsets
 * @param value
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetVoxelAtPackedBlockOffsetsPos( uint8 iPackedBlockOffsets, 
                                                                        const VoxelType & voxelValue )
//*******************************************************************
{
  int4 iL2VolOffset( iPackedBlockOffsets >> 18 );    // 18 = eL0VoxOffsetBits + eL1BlkOffsetBits
  int4 iL1BlkOffset( int4(iPackedBlockOffsets >> eL0VoxOffsetBits) & eL1BlkOffsetMaskAll );
  int4 iL0VoxOffset( iPackedBlockOffsets & 0xFFF );

  vxBlockVolume<VoxelType>::BlockL1Blk * pL1  = m_rVol.m_BlockL2Vol.GetDataPtr() + iL2VolOffset;
  vxBlockVolume<VoxelType>::BlockL0Vox * pL0  = pL1->GetDataPtr() + iL1BlkOffset;
  VoxelType                            * pVox = pL0->GetDataPtr() + iL0VoxOffset;

  if (pL1->IsUnShared() && pL0->IsUnShared())
  {
    *pVox = voxelValue;

    // this is inefficient but needed because there is either
    // a bug or it really is needed. It appears as if a block
    // becomes unshared but then succesively performed SetVoxel()
    // results in it becoming homogeneous. Hence, it is not 
    // shared and the transition from homogenous but unshared!!
    // to not homogeneous is lost.
    //////////////////////////////////////////////////////////////
    pL0->m_pBlockInfo->m_bIsHomogenous = false;
    return;
  }
  else
  {
    SetPosFromPackedBlockOffsets( iPackedBlockOffsets );
    SetVoxelRawElse(voxelValue);
  }
} // SetVoxelAtPackedBlockOffsetsPos()


/**
 * Gets the value from the delta offset of the current location
 * May be garbage or segfault if the location is not valid
 * @param delta vector
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::PeekRel(const Vector3D<int4> & deltaVector) const
//*******************************************************************
{
  return GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX() + deltaVector.X(), GetPosY() + deltaVector.Y(), GetPosZ() + deltaVector.Z()));
} // PeekRel()


/**
 * Gets the value from the delta offset of the current location
 * May be garbage or segfault if the location is not valid
 * @param delta X
 * @param delta X
 * @param delta X
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::PeekRel(const int4 iDeltaX, 
                                                            const int4 iDeltaY, 
                                                            const int4 iDeltaZ) const
//*******************************************************************
{
  return GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX() + iDeltaX, GetPosY() + iDeltaY, GetPosZ() + iDeltaZ));
} // PeekRel()


/**
 * Gets the value from the delta offset of the current location.
 * May be garbage or segfault if the location is not valid.
 * @param position in X
 * @param position in Y
 * @param position in Z
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek(const int4 iPosX, 
                                                         const int4 iPosY, 
                                                         const int4 iPosZ) const
//*******************************************************************
{
  return GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(iPosX, iPosY, iPosZ));
} // PeekRel()


/**
 * Peeks at the next voxel in the x direction w/out moving the iterator
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek1px0py0pz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskX) < eL0VoxOffsetMaskX)
  {
    return *(m_pVox+eL0VoxOffsetDeltaX); // at a position that is not on the right edge of the L0 voxel block
  }
  else
  {
    return PeekNextXElse();
  }   
} // Peek1px0py0pz()


/**
 * Peeks at the next voxel in the y direction w/out moving the iterator
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek0px1py0pz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskY) < eL0VoxOffsetMaskY)
  {
    return *(m_pVox+eL0VoxOffsetDeltaY);  // at a position that is not on the right edge of the L0 voxel block
  }
  else
  {
    return PeekNextYElse();
  } 
} // Peek0px1py0pz()


/**
 * Peeks at the next voxel in the z direction w/out moving the iterator.
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek0px0py1pz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskZ) < eL0VoxOffsetMaskZ)
  {
    return *(m_pVox+eL0VoxOffsetDeltaZ);
  }
  else
  {
    return PeekNextZElse();
  }
} // Peek0px0py1pz()


/**
 * Peeks at the prev voxel in the x direction w/out moving the iterator.
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek1nx0py0pz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskX) > 0)
  {
    return *(m_pVox-eL0VoxOffsetDeltaX);
  }
  else
  {
    return PeekPrevXElse();
  }
} // Peek1nx0py0pz()


/**
 * Peeks at the prev voxel in the y direction w/out moving the iterator
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek0px1ny0pz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskY) > 0)
  {
    return *(m_pVox-eL0VoxOffsetDeltaY);
  }
  else
  {
    return PeekPrevYElse();
  }  
} // Peek0px1ny0pz()


/**
 * Peeks at the prev voxel in the z direction w/out moving the iterator
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek0px0py1nz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskZ) > 0)
  {
    return *(m_pVox-eL0VoxOffsetDeltaZ);
  }
  else
  {
    return PeekPrevZElse();
  }
} // Peek0px0py1nz()


/**
 * Peeks at the next voxel in the xy direction w/out moving the iterator.
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek1px1py0pz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskX) < eL0VoxOffsetMaskX  &&  
       (m_L0VoxOffset & eL0VoxOffsetMaskY) < eL0VoxOffsetMaskY)
  {
    return *(m_pVox+17);   // 17 = eL0VoxOffsetDeltaX+eL0VoxOffsetDeltaY);
  }
  else
  {
    return PeekNextXYElse();
  }
} // Peek1px1py0pz()


/**
 * Peeks at the next voxel in the xz direction w/out moving the iterator.
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek1px0py1pz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskX) < eL0VoxOffsetMaskX  &&  
       (m_L0VoxOffset & eL0VoxOffsetMaskZ) < eL0VoxOffsetMaskZ)
  {
    return *(m_pVox+257);    //257 = eL0VoxOffsetDeltaX+eL0VoxOffsetDeltaZ);
  }
  else
  {
    return PeekNextXZElse();
  }
} // Peek1px0py1pz()


/**
 * Peeks at the next voxel in the xy direction w/out moving the iterator.
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek0px1py1pz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskY) < eL0VoxOffsetMaskY  &&  
       (m_L0VoxOffset & eL0VoxOffsetMaskZ) < eL0VoxOffsetMaskZ)
  {
    return *(m_pVox+272);    //272 = eL0VoxOffsetDeltaY+eL0VoxOffsetDeltaZ);
  }
  else
  {
    return PeekNextYZElse();
  }
} // Peek0px1py1pz()


/**
 * Peeks at the next voxel in the xyz direction w/out moving the iterator.
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek1px1py1pz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskX) < eL0VoxOffsetMaskX  &&  
       (m_L0VoxOffset & eL0VoxOffsetMaskY) < eL0VoxOffsetMaskY  &&  
       (m_L0VoxOffset & eL0VoxOffsetMaskZ) < eL0VoxOffsetMaskZ)
  {
    return *(m_pVox+273);    // 273 = eL0VoxOffsetDeltaX+eL0VoxOffsetDeltaY+eL0VoxOffsetDeltaZ);
  }
  else
  {
    return PeekNextXYZElse();
  }
} // Peek1px1py1pz()


/**
 * Peeks at the prev voxel in the xy direction w/out moving the iterator.
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek1nx1ny0pz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskX) > 0  &&  
       (m_L0VoxOffset & eL0VoxOffsetMaskY) > 0)
  {
    return *(m_pVox-17);    // -17 = -eL0VoxOffsetDeltaX-eL0VoxOffsetDeltaY);
  }
  else
  {
    return GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX()-1, GetPosY()-1, GetPosZ()));
  }
} // Peek1nx1ny0pz()


/**
 * Peeks at the prev voxel in the xy direction w/out moving the iterator.
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek1nx0py1nz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskX) > 0  &&  
       (m_L0VoxOffset & eL0VoxOffsetMaskZ) > 0)
  {
    return *(m_pVox-257);    // -257 = -eL0VoxOffsetDeltaX-eL0VoxOffsetDeltaZ);
  }
  else
  {
    return GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX()-1, GetPosY(), GetPosZ()-1));
  }
} // Peek1nx0py1nz()


/**
 * Peeks at the prev voxel in the yz direction w/out moving the iterator.
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek0px1ny1nz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskY) > 0  &&  
       (m_L0VoxOffset & eL0VoxOffsetMaskZ) > 0)
  {
    return *(m_pVox-272);    // -272 = -eL0VoxOffsetDeltaY-eL0VoxOffsetDeltaZ);
  }
  else
  {
    return GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX(), GetPosY()-1, GetPosZ()-1));
  }
} // Peek0px1ny1nz()


/**
 * Peeks at the prev voxel in the xyz direction w/out moving the iterator.
 * @return voxel value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::Peek1nx1ny1nz() const
//*******************************************************************
{
  if ((m_L0VoxOffset & eL0VoxOffsetMaskX) > 0  &&  
       (m_L0VoxOffset & eL0VoxOffsetMaskY) > 0  &&  
       (m_L0VoxOffset & eL0VoxOffsetMaskZ) > 0)
  {
    return *(m_pVox-273);    //-273 = -eL0VoxOffsetDeltaX-eL0VoxOffsetDeltaY-eL0VoxOffsetDeltaZ);
  }
  else
  {
    return GetVoxelAtPackedBlockOffsets(GetPosAsPackedBlockOffsetsFromPosXyz(GetPosX()-1, GetPosY()-1, GetPosZ()-1));
  }
} // Peek1nx1ny1nz()


/**
 * Test if it we can call quick peek functions (next x, next y or next XY in Z-Slice)
 * @return successful
 */
template <class VoxelType>
const bool vxBlockVolumeIterator<VoxelType>::PeekXYSafety() const
//*******************************************************************
{
  return ((m_L0VoxOffset & eL0VoxOffsetMaskX) < eL0VoxOffsetMaskX  &&  (m_L0VoxOffset & eL0VoxOffsetMaskY) < eL0VoxOffsetMaskY);
}


/**
 * Test if the pointer is right at the block corner (0,0)
 * @return successful
 */
template <class VoxelType>
const bool vxBlockVolumeIterator<VoxelType>::IsAtBlockCorner() const
//*******************************************************************
{
  return !(((m_L0VoxOffset & eL0VoxOffsetMaskX) !=0) || ((m_L0VoxOffset & eL0VoxOffsetMaskY) !=0));
}


/**
 * Quick peek neighbour in X
 * @return value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::QuickPeek1px0py0pz() const
//*******************************************************************
{
  return *(m_pVox + eL0VoxOffsetDeltaX);
}
  

/**
 * Quick peek neighbour in Y
 * @return value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::QuickPeek0px1py0pz() const
//*******************************************************************
{
  return *(m_pVox + eL0VoxOffsetDeltaY);
}


/**
 * Quick peek neighbour in X and Y
 * @return value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::QuickPeek1px1py0pz() const
//*******************************************************************
{
  return *(m_pVox + 17);    // 17 = eL0VoxOffsetDeltaX + eL0VoxOffsetDeltaY);
}


/**
 * Quick peek inside block
 * @param x offset
 * @param y offset
 * @return value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::QuickPeekInsideBlock(const int x, 
                                                                         const int y) const
//*******************************************************************
{
  return *(m_pVox + y * eL0VoxOffsetDeltaY + x);
}


/**
 * Quick peek inside block
 * @param x offset
 * @param y offset
 * @param z offset
 * @return value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::QuickPeekInsideBlock(const int x, 
                                                                         const int y, 
                                                                         const int z) const
//*******************************************************************
{
  return *(m_pVox + z * eL0VoxOffsetDeltaZ +  y * eL0VoxOffsetDeltaY + x);
}


/**
 * Quick peek inside block
 * @param offset
 * @return value
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIterator<VoxelType>::QuickPeekInsideBlock(const int iOffset) const
//*******************************************************************
{
  return *(m_pVox + iOffset);
}


/**
 * Returns the current x,y,z position
 * This is computed from the current voxel pointer offset
 * @return position
 */
template <class VoxelType>
const Vector3D<int4> vxBlockVolumeIterator<VoxelType>::GetPos() const
//*******************************************************************
{
  return Vector3D<int4>(GetPosX(), GetPosY(), GetPosZ());
} // GetPos()


/**
 * Returns the offset that corresponds to the given position
 * @return position in X
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetPosX() const
//*******************************************************************
{
  return m_iBlockPosX + (m_L0VoxOffset & eL0VoxOffsetMaskX);
} // GetPosX()


/**
 * Returns the offset that corresponds to the given position
 * @return position in Y
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetPosY() const
//*******************************************************************
{
  return m_iBlockPosY + ((m_L0VoxOffset & eL0VoxOffsetMaskY) >> 4);
} // GetPosY()


/**
 * Returns the offset that corresponds to the given position
 * @return position in Z
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetPosZ() const
//*******************************************************************
{
  return m_iBlockPosZ + ((m_L0VoxOffset & eL0VoxOffsetMaskZ) >> 8);
} // GetPosZ()

/**
 * Returns the current x,y,z position inside the block
 * This is computed from the current voxel pointer offset
 * @param position
 */
template <class VoxelType>
const Vector3D<int4> vxBlockVolumeIterator<VoxelType>::GetPosInsideBlock() const
//*******************************************************************
{
  return Vector3D<int4>(GetPosXInsideBlock(), GetPosYInsideBlock(), GetPosZInsideBlock());
} // GetPosInsideBlock()


/**
 * Returns the offset that corresponds to the given position
 * @return offset in X
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetPosXInsideBlock() const
//*******************************************************************
{
  return (m_L0VoxOffset & eL0VoxOffsetMaskX);
} // GetPosXInsideBlock()


/**
 * Returns the offset that corresponds to the given position
 * @return offset in Y
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetPosYInsideBlock() const
//*******************************************************************
{
  return ((m_L0VoxOffset & eL0VoxOffsetMaskY) >> 4);
} // GetPosYInsideBlock()


/**
 * Returns the offset that corresponds to the given position
 * @return offset in Z
 */
template <class VoxelType>
const int4 vxBlockVolumeIterator<VoxelType>::GetPosZInsideBlock() const
//*******************************************************************
{
  return ((m_L0VoxOffset & eL0VoxOffsetMaskZ) >> 8);
} // GetPosZInsideBlock()


/**
 * Sets the current x,y,z position.
 * This is computes and sets the corresponding voxel pointer offset
 * @param position
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetPos(const Vector3D<int4> & position)
//*******************************************************************
{
  //m_iBlockPosX     = position.X() & 0xFFFFfff0;
  //m_iBlockPosY     = position.Y() & 0xFFFFfff0;
  //m_iBlockPosZ     = position.Z() & 0xFFFFfff0;
  m_iBlockPosX     = position.X() & 0x1FFFFfff0;
  m_iBlockPosY     = position.Y() & 0x1FFFFfff0;
  m_iBlockPosZ     = position.Z() & 0x1FFFFfff0;
  m_L0VoxOffset = GetOffsetL0VoxFromPos(position);
  m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
  m_pL0       = m_pL1->GetDataPtr() + GetOffsetL1Blk();
  m_pVox      = m_pL0->GetDataPtr() + GetOffsetL0Vox();
} // SetPos()


/**
 * Sets the current x,y,z position.
 * This is computes and sets the corresponding voxel pointer offset.
 * @param position in X
 * @param position in Y
 * @param position in Z
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetPos(const int4 iX, 
                                              const int4 iY, 
                                              const int4 iZ)
//*******************************************************************
{
  m_iBlockPosX     = iX & 0x1FFFFfff0;
  m_iBlockPosY     = iY & 0x1FFFFfff0;
  m_iBlockPosZ     = iZ & 0x1FFFFfff0;
  m_L0VoxOffset = GetOffsetL0VoxFromPosXyz(iX, iY, iZ);
  m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
  m_pL0       = m_pL1->GetDataPtr() + GetOffsetL1Blk();
  m_pVox      = m_pL0->GetDataPtr() + GetOffsetL0Vox();
} // SetPos()


/**
 * Sets the current x,y,z position.
 * This is computes and sets the corresponding voxel pointer offset.
 * @param position in X
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetPosX(const int4 iX)
//*******************************************************************
{
  m_iBlockPosX     = iX & 0x1FFFFfff0;
  const int4 iY(GetPosY());
  const int4 iZ(GetPosZ());
  m_L0VoxOffset = GetOffsetL0VoxFromPosXyz(iX, iY, iZ); 

  m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
  m_pL0       = m_pL1->GetDataPtr() + GetOffsetL1Blk();
  m_pVox      = m_pL0->GetDataPtr() + GetOffsetL0Vox();
} // SetPosX()


/**
 * Sets the current x,y,z position.
 * This is computes and sets the corresponding voxel pointer offset.
 * @param position in Y
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetPosY(const int4 iY)
//*******************************************************************
{
  m_iBlockPosY     = iY & 0x1FFFFfff0; 
  const int4 iX(GetPosX());
  const int4 iZ(GetPosZ());
  m_L0VoxOffset = GetOffsetL0VoxFromPosXyz(iX, iY, iZ); 
  m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
  m_pL0       = m_pL1->GetDataPtr() + GetOffsetL1Blk();
  m_pVox      = m_pL0->GetDataPtr() + GetOffsetL0Vox();
} // SetPosY()


/**
 * Sets the current x,y,z position.
 * This is computes and sets the corresponding voxel pointer offset.
 * @param position in Z
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetPosZ(const int4 iZ)
//*******************************************************************
{
  m_iBlockPosZ     = iZ & 0x1FFFFfff0;
  const int4 iX(GetPosX());
  const int4 iY(GetPosY());
  m_L0VoxOffset = GetOffsetL0VoxFromPosXyz(iX, iY, iZ); 
  m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
  m_pL0       = m_pL1->GetDataPtr() + GetOffsetL1Blk();
  m_pVox      = m_pL0->GetDataPtr() + GetOffsetL0Vox();
} // SetPosZ()


/**
 * Sets the current x,y position.
 * This is computes and sets the corresponding voxel pointer offset.
 * @param position in X
 * @param position in Y
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetPosXY(const int4 iX, 
                                                const int4 iY)
//*******************************************************************
{
  m_iBlockPosX     = iX & 0x1FFFFfff0;
  m_iBlockPosY     = iY & 0x1FFFFfff0;
  const int4 iZ(GetPosZ());
  m_L0VoxOffset = GetOffsetL0VoxFromPosXyz(iX, iY, iZ); 
  m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
  m_pL0       = m_pL1->GetDataPtr() + GetOffsetL1Blk();
  m_pVox      = m_pL0->GetDataPtr() + GetOffsetL0Vox();
} // SetPosXY()


/**
 * Sets the current y,z position.
 * This is computes and sets the corresponding voxel pointer offset.
 * @param position in Y
 * @param position in Z
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetPosYZ(const int4 iY, 
                                                const int4 iZ)
//*******************************************************************
{
  m_iBlockPosY     = iY & 0x1FFFFfff0;
  m_iBlockPosZ     = iZ & 0x1FFFFfff0;
  const int4 iX(GetPosX());
  m_L0VoxOffset = GetOffsetL0VoxFromPosXyz(iX, iY, iZ); 
  m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
  m_pL0       = m_pL1->GetDataPtr() + GetOffsetL1Blk();
  m_pVox      = m_pL0->GetDataPtr() + GetOffsetL0Vox();
} // SetPosYZ()


/**
 * Sets the current x,z position
 * This is computes and sets the corresponding voxel pointer offset
 * @param position in X
 * @param position in Z
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetPosXZ(const int4 iX, 
                                                const int4 iZ)
//*******************************************************************
{
  m_iBlockPosX     = iX & 0x1FFFFfff0;
  m_iBlockPosZ     = iZ & 0x1FFFFfff0;
  const int4 iY(GetPosY());
  m_L0VoxOffset = GetOffsetL0VoxFromPosXyz(iX, iY, iZ); 
  m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
  m_pL0       = m_pL1->GetDataPtr() + GetOffsetL1Blk();
  m_pVox      = m_pL0->GetDataPtr() + GetOffsetL0Vox();
} // SetPosXZ()


/**
 * Sets the current x,y,z position as delta from previous position
 * This is computes and sets the corresponding voxel pointer offset
 * @param delta vector
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetPosRel(const Vector3D<int4> & deltaVector)
//*******************************************************************
{
  m_iBlockPosX += deltaVector.X() & 0x1FFFFfff0;
  m_iBlockPosY += deltaVector.Y() & 0x1FFFFfff0;
  m_iBlockPosZ += deltaVector.Z() & 0x1FFFFfff0;
  m_L0VoxOffset = GetOffsetL0VoxFromPosXyz(GetPosX()+deltaVector.X(), GetPosY()+deltaVector.Y(), GetPosZ()+deltaVector.Z());

  m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
  m_pL0       = m_pL1->GetDataPtr() + GetOffsetL1Blk();
  m_pVox      = m_pL0->GetDataPtr() + GetOffsetL0Vox();
} // SetPosRel()


/**
 * Sets the current x,y,z position as delta from previous position
 * This is computes and sets the corresponding voxel pointer offset
 * @param delta in X
 * @param delta in Y
 * @param delta in Z
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetPosRel(const int4 iDeltaX, 
                                                 const int4 iDeltaY, 
                                                 const int4 iDeltaZ)
//*******************************************************************
{
  m_iBlockPosX += iDeltaX & 0x1FFFFfff0;
  m_iBlockPosY += iDeltaY & 0x1FFFFfff0;
  m_iBlockPosZ += iDeltaZ & 0x1FFFFfff0;
  m_L0VoxOffset = GetOffsetL0VoxFromPosXyz(GetPosX()+iDeltaX, GetPosY()+iDeltaY, GetPosZ()+iDeltaZ);
  m_pL1       = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
  m_pL0       = m_pL1->GetDataPtr() + GetOffsetL1Blk();
  m_pVox      = m_pL0->GetDataPtr() + GetOffsetL0Vox();
} // SetPosRel()



/**
 * Returns the current offset position of all three blocks in the hierarchy packed into one int
 * @return packed offsets
 */
template <class VoxelType>
const uint8 vxBlockVolumeIterator<VoxelType>::GetPosAsPackedBlockOffsets() const
//*******************************************************************
{
  const uint8 iL2VolOffset(  static_cast< uint8 >(m_pL1  - m_rVol.m_BlockL2Vol.GetDataPtr()));
  const uint8 iL1BlkOffset(  static_cast< uint8 >(m_pL0  - m_pL1->GetDataPtr() ));

  return ((iL2VolOffset << 18) + 
          (iL1BlkOffset << eL0VoxOffsetBits) +
          uint8(m_L0VoxOffset));
} // GetPosAsPackedBlockOffsets()


/**
 * Returns the current offset position of all three blocks in the hierarchy packed into one int.
 * @param position in X
 * @param position in Y
 * @param position in Z
 * @return packed offsets
 */
template <class VoxelType>
const uint8 vxBlockVolumeIterator<VoxelType>::GetPosAsPackedBlockOffsetsFromPosXyz(const int4 iPosX, 
                                                                                  const int4 iPosY, 
                                                                                  const int4 iPosZ) const
//*******************************************************************
{
  const uint8 iL2VolOffset( GetOffsetL2VolFromPosXyz(iPosX, iPosY, iPosZ) );
  const uint8 iL1BlkOffset( GetOffsetL1BlkFromPosXyz(iPosX, iPosY, iPosZ) );
  const uint8 iL0VoxOffset( GetOffsetL0VoxFromPosXyz(iPosX, iPosY, iPosZ) );

  return ((iL2VolOffset << 18) + 
          (iL1BlkOffset << eL0VoxOffsetBits) +
          iL0VoxOffset);
} // GetPosAsPackedBlockOffsetsFromPosXyz()


/**
 * Returns the current offset position of all three blocks in the hierarchy packed into one int.
 * @return packed offsets
 */
template <class VoxelType>
const uint8 vxBlockVolumeIterator<VoxelType>::GetPosAsPackedBlockOffsetsNextX() const
//*******************************************************************
{
  const uint8 iL2VolOffset( GetOffsetL2VolFromPosXyz(GetPosX()+1, GetPosY(), GetPosZ()) );
  const uint8 iL1BlkOffset( GetOffsetL1BlkFromPosXyz(GetPosX()+1, GetPosY(), GetPosZ()) );
  const uint8 iL0VoxOffset( GetOffsetL0VoxFromPosXyz(GetPosX()+1, GetPosY(), GetPosZ()) );

  return ((iL2VolOffset << 18) + 
          (iL1BlkOffset << eL0VoxOffsetBits) +
          iL0VoxOffset);
} // GetPosAsPackedBlockOffsetsNextX()


/**
 * Returns the current offset position of all three blocks in the hierarchy packed into one int
 * @return packed offsets
 */
template <class VoxelType>
const uint8 vxBlockVolumeIterator<VoxelType>::GetPosAsPackedBlockOffsetsNextY() const
//*******************************************************************
{
  const uint8 iL2VolOffset( GetOffsetL2VolFromPosXyz(GetPosX(), GetPosY()+1, GetPosZ()) );
  const uint8 iL1BlkOffset( GetOffsetL1BlkFromPosXyz(GetPosX(), GetPosY()+1, GetPosZ()) );
  const uint8 iL0VoxOffset( GetOffsetL0VoxFromPosXyz(GetPosX(), GetPosY()+1, GetPosZ()) );

  return ((iL2VolOffset << 18) + 
          (iL1BlkOffset << eL0VoxOffsetBits) +
          iL0VoxOffset);
} // GetPosAsPackedBlockOffsetsNextY()

/**
 * Returns the current offset position of all three blocks in the hierarchy packed into one int.
 * @return packed offsets
 */
template <class VoxelType>
const uint8 vxBlockVolumeIterator<VoxelType>::GetPosAsPackedBlockOffsetsNextZ() const
//*******************************************************************
{
  const uint8 iL2VolOffset( GetOffsetL2VolFromPosXyz(GetPosX(), GetPosY(), GetPosZ()+1) );
  const uint8 iL1BlkOffset( GetOffsetL1BlkFromPosXyz(GetPosX(), GetPosY(), GetPosZ()+1) );
  const uint8 iL0VoxOffset( GetOffsetL0VoxFromPosXyz(GetPosX(), GetPosY(), GetPosZ()+1) );

  return ((iL2VolOffset << 18) + 
          (iL1BlkOffset << eL0VoxOffsetBits) +
          iL0VoxOffset);
} // GetPosAsPackedBlockOffsetsNextZ()


/**
 * Returns the current offset position of all three blocks in the hierarchy packed into one int.
 * @return packed offsets
 */
template <class VoxelType>
const uint8 vxBlockVolumeIterator<VoxelType>::GetPosAsPackedBlockOffsetsPrevX() const
//*******************************************************************
{
  const uint8 iL2VolOffset( GetOffsetL2VolFromPosXyz(GetPosX()-1, GetPosY(), GetPosZ()) );
  const uint8 iL1BlkOffset( GetOffsetL1BlkFromPosXyz(GetPosX()-1, GetPosY(), GetPosZ()) );
  const uint8 iL0VoxOffset( GetOffsetL0VoxFromPosXyz(GetPosX()-1, GetPosY(), GetPosZ()) );

  return ((iL2VolOffset << 18) + 
          (iL1BlkOffset << eL0VoxOffsetBits) +
          iL0VoxOffset);
} // GetPosAsPackedBlockOffsetsPrevX()


/**
 * Returns the current offset position of all three blocks in the hierarchy packed into one int.
 * @return packed offsets
 */
template <class VoxelType>
const uint8 vxBlockVolumeIterator<VoxelType>::GetPosAsPackedBlockOffsetsPrevY() const
//*******************************************************************
{
  const uint8 iL2VolOffset( GetOffsetL2VolFromPosXyz(GetPosX(), GetPosY()-1, GetPosZ()) );
  const uint8 iL1BlkOffset( GetOffsetL1BlkFromPosXyz(GetPosX(), GetPosY()-1, GetPosZ()) );
  const uint8 iL0VoxOffset( GetOffsetL0VoxFromPosXyz(GetPosX(), GetPosY()-1, GetPosZ()) );

  return ((iL2VolOffset << 18) + 
         (iL1BlkOffset << eL0VoxOffsetBits) +
         iL0VoxOffset);
} // GetPosAsPackedBlockOffsetsPrevY()


/**
 * Returns the current offset position of all three blocks in the hierarchy packed into one int.
 * @return packed offsets
 */
template <class VoxelType>
const uint8 vxBlockVolumeIterator<VoxelType>::GetPosAsPackedBlockOffsetsPrevZ() const
//*******************************************************************
{
  const uint8 iL2VolOffset( GetOffsetL2VolFromPosXyz(GetPosX(), GetPosY(), GetPosZ()-1) );
  const uint8 iL1BlkOffset( GetOffsetL1BlkFromPosXyz(GetPosX(), GetPosY(), GetPosZ()-1) );
  const uint8 iL0VoxOffset( GetOffsetL0VoxFromPosXyz(GetPosX(), GetPosY(), GetPosZ()-1) );

  return ((iL2VolOffset << 18) + 
          (iL1BlkOffset << eL0VoxOffsetBits) +
          iL0VoxOffset);
} // GetPosAsPackedBlockOffsetsPrevZ()


/**
 * Sets the current offset position of all three blocks(?) in the hierarchy packed into one int.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetPosFromPackedBlockOffsets( uint8 iPackedBlockOffsets )
//*******************************************************************
{
  int4 iL2VolOffset( (iPackedBlockOffsets >> 18) );    // 18 = (eL0VoxOffsetBits + eL1BlkOffsetBits)
  int4 iL1BlkOffset( int4(iPackedBlockOffsets >> eL0VoxOffsetBits) & eL1BlkOffsetMaskAll );
  m_L0VoxOffset = int4(iPackedBlockOffsets & 0xFFF);

  m_pL1  = m_rVol.m_BlockL2Vol.GetDataPtr() + iL2VolOffset;
  m_pL0  = m_pL1->GetDataPtr() + iL1BlkOffset;
  m_pVox = m_pL0->GetDataPtr() + m_L0VoxOffset;

  m_iBlockPosX = ((iL2VolOffset & 0x001F)  << 6) + ((iL1BlkOffset & 0x03) << 4);
  m_iBlockPosY = ((iL2VolOffset & 0x03E0)  << 1) + ((iL1BlkOffset & 0x0C) << 2);
  m_iBlockPosZ = ((iL2VolOffset & 0x7C00)  >> 4) + (iL1BlkOffset & 0x30);

} // SetPosFromPackedBlockOffsets()


/**
 * Increments the iterator location in the x direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::IncX()
//*******************************************************************
{
  ++m_L0VoxOffset;

  if ((m_L0VoxOffset & eL0VoxOffsetMaskX) != 0)
  {
    ++m_pVox;   
    return;     
  }
  else
  {
    IncXElse();
  }
} // IncX


/**
 * Decrements the iterator location in the x direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::DecX()
//*******************************************************************
{
  --m_L0VoxOffset;

  if ((m_L0VoxOffset & eL0VoxOffsetMaskX) < eL0VoxOffsetMaskX)
  {
    --m_pVox;   
    return;     
  }
  else
  {
    DecXElse();
  }
} // DecX


/**
 * Increments the iterator location in the x direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::IncXbyDelta(const int4 & iDelta)
//*******************************************************************
{
  SetPosX(GetPosX() + iDelta);
} // IncX


/**
 * Increments the iterator location in the y direction
 * This is computes and sets the corresponding voxel pointer offset
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::IncY()
//*******************************************************************
{
  m_L0VoxOffset += eL0VoxOffsetDeltaY;

  if ((m_L0VoxOffset & eL0VoxOffsetMaskY) != 0)
  {
    m_pVox += eL0VoxOffsetDeltaY;
    return;
  }
  else
  {
    IncYElse();
  }
} // IncY()


/**
 * Decrements the iterator location in the y direction
 * This is computes and sets the corresponding voxel pointer offset
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::DecY()
//*******************************************************************
{
  m_L0VoxOffset -= eL0VoxOffsetDeltaY;

  if ((m_L0VoxOffset & eL0VoxOffsetMaskY) < eL0VoxOffsetMaskY)
  {
    m_pVox -= eL0VoxOffsetDeltaY;
    return;
  }
  else
  {
    DecYElse();
  }
} // DecY()


/**
 * Increments the iterator location in the y direction
 * This is computes and sets the corresponding voxel pointer offset
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::IncYbyDelta(const int4 & iDelta)
//*******************************************************************
{
  SetPosY(GetPosY() + iDelta);
}


/**
 * Increments the iterator location in the z direction
 * This is computes and sets the corresponding voxel pointer offset
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::IncZ()
//*******************************************************************
{
  m_L0VoxOffset += eL0VoxOffsetDeltaZ;  

  if ((m_L0VoxOffset & eL0VoxOffsetMaskZ) != 0)
  {
    m_pVox += eL0VoxOffsetDeltaZ;
    return;
  }
  else
  {
    IncZElse();
  }
} // IncZ()


/**
 * Decrements the iterator location in the z direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::DecZ()
//*******************************************************************
{
  m_L0VoxOffset -= eL0VoxOffsetDeltaZ;

  if ((m_L0VoxOffset & eL0VoxOffsetMaskZ) < eL0VoxOffsetMaskZ)
  {
    m_pVox -= eL0VoxOffsetDeltaZ;
    return;
  }
  else
  {
    DecZElse();
  }
} // DecZ()


/**
 * Increments the iterator location in the z direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::IncZbyDelta(const int4 & iDelta)
//*******************************************************************
{
  SetPosZ(GetPosZ() + iDelta);
}


/**
 * Decrements the iterator location in the z direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::DecZbyDelta(const int4 & iDelta)
//*******************************************************************
{
  /// THIS NEEDS TO BE OPTIMIZED
  SetPosZ(GetPosZ() - iDelta);
}


/**
 * Decrements the iterator location in the x direction.
 * This is computes and sets the corresponding voxel pointer offset.
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::DecXbyDelta(const int4 & iDelta)
//*******************************************************************
{
  /// THIS NEEDS TO BE OPTIMIZED
  SetPosX(GetPosX() - iDelta);
}


/**
 * Decrements the iterator location in the y direction
 * This is computes and sets the corresponding voxel pointer offset
 * @param delta
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::DecYbyDelta(const int4 & iDelta)
//*******************************************************************
{
  /// THIS NEEDS TO BE OPTIMIZED
  SetPosY(GetPosY() - iDelta);
}


/**
 * Returns true if the iterator is inside the volume.
 * This is computed from the current voxel pointer, hence
 * overruns in x and y are NOThandled corretly,
 * but it is FAST.
 */
template <class VoxelType>
bool vxBlockVolumeIterator<VoxelType>::IsInsideVolume() const
//*******************************************************************
{
  return (GetPosX() < m_iVolumeDimensionX) && (GetPosY() < m_iVolumeDimensionY) && (GetPosZ() < m_iVolumeDimensionZ) &&
         (GetPosX() >= 0) && (GetPosY() >= 0) && (GetPosZ() >= 0);
} // IsInsideVolume


/**
 * Clears current Block to current clear block
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::ClearCurrentBlock()
//*******************************************************************
{
  m_L0VoxOffset = 0;

  if (m_pL1->IsUnShared())
  {
    (*m_pL0) = m_ClearBlockVox;
    m_pVox   = m_pL0->GetDataPtr() + GetOffsetL0Vox();
    return;
  }
  else
  {
    m_pL1->UnShare();
    m_pL1    = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
    m_pL0    = m_pL1->GetDataPtr() + GetOffsetL1Blk();
    (*m_pL0) = m_ClearBlockVox;
    m_pVox   = m_pL0->GetDataPtr() + GetOffsetL0Vox();
  }
} // ClearCurrentBlock()


/**
 * Sets clear block
 * @param clear value
 */
template <class VoxelType>
void vxBlockVolumeIterator<VoxelType>::SetClearBlock(VoxelType clearValue)
//*******************************************************************
{
  std::vector< vxBlock < VoxelType > * >::iterator iter;
  for (iter = m_vpClearBlocks.begin();iter != m_vpClearBlocks.end() ; iter++)
  {
    if((*iter)->At(0) == clearValue)
    {
      m_ClearBlockVox = *(*iter);
      return;
    }
  }
  m_vpClearBlocks.push_back(new vxBlock<VoxelType>(eL0VoxVoxelNum, clearValue, true));
  m_ClearBlockVox = *(*(m_vpClearBlocks.end() - 1));

  return;
} // SetClearBlock()


/**
 * Overloaded << Operator.
 * @param iterator 
 * @return out stream
 */
template <class VoxelType>
std::ostream & operator<<(std::ostream & os, 
                          const vxBlockVolumeIterator<VoxelType> & iter)
//*******************************************************************
{
  os << "vxBlockVolumeIterator{Pos:"<<iter.GetPos();
  if (iter.IsInsideVolume())
  {
    os << ",inside, Vox:" << iter.GetVoxelRaw();
  }
  else
  {
    os << ",outside,Vox:" <<iter.GetVoxel();
  }
  os << "}";
  return os;
} // operator<<()


#undef Trilin


#undef FILE_REVISION 


// Revision History:
// $Log: vxBlockVolumeIterator.inl,v $
// Revision 1.6.2.1  2010/07/16 20:31:28  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.6  2007/11/03 00:02:40  jmeade
// Merge_from_CON_30_on_20071102
//
// Revision 1.4.2.1  2007/10/22 20:14:42  gdavidson
// Issue #5510: Fixed creation of the blocks
//
// Revision 1.5  2007/09/12 21:57:46  gdavidson
// Removed unnecessary increment of reference count
//
// Revision 1.4  2007/02/15 00:08:24  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.3  2007/01/05 20:41:57  geconomos
// Properly incrementing reference count on  background block in constructor
//
// Revision 1.2  2006/03/29 19:49:20  geconomos
// code cleanup
//
// Revision 1.1.1.1.18.1  2007/02/12 20:48:14  jmeade
// Merge from VC_2-0-2-0-RC50_Branch on 070212
//
// Revision 1.1.1.1.22.1  2007/01/08 18:44:36  geconomos
// Properly incrementing reference count on  background block in constructor
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.73  2003/07/08 19:22:13  michael
// added fix for correct handling of homogeneous blocks. So far, it was only
// tested for being shared() on SetVoxel() but even an unshared block can later
// on become homogeneous which needs to be overwritten with subsequent
// SetVoxel() operations.
//
// Revision 1.72  2003/07/02 15:57:37  michael
// renamed member to reflect in name that it is the block position, not the actual
// iterator position.
//
// Revision 1.71  2003/05/17 16:19:40  michael
// reformatting
//
// Revision 1.70  2003/05/16 11:54:10  michael
// code review
//
// Revision 1.69  2003/05/02 18:29:12  michael
// initial code review
//
// Revision 1.68  2002/08/15 14:25:32  jaddonisio
// Changed Get functions to pass iPos by reference and not by value.
//
// Revision 1.67  2002/07/22 23:17:21  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.66  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.65  2002/07/02 01:22:39  soeren
// added clearblock array
//
// Revision 1.64  2002/06/27 19:01:17  soeren
// added GetPos functions inside a block
//
// Revision 1.63.2.2  2002/07/08 21:11:23  binli
// new quick peek function: pointer
//
// Revision 1.63  2002/06/20 19:18:55  binli
// quick peek function.
//
// Revision 1.62  2002/06/17 17:08:12  soeren
// fixed SetCurrentBLock
//
// Revision 1.61  2002/06/17 15:34:12  soeren
// added UpdateLabelRenderVolume
//
// Revision 1.60  2002/06/05 19:13:34  soeren
// Added Homogenous Flag
//
// Revision 1.59  2002/06/04 22:47:37  soeren
// new VolIterator Peek functions that allow peeking into a mix of positive and negative offsets
//
// Revision 1.58  2002/05/22 18:24:20  ingmar
// made some end of block and volume tests more robust
//
// Revision 1.57  2002/05/17 13:46:48  ingmar
// removed inefficient operator ++
//
// Revision 1.56  2002/05/15 18:15:42  soeren
// added isClear function in volume
//
// Revision 1.55  2002/05/15 15:47:28  ingmar
// NextBlockZ() ->NextBlockZsameOffset()
//
// Revision 1.54  2002/05/13 20:11:31  ingmar
// fixed GetVoxelAtPackedBlockOffsets()
//
// Revision 1.53  2002/05/13 14:26:34  ingmar
// added and used GetVoxelAtPackedBlockOffsets() GetPosAsPackedBlockOffsetsFromPosXyz()
//
// Revision 1.52  2002/05/11 00:13:58  ingmar
// added NextBlockX() Y() Z()
// and fixed DecZ()
//
// Revision 1.51  2002/05/09 22:17:27  ingmar
// towards improving the speed of IsNotAtEnd() throughthe use of 3 offset max values
//
// Revision 1.50  2002/05/09 18:23:16  ingmar
// Restricted m_iPosX/Y/Z to onlt keep the position outside the current block scope. Inside an ofset is used instead.
// Resulted in the efficient traversal now being at the same speed as *poniter++ of a linear array
//
// Revision 1.49  2002/05/08 19:13:52  tushar
// made vxVolume complatible with float4
//
// Revision 1.48  2002/05/08 18:04:38  soeren
// fixed Peek()
//
// Revision 1.47  2002/05/07 22:42:11  ingmar
// changed to coding standard compliant enums
//
// Revision 1.46  2002/05/03 21:11:10  soeren
// fixed firstnon.. functions
//
// Revision 1.45  2002/05/03 18:19:47  soeren
// fixed clearcurrentblock
//
// Revision 1.44  2002/05/03 17:08:15  frank
// added Backgroundflag in constructor
//
// Revision 1.43  2002/05/02 19:09:22  frank
// Changed number to constant.
//
// Revision 1.42  2002/04/30 15:30:17  soeren
// added firstnonbackgroundzyxinsideblock
//
// Revision 1.41  2002/04/30 14:53:27  ingmar
// fixed SetVoxel Prev/Next X/Y/Z
//
// Revision 1.40  2002/04/30 14:38:57  soeren
// removed two functions
//
// Revision 1.39  2002/04/29 23:11:02  soeren
// fixed NextNonBackground
//
// Revision 1.38  2002/04/29 16:02:23  soeren
// added get packed offsets of neighbor
//
// Revision 1.37  2002/04/29 15:41:14  soeren
// added SetVoxel at neighbor pos
//
// Revision 1.36  2002/04/27 18:19:36  ingmar
// added SetVoxelAtPackedBlockOffsetsPos
// and GetVoxelAtPackedBlockOffsetsPos
//
// Revision 1.35  2002/04/27 17:58:31  jaddonisio
// Fixed NextNonBackground().
//
// Revision 1.34  2002/04/26 19:56:08  soeren
// optimized
//
// Revision 1.33  2002/04/26 17:31:13  jaddonisio
// added next non background -
//
// Revision 1.32  2002/04/25 16:58:28  michael
// fixed bug in iterator update while walking in block
//
// Revision 1.31  2002/04/24 18:59:58  soeren
// optimized SetClearBlock
//
// Revision 1.30  2002/04/24 18:54:05  soeren
// added set clear block function
//
// Revision 1.29  2002/04/24 18:40:40  soeren
// fixed background bug and added clear of the whole volume by value function
//
// Revision 1.28  2002/04/24 03:04:02  michael
// added method to clear the current block of the iterator's location
//
// Revision 1.27  2002/04/24 01:42:57  soeren
// optimized NextBlockZyx
//
// Revision 1.26  2002/04/23 21:48:47  ingmar
// fixed NextinsideBlock
//
// Revision 1.25  2002/04/23 21:35:32  jaddonisio
// change something soeren
//
// Revision 1.24  2002/04/23 19:20:26  ingmar
// fixed operator++
//
// Revision 1.23  2002/04/23 17:37:50  ingmar
// added m_pCurrentBlockStart andm_pCurrentBlockEnd
//
// Revision 1.22  2002/04/23 16:43:46  ingmar
// added FirstNonBackgroundBlockZYX() and FirstNonBackgroundZYX()
//
// Revision 1.21  2002/04/23 16:33:48  ingmar
// optimized isAtEnd
//
// Revision 1.20  2002/04/23 16:23:32  ingmar
// added traversal inside a block
//
// Revision 1.19  2002/04/22 20:23:45  jaddonisio
// Stop 'NextNonBackgroundBlock' when outside volume.
//
// Revision 1.18  2002/04/18 19:55:38  ingmar
// removed obsolete GetVoxelRaw() from vxBlockVolumeIterator class
// and renamed GetVoxelRaw(offset) to GetVoxelAtLinOffset(offset)
//
// Revision 1.17  2002/04/17 20:39:32  ingmar
// changed parameters GetSample
//
// Revision 1.16  2002/04/17 20:32:28  ingmar
// added GetSample
//
// Revision 1.15  2002/04/17 14:14:29  ingmar
// added nextnonBackgroundBlockZYX
//
// Revision 1.14  2002/04/17 00:59:00  ingmar
// fixed NextBlock()
//
// Revision 1.13  2002/04/16 18:53:53  ingmar
// added GetDataBlockPointer() and NextBlock()
//
// Revision 1.12  2002/04/16 16:47:20  ingmar
// added SetPosRel() with new parameters
//
// Revision 1.11  2002/04/12 23:04:31  ingmar
// GetVolumeDimension()  -> GetVolDim()  ;-)
//
// Revision 1.10  2002/04/11 18:45:16  ingmar
// added IsNotAtEnd
//
// Revision 1.9  2002/04/10 18:07:46  ingmar
// worked on new header volume class
//
// Revision 1.8  2002/04/09 15:38:34  michael
// fixed iterator compare from >0 to !=0
//
// Revision 1.7  2002/04/08 23:23:37  michael
// added two more iterators NextXZY(), NextYXZ()
//
// Revision 1.6  2002/04/08 15:26:44  ingmar
// added fast constants for volume dimensions
//
// Revision 1.5  2002/04/05 19:13:53  ingmar
// fixed PeekPrevXYZ() and started to use inheritance from enum class
//
// Revision 1.4  2002/04/05 15:27:06  ingmar
// added new Peek function
//
// Revision 1.3  2002/04/04 21:22:31  ingmar
// fixed GetOffsetL2Vol()
//
// Revision 1.2  2002/04/04 19:08:56  ingmar
// fixed some bugs in Peek and code beautifiy
//
// Revision 1.1  2002/04/04 14:02:38  ingmar
// initial version
//
// Revision 1.12  2002/03/20 19:09:21  ingmar
// optimized, removed two fast unnecessary fast access data pointer
//
// Revision 1.11  2002/03/20 18:16:26  ingmar
// Changed At(0) to GetDataPtr()
//
// Revision 1.10  2002/03/19 15:50:45  ingmar
// proper comment indenting
//
// Revision 1.9  2002/03/18 23:02:14  ingmar
// code cleanup and move of non-inline parts to the .C
//
// Revision 1.8  2002/03/18 21:38:09  ingmar
// code cleanup and move of non-inline parts to the .C
//
// Revision 1.7  2002/03/13 20:16:34  ingmar
// removed virtual and added GetVolume()
//
// Revision 1.6  2002/03/13 16:12:17  ingmar
// allPeek() functions implemented
//
// Revision 1.5  2002/03/13 01:13:13  ingmar
// towards more Peek() functions
//
// Revision 1.4  2002/03/08 16:21:33  ingmar
// optimized setFuntion
//
// Revision 1.3  2002/03/08 00:43:37  ingmar
// removed inheritance of vxBlockVolumeIterator from VolIter. Stand alone allows inlining of small functions
//
// Revision 1.2  2002/03/07 01:47:36  ingmar
// removed commented GetGradientMagnitudeManhattanRaw code
//
// Revision 1.1  2002/03/06 18:36:54  ingmar
// intial version
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeIterator.inl,v 1.6.2.1 2010/07/16 20:31:28 dongqing Exp $
// $Id: vxBlockVolumeIterator.inl,v 1.6.2.1 2010/07/16 20:31:28 dongqing Exp $
