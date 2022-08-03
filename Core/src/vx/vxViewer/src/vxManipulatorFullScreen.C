// $Id: vxManipulatorFullScreen.C,v 1.1.2.2 2009/02/27 15:40:52 gdavidson Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorFullScreen.h"
#include "vxEnvironment.h"
#include "vxFullScreen.h"


// defines
#define FILE_REVISION "$Revision: 1.1.2.2 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorFullScreen::vxManipulatorFullScreen( vxEnvironment & environment ) :  vxManipulator(environment)
{
} // constructor


/**
 * Handle the button down event
 * @param position       mouse position
 * @param eButtonId      mouse button
 * @param uKeyboardState keyboard state
 * @return handled?
 */
bool vxManipulatorFullScreen::OnDoubleClick( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState )
{
  bool bResult( false );

  if( eButtonId == BUTTON_LEFT )
  {
    vxShareableObject< vxFullScreen > * pFullScreen = dynamic_cast< vxShareableObject< vxFullScreen > * >( GetEnvironment().GetElement( vxIDs::FullScreen ));
    const int4 iProportion = Bound( 0, pFullScreen->GetProportion(), 100 );
    float4 iWidth = ( iProportion * GetEnvironment().GetWinRect().GetWindowSize().first ) / 100;
    float4 iHeight = ( iProportion * GetEnvironment().GetWinRect().GetWindowSize().second ) / 100;

    if( position.m_x < iWidth && position.m_y < iHeight )
    {
      //if( m_bEnabled || GetEnvironment().GetWinRect().GetBound() == vxWinRect::LOWER_RIGHT )
      {
        pFullScreen->SetEnvironment( &GetEnvironment() );
        pFullScreen->Modified( vxModInfoID( vxElement, vxIDs::FullScreen, 0 ));
        bResult = true;
      }
    }
  }

  return bResult;
} // OnDoubleClick( position, eButtonId, uKeyboardState )


// $Log: vxManipulatorFullScreen.C,v $
// Revision 1.1.2.2  2009/02/27 15:40:52  gdavidson
// Reworked implementation of full screen
//
// Revision 1.1.2.1  2008/11/24 17:11:03  gdavidson
// Issue #6171: Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorFullScreen.C,v 1.1.2.2 2009/02/27 15:40:52 gdavidson Exp $
// $Id: vxManipulatorFullScreen.C,v 1.1.2.2 2009/02/27 15:40:52 gdavidson Exp $
