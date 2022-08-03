// $Id: vxManipulatorPlaneOrtho.C,v 1.14 2007/02/05 15:29:25 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Uday Chebrolu (uday@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorPlaneOrtho.h"
#include "vxEnvironment.h"
#include "vxPlaneOrtho.h"
#include "vxUndoActionPlane.h"

// defines
#define FILE_REVISION "$Revision: 1.14 $"


/**
 * default constructor
 * @param environment
 */
vxManipulatorPlaneOrtho::vxManipulatorPlaneOrtho(vxEnvironment & environment):
vxManipulator(environment),
m_bDraggingPlane(false),
m_bSwingForward(true),
m_bPan( false )
{  
} // constructor


/**
 * Event handler if key is pressed
 * @param uKeyId
 * @param uKeyboardState
 * @return handled?
 */ 
bool vxManipulatorPlaneOrtho::OnKeyPressed(const uint1 uKeyId, 
                                           const uint4 uKeyboardState)
{

  bool bIfHandled( false );

  vxPlaneOrtho<float4> * pPlane( GetEnvironment().GetPlaneOrtho() );
  Triple<int4> dim( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim() );
  Triple<float4> units( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
  if ( pPlane != NULL )
  {
    bool fUpdate( false );
    switch ( uKeyId )
    {

    case vxInput::KEY_PAGE_UP:
      {
        vxEnvironment::GetUndoActions().PushAction( new vxUndoActionPlaneOrtho( GetEnvironment().GetPlaneOrtho() ) );
        int4 iDelta( GetEnvironment().GetScrollFactor() );
        Move( iDelta, uKeyboardState );
      }
      break;

    case vxInput::KEY_PAGE_DOWN:
      {
        vxEnvironment::GetUndoActions().PushAction( new vxUndoActionPlaneOrtho( GetEnvironment().GetPlaneOrtho() ) );
        int4 iDelta( -1 * GetEnvironment().GetScrollFactor() );
        Move( iDelta, uKeyboardState );
      }
      break;

    case vxInput::KEY_HOME:
      {
        vxEnvironment::GetUndoActions().PushAction( new vxUndoActionPlaneOrtho( GetEnvironment().GetPlaneOrtho() ) );
        pPlane->SetDistance( 0 );
        fUpdate = true;
        break;
      } 

    case vxInput::KEY_END:
      {
        vxEnvironment::GetUndoActions().PushAction( new vxUndoActionPlaneOrtho( GetEnvironment().GetPlaneOrtho() ) );
        int4 iSliceIndex( dim[ pPlane->GetNormal().GetMaxAbsComponentCoord() ] - 1 - GetEnvironment().GetMaximumOffset() );
        if ( iSliceIndex < 0 ) iSliceIndex = 0;
        pPlane->SetDistance( iSliceIndex * units[ pPlane->GetNormal().GetMaxAbsComponentCoord() ] );
        fUpdate = true;
        break;
      }

    default:
      break;
    }
  
    if (fUpdate == true)
    {
      GetEnvironment().GetPlaneOrtho()->Modified( vxModInfo( vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED ) );
    }
  } // plane exists?

  return bIfHandled;
} // OnKeyPressed




/**
 * Event handler if key is pressed
 * @param position
 * @param button id
 * @param keyboard state
 * @return handled?
 */ 
bool vxManipulatorPlaneOrtho::OnButtonDown(const Point2D<int2> & position, 
                                           const ButtonEnum eButtonId, 
                                           const uint4 uKeyboardState)
{
  bool bIfHandled(false);

  if (eButtonId == BUTTON_MIDDLE)
  {
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionPlaneOrtho( GetEnvironment().GetPlaneOrtho() ) );
    SetDraggingPlane(true);
    SetLastMousePosition(position);
  }
  else if ( false && ( eButtonId == BUTTON_LEFT ) && ( uKeyboardState & STATE_SHIFT ) )
  {
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionPlaneOrtho( GetEnvironment().GetPlaneOrtho() ) );
    m_bPan = true;
    bIfHandled = true;
    SetLastMousePosition(position);
  }

  if( m_bDraggingPlane || m_bPan )
    GetEnvironment().SetUserInteracting( true );

  return bIfHandled;
} // OnButtonDown()


/**
 * Event handler if key is pressed
 * @param position
 * @param buttton id
 * @param keyboard state
 * @return handled?
 */ 
bool vxManipulatorPlaneOrtho::OnButtonUp(const Point2D<int2> & position, 
                                         const ButtonEnum eButtonId,
                                         const uint4 uKeyboardState)
{
  bool bIfHandled(false);

  try
  {
    if( m_bPan || m_bDraggingPlane )
      GetEnvironment().SetUserInteracting( false );
    if (eButtonId == BUTTON_MIDDLE)
    {
      SetDraggingPlane(false);
    }

    m_bPan = false;
  }
  catch( ... )
  {
    throw ex::InvalidDataException( LogRec("Unhandle exception on", "OnButtonUp", "vxManipulatorPlaneOrtho") );
  }

  return bIfHandled;
} // OnButtonUp()


/**
 * Event handler if mouse is moved
 * @param position
 * @param uKeyboardState
 * @return handled?
 */ 
bool vxManipulatorPlaneOrtho::OnMouseMove(const Point2D<int2> & position, 
                                          const uint4 uKeyboardState)
{
  bool bIfHandled(false);

  // are we dragging the middle mouse to change the slice number?
  if (GetDraggingPlane() == true)
  {
    // get difference between previous and this mouse positions
    const int4 iDelta(GetLastMousePosition().m_y - position.m_y);
    SetLastMousePosition(position);
    Move(iDelta, uKeyboardState);
  }
  else if ( m_bPan == true )
  {
    PanCamera( position );
    bIfHandled = true;
  }

  return bIfHandled;
} // OnMouseMove()


/**
 * Pan the camera along the plane
 * @param mousePosition the current mouse position
 */
void vxManipulatorPlaneOrtho::PanCamera(const Point2D<int2> & mousePosition)
{
  // get the old position
  Point3Df worldPositionOld;
  if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(GetLastMousePosition(), worldPositionOld, false ) == false)
    return;

  // get the new position
  Point3Df worldPositionNew;
  if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(mousePosition, worldPositionNew, false ) == false)
    return;

  // translate the bounding box to move the camera - why does it have to be so indirect?
  const Vector3Df translation(worldPositionOld - worldPositionNew);
  GetEnvironment().GetCamera()->TranslateEyePoint( translation );
  GetEnvironment().GetCamera()->TranslateReferencePoint( translation );

  SetLastMousePosition( mousePosition );
} // PanCamera


/**
 * Event handler if mouse wheel is scrolled
 * @param position
 * @param delta of mouse wheel
 * @param keyboard state
 * @return handled?
 */
bool vxManipulatorPlaneOrtho::OnMouseWheel(const Point2D<int2> & mousePosition, 
                                           const int2 iDelta, 
                                           const uint4 uKeyboardState)
{
  bool bIfHandled(false);

  // increase or decrease the slice number based on mouse moving up or down
  Move((iDelta > 0) ? 1 : -1, uKeyboardState);

  return bIfHandled;
} // OnMouseWheel()


/**
 * Sets the new dragging status
 * @param bValue
 */ 
void vxManipulatorPlaneOrtho::SetDraggingPlane(bool bValue)
{

  m_bDraggingPlane = bValue;
} // SetDraggingPlane()


/**
 * Move the plane by the given value
 * @param iDelta
 * @param uKeyboardState
 */
void vxManipulatorPlaneOrtho::Move(int2 iDelta, 
                                   const uint4 uKeyboardState)
{
  vxPlaneOrtho<float4> * pPlane = dynamic_cast<vxPlaneOrtho<float4> *>(GetEnvironment().GetPlaneOrtho());
  if (pPlane != NULL)
  {
    // go faster with special keys
    iDelta *= GetKeyMultiplier(uKeyboardState);

    // determine new slice index and bound it to valid interval
    Normal<float4> normal(pPlane->GetNormal());
    const uint1 uAxis((uint1)normal.GetMaxAbsComponentCoord());
    Triple<int4> dim(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVolDim());
    Triple<float4> units(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVoxelUnits());

    const int iDistance(0.5 + pPlane->GetDistance()/units[uAxis]);
    const int4 iNewDistance(Bound(0, (int4)(iDistance + iDelta), (int4)(dim[uAxis]-1-GetEnvironment().GetMaximumOffset())));
    const float4 fNewDistance(iNewDistance*units[uAxis]);
    if (fabs(fNewDistance - pPlane->GetDistance()) > 0.0001)
    {

      //
      // update the plane distance and store undo information
      //
      pPlane->SetDistance( fNewDistance );
      GetEnvironment().GetPlaneOrtho()->Modified(vxModInfo(vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED));

    }
  } // plane exists?

} // Move()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorPlaneOrtho.C,v $
// Revision 1.14  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.13  2007/01/16 17:04:06  gdavidson
// Removed OnTimer
//
// Revision 1.12  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.11  2006/08/03 14:01:35  geconomos
// removed inclusion of "vxRenderEngineData.h"
//
// Revision 1.10  2006/07/07 13:34:37  geconomos
// set "UserInteracting" flag in associated environment as needed
//
// Revision 1.9  2006/06/27 20:13:23  geconomos
// setting "user interacting" flag in environment accordingly
//
// Revision 1.8  2006/02/24 20:14:32  frank
// made the undo more granular
//
// Revision 1.7  2006/02/24 16:57:00  frank
// added undo information to oblique plane manipulation
//
// Revision 1.6  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.5  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.4.2.3  2005/08/23 17:29:17  frank
// Issue #4385: Cine looped in the wrong direction for axial
//
// Revision 1.4.2.2  2005/08/15 19:37:41  geconomos
// switched axial start and end distances
//
// Revision 1.4.2.1  2005/07/29 16:27:27  frank
// removed dead code
//
// Revision 1.4  2005/05/18 15:06:48  frank
// cleaning up slice shadows
//
// Revision 1.3  2004/04/26 19:00:16  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.2  2004/04/06 16:25:36  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.66  2004/03/02 01:10:39  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.65.2.1  2004/02/24 20:42:09  frank
// fixed axial cine
//
// Revision 1.65  2004/02/11 14:06:48  frank
// fixed restricted segmentation and slice computations
//
// Revision 1.64  2004/01/20 20:11:37  frank
// removed dead code
//
// Revision 1.63  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.62  2003/12/24 15:37:33  dongqing
// add try-catch for OnButtonUp. This is for tracking dangling
// pointer bug
//
// Revision 1.61  2003/12/16 19:06:04  frank
// Added additional key definitions
//
// Revision 1.60  2003/11/18 15:29:42  frank
// Fixed problem with slice shadows in 3D
//
// Revision 1.59  2003/11/04 15:49:26  michael
// cosmetics
//
// Revision 1.58  2003/09/10 15:58:50  frank
// fixed slice shadow delay on mouse wheel rotate
//
// Revision 1.57  2003/09/09 13:35:50  frank
// removed debug messages
//
// Revision 1.56  2003/09/09 13:35:01  frank
// fixed page up/down and home/end delay problem
//
// Revision 1.55  2003/07/09 14:18:17  frank
// Simplifying zoom/pan
//
// Revision 1.54  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.53  2003/06/18 15:19:56  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.52  2003/05/16 13:21:34  frank
// formatting
//
// Revision 1.51  2003/05/15 20:07:53  frank
// Moving away from old point class
//
// Revision 1.50  2003/05/12 18:18:27  frank
// Added slice shadows object
//
// Revision 1.49  2003/05/07 13:17:31  michael
// initial code review
//
// Revision 1.48  2003/04/29 21:17:01  michael
// cosmetics
//
// Revision 1.47  2003/04/11 13:27:40  frank
// Issue #3102: Fixed mouse wheel slice problem.
//
// Revision 1.46  2003/03/31 13:50:57  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.45  2003/03/26 03:31:15  michael
// formatting
//
// Revision 1.44  2003/03/19 19:34:07  frank
// Removed obsolete VIEW_CHANGING mode
//
// Revision 1.43  2003/03/12 15:19:02  frank
// Propertly interpreted scroll wheel messages
//
// Revision 1.42  2003/01/22 21:58:05  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.41  2001/01/09 12:59:29  frank
// spelling
//
// Revision 1.40  2002/11/28 00:11:14  michael
// Fixed rendering mode modification to prevent redundant updates
//
// Revision 1.39  2002/11/27 16:21:17  ingmar
// better variable reuse
//
// Revision 1.38  2002/11/20 19:52:16  michael
// fixed swing and loop of plane ortho
//
// Revision 1.37  2002/11/18 16:36:56  michael
// completed plane object renaming
//
// Revision 1.36  2002/11/12 16:44:55  michael
// took out debug log message
//
// Revision 1.35  2002/11/11 19:33:00  michael
// fixed "paging" when having multiple viewers using same plane with different offset
//
// Revision 1.34  2002/11/08 02:39:49  michael
// fixed issue with number of slices that appear on "home" and "end" key
//
// Revision 1.33  2002/11/07 18:10:59  ingmar
// adjusted slice advancing to policy that plane distance represents world coordinates
//
// Revision 1.32  2002/10/17 15:33:09  frank
// Centralized key multiplier functionality.
//
// Revision 1.31  2002/09/10 20:37:55  michael
// Fixed "foreward" --> "forward"
//
// Revision 1.30  2002/08/20 22:39:19  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.29  2002/08/05 16:20:36  dmitry
// Event name added to OnTimer.
//
// Revision 1.28  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.27  2002/07/01 15:48:11  michael
// took out stuff that did not belong there
//
// Revision 1.26.2.3  2002/07/10 14:24:22  michael
// threw out redundant debug code (is now in vxManipulatorDebug)
//
// Revision 1.26.2.2  2002/07/09 18:21:22  michael
// Changed display of slice number to show 1 at top (head of patient) and also
// adjusted vxManipulatorPlaneOrtho to have HOME is slice 1 and END is the
// last one.
//
// Revision 1.26.2.1  2002/07/02 13:47:13  michael
// added changing of rendering mode so that 3D can render every second
// slice if slice shadow is dragged. Also adjusted renderer to prevent flickering
// for odd index of slice shadow.
//
// Revision 1.26  2002/06/19 21:16:14  ingmar
// debug image in now made out of PixelRGB<uint1>
//
// Revision 1.25  2002/06/14 18:04:27  manju
// Moved the key press handler for unit test for annotations to
// vxManipulatorHandlesObjCreate.
//
// Revision 1.24  2002/06/14 15:50:45  manju
// Added a key handler for unit test for annotations.
//
// Revision 1.23  2002/06/10 18:47:01  uday
// Added ComputeLaplacianWeightedHistogram().
// Will be here Temporarily.
//
// Revision 1.22  2002/06/06 18:01:08  dmitry
// SWING functionality added.
//
// Revision 1.21  2002/06/04 16:11:45  dmitry
// slice scroll factor added
//
// Revision 1.20  2002/05/08 21:24:27  manju
// Changed GetMaxComponentCoord to GetAbsMaxComponentCoord
//
// Revision 1.19  2002/05/07 18:37:46  jenny
// switched home/end keys.
//
// Revision 1.18  2002/05/06 21:46:19  dmitry
// Cine implemented.
//
// Revision 1.17  2002/04/30 20:22:04  michael
// removed manipulation of render mode since this is not duty of this manipulator
//
// Revision 1.16  2002/04/24 14:58:52  jenny
// vxBlockVolume stuff
//
// Revision 1.15  2002/04/23 20:12:23  michael
// replaced Density by Intensity
//
// Revision 1.14  2002/04/23 15:33:47  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.13  2002/04/22 14:35:41  binli
// Toggled SlidingThinSlab mode with 'S' key.
//
// Revision 1.12  2002/04/22 14:34:56  binli
// Toggled SlidingThinSlab mode with 'T' key.
//
// Revision 1.11  2002/04/22 13:29:51  michael
// fixed wrong update on enabling mip mode. Not plane object but rendering mode
// object needs is modified and triggers the update
//
// Revision 1.10  2002/04/19 22:45:12  frank
// Toggled MIP mode with M key.
//
// Revision 1.9  2002/04/17 18:17:55  frank
// Cleaned up header & footer.
//
// Revision 1.8  2002/04/15 19:57:28  manju
// Initializing m_bDraggingPlane to false.
//
// Revision 1.7  2002/03/27 20:19:46  michael
// planeMPR is now included in the modifed/update mechanism
//
// Revision 1.6  2002/03/27 17:18:44  michael
// changed uint in various manipulators to use int, otherwise overloading doesn't work
//
// Revision 1.5  2002/03/24 16:36:06  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.4  2002/03/20 15:34:21  dmitry
// Call to vxEnvironment to get and set the plane changed.
//
// Revision 1.3  2002/03/12 00:46:16  michael
// SetModified() is only called if value is different than previous value
//
// Revision 1.2  2002/03/11 23:46:23  michael
// adjusted PlaneOrtho/Oblique/CurvedManipulator correspondingly
//
// Revision 1.1  2002/03/11 19:42:43  michael
// moved xvManipulators, vxInput, and vxEnvironment to 75_vxManipulators
//
// Revision 1.8  2002/03/07 22:31:08  dmitry
// Modified to fit new architecture.
//
// Revision 1.7  2002/03/04 20:33:23  michael
// adjusted all PlaneOrtho and PlaneOblique instantiations with the new
// templetized types ...
//
// Revision 1.6  2002/03/04 16:43:27  michael
// fomatting and coding guidelines
//
// Revision 1.5  2002/02/26 14:42:43  michael
// adjusted manipulators to perform same tests/operations
//
// Revision 1.4  2002/02/25 13:21:50  michael
// formatting
//
// Revision 1.3  2002/02/15 21:41:55  michael
// final minor clean-up
//
// Revision 1.1  2002/02/14 18:15:40  uday
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorPlaneOrtho.C,v 1.14 2007/02/05 15:29:25 geconomos Exp $
// $Id: vxManipulatorPlaneOrtho.C,v 1.14 2007/02/05 15:29:25 geconomos Exp $
