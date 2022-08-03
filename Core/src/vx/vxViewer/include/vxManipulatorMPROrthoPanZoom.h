// $Id: vxManipulatorMPROrthoPanZoom.h,v 1.1 2004/04/26 21:25:07 vxconfig Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

#ifndef vxManipulatorMPROrthoPanZoom_h
#define vxManipulatorMPROrthoPanZoom_h


// includes
#include "vxManipulator.h"


// forward declarations
class vxEnvironment;


// class definitions
class VX_VIEWER_DLL vxManipulatorMPROrthoPanZoom : public vxManipulator
{
// functions
public:
  
  /// default constructor
  vxManipulatorMPROrthoPanZoom(vxEnvironment & environment);

  /// default destructor
  ~vxManipulatorMPROrthoPanZoom() {};
  
  /// event handler for receiving wheel events for the viewer
  virtual bool OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState);
  
  /// event handler for receiving mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);
  
  /// event handler for mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);
  
  /// event handler for receiving mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive keyboard down events
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

  /// called when the mouse cursor is to be set
  virtual bool OnSetCursor();

  /// resize the renderers area
  void ReCalculateLocalBox();

private:
    
  /// copy constructor
  vxManipulatorMPROrthoPanZoom(const vxManipulatorMPROrthoPanZoom & other);                   // should only be public if really implemented!
  
  /// assignment operator
  const vxManipulatorMPROrthoPanZoom & operator=(const vxManipulatorMPROrthoPanZoom & other); // should only be public if really implemented!

// data
private:

  /// panning enabled
  bool m_bPan;

}; // vxManipulatorMPROrthoPanZoom


#endif//vxManipulatorMPROrthoPanZoom_h


// Revision history:
// $Log: vxManipulatorMPROrthoPanZoom.h,v $
// Revision 1.1  2004/04/26 21:25:07  vxconfig
// added renamed classes
//
// Revision 1.2  2004/04/26 19:04:42  michael
// Adjusted code to reflect the changes in zomm factor, roi not being part of
// camera anymore, etc.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2004/01/20 20:13:01  frank
// added ability to reset the zoom of the image
//
// Revision 1.11  2003/07/11 19:49:15  michael
// final code reviews
//
// Revision 1.10  2003/05/16 13:21:34  frank
// formatting
//
// Revision 1.9  2003/04/29 21:17:36  michael
// removed vxManipulatorCamera from inheritance
//
// Revision 1.8  2002/11/25 13:44:56  geconomos
// New mouse cursor handling.
//
// Revision 1.7  2002/10/17 22:16:46  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.6  2002/08/20 22:37:20  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.5  2002/07/02 20:44:59  geconomos
// Exporting classes .
//
// Revision 1.4  2002/06/20 02:15:45  dmitry
// OnMouseEnter and OnMouseLeave implemented to setup the mouse cursor.
//
// Revision 1.3  2002/04/16 16:41:18  uday
// recalculate box instead of resize
//
// Revision 1.2  2002/04/10 17:48:26  uday
// PanZoom in windows with arbitrary aspect ratio.
//
// Revision 1.1  2002/03/26 21:14:49  uday
// Initial Version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorMPROrthoPanZoom.h,v 1.1 2004/04/26 21:25:07 vxconfig Exp $
// $Id: vxManipulatorMPROrthoPanZoom.h,v 1.1 2004/04/26 21:25:07 vxconfig Exp $
