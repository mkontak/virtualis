// $Id: vxManipulatorVesselCurved.C,v 1.2 2006/09/25 13:22:06 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxManipulatorVesselCurved.h"
#include "vxVessel.h"
#include "vxVesselUtilities.h"
#include "vxEnvironment.h"



// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * default constructor
 * @param environment the environment
 */
vxManipulatorVesselCurved::vxManipulatorVesselCurved(vxEnvironment & environment):
vxManipulator(environment)
{
} // Constructor()


/**
 * Zoom the view to fit the entire volume.
 * @param environment the environment
 * @param imageDim    the dimensions of the image
 */
void vxManipulatorVesselCurved::ZoomEntireVolume( vxEnvironment & environment, const Triple<uint4> & imageDim )
{

  // get the vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( environment.GetComponents() );
  if ( pVessel == NULL )
  {
    //LogDbg( "Vessel pointer is NULL", "vxManipulatorVesselCurved", "ZoomEntireVolume" );
    return;
  }
  const float4 fVesselHeightMM( pVessel->GetLength( pVessel->GetEntireDomain() ) );

  // determine the maximum size of the vessel
  // (comparing the x-dim of the volume with the
  // overall vessel length in mm)
  const float4 fVesselWidthMM( environment.GetIntensityVolume()->GetHeader().GetWorldDim().m_x );
  const float4 fVesselAspectRatio( fVesselHeightMM / fVesselWidthMM );
  const float4 fImageAspectRatio( float4( imageDim.m_y ) / float4( imageDim.m_x ) );

  // compare the aspect ratio of the image to the vessel
  if ( fImageAspectRatio > fVesselAspectRatio )
  {
    // note: plane width is really half width
    environment.GetPlaneCurved()->SetWidth( fVesselWidthMM / 2.0F );
  }
  else
  {
    // note: plane width is really half width
    environment.GetPlaneCurved()->SetWidth( fVesselHeightMM / fImageAspectRatio / 2.0F );
  }

  //environment.GetPlaneCurved()->Modified( vxModInfo( vxPlaneCurved, vxUtils::vxEventPlaneEnum::PLANE_MODIFIED ) );
  environment.SetModified( vxEnvironment::PLANE_MPR, true );

} // ZoomEntireVolume()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorVesselCurved.C,v $
// Revision 1.2  2006/09/25 13:22:06  geconomos
// + commented out unneeded  log debugging
// + removed "DetailViewMode" extended element
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.3  2005/06/07 18:09:26  vxconfig
// removed the vessel array
//
// Revision 1.2  2005/05/03 15:52:42  frank
// Adjusted curved MPR rendering for non-uniform aspect ratio windows
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
// Revision 1.5  2004/03/02 01:10:32  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.4.2.1  2004/02/25 14:58:42  frank
// formatting
//
// Revision 1.4  2004/01/08 12:47:27  frank
// formatting
//
// Revision 1.3  2003/05/30 12:31:32  frank
// zoomed the curved view more
//
// Revision 1.2  2003/05/29 14:04:58  frank
// Fixed debug messages
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.16  2003/05/07 20:59:46  dongqing
// code walkthrough
//
// Revision 1.15  2003/04/30 14:12:07  frank
// code review
//
// Revision 1.14  2003/02/14 19:26:15  frank
// Moved zoom function out to curved
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorVesselCurved.C,v 1.2 2006/09/25 13:22:06 geconomos Exp $
// $Id: vxManipulatorVesselCurved.C,v 1.2 2006/09/25 13:22:06 geconomos Exp $