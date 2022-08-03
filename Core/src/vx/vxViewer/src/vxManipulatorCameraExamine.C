// $Id: vxManipulatorCameraExamine.C,v 1.24 2007/02/06 18:58:18 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: jenny hizver (mailto:jenny@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorCameraExamine.h"
#include "vxCamera.h"
#include "vxEnvironment.h"
#include "vxUtils.h"
#include "vxDataRenderer.h"
#include "vxUndoActionCamera.h"


// defines
#define FILE_REVISION "$Revision: 1.24 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorCameraExamine::vxManipulatorCameraExamine(vxEnvironment & environment):
vxManipulatorCamera(environment)
{
  m_bRotateVolume      = false;
  m_bTranslate         = false;
  m_bFirstMouseMove    = false;
} // constructor


/**
 * Destructor
 */
vxManipulatorCameraExamine::~vxManipulatorCameraExamine()
{
} // ~vxManipulatorCameraExamine()


/**
 * Handle the mouse scroll events
 * @param mouse position
 * @param delta
 * @param key state
 * @return handled?
 */
bool vxManipulatorCameraExamine::OnMouseWheel(const Point2D<int2> & position, 
                                              const int2 iDelta, 
                                              const uint4 uKeyboardState)
{
  bool bHandled(false);
  vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );

  const float4 fEpsilon(1.0);
  int4 iStep(-iDelta/vxInput::GetMouseWheelQuantum());

  // go faster with special keys
  iStep *= GetKeyMultiplier(uKeyboardState);

  float4 fStep(static_cast<float4>(iStep) * 10.0);
  // In perspective projection, translate the eye point for zoom.
  Point<float4> eye(GetEnvironment().GetCamera()->GetEye());
  Vector<float4> viewDirection(GetEnvironment().GetCamera()->GetViewReference() - GetEnvironment().GetCamera()->GetEye());
  viewDirection.Normalize();
  float4 fDistance(eye.GetEuclideanDistance(GetEnvironment().GetCamera()->GetViewReference()));
  if ((fDistance - fEpsilon) < fStep)
  {
    fStep = fDistance - fEpsilon;
  }
  viewDirection *= fStep;
  Point<float4> eyeNew(eye + viewDirection);
  Box<float4> box(GetEnvironment().GetRenderingMode()->GetVisibleDataBox());
  if (eyeNew.GetEuclideanDistance(GetEnvironment().GetCamera()->GetViewReference()) < (box.GetOuterRadius()) * 0.5F)
  {
    return bHandled;
  }

  GetEnvironment().GetCamera()->TranslateEyePoint(viewDirection);
  GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));

  return bHandled;
} // OnMouseWheel()




/**
 * Handle the mouse move events for rotation.
 * Rotation axis is a linear combination of the View Direction
 * and the Up vector/Right vector. The rotation is more about the
 * Up vector/Right vector near the object and is more about the
 * View Direction further away from the object.
 *
 * @param position       mouse position
 * @param uKeyboardState key state
 *
 * @return handled?
 */
bool vxManipulatorCameraExamine::OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState )
{

  bool bHandled(false);
  bool bModified(false);

  if (m_bTranslate == true)
  {
    if ( m_bFirstMouseMove )
    {
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
      m_bFirstMouseMove = false;
    }

    SetMousePosition(position);
    //get a scaling factor
    Point2D<float4> fDelta(GetMousePosition() - GetLastMousePosition());
    fDelta.m_x /= GetEnvironment().GetWinRect().GetViewportSize().first;
    fDelta.m_y /= GetEnvironment().GetWinRect().GetViewportSize().second;
    float4 fFactor(GetEnvironment().GetCamera()->GetEye().GetEuclideanDistance(GetEnvironment().GetCamera()->GetViewReference()) + 1.0F);
    fDelta.m_x *= fFactor; 
    fDelta.m_y *= fFactor;
    // initialize vectors
    Vector<float4> upVector(GetEnvironment().GetCamera()->GetViewUp());
    Vector<float4> viewVector(GetEnvironment().GetCamera()->GetViewDirection());
    Vector<float4> rightVector(upVector.Cross(viewVector));
    rightVector.Normalize();
    Vector<float4> translateVector;

    // translate along the right vector
    if (fDelta.m_x != 0)
    {
      translateVector.m_x = rightVector.m_x * fDelta.m_x;
      translateVector.m_y = rightVector.m_y * fDelta.m_x;
      translateVector.m_z = rightVector.m_z * fDelta.m_x;
      GetEnvironment().GetCamera()->TranslateEyePoint(translateVector);
      GetEnvironment().GetCamera()->TranslateReferencePoint(translateVector);
      bModified = true;
    }

    // translate along the up vector
    if (fDelta.m_y != 0)
    {
      translateVector.m_x = upVector.m_x * fDelta.m_y;
      translateVector.m_y = upVector.m_y * fDelta.m_y;
      translateVector.m_z = upVector.m_z * fDelta.m_y;
      GetEnvironment().GetCamera()->TranslateEyePoint(translateVector);
      GetEnvironment().GetCamera()->TranslateReferencePoint(translateVector);
      bModified = true;
    }
    SetLastMousePosition(position);
  }
  else if (m_bRotateVolume)
  {
    if ( m_bFirstMouseMove )
    {
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
      m_bFirstMouseMove = false;
    }
    SetMousePosition(position);
    Point2D<int2> iDelta(GetMousePosition() - GetLastMousePosition());
    vxCamera * pCamera(GetEnvironment().GetCamera());
    Box<float4> box(GetEnvironment().GetRenderingMode()->GetVisibleDataBox());
    Point<float4> centerOfRotation(box.GetMidPoint());

    const float fRotationOnlyPadding(0.1F);
    float4 fWeightX(min(1.0f, fabs(Interpolate(0.0f, float4(GetMousePosition().X()), float4(GetEnvironment().GetWinRect().GetViewportSize().first -1), -1.0f, 1.0f))));
    float4 fWeightY(min(1.0f, fabs(Interpolate(0.0f, float4(GetMousePosition().Y()), float4(GetEnvironment().GetWinRect().GetViewportSize().second-1), -1.0f, 1.0f))));

    // rotate about the right vector and view vector
    if (iDelta.m_y != 0)
    {
      float4 fSign(1);
      if (GetMousePosition().m_x > 0 && GetQuadrant() & 1)
      {
        fSign = -1;
      }

      const float4 fAngle(Interpolate(0.0f, float4(iDelta.Y()), float4(GetEnvironment().GetWinRect().GetViewportSize().second-1), 0.0f, (float4)M_PI));
      float4 fFactor1(Max(0.0F, 1.0F - fRotationOnlyPadding - fWeightX));
      const float4 fFactor2(Min(1.0F, fWeightX + fRotationOnlyPadding));
      if (fWeightX >= (1.0F - fRotationOnlyPadding) || fWeightY >= (1.0F - fRotationOnlyPadding))
        fFactor1 = 0.0F;
      float4 fRightAngle = fFactor1*fAngle;
      float4 fViewAngleOne = -fFactor2*fAngle*fSign;      
      pCamera->Rotate( pCamera->GetViewRight(), fRightAngle, centerOfRotation);
      pCamera->Rotate( pCamera->GetViewDirection(), fViewAngleOne, centerOfRotation);

      GetEnvironment().GetMomentum()->SetRightAngle( fRightAngle );
      GetEnvironment().GetMomentum()->SetViewDirectionRightAngle( fViewAngleOne );

      bModified = true;
    }
    else
    {
      GetEnvironment().GetMomentum()->SetRightAngle( 0.0F );
      GetEnvironment().GetMomentum()->SetViewDirectionRightAngle( 0.0F );
    }

    // rotate about the up vector and view vector
    if (iDelta.m_x != 0)
    {
      float4 fSign(1);
      if (GetMousePosition().m_y > 0 && GetQuadrant() & 2)
      {
        fSign = -1;
      }

      const float4 fAngle(Interpolate(0.0f, float4(iDelta.X()), float4(GetEnvironment().GetWinRect().GetViewportSize().first-1), 0.0f, (float4)M_PI));
      float4 fFactor1(Max(0.0F, 1.0F-fRotationOnlyPadding-fWeightY));
      const float4 fFactor2(Min(1.0F, fWeightY+fRotationOnlyPadding));
      if (fWeightY >= 1.0F-fRotationOnlyPadding)
        fFactor1 = 0.0F;
      float4 fUpAngle = fFactor1*fAngle;
      float4 fViewAngleTwo = fFactor2*fAngle*fSign;      
      pCamera->Rotate( pCamera->GetViewUp(), fUpAngle, centerOfRotation);
      pCamera->Rotate( pCamera->GetViewDirection(), fViewAngleTwo, centerOfRotation);

      GetEnvironment().GetMomentum()->SetUpAngle( fUpAngle );
      GetEnvironment().GetMomentum()->SetViewDirectionUpAngle( fViewAngleTwo );

      bModified = true;
    }
    else
    {
      GetEnvironment().GetMomentum()->SetUpAngle( 0.0F );
      GetEnvironment().GetMomentum()->SetViewDirectionUpAngle( 0.0F );
    }

    if (GetEnvironment().GetMomentum()->GetEnabled() )
    {
      GetEnvironment().GetMomentum()->SetInitiated( true );
    }
    else
    {
      GetEnvironment().GetMomentum()->SetInitiated( false );
    }

    SetLastMousePosition(position);  

  } // rotate 

  if ( bModified )
  {
    GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
  }

  return bHandled;

} // OnMouseMove()


/**
 * Handle the button down event
 *
 * @param mouse position
 * @param mouse button
 * @param key state
 *
 * @return handled?
 */
bool vxManipulatorCameraExamine::OnButtonDown( const Point2D<int2> & position, 
                                               const ButtonEnum eButtonId, 
                                               const uint4 uKeyboardState )
{

  bool bHandled( false );

  if ( eButtonId == BUTTON_LEFT )
  {
    if ( uKeyboardState & STATE_SHIFT )
    {
      m_bFirstMouseMove = true;
      m_bTranslate = true;
      //vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
      GetEnvironment().SetUserInteracting( true );
    }
    else
    {
      m_bFirstMouseMove = true;
      m_bRotateVolume = true;
      //vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
      GetEnvironment().SetUserInteracting( true );
    }

    // disable momentum
    if( GetEnvironment().GetMomentum()->GetInitiated() )
    {
      GetEnvironment().GetMomentum()->SetInitiated( false );
      GetEnvironment().SetUserInteracting( false );
      GetEnvironment().GetMomentum()->Modified( vxModInfo(vxMomentum, vxUtils::MOMENTUM_MODIFIED) );
    }
    

  } // left mouse button

  SetMousePosition(position);
  SetLastMousePosition(position);

  return bHandled;

} // OnButtonDown()


/**
 * Handle button up event
 *
 * @param mouse position
 * @param mouse button
 * @param key state
 *
 * @return handled?
 */
bool vxManipulatorCameraExamine::OnButtonUp( const Point2D<int2> & position, 
                                             const ButtonEnum eButtonId, 
                                             const uint4 uKeyboardState )
{

  bool bHandled( m_bRotateVolume || m_bTranslate  );

  try
  {
    if (eButtonId == BUTTON_LEFT)
    {
      if( m_bRotateVolume  || m_bTranslate )
        GetEnvironment().SetUserInteracting( false );

    if( GetEnvironment().GetMomentum()->GetEnabled() && GetEnvironment().GetMomentum()->GetInitiated() )
      GetEnvironment().SetUserInteracting( true );

      m_bFirstMouseMove = false;
      m_bRotateVolume = false; 
      m_bTranslate    = false;
    } // left mouse button

    SetMousePosition(position);
  }
  catch( ... )
  {
    throw ex::InvalidDataException( LogRec("Unhandle exception on", "OnButtonUp", "vxManipulatorCameraExamine") );
  }

  return bHandled;

} // OnButtonUp()


/**
 * Handle the keyboard events
 *
 * @param key
 * @param key state
 *
 * @return handled?
 */
bool vxManipulatorCameraExamine::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{

  bool bHandled(false);

  switch (uKeyId)
  {
  case KEY_ARROW_LEFT:
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
    GetEnvironment().GetCamera()->Rotate(GetEnvironment().GetCamera()->GetViewUp(), static_cast<float4>(-10.0 * M_PI/180.0));
    GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
    break;
  case KEY_ARROW_RIGHT:
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
    GetEnvironment().GetCamera()->Rotate(GetEnvironment().GetCamera()->GetViewUp(), static_cast<float4>(10.0 * M_PI/180.0));
    GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
    break;
  case KEY_ARROW_UP:
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
    GetEnvironment().GetCamera()->Rotate(GetEnvironment().GetCamera()->GetViewRight(), static_cast<float4>(-10.0 * M_PI/180.0));
    GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
    break;
  case KEY_ARROW_DOWN:
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
    GetEnvironment().GetCamera()->Rotate(GetEnvironment().GetCamera()->GetViewRight(), static_cast<float4>(10.0 * M_PI/180.0));
    GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
    break;
  case vxInput::KEY_A:
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
    vxUtils::CameraViewAll( * GetEnvironment().GetCamera(), GetEnvironment().GetRenderingMode()->GetVisibleDataBox() );
    GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
    bHandled = true;
    break;
  case vxInput::KEY_C:
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
    ToggleProjectionType();
    break;
  case vxInput::KEY_MINUS:
  case vxInput::KEY_MINUS_KEYPAD:
    OnMouseWheel(GetMousePosition(), vxInput::GetMouseWheelQuantum(), uKeyboardState);
    break;
  case vxInput::KEY_EQUALS:
  case vxInput::KEY_PLUS_KEYPAD:
    OnMouseWheel(GetMousePosition(), -vxInput::GetMouseWheelQuantum(), uKeyboardState);
    break;
  default:
    break;
  }

  // handle the camera presets
  if ( uKeyId >= '0' && uKeyId <= '9' && uKeyboardState & STATE_SHIFT )
  {
    // convert zero key into tenth preset
    static int viFlip[10] = { 9, 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    const int4 iPresetIndex( viFlip[ uKeyId - '0' ] );

    // check for a valid camera preset
    if ( GetEnvironment().GetPreset()->GetCameraPresets().size() > iPresetIndex )
    {

      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );

      // get camera from presets, adjust it to the current volume size, and set it as new camera
      vxCamera camera(GetEnvironment().GetPreset()->GetCameraPresets()[iPresetIndex].GetCamera());
      Triple<float4> size(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());
      vxUtils::AdjustCameraProportionally(camera, GetEnvironment().GetPreset()->GetBoundingBox(),
                                          Box<float4>( 0, 0, 0, size[0], size[1], size[2]));
      *(GetEnvironment().GetCamera()) = camera;
      GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
      bHandled = true;

    } // if valid camera preset
  } // if <shift>-0 through <shift>-9 pressed

  // perform the standard camera manipulations
  if ( ManipulateStandardViews( uKeyId, uKeyboardState ) == true )
  {
    bHandled = true;
  }

  return bHandled;

} // OnKeyPressed()


/**
 * Returns in which quadrant the current mouse position is in
 *
 *              +----------->X
 *              |  0 | 1
 *              | ___|___
 *              |    |
 *              |  2 | 3
 *              |
 *              Y
 *
 * @return uint1 the quadrant
 */
uint1 vxManipulatorCameraExamine::GetQuadrant()
{

  Point2D<uint2> position(GetMousePosition());
  Point2D<float4> halfWindow;
  halfWindow.m_x = float4(GetEnvironment().GetWinRect().GetViewportSize().first) * 0.5F;
  halfWindow.m_y = float4(GetEnvironment().GetWinRect().GetViewportSize().second) * 0.5F;
  uint1 uQuadrant(0);

  if (position.m_x > halfWindow.m_x)
    uQuadrant += 1;
  if (position.m_y > halfWindow.m_y)
    uQuadrant += 2;

  return uQuadrant;

} // GetQuadrant()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorCameraExamine.C,v $
// Revision 1.24  2007/02/06 18:58:18  geconomos
// corrected "user interacting" usage with momentum
//
// Revision 1.23  2007/02/05 18:06:15  geconomos
// corrected setting of "interacting flag"
//
// Revision 1.22  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.21  2007/01/16 17:04:42  gdavidson
// Removed OnTimer
//
// Revision 1.20  2007/01/04 18:02:29  romy
// adjusted the Undo action only for the first mouse movement
//
// Revision 1.19  2006/08/03 14:01:34  geconomos
// removed inclusion of "vxRenderEngineData.h"
//
// Revision 1.18  2006/07/07 13:34:37  geconomos
// set "UserInteracting" flag in associated environment as needed
//
// Revision 1.17  2006/06/27 20:13:23  geconomos
// setting "user interacting" flag in environment accordingly
//
// Revision 1.16  2006/02/24 20:14:32  frank
// made the undo more granular
//
// Revision 1.15  2006/02/24 16:57:00  frank
// added undo information to oblique plane manipulation
//
// Revision 1.14  2006/01/27 15:47:45  geconomos
// still migrating to visual studio 2005
//
// Revision 1.13  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.12  2005/12/08 16:42:30  frank
// added undo support
//
// Revision 1.11  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.10.2.4  2005/08/12 17:20:51  geconomos
// Implmentation of vxMomentum environment element
//
// Revision 1.10.2.3  2005/08/11 15:06:04  frank
// formatting
//
// Revision 1.10.2.2  2005/08/04 18:57:09  frank
// removed unused cursor 3D tracking
//
// Revision 1.10.2.1  2005/08/01 19:12:07  frank
// changed function name according to coding standards
//
// Revision 1.10  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.9.2.2  2005/02/10 20:57:24  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
//
// Revision 1.9.2.1  2005/02/08 15:34:51  frank
// added plus/minus key on standard keyboard in addition to numeric keypad
//
// Revision 1.9  2004/10/18 14:48:58  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.8  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.7  2004/08/16 20:41:30  frank
// typo
//
// Revision 1.6  2004/08/16 20:30:45  frank
// added back the A key to view all of the 3D data
//
// Revision 1.5  2004/05/25 14:31:16  frank
// added base class for camera manipulators
//
// Revision 1.4  2004/04/26 19:00:16  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.3  2004/04/06 16:24:46  frank
// unified environment access naming
//
// Revision 1.2  2004/04/05 18:31:03  frank
// removed eyepoint rotation from camera examine manipulator
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.68  2004/01/09 17:21:44  frank
// fixed hardcoded key values - replaced with enumeration
//
// Revision 1.67  2003/12/24 15:37:32  dongqing
// add try-catch for OnButtonUp. This is for tracking dangling
// pointer bug
//
// Revision 1.66  2003/11/20 18:27:24  frank
// Centralized the camera manipulations
//
// Revision 1.65  2003/11/16 23:39:48  michael
// added ability to controle the rendering speed/quality ...
//
// Revision 1.64  2003/10/21 17:13:41  frank
// Issue #3522: Did not reset zoom for standard views
//
// Revision 1.63  2003/10/03 16:31:27  michael
// fixed rotation so that 10% of the border (on each side) are for rotation only
//
// Revision 1.62  2003/09/08 20:43:18  michael
// implemented pure view direction rotation in the 10% border on the left, right,
// top, and bottom of the window during OnMouseMove() (if dragging).
//
// Revision 1.61  2003/08/22 19:35:24  frank
// Moved standard views to ctrl key
//
// Revision 1.60  2003/08/22 19:27:32  frank
// early check-in
//
// Revision 1.59  2003/08/22 19:22:18  frank
// Added standard views
//
// Revision 1.58  2003/08/21 13:11:18  michael
// using M_PI instead of typed in values for it
//
// Revision 1.57  2003/08/20 15:48:17  michael
// using M_PI instead of typed in values for it
//
// Revision 1.56  2003/08/01 21:39:41  michael
// took out redundant modified(camera) in OnButtonUp() since between the last
// OnMouseMove() and the OnButtonUp() was no more change and the rerendering
// is redundant.
//
// Revision 1.55  2003/06/19 20:21:21  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.54  2003/05/16 13:21:33  frank
// formatting
//
// Revision 1.53  2003/05/16 11:45:25  michael
// code review
//
// Revision 1.52  2003/05/07 11:44:01  michael
// initial code review
//
// Revision 1.51  2003/05/01 18:50:43  dongqing
// code review
//
// Revision 1.50  2003/04/29 21:17:35  michael
// removed vxManipulatorCamera from inheritance
//
// Revision 1.49  2003/04/08 18:47:52  frank
// Issue #3054: Permitted greater zooming into the dataset.
//
// Revision 1.48  2003/03/19 19:34:26  frank
// Removed obsolete VIEW_CHANGING mode
//
// Revision 1.47  2003/03/12 15:20:11  frank
// Properly interpreted scroll wheel messages
//
// Revision 1.46  2003/03/10 19:09:55  geconomos
// Forced a modified event only if volume was actually rotated.
//
// Revision 1.45  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.44  2003/01/24 23:31:39  ingmar
// code walkthrough changes
//
// Revision 1.43  2003/01/22 21:54:54  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.42  2003/01/21 20:39:45  michael
// using M_PI instead of hardcoded numbers
//
// Revision 1.41  2002/11/26 16:21:37  ingmar
// added automatic animated changeing loop rotation on key m
//
// Revision 1.40  2002/11/20 15:27:01  ingmar
// removed compiler warning
//
// Revision 1.39  2002/11/19 21:24:22  michael
// removed redundant code (Enter/Leave)
//
// Revision 1.38  2002/11/14 17:29:21  michael
// added rotation of camera on arrow keys
//
// Revision 1.37  2002/11/14 16:31:01  michael
// added rotation of camera on arrow keys
//
// Revision 1.36  2002/11/11 16:36:08  ingmar
// upon instantiation this manipulator now sets the frame rate demand to low
//
// Revision 1.35  2002/11/07 18:46:53  michael
// after viewAll(), modified is now called
//
// Revision 1.34  2002/11/06 23:27:55  michael
// brought back toggling of orthographic and perspective
//
// Revision 1.33  2002/10/25 18:23:50  ingmar
// added ratation about the eye when CTRL is pressed
//
// Revision 1.32  2002/10/17 15:32:00  frank
// Centralized key multiplier function.
//
// Revision 1.31  2002/10/16 22:27:45  michael
// took out unused code for serialization that was there just temporary
//
// Revision 1.30  2002/10/16 15:14:41  ingmar
// removed include of vxSerializer.h
//
// Revision 1.29  2002/10/14 22:55:30  michael
// checked in code to generate XML
//
// Revision 1.28  2002/10/13 23:51:01  michael
// bounding box adjustments
//
// Revision 1.27  2002/10/11 14:44:04  frank
// Disabled perspective till it works again.
//
// Revision 1.26  2002/09/20 21:21:05  geconomos
// Removed ugly hand cursor.
//
// Revision 1.25  2002/09/19 15:49:37  dmitry
// Mouse cursor added.
//
// Revision 1.24  2002/08/20 22:30:58  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.23  2002/08/05 16:20:36  dmitry
// Event name added to OnTimer.
//
// Revision 1.22  2002/06/27 19:27:34  michael
// completed transition from CameraNew to vxCamera
//
// Revision 1.21  2002/06/26 19:05:09  dmitry
// Rendering mode's VIEW_CHANGING set when momentum is enabled.
//
// Revision 1.20  2002/06/26 17:47:23  michael
// added SHIFt driven switch between rotate and translate
//
// Revision 1.19  2002/06/20 02:14:55  dmitry
// OnMouseEnter and OnMouseLeave implemented to setup the mouse cursor.
//
// Revision 1.18  2002/06/18 20:55:41  michael
// added lower case 'c' to sensitivity list for toggling camera
//
// Revision 1.17  2002/06/17 23:29:53  uday
// Added Panning in 3D, removed unused function GetProjectionType().
//
// Revision 1.16  2002/06/14 21:47:48  michael
// changed projection mode toggle from 'P' to 'C'
//
// Revision 1.15  2002/06/14 20:43:41  michael
// fixed my own bug :-)
//
// Revision 1.14  2002/06/13 22:08:09  michael
// additional modified event when the mouse button in released
//
// Revision 1.13  2002/06/13 19:25:32  michael
// added further rendering mode to identify a dragging camera so that lower resolution
// rendering can be done while moving ...
//
// Revision 1.12  2002/05/09 17:42:11  uday
// Toggle Projection Mode,
// Disabled Panning.
//
// Revision 1.11  2002/05/08 19:46:38  dmitry
// vector bug fixed
//
// Revision 1.10  2002/05/06 21:46:09  dmitry
// Momentum implemented.
//
// Revision 1.9  2002/04/17 18:16:41  frank
// Cleaned up header & footer.
//
// Revision 1.8  2002/03/29 16:06:18  michael
// fixed modified calls
//
// Revision 1.7  2002/03/27 15:07:56  michael
// adjusted Get.SetSize() to Get/SetViewportSize()
//
// Revision 1.6  2002/03/27 14:25:17  michael
// fixed window size problem by using correct call in winRect
//
// Revision 1.5  2002/03/26 19:35:33  michael
// removed constant redrawing
//
// Revision 1.4  2002/03/24 16:36:06  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.3  2002/03/22 19:24:52  manju
// Added WinRect and resize method. --- Dmitry
//
// Revision 1.2  2002/03/20 14:25:58  dmitry
// OnMouseMove flag modified set only if modification occurred.
//
// Revision 1.1  2002/03/11 19:42:43  michael
// moved xvManipulators, vxInput, and vxEnvironment to 75_vxManipulators
//
// Revision 1.1  2002/03/07 22:46:43  dmitry
// Initial Revision
//
// Revision 1.6  2002/03/04 16:43:27  michael
// fomatting and coding guidelines
//
// Revision 1.5  2002/03/01 20:34:04  michael
// minor changes
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorCameraExamine.C,v 1.24 2007/02/06 18:58:18 geconomos Exp $
// $Id: vxManipulatorCameraExamine.C,v 1.24 2007/02/06 18:58:18 geconomos Exp $
