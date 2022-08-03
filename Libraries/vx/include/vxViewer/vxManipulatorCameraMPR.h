// $Id: vxManipulatorCameraMPR.h,v 1.2 2004/10/20 20:03:34 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxManipulator.h"


/**
 * Manipulator for the camera in MPR views
 */
class VX_VIEWER_DLL vxManipulatorCameraMPR : public vxManipulator
{
// functions
public:

  // constructor
  vxManipulatorCameraMPR( vxEnvironment & environment );

  // destructor
  ~vxManipulatorCameraMPR() {};

  /// event handler can be reimplemented in a subclass to receive keyboard events for the viewer
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

private:

  /// copy constructor
  vxManipulatorCameraMPR(const vxManipulatorCameraMPR &);                   // should only be public if really implemented!
  
  /// assignment operator
  const vxManipulatorCameraMPR & operator=(const vxManipulatorCameraMPR &); // should only be public if really implemented!

  /// zoom in or out
  void ZoomCamera( bool bZoomIn );

  /// pan the camera around on the plan
  void PanCamera( const Point2D<int2> & direction );
  
}; // vxManipulatorCameraMPR


// $Log: vxManipulatorCameraMPR.h,v $
// Revision 1.2  2004/10/20 20:03:34  frank
// added pan with arrow keys on MPRs
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2004/01/20 20:07:19  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorCameraMPR.h,v 1.2 2004/10/20 20:03:34 frank Exp $
// $Id: vxManipulatorCameraMPR.h,v 1.2 2004/10/20 20:03:34 frank Exp $
