// $Id: vxManipulatorPlaneOblique.h,v 1.5 2007/01/31 15:41:36 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)

/*
  This class implements a manipulator of a plane oblique and then camera
*/


// pragmas
#pragma once


// includes
#include "vxManipulator.h"
#include "Normal.h"


// forward declarations
template <class T> class vxShareableObject;
class vxUndoActionPlaneOblique;


// class definition
class VX_VIEWER_DLL vxManipulatorPlaneOblique : public vxManipulator
{
// functions
public:
  
  /// constructor
  vxManipulatorPlaneOblique(vxEnvironment & environment);
  
  /// destructor
  ~vxManipulatorPlaneOblique() {};
  
  /// event handler can be reimplemented in a subclass to receive wheel events for the viewer
  virtual bool OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);
  
  /// event handler can be reimplemented in a subclass to receive keyboard events for the viewer
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

// static helper functions
public:

  /// zoom on or out of the current view
  static void ZoomView( vxEnvironment & environment, const float4 iZoomFactor );

private:
  
  /// copy constructor
  vxManipulatorPlaneOblique(const vxManipulatorPlaneOblique & other);                   // should only be public if really implemented!
  
  /// assignment operator
  const vxManipulatorPlaneOblique & operator=(const vxManipulatorPlaneOblique & other); // should only be public if really implemented!
  
  /// rotate plane and camera
  void Rotate(Normal<float4> vector, float4 fRadians);

  /// checks for extreme cases of the plane oblique
  void CheckExtremeCases();

  /// move plane by the given Delta
  void Move(int2 iDelta, uint4 uKeyboardState);
  
  /// set the dragging mode for the distance.
  void SetDragging(bool bDragging) { m_bIsDraggingDistance = bDragging; };
  
  /// returns the dragging mode for the distance.
  bool IsDragging() { return m_bIsDraggingDistance; };
  
  /// returns the quadrant the current mouse position is in
  uint1 GetQuadrant();

  /// set a restore point
  void SetRestorePoint();

// data
private:

  /// flag to indicate whether the distance is in dragging mode or not.
  bool m_bIsDraggingDistance;
  
  /// are we rotating
  bool m_bRotate;

  /// are we translating
  bool m_bTranslate;

  /// swing direction
  bool m_bSwingForward;

  /// the undo information
  //vxUndoActionPlaneOblique * m_pUndo;

}; // class vxManipulatorPlaneOblique


// $Log: vxManipulatorPlaneOblique.h,v $
// Revision 1.5  2007/01/31 15:41:36  romy
// commented out UndoActions for Mouse Scroll operations.
//
// Revision 1.4  2007/01/16 17:04:06  gdavidson
// Removed OnTimer
//
// Revision 1.3  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.2  2006/02/24 16:57:00  frank
// added undo information to oblique plane manipulation
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.15  2003/11/06 02:58:04  michael
// Fixed display of slice number and number of total slices for the oblique MPR,
// added text to be displayed if after rotation the slice is outside the volume,
// and added mechanism so that axis aligned obliques can reach the very first
// and very last slice/facs of volume (numerical precision problem).
//
// Revision 1.14  2003/09/25 03:06:49  frank
// Merged oblique views into one
//
// Revision 1.14  2003/09/11 20:37:56  frank
// Updated the slice shadow data after modifying it
//
// Revision 1.13  2003/09/04 17:29:44  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.12  2003/08/20 14:20:27  michael
// renamed degrees to radians
//
// Revision 1.11  2003/08/13 19:25:31  michael
// removed redundant forward declaration
//
// Revision 1.10.2.1  2003/08/21 19:22:16  michael
// added "mailto:"
//
// Revision 1.10  2003/07/21 18:38:29  frank
// Issue #3351: Fixed zooming in/out
//
// Revision 1.9  2003/07/01 13:44:39  frank
// Issue #3351: Fixed so that you can zoom out again after zooming in
//
// Revision 1.8  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.7  2003/05/07 13:17:31  michael
// initial code review
//
// Revision 1.6  2003/03/25 21:11:26  michael
// fixed issue #2863
//
// Revision 1.5  2003/01/22 21:57:37  ingmar
// added missing includes
//
// Revision 1.4  2002/11/20 19:51:51  michael
// added OnTimer method
//
// Revision 1.3  2002/11/20 17:45:23  michael
// enabled plane dragging
//
// Revision 1.2  2002/11/19 22:17:42  michael
// added arrow keys to plane oblique manipulation
//
// Revision 1.1  2002/11/19 21:21:24  michael
// Initial checkin of the oblique manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorPlaneOblique.h,v 1.5 2007/01/31 15:41:36 romy Exp $
// $Id: vxManipulatorPlaneOblique.h,v 1.5 2007/01/31 15:41:36 romy Exp $
