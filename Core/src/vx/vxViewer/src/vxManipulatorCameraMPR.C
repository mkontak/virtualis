// $Id: vxManipulatorCameraMPR.C,v 1.10 2006/08/03 14:01:35 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorCameraMPR.h"
#include "vxEnvironment.h"
#include "vxPlaneOrtho.h"


// defines
#define FILE_REVISION "$Revision: 1.10 $"


/**
 * default constructor
 * @param environment the environment
 */
vxManipulatorCameraMPR::vxManipulatorCameraMPR(vxEnvironment & environment)
: vxManipulator( environment )
{  
} // vxManipulatorCameraMPR()


/**
 * Event handler if key is pressed
 * @param uKeyId         the key identifier
 * @param uKeyboardState the keyboard state
 * @return handled?
 */ 
bool vxManipulatorCameraMPR::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{
  bool bHandled( false );

  switch ( uKeyId )
  {

  case vxInput::KEY_PLUS_KEYPAD:
  case vxInput::KEY_EQUALS:
    ZoomCamera( false );
    bHandled = true;
    break;

  case vxInput::KEY_MINUS_KEYPAD:
  case vxInput::KEY_MINUS:
    ZoomCamera( true );
    bHandled = true;
    break;

  case vxInput::KEY_A:
    GetEnvironment().GetViewerType()->InitializeView();
    GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
    bHandled = true;
    break;

  case vxInput::KEY_ARROW_LEFT:
    PanCamera( Point2D<int2>( -1, 0 ) );
    break;

  case vxInput::KEY_ARROW_RIGHT:
    PanCamera( Point2D<int2>( 1, 0 ) );
    break;

  case vxInput::KEY_ARROW_UP:
    PanCamera( Point2D<int2>( 0, -1 ) );
    break;

  case vxInput::KEY_ARROW_DOWN:
    PanCamera( Point2D<int2>( 0, 1 ) );
    break;

  default:
    break;
  }

  return bHandled;

} // OnKeyPressed()


/**
 * zoom in or out
 *
 * @param bZoomIn if zooming in
 */
void vxManipulatorCameraMPR::ZoomCamera( bool bZoomIn )
{

  vxWinRect winRect(GetEnvironment().GetWinRect());
  uint2 uAxisX(GetEnvironment().GetCamera()->GetViewRight().GetMaxAbsComponentCoord());
  uint2 uAxisY(GetEnvironment().GetCamera()->GetViewUp().GetMaxAbsComponentCoord());
  float4 fWidthX(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisX]);
  float4 fHeightY(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisY]);
  float4 fAspectRatioVolume(fHeightY/fWidthX);
  float4 fAspectRatioWindow((float4)winRect.GetViewportHeight()/(float4)winRect.GetViewportWidth());
  float4 fWidth(GetEnvironment().GetZoomFactor()->GetSize());
  float4 fWidthMax(max(fWidthX, fHeightY));

  int4 iDirection( bZoomIn ? 1 : -1 );
  float4 fDirection(5.0F*static_cast<float4>(iDirection));

  float4 fWidthCopy(fWidth);
  fWidth += fDirection * 0.01F * fWidthMax;

  if (fWidth > fWidthMax)
  {
    fWidth = fWidthMax;
  }
  if (fWidth <= 0.01F * fWidthMax)
  {
    fWidth = fWidthCopy;
  }

  GetEnvironment().GetZoomFactor()->SetSize(fWidth);
  GetEnvironment().GetZoomFactor()->Modified(vxModInfo(vxZoomFactor, 0));

} // ZoomCamera()


/**
 * pan the camera around on the plan
 *
 * @param direction the direction to move the camera (-1,0,1)
 */
void vxManipulatorCameraMPR::PanCamera( const Point2D<int2> & direction )
{

  //
  // Get a point in the center of the window then try to offset it and reset the image about it.
  //

  Point2D<int2> centerPoint( GetEnvironment().GetWinRect().GetViewportWidth() / 2, GetEnvironment().GetWinRect().GetViewportHeight() / 2 );
  const int2 iMinWindowSize( Min( GetEnvironment().GetWinRect().GetViewportWidth(), GetEnvironment().GetWinRect().GetViewportHeight() ) );

  // move the one-fifth of the window at a time
  const int2 iPanDistancePixels( Max( 1, iMinWindowSize / 5 ) );
  centerPoint += direction * iPanDistancePixels;

  // convert to world coordinates and clamp to volume
  Point3Df world;
  GetEnvironment().GetViewerType()->ConvertMouseToWorld( centerPoint, world );
  Triple<float4> volDim( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim() );
  for ( int4 i(0); i<3; i++ )
  {
    world[i] = Bound( 0.0F, world[i], volDim[i] );
  }

  GetEnvironment().GetZoomFactor()->SetCenter( world );
  GetEnvironment().GetZoomFactor()->Modified( vxModInfo( vxZoomFactor, vxUtils::ZOOMFACTOR_MODIFIED ) );
  
} // PanCamera()


#undef FILE_REVISION


// $Log: vxManipulatorCameraMPR.C,v $
// Revision 1.10  2006/08/03 14:01:35  geconomos
// removed inclusion of "vxRenderEngineData.h"
//
// Revision 1.9  2006/06/27 20:13:23  geconomos
// setting "user interacting" flag in environment accordingly
//
// Revision 1.8  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.7  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.6.2.2  2005/02/08 15:34:51  frank
// added plus/minus key on standard keyboard in addition to numeric keypad
//
// Revision 1.6.2.1  2005/02/07 13:49:23  frank
// comments
//
// Revision 1.6  2004/10/20 20:03:34  frank
// added pan with arrow keys on MPRs
//
// Revision 1.5  2004/04/27 14:10:27  frank
// added +/- in ortho view
//
// Revision 1.4  2004/04/26 19:00:16  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.3  2004/04/06 16:24:58  frank
// unified environment access naming
//
// Revision 1.2  2004/03/02 19:19:35  frank
// renamed method in viewer type to more accurately describe it
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2004/01/20 21:29:28  frank
// commented out zooming
//
// Revision 1.1  2004/01/20 20:07:19  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorCameraMPR.C,v 1.10 2006/08/03 14:01:35 geconomos Exp $
// $Id: vxManipulatorCameraMPR.C,v 1.10 2006/08/03 14:01:35 geconomos Exp $
