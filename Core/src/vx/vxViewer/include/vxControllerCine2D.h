// $Id: vxControllerCine2D.h,v 1.3 2007/03/06 19:20:11 gdavidson Exp $
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
 * Controller to cycle through all slices.
 */
class VX_VIEWER_DLL vxControllerCine2D : public vxController
{
public:

  /// constructor
  vxControllerCine2D( vxEnvironment & environment );

  /// destructor
  virtual ~vxControllerCine2D();

  /// determine if processing is necessary
  bool IsProcessingNecessary();

  /// process the data
  void Process();

private:

  /// cine for ortho planes
  void CinePlaneOrtho();

  /// cine for oblique planes
  void CinePlaneOblique();

private:

  /// tracks the direction for the swing
  bool m_bSwingForward;

  /// timer
  Timer m_timer;

}; // class vxControllerCine2D



// $Log: vxControllerCine2D.h,v $
// Revision 1.3  2007/03/06 19:20:11  gdavidson
// Added class comments
//
// Revision 1.2  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.1  2007/01/16 16:20:50  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerCine2D.h,v 1.3 2007/03/06 19:20:11 gdavidson Exp $
// $Id: vxControllerCine2D.h,v 1.3 2007/03/06 19:20:11 gdavidson Exp $

