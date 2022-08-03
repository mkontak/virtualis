// $Id: vxManipulatorCameraExamine.h,v 1.8 2007/02/05 15:29:25 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: jenny hizver (jenny@viatronix.com)

/**
 * This class inherits from ManipulatorCamera
 */

#ifndef vxManipulatorCameraExamine_h
#define vxManipulatorCameraExamine_h


// includes
#include "vxManipulatorCamera.h"
#include "Normal.h"


// forward declarations


// class definition
class VX_VIEWER_DLL vxManipulatorCameraExamine : public vxManipulatorCamera
{
// functions
public:
  
  /// constructor
  vxManipulatorCameraExamine(vxEnvironment & environment);
  
  /// destructor
  virtual ~vxManipulatorCameraExamine();
  
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
  
  /// copy constructor
  vxManipulatorCameraExamine(const vxManipulatorCameraExamine & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxManipulatorCameraExamine & operator=(const vxManipulatorCameraExamine & other); // should only be public if really implemented!

  /// returns the quadrant the current mouse position is in
  uint1 GetQuadrant();

// data
private:

  /// rotate around volume center
  bool m_bRotateVolume;

  /// translate
  bool m_bTranslate;

  ///Flag indicates that first mouse move signal
  bool m_bFirstMouseMove;
  
  /// momentum: up vector
  Normal<float4> m_upVector;

  /// momentum: right vector
  Normal<float4> m_rightVector;

  /// momentum: view vector
  Normal<float4> m_viewVector;

}; // vxManipulatorCameraExamine


#endif // ManipulatorCameraExamine_h


// $Log: vxManipulatorCameraExamine.h,v $
// Revision 1.8  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.7  2007/01/16 17:04:42  gdavidson
// Removed OnTimer
//
// Revision 1.6  2007/01/04 18:02:29  romy
// adjusted the Undo action only for the first mouse movement
//
// Revision 1.5  2006/07/07 13:34:37  geconomos
// set "UserInteracting" flag in associated environment as needed
//
// Revision 1.4  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.12.1  2005/08/12 17:20:51  geconomos
// Implmentation of vxMomentum environment element
//
// Revision 1.3  2004/05/25 14:30:14  frank
// added base class for camera manipulators
//
// Revision 1.2  2004/04/05 18:31:03  frank
// removed eyepoint rotation from camera examine manipulator
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.17  2003/06/19 20:21:21  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.16  2003/05/16 13:21:33  frank
// formatting
//
// Revision 1.15  2003/05/16 11:45:25  michael
// code review
//
// Revision 1.14  2003/05/01 18:50:43  dongqing
// code review
//
// Revision 1.13  2003/04/29 21:17:35  michael
// removed vxManipulatorCamera from inheritance
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorCameraExamine.h,v 1.8 2007/02/05 15:29:25 geconomos Exp $
// $Id: vxManipulatorCameraExamine.h,v 1.8 2007/02/05 15:29:25 geconomos Exp $
