// $Id: RegionGrow.h,v 1.3 2007/06/28 19:34:09 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/**
 * Class that encapsulates region grow image processing.
 */

#ifndef RegionGrow_h
#define RegionGrow_h


// includes
#include "Point.h"
#include "vxBlockVolume.h"


// forward declarations
template <class T> class vxBlockVolume;
template <class T> class Box;


// class definition
class RegionGrow
{
public:
  
  /// default constructor
  RegionGrow();

  /// destructor
  virtual ~RegionGrow() {};

// enums
public:

  enum PropIDsEnum { REGION_THRESHOLD_RANGE, NUMBER_OF_REGION_VOXELS };

// functions
public:

  /// get properties
  int4 Get(const int2 iPropID, ...);

  /// set properties
  int4 Set(const int2 iPropID, ...);

  /// run method
  void Run(const Point<uint2> & seedPoint, vxBlockVolume<uint2> * pScanVol, 
           vxBlockVolume<bool> * pMskTagVol, vxBlockVolume<bool> * pRegTagVol,
           Box<int4> validSubset);

// data
private:

  /// number of growth voxels in region
  uint4 m_uNumOfRegVoxels;

  /// threshold low
  int4  m_iThresholdLow;    

  // threshold high
  int4  m_iThresholdHigh;
}; // RegionGrow


#endif // RegionGrow_h


// Revision History:
// $Log: RegionGrow.h,v $
// Revision 1.3  2007/06/28 19:34:09  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.2.4.1  2007/04/25 19:45:46  romy
// converted threshold values to int4. Issue#5611
//
// Revision 1.2  2006/03/29 20:03:19  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.20  2003/12/22 12:49:54  frank
// Issue #3593: Restricted segmentation to the visible volume on the shift key
//
// Revision 1.19  2003/06/19 20:20:05  michael
// adjusted segmentation from int2 to uint2, following the changes in the block volume.
//
// Revision 1.18  2003/06/18 15:19:54  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.17  2003/06/09 12:29:55  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.16  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.15  2003/05/16 13:20:38  frank
// formatting
//
// Revision 1.14  2003/04/14 21:06:04  michael
// performed initial code review
//
// Revision 1.13  2002/12/12 17:07:38  ingmar
// removed binary histogram 04 (3x better histogram creation time, only slightly slower raw region growing, faster overall region grow time when also counting volume updates)
//
// Revision 1.12  2002/09/10 19:04:25  jaddonisio
// i. New call to region grow(passing pMsk04Vol).
// ii. Fix bug in RegionSegmentThreshold by removing bad region with index.
//
// Revision 1.11  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.10  2002/05/01 14:06:44  jaddonisio
// Clean-up.
//
// Revision 1.9  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.8  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.7  2001/12/17 16:38:39  jaddonisio
// Changed Seta and Get to pass iPropID by value(to fix problem when value is an enum).
//
// Revision 1.6  2001/12/06 20:10:54  jaddonisio
// Changed iPropID as reference into Get/Set.
//
// Revision 1.5  2001/12/06 17:06:07  jaddonisio
// Mods to comply with coding standard.
//
// Revision 1.4  2001/11/18 00:38:36  jaddonisio
// New member variable, 'number of voxels in region".
//
// Revision 1.3  2001/11/09 21:52:16  jaddonisio
// comments
//
// Revision 1.2  2001/11/06 21:41:37  jaddonisio
// renamed argument
//
// Revision 1.1  2001/10/31 22:32:58  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/RegionGrow.h,v 1.3 2007/06/28 19:34:09 jmeade Exp $
// $Id: RegionGrow.h,v 1.3 2007/06/28 19:34:09 jmeade Exp $
