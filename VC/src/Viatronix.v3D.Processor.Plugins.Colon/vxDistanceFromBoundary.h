/// $Id: vxDistanceFromBoundary.h,v 1.6 2007/03/06 16:34:01 dongqing Exp $
//  $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxDistanceFromBoundary.h,v 1.6 2007/03/06 16:34:01 dongqing Exp $
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
*  This class is to compute DFB for both internal and external
*/


#ifndef vxDistanceFromBoundary_h
#define vxDistanceFromBoundary_h


#include "vxBlockVolume.h"


class vxDistanceFromBoundary
{
  // member functions
public :

  /// constructor
	vxDistanceFromBoundary( vxBlockVolume<uint1> *pIns, vxBlockVolume<uint1> *pSld,
		           const std::string & sourceFilePath, const std::string destFilePath );

  /// destructor
  ~vxDistanceFromBoundary();

  /// compute internal DFB
  bool ComputeDFB();

  /// compute the external DFB
  bool ComputeDFBExt();

  // member functions
private :

  /// label colon wall
  void IdentifyColonWall(vxBlockVolume<uint1> * pMask, const uint1 & uWallLabel);

  // member variables
private :
  /// ins mask
  vxBlockVolume<uint1> * m_pIns;

  /// sld
  vxBlockVolume<uint1> * m_pSld;

  /// output file path
  std::string m_sourceFilePath;
  std::string m_destFilePath;

  /// predefine the possible maximum value
  float4 m_fMaxDfb;
};

#endif

// complete revision history:
// $Log: vxDistanceFromBoundary.h,v $
// Revision 1.6  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.5  2006/04/21 20:55:28  dongqing
// First complete beta version
//
// Revision 1.4  2006/04/12 20:52:43  dongqing
// Rewrite CRES to get rid of gradient feature
//
// Revision 1.3  2006/04/06 15:08:57  dongqing
// rewrite the iterator
//
// Revision 1.2  2006/03/03 20:47:00  dongqing
// finalize DFB codes
//
// Revision 1.1  2006/03/02 21:22:43  dongqing
// check in initial DFB module
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxDistanceFromBoundary.h,v 1.6 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxDistanceFromBoundary.h,v 1.6 2007/03/06 16:34:01 dongqing Exp $
