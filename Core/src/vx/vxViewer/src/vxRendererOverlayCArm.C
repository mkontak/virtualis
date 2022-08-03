// $Id: vxRendererOverlayCArm.C,v 1.3 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxRendererOverlayCArm.h"
#include "vxEnvironment.h"
#include "vxOverlayStrings.h"


// Defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxRendererOverlayCArm::vxRendererOverlayCArm(vxEnvironment & environment)
:vxRendererOverlay(environment)
{
} // vxRendererOverlayCArm()


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererOverlayCArm::IsRedrawNecessary()
{

  const bool bCameraChanged(
    GetEnvironment().IsModified( vxEnvironment::CAMERA ) &&
    GetEnvironment().GetRenderingMode()->GetSliceShadowMode() != vxRenderingMode::SHADOWS_NONE 
  );
  const bool bRenderingModeChanged( GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE ) );
  const bool bFontChanged( GetEnvironment().IsModified( vxEnvironment::FONT ) );

  return bCameraChanged || bRenderingModeChanged | bFontChanged;

} // IsRedrawNecessary()


/**
 * Render overlay text
 */  
void vxRendererOverlayCArm::Render()
{
  // don't show if the image should be "clean"
  if ( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION ) == false )
    return;

  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );

  // display the C-arm style angles
  vxOverlayStrings::vxOverlayElement overlayElement( "nothing", vxOverlayStrings::vxOverlayElement::UpperRight, Triple<float4>( 1, 1, 1 ) );
  overlayElement.SetText( GetRaoLaoString() );
  pOverlayStrings->AddElement( overlayElement );
  overlayElement.SetText( GetCranCaudString() );
  pOverlayStrings->AddElement( overlayElement );

} // Render()


// undefines
#undef FILE_REVISION


// $Log: vxRendererOverlayCArm.C,v $
// Revision 1.3  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.2  2006/10/03 20:07:31  geconomos
// modified to use new SliceShadowMode enumeration
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.4  2006/06/05 15:07:38  frank
// updated for visual studio 2005
//
// Revision 1.3  2005/06/08 15:00:39  vxconfig
// formatting
//
// Revision 1.2  2004/10/23 10:25:19  frank
// made some functions not static
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/03/02 01:10:31  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.3.2.1  2004/02/25 14:59:52  frank
// formatting
//
// Revision 1.3  2004/01/05 20:11:24  frank
// formatting
//
// Revision 1.2  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.1  2003/09/08 19:03:56  frank
// Moved C-arm style information text to separate renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlayCArm.C,v 1.3 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlayCArm.C,v 1.3 2007/03/07 17:04:39 geconomos Exp $
