// $Id: vxManipulatorFullScreen.h,v 1.1.2.3 2009/07/28 15:43:23 kchalupa Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#pragma once


// includes
#include "vxManipulator.h"


/**
 * This class inherits from ManipulatorCamera
 */
class VX_VIEWER_DLL vxManipulatorFullScreen : public vxManipulator
{
// construction
public:
  
  /// constructor
  vxManipulatorFullScreen( vxEnvironment & environment );
  
  /// destructor
  virtual ~vxManipulatorFullScreen() {}

// member functions
public:

  /// event handler can be reimplemented in a subclass to receive mouse down events
  virtual bool OnDoubleClick( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState );

}; // class vxManipulatorFullScreen


// Revision History:
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorFullScreen.h,v 1.1.2.3 2009/07/28 15:43:23 kchalupa Exp $
// $Id: vxManipulatorFullScreen.h,v 1.1.2.3 2009/07/28 15:43:23 kchalupa Exp $
