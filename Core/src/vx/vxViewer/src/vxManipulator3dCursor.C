// $Id: vxManipulator3dCursor.C,v 1.8 2007/02/05 18:06:14 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/**
 * This is the class definition for vxManipulatorCursor3d. 
 * It is the manipulator class for slice correlation
 */


// includes
#include "stdafx.h"
#include "vxManipulator3dCursor.h"
#include "vxEnvironment.h"
#include "vxViewerTypes.h"
#include "vxRenderingMode.h"


// defines
#define FILE_REVISION "$Revision: 1.8 $"


/**
 * Constructor
 * @param environment the environment
 */
vxManipulatorCursor3d::vxManipulatorCursor3d(vxEnvironment & environment)
: vxManipulator(environment),
m_bDraggingLeftMouse(false)
//___________________________________________________________________
{
}


/**
 * Public method to process button down event
 * @param position mouse position
 * @param mouse button enum
 * @param state of keyboard
 * @return handled?
 */ 
bool vxManipulatorCursor3d::OnButtonDown(const Point2D<int2> & position,
                                         const ButtonEnum eButtonId, 
                                         const uint4 uKeyboardState)
//___________________________________________________________________
{
  bool bIfHandled(false);

  if (eButtonId == BUTTON_LEFT)
  {
    m_bDraggingLeftMouse = true;
    SetLastMousePosition(position);

    // sync from 2D right away; sync from 3D only on mouse up with no movement
    if (dynamic_cast< vxViewerType3D * >(GetEnvironment().GetViewerType()) == NULL)
    {
      GetEnvironment().GetCursor3d()->EnableCrossHair(true);
      GetEnvironment().GetCursor3d()->Modified(vxModInfo(vxCursor3d, vxUtils::CURSOR3D_MODIFIED));
      GetEnvironment().SetUserInteracting( true );
      SetCursor(position);
    }
  } // if left button

  return bIfHandled;
} // OnButtonDown()


/**
 * Public method to process button up event
 * @param position mouse position
 * @param mouse button enum
 * @param state of keyboard
 * @return handled?
 */ 
bool vxManipulatorCursor3d::OnButtonUp(const Point2D<int2> & position,
                                       const ButtonEnum eButtonId, 
                                       const uint4 uKeyboardState)
//___________________________________________________________________
{
  bool bIfHandled(false);

  try
  {
    if (eButtonId == BUTTON_LEFT)
    {
      // sync 3D cursor from 3D only if you didn't move at all
      if( dynamic_cast< vxViewerType3D * >( GetEnvironment().GetViewerType() ) )
      {
        if ( position == GetLastMousePosition() )
          SetCursor(position);
      }
      else
      {
        GetEnvironment().GetCursor3d()->EnableCrossHair(false);
        GetEnvironment().GetCursor3d()->Modified(vxModInfo(vxCursor3d, vxUtils::CURSOR3D_MODIFIED));
        GetEnvironment().SetUserInteracting( false );
      }
      m_bDraggingLeftMouse = false;

    } // if left button
  }
  catch( ... )
  {
    throw ex::InvalidDataException( LogRec("Unhandled exception on", "OnButtonUp", "vxManipulatorCursor3d") );
  }

  return bIfHandled;
} // OnButtonUp()


/**
 * Event handler if key is pressed
 * @param position
 * @param keyboard state
 * @return handled?
 */ 
bool vxManipulatorCursor3d::OnMouseMove(const Point2D<int2> & position, 
                                        const uint4 uKeyboardState)
//___________________________________________________________________
{
  bool bIfHandled(false);

  // sync from 3D only if you didn't move at all
  if ( m_bDraggingLeftMouse && dynamic_cast< vxViewerType3D * >( GetEnvironment().GetViewerType() ) == NULL )
  {
    SetCursor(position);
  }

  return bIfHandled;
} // OnMouseMove()


/**
 * SetCursor
 * @param position mouse position
 */ 
void vxManipulatorCursor3d::SetCursor(const Point2D<int2> & mousePosition)
//___________________________________________________________________
{
  // Check for null pointers:
  if (GetEnvironment().GetIntensityVolume() == NULL)
  {
    return;
  }

  // Check whether we are inside the valid window area
  if (mousePosition.X() <  GetEnvironment().GetWinRect().GetReferencePoint().first  || 
      mousePosition.X() >= GetEnvironment().GetWinRect().GetViewportSize().first    || 
      mousePosition.Y() <  GetEnvironment().GetWinRect().GetReferencePoint().second || 
      mousePosition.Y() >= GetEnvironment().GetWinRect().GetViewportSize().second)
  {
    return;
  }

  // get the world coordinates for the mouse position
  Point<float4> mouseWorldPos;
  if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(mousePosition, mouseWorldPos) == false)
  {
    return;
  }

  vxViewerType3D * pViewerType3D = dynamic_cast<vxViewerType3D *>(GetEnvironment().GetViewerType());
  if (pViewerType3D != NULL)
  {
    std::vector<float4> profile;
    if (pViewerType3D->GetVesselLumenWorldFromMouse(mousePosition, mouseWorldPos, profile) == false)
      return;
  }

  Point<float4> volWorldSize(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());

  if ( mouseWorldPos[0] >= 0.0 && mouseWorldPos[0] <= volWorldSize[0] && 
       mouseWorldPos[1] >= 0.0 && mouseWorldPos[1] <= volWorldSize[1] && 
       mouseWorldPos[2] >= 0.0 && mouseWorldPos[2] <= volWorldSize[2] )
  {
    vxCursor3d * pCursor3d(GetEnvironment().GetCursor3d());
    pCursor3d->SetPosition(mouseWorldPos);

    const bool b3dView(dynamic_cast<vxViewerType3D *>(GetEnvironment().GetViewerType()) != NULL);
    const bool bOrthoView(dynamic_cast<vxViewerTypeMPROrtho *>(GetEnvironment().GetViewerType()) != NULL);

    // Obtain the slice distance, taking into account the slice offset
    uint1 uOrientation(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());
    uint4 uNumberOfSlices( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim()[ uOrientation ] );
    float4 fInterSliceDistance( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits()[ uOrientation ] );
    uint4 uSliceIndex( static_cast< uint2 >( 0.5F + GetEnvironment().GetPlaneOrtho()->GetDistance() / fInterSliceDistance ) );
    uint4 uCurrentSlice( vxUtils::CalculateOffsetDistance( uSliceIndex, uNumberOfSlices, GetEnvironment().GetOffset(), GetEnvironment().GetMaximumOffset() ) );
    float4 fDistanceWithOffset( uCurrentSlice * fInterSliceDistance );

    // HACK, this actually should go into V3D Environment when we set the plane location based
    // on the cursor 3D location. There the check for update/precision should be done.
    if ( bOrthoView )
    {
      Point<float4> position(pCursor3d->GetPosition());
      position[uOrientation] = fDistanceWithOffset;
      pCursor3d->SetPosition(position);
    }

    // send modified event because we changed the 3D location
    GetEnvironment().GetCursor3d()->Modified(vxModInfo(vxCursor3d, vxUtils::CURSOR3D_MODIFIED));
  }
  return;
} // SetCursor()


// undefines
#undef FILE_REVISION


// $Log: vxManipulator3dCursor.C,v $
// Revision 1.8  2007/02/05 18:06:14  geconomos
// corrected setting of "interacting flag"
//
// Revision 1.7  2006/07/07 13:34:37  geconomos
// set "UserInteracting" flag in associated environment as needed
//
// Revision 1.6  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.5  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.4.2.3  2005/08/04 18:57:09  frank
// removed unused cursor 3D tracking
//
// Revision 1.4.2.2  2005/08/04 18:30:56  frank
// Issue #4330: Moved cursor tracking from the 3D renderer to a controller
//
// Revision 1.4.2.1  2005/07/19 11:49:31  frank
// missing include
//
// Revision 1.4  2005/05/20 19:09:39  frank
// fixed 3D cursor on 3x3 view, fixed active environment and oblique 'self-shadowing' problem
//
// Revision 1.3  2005/03/21 20:48:43  michael
// Added code to trigger the display of the cross hair
//
// Revision 1.2  2004/04/06 16:24:32  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.30  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.29  2003/12/24 15:37:32  dongqing
// add try-catch for OnButtonUp. This is for tracking dangling
// pointer bug
//
// Revision 1.28  2003/11/11 02:52:07  michael
// simplified logic ...
//
// Revision 1.27  2003/09/29 14:50:47  michael
// cosmetics
//
// Revision 1.26  2003/08/27 14:35:42  michael
// reworked the rendering mode by sorting flags into groups
//
// Revision 1.25  2003/06/19 20:21:21  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.24  2003/05/16 13:21:33  frank
// formatting
//
// Revision 1.23  2003/05/07 11:44:01  michael
// initial code review
//
// Revision 1.22  2003/05/01 13:19:58  michael
// replaced modified parameter from Triple<> to vxCursor3d
//
// Revision 1.21  2003/04/29 14:13:12  frank
// Moved projection functionality to the viewer type
//
// Revision 1.20  2003/04/25 12:35:21  michael
// more code review
//
// Revision 1.19  2003/04/24 14:03:02  michael
// accounted for new Cursor3d handline (real class, not just triple)
//
// Revision 1.18  2003/04/16 14:42:02  michael
// removed redundant include
//
// Revision 1.17  2003/04/08 14:45:21  frank
// Moved GetVesselLumenWorldCoordinates() into vxViewerType3D
//
// Revision 1.16  2003/03/21 16:19:21  geconomos
// Fixed bugs caused by removing viewer type enum.
//
// Revision 1.15  2003/03/21 15:40:39  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.14  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.13  2003/02/14 19:23:36  frank
// Only changed rendering mode if necessary
//
// Revision 1.12  2003/01/22 21:54:41  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.11  2001/01/09 12:59:12  frank
// Cleaned up picking in 3D
//
// Revision 1.10  2002/12/17 14:38:56  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.9  2002/11/07 00:07:08  ingmar
// code cleanup and handling of PlaneDistance as wold space distance
//
// Revision 1.8  2002/11/05 22:50:34  ingmar
// 3DCursor -> Cursor3d
//
// Revision 1.7  2002/08/20 22:30:22  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.6  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.5  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.4.2.1  2002/07/11 22:37:05  michael
// took out the MakeCurrent() since this is ensured by V3D_Viewer and vxViewer
//
// Revision 1.4  2002/06/26 18:23:14  dmitry
// Making sure that the relative coordinate of this plane is not calculated with the offset.
//
// Revision 1.3  2002/06/20 02:19:41  dmitry
// GetCursor3d method removed from Environment, Ptr used instead.
//
// Revision 1.2  2002/06/14 19:59:19  michael
// check for action to be within window
//
// Revision 1.1  2002/06/13 19:29:55  michael
// added Manipulator Cursor3d
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulator3dCursor.C,v 1.8 2007/02/05 18:06:14 geconomos Exp $
// $Id: vxManipulator3dCursor.C,v 1.8 2007/02/05 18:06:14 geconomos Exp $
