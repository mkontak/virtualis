// $Id: vxManipulatorCameraFly.h,v 1.3 2007/01/16 17:04:31 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Ingmar Bitter (ingmar@viatronix.com)

#ifndef vxManipulatorCameraFly_h
#define vxManipulatorCameraFly_h


// includes
#include "vxManipulatorCamera.h"
#include "vxSubmarine.h"


/**
 * Manipulator for camera flight
 */
class VX_VIEWER_DLL vxManipulatorCameraFly : public vxManipulatorCamera
{
// functions
public:
  
  /// constructor
  vxManipulatorCameraFly(vxEnvironment & environment);
  
  /// destructor
  ~vxManipulatorCameraFly() {};
  
  /// event handler for receiving wheel events for the viewer
  virtual bool OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState);
  
  /// event handler for receiving mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);
  
  /// event handler for mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);
  
  /// event handler for receiving mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);
  
  /// event handler for receiving keyboard events for the viewer
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

private:

  /// compute an image space gradient in which direction the camera should rotate
  void GetForcesFromDepthImage(Vector3D<float4> & fForceVec, Vector3D<float4> & fPanVec);

  /// modify the camera
  void UpdateCameraFlySettings( const Point2D<int2> & position, const uint4 uKeyboardState );

// members
private:

  /// the position of maximum depth toward which to fly
  Vector3D<float4> m_fMaxDepthPos2d;

  /// Copy constructor
  vxManipulatorCameraFly(const vxManipulatorCameraFly &);                   // should only be public if really implemented!
  
  /// Assignment operator
  const vxManipulatorCameraFly & operator=(const vxManipulatorCameraFly &); // should only be public if really implemented!
  
}; // vxManipulatorCameraFly


#endif // ManipulatorCameraFly_h


// $Log: vxManipulatorCameraFly.h,v $
// Revision 1.3  2007/01/16 17:04:31  gdavidson
// Removed OnTimer
//
// Revision 1.2  2004/05/25 14:30:14  frank
// added base class for camera manipulators
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/11/05 13:40:21  frank
// Added the ability to fly backward
//
// Revision 1.7  2003/05/16 13:21:33  frank
// formatting
//
// Revision 1.6  2003/05/01 18:50:43  dongqing
// code review
//
// Revision 1.5  2003/04/29 21:17:35  michael
// removed vxManipulatorCamera from inheritance
//
// Revision 1.4  2002/11/12 16:32:06  ingmar
// added combination of pan and rotate to facilitate auto flight without centerline
//
// Revision 1.3  2002/11/12 00:31:52  ingmar
// smoother autoflight
//
// Revision 1.2  2002/11/11 22:45:37  ingmar
// first implementation of auto-fly mode
// also slight restructuring of Camera update code
// dropped non-submarine camera update code
//
// Revision 1.1  2002/10/21 23:19:30  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorCameraFly.h,v 1.3 2007/01/16 17:04:31 gdavidson Exp $
// $Id: vxManipulatorCameraFly.h,v 1.3 2007/01/16 17:04:31 gdavidson Exp $
