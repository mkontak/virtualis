// $Id: vxManipulator.C,v 1.2 2001/01/03 19:16:07 michael Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulator.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Returns the mouse position
 * @param environment
 */ 
vxManipulator::vxManipulator(vxEnvironment & environment):
m_pEnvironment(&environment)
//*******************************************************************
{
  m_mousePosition     = Point2D<uint2>(0,0);
  m_lastMousePosition = Point2D<uint2>(0,0);
}


/**
 * Get the multiplier based on the shift/control keys when manipulating a linear variable (e.g., slice number)
 * @param uKeyboardState state of the keyboard
 * @return the multiplier
 */
int4 vxManipulator::GetKeyMultiplier(const uint4 uKeyboardState)
//*******************************************************************
{
  uint4 uMultiplier(1);

  // if CTRL is pressed, go four times faster
  if (uKeyboardState & STATE_CTRL)
    uMultiplier *= 4;

  // if SHIFT is pressed, go two times faster
  if (uKeyboardState & STATE_SHIFT)
    uMultiplier *= 2;

  return uMultiplier;
} // GetKeyMultiplier()


#undef FILE_REVISION


// Revision History:
// $Log: vxManipulator.C,v $
// Revision 1.2  2001/01/03 19:16:07  michael
// Moved function WarnShiftOblique out of the base class and into vxManipulatorUtils
// where it can be used by the (few) manipulators that need it.
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.67  2004/01/16 21:22:42  frank
// Prevented the oblique-shift warning from flashing as you move the mouse
//
// Revision 1.66  2003/08/12 17:00:08  michael
// Took out the GetWorldCoord() from manipulator since it is part of viewer type
//
// Revision 1.65  2003/07/25 17:00:34  frank
// uint4 as a return type messes up some calculations.  int4 is better for GetKeyboardMultiplier()
//
// Revision 1.64  2003/06/18 15:19:55  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.63  2003/05/15 20:06:20  frank
// Moving away from old point class
//
// Revision 1.62  2003/04/29 14:30:35  frank
// Moved projection functionality to the viewer type
//
// Revision 1.61  2003/04/25 12:35:21  michael
// more code review
//
// Revision 1.60  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.59  2003/04/08 14:44:56  frank
// Moved GetVesselLumenWorldCoordinates() into vxViewerType3D
//
// Revision 1.58  2003/03/21 15:40:39  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.57  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.56  2003/01/22 21:54:22  ingmar
// the RayIterator is now only using a RaySegment and a Vector4Df increment to determine how to traverse the ray
//
// Revision 1.55  2001/01/09 16:57:28  frank
// Please use const keyword for const functions!!!
//
// Revision 1.54  2001/01/09 12:58:48  frank
// Added ray profile data used for sampling along a ray for 3D picking.
//
// Revision 1.53  2002/12/24 17:36:22  frank
// Cleaned up picking in 3D
//
// Revision 1.52  2002/12/20 16:15:41  frank
// Added x-ray projection
//
// Revision 1.51  2002/12/20 15:56:33  frank
// Implemented picking in 3D.
//
// Revision 1.50  2002/12/19 21:28:53  ingmar
// HitBoundingBox now takes a Box and not 2 vectors as box argument
//
// Revision 1.49  2002/12/19 16:57:44  michael
// took out GetWorldCooridinate() for oblique view, using the viewer type instead
//
// Revision 1.48  2002/12/18 18:22:40  michael
// manipulator now uses the viewer type's GetWorldCoordinate()
//
// Revision 1.47  2002/12/17 14:38:56  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.46  2002/12/16 23:47:42  frank
// spelling
//
// Revision 1.45  2002/11/26 18:12:44  ingmar
// now using Vector4d<float4> in the rayIterator
//
// Revision 1.44  2002/11/22 13:14:26  michael
// fixed bu in ortho backprojection
//
// Revision 1.43  2002/11/20 21:41:08  michael
// removed warnings
//
// Revision 1.42  2002/11/20 17:44:49  michael
// adjusted the GetWorldCoordinatesFromOpenGL() to be purely OpenGL
// based. GetWorldCoordinate() does the view dependent correction(s).
//
// Revision 1.41  2002/11/20 15:23:39  ingmar
// removed compiler warning
//
// Revision 1.40  2002/11/19 21:24:42  michael
// added GetWorldCoord() for oblique view
//
// Revision 1.39  2002/11/18 16:36:56  michael
// completed plane object renaming
//
// Revision 1.38  2002/11/15 21:06:10  michael
// changed view types
//
// Revision 1.37  2002/11/13 02:18:16  frank
// Moved unique component name function into vxUtils.
//
// Revision 1.36  2002/11/13 01:07:34  frank
// Added function to create a new component from a bool volume.
//
// Revision 1.35  2002/11/12 00:11:54  michael
// accounted for rounding error when converting world plane distance to volume index coordinates
//
// Revision 1.34  2002/11/11 15:34:43  frank
// Fixed comment.
//
// Revision 1.33  2002/11/08 18:03:30  michael
// fixed world/index coordinates
//
// Revision 1.32  2002/11/08 16:04:20  ingmar
// adjusted slicei ndext o policy that plane distance represents world coordinates
//
// Revision 1.31  2002/11/07 00:07:29  ingmar
// code cleanup and handling of PlaneDistance as wold space distance
//
// Revision 1.30  2002/10/17 15:31:06  frank
// Centralized key multiplier function.
//
// Revision 1.29  2002/10/16 15:29:19  michael
// replace detection of minimum value by call to vxIntensityConverter::GetMin()
//
// Revision 1.28  2002/10/03 19:09:11  dmitry
// Parameter flag added to indicate if the rendering mode should be used to calculate the world coordinates.
//
// Revision 1.27  2002/09/23 17:36:52  geconomos
// Issue 2670 :: Large log file. Removed debug out from GetWorldCoordinatesFromOpenGL()
//
// Revision 1.26  2002/09/20 16:09:01  michael
// changed name to GetWorldCoordinateOpenGL()
//
// Revision 1.25  2002/09/19 19:01:56  michael
// fixed GetWorlCoordinate() for positions outside the volume ...
//
// Revision 1.24  2002/09/19 17:41:13  dmitry
// First step towards clipping annotations to volume boundaries.
//
// Revision 1.23  2002/08/16 21:38:26  uday
// Renamed GetWorldCoordinates() so that all calls to it work in MIP and thin slab modes.
//
// Revision 1.22  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.21  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.20  2002/07/02 19:32:12  soeren
// uncomented unnecessary debug messages
//
// Revision 1.19  2002/07/02 19:23:19  soeren
// changed to new camera
//
// Revision 1.18  2002/07/01 16:31:32  uday
// GetVoxelFromScreenPos() Update for 3D
//
// Revision 1.17  2002/07/01 15:25:07  michael
// adjusted reading from framebuffer
//
// Revision 1.16  2002/06/27 19:27:34  michael
// completed transition from CameraNew to vxCamera
//
// Revision 1.15  2002/06/27 18:07:30  uday
// Added
// 1> GetVoxelFromScreenPos()
// 2> ViewTypeEnum access functions.
//
// Revision 1.14.4.7  2002/07/11 22:37:05  michael
// took out the MakeCurrent() since this is ensured by V3D_Viewer and vxViewer
//
// Revision 1.14.4.6  2002/07/08 16:55:16  uday
// GetWorldCoordinates() now takes slice offsets into account.
//
// Revision 1.14.4.5  2002/07/03 17:57:54  uday
// In 2D, get the depth from slice distance and not from the depth buffer.
//
// Revision 1.14.4.4  2002/07/01 23:20:11  uday
// GetVoxelFromScreenPosistion() update.
//
// Revision 1.14.4.3  2002/07/01 19:58:22  uday
// Added GetVoxelFromSreenPosition().
//
// Revision 1.14.4.2  2002/07/01 18:03:34  michael
// fixed bug because only one value was set correctly
//
// Revision 1.14.4.1  2002/07/01 16:45:33  michael
// took out initialization of view since this is ensured to be correct
//
// Revision 1.14  2002/04/24 14:56:42  jenny
// vxBlockVolume stuff
//
// Revision 1.13  2002/04/23 20:12:23  michael
// replaced Density by Intensity
//
// Revision 1.12  2002/04/23 15:33:46  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.11  2002/04/17 18:16:41  frank
// Cleaned up header & footer.
//
// Revision 1.10  2002/04/14 00:18:00  michael
// adjusted method name
//
// Revision 1.9  2002/04/10 20:35:08  manju
// Using CalcOffsetDistance to get the current slice number.
//
// Revision 1.8  2002/04/10 14:22:20  jaddonisio
// Corrected units to be float4. Also, did some alignment of code.
//
// Revision 1.7  2002/04/08 21:21:35  manju
// Modified GetWorldCoordinate function to take into account Sagittal and
// Coronal views.
//
// Revision 1.6  2002/04/06 00:07:47  jaddonisio
// Made GL context part of 'GetMouseCoord'.
//
// Revision 1.5  2002/04/01 21:24:30  michael
// comment
//
// Revision 1.4  2002/03/29 22:34:00  jaddonisio
// Moved world coordinate function into class.
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
// Revision 1.3  2002/03/07 22:40:30  dmitry
// Modified to fit new architecture.
//
// Revision 1.2  2002/02/28 15:02:59  michael
// made code (alomst) coding guidelines conform
//
// Revision 1.1  2002/02/22 15:28:58  dmitry
// Initial Versions Replacing same classes w/o vx prefix.
//
// Revision 1.5  2002/01/29 20:09:59  michael
// comments, restructuring, and misc
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulator.C,v 1.2 2001/01/03 19:16:07 michael Exp $
// $Id: vxManipulator.C,v 1.2 2001/01/03 19:16:07 michael Exp $
