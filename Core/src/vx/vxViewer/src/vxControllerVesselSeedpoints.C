// $Id: vxControllerVesselSeedpoints.C,v 1.1.2.1 2009/03/31 21:04:25 gdavidson Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson (mailto:gdavidson@viatronix.com)


#include "StdAfx.h"
#include "vxControllerVesselSeedpoints.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxEnvironment.h"


// defines
#define FILE_REVISION "$Revision: 1.1.2.1 $"


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxControllerVesselSeedpoints::vxControllerVesselSeedpoints( vxEnvironment & environment ): vxController( environment )
{
} // vxControllerVesselSeedpoints()



/**
 * Determine if processing is necessary
 *
 * @return if processing is necessary
 */
bool vxControllerVesselSeedpoints::IsProcessingNecessary()
{
  vxShareableObject< vxSeedpoints > * pSeedpoints = GetSeedpoints();
  const bool bSeedpointsExist = ( pSeedpoints != NULL ) && pSeedpoints->GetNumClicks() > 0;
  const bool bSeedpointsChanged = GetEnvironment().IsModified( vxIDs::Seedpoints );

  return bSeedpointsChanged && bSeedpointsExist;
} // IsProcessingNecessary()



/**
 * Synchrozies the detail view to the current vessel section index.
 */
void vxControllerVesselSeedpoints::Process()
{
  Point3Df seedPointWorld( GetSeedpoints()->GetLastSeedPoint().GetScaledAnIso( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() ));

  // if we are in 2D, jump to the slice in which the seedpoint was found
  if( dynamic_cast< vxViewerTypeMPROrtho * >( GetEnvironment().GetViewerType() ) != NULL )
  {
    const float8 fDistance = GetEnvironment().GetPlaneOrtho()->GetDistance( seedPointWorld );
    GetEnvironment().GetPlaneOrtho()->Move( fDistance );
    GetEnvironment().SetModified( vxEnvironment::PLANE_MPR, true );
  }
  else if( dynamic_cast< vxViewerTypeMPROblique * >( GetEnvironment().GetViewerType() ) != NULL )
  { 
    const float8 fDistance = GetEnvironment().GetPlaneOblique()->GetDistance( seedPointWorld );
    GetEnvironment().GetPlaneOblique()->Move( fDistance );
    GetEnvironment().SetModified( vxEnvironment::PLANE_MPR, true );
  } // else if
} // Process()



/**
 * Get the seedpoints from the environment
 *
 * @return 
 */
vxShareableObject< vxSeedpoints > * vxControllerVesselSeedpoints::GetSeedpoints()
{
  vxShareableObject< vxSeedpoints > * pSeedpoints = dynamic_cast< vxShareableObject< vxSeedpoints > * >( GetEnvironment().GetElement( vxIDs::Seedpoints ) );
  if( pSeedpoints == NULL )
    LogDbg( "No seedpoints found", "vxManipulatorVesselCreate", "GetSeedpoints" );

  return pSeedpoints;
} // GetSeedpoints()



// $Log: vxControllerVesselSeedpoints.C,v $
// Revision 1.1.2.1  2009/03/31 21:04:25  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerVesselSeedpoints.C,v 1.1.2.1 2009/03/31 21:04:25 gdavidson Exp $
// $Id: vxControllerVesselSeedpoints.C,v 1.1.2.1 2009/03/31 21:04:25 gdavidson Exp $
