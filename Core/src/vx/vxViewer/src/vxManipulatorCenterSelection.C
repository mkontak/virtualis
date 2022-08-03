// $Id: vxManipulatorCenterSelection.C,v 1.1.2.1 2008/11/18 19:33:19 gdavidson Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorCenterSelection.h"
#include "vxEnvironment.h"
#include "vxViewerTypes.h"


// defines
#define FILE_REVISION "$Revision: 1.1.2.1 $"


/**
 * Constructor
 * @param environment the environment
 */
vxManipulatorCenterSelection::vxManipulatorCenterSelection( vxEnvironment & environment ) : vxManipulator(environment),
  m_bActive( false )
{
}


/**
 * Public method to process button down event
 * @param position mouse position
 * @param mouse button enum
 * @param state of keyboard
 * @return handled?
 */ 
bool vxManipulatorCenterSelection::OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState)
{
  m_bActive = false;
  if( eButtonId == BUTTON_LEFT )
  {
    vxShareableObject< vxCamera > * pCamera = GetEnvironment().GetCamera();
    Point3Df centerWC;
    if( GetEnvironment().GetPlaneOblique()->GetIntersection( pCamera->GetViewReference(), pCamera->GetViewDirection(), centerWC ) == false )
      return false;
      
    Point3Df pointWC;
    if( GetEnvironment().GetViewerType()->ConvertMouseToWorld( position, pointWC ) == false )
      return false;

    m_bActive = true;
    GetEnvironment().SetUserInteracting( true );
        
    // translate eye
    Vector3Df translateWC = pointWC - centerWC;
    pCamera->TranslateEyePoint( translateWC );
    pCamera->TranslateReferencePoint( translateWC );

    pCamera->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ));
  }

  return m_bActive;
} // OnButtonDown()



bool vxManipulatorCenterSelection::OnButtonUp( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState )
{
  if( m_bActive )
  {
    GetEnvironment().SetUserInteracting( false );
    GetEnvironment().GetInputFeedback().Add( vxFeedback( typeid( vxManipulatorCenterSelection ), vxFeedback::DONE ));
  }

  return m_bActive;
}


/**
 * Called when the mouse cursor is to be set
 *
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorCenterSelection::OnSetCursor()
{
  vxMouseCursor::SetCursor( "CURSOR_CROSSHAIR" );
  return true;
} // OnSetCursor()


// $Log: vxManipulatorCenterSelection.C,v $
// Revision 1.1.2.1  2008/11/18 19:33:19  gdavidson
// Issue #6203: Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorCenterSelection.C,v 1.1.2.1 2008/11/18 19:33:19 gdavidson Exp $
// $Id: vxManipulatorCenterSelection.C,v 1.1.2.1 2008/11/18 19:33:19 gdavidson Exp $
