/// $Id: vxLungRemoval.h,v 1.4 2007/03/06 16:34:01 dongqing Exp $
//  $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxLungRemoval.h,v 1.4 2007/03/06 16:34:01 dongqing Exp $
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
 *  Removing lung region
 */


#ifndef vxLungRemoval_h
#define vxLungRemoval_h


#include "vxBlockVolume.h"


class vxLungRemoval
{
  // member functions
public :

  /// constructor
  vxLungRemoval(vxBlockVolume<uint2> * pVol, vxBlockVolume<uint1> *pMask );

  /// destructor
  ~vxLungRemoval();

  /// apply bone removal procedure
  bool Run();

  // member functions
private :

  /// Grow out air region 
  void GrowAirRegion( Vector3D<int2> & viSeed, int4 & iNumOfAirLow, int4 & iNumOfAirHigh, uint1 uNewLabel );

  /// 3D region grow in 6-neighbors 
  void Grow3D6NB( Vector3D<int2> & viSeed, uint1 uROILabel, uint1 uNewLabel );

  /// Dilation
  void Dilation();

  // member variables
private :
  /// volume 
  vxBlockVolume<uint2> *m_pVol;

  /// segmentation mask
  vxBlockVolume<uint1> *m_pMask;

  /// background label index
  uint1 m_uBGLabel;

  /// air threshold for lung seed determination
  uint2 m_uAirThHigh;

  /// air threshold for lung region grow
  uint2 m_uAirThLow;

  /// z-length bound ratio for lung region
  float4 m_fZBratio;

};

#endif


// complete revision history:
// $Log: vxLungRemoval.h,v $
// Revision 1.4  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.3  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.2  2006/03/15 16:15:19  dongqing
// best algorith so far
//
// Revision 1.1  2006/02/06 21:44:18  dongqing
// initial check in
//
// Revision 1.0  2006/01/31  dongqing
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxLungRemoval.h,v 1.4 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxLungRemoval.h,v 1.4 2007/03/06 16:34:01 dongqing Exp $
