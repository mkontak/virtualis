// $Id: vxControllerMomentum.C,v 1.3 2007/03/07 18:06:10 geconomos Exp $
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
#include "vxControllerMomentum.h"
#include "Box.h"
#include "vxUtils.h"
#include "vxEnvironment.h"
#include "vxRenderingMode.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxControllerMomentum::vxControllerMomentum(vxEnvironment & environment) :
  vxController(environment)
{
} // vxControllerMomentum()


/**
 * determine if processing is necessary
 *
 * @return if processing is necessary
 */
bool vxControllerMomentum::IsProcessingNecessary()
{
  if( GetEnvironment().GetMomentum()->GetInitiated() == false || GetEnvironment().GetMomentum()->GetEnabled() == false )
    return false;

  bool bIsProcessingNecessary( false );
  const float8 delta = 1.0F / GetEnvironment().GetMomentum()->GetFramesPerSecond();
  if( m_timer.ElapsedTime() > delta )
  {
    bIsProcessingNecessary = true;
    m_timer.Reset();
  }
  return bIsProcessingNecessary;
} // IsProcessingNecessary()


/**
 * Process the data
 */
void vxControllerMomentum::Process()
{
  GetEnvironment().GetCamera()->Rotate( GetEnvironment().GetCamera()->GetViewRight(), GetEnvironment().GetMomentum()->GetRightAngle() );
  GetEnvironment().GetCamera()->Rotate( GetEnvironment().GetCamera()->GetViewDirection(), GetEnvironment().GetMomentum()->GetViewDirectionRightAngle() );
  GetEnvironment().GetCamera()->Rotate( GetEnvironment().GetCamera()->GetViewUp(), GetEnvironment().GetMomentum()->GetUpAngle() );
  GetEnvironment().GetCamera()->Rotate( GetEnvironment().GetCamera()->GetViewDirection(), GetEnvironment().GetMomentum()->GetViewDirectionUpAngle() );
  GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
} // Process()



// $Log: vxControllerMomentum.C,v $
// Revision 1.3  2007/03/07 18:06:10  geconomos
// code review
//
// Revision 1.2  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.1  2007/01/16 16:27:12  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerMomentum.C,v 1.3 2007/03/07 18:06:10 geconomos Exp $
// $Id: vxControllerMomentum.C,v 1.3 2007/03/07 18:06:10 geconomos Exp $

