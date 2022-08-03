// $Id: vxManipulatorCameraPath.C,v 1.7 2007/02/05 15:29:25 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (dachille@viatronix.com)


// Includes
#include "stdafx.h"
#include "vxElementFloat4.h"
#include "vxIDs.h"
#include "vxVessel.h"
#include "vxManipulatorCameraPath.h"
#include "vxEnvironment.h"
#include "vxCursor3D.h"
#include "vxUtils.h"

// Defines
#define FILE_REVISION "$Revision: 1.7 $"


/**
 * default constructor
 *
 * @param environment the environment
 */
vxManipulatorCameraPath::vxManipulatorCameraPath( vxEnvironment & environment )
: vxManipulatorCamera( environment ),
m_bRotating( false ),
m_bDragging( false )
{

  // only works if the camera is perspective
  GetEnvironment().GetCamera()->SetProjectionType( vxCamera::PERSPECTIVE );

  // initialize the fractional index along the centerline to zero
  vxShareableObject< vxElementFloat4 > * pIndexPosition( dynamic_cast< vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::PathIndexPosition ) ) );
  if ( pIndexPosition != NULL )
  {
    pIndexPosition->SetValue( 0 );
    pIndexPosition->Modified( vxModInfoID( vxElement, vxIDs::PathIndexPosition, 0 ) );
  }

} // Constructor()


/**
 * Event handler if key is pressed
 *
 * @param uKeyId         the key that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorCameraPath::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{

  bool bHandled( false );

  try
  {

    // get the vessel
    vxVessel * pVessel = dynamic_cast< vxVessel * >( GetEnvironment().GetElement( vxIDs::Path ) );
    if ( pVessel == NULL )
    {
      LogWrn( "Vessel array pointer is NULL", "vxManipulatorCameraPath", "OnKeyPressed" );
      return false;
    }
    if ( pVessel->size() == 0 ) return false;

    // get the fractional index along the centerline
    vxShareableObject< vxElementFloat4 > * pIndexPosition( dynamic_cast< vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::PathIndexPosition ) ) );
    if ( pIndexPosition == NULL )
    {
      LogWrn( "Path index position is null", "vxManipulatorCameraPath", "OnKeyPressed" );
      return false;
    }

    const float4 fDegreesRotateOnKeyPress( float4( 10 * M_PI / 180 ) );

    switch ( uKeyId )
    {
    case vxInput::KEY_PAGE_UP:
      TranslateSection( GetKeyMultiplier( uKeyboardState ) );
      bHandled = true;
      break;

    case vxInput::KEY_PAGE_DOWN:
      TranslateSection( - int4( GetKeyMultiplier( uKeyboardState ) ) );
      bHandled = true;
      break;

    case vxInput::KEY_HOME:
      pIndexPosition->SetValue( 0 );
      pIndexPosition->Modified( vxModInfoID( vxElement, vxIDs::PathIndexPosition, 0 ) );
      UpdateCameraPosition();
      bHandled = true;
      break;

    case vxInput::KEY_END:
      pIndexPosition->SetValue( - float4( pVessel->size() - 1 ) );
      pIndexPosition->Modified( vxModInfoID( vxElement, vxIDs::PathIndexPosition, 0 ) );
      UpdateCameraPosition();
      bHandled = true;
      break;

    // rotate with <ctrl>-arrow keys
    case KEY_ARROW_LEFT:
      GetEnvironment().GetCamera()->Rotate( GetEnvironment().GetCamera()->GetViewUp(), - fDegreesRotateOnKeyPress );
      GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
      break;

    case KEY_ARROW_RIGHT:
      GetEnvironment().GetCamera()->Rotate( GetEnvironment().GetCamera()->GetViewUp(), fDegreesRotateOnKeyPress );
      GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
      break;

    case KEY_ARROW_UP:
      GetEnvironment().GetCamera()->Rotate( GetEnvironment().GetCamera()->GetViewRight(), - fDegreesRotateOnKeyPress );
      GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
      break;

    case KEY_ARROW_DOWN:
      GetEnvironment().GetCamera()->Rotate( GetEnvironment().GetCamera()->GetViewRight(), fDegreesRotateOnKeyPress );
      GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
      break;

    default:
      break;

    } // switch uKeyId

    // perform the standard camera manipulations
    bHandled |= ManipulateStandardViews( uKeyId, uKeyboardState );

  }
  catch ( ... )
  {
    throw ex::VException( LogRec( "unhandled exception", "vxManipulatorCameraPath", "OnKeyPressed" ) );
  }

  return bHandled;

} // OnKeyPressed()


/**
 * Event handler if mouse wheel is scrolled
 *
 * @param uKeyId         the key that was pressed
 * @param iDelta         the amount the wheel was scrolled in either direction
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */
bool vxManipulatorCameraPath::OnMouseWheel( const Point2D<int2> & position, const int2 iDelta,
                                                  const uint4 uKeyboardState )
{

  try
  {
    // scroll along the vessel on wheel rotation
    TranslateSection( float4( iDelta ) / vxInput::GetMouseWheelQuantum() * float4( GetKeyMultiplier( uKeyboardState ) ) );
    UpdateCursor3DPosition();
  }
  catch ( ... )
  {
    throw ex::VException( LogRec( "unhandled exception", "vxManipulatorCameraPath", "OnMouseWheel" ) );
  }

  return true;

} // OnMouseWheel()


/**
* Event handler if the mouse button is pressed
*
* @param position  mouse position
* @param eButtonId mouse button
* @param uKeyboardState state of the mouse
*
* @return true if handled
*/ 
bool vxManipulatorCameraPath::OnButtonDown( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState )
{
  bool bHandled( false );

  try
  {

    // rotate on the left mouse button
    if ( eButtonId == BUTTON_LEFT )
    {
      // begin rotating
      SetLastMousePosition( position );
      m_upDirection = GetEnvironment().GetCamera()->GetViewUp();
      m_rightDirection = GetEnvironment().GetCamera()->GetViewRight();
      m_bRotating = true;
      bHandled = true;
    } // if left button

    // drag on the middle mouse button
    else if ( eButtonId == BUTTON_MIDDLE )
    {
      // begin dragging
      SetLastMousePosition( position );
      m_bDragging = true;
      bHandled = true;
    } // if left button

    if( m_bRotating || m_bDragging )
      GetEnvironment().SetUserInteracting( true );

  } // try
  catch ( ... )
  {
    throw ex::VException( LogRec( "OnButtonDown failed.", "vxManipulatorCameraPath", "OnButtonDown" ) );
  }

  return bHandled;

} // OnButtonDown()


/**
 * Event handler if the mouse is moved
 *
 * @param position       mouse position
 * @param uKeyboardState mouse state
 *
 * @return true if handled
 */ 
bool vxManipulatorCameraPath::OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState )
{
  bool bIfHandled(false);

  try
  {

    // handle looking around (rotating the viewpoint)
    if ( m_bRotating )
    {
      Vector3Di mouseChange( position.m_x - GetLastMousePosition().m_x, position.m_y - GetLastMousePosition().m_y, 0 );
      const double fMaxRotationRadians( M_PI );
      const float4 fRotationRadiansX( float4( mouseChange.m_x ) / GetEnvironment().GetWinRect().GetViewportWidth() / 2 * fMaxRotationRadians );
      const float4 fRotationRadiansY( float4( mouseChange.m_y ) / GetEnvironment().GetWinRect().GetViewportHeight() / 2 * fMaxRotationRadians );
      GetEnvironment().GetCamera()->Rotate( m_upDirection, fRotationRadiansX, GetEnvironment().GetCamera()->GetEye() );
      GetEnvironment().GetCamera()->Rotate( m_rightDirection, fRotationRadiansY, GetEnvironment().GetCamera()->GetEye() );
      GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
      SetLastMousePosition( position );
    }

    // handle dragging the viewpoint along the centerline
    if ( m_bDragging )
    {
      const int4 iMouseChange( position.m_y - GetLastMousePosition().m_y );
      const float4 fMmPerPixel( -0.25F );
      const float4 fMmTranslation( GetKeyMultiplier( uKeyboardState ) * fMmPerPixel * iMouseChange );
      TranslateSection( fMmTranslation );
      SetLastMousePosition( position );
    }
  } // try
  catch ( ... )
  {
    throw ex::VException( LogRec( "OnMouseMove failed.", "vxManipulatorCameraPath", "OnMouseMove" ) );
  }

  return bIfHandled;

} // OnMouseMove()


/**
 * Event handler if the mouse is released
 *
 * @param position       mouse position
 * @param eButtonId      mouse button
 * @param uKeyboardState mouse state
 *
 * @return true if handled
 */ 
bool vxManipulatorCameraPath::OnButtonUp( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState )
{

  bool bHandled( false );

  try
  {
    // update the other views
    if ( m_bDragging ) UpdateCursor3DPosition();

    // stop everything
    m_bRotating = false;
    m_bDragging = false;
    GetEnvironment().SetUserInteracting( false );
    bHandled = true;
  } // try
  catch ( ... )
  {
    throw ex::VException( LogRec( "OnButtonUp failed.", "vxManipulatorCameraPath", "OnButtonUp" ) );
  }

  return bHandled;

} // OnButtonUp()


/**
 * Translate the section by a certain distance.
 *
 * @param fDistance the distance in millimeters
 */
void vxManipulatorCameraPath::TranslateSection( const float4 fDistance )
{
  try
  {

   // get the vessel
    vxVessel * pVessel = dynamic_cast< vxVessel * >( GetEnvironment().GetElement( vxIDs::Path ) );
    if ( pVessel == NULL )
    {
      LogWrn( "Vessel array pointer is NULL", "vxManipulatorCameraPath", "TranslateSection" );
      return;
    }
    if ( pVessel->size() == 0 ) return;

    // get the fractional index along the centerline
    vxShareableObject< vxElementFloat4 > * pIndexPosition( dynamic_cast< vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::PathIndexPosition ) ) );
    if ( pIndexPosition == NULL )
    {
      LogWrn( "Path index position is null", "vxManipulatorCameraPath", "TranslateSection" );
      return;
    }

    const float4 fCurrentMM( pVessel->ResampleAccumulatedLengthMM( fabs( pIndexPosition->GetValue() ) ) );
    const float4 fMaximumMM( pVessel->ResampleAccumulatedLengthMM( pVessel->size() -1 ) );
    const float4 fDesiredMM( Bound( 0.0F, fCurrentMM + fDistance, float4( fMaximumMM ) ) );
    float4 fNewIndex( pVessel->GetFractionalIndex( fDesiredMM ) );

    // clamp to valid range
    fNewIndex = Bound( 0.0F, fNewIndex, float4( pVessel->size()-1 ) );

    // bounds check the section index
    if ( fNewIndex < 0.0F || fNewIndex > static_cast< float4 >( pVessel->size()-1 ) ) return;

    // make sure it's actually changing
    const float4 fOldIndex( fabs( pIndexPosition->GetValue() ) );
    if ( fOldIndex == fNewIndex ) return;

    // set the current vessel section index
    // (make it negative to indicate looking backward, so use the sign to determine direction, but abs(index) for position)
    const bool bLookForward( fOldIndex < fNewIndex );
    pIndexPosition->SetValue( fNewIndex * ( bLookForward ? 1 : -1 ) );
    pIndexPosition->Modified( vxModInfoID( vxElement, vxIDs::PathIndexPosition, 0 ) );

    UpdateCameraPosition();

  } // try

  catch ( ... )
  {
    throw ex::VException( LogRec( "unhandled exception", "vxManipulatorVesselNavigati", "TranslateSection" ) );
  }

} // TranslateSection()


/**
* Update the camera position to the current position along the path
*
* @param bLookForward if should point the camera forward
*/
void vxManipulatorCameraPath::UpdateCameraPosition()
{
  
  // get the vessel
  vxVessel * pVessel = dynamic_cast< vxVessel * >( GetEnvironment().GetElement( vxIDs::Path ) );
  if ( pVessel == NULL )
  {
    LogWrn( "Vessel array pointer is NULL", "vxManipulatorCameraPath", "SetVesselSectionIndex" );
    return;
  }
  if ( pVessel->size() == 0 ) return;

  // get the fractional index along the centerline
  vxShareableObject< vxElementFloat4 > * pIndexPosition( dynamic_cast< vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::PathIndexPosition ) ) );
  if ( pIndexPosition == NULL )
  {
    LogWrn( "Path index position is null", "vxManipulatorCameraPath", "TranslateSection" );
    return;
  }

  // set the camera location - need to interpolate these
  const float4 fIndexPosition( fabs( pIndexPosition->GetValue() ) );
  const Point<float4> position( pVessel->GetResampledPosition( fIndexPosition ) );
  const Normal<float4> upDirection( pVessel->GetResampledUpDirection( fIndexPosition ) );

  // look ahead a bit
  // + indicates looking forward, - indicates looking backward
  const float4 fLookAheadMM( 50 * ( ( pIndexPosition->GetValue() > 0 ) ? 1 : -1 ) );
  const float4 fCurrentMM( pVessel->ResampleAccumulatedLengthMM( fabs( pIndexPosition->GetValue() ) ) );
  const float4 fMaximumMM( pVessel->ResampleAccumulatedLengthMM( pVessel->size() -1 ) );
  const float4 fDesiredMM( Bound( 0.0F, fCurrentMM + fLookAheadMM, float4( fMaximumMM ) ) );
  float4 fLookAheadIndex( Bound( 0.0F, pVessel->GetFractionalIndex( fDesiredMM ), float4( pVessel->size()-1 ) ) );
  const Point3Df lookAheadPosition( pVessel->GetResampledPosition( fLookAheadIndex ) );
  const Normal3Df tangent( Normal3Df( lookAheadPosition - position ) );

  GetEnvironment().GetCamera()->SetEye( position );
  GetEnvironment().GetCamera()->SetViewDirection( tangent );
  GetEnvironment().GetCamera()->SetViewUp( upDirection );
  GetEnvironment().GetCamera()->Modified(vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

} // UpdateCameraPosition()


/**
 * Update the cursor3D position to the current position along the path
 *
 * @param bLookForward if should point the camera forward
 */
void vxManipulatorCameraPath::UpdateCursor3DPosition()
{
  // get the vessel
  vxVessel * pVessel = dynamic_cast< vxVessel * >( GetEnvironment().GetElement( vxIDs::Path ) );
  if ( pVessel == NULL )
  {
    LogWrn( "Vessel array pointer is NULL", "vxManipulatorCameraPath", "SetVesselSectionIndex" );
    return;
  }
  if ( pVessel->size() == 0 ) return;

  // get the fractional index along the centerline
  vxShareableObject< vxElementFloat4 > * pIndexPosition( dynamic_cast< vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::PathIndexPosition ) ) );
  if ( pIndexPosition == NULL )
  {
    LogWrn( "Path index position is null", "vxManipulatorCameraPath", "TranslateSection" );
    return;
  }

  // set the camera location - need to interpolate these
  const float4 fIndexPosition( fabs( pIndexPosition->GetValue() ) );
  const Point<float4> position( pVessel->GetResampledPosition( fIndexPosition ) );
  Normal<float4> tangent( pVessel->GetResampledTangent( fIndexPosition ) );
  // + indicates looking forward, - indicates looking backward
  if ( pIndexPosition->GetValue() < 0 ) tangent = - tangent;
  const Normal<float4> upDirection( pVessel->GetResampledUpDirection( fIndexPosition ) );

  GetEnvironment().GetCursor3d()->SetEye( position );
  GetEnvironment().GetCursor3d()->SetViewDirection( tangent );
  GetEnvironment().GetCursor3d()->SetViewUp( upDirection );
  GetEnvironment().GetCursor3d()->Modified( vxModInfo( vxCursor3d, vxUtils::CURSOR3D_MODIFIED ) );

} // UpdateCursor3DPosition()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorCameraPath.C,v $
// Revision 1.7  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.6  2006/08/03 14:01:35  geconomos
// removed inclusion of "vxRenderEngineData.h"
//
// Revision 1.5  2006/07/07 13:34:37  geconomos
// set "UserInteracting" flag in associated environment as needed
//
// Revision 1.4  2006/06/27 20:13:23  geconomos
// setting "user interacting" flag in environment accordingly
//
// Revision 1.3  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.1.12.1  2005/07/19 12:17:22  frank
// added a little look-ahead for centerline flights
//
// Revision 1.1  2004/05/25 14:31:16  frank
// added base class for camera manipulators
//
// Revision 1.1  2004/04/06 18:19:41  frank
// renamed vessel to path
//
// Revision 1.2  2004/04/06 16:26:11  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2003/12/18 14:14:18  frank
// Renamed the element ID filename to match class name
//
// Revision 1.12  2003/12/16 19:06:46  frank
// Added additional key definitions
//
// Revision 1.11  2003/11/20 18:27:24  frank
// Centralized the camera manipulations
//
// Revision 1.10  2003/11/12 15:35:18  frank
// Issue #3471: Fixed crash when no path is available
//
// Revision 1.9  2003/09/11 20:37:12  frank
// disabled momentum in the button rather than here
//
// Revision 1.8  2003/09/11 20:17:22  frank
// Issue #3479: Disabled momentum when you start camera or path fly
//
// Revision 1.7  2003/09/08 20:05:45  frank
// Added more features such as updating cursor 3D and arrow key navigation
//
// Revision 1.6  2003/08/29 19:11:28  frank
// Cleaned up rendering of arrow and path navigation
//
// Revision 1.5  2003/08/20 21:20:58  dongqing
// raplace codes with new function GetFractionalIndex() in vxVessel class
//
// Revision 1.4  2003/08/18 17:35:57  frank
// Added middle-mouse dragging along the path
//
// Revision 1.3  2003/08/18 15:11:20  frank
// Fixed scroll-wheel navigation
//
// Revision 1.2  2003/08/14 16:55:06  frank
// Added turning of viewpoint
//
// Revision 1.1  2003/08/13 20:03:35  frank
// moved from Vascular project
//
// Revision 1.5  2003/07/15 17:09:57  frank
// Fixed endoluminal flight speed
//
// Revision 1.4  2003/05/27 13:20:01  frank
// code review
//
// Revision 1.3  2003/05/21 12:04:21  frank
// code review
//
// Revision 1.2  2003/05/20 18:18:19  dongqing
// code review
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.12  2003/04/30 15:51:25  frank
// code review
//
// Revision 1.11  2003/03/14 18:02:54  frank
// Fixed view direction along vessel
//
// Revision 1.10  2003/03/14 17:29:05  frank
// Turned the camera in the direction of movement
//
// Revision 1.9  2003/03/14 16:06:30  frank
// Organized code better
//
// Revision 1.8  2003/03/13 14:32:31  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.7  2003/03/12 22:38:51  geconomos
// Commented out code to initialize the logger.
//
// Revision 1.6  2003/03/07 20:42:43  geconomos
// Defaulted handled flag in OnKeyPress to false instead of true.
//
// Revision 1.5  2003/02/18 17:59:16  frank
// Cleaned up debug messages
//
// Revision 1.4  2003/02/14 22:34:52  frank
// Made it work
//
// Revision 1.3  2003/02/14 19:25:02  frank
// Working on endoluminal flight
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorCameraPath.C,v 1.7 2007/02/05 15:29:25 geconomos Exp $
// $Id: vxManipulatorCameraPath.C,v 1.7 2007/02/05 15:29:25 geconomos Exp $
