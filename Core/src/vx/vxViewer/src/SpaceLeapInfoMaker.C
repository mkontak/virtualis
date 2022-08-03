// $Id: SpaceLeapInfoMaker.C,v 1.2 2006/03/29 20:03:08 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// includes
#include "stdafx.h"
#include "SpaceLeapInfoMaker.h"
#include "vxBlockVolumeIterator.h"
#include "Timer.h"


// forward declarations
template class SpaceLeapInfoMaker<int2>;


// defines
#define FILE_REVISION "$Revision: 1.2 $"
#define Global_bShowTimingsInLog false




/* 
 * Add the space leap info to the upper 4 bits in the CT volume
 *
 * @param volume
 * @param minimum range
 * @param maximum range
 * @param bounding box lower left
 * @param bounding box upper right
 * @param radius
 * @param mask
 * /
template <class VoxelType> 
void SpaceLeapInfoMaker<VoxelType>::ComputeSingleBitForWithinRadius(vxBlockVolume<VoxelType> & ctvVol, 
                                                                    const VoxelType rangeMin, 
                                                                    const VoxelType rangeMax,
                                                                    const Vector3D<float4> & fvBBoxMin,  
                                                                    const Vector3D<float4> & fvBBoxMax,
                                                                    const int4 iRadius, 
                                                                    const int4 iMask)
{
  // compute summed count volume 
  ComputeSummedCountVolume(ctvVol, rangeMin, rangeMax, fvBBoxMin, fvBBoxMax, iRadius);

  // get volume iterators
  vxBlockVolumeIterator<int4> scvIter(m_summedCountBlkVol);
  vxBlockVolumeIterator<VoxelType> ctvIter(ctvVol);

  for (ctvIter.SetPos(0,0,0); !ctvIter.IsAtEnd(); ctvIter.NextZYX())
  {
    scvIter.SetPos(ctvIter.GetPos()); 
    const int4 iClearMask(0xFFFF ^ iMask);
    int4 ctvVoxel = 0x0FFF & max(0, ctvIter.GetVoxel());
    int4 iCount(0);
    const int4 r(iRadius+1);
    const int4 f(-r-1);
    iCount += scvIter.PeekRel(r, r, r);
    iCount -= scvIter.PeekRel(f, r, r);
    iCount -= scvIter.PeekRel(r, f, r);
    iCount += scvIter.PeekRel(f, f, r);

    iCount -= scvIter.PeekRel(r, r, f);
    iCount += scvIter.PeekRel(f, r, f);
    iCount += scvIter.PeekRel(r, f, f);
    iCount -= scvIter.PeekRel(f, f, f);

    if (iCount > 0)
    {
      ctvVoxel |= iMask;
    }
    ctvIter.SetVoxel(ctvVoxel);
  } // end for iterator over volume

} // ComputeSingleBitForWithinRadius()


/*
 * Add the space leap info to the upper 4 bits in the CT volume
 *
 * @param input volume
 * @param output volume
 * @param minimum range
 * @param maximum range
 * @param bounding box lower left
 * @param bounding box upper right
 * @param radius
 * @param mask
 * /
template <class VoxelType> 
void SpaceLeapInfoMaker<VoxelType>::ComputeSingleBitForWithinRadius(vxBlockVolume<VoxelType> & ctvInputVol, 
                                                                    vxBlockVolume<VoxelType> & ctvOutPutVol, 
                                                                    const VoxelType rangeMin, 
                                                                    const VoxelType rangeMax, 
                                                                    const Vector3D<float4> & fvBBoxMin, 
                                                                    const Vector3D<float4> & fvBBoxMax,
                                                                    const int4 iRadius, 
                                                                    const int4 iMask)
{
  // compute summed count volume 
  ComputeSummedCountVolume(ctvInputVol, rangeMin, rangeMax, fvBBoxMin, fvBBoxMax, iRadius);

  // get volume iterators
  vxBlockVolumeIterator<int4>      scvIter(m_summedCountBlkVol);
  vxBlockVolumeIterator<VoxelType> ctvInputIter(ctvInputVol);
  vxBlockVolumeIterator<VoxelType> ctvOutPutIter(ctvOutPutVol);

  Timer timer;
  for (ctvInputIter.SetPos(0,0,0); !ctvInputIter.IsAtEnd(); ctvInputIter.NextZYX(), ctvOutPutIter.NextZYX())
  {
    scvIter.SetPos(ctvInputIter.GetPos()); 
    const int4 iClearMask(0xFFFF ^ iMask);
    int4 ctvVoxel = iClearMask & max(0, ctvInputIter.GetVoxel());
    int4 iCount(0);
    const int4 r(iRadius+1);
    const int4 f(-r);
    iCount += scvIter.PeekRel(r, r, r);
    iCount -= scvIter.PeekRel(f, r, r);
    iCount -= scvIter.PeekRel(r, f, r);
    iCount += scvIter.PeekRel(f, f, r);

    iCount -= scvIter.PeekRel(r, r, f);
    iCount += scvIter.PeekRel(f, r, f);
    iCount += scvIter.PeekRel(r, f, f);
    iCount -= scvIter.PeekRel(f, f, f);
    if (iCount > 0)
    {
      ctvVoxel |= iMask;
      ctvOutPutIter.SetVoxel(ctvVoxel);
    }
  } // end for iter over volume
  if (Global_bShowTimingsInLog == true)
  {
    LogDbg(std::string("ComputeSingleBitForWithinRadius I took ")+ToAscii(timer.ElapsedTime()), "SpaceLeapInfoMaker", "ComputeSingleBitForWithinRadius");
  }

} // ComputeSingleBitForWithinRadius()


/* 
 * Add the space leap info to the upper 4 bits in the CT volume
 *
 * @param input volume
 * @param output volume
 * @param minimum range
 * @param maximum range
 * @param bounding box lower left
 * @param bounding box upper right
 * @param mask
 * /
template <class VoxelType> 
void SpaceLeapInfoMaker<VoxelType>::ComputeFourBitsSpaceLeapRadius(vxBlockVolume<VoxelType> & ctvInputVol, 
                                                                   vxBlockVolume<VoxelType> & ctvOutPutVol, 
                                                                   const VoxelType rangeMin, 
                                                                   const VoxelType rangeMax, 
                                                                   const Vector3D<float4> & fvBBoxMin, 
                                                                   const Vector3D<float4> & fvBBoxMax, 
                                                                   const int4 iRadiusMax)
{
  // compute summed count volume 
  ComputeSummedCountVolume(ctvInputVol, rangeMin, rangeMax, fvBBoxMin, fvBBoxMax, iRadiusMax);

  // get volume iterators
  vxBlockVolumeIterator<int4>      scvIter(m_summedCountBlkVol);
  vxBlockVolumeIterator<VoxelType> ctvInputIter(ctvInputVol);
  vxBlockVolumeIterator<VoxelType> ctvOutPutIter(ctvOutPutVol);

  Timer timer;
  int4 iLastClearBoxRadius(iRadiusMax);
  for (ctvInputIter.SetPos(0,0,0); !ctvInputIter.IsAtEnd(); ctvInputIter.NextZYX(), ctvOutPutIter.NextZYX())
  {
    scvIter.SetPos(ctvInputIter.GetPos()); 
    int4 iRadius(iLastClearBoxRadius);
    int4 iRadiusDelta(-1);
    bool bIsBoxClear(false);
    bool bIsBoxClearFirst(false);
    bool bIsFirstLoopRun(true);

    for (bool bContinueLoop(true); bContinueLoop == true; iRadius += iRadiusDelta)
    {
      int4 iCount(0);
      const int4 iRadiusPos(iRadius+3);
      const int4 iRadiusNeg(-iRadiusPos);
      iCount += scvIter.PeekRel(iRadiusPos, iRadiusPos, iRadiusPos);
      iCount -= scvIter.PeekRel(iRadiusNeg, iRadiusPos, iRadiusPos);
      iCount -= scvIter.PeekRel(iRadiusPos, iRadiusNeg, iRadiusPos);
      iCount += scvIter.PeekRel(iRadiusNeg, iRadiusNeg, iRadiusPos);

      iCount -= scvIter.PeekRel(iRadiusPos, iRadiusPos, iRadiusNeg);
      iCount += scvIter.PeekRel(iRadiusNeg, iRadiusPos, iRadiusNeg);
      iCount += scvIter.PeekRel(iRadiusPos, iRadiusNeg, iRadiusNeg);
      iCount -= scvIter.PeekRel(iRadiusNeg, iRadiusNeg, iRadiusNeg);

      bIsBoxClear = (iCount == 0);

      if (bIsFirstLoopRun == true)
      {
        bIsFirstLoopRun = false;
        bIsBoxClearFirst = bIsBoxClear;
        if (bIsBoxClearFirst == true) // this radius is fine, lets try for even larger ones
        {
          iRadiusDelta = +1;
        }
        else // this radius is already too large, lets try smaller ones
        { 
          iRadiusDelta = -1;
        }
      }

      // in the common case continue until we find a difference between first and current
      if (bIsBoxClearFirst == bIsBoxClear)
      {
        bContinueLoop = true;
      }
      else
      {
        bContinueLoop = false;
      }

      // test for special case of min radius
      if ((iRadius == 0) && (!bIsBoxClear))
      {
        bContinueLoop = false;
      }

      // test for special case of max radius
      if ((iRadius == iRadiusMax) && (bIsBoxClear))
      {
        bContinueLoop = false;
      }

      // normally iRadius has the correct value now
      iLastClearBoxRadius = iRadius;

      // if we just discovered a radius that does no longer work, go back to the previous one
      if ((bIsBoxClearFirst) && (!bIsBoxClear))
      {
        iLastClearBoxRadius -= iRadiusDelta;
      }
    } // end for any possible radius

    ctvOutPutIter.SetVoxel(ctvInputIter.GetVoxel() | (iLastClearBoxRadius << 12));
  } // end for iter over all voxels

  if (Global_bShowTimingsInLog == true)
  {
    LogDbg(std::string("ComputeFourBitsSpaceLeapRadius took ")+ToAscii(timer.ElapsedTime()), "SpaceLeapInfoMaker", "ComputeSingleBitForWithinRadius");
  }

} // ComputeSingleBitForWithinRadius()


/**
 * Compute a summed count volume
 *
 * @param input volume
 * @param minimum range
 * @param maximum range
 * @param bounding box lower left
 * @param bounding box upper right
 * @param radius
 */
template <class VoxelType> 
void SpaceLeapInfoMaker<VoxelType>::ComputeSummedCountVolume(vxBlockVolume<VoxelType> & ctvInputVol, 
                                                             const VoxelType rangeMin, 
                                                             const VoxelType rangeMax, 
                                                             const Vector3D<float4> & BBoxMin, 
                                                             const Vector3D<float4> & BBoxMax,
                                                             int4 iRadiusMax)
{
  Timer timer;
  m_summedCountBlkVol.GetHeader().SetVolDim(Triple<int4>(Vector3D<int4>(ctvInputVol.GetHeader().GetVolDim())
                                                         +Vector3D<int4>(iRadiusMax+1, iRadiusMax+1, iRadiusMax+1)));
  // get volume iterators
  vxBlockVolumeIterator<int4>      scvIter(m_summedCountBlkVol);
  vxBlockVolumeIterator<VoxelType> ctvInputIter(ctvInputVol);

  // build scv volume table
  for (scvIter.SetPos(0,0,0); !scvIter.IsAtEnd(); scvIter.NextZYX())
  {
    int4 iSummedCount(0);

    ctvInputIter.SetPos(scvIter.GetPos()); // may be outside the ctv volume
    const VoxelType iVoxel(ctvInputIter.GetVoxel()); // may be background value at outside position
    if (rangeMin <= iVoxel && iVoxel <= rangeMax)
    {
      iSummedCount = 1;
    }
    iSummedCount += scvIter.Peek1nx0py0pz(); 
    iSummedCount += scvIter.Peek0px1ny0pz(); 
    iSummedCount -= scvIter.Peek1nx1ny0pz(); 

    iSummedCount += scvIter.Peek0px0py1nz();
    iSummedCount -= scvIter.Peek1nx0py1nz();
    iSummedCount -= scvIter.Peek0px1ny1nz();
    iSummedCount += scvIter.Peek1nx1ny1nz();

    scvIter.SetVoxel(iSummedCount);
  } // end for iter over all voxels

  if (Global_bShowTimingsInLog == true)
  {
    LogDbg(std::string("ComputeSummedCountVolume took ")+ToAscii(timer.ElapsedTime()), "SpaceLeapInfoMaker", "ComputeSummedCountVolume");
  }

} // ComputeSummedCountVolume()


/* 
 * Add the space leap info to the upper 4 bits in the CT volume
 *
 * @param histogram intensity volume
 * @param histogram label volume
 * @param volume
 * @param intensity mask
 * @param label mask
 * @param maximum radius
 */
template <class VoxelType> 
void SpaceLeapInfoMaker<VoxelType>::ComputeInt1SpaceLeapRadius(vxBlockVolume<uint2> & volIntensity, 
                                                               vxBlockVolume<uint2> & volLabel, 
                                                               vxBlockVolume<int1> & sldVol, 
                                                               const int4 iMaskIntensity, 
                                                               const int4 iMaskLabel, 
                                                               const int4 iRadiusMax)
{
  // compute summed count volume 
  ComputeSmallSummedCountVolume(volIntensity, volLabel, iMaskIntensity, iMaskLabel, iRadiusMax);

  // get volume iterators
  vxBlockVolumeIterator<int4> scvIter(m_summedCountBlkVol);
  vxBlockVolumeIterator<int1> sldIter(sldVol);

  Timer timer;
  int4 iLastClearBoxRadius(iRadiusMax);

  for (sldIter.SetPos(0,0,0); !sldIter.IsAtEnd(); sldIter.NextZYX())
  {
    scvIter.SetPos(sldIter.GetPos()); 
    int4 iRadius(iLastClearBoxRadius);
    int4 iRadiusDelta(-1);
    bool bIsBoxClear(false);
    bool bIsBoxClearFirst(false);
    bool bIsFirstLoopRun(true);
    for (bool bContinueLoop(true); bContinueLoop; iRadius += iRadiusDelta)
    {
      int4 iCount(0);
      const int4 iRadiusPos(iRadius );
      const int4 iRadiusNeg(-iRadius-1);
      iCount += scvIter.PeekRel(iRadiusPos, iRadiusPos, iRadiusPos);
      iCount -= scvIter.PeekRel(iRadiusNeg, iRadiusPos, iRadiusPos);
      iCount -= scvIter.PeekRel(iRadiusPos, iRadiusNeg, iRadiusPos);
      iCount += scvIter.PeekRel(iRadiusNeg, iRadiusNeg, iRadiusPos);

      iCount -= scvIter.PeekRel(iRadiusPos, iRadiusPos, iRadiusNeg);
      iCount += scvIter.PeekRel(iRadiusNeg, iRadiusPos, iRadiusNeg);
      iCount += scvIter.PeekRel(iRadiusPos, iRadiusNeg, iRadiusNeg);
      iCount -= scvIter.PeekRel(iRadiusNeg, iRadiusNeg, iRadiusNeg);

      bIsBoxClear = (iCount == 0);

      if (bIsFirstLoopRun == true)
      {
        bIsFirstLoopRun = false;
        bIsBoxClearFirst = bIsBoxClear;
        if (bIsBoxClearFirst == true) // this radius is fine, lets try for even larger ones
        { 
          iRadiusDelta = +1;
        }
        else // this radius is already too large, lets try smaller ones
        {
          iRadiusDelta = -1;
        }
      }

      // in the common case continue until we find a difference between first and current
      if (bIsBoxClearFirst == bIsBoxClear)
      {
        bContinueLoop = true;
      }
      else
      {
        bContinueLoop = false;
      }

      // test for special case of min radius
      if ((iRadius < 0) && (!bIsBoxClear))
      {
        bContinueLoop = false;
      }

      // test for special case of max radius
      if ((iRadius == iRadiusMax) && (bIsBoxClear))
      {
        bContinueLoop = false;
      }

      // normally iRadius has the correct value now
      iLastClearBoxRadius = iRadius;

      // if we just discovered a radius that does no longer work, go back to the previous one
      if ((bIsBoxClearFirst) && (!bIsBoxClear))
      {
        iLastClearBoxRadius -= iRadiusDelta;
      }
    } // end for all possible radii

    sldIter.SetVoxel(iLastClearBoxRadius+1);
  } // end for iter over all voxels

  if (Global_bShowTimingsInLog == true)
  {
    LogDbg(std::string("ComputeInt1SpaceLeapRadius took ")+ToAscii(timer.ElapsedTime()), "SpaceLeapInfoMaker", "ComputeInt1SpaceLeapRadius");
  }

} // ComputeInt1SpaceLeapRadius()



/**
 * Compute a small summed count volume
 *
 * @param histogram intensity volume
 * @param histogram label volume
 * @param intensity mask
 * @param label mask
 * @param maximum radius
 */
template <class VoxelType> 
void SpaceLeapInfoMaker<VoxelType>::ComputeSmallSummedCountVolume(vxBlockVolume<uint2> & volIntensity, 
                                                                  vxBlockVolume<uint2> & volLabel, 
                                                                  const int4 iMaskIntensity, 
                                                                  const int4 iMaskLabel, 
                                                                  const int4 iRadiusMax)
{
  Timer timer;
  m_summedCountBlkVol.GetHeader().SetVolDim(Triple<int4>(Vector3D<int4>(volIntensity.GetHeader().GetVolDim() / 16)
                                                         +Vector3D<int4>(iRadiusMax+1, iRadiusMax+1, iRadiusMax+1)));
  // get volume iterators
  vxBlockVolumeIterator<int4> iterSummedCountVol(m_summedCountBlkVol);
  vxBlockVolumeIterator<uint2> iterVolIntensity(volIntensity);
  vxBlockVolumeIterator<uint2> iterVolLabel(volLabel);

  // build scv volume table
  for (iterSummedCountVol.SetPos(0,0,0); !iterSummedCountVol.IsAtEnd(); iterSummedCountVol.NextZYX())
  {
    int4 iSummedCount(0);

    iterVolIntensity.SetPos(iterSummedCountVol.GetPos()*16); // may be outside the input volume
    iterVolLabel.SetPos(iterVolIntensity); // may be outside the input volume
    if ((iMaskIntensity & iterVolIntensity.GetCurrentBlock().GetBinaryHistogram()) || (iMaskLabel & iterVolLabel.GetCurrentBlock().GetBinaryHistogram()))
    {
      iSummedCount = 1;
    }
    iSummedCount += iterSummedCountVol.Peek1nx0py0pz(); 
    iSummedCount += iterSummedCountVol.Peek0px1ny0pz(); 
    iSummedCount -= iterSummedCountVol.Peek1nx1ny0pz(); 

    iSummedCount += iterSummedCountVol.Peek0px0py1nz();
    iSummedCount -= iterSummedCountVol.Peek1nx0py1nz();
    iSummedCount -= iterSummedCountVol.Peek0px1ny1nz();
    iSummedCount += iterSummedCountVol.Peek1nx1ny1nz();

    iterSummedCountVol.SetVoxel(iSummedCount);
  } // end for iter over all voxels

  if (Global_bShowTimingsInLog == true)
  {
    LogDbg(std::string("ComputeSmallSummedCountVolume took ")+ToAscii(timer.ElapsedTime()), "SpaceLeapInfoMaker", "ComputeSummedCountVolume");
  }

} // ComputeSmallSummedCountVolume()


/**
 * Compute a summed count volume.
 */
template <class VoxelType> 
void SpaceLeapInfoMaker<VoxelType>::ComputeSummedCountVolumeBlockWise(vxBlockVolume<uint2> & inputVol,
                                                                      vxBlockVolume<int4> & histogramIntensityVol,
                                                                      vxBlockVolume<int4> & histogramLabelVol,
                                                                      const Vector3D<float4> & fvBBoxMin, 
                                                                      const Vector3D<float4> & fvBBoxMax, 
                                                                      const int4 iMaskIntensity, 
                                                                      const int4 iMaskLabel)
{
  Timer timer;
  m_summedCountBlkVol.Clear();
  m_summedCountBlkVol.GetHeader().SetVolDim(inputVol.GetHeader().GetVolDim());

  // get volume iterators
  vxBlockVolumeIterator<int4> iterSummedCountVol(m_summedCountBlkVol);
  vxBlockVolumeIterator<uint2> iterInputVol(inputVol);
  vxBlockVolumeIterator<int4> iterHistogramIntensityVol(histogramIntensityVol);
  vxBlockVolumeIterator<int4> iterHistogramLabelVol(histogramLabelVol);

  // build scv volume table
  for (iterInputVol.SetPos(0,0,0); iterInputVol.IsNotAtEnd(); 
      iterInputVol.NextBlockZYX(), 
      iterSummedCountVol.NextBlockZYX(), 
      iterHistogramIntensityVol.NextZYX(),
      iterHistogramLabelVol.NextZYX()
     )
  {

    if (((iMaskIntensity & iterHistogramIntensityVol.GetVoxel()) != 0) ||
         ((iMaskLabel & iterHistogramLabelVol.GetVoxel()) != 0))
    {
      for (; iterInputVol.IsNotAtEndOfBlock();iterInputVol.NextZYXinsideBlock(), iterSummedCountVol.NextZYXinsideBlock())
      {
        int4 iSummedCount(1);

        iSummedCount += iterSummedCountVol.Peek1nx0py0pz(); 
        iSummedCount += iterSummedCountVol.Peek0px1ny0pz(); 
        iSummedCount -= iterSummedCountVol.Peek1nx1ny0pz(); 

        iSummedCount += iterSummedCountVol.Peek0px0py1nz();
        iSummedCount -= iterSummedCountVol.Peek1nx0py1nz();
        iSummedCount -= iterSummedCountVol.Peek0px1ny1nz();
        iSummedCount += iterSummedCountVol.Peek1nx1ny1nz();
        iterSummedCountVol.SetVoxel(iSummedCount);
      } // end for inside block
    } // end if mask fits
    else
    {
      int4 iSummedCount(0);

      iSummedCount += iterSummedCountVol.Peek1nx0py0pz(); 
      iSummedCount += iterSummedCountVol.Peek0px1ny0pz(); 
      iSummedCount -= iterSummedCountVol.Peek1nx1ny0pz(); 

      iSummedCount += iterSummedCountVol.Peek0px0py1nz();
      iSummedCount -= iterSummedCountVol.Peek1nx0py1nz();
      iSummedCount -= iterSummedCountVol.Peek0px1ny1nz();
      iSummedCount += iterSummedCountVol.Peek1nx1ny1nz();
      iterSummedCountVol.SetVoxel(iSummedCount);            
    }  // end else mask doesn't fit
  } // end for iter over all voxels

  if (Global_bShowTimingsInLog == true)
  {
    LogDbg(std::string("ComputeSummedCountVolumeBlockWise took ")+ToAscii(timer.ElapsedTime()), "SpaceLeapInfoMaker", "ComputeSummedCountVolumeBlockWise");
  }
} // ComputeSummedCountVolume()


#undef FILE_REVISION
#undef Global_bShowTimingsInLog


// $Log: SpaceLeapInfoMaker.C,v $
// Revision 1.2  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.41  2003/06/18 15:19:51  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.40  2003/06/09 14:58:30  michael
// commented out unused code
//
// Revision 1.39  2003/04/29 14:33:42  frank
// code review
//
// Revision 1.38  2003/04/16 12:47:44  michael
// performed initial code review
//
// Revision 1.37  2003/04/15 21:39:38  michael
// performed more code review
//
// Revision 1.36  2003/04/14 21:06:02  michael
// performed initial code review
//
// Revision 1.35  2003/02/25 19:03:16  frank
// Fixed "hiding none hides everything" bug
//
// Revision 1.34  2003/01/22 21:40:17  ingmar
// dimension and units are now Triples
//
// Revision 1.33  2002/12/11 15:53:12  ingmar
// removed unused box from function interface
//
// Revision 1.32  2002/10/23 21:36:23  ingmar
// no more timings in logfile (reenable by setting Global_bShowTimingsInLog true)
//
// Revision 1.31  2002/08/21 22:05:33  ingmar
// now differentiating between visibleVoxelCropBox and enclosing multipleOf16BlockCropBox
//
// Revision 1.30  2002/07/25 19:46:17  ingmar
// removed compiler complaint about 255+ character symbols
//
// Revision 1.29  2002/07/12 21:21:00  ingmar
// fixed space leaoing, current framerate 512^3 256^2 => 5.3
//
// Revision 1.28  2002/07/08 20:35:37  ingmar
// seperated the histogram information into two volumes, one for intensity and one for label, each now with 32 bits
//
// Revision 1.27  2002/07/01 23:23:55  soeren
// added more timings
//
// Revision 1.26  2002/06/28 22:58:56  soeren
// added new summed count computation .....
//
// Revision 1.25  2002/06/26 14:42:42  soeren
// just renamed some iters
//
// Revision 1.24  2002/06/04 22:48:23  soeren
// new VolIterator Peek functions that allow peeking into a mix of positive and negative offsets
//
// Revision 1.23  2002/05/30 17:02:05  soeren
// added space leaping though turned off components
//
// Revision 1.22  2002/05/29 22:37:33  ingmar
// fixed space leap info writing into the sld volume
//
// Revision 1.21  2002/05/29 19:49:58  ingmar
// made space leap buffr zone much tighter
//
// Revision 1.20  2002/05/29 19:49:13  soeren
// added more space leap debug information and enabled broken space leaping
//
// Revision 1.19  2002/05/28 21:53:58  soeren
// added new spaceleap method
//
// Revision 1.18  2002/05/08 15:04:26  soeren
// fixed spelling
//
// Revision 1.17  2002/05/08 15:03:51  soeren
// removed & 0x0FFF
//
// Revision 1.16  2002/05/07 22:16:58  soeren
// added timers
//
// Revision 1.15  2002/04/18 21:44:00  frank
// GetVoxelRaw is no longer.
//
// Revision 1.14  2002/04/16 21:41:02  geconomos
// Fixed soeren's and ingmar's compile bug.
//
// Revision 1.13  2002/04/14 00:38:39  michael
// adjusted member method call to existing function (did not compile)
//
// Revision 1.12  2002/04/10 18:17:03  ingmar
// worked on new header volume class
//
// Revision 1.11  2002/04/09 21:22:58  ingmar
// more conservative distace compute
//
// Revision 1.10  2002/04/08 23:55:19  ingmar
// incremental version of SpaceLeapDistCompute
//
// Revision 1.9  2002/04/08 22:50:53  ingmar
// moved more duplicated code
//
// Revision 1.8  2002/04/08 22:47:32  ingmar
// towards incremental version
//
// Revision 1.7  2002/04/08 22:34:54  ingmar
// moved duplicated code into seperate function ComputeSummedCountVolume()
//
// Revision 1.6  2002/04/08 21:48:06  ingmar
// new spaceleap computation
//
// Revision 1.5  2002/04/05 19:12:09  ingmar
// fixed space leap info computation
//
// Revision 1.4  2002/04/05 15:34:56  ingmar
// added new Peek function
//
// Revision 1.3  2002/04/05 15:27:06  ingmar
// added new Peek function
//
// Revision 1.2  2002/04/05 15:15:12  ingmar
// activated new space leap function (still broken)
//
// Revision 1.1  2002/04/04 19:09:45  ingmar
// inital version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/SpaceLeapInfoMaker.C,v 1.2 2006/03/29 20:03:08 geconomos Exp $
// $Id: SpaceLeapInfoMaker.C,v 1.2 2006/03/29 20:03:08 geconomos Exp $