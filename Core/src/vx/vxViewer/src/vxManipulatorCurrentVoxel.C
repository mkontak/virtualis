// $Id: vxManipulatorCurrentVoxel.C,v 1.3 2007/10/31 14:35:57 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: jenny hizver (jenny@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorCurrentVoxel.h"
#include "vxEnvironment.h"
#include "vxBlockVolume.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorCurrentVoxel::vxManipulatorCurrentVoxel(vxEnvironment & environment):
vxManipulator(environment)
//*******************************************************************
{
} // constructor


/**
 * Event handler if key is pressed
 * @param position
 * @param keyboard state
 * @return handled?
 */ 
bool vxManipulatorCurrentVoxel::OnMouseMove(const Point2D<int2> & position, 
                                            const uint4 uKeyboardState)
//*******************************************************************
{
  SetCurrentVoxel(position);

  return false;
} // OnMouseMove()


/**
 * Event handler if mouse wheel is scrolled
 * @param position
 * @param delta
 * @param keyboard state
 */
bool vxManipulatorCurrentVoxel::OnMouseWheel(const Point2D<int2> & position, 
                                             const int2 iDelta, 
                                             const uint4 uKeyboardState)
//*******************************************************************
{
  SetCurrentVoxel(position);

  return false;
} // OnMouseWheel()


/**
 * event handler can be reimplemented in a subclass to receive mouse leave events
 */
void vxManipulatorCurrentVoxel::OnMouseLeave()
{
  GetEnvironment().GetCurrentVoxelPos()->X() = -1;
  GetEnvironment().GetCurrentVoxelPos()->Y() = -1;
  GetEnvironment().GetCurrentVoxelPos()->Z() = -1;

  GetEnvironment().GetCurrentVoxelPos()->Modified(vxModInfo(Vector3D<int4>, 0));
} // OnMouseLeave()


/**
 * Set the current voxel in the environment
 * @param position the mouse position
 */
void vxManipulatorCurrentVoxel::SetCurrentVoxel(const Point2D<int2> & mousePosition)
//*******************************************************************
{
  try
  {
    vxBlockVolume<uint2> * pVolume = static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume());

    if (pVolume != NULL)
    {
      Point<float4> mouseWorldCoord;
      if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(mousePosition, mouseWorldCoord, true) == false)
        return;

      Triple<float4> units(pVolume->GetHeader().GetVoxelUnits());
      Point<float4> mouseVolumeCoord(mouseWorldCoord[0] / units[0], 
                                     mouseWorldCoord[1] / units[1], 
                                     mouseWorldCoord[2] / units[2]);
      const Vector3D<int4> mouseVolumeIndexCoord(mouseVolumeCoord.m_x+0.5F,
                                                 mouseVolumeCoord.m_y+0.5F,
                                                 mouseVolumeCoord.m_z+0.5F);
      GetEnvironment().GetCurrentVoxelPos()->X() = mouseVolumeIndexCoord.X();
      GetEnvironment().GetCurrentVoxelPos()->Y() = mouseVolumeIndexCoord.Y();
      GetEnvironment().GetCurrentVoxelPos()->Z() = mouseVolumeIndexCoord.Z();

      GetEnvironment().GetCurrentVoxelPos()->Modified(vxModInfo(Vector3D<int4>, 0));
    }
  }
  catch ( ... )
  {
    LogDbg( "unhandled exception", "vxManipulatorCurrentVoxel", "SetCurrentVoxel" );
  }
} // SetCurrentVoxel()


#undef FILE_REVISION


// Revision History:
// $Log: vxManipulatorCurrentVoxel.C,v $
// Revision 1.3  2007/10/31 14:35:57  gdavidson
// Issue #5835: Set current voxel to (-1,-1,-1) when mouse leaves the viewer
//
// Revision 1.2  2004/04/06 16:25:11  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.21  2004/02/03 12:27:04  michael
// fixed typo
//
// Revision 1.20  2004/01/14 17:05:52  frank
// removed debug
//
// Revision 1.19  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.18  2003/09/10 20:12:01  dongqing
// Add try-catch on SetCurrentVoxel
//
// Revision 1.17  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.16  2003/06/18 15:19:55  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.15  2003/05/07 11:44:02  michael
// initial code review
//
// Revision 1.14  2003/03/24 19:35:39  frank
// Removed reliance upon vxManipulator::GetWorldCoordinates().  Used GetEnvironment()->GetViewerTypePtr()->GetWorldCoordinate() instead.
//
// Revision 1.13  2003/03/21 19:28:34  frank
// Cleaned up code.  Refactored where code was originally copied.
//
// Revision 1.12  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulator. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.11  2002/12/17 14:38:56  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.10  2002/11/20 17:45:59  michael
// added comment
//
// Revision 1.9  2002/08/20 22:32:06  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.8  2002/08/16 21:37:32  uday
// GetWorldCoordinates() update.
//
// Revision 1.7  2002/08/08 15:10:37  uday
// Added OnMouseWheel().
//
// Revision 1.6  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.5  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.4.4.1  2002/07/01 23:17:35  uday
// Replaced GetworldCoordinates() by GetVoxelFromScreenPosistion().
//
// Revision 1.4  2002/04/29 23:47:16  frank
// Formatting footer.
// 
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorCurrentVoxel.C,v 1.3 2007/10/31 14:35:57 gdavidson Exp $
// $Id: vxManipulatorCurrentVoxel.C,v 1.3 2007/10/31 14:35:57 gdavidson Exp $
