// $Id: vxManipulator.h,v 1.2 2001/01/03 19:16:07 michael Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

/**
 * This class inherits all inputs and has interfaces to provide
 * the current mouse position and window size to perform certain
 * manipulations of data that is relative or absolut to the mouse
 * movement, click, etc.
 */

#ifndef vxManipulator_h
#define vxManipulator_h


// includes
#include "vxInput.h"
#include "Point2D.h"
#include "vxWinRect.h"
#include "vxUtils.h"


// forwards
class vxEnvironment;


// class definition
class VX_VIEWER_DLL vxManipulator :  public vxInput
{
// functions
public:
  
  /// default constructor
  vxManipulator(vxEnvironment & environment);
  
  /// destructor
  virtual ~vxManipulator() {};
  
  /// get mouse position
  const Point2D<int2> & GetMousePosition() const { return m_mousePosition; };
  
  /// get last selected mouse position
  const Point2D<int2> & GetLastMousePosition() const { return m_lastMousePosition; };

  /// set the environment
  void SetEnvironment(vxEnvironment & environment) { m_pEnvironment = &environment; };

  /// get the environment
  vxEnvironment & GetEnvironment() const { return *m_pEnvironment; };

protected:
  
  /// set last mouse position, done internally
  void SetMousePosition(const Point2D<int2> & position) { m_mousePosition = position; };
  
  /// set last mouse position, done internally
  void SetLastMousePosition(const Point2D<int2> & position) { m_lastMousePosition = position; };

  /// Get the multiplier for the shift/alt/control keys when manipulating a linear variable (e.g., slice number).
  static int4 GetKeyMultiplier( const uint4 uKeyboardState );

private:
  
  /// default constructor
  vxManipulator();  

  /// Copy constructor
  vxManipulator(const vxManipulator & other);                   // should only be public if really implemented!

  /// Assignment operator
  const vxManipulator & operator=(const vxManipulator & other); // should only be public if really implemented!

// data
private:

  /// environment
  vxEnvironment * m_pEnvironment;

  /// mouse position inside the window
  Point2D<int2> m_mousePosition;
  
  /// last mouse position inside the window
  Point2D<int2> m_lastMousePosition;

}; // vxManipulator


#endif // vxManipulator_h


// $Log: vxManipulator.h,v $
// Revision 1.2  2001/01/03 19:16:07  michael
// Moved function WarnShiftOblique out of the base class and into vxManipulatorUtils
// where it can be used by the (few) manipulators that need it.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.33  2004/01/16 21:22:43  frank
// Prevented the oblique-shift warning from flashing as you move the mouse
//
// Revision 1.32  2003/08/12 17:00:08  michael
// Took out the GetWorldCoord() from manipulator since it is part of viewer type
//
// Revision 1.31  2003/07/25 17:00:34  frank
// uint4 as a return type messes up some calculations.  int4 is better for GetKeyboardMultiplier()
//
// Revision 1.30  2003/05/15 20:06:20  frank
// Moving away from old point class
//
// Revision 1.29  2003/04/29 14:30:35  frank
// Moved projection functionality to the viewer type
//
// Revision 1.28  2003/04/25 12:35:21  michael
// more code review
//
// Revision 1.27  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.26  2003/04/08 14:44:56  frank
// Moved GetVesselLumenWorldCoordinates() into vxViewerType3D
//
// Revision 1.25  2003/03/21 15:40:39  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.24  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.23  2003/01/28 15:02:41  frank
// Const access functions should be const!
//
// Revision 1.22  2001/01/09 16:57:14  frank
// Please use const keyword for const functions!!!
//
// Revision 1.21  2001/01/09 12:58:31  frank
// Added ray profile data used for sampling along a ray for 3D picking.
//
// Revision 1.20  2002/12/17 14:38:56  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.19  2002/11/20 18:33:35  michael
// cosmetics
//
// Revision 1.18  2002/11/20 17:44:49  michael
// adjusted the GetWorldCoordinatesFromOpenGL() to be purely OpenGL
// based. GetWorldCoordinate() does the view dependent correction(s).
//
// Revision 1.17  2002/11/13 02:18:16  frank
// Moved unique component name function into vxUtils.
//
// Revision 1.16  2002/11/13 01:07:34  frank
// Added function to create a new component from a bool volume.
//
// Revision 1.15  2002/10/17 15:31:06  frank
// Centralized key multiplier function.
//
// Revision 1.14  2002/10/03 19:09:11  dmitry
// Parameter flag added to indicate if the rendering mode should be used to calculate the world coordinates.
//
// Revision 1.13  2002/09/20 16:09:01  michael
// changed name to GetWorldCoordinateOpenGL()
//
// Revision 1.12  2002/08/29 13:25:45  michael
// made default constructor private so that one can only use it with environment, as intended
//
// Revision 1.11  2002/08/20 22:28:18  uday
// Added GetWorldCoordinates2D(), which gives the point in that particular slice.
//
// Revision 1.10  2002/08/16 21:38:26  uday
// Renamed GetWorldCoordinates() so that all calls to it work in MIP and thin slab modes.
//
// Revision 1.9  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.8  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 1.7  2002/07/01 16:31:32  uday
// GetVoxelFromScreenPos() Update for 3D
//
// Revision 1.6  2002/06/27 18:07:30  uday
// Added
// 1> GetVoxelFromScreenPos()
// 2> ViewTypeEnum access functions.
//
// Revision 1.5.4.1  2002/07/01 19:58:22  uday
// Added GetVoxelFromSreenPosition().
//
// Revision 1.5  2002/03/29 22:34:00  jaddonisio
// Moved world coordinate function into class.
//
// Revision 1.4  2002/03/28 23:14:29  jaddonisio
// Inherits from Sec_75_vxManipulators and made destructor virtual.
//
// Revision 1.3  2002/03/27 14:24:19  michael
// moved  simple Get/Set into header file
//
// Revision 1.2  2002/03/22 19:24:02  manju
// Added WinRect and resize method. --- Dmitry
//
// Revision 1.1  2002/03/11 19:42:44  michael
// moved xvManipulators, vxInput, and vxEnvironment to 75_vxManipulators
//
// Revision 1.3  2002/03/07 22:40:44  dmitry
// Modified to fit new architecture.
//
// Revision 1.2  2002/02/28 15:02:59  michael
// made code (alomst) coding guidelines conform
//
// Revision 1.1  2002/02/22 15:28:58  dmitry
// Initial Versions Replacing same classes w/o vx prefix.
//
// Revision 1.5  2002/02/06 22:02:39  michael
// spacing
//
// Revision 1.4  2002/01/22 21:12:56  michael
// cleaned the manipulators ... no more protected member data
//
// Revision 1.3  2001/12/19 13:49:47  geconomos
// Changed signature of OnMouseMove, SetLastMousePos, SetMousePos from uint to int
// because on MouseDown the coordinates are outside the window and can become negative!
//
// Revision 1.2  2001/12/12 15:35:08  michael
// made parameters const
//
// Revision 1.1  2001/12/06 17:24:24  michael
// Added Inputs class and the first manipulator classes. One is an implementation
// class for manipulating a TransferFunction (presets and right mouse).
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulator.h,v 1.2 2001/01/03 19:16:07 michael Exp $
// $Id: vxManipulator.h,v 1.2 2001/01/03 19:16:07 michael Exp $
