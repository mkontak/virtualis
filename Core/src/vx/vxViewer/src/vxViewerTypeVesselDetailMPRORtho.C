// $Id: vxViewerTypeVesselDetailMPRORtho.C,v 1.1 2006/09/25 13:16:23 geconomos Exp $
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
#include "vxViewerTypeVesselDetailMPROrtho.h"
#include "vxManipulatorVesselDetailMPROrtho.h"
#include "vxVesselUtilities.h"
#include "vxElementFloat4.h"
#include "vxEnvironment.h"
#include "vxVessel.h"


// defines
#define FILE_REVISION "$Revision: 1.1 $"


/**
 * Constructor
 */
vxViewerTypeVesselDetailMPROrtho::vxViewerTypeVesselDetailMPROrtho( vxEnvironment & environment )
: vxViewerTypeMPROrtho( environment )
{
} // vxViewerTypeVesselDetailMPROrtho()


/**
 * Initializes the view
 */
void vxViewerTypeVesselDetailMPROrtho::InitializeView()
{

  // call the base class
  vxViewerTypeMPROrtho::InitializeView();

  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( pVessel == NULL )
    return;

  vxShareableObject< vxElementFloat4 > * pCurrentVesselSection = dynamic_cast< vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ) );
  if( pCurrentVesselSection == NULL )
    return;
  
  // get the position along the centerline
  Point<float4> position( pVessel->GetResampledPosition( pCurrentVesselSection->GetValue() ) );

  // set the ortho plane location
  uint1 uOrientation( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );
  GetEnvironment().GetPlaneOrtho()->SetDistance( position[ uOrientation ] );
  GetEnvironment().SetModified( vxEnvironment::PLANE_MPR, true );

    // set the slice at the start of the vessel
//  vxManipulatorVesselDetailMPROrtho::SetVesselSectionIndex( GetEnvironment(), 0 );


} // InitializeView()


// $Log: vxViewerTypeVesselDetailMPRORtho.C,v $
// Revision 1.1  2006/09/25 13:16:23  geconomos
// moved from vascular and renamed
//
// Revision 1.3  2006/09/14 20:16:27  geconomos
// proper initialization of viewer
//
// Revision 1.2  2005/06/08 15:02:49  vxconfig
// formatting
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
// Revision 1.2  2004/03/02 01:10:28  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.1.2.2  2004/02/25 16:10:30  frank
// removed debug
//
// Revision 1.1.2.1  2004/02/25 16:08:28  frank
// initial version
//
// $Id: vxViewerTypeVesselDetailMPRORtho.C,v 1.1 2006/09/25 13:16:23 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerTypeVesselDetailMPRORtho.C,v 1.1 2006/09/25 13:16:23 geconomos Exp $
