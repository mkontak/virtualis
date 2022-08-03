// $Id: vxRendererOverlayNoVessel.C,v 1.3 2007/03/07 17:04:39 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlayNoVessel.C,v 1.3 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxRendererOverlayNoVessel.h"
#include "vxEnvironment.h"
#include "vxOverlayStrings.h"
#include "vxVesselUtilities.h"


// Defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Constructor.
 * @param environment the environment
 */
vxRendererOverlayNoVessel::vxRendererOverlayNoVessel( vxEnvironment & environment )
:vxRendererOverlay( environment )
{
} // Constructor


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererOverlayNoVessel::IsRedrawNecessary()
{

  return false;

} // IsRedrawNecessary()


/**
 * Render 2D overlay.
 */
void vxRendererOverlayNoVessel::Render()
{
  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );
  vxOverlayStrings::vxOverlayElement overlayElement( "", vxOverlayStrings::vxOverlayElement::MiddleMiddle, Triple<float4>( 1, 1, 0 ) );

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL )
  {
    vxOverlayStrings::vxOverlayElement overlayElement( "", vxOverlayStrings::vxOverlayElement::MiddleMiddle, Triple<float4>( 1, 1, 0 ) );
    overlayElement.SetText( "No current vessel is selected.  Please select a vessel from" );
    pOverlayStrings->AddElement( overlayElement );
    overlayElement.SetText( "the list of components, or create a new one if necessary." );
    pOverlayStrings->AddElement( overlayElement );

    glClear( GL_COLOR_BUFFER_BIT );
  }

} // Render()


// undefines
#undef FILE_REVISION


// $Log: vxRendererOverlayNoVessel.C,v $
// Revision 1.3  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.2  2006/11/10 04:29:57  geconomos
// clearing color buffer if a vessel isn't selected
//
// Revision 1.1  2006/09/21 15:09:24  geconomos
// moved from vascular
//
// Revision 1.5  2006/09/14 20:17:56  geconomos
// moved code ito display "no vessel selected" to vxRendererOverlayVessel in  vxViewer
//
// Revision 1.4  2006/08/17 15:52:30  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.3  2005/06/08 15:01:32  vxconfig
// formatting
//
// Revision 1.2  2005/06/07 18:10:42  vxconfig
// removed the vessel array
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2004/03/02 01:10:31  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.9.2.2  2004/02/25 15:00:01  frank
// formatting
//
// Revision 1.9.2.1  2004/02/23 15:16:49  frank
// fixed problem when no vascular dataset is loaded
//
// Revision 1.9  2004/01/08 12:47:27  frank
// formatting
//
// Revision 1.8  2004/01/05 20:25:26  frank
// Used the is-pre-processed extended element instead of the static boolean to keep track of the processed state of the dataset.  Now it can handle more than one dataset at a time.
//
// Revision 1.7  2003/12/22 18:53:22  frank
// optimized redraws
//
// Revision 1.6  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.5  2003/08/13 16:37:06  michael
// accounted for change from OverlayStrings to vxOverlayStrings
//
// Revision 1.4  2003/05/27 13:23:25  frank
// Throttled the dislay of image information
//
// Revision 1.3  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.2  2003/05/16 19:35:35  frank
// Issue #3200: Show message if vascular dataset is not loaded
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.4  2003/04/30 18:03:39  frank
// code review
//
// Revision 1.3  2003/04/15 19:05:47  frank
// Moving over to overlay strings
//
// Revision 1.2  2003/04/09 14:00:08  frank
// Reduced the number of debug messages
//
// Revision 1.1  2002/11/21 02:31:55  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlayNoVessel.C,v 1.3 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlayNoVessel.C,v 1.3 2007/03/07 17:04:39 geconomos Exp $
