// $Id: vxManipulatorSliceShadows.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © (2002), Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

#ifndef vxManipulatorSliceShadows_h
#define vxManipulatorSliceShadows_h


// includes
#include "vxManipulator.h"


// forward declarations
class vxEnvironment;


/**
 *  Class to handle the setting of the slice shadow
 */
class VX_VIEWER_DLL vxManipulatorSliceShadows : public vxManipulator
{
// functions
public:
  
  /// constructor
  vxManipulatorSliceShadows(vxEnvironment & environment);

  /// destructor
  ~vxManipulatorSliceShadows() {};
  
  /// event handler to receive mouse enter events for the viewer
  virtual void OnMouseEnter();

  /// event handler for receiving wheel events for the viewer
  virtual bool OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState);
  
  /// event handler can be reimplemented in a subclass to receive keyboard events for the viewer
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

private:
    
  /// copy constructor
  vxManipulatorSliceShadows(const vxManipulatorSliceShadows &);                   // should only be public if really implemented!
  
  /// assignment operator
  const vxManipulatorSliceShadows & operator=(const vxManipulatorSliceShadows &); // should only be public if really implemented!

  /// if slice shadow is enabled, set slice shadow ptr to the current orthoplane position
  void SetSliceShadowPtr();
}; // vxManipulatorSliceShadows


#endif // vxManipulatorSliceShadows_h


// $Log: vxManipulatorSliceShadows.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/05/16 13:21:35  frank
// formatting
//
// Revision 1.9  2003/05/07 15:01:30  dongqing
// code walkthrough
//
// Revision 1.8  2003/05/07 11:44:02  michael
// initial code review
//
// Revision 1.7  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.6  2002/08/20 22:51:13  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.5  2002/07/02 20:44:59  geconomos
// Exporting classes .
//
// Revision 1.4  2002/05/03 14:29:28  jenny
// implemented OnMouseMove and SetSliceShadowPtr
//
// Revision 1.3  2002/05/02 15:46:03  jenny
// implemented onKeyPressed()
//
// Revision 1.2  2002/04/29 19:14:06  uday
// added OnMouseWheel()
//
// Revision 1.1  2002/04/24 16:35:58  uday
// Added initial versions of the slice shadow manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorSliceShadows.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxManipulatorSliceShadows.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
