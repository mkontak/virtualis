// $Id: vxManipulatorPlaneOrtho.h,v 1.3 2007/01/16 17:04:06 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Uday Chebrolu (uday@viatronix.com)

/**
 *  manipulator for ortho plane
 */

#ifndef vxManipulatorPlaneOrtho_h
#define vxManipulatorPlaneOrtho_h


// includes
#include "vxManipulator.h"


// class definition
class VX_VIEWER_DLL vxManipulatorPlaneOrtho : public vxManipulator
{
// functions
public:

  // constructor
  vxManipulatorPlaneOrtho(vxEnvironment & environment);

  // destructor
  ~vxManipulatorPlaneOrtho() {};

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
  vxManipulatorPlaneOrtho(const vxManipulatorPlaneOrtho &);                   // should only be public if really implemented!
  
  /// assignment operator
  const vxManipulatorPlaneOrtho & operator=(const vxManipulatorPlaneOrtho &); // should only be public if really implemented!
  
  /// move plane by the given Delta
  void Move(int2 iDelta, uint4 uKeyboardState);

  /// return whether the slices are being dragged
  bool GetDraggingPlane() { return m_bDraggingPlane; };
  
  /// set the current dragging status
  void SetDraggingPlane(bool bFlag);

  /// Pan the camera along the plane
  void PanCamera( const Point2D<int2> & mousePosition );


// data
private:

  /// dragging slices
  bool m_bDraggingPlane;

  /// swing direction
  bool m_bSwingForward;

  /// if panning
  bool m_bPan;

}; // vxManipulatorPlaneOrtho


#endif // vxManipulatorPlaneOrtho_h


// Revision History:
// $Log: vxManipulatorPlaneOrtho.h,v $
// Revision 1.3  2007/01/16 17:04:06  gdavidson
// Removed OnTimer
//
// Revision 1.2  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.20  2003/09/09 13:35:01  frank
// fixed page up/down and home/end delay problem
//
// Revision 1.19  2003/08/13 19:25:31  michael
// removed redundant forward declaration
//
// Revision 1.18  2003/07/09 14:18:18  frank
// Simplifying zoom/pan
//
// Revision 1.17  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.16  2003/05/16 13:21:34  frank
// formatting
//
// Revision 1.15  2003/05/07 21:00:21  dongqing
// code walkthrough
//
// Revision 1.14  2003/05/07 13:17:31  michael
// initial code review
//
// Revision 1.13  2002/11/20 19:52:16  michael
// fixed swing and loop of plane ortho
//
// Revision 1.12  2002/10/17 22:16:46  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.11  2002/09/10 20:37:55  michael
// Fixed "foreward" --> "forward"
//
// Revision 1.10  2002/08/20 22:39:19  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.9  2002/08/05 16:20:36  dmitry
// Event name added to OnTimer.
//
// Revision 1.8  2002/07/02 20:44:59  geconomos
// Exporting classes .
//
// Revision 1.7  2002/07/01 15:48:11  michael
// took out stuff that did not belong there
//
// Revision 1.6  2002/06/10 18:47:01  uday
// Added ComputeLaplacianWeightedHistogram().
// Will be here Temporarily.
//
// Revision 1.5  2002/06/06 18:01:08  dmitry
// SWING functionality added.
//
// Revision 1.4  2002/05/06 21:46:19  dmitry
// Cine implemented.
//
// Revision 1.3  2002/03/27 17:18:44  michael
// changed uint in various manipulators to use int, otherwise overloading doesn't work
//
// Revision 1.2  2002/03/11 23:46:23  michael
// adjusted PlaneOrtho/Oblique/CurvedManipulator correspondingly
//
// Revision 1.1  2002/03/11 19:42:43  michael
// moved xvManipulators, vxInput, and vxEnvironment to 75_vxManipulators
//
// Revision 1.6  2002/03/07 22:37:34  dmitry
// Modified to fit new architecture.
//
// Revision 1.5  2002/03/04 16:43:27  michael
// fomatting and coding guidelines
//
// Revision 1.4  2002/03/01 20:34:04  michael
// minor changes
//
// Revision 1.3  2002/02/26 14:42:43  michael
// adjusted manipulators to perform same tests/operations
//
// Revision 1.2  2002/02/15 15:54:44  michael
// clean-up, all member data is private, dragging slice is not anywmore in Manipulator
// base class, and PlaneOrtho manipulator got simplified.
//
// Revision 1.1  2002/02/14 18:15:40  uday
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorPlaneOrtho.h,v 1.3 2007/01/16 17:04:06 gdavidson Exp $
// $Id: vxManipulatorPlaneOrtho.h,v 1.3 2007/01/16 17:04:06 gdavidson Exp $
