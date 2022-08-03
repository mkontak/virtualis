/// $Id: vxSegmentFinder.h,v 1.8.2.1 2007/10/24 14:58:18 dongqing Exp $
//  $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxSegmentFinder.h,v 1.8.2.1 2007/10/24 14:58:18 dongqing Exp $
//
// Copyright(c) 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen, dongqing@viatronix.net
//
// Complete history at bottom of file.

/** 
*  This class is to extract all colon lumen segments and assigne indexes to them
*/


#ifndef vxSegmentFinder_h
#define vxSegmentFinder_h


#include "vxBlockVolume.h"


class vxSegmentFinder
{
  // member functions
public :

  /// constructor
	vxSegmentFinder( vxBlockVolume<uint2> *pVol, vxBlockVolume<uint1> *pMask, vxBlockVolume<uint1> *pIns,
		               const std::string & sourceFilePath, const std::string destFilePath );

  /// destructor
  ~vxSegmentFinder();

  /// extracting colonic segments
  bool Run();

  // member functions
private :

  /// grow out air segment
  int4 GrowOutAirSegment( const Vector3D<int2> & viSeed, const uint1 & uNewLabel );

  /// Grow out 3D region with 6-connected 
  int4 Grow3D6NB( vxBlockVolume<uint1> * pMask, const Vector3D<int2> & viSeed, 
								  const uint1 & uROIlabel, const uint1 & uNewLabel );

  /// smoothing the contour of the lumen segments
  void SmoothingLumenContour( const uint1 & uHighestLabelIndex );

  /// fill cavity in segments
	void FillCavity( const uint1 & uHighestLabelIndex );

  /// Determine segment shell
  int4 DetermineSegmentShell( const Vector3D<int2> & viSeed, const uint1 & uRoiLabel, 
		                          const uint1 & uNewLabel );

  /// Grow out 3D region with 6-connected neighbor 
  void GrowWithinShell( const Vector3D<int2> & viSeed, const uint1 & uROIlabel, const uint1 & uNewLabel );

  /// recover the colon segment contour layer by dilation
  void RecoverSegmentContourLayer( vxBlockVolumeIterator<uint1> & iti, vxBlockVolume<uint1> * pTmpMask );


  // member variables
private :
  /// volume
  vxBlockVolume<uint2> * m_pVol;

  /// ec mask
  vxBlockVolume<uint1> * m_pMask;

  /// ins mask
  vxBlockVolume<uint1> * m_pIns;

  /// output file path
  std::string m_sourceFilePath;
  std::string m_destFilePath;

  // label index
  uint1 m_uBGlabel;   // for background
  uint1 m_uTagLabel;  // for contrast enhanced residue

  // the thresholds for growing out air lumen
  uint2 m_uAirTh;

	// the thresholds for smallest air segment
	int4 m_iSmallestAirLumenVolume;
};

#endif

// complete revision history:
// $Log: vxSegmentFinder.h,v $
// Revision 1.8.2.1  2007/10/24 14:58:18  dongqing
// add post INS dilation for recovering the lumen lose
// that should fix the skinny overview only. It should not
// affect anything else
//
// Revision 1.8  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.7  2006/06/06 20:14:02  dongqing
// fix fill cavity bug for the vxSegmentFinder
//
// Revision 1.6  2006/04/21 20:55:28  dongqing
// First complete beta version
//
// Revision 1.5  2006/04/19 19:53:45  dongqing
// bug free beta version
//
// Revision 1.4  2006/04/06 12:35:41  dongqing
// rewrite the region growing by using Peek function instead of GetVoxel
//
// Revision 1.3  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.2  2006/03/02 21:22:43  dongqing
// check in initial DFB module
//
// Revision 1.1  2006/02/24 20:24:17  dongqing
// add INS module
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxSegmentFinder.h,v 1.8.2.1 2007/10/24 14:58:18 dongqing Exp $
// $Id: vxSegmentFinder.h,v 1.8.2.1 2007/10/24 14:58:18 dongqing Exp $
