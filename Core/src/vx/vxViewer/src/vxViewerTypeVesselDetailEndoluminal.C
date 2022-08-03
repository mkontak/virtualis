// $Id: vxViewerTypeVesselDetailEndoluminal.C,v 1.2 2007/08/28 17:04:22 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxViewerTypeVesselDetailEndoluminal.h"
#include "vxEnvironment.h"
#include "vxManipulatorVesselNavigation.h"
#include "vxElementFloat4.h"
#include "vxVesselUtilities.h"
#include "vxVessel.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 */
vxViewerTypeVesselDetailEndoluminal::vxViewerTypeVesselDetailEndoluminal( vxEnvironment & environment )
: vxViewerType3D( environment )
{
} // vxViewerTypeVesselDetailEndoluminal()


/**
 * Initializes the vxCamera
 */
void vxViewerTypeVesselDetailEndoluminal::InitializeView()
{

  // make sure there is a current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if ( pVessel == NULL )
  {
    LogDbg( "there is no current vessel...unable to initialize view", "vxViewerTypeVesselDetailEndoluminal", "InitializeView" );
    return;
  }

  vxShareableObject< vxElementFloat4 > * pCurrentVesselSection = dynamic_cast< vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ) );
  if( pCurrentVesselSection == NULL )
    return;

	//Set to 60 for a nice view.
	GetEnvironment().GetCamera()->SetFovDegrees( 60.0f );
  
  // early return for empty vessels
  if ( pVessel->size() < 1 ) return;

  // start a little bit from the end of the vessel looking down the tube
  vxManipulatorVesselNavigation::SetVesselSectionIndex( GetEnvironment(), pCurrentVesselSection->GetValue(), false );

} // InitializeView()


// $Log: vxViewerTypeVesselDetailEndoluminal.C,v $
// Revision 1.2  2007/08/28 17:04:22  romy
// Modified the Camera FOV from 45 to 60
//
// Revision 1.1  2006/09/25 13:16:23  geconomos
// moved from vascular and renamed
//
// Revision 1.5  2006/09/14 20:16:35  geconomos
// proper initialization of viewer
//
// Revision 1.4  2006/08/17 15:52:30  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.3  2005/06/08 15:02:49  vxconfig
// formatting
//
// Revision 1.2  2005/06/07 18:15:03  vxconfig
// removed the vessel array
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/03/02 19:19:35  frank
// renamed method in viewer type to more accurately describe it
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/03/02 01:10:29  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.3.2.3  2004/02/26 05:18:44  frank
// Issue #3757: Fixed the 3D endoluminal view update when switching among vessels
//
// Revision 1.3.2.2  2004/02/25 16:31:35  frank
// Issue #3759: Updated the endoluminal view when switching vessels
//
// Revision 1.3.2.1  2004/02/25 15:00:40  frank
// formatting
//
// Revision 1.3  2004/01/14 14:18:16  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.2  2003/10/22 14:48:41  frank
// Issue #2793: Initialized the endoluminal viewpoint
//
// Revision 1.1  2003/07/09 14:15:10  frank
// Added 3D endoluminal viewer type
//
// $Id: vxViewerTypeVesselDetailEndoluminal.C,v 1.2 2007/08/28 17:04:22 romy Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerTypeVesselDetailEndoluminal.C,v 1.2 2007/08/28 17:04:22 romy Exp $
