// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: vcUtils.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "vcUtils.h"
#include "FullScreenController.h"
#include "MainAppFrame.h"


USING_UTILITIES_NS


/**
 * Toggle full screen on
 */
void FullScreen::ToggleOn()
{
  Colon::FullScreenController::GetInstance().Notify( Colon::FullScreenArgs( true ));
} // ToggleOn()


/**
 * toggle full screen off
 */
void FullScreen::ToggleOff()
{
  Colon::FullScreenController::GetInstance().UpdateFullScreenViewerState( MainAppFrame::GetMainFrame()->GetCurrentLayout() );
  Colon::FullScreenController::GetInstance().Notify( Colon::FullScreenArgs( false ));
} // ToggleOff()