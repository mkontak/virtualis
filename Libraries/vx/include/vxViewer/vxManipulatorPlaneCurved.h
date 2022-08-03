// $Id: vxManipulatorPlaneCurved.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Manjushree Nulkar (manju@viatronix.com)

/**
 * This class finally implements a manipulator of a 
 * Plane Curved.
 */


#ifndef vxManipulatorPlaneCurved_h
#define vxManipulatorPlaneCurved_h


// includes
#include "vxManipulator.h"


// forward declarations


// class definition
class VX_VIEWER_DLL vxManipulatorPlaneCurved : public vxManipulator
{
// functions
public:
  
  /// constructor
  vxManipulatorPlaneCurved(vxEnvironment & environment);
  
  /// destructor
  ~vxManipulatorPlaneCurved() {};
  
  /// event handler can be reimplemented in a subclass to receive wheel events for the viewer
  virtual bool OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive keyboard events for the viewer
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

private:
  
  /// copy constructor
  vxManipulatorPlaneCurved(const vxManipulatorPlaneCurved & other);                   // should only be public if really implemented!
  
  /// assignment operator
  const vxManipulatorPlaneCurved & operator=(const vxManipulatorPlaneCurved & other); // should only be public if really implemented!
  
  /// move the plane by the given amount
  bool Move(int2 iDelta, uint4 uKeyboardState4);
  
  /// set the dragging mode for the plane
  void SetDragging(bool bDragging) { m_bIsDragging = bDragging; }
  
  /// returns the dragging mode
  bool GetDragging() { return m_bIsDragging; }
  
// data
private:
  
  // flag to indicate whether the plane is in dragging mode or not.
  bool m_bIsDragging;
}; // vxManipulatorPlaneCurved


#endif // vxManipulatorPlaneCurved_h


// $Log: vxManipulatorPlaneCurved.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/05/16 11:45:25  michael
// code review
//
// Revision 1.9  2003/05/07 21:00:21  dongqing
// code walkthrough
//
// Revision 1.8  2003/05/07 13:17:30  michael
// initial code review
//
// Revision 1.7  2002/11/11 20:06:20  frank
// Let you go "around the horn"
//
// Revision 1.6  2002/10/17 22:16:46  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.5  2002/08/20 22:38:51  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.4  2002/07/02 20:44:59  geconomos
// Exporting classes .
//
// Revision 1.3  2002/06/21 19:05:29  frank
// Added width manipulation.
//
// Revision 1.2.2.1  2002/05/23 18:44:32  frank
// Adding GetWorldCoordinates.
//
// Revision 1.2  2002/03/27 17:18:44  michael
// changed uint in various manipulators to use int, otherwise overloading doesn't work
//
// Revision 1.1  2002/03/11 23:45:46  michael
// added new vxManipulator for PlaneCurved
//
// Revision 1.4  2002/03/07 22:34:14  dmitry
// Modified to fit new architecture.
//
// Revision 1.3  2002/03/04 16:43:27  michael
// fomatting and coding guidelines
//
// Revision 1.2  2002/02/26 14:42:43  michael
// adjusted manipulators to perform same tests/operations
//
// Revision 1.1  2002/02/19 22:26:35  manju
// Manipulator for curved plane.
//