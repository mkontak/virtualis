// $Id: RegionThresholdSegment.h,v 1.2 2006/03/29 20:03:19 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/*
  Class definition for RegionThresholdSegment 
*/

#ifndef RegionThresholdSegment_h
#define RegionThresholdSegment_h

// forward declarations
template <class T> class vxBlockVolume;
template <class T> class Box;


// class definition
class RegionThresholdSegment
{
// enums
public:

  enum PropIDsEnum { 
    REGION_THRESHOLD_RANGE, 
    NUMBER_OF_REGION_VOXELS
  }; // PropIDsEnum

// functions
public:
  
  /// default constructor
  RegionThresholdSegment();

  /// destructor
  virtual ~RegionThresholdSegment() {};

  /// get properties
  int4 Get(const int2 iPropID, ...);

  /// set properties
  int4 Set(const int2 iPropID, ...);

  /// run method
  void Run(vxBlockVolume<uint2> * pScanVol, 
           vxBlockVolume<bool> * pMskTagVol, 
           vxBlockVolume<bool> * pRegTagVol,
           Box<int4> validSubset);

// data
private:

  /// number of voxels in region threshold
  uint4 m_uNumOfRegVoxels;  
  
  /// threshold low
  uint2  m_uThresholdLow;

  /// threshold high
  uint2  m_uThresholdHigh;
}; // RegionThresholdSegment


#endif // RegionThresholdSegment_h


// $Log: RegionThresholdSegment.h,v $
// Revision 1.2  2006/03/29 20:03:19  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/12/22 12:51:30  frank
// Issue #3593: Restricted segmentation to the visible volume on the shift key
//
// Revision 1.8  2003/06/19 20:20:06  michael
// adjusted segmentation from int2 to uint2, following the changes in the block volume.
//
// Revision 1.7  2003/06/18 15:19:55  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.6  2003/06/09 12:29:55  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.5  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.4  2003/05/16 13:20:38  frank
// formatting
//
// Revision 1.3  2003/04/14 21:06:03  michael
// performed initial code review
//
// Revision 1.2  2002/07/22 18:24:10  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.1.2.1  2002/07/02 22:24:51  michael
// added optimization using the binary histogram of the intensity volume for auto segmentation
//
// Revision 1.1  2002/06/13 22:21:32  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/RegionThresholdSegment.h,v 1.2 2006/03/29 20:03:19 geconomos Exp $
// $Id: RegionThresholdSegment.h,v 1.2 2006/03/29 20:03:19 geconomos Exp $
