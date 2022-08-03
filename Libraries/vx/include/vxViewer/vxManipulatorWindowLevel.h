// $Id: vxManipulatorWindowLevel.h,v 1.2 2004/04/15 16:05:16 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

#ifndef vxManipulatorWindowLevel_h
#define vxManipulatorWindowLevel_h


// includes
#include "vxManipulator.h"
#include "ZeroOne.h"


/**
 * This class finally implements a manipulator of a window level. It uses the keys 0-9 for window
 * level presets and mouse move while right mouse button down to adjust the current window & level.
 */
class VX_VIEWER_DLL vxManipulatorWindowLevel : public vxManipulator
{
/// functions
public:

  /// constructor
  vxManipulatorWindowLevel(vxEnvironment & environment);

  /// destructor
  ~vxManipulatorWindowLevel() {}

  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

private:

  /// Copy constructor
  vxManipulatorWindowLevel(const vxManipulatorWindowLevel &);                     // should only be public if really implemented!

  /// Assignment operator
  const vxManipulatorWindowLevel & operator=(const vxManipulatorWindowLevel &); // should only be public if really implemented!

  /// Gets a multiplier based on the distance from the initial mouse button down position
  float4 GetDistanceMultiplier( const Point2D<int2> & position, const uint4 uDimension ) const;

// data
private:

  /// adjusting state toggle
  bool m_bAdjusting;

  /// the intial button down location
  Point2D<int2> m_buttonDownPos;

  /// the initial window when adjustment is initiated
  ZeroOne m_initialWindow;
  
  /// the initial level when adjustment is initiated
  ZeroOne m_initialLevel;

}; // vxManipulatorWindowLevel


#endif // vxManipulatorWindowLevel_h


// $Log: vxManipulatorWindowLevel.h,v $
// Revision 1.2  2004/04/15 16:05:16  frank
// split out the preset manipulation from the window/level manipulator
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2003/11/24 13:05:01  frank
// Exported class
//
// Revision 1.15  2003/11/07 13:07:43  frank
// Spacebar is already used for something else :(
//
// Revision 1.14  2003/11/06 16:26:45  frank
// Added the ability to flip through presets with the space bar.
//
// Revision 1.13  2003/10/31 17:12:14  frank
// Issue #3375: Inverted the level and gave the adjustment some tweaking so that you can get fine detail at first, then acceleration as you move away from the initial click point
//
// Revision 1.12  2003/05/16 13:21:35  frank
// formatting
//
// Revision 1.11  2003/05/07 15:01:31  dongqing
// code walkthrough
//
// Revision 1.10  2003/05/07 11:44:02  michael
// initial code review
//
// Revision 1.9  2002/11/25 13:44:57  geconomos
// New mouse cursor handling.
//
// Revision 1.8  2002/11/06 23:31:19  michael
// removed redundant code
//
// Revision 1.7  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.6  2002/10/03 17:37:48  michael
// Extended possible presets (Colormap and Camera)
//
// Revision 1.5  2002/10/01 13:04:02  michael
// Temporarily added presets in case there is no database ...
//
// Revision 1.4  2002/08/20 22:46:13  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.3  2002/07/02 20:44:59  geconomos
// Exporting classes .
//
// Revision 1.2  2002/06/04 14:21:22  dmitry
// Adjusting state toggle added.
//
// Revision 1.1  2002/03/11 19:42:43  michael
// moved xvManipulators, vxInput, and vxEnvironment to 75_vxManipulators
//
// Revision 1.3  2002/03/07 22:31:09  dmitry
// Modified to fit new architecture.
//
// Revision 1.2  2002/02/28 15:02:59  michael
// made code (alomst) coding guidelines conform
//
// Revision 1.1  2002/02/22 15:28:58  dmitry
// Initial Versions Replacing same classes w/o vx prefix.
//
// Revision 1.2  2001/12/19 13:50:08  geconomos
// Changed signature of OnMouseMove and added mouse cursor changing
//
// Revision 1.1  2001/12/06 17:24:24  michael
// Added Inputs class and the first manipulator classes. One is an implementation
// class for manipulating a TransferFunction (presets and right mouse).
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorWindowLevel.h,v 1.2 2004/04/15 16:05:16 frank Exp $
// $Id: vxManipulatorWindowLevel.h,v 1.2 2004/04/15 16:05:16 frank Exp $