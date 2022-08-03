// $Id: vxTriangulator.h,v 1.2 2007/03/06 16:34:01 dongqing Exp $
//
// Copyright (c) 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen  (dongqing@viatronix.net)


// pragmas
#pragma once


// includes
#include "BSPTreePolygon.h"
#include "MCToObjTransfer.h"
#include "ViaStrip.h"
#include "vxBlockVolume.h"


/**
 *  A class to compute the colon overview triangle set.
 */
class vxTriangulator {

// member functions
public:

  /// Constructor.
	vxTriangulator( vxBlockVolume<uint1> *pIns, const std::string & destFilePath );

  /// destructor
  ~vxTriangulator();

  /// this does all the work needed to create the triangle set.
  bool Run();

// data members
private:

  /// the volume labeling the inside region.
  vxBlockVolume<uint1> * m_pIns;

  /// number of segments in multi-segments application.
  int4 m_numSegments;

  /// the root of the file name
  /// the variable is used to guide the generation of temporally disk file.
  std::string m_sFileNameRoot;

  /// the size of data volume
  Triple <uint4> m_size;
}; // Class vxTriangulator


// $Log: vxTriangulator.h,v $
// Revision 1.2  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.1  2006/03/07 13:30:18  dongqing
// initial check in the TRI stage codes
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxTriangulator.h,v 1.2 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxTriangulator.h,v 1.2 2007/03/06 16:34:01 dongqing Exp $


