// $Id: vxManipulatorPlaneOblique.C,v 1.18.2.1 2008/11/10 22:11:44 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorPlaneOblique.h"
#include <math.h>
#include "vxEnvironment.h"
#include "vxCamera.h"
#include "vxMathUtils.h"
#include "vxUtils.h"
#include "Plane.h"
#include "vxUndoActionPlane.h"
#include "vxViewerTypeVesselDetailMPROblique.h"


// defines
#define FILE_REVISION "$Revision: 1.18.2.1 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorPlaneOblique::vxManipulatorPlaneOblique( vxEnvironment & environment )
: vxManipulator( environment ),
m_bRotate( false ),
m_bTranslate( false ),
m_bIsDraggingDistance( false )
//m_pUndo( NULL )
{
} // vxManipulatorPlaneOblique()


/**
 * Event handler if key is pressed
 *
 * @param key id
 * @param keyboard state
 *
 * @return handled?
 */ 
bool vxManipulatorPlaneOblique::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{  

  bool bHandled( false );
  Vector3Df size( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim() );
  Box<float4> box( Point3Df( size * vxUtils::GetObliqueZoomFactor() ), Point3Df( size * ( 1 - vxUtils::GetObliqueZoomFactor() ) ) );
  bool bResetPlane = false;

  switch ( uKeyId )
  {
  case vxInput::KEY_A:
    SetRestorePoint();
    if ( uKeyboardState & STATE_CTRL )
    {
      vxUtils::SetStandardCameraView( * GetEnvironment().GetCamera(), vxUtils::ANTERIOR, box, false );
      bResetPlane = true;
    }
    else
    {
      vxUtils::CameraViewAll( * GetEnvironment().GetCamera(), box );
      GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
    }
    break;
  case vxInput::KEY_P:
    SetRestorePoint();
    if ( uKeyboardState & STATE_CTRL )
    {
      vxUtils::SetStandardCameraView( * GetEnvironment().GetCamera(), vxUtils::POSTERIOR, box, false );
      bResetPlane = true;
    }
    break;
  case vxInput::KEY_L:
    SetRestorePoint();
    if ( uKeyboardState & STATE_CTRL )
    {
      vxUtils::SetStandardCameraView( * GetEnvironment().GetCamera(), vxUtils::LEFT, box, false );
      bResetPlane = true;
    }
    break;
  case vxInput::KEY_R:
    SetRestorePoint();
    if ( uKeyboardState & STATE_CTRL )
    {
      vxUtils::SetStandardCameraView( * GetEnvironment().GetCamera(), vxUtils::RIGHT, box, false );
      bResetPlane = true;
    }
    break;
  case vxInput::KEY_S:
    SetRestorePoint();
    if ( uKeyboardState & STATE_CTRL )
    {
      vxUtils::SetStandardCameraView( * GetEnvironment().GetCamera(), vxUtils::SUPERIOR, box, false );
      bResetPlane = true;
    }
    break;
  case vxInput::KEY_I:
    SetRestorePoint();
    if ( uKeyboardState & STATE_CTRL )
    {
      vxUtils::SetStandardCameraView( * GetEnvironment().GetCamera(), vxUtils::INFERIOR, box, false );
      bResetPlane = true;
    }
    break;
  case vxInput::KEY_PAGE_UP:
    SetRestorePoint();
    Move(1, uKeyboardState);
    bHandled = true;
    break;

  case vxInput::KEY_PAGE_DOWN:
    SetRestorePoint();
    Move(-1, uKeyboardState);
    bHandled = true;
    break;

  case vxInput::KEY_HOME:
    {
      SetRestorePoint();
      vxPlaneOblique<float4> * pPlane = dynamic_cast<vxPlaneOblique<float4> *>(GetEnvironment().GetPlaneOblique());
      const Box<float4> box(Box<float4>(Point<float4>(0,0,0), Point<float4>(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim())));
      const float4 fMax(vxMathUtils::GetMaxDistance(box, *pPlane));
      Triplef units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
      float4 fUnitDistance(Min(Min(units[0], units[1]), units[2]));
      if (fabs(GetEnvironment().GetPlaneOblique()->GetNormal().GetMaxAbsComponent()) != 1.0F)
      {
        pPlane->Move(fUnitDistance*(int4)(fMax/fUnitDistance));
      }
      else
      {
        //LogDbg("end: oblique is axis aligned so set to face slice", "", "");
        pPlane->Move(fMax);
      }
      CheckExtremeCases();
      GetEnvironment().GetPlaneOblique()->Modified(vxModInfo(vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED));
      break;
    } 

  case vxInput::KEY_END:
    {
      SetRestorePoint();
      vxPlaneOblique<float4> * pPlane = dynamic_cast<vxPlaneOblique<float4> *>(GetEnvironment().GetPlaneOblique());
      const Box<float4> box(Box<float4>(Point<float4>(0,0,0), Point<float4>(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim())));
      const float4 fMin(vxMathUtils::GetMinDistance(box, *pPlane));
      Triplef units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
      float4 fUnitDistance(Min(Min(units[0], units[1]), units[2]));
      if (fabs(GetEnvironment().GetPlaneOblique()->GetNormal().GetMaxAbsComponent()) != 1.0F)
      {
        pPlane->Move(fUnitDistance*(int4)(fMin/fUnitDistance));
      }
      else
      {
        //LogDbg("home: oblique is axis aligned so set to face slice", "", "");
        pPlane->Move(fMin);
      }
      CheckExtremeCases();
      GetEnvironment().GetPlaneOblique()->Modified(vxModInfo(vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED));
      break;
    }

  case vxInput::KEY_PLUS_KEYPAD:
  case vxInput::KEY_EQUALS:
  case vxInput::KEY_MINUS_KEYPAD:
  case vxInput::KEY_MINUS:
    {
      SetRestorePoint();

      // maybe we need a non linear mapping at some point ...
      Triplef units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
      float fMin(Min(units[0], units[1], units[2]));
      float fFactor(10);
      const bool bIncrement( uKeyId == vxInput::KEY_PLUS_KEYPAD || uKeyId == vxInput::KEY_EQUALS );
      fFactor *= ( bIncrement ? 1 : -1 );
      fFactor *= fMin;
      ZoomView( GetEnvironment(), int4( GetKeyMultiplier( uKeyboardState ) ) * fFactor);
      GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
      bHandled = true;
      break;
    }
  case KEY_ARROW_LEFT:
    SetRestorePoint();
    Rotate( GetEnvironment().GetCamera()->GetViewUp(), (float4)(-5.0 * 2.0*M_PI/360.0));
    break;
  case KEY_ARROW_RIGHT:
    SetRestorePoint();
    Rotate( GetEnvironment().GetCamera()->GetViewUp(), (float4)(5.0 * 2.0*M_PI/360.0));
    break;
  case KEY_ARROW_UP:
    SetRestorePoint();
    Rotate( GetEnvironment().GetCamera()->GetViewRight(), (float4)(-5.0 * 2.0*M_PI/360.0));
    break;
  case KEY_ARROW_DOWN:
    SetRestorePoint();
    Rotate( GetEnvironment().GetCamera()->GetViewRight(), (float4)(5.0 * 2.0*M_PI/360.0));
    break;
  
  default:
    break;

  } // switch key

  // we modified the camera so initialize the plane
  if ( bResetPlane )
  {
    GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
    * GetEnvironment().GetPlaneOblique() = vxPlaneOblique<float4>( GetEnvironment().GetCamera()->GetViewDirection(), box.GetMidPoint() );
    GetEnvironment().GetPlaneOblique()->Modified( vxModInfo( vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED ) );
  }

 /* if ( m_pUndo != NULL )
  {
    vxEnvironment::GetUndoActions().PushAction( m_pUndo );
    m_pUndo = NULL;
  }*/

  return bHandled;

} // OnKeyPressed()


/**
 * Zoom in or out of the current view
 * @param fZoomFactor   amount to zoom (positive to zoom in)
 */
void vxManipulatorPlaneOblique::ZoomView(vxEnvironment & environment, 
                                         const float4 fZoomFactor)
{
  const float4 fMinFOV = 1.0F;
  const float4 fMaxFOV = 90.0F;
  const float4 fNewFOV = environment.GetCamera()->GetFovDegrees() - fZoomFactor;  
  
  if( fNewFOV >= fMinFOV && fNewFOV <= fMaxFOV )
  {
    environment.GetCamera()->SetFovDegrees( fNewFOV );
    environment.GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
  }
} // ZoomView()


/**
 * Event handler if mouse is scrolled
 * @param mouse position
 * @param amount by which the mosue is scrolled
 * @param keyboard state
 * @return handled
 */ 
bool vxManipulatorPlaneOblique::OnMouseWheel(const Point2D<int2> & position, 
                                             const int2 iDelta, 
                                             const uint4 uKeyboardState)
{
  SetRestorePoint();

  // increase or decrease the slice number by one, based on mouse moving up or down
  Move( -iDelta / vxInput::GetMouseWheelQuantum(), uKeyboardState );

 // vxEnvironment::GetUndoActions().PushAction( m_pUndo );
 // m_pUndo = NULL;

  return false;
} // OnMouseWheel()


/**
 * Move plane by the given Delta
 *
 * @param amount by how much to move the plane
 * @param state of mouse.
 */
void vxManipulatorPlaneOblique::Move( int2 iDelta, uint4 uKeyboardState )
{

  // go faster with special keys
  iDelta *= GetKeyMultiplier(uKeyboardState);

  // determine the min/max distance to go
  Triple<float4> units(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVoxelUnits());
  float4 fUnitDistance(Min(Min(units[0], units[1]), units[2]));
  float4 fDelta(iDelta*fUnitDistance);
  Box<float4> box(Box<float4>(Point<float4>(0,0,0), Point<float4>(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim())));
  vxPlaneOblique<float4> * pPlaneOblique(GetEnvironment().GetPlaneOblique());
  float4 fMin(vxMathUtils::GetMinDistance(box, *pPlaneOblique));
  float4 fMax(vxMathUtils::GetMaxDistance(box, *pPlaneOblique));
  int4 iMin(fMin/fUnitDistance);
  int4 iMax(fMax/fUnitDistance);

  //LogDbg("Plane is   : " + ToAscii(GetEnvironment().GetPlaneOblique()->GetNormal().ToString()) + ", " + ToAscii(GetEnvironment().GetPlaneOblique()->GetDistance()), "", "");
  //LogDbg("Unit distance is: " + ToAscii(fUnitDistance), "", "");
  //LogDbg("Trying to move by: " + ToAscii(fDelta), "", "");
  //LogDbg("Min/max are: " + ToAscii(fMin) + ", " + ToAscii(fMax), "", "");

  if ( fMax * fMin > 0.0F )
  {
    LogDbg( "Move: something is wrong, slice is outside!!!", "vxManipulatorPlaneOblique", "Move" );
    return;
  }

  const float fEpsilon( 0.001F * fUnitDistance );

  if ( iDelta > 0 )
  {
    if ( fDelta > fabs( fMin ) )
    {
      if ( fabs( GetEnvironment().GetPlaneOblique()->GetNormal().GetMaxAbsComponent() ) != 1.0F )
      {
        return;
      }
      else
      {
        fDelta = fabs( fMin );
      }
    }
  }
  else if ( iDelta < 0 )
  {
    if ( fabs( fDelta ) > fMax )
    {
      if ( fabs( GetEnvironment().GetPlaneOblique()->GetNormal().GetMaxAbsComponent() ) != 1.0F )
      {
        return;
      }
      else
      {
        fDelta = -fabs( fMax );
      }
    }
  }

  GetEnvironment().GetPlaneOblique()->Move( - fDelta );

  CheckExtremeCases();

  GetEnvironment().GetPlaneOblique()->Modified( vxModInfo( vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED ) );

} // Move()


/**
 * Handle the button down event
 * @param position       mouse position
 * @param eButtonId      mouse button
 * @param uKeyboardState keyboard state
 * @return handled?
 */
bool vxManipulatorPlaneOblique::OnButtonDown(const Point2D<int2> & position, 
                                                const ButtonEnum eButtonId, 
                                                const uint4 uKeyboardState)
{
  bool bHandled(false);

  if (eButtonId == BUTTON_LEFT)
  {
    bHandled   = true;

    if (uKeyboardState & STATE_SHIFT &&  !dynamic_cast< vxViewerTypeVesselDetailMPROblique * >( GetEnvironment().GetViewerType() ) )
      m_bTranslate = true;
    else
      m_bRotate = true; 
  }
  else if (eButtonId == BUTTON_MIDDLE)
  {
    SetDragging(true);
    bHandled = true;
  }

  if( m_bTranslate || m_bRotate || m_bIsDraggingDistance )
    GetEnvironment().SetUserInteracting( true );


  SetMousePosition(position);
  SetLastMousePosition(position);

  return bHandled;
} // OnButtonDown()


/**
 * Handle the mouse move events for rotation.
 * Rotation axis is a linear combination of the View Direction
 * and the Up vector/Right vector. The rotation is more about the
 * Up vector/Right vector near the object and is more about the
 * View Direction further away from the object.
 *
 * @param mouse position
 * @param key state
 *
 * @return handled?
 */
bool vxManipulatorPlaneOblique::OnMouseMove( const Point2D<int2> & mousePosition, const uint4 uKeyboardState )
{

  bool bModified(false);

  SetMousePosition(mousePosition);

  vxShareableObject< vxPlaneOblique<float4> > * pPreviousPlaneOblique = GetEnvironment().GetPlaneOblique();
  vxShareableObject< vxCamera > * pPreviousCamera = GetEnvironment().GetCamera();

  if (IsDragging() == true)
  {
    SetRestorePoint();
    uint4 uDiff(GetMousePosition().m_y - GetLastMousePosition().m_y);
    Move(uDiff, uKeyboardState);
    SetLastMousePosition(mousePosition);
  }

  vxCamera * pCamera(GetEnvironment().GetCamera());

  if (m_bTranslate == true )
  {
    SetRestorePoint();

    // get a scaling factor
    Point2D<float4> fDelta(GetMousePosition() - GetLastMousePosition());
    fDelta.m_x /= GetEnvironment().GetWinRect().GetViewportSize().first;
    fDelta.m_y /= GetEnvironment().GetWinRect().GetViewportSize().second;
    float4 fFactor(pCamera->GetEye().GetEuclideanDistance(pCamera->GetViewReference()) + 1.0F);
    fDelta.m_x *= fFactor; 
    fDelta.m_y *= fFactor;

    // initialize vectors
    Vector<float4> fUpVector(pCamera->GetViewUp());fUpVector.Normalize();
    Vector<float4> fViewVector(pCamera->GetViewReference() - pCamera->GetEye());fViewVector.Normalize();
    Vector<float4> fRightVector(fUpVector.Cross(fViewVector));fRightVector.Normalize();
    Vector<float4> fTranslate;

    // translate along the right vector
    if (fDelta.m_x != 0)
    {
      fTranslate.m_x = fRightVector.m_x * fDelta.m_x;
      fTranslate.m_y = fRightVector.m_y * fDelta.m_x;
      fTranslate.m_z = fRightVector.m_z * fDelta.m_x;
      pCamera->TranslateEyePoint(fTranslate);
      pCamera->TranslateReferencePoint(fTranslate);
      bModified = true;
    }
    // translate along the up vector
    if (fDelta.m_y != 0)
    {
      fTranslate.m_x = fUpVector.m_x * fDelta.m_y;
      fTranslate.m_y = fUpVector.m_y * fDelta.m_y;
      fTranslate.m_z = fUpVector.m_z * fDelta.m_y;
      pCamera->TranslateEyePoint(fTranslate);
      pCamera->TranslateReferencePoint(fTranslate);
      bModified = true;
    }
    SetLastMousePosition(mousePosition);
  }
  else if (m_bRotate == true)
  {
    SetRestorePoint();

    // calculate the center of rotation (at the center of the view - the intersection of
    // the camera view direction and the oblique plane)
    Point3Df centerOfRotation;
    if ( GetEnvironment().GetPlaneOblique()->GetIntersection( GetEnvironment().GetCamera()->GetViewReference(),
                                                              GetEnvironment().GetCamera()->GetViewDirection(),
                                                              centerOfRotation ) == false )
      return false;

    Point2D<int2> iDelta(GetMousePosition() - GetLastMousePosition());
    Normal<float4> upVector(pCamera->GetViewUp());
    Normal<float4> rightVector(pCamera->GetViewRight());
    Normal<float4> viewVector(pCamera->GetViewDirection());
    float fDistanceEyeToOblique(GetEnvironment().GetPlaneOblique()->GetDistance(pCamera->GetEye()));
    const float fRotationOnlyPadding(0.1F);
    float4 fWeightX(min(1.0f, fabs(Interpolate(0.0f, float4(GetMousePosition().X()), float4(GetEnvironment().GetWinRect().GetViewportSize().first -1), -1.0f, 1.0f))));
    float4 fWeightY(min(1.0f, fabs(Interpolate(0.0f, float4(GetMousePosition().Y()), float4(GetEnvironment().GetWinRect().GetViewportSize().second-1), -1.0f, 1.0f))));

    // rotate about the right vector and view vector
    //////////////////////////////////////////////////
    if (iDelta.m_y != 0)
    {
      float4 fSign(1);
      if (GetMousePosition().m_x > 0 && GetQuadrant() & 1)
      {
        fSign = -1;
      }

      float4 fAngle(Interpolate(0.0f, float4(iDelta.Y()), float4(GetEnvironment().GetWinRect().GetViewportSize().second-1), 0.0f, (float4)M_PI));
      float4 fFactor1(Max(0.0F, 1.0F-fRotationOnlyPadding-fWeightX));
      const float4 fFactor2(Min(1.0F, fWeightX + fRotationOnlyPadding));
      if (fWeightX >= (1.0F - fRotationOnlyPadding) || fWeightY >= (1.0F - fRotationOnlyPadding))
        fFactor1 = 0.0F;
      float4 fRightAngle(fFactor1*fAngle);
      float4 fViewAngle(-fFactor2*fAngle*fSign);
      pCamera->Rotate(rightVector, fRightAngle, centerOfRotation);
      pCamera->Rotate(viewVector, fViewAngle, centerOfRotation);
      bModified = true;
    }

    // rotate about the up vector and view vector
    ///////////////////////////////////////////////
    if (iDelta.m_x != 0)
    {
      float4 fSign(1);
      if (GetMousePosition().m_y > 0 && GetQuadrant() & 2)
      {
        fSign = -1;
      }

      float4 fAngle(Interpolate(0.0f, float4(iDelta.X()), float4(GetEnvironment().GetWinRect().GetViewportSize().first-1), 0.0f, (float)M_PI));
      float4 fFactor1(Max(0.0F, 1.0F-fRotationOnlyPadding-fWeightY));
      const float4 fFactor2(Min(1.0F, fWeightY+fRotationOnlyPadding));
      if (fWeightX >= (1.0F - fRotationOnlyPadding) || fWeightY >= (1.0F - fRotationOnlyPadding))
        fFactor1 = 0.0F;
      float4 fUpAngle(fFactor1*fAngle);
      float4 fViewAngle(fFactor2*fAngle*fSign);
      pCamera->Rotate(upVector, fUpAngle, centerOfRotation);
      pCamera->Rotate(viewVector, fViewAngle, centerOfRotation);
      bModified = true;
    } // delta x!=0?

    // adjust plane oblique relative to the camera
    if (bModified == true)
    {
      // (1) create a plane through origin
      // (2) orient this plane with its normal to the origin
      // (3) make plane go through center of roation
      Normal3Df normal(pCamera->GetViewReference()-pCamera->GetEye());
      vxPlaneOblique<float4> planeOblique(normal, centerOfRotation);
      *GetEnvironment().GetPlaneOblique() = planeOblique;
    } // modified?

    SetLastMousePosition(mousePosition);
  }// rotate 

  if (bModified == true)
  {
    GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
    GetEnvironment().GetPlaneOblique()->Modified(vxModInfo(vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED));
  }

 /* if ( m_pUndo != NULL )
  {
    vxEnvironment::GetUndoActions().PushAction( m_pUndo );
    m_pUndo = NULL;
  }*/

  return bModified;

} // OnMouseMove()


/**
 * Handle button up event
 * @param mouse mousePosition
 * @param mouse button
 * @param key state
 * @return handled?
 */
bool vxManipulatorPlaneOblique::OnButtonUp(const Point2D<int2> & mousePosition, 
                                           const ButtonEnum eButtonId, 
                                           const uint4 uKeyboardState)
{
  bool bHandled(false);

  if( m_bIsDraggingDistance || m_bRotate || m_bTranslate )
    GetEnvironment().SetUserInteracting( false );

  if (IsDragging() == true)
  {
    SetDragging(false);
    bHandled = true;
  }

  if (eButtonId == BUTTON_LEFT)
  {
    m_bRotate    = false; 
    m_bTranslate = false;
  }
  SetMousePosition(mousePosition);

  return bHandled;
} //OnButtonUp()


/**
 * Rotate
 * @param vector   vector to rotate around
 * @param fRadians angle to rotate
 */
void vxManipulatorPlaneOblique::Rotate(Normal<float4> vector,
                                       float4 fRadians)
{
  Point2D<int2> centerOfWindow(0.5*GetEnvironment().GetWinRect().GetViewportWidth(), 0.5*GetEnvironment().GetWinRect().GetViewportHeight());
  Point<float4> centerOfRotation;
  if (GetEnvironment().GetViewerType()->ConvertScreenToWorld(centerOfWindow, centerOfRotation) == false)
  {
    return;
  }

  float4 fSignDistance((GetEnvironment().GetPlaneOblique()->GetDistance() < 0.0F) ? -1.0F : 1.0F);
  GetEnvironment().GetCamera()->Rotate(vector, fRadians, centerOfRotation);

  Normal3Df normal(GetEnvironment().GetCamera()->GetViewReference()-GetEnvironment().GetCamera()->GetEye());
  vxPlaneOblique<float4> planeOblique(normal, centerOfRotation);

  *GetEnvironment().GetPlaneOblique() = planeOblique;
  
  CheckExtremeCases();

  GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
  GetEnvironment().GetPlaneOblique()->Modified(vxModInfo(vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED));
} // Rotate()


/**
 * Check for the extreme cases of the oblique plane (numerical problems)
 */
void vxManipulatorPlaneOblique::CheckExtremeCases()
{
  Vector3Df vector(GetEnvironment().GetPlaneOblique()->GetNormal());

  Box<float4> box(Box<float4>(Point<float4>(0,0,0), Point<float4>(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim())));
  vxPlaneOblique<float4> * pPlaneOblique(GetEnvironment().GetPlaneOblique());
  float4 fMin(vxMathUtils::GetMinDistance(box, *pPlaneOblique));
  float4 fMax(vxMathUtils::GetMaxDistance(box, *pPlaneOblique));

  // is the plane outside the volume (now after the last operation) and the normal almost axis aligned?
  if (fMin * fMax > 0.0F && fabs(vector.GetMaxAbsComponent()) >= 0.999F)
  {
    Triplef units(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVoxelUnits());
    float4 fUnitDistance(Min(Min(units[0], units[1]), units[2]));
    const float fEpsilon(0.001F*fUnitDistance);
    float4 fAbsMin(Min(fabs(fMin), fabs(fMax)));

    // let's see whether we got slightly outside and should align
    // it back with the first or last slice of the ortho view
    if (fAbsMin < fEpsilon)
    {
      LogDbg("snapping plane inside the volume", "", "");
      const int4 iIndex(vector.GetMaxAbsComponentCoord());

      //LogDbg("distance is (before): " + ToAscii(pPlaneOblique->GetDistance()), "", "");
      float4 fSign = (pPlaneOblique->GetDistance() < 0.0F) ? -1.0F : 1.0F;

      if (fabs(pPlaneOblique->GetDistance()) < fEpsilon)
        pPlaneOblique->SetDistance(0.0F);
      else
        pPlaneOblique->SetDistance(fSign * GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[iIndex]);

      LogDbg("snapping normal to axis aligned plane", "", "");
      vector[iIndex]       = (vector.GetMaxAbsComponent() < 0.0F) ? -1.0F : 1.0F;
      vector[(iIndex+1)%3] = 0.0F;
      vector[(iIndex+2)%3] = 0.0F;
      GetEnvironment().GetPlaneOblique()->SetNormal(Normal3Df(vector[0], vector[1], vector[2]));
      //LogDbg("normal (after): " + GetEnvironment().GetPlaneOblique()->GetNormal().ToString(), "", "");
    }
  }

  return;
}


/**
 * Returns in which quadrant the current mouse position is in
 * @return uint1 the quadrant
 *
 *              +----------->X
 *              |  0 | 1
 *              | ___|___
 *              |    |
 *              |  2 | 3
 *              |
 *              Y 
 */
uint1 vxManipulatorPlaneOblique::GetQuadrant()
{
  Point2D<uint2> uCurPos(GetMousePosition());
  Point2D<uint2> uWindow(GetEnvironment().GetWinRect().GetViewportSize().first, 
                         GetEnvironment().GetWinRect().GetViewportSize().second);
  Point2D<float4> fHalfWindow;
  fHalfWindow.m_x = float4(GetEnvironment().GetWinRect().GetViewportSize().first) * 0.5F;
  fHalfWindow.m_y = float4(GetEnvironment().GetWinRect().GetViewportSize().second) * 0.5F;
  uint1 uQuadrant(0);

  if (uCurPos.m_x > fHalfWindow.m_x)
    uQuadrant += 1;
  if (uCurPos.m_y > fHalfWindow.m_y)
    uQuadrant += 2;

  return uQuadrant;
} // GetQuadrant()


/**
 * set a restore point
 */
void vxManipulatorPlaneOblique::SetRestorePoint()
{

 // if ( m_pUndo != NULL )
  //  delete m_pUndo;

 // m_pUndo = new vxUndoActionPlaneOblique( GetEnvironment().GetPlaneOblique(), GetEnvironment().GetCamera() );

} // SetRestorePoint()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorPlaneOblique.C,v $
// Revision 1.18.2.1  2008/11/10 22:11:44  gdavidson
// Issue #6187: Don't set plane modified on zoom
//
// Revision 1.18  2007/06/07 18:27:54  gdavidson
// Mark plane as modified when zooming
//
// Revision 1.17  2007/02/16 20:32:10  geconomos
// + issue #5414:  fixed zooming
// + not allowing translation for "vessel detail " views
//
// Revision 1.16  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.15  2007/01/31 15:41:36  romy
// commented out UndoActions for Mouse Scroll operations.
//
// Revision 1.14  2007/01/16 17:04:06  gdavidson
// Removed OnTimer
//
// Revision 1.13  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.12  2006/08/03 14:01:35  geconomos
// removed inclusion of "vxRenderEngineData.h"
//
// Revision 1.11  2006/07/07 13:34:37  geconomos
// set "UserInteracting" flag in associated environment as needed
//
// Revision 1.10  2006/06/27 20:13:23  geconomos
// setting "user interacting" flag in environment accordingly
//
// Revision 1.9  2006/02/24 16:57:00  frank
// added undo information to oblique plane manipulation
//
// Revision 1.8  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.7  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.6.2.5  2005/08/24 20:02:42  frank
// Issue #4385: Cine looped in the wrong direction for oblique
//
// Revision 1.6.2.4  2005/08/01 19:12:07  frank
// changed function name according to coding standards
//
// Revision 1.6.2.3  2005/07/29 18:20:17  frank
// Issue #4318: Added standard orientations button to oblique view
//
// Revision 1.6.2.2  2005/07/29 16:27:04  frank
// removed dead code
//
// Revision 1.6.2.1  2005/07/29 14:35:40  frank
// Issue #4318: Added standard views to oblique view
//
// Revision 1.6  2005/05/23 12:21:24  frank
// fixed oblique rotation through the pan/zoom window
//
// Revision 1.5  2005/05/18 15:06:48  frank
// cleaning up slice shadows
//
// Revision 1.4  2001/01/03 19:30:23  michael
// "Replaced" ActiveViewerType by ActiveEnvironment.
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
// Revision 1.34  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.33  2003/12/16 19:05:49  frank
// Added additional key definitions
//
// Revision 1.32  2003/11/18 15:29:57  frank
// Fixed problem with slice shadows in 3D
//
// Revision 1.31  2003/11/12 13:11:09  michael
// simplified check to see if plane is close to being aligned with first or last ortho slice
//
// Revision 1.30  2003/11/06 02:58:04  michael
// Fixed display of slice number and number of total slices for the oblique MPR,
// added text to be displayed if after rotation the slice is outside the volume,
// and added mechanism so that axis aligned obliques can reach the very first
// and very last slice/facs of volume (numerical precision problem).
//
// Revision 1.29  2003/11/04 15:50:09  michael
// took out duplicated increment increase due to SHIFT/CTRL
//
// Revision 1.28  2003/10/03 16:31:28  michael
// fixed rotation so that 10% of the border (on each side) are for rotation only
//
// Revision 1.27  2003/09/29 17:20:12  michael
// not camera but plane is modified!!!
//
// Revision 1.26  2003/09/25 03:06:49  frank
// Merged oblique views into one
//
// Revision 1.29  2003/09/11 20:37:56  frank
// Updated the slice shadow data after modifying it
//
// Revision 1.28  2003/09/04 17:29:44  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.27  2003/08/20 15:40:40  michael
// made scaling factor for zoom to be dependent on the volume units
//
// Revision 1.26  2003/08/20 14:20:27  michael
// renamed degrees to radians
//
// Revision 1.25.2.1  2003/08/21 19:22:16  michael
// added "mailto:"
//
// Revision 1.25  2003/07/21 18:38:29  frank
// Issue #3351: Fixed zooming in/out
//
// Revision 1.24  2003/07/07 13:22:12  frank
// Added key multipliers back to zoom
//
// Revision 1.23  2003/07/01 13:44:39  frank
// Issue #3351: Fixed so that you can zoom out again after zooming in
//
// Revision 1.22  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.21  2003/06/18 15:19:56  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.20  2003/05/29 19:16:01  michael
// took out log message
//
// Revision 1.19  2003/05/23 15:45:48  michael
// minor changes
//
// Revision 1.18  2003/05/21 17:53:04  frank
// zoomed faster
//
// Revision 1.17  2003/05/15 20:07:52  frank
// Moving away from old point class
//
// Revision 1.16  2003/05/07 13:17:30  michael
// initial code review
//
// Revision 1.15  2003/04/29 14:12:57  frank
// Moved projection functionality to the viewer type
//
// Revision 1.14  2003/03/31 13:50:57  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.13  2003/03/25 21:11:25  michael
// fixed issue #2863
//
// Revision 1.12  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.11  2003/01/17 17:39:40  michael
// text of annotations can now be selected and moved
//
// Revision 1.10  2002/12/17 14:38:55  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.9  2002/11/24 21:54:57  ingmar
// removed compiler warnings
//
// Revision 1.8  2002/11/21 23:27:24  michael
// added SHIFT/CTRL acceleration for zoom
//
// Revision 1.7  2002/11/21 22:52:39  michael
// fixed min/max issues
//
// Revision 1.6  2002/11/20 21:15:33  michael
// fixed inverted translation
//
// Revision 1.5  2002/11/20 19:51:51  michael
// added OnTimer method
//
// Revision 1.4  2002/11/20 17:45:23  michael
// enabled plane dragging
//
// Revision 1.3  2002/11/19 22:48:29  michael
// added zoom in and out for oblique view
//
// Revision 1.2  2002/11/19 22:17:42  michael
// added arrow keys to plane oblique manipulation
//
// Revision 1.1  2002/11/19 21:21:24  michael
// Initial checkin of the oblique manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorPlaneOblique.C,v 1.18.2.1 2008/11/10 22:11:44 gdavidson Exp $
// $Id: vxManipulatorPlaneOblique.C,v 1.18.2.1 2008/11/10 22:11:44 gdavidson Exp $
