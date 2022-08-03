// $Id: vxManipulatorPlaneCurved.C,v 1.4 2006/01/18 20:39:26 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Manjushree Nulkar (manju@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorPlaneCurved.h"
#include "vxEnvironment.h"
#include "vxPlaneCurved.h"


// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorPlaneCurved::vxManipulatorPlaneCurved( vxEnvironment & environment )
:vxManipulator( environment )
{

  SetDragging( false );

} // vxManipulatorPlaneCurved()


/**
 * Event handler if key is pressed
 *
 * @param uKeyId         the idendifier of the key
 * @param uKeyboardState the keyboard state
 *
 * @return handled?
 */ 
bool vxManipulatorPlaneCurved::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{  

  vxPlaneCurved * pPlane = dynamic_cast< vxPlaneCurved * >( GetEnvironment().GetPlaneCurved() );
  if ( pPlane != NULL )
  {
    bool bUpdate( false );
    switch ( uKeyId )
    {
    case vxInput::KEY_PAGE_UP:
      Move( 1, uKeyboardState );
      break;

    case vxInput::KEY_PAGE_DOWN:
      Move( -1, uKeyboardState );
      break;

    case vxInput::KEY_HOME:
      if ( pPlane->GetAngle() != 0.0F )
      {
        pPlane->SetAngle( 0.0F );
        bUpdate = true;
      }
      break;

    case vxInput::KEY_END:
      if(pPlane->GetAngle() != 90.0F)
      {
        pPlane->SetAngle( 90.0F );
        bUpdate = true;
      }
      break;

    case vxInput::KEY_MINUS:
    case vxInput::KEY_MINUS_KEYPAD:
      pPlane->SetWidth( pPlane->GetWidth() * 1.1F * GetKeyMultiplier( uKeyboardState ) );
      bUpdate = true;
      break;

    case vxInput::KEY_EQUALS:
    case vxInput::KEY_PLUS_KEYPAD:
      if ( pPlane->GetWidth() > 1.0F )
      {
        pPlane->SetWidth( pPlane->GetWidth() / ( 1.1F * GetKeyMultiplier( uKeyboardState ) ) );
        bUpdate = true;
      }
      break;

    default:
      break;

    } //  end switch case.

    if ( bUpdate )
    {
      GetEnvironment().GetPlaneOrtho()->Modified( vxModInfo( vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED ) );
    }
  } // plane exists?

  return false;

} // OnKeyPressed()


/**
 * Event handler if mouse button is held down
 *
 * @param mouse position
 * @param mouse button that is held down
 * @param keyboard state
 *
 * @return handled?
 */ 
bool vxManipulatorPlaneCurved::OnButtonDown( const Point2D<int2> & position, const ButtonEnum eButtonId, 
                                             const uint4 uKeyboardState )
{

  if (eButtonId == BUTTON_MIDDLE)
  {
    SetDragging(true);
    SetLastMousePosition(position);
  }

  return false;

} // OnButtonDown()


/**
 * Event handler if mouse is moved
 *
 * @param const position mouse position
 * @param const uKeyboardState state of mouse button
 *
 * @return handled?
 */ 
bool vxManipulatorPlaneCurved::OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState )
{

  bool bHandled( false );

  // middle button is pressed and the mouse is being dragged.
  if ( GetDragging() == true )
  {
    uint4 uDiff( position.m_y - GetLastMousePosition().m_y );
    SetLastMousePosition( position );
    bHandled = Move( uDiff, uKeyboardState );
  }

  return bHandled;

} // OnMouseMove()


/**
 * Event handler if mouse button is released.
 *
 * @param const position mouse position.
 * @param const eButtonId the mouse button that is released.
 * @param const uKeyboardState state of mouse button.
 *
 * @return handled?
 */ 
bool vxManipulatorPlaneCurved::OnButtonUp( const Point2D<int2> & position, const ButtonEnum eButtonId,
                                           const uint4 uKeyboardState )
{

  if (GetDragging() == true && eButtonId == BUTTON_MIDDLE) SetDragging(false);

  return false;

} // OnButtonUp()


/**
 * Event handler if mouse is scrolled
 * @param const position mouse position
 * @param const iDelta amount by which the mosue is scrolled
 * @param const uKeyboardState state of mouse button
 * @return handled?
 */ 
bool vxManipulatorPlaneCurved::OnMouseWheel( const Point2D<int2> & position, const int2 iDelta, 
                                             const uint4 uKeyboardState )
{

  return Move( iDelta / vxInput::GetMouseWheelQuantum(), uKeyboardState );

} // OnMouseWheel()


/**
 * Move plane by the given Delta
 * @param amount by how much to move the plane
 * @param keyboard state
 * @return handled?
 */
bool vxManipulatorPlaneCurved::Move( int2 iDelta, uint4 uKeyboardState )
{

  bool bHandled( false );
  vxPlaneCurved * pPlane = dynamic_cast< vxPlaneCurved * >( GetEnvironment().GetPlaneCurved() );

  if ( pPlane != NULL )
  {
    // go faster with special keys
    iDelta *= GetKeyMultiplier( uKeyboardState );

    //get the angle from the plane.
    float4 fAngle( pPlane->GetAngle() );
    const float4 fNewAngle( fmod( fAngle + iDelta + 360.0F, 360.0F ) );
  
    // If it is a new angle, update the plane and render the slice image.
    if ( fNewAngle != pPlane->GetAngle() )
    {
      pPlane->SetAngle( fNewAngle );
      GetEnvironment().GetPlaneCurved()->Modified( vxModInfo( vxPlaneCurved, vxUtils::PLANE_MODIFIED ) );
      bHandled = true;
    }
  } // plane exists?

  return bHandled;

} // Move()


#undef FILE_REVISION


// $Log: vxManipulatorPlaneCurved.C,v $
// Revision 1.4  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.6.1  2005/02/08 15:34:51  frank
// added plus/minus key on standard keyboard in addition to numeric keypad
//
// Revision 1.2  2004/04/06 16:25:23  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.24  2003/12/16 19:05:21  frank
// Added additional key definitions
//
// Revision 1.23  2003/11/05 13:38:24  frank
// Changed end to rotate to 90 degrees instead of 180
//
// Revision 1.22  2003/10/29 20:54:20  frank
// Removed double rendering in curved MPR
//
// Revision 1.21  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.20  2003/05/16 13:21:34  frank
// formatting
//
// Revision 1.19  2003/05/07 13:17:30  michael
// initial code review
//
// Revision 1.18  2003/03/25 17:40:57  frank
// Defaulted dragging to off
//
// Revision 1.17  2003/03/12 15:19:32  frank
// Propertly interpreted zoom messages with multiplier keys
//
// Revision 1.16  2003/03/05 13:35:31  frank
// Removed debug
//
// Revision 1.15  2002/11/18 16:36:56  michael
// completed plane object renaming
//
// Revision 1.14  2002/11/11 20:06:20  frank
// Let you go "around the horn"
//
// Revision 1.13  2002/11/01 21:30:17  frank
// Flipped + and - keys
//
// Revision 1.12  2002/10/17 15:32:31  frank
// Centralized key multiplier functionality.
//
// Revision 1.11  2002/08/20 22:49:11  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.10  2002/08/20 14:58:27  wenli
// initial version - Vascular
//
// Revision 1.9  2002/06/21 19:05:29  frank
// Added width manipulation.
//
// Revision 1.8.2.4  2002/05/23 18:44:32  frank
// Adding GetWorldCoordinates.
//
// Revision 1.8.2.3  2002/05/17 23:19:02  frank
// Added width manipulation.
//
// Revision 1.8.2.2  2002/05/08 22:36:12  frank
// Made the curved mainpulator work.
//
// Revision 1.8.2.1  2002/05/08 19:38:01  frank
// Called modified upon change in slice.
//
// Revision 1.8  2002/05/01 00:01:55  michael
// Adjusted the Modify calls to use the Modified on the object instead of the one in the environment
//
// Revision 1.7  2002/04/17 19:24:14  dmitry
// Switched to the new event handling architecture.
//
// Revision 1.6  2002/04/17 18:17:55  frank
// Cleaned up header & footer.
//
// Revision 1.5  2002/03/27 17:18:44  michael
// changed uint in various manipulators to use int, otherwise overloading doesn't work
//
// Revision 1.4  2002/03/24 16:36:06  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.3  2002/03/20 15:34:21  dmitry
// Call to vxEnvironment to get and set the plane changed.
//
// Revision 1.2  2002/03/12 00:46:16  michael
// SetModified() is only called if value is different than previous value
//
// Revision 1.1  2002/03/11 23:45:46  michael
// added new vxManipulator for PlaneCurved
//
// Revision 1.5  2002/03/07 22:32:17  dmitry
// Modified to fit new architecture.
//
// Revision 1.4  2002/03/04 16:43:27  michael
// fomatting and coding guidelines
//
// Revision 1.3  2002/02/28 15:03:23  michael
// consitent look
//
// Revision 1.2  2002/02/26 14:42:43  michael
// adjusted manipulators to perform same tests/operations
//
// Revision 1.1  2002/02/19 22:26:35  manju
// Manipulator for curved plane.
//