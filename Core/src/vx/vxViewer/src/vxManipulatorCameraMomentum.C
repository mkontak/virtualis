// $Id: vxManipulatorCameraMomentum.C,v 1.2 2007/03/02 18:11:45 geconomos Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxManipulatorCameraMomentum.h"
#include "vxEnvironment.h"


/**
 * Constructor
 * @param environment
 */
vxManipulatorCameraMomentum::vxManipulatorCameraMomentum( vxEnvironment & environment ):
vxManipulator(environment)
{
} // vxManipulatorCameraMomentum()


/**
 * Destructor
 */
vxManipulatorCameraMomentum::~vxManipulatorCameraMomentum()
{
  // disable momentum
  if( GetEnvironment().GetMomentum()->GetEnabled() && GetEnvironment().GetMomentum()->GetInitiated() )
  {
    GetEnvironment().GetMomentum()->SetInitiated( false );
    GetEnvironment().SetUserInteracting( false );
  }
} // ~vxManipulatorCameraExamine()


// $Log: vxManipulatorCameraMomentum.C,v $
// Revision 1.2  2007/03/02 18:11:45  geconomos
// code review preparation
//
// Revision 1.1  2007/02/06 18:57:48  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorCameraMomentum.C,v 1.2 2007/03/02 18:11:45 geconomos Exp $
// $Id: vxManipulatorCameraMomentum.C,v 1.2 2007/03/02 18:11:45 geconomos Exp $
