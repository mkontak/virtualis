// $Id: vxManipulatorCameraMomentum.h,v 1.2 2007/03/02 18:11:45 geconomos Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: George Economos (mailto:george@viatronix.com)

#pragma once

// include declarations
#include "vxManipulator.h"

/**
 * Ensures that momentum is turned off when manupulator is removed. All other momentum logic is contained
 * in vxControllerMomentum.  Should revisit this solution.
 */
class VX_VIEWER_DLL vxManipulatorCameraMomentum : public vxManipulator
{
// member functions
public:
  
  /// constructor
  vxManipulatorCameraMomentum( vxEnvironment & environment );
  
  /// destructor
  virtual ~vxManipulatorCameraMomentum();

}; // class vxManipulatorCameraMomentum


// $Log: vxManipulatorCameraMomentum.h,v $
// Revision 1.2  2007/03/02 18:11:45  geconomos
// code review preparation
//
// Revision 1.1  2007/02/06 18:57:48  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorCameraMomentum.h,v 1.2 2007/03/02 18:11:45 geconomos Exp $
// $Id: vxManipulatorCameraMomentum.h,v 1.2 2007/03/02 18:11:45 geconomos Exp $
