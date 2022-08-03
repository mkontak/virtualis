// $Id: vxCameraFlySettings.C,v 1.1 2007/01/16 16:27:12 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxCameraFlySettings.h"
#include "vxEnvironment.h"
#include "vxCamera.h"
#include "vxUtils.h"


/**
 * default constructor
 */
vxCameraFlySettings::vxCameraFlySettings() :
  m_bIsAutoFlyMode( false ),
  m_bEnabled( false ),
  m_bIsFlyingForward( true ),
  m_iFramesPerSecond( 15 )
{
}


/**
 * intialize the submarine
 *
 *  @param       environment
 */
void vxCameraFlySettings::InitializeSubmarine( vxEnvironment & environment )
{
  vxCamera * pCamera( environment.GetCamera() );

  m_submarine.SetPositionWorld( pCamera->GetEye() );
  m_submarine.SetOrientation( pCamera->GetViewDirection() );
  m_submarine.SetUpDirection( pCamera->GetViewUp() );
} // InitializeSubmarine( environment )


/**
 * applies the force and pan vectors to the camera
 *
 * @param       environment
 */
void vxCameraFlySettings::UpdateSubmarine( vxEnvironment & environment )
{
  vxCamera * pCamera( environment.GetCamera() );
  float4 fThrustInhibit(1);

  if( environment.GetRenderingMode()->GetVisibleDataBox().IsInside( Triplef( pCamera->GetEye() )))
  {
    Image<float4> & depthImage( const_cast< vxRenderBuffer & >( environment.GetRenderBuffer() ).GetDepthImageVC() ) ;
    float4 fDepth( depthImage.GetPixel( Point2D<uint2>( depthImage.GetDim().X() / 2, ( depthImage.GetDim().Y() - 2 ) / 2 )));
    fThrustInhibit = 0.9F - exp( - fDepth / 10 );
  }
  m_forceVector.X() *= fThrustInhibit;

  // scale thrust by speed slider (currently the cine/momentum slider)
  const float4 fSpeedTimerMin( 50.0F );
  const float4 fSpeedTimerMax( 950.0F );
  //const float4 fSpeedTimer( Bound( fSpeedTimerMin, float4( vxEventTimer::GetElapseTime("CINE")), fSpeedTimerMax));
  const float4 fSpeedSlow( 0.1F );
  const float4 fSpeedFast( 4.0F );
  const float4 fSliderScale( Interpolate( 1, m_iFramesPerSecond, 30, fSpeedSlow, fSpeedFast ));
  m_forceVector.X() *= fSliderScale;

  m_submarine.SetForces( ( m_bIsFlyingForward ) ? m_forceVector.X() : -m_forceVector.X() , m_forceVector.Y(), m_forceVector.Z() );

  // move submarine
  m_submarine.UpdatePosition( m_panVector * fSliderScale );

  // update camera according to new submarine location and orientation
  pCamera->SetEye( m_submarine.GetPositionWorld() );
  pCamera->SetViewReference( Point3Df( m_submarine.GetPositionWorld() + Vector3Df( m_submarine.GetOrientation() )));
  pCamera->SetViewUp( m_submarine.GetUpDirection() );

  // update 3D cursor point
  environment.GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ));
} // UpdateSubmarine( environment )


// $Log: vxCameraFlySettings.C,v $
// Revision 1.1  2007/01/16 16:27:12  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxCameraFlySettings.C,v 1.1 2007/01/16 16:27:12 gdavidson Exp $
// $Id: vxCameraFlySettings.C,v 1.1 2007/01/16 16:27:12 gdavidson Exp $

