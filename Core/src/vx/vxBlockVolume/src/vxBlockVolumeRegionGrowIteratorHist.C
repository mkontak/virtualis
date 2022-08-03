// $Id: vxBlockVolumeRegionGrowIteratorHist.C,v 1.3.2.1 2010/07/16 20:31:42 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Soeren & Ingmar {soeren|ingmar}@viatronix.com


// includes
#include "StdAfx.h"
#include "vxBlockVolumeRegionGrowIteratorHist.h" // header
#include "vxIntensityConverter.h"


// defines
#define FILE_REVISION "$Revision: 1.3.2.1 $"


// namespaces
using namespace std;


/**
 * Constructor
 * @param data type of each voxel.
 * @param seedposition that must be part of the region.
 * @param volume to find the region in.
 * @param decides wether to inclue a voxel into the region.
 * @param mask of voxels that should NOT be touched (will be modified/extended by the region grow iterator).
 */
vxBlockVolumeRegionGrowIteratorHist::vxBlockVolumeRegionGrowIteratorHist(Vector3D<int4> seedPos, 
                                                                         vxBlockVolume<uint2> & dataVolume, 
                                                                         const vxDeciderVoxelValue<int4> & decider,
                                                                         vxBlockVolume<bool> * pMaskVolume,
                                                                         vxBlockVolume<bool> * pRegionTagVolume)
//*******************************************************************
:m_rDataVolume(dataVolume),
m_iterData(dataVolume, seedPos), 
m_iterHist(dataVolume, ( seedPos / 16.F ) * 16 ), // let's just see if this works
m_decider(decider),
m_pRegionTagVol((pRegionTagVolume != NULL) ? NULL : new vxBlockVolume<bool>(dataVolume.GetHeader())),
m_rRegionTagVol((pRegionTagVolume != NULL) ? *pRegionTagVolume : *m_pRegionTagVol),
m_pMaskVol((pMaskVolume != NULL) ? NULL : new vxBlockVolume<bool>(dataVolume.GetHeader())),
m_rMaskVol((pMaskVolume != NULL) ? *pMaskVolume : *m_pMaskVol),
m_iterRegion(m_rRegionTagVol, seedPos),
m_iterMask(m_rMaskVol, seedPos),
m_bGrowEnded(false),
m_maskOrgVol(m_rMaskVol),
m_iterMaskOrg(m_maskOrgVol),
m_uNumOfVoxels(0)
{
  if (!m_iterData.IsInsideVolume())
  {
    LogWrn("seed iterator not in volume","vxBlockVolumeRegionGrowIteratorHist","vxBlockVolumeRegionGrowIteratorHist");
  }

  m_NextVoxelOrQueuePtr = &vxBlockVolumeRegionGrowIteratorHist::NextNewVoxelOfRegion;
  
  // make sure the seedpoint is not locked and is part of the region
  if (m_iterMask.GetVoxel() == false && m_decider.IsPartOfRegion(m_iterData.GetVoxel()))
  { 
    m_uMaskBlockIsEntirelyInsideRegion  = 0;
    m_uMaskBlockIsEntirelyOutsideRegion = 0;

    uint2 uBinSize((vxIntensityConverter::GetMaxValue(dataVolume.GetHeader()) + 1) >> 5); // should be 128 for 12 bit data
    for (uint4 uBinIndex(0); uBinIndex<32; ++uBinIndex)
    {
      bool bIsEntirelyInside(true);

      for (uint4 uVoxel(uBinIndex*uBinSize); bIsEntirelyInside && uVoxel<((uBinIndex+1)*uBinSize); ++uVoxel)
      {
        bIsEntirelyInside = m_decider.IsPartOfRegion(uVoxel);
      }
      if (bIsEntirelyInside == true)
      {
        m_uMaskBlockIsEntirelyInsideRegion |= (1 << uBinIndex);
      }

      m_uMaskBlockHasSomeOutsideRegionVoxels = ~m_uMaskBlockIsEntirelyInsideRegion;
      
      bool bIsEntirelyOutside(true);

      for (uint4 uVoxel(uBinIndex*uBinSize); bIsEntirelyOutside && uVoxel<((uBinIndex+1)*uBinSize); ++uVoxel)
      {
        bIsEntirelyOutside = !m_decider.IsPartOfRegion(uVoxel);
      }
      if (bIsEntirelyOutside == true)
      {
        m_uMaskBlockIsEntirelyOutsideRegion |= (1 << uBinIndex);
      }
    }

    m_iterMask.SetClearBlock(true);
    m_iterMaskOrg.SetClearBlock(true);
    m_iterRegion.SetClearBlock(true);

    // test whether coresponding 16x16x16 cube block is entirely part of region
    if (IsEntireBlockInRegion(m_iterHist.GetCurrentBlock().GetBinaryHistogram(), m_iterMask))
    {
      m_queueOfBlock16.push(m_iterHist.GetPosAsPackedBlockOffsets());
      UpdateForFullyContainedBlock();
      m_NextVoxelOrQueuePtr = &vxBlockVolumeRegionGrowIteratorHist::NextNewBlock16OfRegion;
    }
    else
    {
      m_queueOfVoxels.push(m_iterData.GetPosAsPackedBlockOffsets());
      m_iterMask.SetVoxel(true);
      m_iterRegion.SetVoxel(true);
    }
    m_pLastBlock = &m_iterMask.GetCurrentBlock();
  }
  else
  {
    LogWrn("seed iterator not in region","vxBlockVolumeRegionGrowIteratorHist","vxBlockVolumeRegionGrowIteratorHist");
  }
} // constructor


/**
 * Destructor
 */
vxBlockVolumeRegionGrowIteratorHist::~vxBlockVolumeRegionGrowIteratorHist()
//*******************************************************************
{
  if (m_pMaskVol != NULL)
  {
    delete m_pMaskVol;
    m_pMaskVol = NULL;
  }
} // destructor


/**
 * Get as string
 * @return the current state of the iterator in a string
 */
std::string vxBlockVolumeRegionGrowIteratorHist::GetAsString() const
//*******************************************************************
{
  stringstream ss;

  ss << "vxBlockVolumeRegionGrowIteratorHist{Pos: " << m_iterData.GetPos();

  if (IsInsideVolume() == true)
  {
    ss << ", inside, Vox: ";
  }
  else
  {
    ss << ", outside,Vox: ";
  }
  ss << m_iterData.GetVoxel() << "}";

  return(string(ss.str()));
} // GetAsString()


/**
 * If a block is fully contained and has been pushed on queue,
 * update mask, region, number of voxels, etc.
 */
void vxBlockVolumeRegionGrowIteratorHist::UpdateForFullyContainedBlock()
//*******************************************************************
{
  m_iterMask.ClearCurrentBlock();
  m_iterRegion.SetPos(m_iterMask);
  m_iterRegion.ClearCurrentBlock();

  // account for border blocks that might not contain 4096 (16x16x16) voxels
  const Triple<int4> volDim(m_iterMask.GetVolume().GetHeader().GetVolDim());
  const Triple<int4> iterPos(m_iterMask.GetPos().X(), m_iterMask.GetPos().Y(), m_iterMask.GetPos().Z());
  m_uNumOfVoxels += min(16, volDim.X()-iterPos.X()) * min(16, volDim.Y()-iterPos.Y()) * min(16, volDim.Z()-iterPos.Z()); // just in case it is less than 16 in size in any dimension
}


/**
 * Based on a given position that 
 * Does the book keeping to find the next new Block16 that could be included in the region.
 */
void vxBlockVolumeRegionGrowIteratorHist::NextNewBlock16OfRegion() 
//*******************************************************************
{
  // set m_bGrowEnded if there are no more voxels to process
  if (m_queueOfBlock16.empty())
  {
    // check next queue
    m_NextVoxelOrQueuePtr = &vxBlockVolumeRegionGrowIteratorHist::NextNewVoxelOfRegion;
    (this->*m_NextVoxelOrQueuePtr)();
    return;
  }

  // get next block of region from queue
  const uint8 nextPos(m_queueOfBlock16.front());
  m_iterHist.SetPosFromPackedBlockOffsets(nextPos);
  m_queueOfBlock16.pop();
  m_iterMask.SetPos(m_iterHist.GetPos());

  int4 iHistVoxel(0);

  const Vector3D<int4> position(m_iterMask.GetPos());
  const int4 iMaxX(m_rDataVolume.GetHeader().GetVolDim().m_x-1);
  const int4 iMaxY(m_rDataVolume.GetHeader().GetVolDim().m_y-1);
  const int4 iMaxZ(m_rDataVolume.GetHeader().GetVolDim().m_z-1);

  // test block to the left, if still inside volume
  ///////////////////////////////////////////////////
  if (position.m_x > 15)
  {
    // X: lower neighbour
    m_iterMask.DecXbyDelta(16);
    m_iterHist.DecXbyDelta(16);

    if (IsEntireBlockInRegion(m_iterHist.GetCurrentBlock().GetBinaryHistogram(), m_iterMask))
    {
      m_queueOfBlock16.push(m_iterHist.GetPosAsPackedBlockOffsets());
      UpdateForFullyContainedBlock();
    }
    else
    {
      // check face of voxels
      m_iterMask.IncXbyDelta(15);
      NextVoxelsByYZ();
      m_iterMask.DecXbyDelta(15);
    }
    
    m_iterMask.IncXbyDelta(16);
    m_iterHist.IncXbyDelta(16);
  }

  // test block to the right, if still inside volume
  ////////////////////////////////////////////////////
  if (position.m_x + 16 <= iMaxX)
  {
    // X: upper neighbour
    m_iterMask.IncXbyDelta(16);
    m_iterHist.IncXbyDelta(16);

    if (IsEntireBlockInRegion(m_iterHist.GetCurrentBlock().GetBinaryHistogram(), m_iterMask))
    {
      m_queueOfBlock16.push(m_iterHist.GetPosAsPackedBlockOffsets());
      UpdateForFullyContainedBlock();
    }
    else
    {
      // check face of voxels
      NextVoxelsByYZ();
    }
    m_iterMask.DecXbyDelta(16);
    m_iterHist.DecXbyDelta(16);
  }

  
  // test block below, if still inside volume
  /////////////////////////////////////////////
  if (position.m_y > 15)
  {
    // Y: lower neighbour
    m_iterMask.DecYbyDelta(16);
    m_iterHist.DecYbyDelta(16);

    if (IsEntireBlockInRegion(m_iterHist.GetCurrentBlock().GetBinaryHistogram(), m_iterMask))
    {
      m_queueOfBlock16.push(m_iterHist.GetPosAsPackedBlockOffsets());
      UpdateForFullyContainedBlock();
    }
    else
    {
      // check face of voxels
      m_iterMask.IncYbyDelta(15);
      NextVoxelsByXZ();
      m_iterMask.DecYbyDelta(15);
    }
    m_iterMask.IncYbyDelta(16);
    m_iterHist.IncYbyDelta(16);
  }

  // test block below, if still inside volume
  /////////////////////////////////////////////
  if (position.m_y + 16 <= iMaxY)
  {
    // Y: upper neighbour
    m_iterMask.IncYbyDelta(16);
    m_iterHist.IncYbyDelta(16);

    if (IsEntireBlockInRegion(m_iterHist.GetCurrentBlock().GetBinaryHistogram(), m_iterMask))
    {
      m_queueOfBlock16.push(m_iterHist.GetPosAsPackedBlockOffsets());
      UpdateForFullyContainedBlock();
    }
    else
    {
      // check face of voxels
      NextVoxelsByXZ();
    }  
    m_iterMask.DecYbyDelta(16);
    m_iterHist.DecYbyDelta(16);
  }

  // test block in front, if still inside volume
  ////////////////////////////////////////////////
  if (position.m_z > 15)
  {
    // Z: lower neighbour
    m_iterMask.DecZbyDelta(16);
    m_iterHist.DecZbyDelta(16);

    if (IsEntireBlockInRegion(m_iterHist.GetCurrentBlock().GetBinaryHistogram(), m_iterMask))
    {
      m_queueOfBlock16.push(m_iterHist.GetPosAsPackedBlockOffsets());
      UpdateForFullyContainedBlock();
    }
    else
    {
      // check face of voxels
      m_iterMask.IncZbyDelta(15);
      NextVoxelsByXY();
      m_iterMask.DecZbyDelta(15);
    }
    m_iterMask.IncZbyDelta(16);
    m_iterHist.IncZbyDelta(16);
  }

  // test block in behind, if still inside volume
  /////////////////////////////////////////////////
  if (position.m_y + 16 <= iMaxY)
  {
    // Z: upper neighbour
    m_iterMask.IncZbyDelta(16);
    m_iterHist.IncZbyDelta(16);

    if (IsEntireBlockInRegion(m_iterHist.GetCurrentBlock().GetBinaryHistogram(), m_iterMask))
    {
      m_queueOfBlock16.push(m_iterHist.GetPosAsPackedBlockOffsets());
      UpdateForFullyContainedBlock();
    }
    else
    {
      // check face of voxels
      NextVoxelsByXY();
    }
    m_iterMask.DecZbyDelta(16);
    m_iterHist.DecZbyDelta(16);
  }
} // NextNewBlock16OfRegion()


/**
 * Traverses the voxels along a 16x16 cube face in the y & z plane.
 * After the traversal, the mask iterator needs to be where it was
 * when entering this function. No need to test for volume boundaries
 * since only blocks that are fully inside the volume are added.
 */
void vxBlockVolumeRegionGrowIteratorHist::NextVoxelsByYZ()
//*******************************************************************
{
  m_iterData.SetPos(m_iterMask);

  for (int4 z(0); z<16; ++z, m_iterData.IncZ())
  {
    for (int4 y(0); y<16; ++y, m_iterData.IncY())
    {
      m_iterMask.SetPos(m_iterData);
      if (!m_iterMask.GetVoxel() && m_decider.IsPartOfRegion(m_iterData.GetVoxel()))
      {
        m_queueOfVoxels.push(m_iterData.GetPosAsPackedBlockOffsets());
        m_iterMask.SetVoxel(true);
        m_iterRegion.SetVoxel(true, m_iterData);
      }
    }
    m_iterData.DecYbyDelta(16);
  }
  m_iterData.DecZbyDelta(16);
  m_iterMask.SetPos(m_iterData);

  return;
} // NextVoxelsByYZ()


/**
 * Traverses the voxels along a 16x16 cube face in the x & z plane.
 * After the traversal, the mask iterator needs to be where it was
 * when entering this function.No need to test for volume boundaries
 * since only blocks that are fully inside the volume are added.
 */
void vxBlockVolumeRegionGrowIteratorHist::NextVoxelsByXZ()
//*******************************************************************
{
  m_iterData.SetPos(m_iterMask);

  for (int4 z(0); z<16; ++z, m_iterData.IncZ())
  {
    for (int4 x(0); x<16; ++x, m_iterData.IncX())
    {
      m_iterMask.SetPos(m_iterData);
      if (!m_iterMask.GetVoxel() && m_decider.IsPartOfRegion(m_iterData.GetVoxel()))
      {
        m_queueOfVoxels.push( m_iterData.GetPosAsPackedBlockOffsets() );
        m_iterMask.SetVoxel(true);
        m_iterRegion.SetVoxel(true, m_iterData);
      }
    }
    m_iterData.DecXbyDelta(16);
  }
  m_iterData.DecZbyDelta(16);
  m_iterMask.SetPos(m_iterData);

  return;
} // NextVoxelsByXZ()


/**
 * Traverses the voxels along a 16x16 cube face in the x & y plane.
 * After the traversal, the mask iterator needs to be where it was
 * when entering this function.No need to test for volume boundaries
 * since only blocks that are fully inside the volume are added.
 */
void vxBlockVolumeRegionGrowIteratorHist::NextVoxelsByXY()
//*******************************************************************
{
  m_iterData.SetPos(m_iterMask);

  for (int4 y(0); y<16; ++y, m_iterData.IncY())
  {
    for (int4 x(0); x<16; ++x, m_iterData.IncX())
    {
      m_iterMask.SetPos(m_iterData);
      if (!m_iterMask.GetVoxel() && m_decider.IsPartOfRegion(m_iterData.GetVoxel()))
      {
        m_queueOfVoxels.push( m_iterData.GetPosAsPackedBlockOffsets() );
        m_iterMask.SetVoxel(true);
        m_iterRegion.SetVoxel(true, m_iterData);
      }
    }
    m_iterData.DecXbyDelta(16);
  }
  m_iterData.DecYbyDelta(16);
  m_iterMask.SetPos(m_iterData);

  return;
} // NextVoxelsByXY()


/**
 * If there still is some seedpoint to consider, take it from
 * the queue and search its neighbours for potential candidates
 * to add to the queue.
 */
void vxBlockVolumeRegionGrowIteratorHist::NextNewVoxelOfRegion() 
//*******************************************************************
{
  // set m_bGrowEnded if there are no more voxels to process
  if (m_queueOfVoxels.empty())
  {
    m_bGrowEnded = true;
    return;
  }

  // get next voxel of region from queue
  const uint8 nextPos(m_queueOfVoxels.front());
  m_iterData.SetPosFromPackedBlockOffsets(nextPos);
  m_queueOfVoxels.pop();
  ++m_uNumOfVoxels;

  NextVoxelsInNeighborhood();

} // NextNewVoxelOfRegion()


/**
 * Traverses the 6connected neighborhood of a current voxel
 */
void vxBlockVolumeRegionGrowIteratorHist::NextVoxelsInNeighborhood() 
//*******************************************************************
{
  m_iterMask.SetPos(m_iterData);
  m_iterRegion.SetPos(m_iterData);

  const bool bNewBlock(m_pLastBlock != &m_iterMask.GetCurrentBlock());
  m_pLastBlock = &m_iterMask.GetCurrentBlock();

  if (bNewBlock == true) // new block so check for Block16 within region...
  {
    m_iterHist.SetPos((m_iterMask.GetPos() / 16.0F) * 16);  
    m_iterMaskOrg.SetPos(m_iterMask);

    if (IsEntireBlockInRegion(m_iterHist.GetCurrentBlock().GetBinaryHistogram(), m_iterMaskOrg))
    {
      m_queueOfBlock16.push(m_iterHist.GetPosAsPackedBlockOffsets());
      UpdateForFullyContainedBlock();
      m_NextVoxelOrQueuePtr = &vxBlockVolumeRegionGrowIteratorHist::NextNewBlock16OfRegion;

      return;
    }
  } 

  const Vector3D<int4> position(m_iterMask.GetPos());
  const int4 iMaxX(m_rDataVolume.GetHeader().GetVolDim().m_x-1);
  const int4 iMaxY(m_rDataVolume.GetHeader().GetVolDim().m_y-1);
  const int4 iMaxZ(m_rDataVolume.GetHeader().GetVolDim().m_z-1);

  // peek towards next 6 neighboring voxels that are potentially part of the region and
  // test if these voxels are not yet processed and belong to the region
  if (!m_iterMask.Peek1nx0py0pz() && (position.m_x > 0) && m_decider.IsPartOfRegion(m_iterData.Peek1nx0py0pz()))
  {
    m_queueOfVoxels.push(m_iterData.GetPosAsPackedBlockOffsetsPrevX());
    m_iterMask.SetVoxelPrevX(true);
    m_iterRegion.SetVoxelPrevX(true);
  }

  if (!m_iterMask.Peek1px0py0pz() && (position.m_x < iMaxX) && m_decider.IsPartOfRegion(m_iterData.Peek1px0py0pz()))
  {
    m_queueOfVoxels.push(m_iterData.GetPosAsPackedBlockOffsetsNextX());
    m_iterMask.SetVoxelNextX(true);
    m_iterRegion.SetVoxelNextX(true);
  }

  if (!m_iterMask.Peek0px1ny0pz() && (position.m_y > 0) && m_decider.IsPartOfRegion(m_iterData.Peek0px1ny0pz()))
  {
    m_queueOfVoxels.push(m_iterData.GetPosAsPackedBlockOffsetsPrevY());
    m_iterMask.SetVoxelPrevY(true);
    m_iterRegion.SetVoxelPrevY(true);
  }

  if (!m_iterMask.Peek0px1py0pz() && (position.m_y < iMaxY) && m_decider.IsPartOfRegion(m_iterData.Peek0px1py0pz()))
  {
    m_queueOfVoxels.push(m_iterData.GetPosAsPackedBlockOffsetsNextY());
    m_iterMask.SetVoxelNextY(true);
    m_iterRegion.SetVoxelNextY(true);
  }

  if (!m_iterMask.Peek0px0py1nz() && (position.m_z > 0) && m_decider.IsPartOfRegion(m_iterData.Peek0px0py1nz()))
  {
    m_queueOfVoxels.push(m_iterData.GetPosAsPackedBlockOffsetsPrevZ());
    m_iterMask.SetVoxelPrevZ(true);
    m_iterRegion.SetVoxelPrevZ(true);
  }

  if (!m_iterMask.Peek0px0py1pz() && (position.m_z < iMaxZ) && m_decider.IsPartOfRegion(m_iterData.Peek0px0py1pz()))
  {
    m_queueOfVoxels.push(m_iterData.GetPosAsPackedBlockOffsetsNextZ());
    m_iterMask.SetVoxelNextZ(true);
    m_iterRegion.SetVoxelNextZ(true);
  }

} // NextVoxelsInNeighborhood()


/**
 * Test to determine if a block16 is completely within region
 * @param iHistVoxel Voxel value in histogram
 * @param iterMask mask volume iterator
 * @return Flag indicating if block is in region
 */
bool vxBlockVolumeRegionGrowIteratorHist::IsEntireBlockInRegion(const int4 iHistVoxel, 
                                                          const vxBlockVolumeIterator<bool> & iterMask)
//*******************************************************************
{
  /*
   if( (iHistVoxel == 0) 
      //&& ((!iterMask.GetVoxel() && iterMask.GetCurrentBlock().IsHomogenous() && !(iHistVoxel & m_uMaskBlockHasSomeOutsideRegionVoxels)) == true) &&
      //(iterMask.GetCurrentBlock().IsHomogenous() == true) 
      )
  {
    LogFyi( " ==== It is impossible + iterMask.GetPos().ToString()", "histogram", "histogram.C");
  }
  */
 
  // histogram should always have at least 1 bit set (should not happen but does on occasion)
  if( iHistVoxel == 0 )
    return false;
  
  const bool bIsBlockLocked( iterMask.GetVoxel() && iterMask.GetCurrentBlock().IsHomogenous() );
  const bool bIsEntirelyInside( !( iHistVoxel & m_uMaskBlockHasSomeOutsideRegionVoxels ) );
  return !bIsBlockLocked && bIsEntirelyInside;
} // IsEntireBlockInRegion()


#undef FILE_REVISION


// Revision History:
// $Log: vxBlockVolumeRegionGrowIteratorHist.C,v $
// Revision 1.3.2.1  2010/07/16 20:31:42  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.3  2007/06/28 19:41:47  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.2.4.1  2007/04/25 19:47:15  romy
// converted decider type to int4. Issue#5611
//
// Revision 1.2  2006/03/29 20:05:03  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.25  2003/07/14 16:11:41  dongqing
// add iHistVoxel check back and leaving it for future debug
//
// Revision 1.24  2003/07/02 22:00:18  michael
// found bug, which was self-introduced after fixing it all. :-( No need to test the
// faces of blocks that are in the block queue because only blocks that are fully
// in the volume will be added to the queue in the first place.
//
// Revision 1.23  2003/07/02 15:58:53  michael
// fixed block based region grow iterator
//
// Revision 1.22  2003/06/27 12:52:37  dongqing
// debug purpose
//
// Revision 1.21  2003/06/23 19:30:33  dongqing
// change from int2 to uint2
//
// Revision 1.20  2003/06/19 21:36:58  michael
// made size of 32 bins relative to volume header (bits stored)
//
// Revision 1.19  2003/06/19 20:34:55  michael
// accounting for uint2 instead of int2 as well as 32 bins selecting the five highest
// used bits through the intenisty converter
//
// Revision 1.18  2003/06/18 20:01:26  dongqing
// add voxel count for NextNewVoxelOfRegionAndInsideVolume()
//
// Revision 1.17  2003/06/18 15:19:48  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.16  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.15  2003/06/07 15:37:26  michael
// formatting
//
// Revision 1.14  2003/05/03 16:18:20  michael
// completed code review
//
// Revision 1.13  2003/05/02 14:36:57  dongqing
// code walk through
//
// Revision 1.12  2003/01/22 21:31:59  ingmar
// creating a new volume needs a vxVolumeHeader, not just a dimension
//
// Revision 1.11  2002/12/12 17:07:39  ingmar
// removed binary histogram 04 (3x better histogram creation time, only slightly slower raw region growing, faster overall region grow time when also counting volume updates)
//
// Revision 1.10  2002/10/07 22:22:07  jaddonisio
// Count the number of voxels correctly.
//
// Revision 1.9  2002/08/20 15:46:49  jaddonisio
// Clean up.
//
// Revision 1.8  2002/08/20 15:11:34  jaddonisio
// Use new mask based on block04.
//
// Revision 1.7  2002/08/16 21:51:46  jaddonisio
// In blk16, after checking blk04 needed to go onto voxels.
//
// Revision 1.6  2002/08/16 18:19:03  jaddonisio
// Moved some code into functions for clarity.
//
// Revision 1.5  2002/08/15 14:29:03  jaddonisio
// Fixed logic to account for locking but now a little bit slower.
//
// Revision 1.4  2002/08/13 19:19:35  jaddonisio
// Bidirectional traversal between 16, 04, & 01 regions.
//
// Revision 1.3  2002/08/12 19:25:45  jaddonisio
// New 16x16x16 -> 4x4x4 -> voxel iteration works. Faster by a factor of 3.7, approximately.
//
// Revision 1.2  2002/08/06 22:31:55  jaddonisio
// Finished finding consecutive blocks of 16x16x16 that are in the region. More to come...
//
// Revision 1.1  2002/08/05 22:09:31  jaddonisio
// Initial version.
//
// Revision 1.6  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.5  2002/06/04 22:47:17  soeren
// new VolIterator Peek functions that allow peeking into a mix of positive and negative offsets
//
// Revision 1.4  2002/05/03 15:35:09  soeren
// fixed regiongrow for range which contains background
//
// Revision 1.3  2002/05/03 13:08:33  ingmar
// in constructor: make sure the seedpoint is not locked
//
// Revision 1.2  2002/04/30 13:47:58  ingmar
// completed renaming Intensity to VoxelValue
//
// Revision 1.1  2002/04/30 13:39:05  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeRegionGrowIteratorHist.C,v 1.3.2.1 2010/07/16 20:31:42 dongqing Exp $
// $Id: vxBlockVolumeRegionGrowIteratorHist.C,v 1.3.2.1 2010/07/16 20:31:42 dongqing Exp $
