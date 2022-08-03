// $Id: RegionThresholdSegment.C,v 1.3 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/**
 * Class implementation for RegionThresholdSegment. This class 
 * encapsulates region segmentation based on thresholding.
 */


// includes
#include "stdafx.h"
#include "RegionThresholdSegment.h"
#include "vxBlockVolumeIterator.h"
#include "vxUtils.h"
#include "Timer.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Default constructor
 */
RegionThresholdSegment::RegionThresholdSegment()
:m_uNumOfRegVoxels(0),
m_uThresholdLow(0),
m_uThresholdHigh(0)
//*******************************************************************
{
}


/**
 * Public method that runs the RegionThresholdSegment class
 * @param intensity volume
 * @param binary histogram volume
 * @param mask volume
 * @param region tag volume
 * @param validSubset the subset of the volume that is allowed to be grown
 */
void RegionThresholdSegment::Run(vxBlockVolume<uint2> * pVolume,
                                 vxBlockVolume<bool> * pMskTagVol, 
                                 vxBlockVolume<bool> * pRegTagVol,
                                 Box<int4> validSubset)
{
  Timer timer;

  // Region Threshold:
  vxBlockVolumeIterator<uint2> iterVol(*pVolume);
  vxBlockVolumeIterator<bool> iterRegTagVol(*pRegTagVol);
  vxBlockVolumeIterator<bool> iterMaskTagVol(*pMskTagVol);
  iterRegTagVol.SetClearBlock(true);
  iterMaskTagVol.SetClearBlock(true);

  m_uNumOfRegVoxels = 0;

  // THIS IS UGLY SHIT AND NEEDS TO BE REMOVED AFTER XP1.1 RELEASE AND DURING vxBase RESTRUCTURING
  // this note was placed/sponsored/brought to you by: Michael Meissner
  //////////////////////////////////////////////////////////////////////////////////////////////////
  uint4 iCode(vxUtils::GenerateBitCodeForInterval(m_uThresholdLow, m_uThresholdHigh));
  //vxBlockVolumeIterator<int4> volIterHistIntensity(pBinaryHistogramVolume->GetBinaryHistogramVolume16());

  for (iterVol.SetPos(0,0,0); iterVol.IsNotAtEnd(); iterVol.NextBlockZYX())
  {
    const Vector3D<int4> currentPos(iterVol.GetPos());

    // check if the entire block has data that is of interest, meaning
    // that it contains values that are within the specified range
    ////////////////////////////////////////////////////////////////////
    //volIterHistIntensity.SetPos(currentPos.X() >> 4, currentPos.Y() >> 4, currentPos.Z() >> 4);
    if ((iCode & iterVol.GetCurrentBlock().GetBinaryHistogram()) > 0)
    {
      int4 iCounter(0);

      for (; iterVol.IsNotAtEndOfBlock(); iterVol.NextZYXinsideBlock())
      {
        if ( validSubset.IsInside( Triplei( iterVol.GetPos() ) ) )
        {
          iterMaskTagVol.SetPos(iterVol.GetPos());
          if (iterMaskTagVol.GetVoxel() == false) // not locked...
          {
            const uint2 uVoxelIntensity(iterVol.GetVoxel());
            if (m_uThresholdLow <= uVoxelIntensity && uVoxelIntensity <= m_uThresholdHigh)
            {
              iterRegTagVol.SetPos(iterVol.GetPos());
              iterRegTagVol.SetVoxel(true);
              iterMaskTagVol.SetVoxel(true);
              iCounter++;
            }
          }
        }
      }
      if (iCounter == 4096)
      {
        iterRegTagVol.SetPos(currentPos);
        iterMaskTagVol.SetPos(currentPos);
        iterRegTagVol.ClearCurrentBlock();
        iterMaskTagVol.ClearCurrentBlock();
      }

      // update overall voxel counter
      m_uNumOfRegVoxels += iCounter;

    } // if block has values in range
  } // for all blocks

  LogDbg("Took " + ToAscii(timer.ElapsedTime()) + " seconds to perform auto segmentation", "Run", "RegionThresholdSegment");

  return;
} // RegionThresholdSegment()


/**
 * Public method that gets properties of the RegionThresholdSegment class
 * @param iPropID   property id
 * @param ...       variable argument list for get
 * @return Status of get
 */
int4 RegionThresholdSegment::Get(const int2 iPropID, ...)
//*******************************************************************
{
  int4 iStatus(0);
  va_list ap;
  va_start(ap,iPropID);

  switch (iPropID)
  {
    case REGION_THRESHOLD_RANGE:
    {
      int4 * pVar1 = va_arg(ap, int4*);
      if (pVar1 != NULL)
      {
        *pVar1 = m_uThresholdLow;
      }
      int4 * pVar2 = va_arg(ap, int4*);
      if (pVar2 != NULL)
      {
        *pVar2 = m_uThresholdHigh;
      }
    }
    break; // case REGION_THRESHOLD_RANGE
    case NUMBER_OF_REGION_VOXELS:
    {
      uint4 * pVar = va_arg(ap, uint4*);
      if (pVar != NULL)
      {
        *pVar = m_uNumOfRegVoxels;
      }
    }
    break; // case NUMBER_OF_REGION_VOXELS
    default:
    break;
  }

  va_end(ap);

  return(iStatus);
} // Get()


/**
 * Public method that sets properties of the RegionThresholdSegment class
 * @param iPropID   property id
 * @param ...       variable argument list for set
 * @return Status of set
 */
int4 RegionThresholdSegment::Set(const int2 iPropID, ...)
//*******************************************************************
{
  int4 iStatus(0);
  va_list ap;
  va_start(ap,iPropID);

  switch (iPropID)
  {
    case REGION_THRESHOLD_RANGE:
      m_uThresholdLow  = va_arg(ap, int4);
      m_uThresholdHigh = va_arg(ap, int4);
    break;
    default:
    break;
  }

  va_end(ap);

  return(iStatus);
} // Set()


#undef FILE_REVISION


// Revision History:
// $Log: RegionThresholdSegment.C,v $
// Revision 1.3  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.2.4.1  2007/04/25 19:46:20  romy
// converted threshold values to int4. Issue#5611
//
// Revision 1.2  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.17  2003/12/22 12:51:30  frank
// Issue #3593: Restricted segmentation to the visible volume on the shift key
//
// Revision 1.16  2003/06/19 20:20:06  michael
// adjusted segmentation from int2 to uint2, following the changes in the block volume.
//
// Revision 1.15  2003/06/18 15:19:54  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.14  2003/06/09 12:29:55  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.13  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.12  2003/05/16 13:20:38  frank
// formatting
//
// Revision 1.11  2003/04/28 17:01:31  michael
// more code reviews
//
// Revision 1.10  2003/04/14 21:06:03  michael
// performed initial code review
//
// Revision 1.9  2003/04/09 15:07:01  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.8  2003/04/08 20:54:54  michael
// cosmetics
//
// Revision 1.7  2002/08/02 14:55:19  michael
// Moved helper methods for 32 bit encoding from 79/vxRendererUtils to 70/vxUtils
//
// Revision 1.6  2002/07/30 20:22:18  dmitry
// bunch of stuff changed to vx
//
// Revision 1.5  2002/07/25 19:47:14  ingmar
// added relative path to include files that are further down in the dependency list (they really should not be referenced here)
//
// Revision 1.4  2002/07/22 23:17:21  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.3  2002/07/22 18:24:10  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.2  2002/06/28 19:03:12  soeren
// optimzed Run by checking homogenous blocks
//
// Revision 1.1.2.2  2002/07/02 22:24:51  michael
// added optimization using the binary histogram of the intensity volume for auto segmentation
//
// Revision 1.1.2.1  2002/07/01 14:13:57  jaddonisio
// Quicker way to set voxels by homogenous blocks per Soeren.
//
// Revision 1.1  2002/06/13 22:21:32  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/RegionThresholdSegment.C,v 1.3 2007/06/28 19:33:55 jmeade Exp $
// $Id: RegionThresholdSegment.C,v 1.3 2007/06/28 19:33:55 jmeade Exp $
