// $Id: vxManipulatorCamera.h,v 1.2 2004/11/18 02:23:42 michael Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxManipulatorCamera_h
#define vxManipulatorCamera_h


// includes
#include "vxManipulator.h"


/**
 * This class inherits from ManipulatorCamera
 */
class VX_VIEWER_DLL vxManipulatorCamera : public vxManipulator
{
// functions
public:
  
  /// constructor
  vxManipulatorCamera(vxEnvironment & environment);
  
  /// destructor
  virtual ~vxManipulatorCamera() {}

protected:

  /// apply the six standard views (left, right, superior, inferior, anterior, posterior)
  bool ManipulateStandardViews(const uint1 uKeyId, const uint4 uKeyboardState);

  /// toggle the projection type between perspective and orthographic
  void ToggleProjectionType();

private:
  
  /// copy constructor
  vxManipulatorCamera(const vxManipulatorCamera & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxManipulatorCamera & operator=(const vxManipulatorCamera & other); // should only be public if really implemented!

}; // class vxManipulatorCamera


#endif // ManipulatorCameraExamine_h


// Revision History:
// $Log: vxManipulatorCamera.h,v $
// Revision 1.2  2004/11/18 02:23:42  michael
// fixed wrong logic that always made it handled == true by default and bailing
// out without setting it to false if CTRL was not down
//
// Revision 1.1  2004/05/25 14:30:14  frank
// added base class for camera manipulators
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorCamera.h,v 1.2 2004/11/18 02:23:42 michael Exp $
// $Id: vxManipulatorCamera.h,v 1.2 2004/11/18 02:23:42 michael Exp $
