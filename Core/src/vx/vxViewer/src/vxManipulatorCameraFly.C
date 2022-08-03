// $Id: vxManipulatorCameraFly.C,v 1.15 2007/02/05 15:29:25 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Ingmar Bitter (ingmar@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorCameraFly.h"
#include "vxCamera.h"
#include "vxEnvironment.h"
#include "vxModifiedQueue.h"
#include "Image.h"
#include "vxCursor3d.h"
#include "vxDataRendererSoftware.h"
#include "vxCameraFlySettings.h"


// defines
#define FILE_REVISION "$Revision: 1.15 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorCameraFly::vxManipulatorCameraFly(vxEnvironment & environment):
  vxManipulatorCamera(environment)
{
  GetEnvironment().GetCameraFlySettings()->InitializeSubmarine( GetEnvironment() );

  GetEnvironment().GetCamera()->SetProjectionType(vxCamera::PERSPECTIVE);
} // vxManipulatorCameraFly()


/**
 * Handle the mouse scroll events -> translate into zoom in and out
 * @param mouse position
 * @param delta
 * @param key state
 * @return handled?
 */
bool vxManipulatorCameraFly::OnMouseWheel( const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState )
{
  bool bHandled(false);

  vxCamera * pCamera( GetEnvironment().GetCamera() );
  Vector<float4> viewVector = Vector<float4>( pCamera->GetViewDirection() );
  viewVector.Normalize();
  Vector<float4> deltaVector( viewVector * ( -0.0025 * iDelta * GetKeyMultiplier( uKeyboardState )));
  pCamera->TranslateEyePoint( deltaVector );
  pCamera->TranslateReferencePoint( deltaVector );

  GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ));

  return bHandled;
} // OnMouseWheel()


/**
 * Handle the mouse move -> translate into move forward + rotate
 * @param mouse position
 * @param key state
 * @return handled?
 */
bool vxManipulatorCameraFly::OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState )
{
  bool bHandled( false );

  // only do somethig on left mouse button down (with or without ALT modifier)
  // return if we do not do handle this key/mouse combination
  if( GetEnvironment().GetCameraFlySettings()->IsEnabled() && ( uKeyboardState & STATE_LBUTTON ))
    bHandled = true;
  else
    return bHandled;

  SetMousePosition( position );
  SetLastMousePosition( position );

  UpdateCameraFlySettings( position, uKeyboardState );

  return true;
} // OnMouseMove()

/**
 * Handle the button down event -> setup submarine and start flying
 * @param mouse position
 * @param mouse button
 * @param key state
 * @return handled?
 */
bool vxManipulatorCameraFly::OnButtonDown( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState )
{
  if( ! ( uKeyboardState & STATE_LBUTTON ))
    return false;
  
  SetMousePosition(position);
  SetLastMousePosition(position);

  GetEnvironment().GetCameraFlySettings()->InitializeSubmarine( GetEnvironment() );
  GetEnvironment().GetCameraFlySettings()->GetSubmarine().StartFlying();

  UpdateCameraFlySettings( position, uKeyboardState );

  GetEnvironment().GetCameraFlySettings()->SetAutoFlyMode( false );
  GetEnvironment().GetRenderingMode()->SetEnabled( vxRenderingMode::AUTO_FLY_MODE, false );
  GetEnvironment().GetCameraFlySettings()->SetEnabled( true );

  GetEnvironment().SetUserInteracting( true );

  return true;
} // OnButtonDown()


/**
 * Handle button up event -> stop submarine from moving
 * @param mouse position
 * @param mouse button
 * @param key state
 * @return handled?
 */
bool vxManipulatorCameraFly::OnButtonUp(const Point2D<int2> & position, 
                                        const ButtonEnum eButtonId, 
                                        const uint4 uKeyboardState)
//*******************************************************************
{
  if( GetEnvironment().GetCameraFlySettings()->IsEnabled() == false ) 
    return false;

  GetEnvironment().GetCameraFlySettings()->SetEnabled( false );

  // now synchronizwe 2D views
  *GetEnvironment().GetCursor3d() = vxCursor3d( *GetEnvironment().GetCamera() );
  GetEnvironment().GetCursor3d()->Modified( vxModInfo( vxCursor3d, vxUtils::CURSOR3D_MODIFIED ));

  GetEnvironment().GetCameraFlySettings()->GetSubmarine().StopFlying();

  GetEnvironment().SetUserInteracting( false );

  return true;
} //OnButtonUp()


/**
 * Handle the keyboard events
 * @param key
 * @param key state
 * @return handled?
 */
bool vxManipulatorCameraFly::OnKeyPressed(const uint1 uKeyId, 
                                          const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);

  switch (uKeyId)
  {
  // toggle Projection type
  case vxInput::KEY_C:
    {
      ToggleProjectionType();
      bHandled = true;
      break;
    }
  case vxInput::KEY_W:
    // auto fly
    {
/*
      m_bIsAutoFlyMode = true;
      m_bIsFlying = true;
      GetEnvironment().GetRendererCpu3dData()->SetAutoFlyMode(true);

      GetEnvironment().GetEventTimer()->SetTimer("FLY", 1);
      GetEnvironment().GetEventTimer()->Modified(vxModInfoTxt(vxEventTimer, vxUtils::EVENTTIMER_STARTED, "FLY"));
      vxModifiedQueue::Flush();

      vxCamera * pCamera(GetEnvironment().GetCamera());
      m_submarine.SetPositionWorld(pCamera->GetEye());
      m_submarine.SetOrientation(pCamera->GetViewDirection());
      m_submarine.SetUpDirection(pCamera->GetViewUp());
      m_submarine.StartFlying();
      Vector3D<float4> forceVec, panVec;
      GetForcesFromDepthImage(forceVec, panVec);
      UpdateCamera(forceVec, panVec);
*/
    }
  default:
    // not handled
    break;
  }

  // perform the standard camera manipulations
  bHandled |= ManipulateStandardViews( uKeyId, uKeyboardState );

  return bHandled;
} // OnKeyPressed()


/**
 * Updates the cameraFlySettings object
 *
 * @param     position          position
 * @param     uKeyboardState    state
 */
void vxManipulatorCameraFly::UpdateCameraFlySettings( const Point2D<int2> & position, const uint4 uKeyboardState )
{
  // convert mouse input into forces on submarine
  Vector<float4> direction;
  direction.m_x = Interpolate( 0.0f , float4( position.X() ), float4( GetEnvironment().GetWinRect().GetViewportSize().first-1 ), -1.0f, 1.0f );
  direction.m_y = Interpolate( 0.0f , float4( position.Y() ), float4( GetEnvironment().GetWinRect().GetViewportSize().second-1 ), -1.0f, 1.0f );
  direction.m_z = 1 - min( 1, fabs( direction.X() + fabs(direction.Y() )));
  
  // fly backward?
  GetEnvironment().GetCameraFlySettings()->SetFlyingForward( ! ( uKeyboardState & STATE_ALT ));

  Vector3Df forceVector; 
  Vector3Df panVector;
  forceVector.X() =  0.5 * direction.Z() * GetKeyMultiplier( uKeyboardState );
  forceVector.Y() = -0.5 * direction.X();
  forceVector.Z() =  0.5 * direction.Y();

  GetEnvironment().GetCameraFlySettings()->SetForceVector( forceVector );
  GetEnvironment().GetCameraFlySettings()->SetPanVector( panVector );

  GetEnvironment().GetCameraFlySettings()->UpdateSubmarine( GetEnvironment() );
} // UpdateCameraFlySettings( position, uKeyboardState )


/** 
 * Compute the forces for the automatic camera steering from the depth image
 * @param force vector
 * @param pan vector
 */
void vxManipulatorCameraFly::GetForcesFromDepthImage(Vector3D<float4> & forceVector,
                                                     Vector3D<float4> & panVector)
//*******************************************************************
{

  // get the depth image from the environment
  Image<float4> & depthImage( const_cast< vxRenderBuffer &  >( GetEnvironment().GetRenderBuffer() ).GetDepthImageVC() ) ;

  // select every 8th depth pixel, as we know that tose are actually computed
  Image<float4> smallImg(Triple<uint4>(depthImage.GetDim().X()/8, (depthImage.GetDim().Y()-2)/8, 0),0.0f);

  // create the image
  for (int4 y(0); y<smallImg.GetDim().Y(); ++y)
  {
    for (int4 x(0); x<smallImg.GetDim().X(); ++x)
    {
      const float4 fDepth(depthImage.GetPixel(Point2D<uint2>(8*x, 8*y)));
      smallImg.SetPixel(Point2D<uint2>(x,y),fDepth);
    } // for all pixels
  } // for all scanlines

  // blur to the positive direction
  for (int4 y(0); y<smallImg.GetDim().Y()-1; ++y)
  {
    for (int4 x(0); x<smallImg.GetDim().X()-1; ++x)
    {
      smallImg.SetPixel(Point2D<uint2>(x,y), (0.25*smallImg.GetPixel(Point2D<uint2>(x  ,y)) + 
                                              0.25*smallImg.GetPixel(Point2D<uint2>(x+1,y)) + 
                                              0.25*smallImg.GetPixel(Point2D<uint2>(x  ,y+1)) + 
                                              0.25*smallImg.GetPixel(Point2D<uint2>(x+1,y+1))));
    } // for all pixels
  } // for all scanlines

  // blur to the negative direction
  for (int4 y(smallImg.GetDim().Y()-1); y>0; --y)
  {
    for (int4 x(smallImg.GetDim().X()-1); x>0; --x)
    {
      smallImg.SetPixel(Point2D<uint2>(x,y), (0.25*smallImg.GetPixel(Point2D<uint2>(x  ,y)) + 
                                              0.25*smallImg.GetPixel(Point2D<uint2>(x-1,y)) + 
                                              0.25*smallImg.GetPixel(Point2D<uint2>(x  ,y-1)) + 
                                              0.25*smallImg.GetPixel(Point2D<uint2>(x-1,y-1))));
    } // for all pixels
  } // for all scanlines

  // read depth pixels and record which is deepest
  float4 fMaxDepth(0.0F);
  int4 iMaxPosX(smallImg.GetDim().X()/2); 
  int4 iMaxPosY(smallImg.GetDim().Y()/2); 
  for (int4 y(0); y<smallImg.GetDim().Y()-1; ++y)
  {
    for (int4 x(0); x<smallImg.GetDim().X()-1; ++x)
    {
      const float4 fDepth(smallImg.GetPixel(Point2D<uint2>(x, y)));
      if (fDepth > fMaxDepth && fDepth < 0.9*(std::numeric_limits<float>::max)())
      {
        fMaxDepth = fDepth;
        iMaxPosX = x;
        iMaxPosY = y;
      }
    } // for all pixels
  } // for all scanlines

  // find image space gradient towards deepest pixel
  Vector3D<float4> gradient(0,0,0.01f);
  gradient.X() = (2.0f*iMaxPosX / smallImg.GetDim().X()) -1.0F; // range [-1, 1]
  gradient.Y() = (2.0f*iMaxPosY / smallImg.GetDim().Y()) -1.0F; // range [-1, 1]
  gradient.Z() = pow(min(1, fabs(gradient.X() + fabs(gradient.Y()))), 1); // range [0, 1]

  // get the distances from the wall (as aproximated by the depth image) (one off the max po sto get the smoothed pixels)
  float4 fLDepth(0.1 * smallImg.GetPixel(Point2D<uint2>(1,                       smallImg.GetDim().Y()/2))); // left
  float4 fRDepth(0.1 * smallImg.GetPixel(Point2D<uint2>(smallImg.GetDim().X()-2, smallImg.GetDim().Y()/2))); // right
  float4 fTDepth(0.1 * smallImg.GetPixel(Point2D<uint2>(smallImg.GetDim().X()/2, smallImg.GetDim().Y()-2))); // top
  float4 fBDepth(0.1 * smallImg.GetPixel(Point2D<uint2>(smallImg.GetDim().X()/2, 1                     ))); // bottom

  // convert it into the forces used for the submarine
  float4 fTightnessSlowdown(0.5 + 0.5*(1-exp(-(fRDepth+fLDepth)/1000.0F)) * (1-exp(-(fTDepth+fBDepth)/1000.0F)));
  forceVector.X() = (1 - gradient.Z()) * fTightnessSlowdown;
  forceVector.Y() = -0.5 * gradient.X();
  forceVector.Z() = -0.5 * gradient.Y();

  // now compute the vector for panning in order to push the camera away from the wall
  float4 fPanCorrectionX(Sign(fLDepth - fRDepth)*(fabs(fRDepth-fLDepth))/(fRDepth+fLDepth+0.1));
  float4 fPanCorrectionY(Sign(fBDepth - fTDepth)*(fabs(fBDepth-fTDepth))/(fBDepth+fTDepth+0.1));
  panVector.X() = fPanCorrectionX * fTightnessSlowdown; 
  panVector.Y() = fPanCorrectionY * fTightnessSlowdown;

  return;
} // GetForcesFromDepthImage()


#undef FILE_REVISION


// $Log: vxManipulatorCameraFly.C,v $
// Revision 1.15  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.14  2007/01/16 17:04:31  gdavidson
// Removed OnTimer
//
// Revision 1.13  2006/10/23 01:38:29  geconomos
// using two depth buffers for world and volume coordinates
//
// Revision 1.12  2006/09/14 20:07:53  geconomos
// corrected to use depth buffer from vxRenderBuffer
//
// Revision 1.11  2006/08/03 14:01:34  geconomos
// removed inclusion of "vxRenderEngineData.h"
//
// Revision 1.10  2006/07/07 13:34:37  geconomos
// set "UserInteracting" flag in associated environment as needed
//
// Revision 1.9  2006/06/27 20:13:23  geconomos
// setting "user interacting" flag in environment accordingly
//
// Revision 1.8  2006/01/27 15:47:45  geconomos
// still migrating to visual studio 2005
//
// Revision 1.7  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.6  2004/10/18 14:48:41  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.5  2004/10/05 14:30:33  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.4  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.3  2004/05/25 14:31:16  frank
// added base class for camera manipulators
//
// Revision 1.2  2004/04/06 16:24:46  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.37  2004/01/09 17:21:44  frank
// fixed hardcoded key values - replaced with enumeration
//
// Revision 1.36  2003/11/28 23:35:06  michael
// made mouse wheel step smaller (0.25 of what it was) since inside the dataset,
// it results in big jumps ... needs to be revisited after RNSA 2004
//
// Revision 1.35  2003/11/20 18:27:24  frank
// Centralized the camera manipulations
//
// Revision 1.34  2003/11/18 19:55:15  frank
// Made you fly backward on <alt>-<left mouse>
//
// Revision 1.33  2003/11/16 23:39:48  michael
// added ability to controle the rendering speed/quality ...
//
// Revision 1.32  2003/11/07 17:14:04  frank
// Added the ability to fly backward in the endoluminal view
//
// Revision 1.31  2003/11/05 13:40:21  frank
// Added the ability to fly backward
//
// Revision 1.30  2003/09/29 17:21:00  michael
// accounting for the new event flags in vxEnvironment
//
// Revision 1.29  2003/09/11 20:37:12  frank
// disabled momentum in the button rather than here
//
// Revision 1.28  2003/09/11 20:17:22  frank
// Issue #3479: Disabled momentum when you start camera or path fly
//
// Revision 1.27  2003/09/08 19:38:04  michael
// only updates 3D cursor after transiting from fly to stop, not while flying
//
// Revision 1.26  2003/07/15 17:09:45  frank
// Fixed endoluminal flight speed
//
// Revision 1.25  2003/06/19 20:21:21  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.24  2003/05/15 20:07:52  frank
// Moving away from old point class
//
// Revision 1.23  2003/05/07 11:44:01  michael
// initial code review
//
// Revision 1.22  2003/04/29 21:17:35  michael
// removed vxManipulatorCamera from inheritance
//
// Revision 1.21  2003/04/24 14:03:02  michael
// accounted for new Cursor3d handline (real class, not just triple)
//
// Revision 1.20  2003/03/19 19:34:26  frank
// Removed obsolete VIEW_CHANGING mode
//
// Revision 1.19  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.18  2003/02/18 20:17:55  frank
// Scaled the speed of flight by using the cine/momentum speed slider.
//
// Revision 1.17  2003/01/22 21:55:41  ingmar
// synchronized to new Point2D access to Image pixels
//
// Revision 1.16  2002/11/19 19:09:57  ingmar
// smoother autoflight
//
// Revision 1.15  2002/11/19 19:06:58  ingmar
// smoother autoflight
//
// Revision 1.14  2002/11/12 17:55:14  ingmar
// the change rates are now dependent on the local tube diameter
//
// Revision 1.13  2002/11/12 16:51:09  ingmar
// fine tuned the amounts to rotate and pan
//
// Revision 1.12  2002/11/12 16:32:06  ingmar
// added combination of pan and rotate to facilitate auto flight without centerline
//
// Revision 1.11  2002/11/12 00:31:52  ingmar
// smoother autoflight
//
// Revision 1.10  2002/11/11 22:45:37  ingmar
// first implementation of auto-fly mode
// also slight restructuring of Camera update code
// dropped non-submarine camera update code
//
// Revision 1.9  2002/11/11 16:35:54  ingmar
// upon instantiation this manipulator now sets the frame rate demand to high
//
// Revision 1.8  2002/11/08 19:00:23  ingmar
// moved the visible data bounding box into the environment to be accessable by the manip CamFly
//
// Revision 1.7  2002/11/08 17:19:46  ingmar
// added collision avoidance
//
// Revision 1.6  2002/11/07 16:27:30  ingmar
// made flying sensitive to only left mouse button
//
// Revision 1.5  2002/11/06 23:43:30  geconomos
// Added timer for flying.
//
// Revision 1.4  2002/11/05 22:52:08  ingmar
// 3DCursor -> Cursor3d
//
// Revision 1.3  2002/10/25 18:59:03  ingmar
// made sure we only stry to fly if the mouse button was pressed inside the viewer
//
// Revision 1.2  2002/10/24 23:04:00  ingmar
// now updating Cursor3d to correlate with 2D Views
//
// Revision 1.1  2002/10/21 23:19:30  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorCameraFly.C,v 1.15 2007/02/05 15:29:25 geconomos Exp $
// $Id: vxManipulatorCameraFly.C,v 1.15 2007/02/05 15:29:25 geconomos Exp $
