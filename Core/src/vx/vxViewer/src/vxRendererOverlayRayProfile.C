// $Id: vxRendererOverlayRayProfile.C,v 1.3 2005/11/18 21:15:41 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "vxRendererOverlayRayProfile.h"
#include "vxUtils.h"
#include "vxEnvironment.h"
#include "vxRayProfile.h"
#include "vxRendererOverlay.h"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxRendererOverlayRayProfile::vxRendererOverlayRayProfile( vxEnvironment & environment )
: vxRendererOverlay( environment )
{
} // vxRendererOverlayRayProfile()


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererOverlayRayProfile::IsRedrawNecessary()
{

  return GetEnvironment().IsModified( vxIDs::RayProfileData );

} // IsRedrawNecessary()


/**
 * Render 2D overlay.
 */  
void vxRendererOverlayRayProfile::Render()
{
  SetupRenderState();

  // Render the profile line if available
  vxOverlayStrings * pOverlayStrings( dynamic_cast< vxOverlayStrings * >(  GetEnvironment().GetElement( vxIDs::OverlayStrings ) ) );
  if ( GetEnvironment().GetElement( vxIDs::RayProfileData ) != NULL )
  {

    // get the ray profile data from the environment
    vxRayProfile * pProfileData = dynamic_cast<vxRayProfile *>( GetEnvironment().GetElement( vxIDs::RayProfileData ) );
    std::vector<float4> & profile( pProfileData->GetRayOpacityProfile() );

    pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( "profile size: " + ToAscii( profile.size() ), vxOverlayStrings::vxOverlayElement::MiddleLeft, Triple<float4>( 1, 1, 1 ) ) );

    const float4 fBottom(50);
    const float4 fHeight(100);

    glColor3f(1,1,1);
    glBegin(GL_LINE_STRIP);
    for ( int i(0); i<profile.size(); ++i )
    {
      const float4 x( Interpolate( 0.0F, float4(i), float4(profile.size()-1), 0.0F, float4(GetEnvironment().GetWinRect().GetViewportWidth()-1) ) );
      glVertex2f( x, profile[i] * fHeight + fBottom );
    }
  }
  else
  {
    pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( "Ray profile data is not found!", vxOverlayStrings::vxOverlayElement::MiddleMiddle, Triple<float4>( 1, 1, 0 ) ) );
  }
  glEnd();

  vxRendererOverlay::RestoreRenderState();

} // Render()


// Revision history:
// $Log: vxRendererOverlayRayProfile.C,v $
// Revision 1.3  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.6.1  2005/08/05 13:06:14  frank
// added rendering necessary function
//
// Revision 1.2  2004/10/23 10:23:34  frank
// made some functions not static
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/07/11 19:49:15  michael
// final code reviews
//
// Revision 1.7  2003/05/27 14:12:00  michael
// completed code review
//
// Revision 1.6  2003/05/27 13:18:42  frank
// code review
//
// Revision 1.5  2003/05/19 14:40:22  frank
// Removed unused return parameter
//
// Revision 1.4  2003/05/01 14:35:29  frank
// reflected parameter change in vxRendererOverlay
//
// Revision 1.3  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.2  2003/01/21 22:16:23  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.1  2001/01/09 12:56:10  frank
// Added ray profile data used for sampling along a ray for 3D picking.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlayRayProfile.C,v 1.3 2005/11/18 21:15:41 frank Exp $
// $Id: vxRendererOverlayRayProfile.C,v 1.3 2005/11/18 21:15:41 frank Exp $
