// $Id: vxManipulator3dCursor.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/*
  This is the class definition for vxManipulatorCursor3d. 
  It is the manipulator class for slice correlation
*/

#ifndef vxManipulatorCursor3d_h
#define vxManipulatorCursor3d_h


// includes
#include "vxManipulator.h"


// class definition
class VX_VIEWER_DLL vxManipulatorCursor3d : public vxManipulator
{
// functions
public:

  /// constructor
  vxManipulatorCursor3d(vxEnvironment & environment);

  /// default destructor
  virtual ~vxManipulatorCursor3d() {};

  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

private:

  /// Copy constructor
  vxManipulatorCursor3d(vxManipulatorCursor3d & other); // should only be public if really implemented!

  /// Assignment operator
  const vxManipulatorCursor3d & operator=(vxManipulatorCursor3d & other); // should only be public if really implemented!

  /// set the cursor to current position
  void SetCursor(const Point2D<int2> & position);

  /// is left mouse down and dragging
  bool m_bDraggingLeftMouse;
}; // vxManipulatorCursor3d


#endif // vxManipulatorCursor3d_h


// $Log: vxManipulator3dCursor.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/05/16 13:21:33  frank
// formatting
//
// Revision 1.8  2003/05/07 11:44:01  michael
// initial code review
//
// Revision 1.7  2003/04/25 12:35:20  michael
// more code review
//
// Revision 1.6  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.5  2002/11/05 22:50:34  ingmar
// 3DCursor -> Cursor3d
//
// Revision 1.4  2002/10/17 22:16:46  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.3  2002/08/20 22:30:22  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.2  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 1.1  2002/06/13 19:29:55  michael
// added Manipulator Cursor3d
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulator3dCursor.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxManipulator3dCursor.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $