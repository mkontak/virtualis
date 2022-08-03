// $Id: vxControllerCameraFly.h,v 1.2 2007/03/06 19:20:11 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#pragma once


// includes
#include "vxController.h"
#include "Timer.h"
#include "vxSubmarine.h"


/**
 * Controller to update the camera position while it is flying.
 */
class VX_VIEWER_DLL vxControllerCameraFly : public vxController
{
public:

  /// constructor
  vxControllerCameraFly( vxEnvironment & environment );

  /// determine if processing is necessary
  bool IsProcessingNecessary();

  /// process the data
  void Process();

private:
  
  /// the navigation model
  bool m_bIsFlying;

  /// if flying forward or not
  bool m_bFlyingForward;

  /// if flying in automatic flight mode
  bool m_bIsAutoFlyMode;

  /// the position of maximum depth toward which to fly
  Vector3D<float4> m_fMaxDepthPos2d;

  /// timer
  Timer m_timer;

}; // class vxControllerCine2D



// $Log: vxControllerCameraFly.h,v $
// Revision 1.2  2007/03/06 19:20:11  gdavidson
// Added class comments
//
// Revision 1.1  2007/01/16 16:20:50  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerCameraFly.h,v 1.2 2007/03/06 19:20:11 gdavidson Exp $
// $Id: vxControllerCameraFly.h,v 1.2 2007/03/06 19:20:11 gdavidson Exp $

