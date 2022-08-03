// $Id: vxControllerMomentum.h,v 1.2 2007/03/06 19:20:11 gdavidson Exp $
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


/**
 * Controller to update the camera position simulating momentum.
 */
class VX_VIEWER_DLL vxControllerMomentum : public vxController
{
public:

  /// constructor
  vxControllerMomentum( vxEnvironment & environment );

  /// determine if processing is necessary
  virtual bool IsProcessingNecessary();

  /// process the data
  virtual void Process();

private:

  /// timer
  Timer m_timer;

}; // class vxControllerMomentum



// $Log: vxControllerMomentum.h,v $
// Revision 1.2  2007/03/06 19:20:11  gdavidson
// Added class comments
//
// Revision 1.1  2007/01/16 16:20:50  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerMomentum.h,v 1.2 2007/03/06 19:20:11 gdavidson Exp $
// $Id: vxControllerMomentum.h,v 1.2 2007/03/06 19:20:11 gdavidson Exp $

