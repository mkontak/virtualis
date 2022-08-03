/// $Id: vxBackgroundRemoval.h,v 1.3 2007/03/06 16:34:01 dongqing Exp $
//  $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxBackgroundRemoval.h,v 1.3 2007/03/06 16:34:01 dongqing Exp $
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
*  This class is to remove background and extract the body voxel for 
*  the following processing stage. The boundary of the mask will be labeled 
*  with 0xFF. The body region will be labeled with 0xFE. All other regions 
*  will be labeled as 0x00.
*
*  RISK ANALYSIS: If FOV is so small that air lung area covers slice conner, 
*      the lung area will be labeled as background. At this situation, if a
*      air colon segment closely touches the same lung area, the colon area
*      will also be incorrectly removed as background.
*/


#ifndef vxBackgroundRemoval_h
#define vxBackgroundRemoval_h


#include "vxBlockVolume.h"


class vxBackgroundRemoval
{
  // member functions
public :

  /// constructor
  vxBackgroundRemoval( vxBlockVolume<uint2> *pVol, vxBlockVolume<uint1> *pMask );

  /// destructor
  ~vxBackgroundRemoval();

  /// apply background removal procedure
  bool Run();

private :

  /// Extract body region
  bool ExtractBodyRegion();

  /// Grow out air background outside the body region
  void GrowBGInSlice( uint1 *pSliceMask, int4 seedOffset, uint1 uNewLabel );

  /// dilation 2 layers for background
  void DilateBackground( uint1 * puSegMask );

  /// 4-neighbor region grow
  void Grow2D4NB( uint1 *puSliceMask, int4 iSeed, uint1 uRoiLabel, uint1 uNewLabel );

  /// 4-neighbor region grow
  void Grow2D4NB( uint1 *puSliceMask, int4 iSeed, uint1 uRoiLabel, uint1 uRoiLabel1, uint1 uNewLabel );

  // member variables
private :
  /// volume
  vxBlockVolume<uint2> * m_pVol;

  /// segmentation mask
  vxBlockVolume<uint1> * m_pMask;

  /// threshold for background
  uint2 m_uBackgroundTh;

  /// label index for the extracted body region
  uint1 m_uBGlabel;

  /// upper bound for the table
  int4 m_iTableUpBound;

};

#endif

// complete revision history:
// $Log: vxBackgroundRemoval.h,v $
// Revision 1.3  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.2  2006/03/14 21:22:45  dongqing
// 1st step tune up
//
// Revision 1.1  2006/02/06 21:44:18  dongqing
// initial check in
//
// Revision 1.0  2006/01/31
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxBackgroundRemoval.h,v 1.3 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxBackgroundRemoval.h,v 1.3 2007/03/06 16:34:01 dongqing Exp $
