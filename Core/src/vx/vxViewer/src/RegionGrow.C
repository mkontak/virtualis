// $Id: RegionGrow.C,v 1.3 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/**
 * Class that encapsulates Region Grow image processing.
 */


// includes
#include "stdafx.h"
#include "RegionGrow.h"
#include "vxBlockVolumeRegionGrowIteratorHist.h"
#include "vxDeciderVoxelValue.h"
#include "Timer.h"
#include "Box.h"


// namespace declarations
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Default constructor
 */
RegionGrow::RegionGrow()
:m_uNumOfRegVoxels(0),
m_iThresholdLow(0),
m_iThresholdHigh(0)
//*******************************************************************
{
}


/**
 * Public method that gets properties of the RegionGrow class
 * @param iPropID   property id
 * @param ...       variable argument list for get
 * @return Status of get
 */
int4 RegionGrow::Get(const int2 iPropID, ...)
//*******************************************************************
{
  int4 iStatus(0);
  va_list ap;
  va_start(ap, iPropID);

  switch (iPropID)
  {
    case REGION_THRESHOLD_RANGE:
    {
      int4 * pVar1 = va_arg(ap, int4*);
      if (pVar1 != NULL)
      {
        *pVar1 = m_iThresholdLow;
      }
      int4 * pVar2 = va_arg(ap, int4*);
      if (pVar2 != NULL)
      {
        *pVar2 = m_iThresholdHigh;
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
  } // end switch

  va_end(ap);

  return(iStatus);
} // Get()


/**
 * Public method that sets properties of the RegionGrow class.
 * @param iPropID   property id
 * @param ...       variable argument list for set
 * @return Status of set
 */
int4 RegionGrow::Set(const int2 iPropID, ...)
//*******************************************************************
{
  int4 iStatus(0);
  va_list ap;
  va_start(ap, iPropID);

  switch (iPropID)
  {
    case REGION_THRESHOLD_RANGE:
      m_iThresholdLow  = va_arg(ap, int4);
      m_iThresholdHigh = va_arg(ap, int4);
    break;
    default:
    break;
  }
  va_end(ap);
  return(iStatus);
} // Set()


/**
 * Public method that runs the RegionGrow class
 * @param seedPoint   reference to a 3D point coordinates of seed used in region grow
 * @param pScanVol    pointer to a scan volume
 * @param pMskTagVol  pointer to a mask tag volume
 * @param pRegTagVol  pointer to a region tag volume
 */
void RegionGrow::Run( const Point<uint2> & seedPoint,
                      vxBlockVolume<uint2> * pScanVol,
                      vxBlockVolume<bool> * pMskTagVol,
                      vxBlockVolume<bool> * pRegTagVol,
                      Box<int4> validSubset )
{

  try
  {
    Timer timer;

    // decider that declares a voxel as part of a region if within a specified range
    vxDeciderVoxelValueRange<int4> decider(m_iThresholdLow, m_iThresholdHigh);

    // create a new mask that incorporates both the valid subset and the locked mask
    vxBlockVolume<bool> compositeMaskVol( * pMskTagVol );
    vxBlockVolumeIterator<bool> compositeIter( compositeMaskVol );
    compositeIter.SetClearBlock( true );
    for ( compositeIter.SetPos(0,0,0); compositeIter.IsNotAtEnd(); compositeIter.NextBlockZYX() )
    {

      // Check to see if the entire block is outside the valid subset and if so,
      // set the entire block true (locked).  Otherwise, check voxel by voxel
      // inside the block.
      const Vector3Di blockSize( 15, 15, 15 );
      Box<int4> blockPosition( Point3Di( compositeIter.GetPos() ), Point3Di( compositeIter.GetPos() + blockSize ) );
      if ( blockPosition.IfOverlap( validSubset ) )
      {

        // check if the entire block is within the valid subset
        if ( validSubset.GetUnion( blockPosition ) == validSubset )
        {

          // Block is entirely within the valid subset.
          // Change nothing since it was originally copied from the mask tag volume.

        } // if block is entirely with the valid subset
        else
        {

          // The block is on the edge of the valid subset
          // check voxel-by-voxel
          for ( ; compositeIter.IsNotAtEndOfBlock(); compositeIter.NextZYXinsideBlock() )
          {
            if ( compositeIter.GetVoxel() == false &&
              validSubset.IsInside( Triplei( compositeIter.GetPos() ) ) == false )
            {
              compositeIter.SetVoxel( true );
            }
          } // for all voxels in block
        } // if block is on the edge of the valid subset
      } // if overlapping
      else
      {

        // entire block is locked
        compositeIter.ClearCurrentBlock();

      } // if block is completely outside
    } // loop over all blocks
    LogDbg( "Mask creation time: " + timer.AsString(), "", "" );
    
    // region grow iterator
    Vector3D<int4> iSeedPoint(seedPoint.X(), seedPoint.Y(), seedPoint.Z());    
    vxBlockVolumeRegionGrowIteratorHist regGrowIter(iSeedPoint, *pScanVol,
                                                    decider, & compositeMaskVol, pRegTagVol);
    // run region grow
    m_uNumOfRegVoxels = regGrowIter.Run();
 
    // consolidate homogenous blocks
    pRegTagVol->Compact();
    
    LogDbg(string("RegionGrow::Run took ") + ToAscii(timer.ElapsedTime()), "RegionGrow", "Run");

  } // end try
  catch (...)
  {
    throw ex::VException(LogRec("RegionGrow::Run failed.","RegionGrow","Run"));    
  }

} // RegionGrow()


#undef FILE_REVISION 


// $Log: RegionGrow.C,v $
// Revision 1.3  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.2.4.1  2007/04/25 19:45:46  romy
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
// Revision 1.34  2003/12/22 12:49:54  frank
// Issue #3593: Restricted segmentation to the visible volume on the shift key
//
// Revision 1.33  2003/07/08 14:41:00  michael
// cosmetics
//
// Revision 1.31  2003/06/19 20:20:05  michael
// adjusted segmentation from int2 to uint2, following the changes in the block volume.
//
// Revision 1.30  2003/06/18 15:19:54  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.29  2003/06/09 12:29:55  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.28  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.27  2003/05/16 13:20:37  frank
// formatting
//
// Revision 1.26  2003/04/28 17:01:31  michael
// more code reviews
//
// Revision 1.25  2003/04/14 21:06:04  michael
// performed initial code review
//
// Revision 1.24  2003/03/26 03:30:20  michael
// formatting
//
// Revision 1.23  2002/12/12 17:07:38  ingmar
// removed binary histogram 04 (3x better histogram creation time, only slightly slower raw region growing, faster overall region grow time when also counting volume updates)
//
// Revision 1.22  2002/09/10 19:04:25  jaddonisio
// i. New call to region grow(passing pMsk04Vol).
// ii. Fix bug in RegionSegmentThreshold by removing bad region with index.
//
// Revision 1.21  2002/07/25 20:15:42  jaddonisio
// Call to Compact() for assurance.
//
// Revision 1.20  2002/07/22 23:17:21  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.19  2002/07/22 18:24:10  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.18  2002/07/10 17:41:25  ingmar
// added timing info to be written to log file for region growing
//
// Revision 1.17  2002/07/02 17:15:55  soeren
// optimzed regib grow through new compact function
//
// Revision 1.16.2.1  2002/07/02 20:30:14  jaddonisio
// Call Compact() after region grow.
//
// Revision 1.16  2002/05/21 21:06:23  jaddonisio
// Replaced SetVoxel with call without iterator(temporarily).
//
// Revision 1.15  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.14  2002/05/03 21:19:11  soeren
// made it faster :-)
//
// Revision 1.13  2002/05/02 22:45:08  ingmar
// applied new region grow iterator for double speed
//
// Revision 1.12  2002/05/01 14:06:44  jaddonisio
// Clean-up.
//
// Revision 1.11  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.10  2001/12/17 16:38:39  jaddonisio
// Changed Seta and Get to pass iPropID by value(to fix problem when value is an enum).
//
// Revision 1.9  2001/12/07 23:51:27  jaddonisio
// Corrected function comments for doxygen.
//
// Revision 1.8  2001/12/06 23:54:21  jaddonisio
// More mods for coding standard compliance.
//
// Revision 1.7  2001/12/06 20:10:54  jaddonisio
// Changed iPropID as reference into Get/Set.
//
// Revision 1.6  2001/12/06 17:06:07  jaddonisio
// Mods to comply with coding standard.
//
// Revision 1.5  2001/11/18 00:38:36  jaddonisio
// New member variable, 'number of voxels in region".
//
// Revision 1.4  2001/11/09 21:52:16  jaddonisio
// comments
//
// Revision 1.3  2001/11/06 21:42:15  jaddonisio
// Fixed comments and implemented Get().
//
// Revision 1.2  2001/11/02 19:37:56  jaddonisio
// Set flags to false in new constructor.
//
// Revision 1.1  2001/10/31 22:32:58  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/RegionGrow.C,v 1.3 2007/06/28 19:33:55 jmeade Exp $
// $Id: RegionGrow.C,v 1.3 2007/06/28 19:33:55 jmeade Exp $
