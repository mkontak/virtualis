// $Id: vxBlockVolumeIteratorConsts.h,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren (ingmar|soeren)@viatronix.com

#ifndef vxBlockVolumeIteratorConsts_h
#define vxBlockVolumeIteratorConsts_h

/**
 *  Define constants for 30_vxVolume
 */
 
class VX_BLOCK_VOLUME_DLL vxBlockVolumeIteratorConsts 
{
public:

  // Scope  Pos          OR  Scope  Offset
  //                                
  // L0Vox  PosNum  X        L0Vox  OffsetDelta X  
  // L1Blk  PosMax  Y        L1Blk  OffsetNum   Y  
  // L2Vol  PosBits Z        L2Vol  OffsetBits  Z  
  // Total  PosMask (All)    Total  OffsetMask  (All)

  /// An enumeration of constants.  These should be immediately replaced by actual m_iXXX constants as soon as everyone moves to MSVC 7.0!!!
  enum LEVEL_ONE : int4
  {
    /// only inside Level 0 (Block of Voxels), number of positions, along any axis
    eL0VoxPosNum = 16,

    /// only inside Level 0 (Block of Voxels), maximum position, along any axis
    eL0VoxPosMax = 15, //eL0VoxPosNum-1,

    /// only inside Level 0 (Block of Voxels), bits to represent position, along any axis
    eL0VoxPosBits = 4,

    /// only inside Level 0 (Block of Voxels), mask for bits to represent position, along any axis, 0xF == 00000 00 1111
    eL0VoxPosMask = 0xF,

    /// only inside Level 0 (Block of Voxels), inverse of mask for bits to represent position, along any axis, 0xFFFFFFF0 == 11111 11 0000
    ///eL0VoxPosMaskNot = 0xFFFFFFF0,


    /// only inside Level 0 (Block of Voxels), delta to next offset, along X-axis
    eL0VoxOffsetDeltaX = 1,

    /// only inside Level 0 (Block of Voxels), delta to next offset, along Y-axis
    eL0VoxOffsetDeltaY = 16, //eL0VoxPosNum,

    /// only inside Level 0 (Block of Voxels), delta to next offset, along Z-axis
    eL0VoxOffsetDeltaZ = 256, //eL0VoxPosNum * eL0VoxPosNum,

    /// only inside Level 0 (Block of Voxels), number of possible offsets/Voxels, all within complete block
    eL0VoxOffsetNum    = 4096, //eL0VoxPosNum * eL0VoxPosNum * eL0VoxPosNum,

    /// only inside Level 0 (Block of Voxels), number of Voxels, all within complete block
    eL0VoxVoxelNum     = 4096, //eL0VoxOffsetNum,


    /// only inside Level 0 (Block of Voxels), bits to represent offset, all within complete block
    eL0VoxOffsetBits   = 12, //3 * eL0VoxPosBits,


    /// only inside Level 0 (Block of Voxels), mask on offset for bits to represent position, along X-axis, 0x00F == 0000 0000 1111
    eL0VoxOffsetMaskX = 0x00F,

    /// only inside Level 0 (Block of Voxels), mask on offset for bits to represent position, along Y-axis, 0x0F0 == 0000 1111 0000
    eL0VoxOffsetMaskY = 0x0F0,

    /// only inside Level 0 (Block of Voxels), mask on offset for bits to represent position, along Z-axis, 0xF00 == 1111 0000 0000
    eL0VoxOffsetMaskZ = 0xF00,

    /// only inside Level 0 (Block of Voxels), mask for bits to represent offset, all within complete block, 0x0FFF = 0000 1111 1111 1111
    eL0VoxOffsetMaskAll= 0xFFF,




    /// only inside Level 1 (Block of Blocks), number of positions, along any axis
    eL1BlkPosNum = 4,

    /// only inside Level 1 (Block of Blocks), maximum position, along any axis
    eL1BlkPosMax = 3, //eL1BlkPosNum-1,

    /// only inside Level 1 (Block of Blocks), bits to represent position, along any axis
    eL1BlkPosBits = 2,

    /// only inside Level 1 (Block of Blocks), mask for bits to represent position, along any axis, 0x03 == 00000 11 0000
    eL1BlkPosMask = 0x030,


    /// only inside Level 1 (Block of Blocks), delta to next offset, along X-axis
    eL1BlkOffsetDeltaX = 1,

    /// only inside Level 1 (Block of Blocks), delta to next offset, along Y-axis
    eL1BlkOffsetDeltaY = 4, //eL1BlkPosNum,

    /// only inside Level 1 (Block of Blocks), delta to next offset, along Z-axis
    eL1BlkOffsetDeltaZ = 16, //eL1BlkPosNum * eL1BlkPosNum,

    /// only inside Level 1 (Block of Blocks), number of possible offsets, all within complete block
    eL1BlkOffsetNum    = 64, //eL1BlkPosNum * eL1BlkPosNum * eL1BlkPosNum,


    /// only inside Level 1 (Block of Blocks), bits to represent offset, all within complete block
    eL1BlkOffsetBits   = 6, //3 * eL1BlkPosBits,

    /// only inside Level 1 (Block of Blocks), mask on offset for bits to represent position, along X-axis, 0x03 == 00 00 11
    eL1BlkOffsetMaskX = 0x03,

    /// only inside Level 1 (Block of Blocks), mask on offset for bits to represent position, along Y-axis, 0x0C == 00 11 00
    eL1BlkOffsetMaskY = 0x0C,

    /// only inside Level 1 (Block of Blocks), mask on offset for bits to represent position, along Z-axis, 0x30 == 11 00 00
    eL1BlkOffsetMaskZ = 0x30,

    /// only inside Level 1 (Block of Blocks), mask on offset for bits to represent offset, all within complete block, 0x0FFF = 00 11 11 11
    eL1BlkOffsetMaskAll= 0x3F,



    /// combined Level 1 (Block of Blocks) and Level 0 (Block of Voxels), number of positions, along any axis
    eL1BlkL0VoxPosNum = 64, //eL1BlkPosNum * eL0VoxPosNum,

    /// combined Level 1 (Block of Blocks) and Level 0 (Block of Voxels), maximum position, along any axis
    eL1BlkL0VoxPosMax = 63, //eL1BlkL0VoxPosNum-1,




    /// only inside Level 2 (complete volume Block), number of positions, along X-axis
    eL2VolPosNumX = 32,
 
    /// only inside Level 2 (complete volume Block), number of positions, along Y-axis
    eL2VolPosNumY = 32,

    /// only inside Level 2 (complete volume Block), number of positions, along Z-axis
    eL2VolPosNumZ = 32,

    /// only inside Level 2 (complete volume Block), maximum position, along X-axis
    eL2VolPosMaxX = 31, //eL2VolPosNumX-1,

    /// only inside Level 2 (complete volume Block), maximum position, along X-axis
    eL2VolPosMaxY = 31, //eL2VolPosNumY-1,

    /// only inside Level 2 (complete volume Block), maximum position, along X-axis
    eL2VolPosMaxZ = 31, //eL2VolPosNumZ-1,

    /// only inside Level 2 (complete volume Block), bits to represent position, along X-axis
    eL2VolPosBitsX = 5,

    /// only inside Level 2 (complete volume Block), bits to represent position, along Y-axis
    eL2VolPosBitsY = 5,

    /// only inside Level 2 (complete volume Block), bits to represent position, along Z-axis
    eL2VolPosBitsZ = 5,

    /// only inside Level 2 (complete volume Block), mask for bits to represent position, along X-axis, 0x7C0 == 11111 00 0000
    eL2VolPosMaskX = 0x7C0,

    /// only inside Level 2 (complete volume Block), mask for bits to represent position, along Y-axis, 0x7C0 == 11111 00 0000
    eL2VolPosMaskY = 0x7C0,

    /// only inside Level 2 (complete volume Block), mask for bits to represent position, along Z-axis, 0x7C0 == 11111 00 0000
    eL2VolPosMaskZ = 0x7C0,


    /// only inside Level 2 (complete volume Block), delta to next offset, along X-axis
    eL2VolOffsetDeltaX = 1,

    /// only inside Level 2 (complete volume Block), delta to next offset, along Y-axis
    eL2VolOffsetDeltaY = 32, //eL2VolPosNumX,

    /// only inside Level 2 (complete volume Block), delta to next offset, along Z-axis
    eL2VolOffsetDeltaZ = 1024, //eL2VolPosNumX * eL2VolPosNumY,

    /// only inside Level 2 (complete volume Block), number of possible offsets, all within complete block
    eL2VolOffsetNum = 32768, //eL2VolPosNumX * eL2VolPosNumY * eL2VolPosNumZ,


    /// only inside Level 2 (complete volume Block), bits to represent offset, all within complete block
    eL2VolOffsetBits = 15, //eL2VolPosBitsX + eL2VolPosBitsY + eL2VolPosBitsZ,

    /// only inside Level 2 (complete volume Block), mask for bits to represent offset, along X-axis, 0x001F = 00000 00000 11111
    eL2VolOffsetMaskX = 0x001F,

    /// only inside Level 2 (complete volume Block), mask for bits to represent offset, along Y-axis, 0x03E0 = 00000 11111 00000
    eL2VolOffsetMaskY = 0x03E0,

    /// only inside Level 2 (complete volume Block), mask for bits to represent offset, along Z-axis, 0x7C00 = 11111 00000 00000
    eL2VolOffsetMaskZ = 0x7C00,

    /// only inside Level 2 (complete volume Block), mask for bits to represent offset, all within complete block, 0x7FFF = 11111 11111 11111
    eL2VolOffsetMaskAll = 0x7FFF,



    /// total volume, number of positions, along X-axis
    eTotalPosNumX = 2048, //eL0VoxPosNum * eL1BlkPosNum * eL2VolPosNumX,

    /// total volume, number of positions, along Y-axis
    eTotalPosNumY = 2048, //eL0VoxPosNum * eL1BlkPosNum * eL2VolPosNumY,

    /// total volume, number of positions, along Z-axis
    eTotalPosNumZ = 2048, //eL0VoxPosNum * eL1BlkPosNum * eL2VolPosNumZ,

    /// total volume, maximum position, along X-axis
    eTotalPosMaxX = 2047, //eTotalPosNumX-1,

    /// total volume, maximum position, along X-axis
    eTotalPosMaxY = 2047, //eTotalPosNumY-1,

    /// total volume, maximum position, along X-axis
    eTotalPosMaxZ = 2047, //eTotalPosNumZ-1,

    /// total volume, bits to represent position, along X-axis
    eTotalPosBitsX = 11, //eL0VoxPosBits + eL1BlkPosBits + eL2VolPosBitsX,

    /// total volume, bits to represent position, along Y-axis
    eTotalPosBitsY = 11, //eL0VoxPosBits + eL1BlkPosBits + eL2VolPosBitsY,

    /// total volume, bits to represent position, along Z-axis
    eTotalPosBitsZ = 11, //eL0VoxPosBits + eL1BlkPosBits + eL2VolPosBitsZ,


    /// total volume, bits to represent position, along X-axis, 0x7FF =  11111 11 1111
    eTotalPosMaskX = 0x7FF,

    /// total volume, bits to represent position, along Y-axis, 0x3FF =  11111 11 1111
    eTotalPosMaskY = 0x7FF,

    /// total volume, bits to represent position, along Z-axis, 0x3FF = 11111 11 1111
    eTotalPosMaskZ = 0x7FF,
  };

  enum LEVEL_TWO : uint8
  {
    /// 00000 00 0000   0000 00 0000   0000 00 1111
    eLinearOffsetMaskForL0VoxPosX = 0x00000F,   

    /// 00000 00 0000   00000 00 1111   00000 00 0000
    eLinearOffsetMaskForL0VoxPosY = 0x007800,   

    /// 00000 00 1111   00000 00 0000   00000 00 0000
    eLinearOffsetMaskForL0VoxPosZ = 0x3C00000,   

    /// 00000 00 0000   00000 00 0000   00000 11 0000
    eLinearOffsetMaskForL1BlkPosX = 0x0000030,  

    /// 00000 00 0000   00000 11 0000   00000 00 0000
    eLinearOffsetMaskForL1BlkPosY = 0x0018000,  

    /// 00000 11 0000   00000 00 0000   00000 00 0000
    eLinearOffsetMaskForL1BlkPosZ = 0xC000000,  

    /// 00000 00 0000   00000 00 0000   11111 00 0000
    eLinearOffsetMaskForL2VolPosX = 0x000007C0, 

    /// 00000 00 0000   11111 00 0000   00000 00 0000
    eLinearOffsetMaskForL2VolPosY = 0x003E0000, 

    /// 11111 00 0000   00000 00 0000   00000 00 0000
    eLinearOffsetMaskForL2VolPosZ = 0x1F0000000,
  };

}; // vxBlockVolumeIteratorConsts


#endif // vxBlockVolumeIteratorConsts_h


// $Log: vxBlockVolumeIteratorConsts.h,v $
// Revision 1.1.1.1.34.1  2010/07/16 20:31:28  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2003/05/27 13:19:12  frank
// code review
//
// Revision 1.11  2003/05/17 16:19:40  michael
// reformatting
//
// Revision 1.10  2003/05/16 14:56:54  dongqing
// code review
//
// Revision 1.9  2003/05/16 11:54:10  michael
// code review
//
// Revision 1.8  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.7  2002/05/21 20:42:29  ingmar
// added sectioning for doxygen documentation
//
// Revision 1.6  2002/05/09 16:17:07  ingmar
// replaces computations by actual numeric values to ease debugging
//
// Revision 1.5  2002/05/07 22:39:59  ingmar
// changed to coding standard compliant enums
//
// Revision 1.4  2002/04/16 21:28:35  ingmar
// enabled write
//
// Revision 1.3  2002/04/16 18:52:05  ingmar
// two more enums
//
// Revision 1.2  2002/04/04 21:23:24  ingmar
// made m_uPosMaskL2Vol  axis specific (z uses one bit more than x and y)
//
// Revision 1.1  2002/04/04 14:02:38  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeIteratorConsts.h,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
// $Id: vxBlockVolumeIteratorConsts.h,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
