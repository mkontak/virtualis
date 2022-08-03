// $Id: vxManipulatorCameraPath.h,v 1.1 2004/05/25 14:30:14 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxManipulatorCameraPath_h
#define vxManipulatorCameraPath_h


// Includes
#include "vxManipulatorCamera.h"


/**
 * A manipulator to help you fly along the centerline of a vessel.
 */
class VX_VIEWER_DLL vxManipulatorCameraPath : public vxManipulatorCamera
{
// Member Functions
public:

  /// Constructor
  vxManipulatorCameraPath( vxEnvironment & environment );

  /// Destructor
  ~vxManipulatorCameraPath() {};

  /// This event handler can be reimplemented in a subclass to receive mouse down events for the viewer.
  virtual bool OnButtonDown( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive mouse up events for the viewer.     
  virtual bool OnButtonUp( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive mouse move events for the viewer.     
  virtual bool OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive wheel events for the viewer. 
  virtual bool OnMouseWheel( const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive keyboard events for the viewer.     
  virtual bool OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState );

// Utility Functions
private:

  /// Update the camera position to the current position along the path
  void UpdateCameraPosition();

  /// Update the cursor3D position to the current position along the path
  void UpdateCursor3DPosition();

  /// Translate the section by a certain distance
  void TranslateSection( const float4 fDistance );

private:

  /// if we are currently rotating the view
  bool m_bRotating;

  /// if we are currently dragging the middle mouse button to fly along the centerline
  bool m_bDragging;

  /// the up direction when you start rotating the viewpoint
  Normal3Df m_upDirection;

  /// the right directio when you start rotating the viewpoint
  Normal3Df m_rightDirection;

}; // class vxManipulatorCameraPath


#endif // vxManipulatorCameraPath_h


// $Log: vxManipulatorCameraPath.h,v $
// Revision 1.1  2004/05/25 14:30:14  frank
// added base class for camera manipulators
//
// Revision 1.1  2004/04/06 18:19:30  frank
// renamed vessel to path
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/09/08 20:05:45  frank
// Added more features such as updating cursor 3D and arrow key navigation
//
// Revision 1.5  2003/08/29 19:11:28  frank
// Cleaned up rendering of arrow and path navigation
//
// Revision 1.4  2003/08/18 17:35:57  frank
// Added middle-mouse dragging along the path
//
// Revision 1.3  2003/08/18 15:11:20  frank
// Fixed scroll-wheel navigation
//
// Revision 1.2  2003/08/14 16:55:06  frank
// Added turning of viewpoint
//
// Revision 1.1  2003/08/13 20:03:35  frank
// moved from Vascular project
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorCameraPath.h,v 1.1 2004/05/25 14:30:14 frank Exp $
// $Id: vxManipulatorCameraPath.h,v 1.1 2004/05/25 14:30:14 frank Exp $
