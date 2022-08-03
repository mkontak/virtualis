// $Id: vxControllerCameraFly.C,v 1.4 2007/03/07 18:06:10 geconomos Exp $
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
#include "vxControllerCameraFly.h"
#include "vxEnvironment.h"
#include "vxCameraFlySettings.h"
#include "vxUtils.h"


// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxControllerCameraFly::vxControllerCameraFly( vxEnvironment & environment ) :
  vxController(environment),
  m_bIsFlying( false ),
  m_bFlyingForward( false ),
  m_bIsAutoFlyMode( false )
{
  vxCamera * pCamera( GetEnvironment().GetCamera() );

  vxSubmarine & submarine = GetEnvironment().GetCameraFlySettings()->GetSubmarine();
  submarine.SetPositionWorld( pCamera->GetEye() );
  submarine.SetOrientation( pCamera->GetViewDirection() );
  submarine.SetUpDirection( pCamera->GetViewUp() );

  pCamera->SetProjectionType( vxCamera::PERSPECTIVE );
} // vxControllerCine2D()


/**
 * determine if processing is necessary
 *
 * @return if processing is necessary
 */
bool vxControllerCameraFly::IsProcessingNecessary()
{
  bool bIsProcessingNecessary( false );
  if( GetEnvironment().GetCameraFlySettings()->IsEnabled() )
  {
    const float8 delta = 1.0F / GetEnvironment().GetCameraFlySettings()->GetFramesPerSecond();
    if( m_timer.ElapsedTime() > delta )
    {
      bIsProcessingNecessary = true;
      m_timer.Reset();
    }
  }
  return bIsProcessingNecessary;
} // IsProcessingNecessary()


/**
 * Process the data
 */
void vxControllerCameraFly::Process()
{
  // check the state of the alt key
  GetEnvironment().GetCameraFlySettings()->SetFlyingForward(( GetAsyncKeyState(VK_MENU) & 0x8000 ) == false );

  GetEnvironment().GetCameraFlySettings()->UpdateSubmarine( GetEnvironment() );
} // Process()


// $Log: vxControllerCameraFly.C,v $
// Revision 1.4  2007/03/07 18:06:10  geconomos
// code review
//
// Revision 1.3  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.2  2007/01/16 19:27:58  geconomos
// Modified setting of "user interacting" flag
//
// Revision 1.1  2007/01/16 16:27:12  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerCameraFly.C,v 1.4 2007/03/07 18:06:10 geconomos Exp $
// $Id: vxControllerCameraFly.C,v 1.4 2007/03/07 18:06:10 geconomos Exp $

