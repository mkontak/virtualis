/// $Id: vxBoneRemoval.h,v 1.4 2007/03/06 16:34:01 dongqing Exp $
//  $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxBoneRemoval.h,v 1.4 2007/03/06 16:34:01 dongqing Exp $
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
 *  Removing bone region. this is only for CT colonography studies. Not for
 *  general purpose
 */


#ifndef vxBoneRemoval_h
#define vxBoneRemoval_h


#include "vxBlockVolume.h"


class vxBoneRemoval
{
  // member functions
public :

  /// constructor
  vxBoneRemoval( vxBlockVolume<uint2> * pVol, vxBlockVolume<uint1> *pMask );

  /// destructor
  ~vxBoneRemoval();

  /// apply bone removal procedure
  bool Run();

private :

  /// Thresholdting region grow in 6-neighbors 
  void Grow3D6NBth( Vector3D<int2> viSeed, uint1 uROIlabel, uint1 uNewLabel, uint2 uThreshold );

  /// 26-neighbor dilation for one layer
  void DilateBone( uint1 & uROIlabel, uint1 & uNewLabel );

  // member variables
private :
  /// volume 
  vxBlockVolume<uint2> *m_pVol;

  /// segmentation mask
  vxBlockVolume<uint1> *m_pMask;

  /// background label index
  uint1 m_uBGlabel;
};

#endif


// complete revision history:
// $Log: vxBoneRemoval.h,v $
// Revision 1.4  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.3  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.2  2006/03/15 19:03:51  dongqing
// speed up by optimizing iterators
//
// Revision 1.1  2006/02/06 21:44:18  dongqing
// initial check in
//
// Revision 1.0  2006/01/31  dongqing
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxBoneRemoval.h,v 1.4 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxBoneRemoval.h,v 1.4 2007/03/06 16:34:01 dongqing Exp $
