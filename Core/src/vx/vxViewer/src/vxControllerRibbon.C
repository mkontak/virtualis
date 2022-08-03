// $Id: vxControllerRibbon.C,v 1.9 2007/11/15 22:32:17 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxControllerRibbon.h"
#include "vxEnvironment.h"
#include "vxVessel.h"
#include "vxVesselUtilities.h"
#include "vxRibbon.h"
#include "vxManipulatorVesselCurved.h"
#include "vxViewerTypeMPRCurvedLuminal.h"

#define FILE_REVISION "$Revision: 1.9 $"


/**
 * Constructor
 */
vxControllerRibbon::vxControllerRibbon( vxEnvironment & environment ) : 
  vxController( environment ),
  m_pLastViewerType( NULL )
{
} // vxControllerRibbon()


/**
 * Determine if processing is necessary
 *
 * @return    true if necessary
 */
bool vxControllerRibbon::IsProcessingNecessary()
{
  bool bIsProcessingNecessary( false );
  bIsProcessingNecessary = GetEnvironment().IsModified( vxEnvironment::WIN_RECT ) || 
                           GetEnvironment().IsModified( vxEnvironment::COMPONENT_SELECTION ) ||
                           GetEnvironment().IsModified( vxEnvironment::COMPONENT_VISUAL ) ||
                           GetEnvironment().IsModified( vxEnvironment::PLANE_MPR ) ||
                           m_pLastViewerType != GetEnvironment().GetViewerType();

  m_pLastViewerType = GetEnvironment().GetViewerType();

  return bIsProcessingNecessary;
} // IsProcessingNecessary()


/**
 * Process the data
 */
void vxControllerRibbon::Process()
{
	//Check to see if there are any components
	if ( GetEnvironment().GetComponents()->GetCount() <= 0 )
		return;

  // get the current vessel
	vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( pVessel == NULL )
  {
    LogDbg( "Vessel pointer is NULL", "vxControllerRibbon", "vxControllerRibbon" );
    return;
  }
  
  // get the intensity volume
  vxBlockVolume<uint2> * pIntensityVolume = dynamic_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume());
  if (pIntensityVolume == NULL)
  {
    LogDbg( "Intensity volume pointer is NULL", "vxControllerRibbon", "vxControllerRibbon" );
    return;
  }

  // get the ribbon
  vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL )
  {
    LogDbg( "Ribbon pointer is NULL", "vxControllerRibbon", "vxControllerRibbon" );
    return;
  }
	
  // check if the window has changed
  if( GetEnvironment().IsModified( vxEnvironment::WIN_RECT ) || 
			GetEnvironment().IsModified( vxEnvironment::COMPONENT_SELECTION )||
		  GetEnvironment().IsModified( vxEnvironment::COMPONENT_VISUAL) )
	{
		vxPlaneCurved * pPlane = dynamic_cast< vxPlaneCurved * >( GetEnvironment().GetPlaneCurved() );
		const float4 fVesselHeightMM( pVessel->GetLength( pVessel->GetEntireDomain() ) );
		pPlane->SetWidth( (fVesselHeightMM*GetEnvironment().GetWinRect().GetViewportWidth()/GetEnvironment().GetWinRect().GetViewportHeight())/2.0F );
   // vxManipulatorVesselCurved::ZoomEntireVolume( GetEnvironment(), Triple<uint4>( GetEnvironment().GetWinRect().GetViewportWidth(), GetEnvironment().GetWinRect().GetViewportHeight(), 0 ) );
  }

  // compute the ribbon based on viewer type
  vxViewerType * pViewerType = GetEnvironment().GetViewerType();
  if( dynamic_cast< vxViewerTypeMPRCurvedLuminal * >( pViewerType ) )
    pRibbon->ComputeScanlinesLuminal( * pVessel, GetEnvironment(), Triple<uint4>( GetEnvironment().GetWinRect().GetViewportWidth(),GetEnvironment().GetWinRect().GetViewportHeight(), 0 ) );
  
  else if( dynamic_cast< vxViewerTypeMPRCurved * >( pViewerType ) )
    pRibbon->ComputeScanlinesCurved( * pVessel, GetEnvironment(), Triple<uint4>( GetEnvironment().GetWinRect().GetViewportWidth(),GetEnvironment().GetWinRect().GetViewportHeight(), 0 ) );

  // indicate the the ribbon has beenmodified
  pRibbon->Modified( vxModInfoID( vxElement, vxIDs::Ribbon, 0 ) );

} // Process()


// $Log: vxControllerRibbon.C,v $
// Revision 1.9  2007/11/15 22:32:17  romy
// code added to display the Vessel in Full View Size
//
// Revision 1.8  2007/08/28 19:14:45  romy
// added extra error checks
//
// Revision 1.7  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.6  2006/11/22 21:48:54  gdavidson
// Check if the ViewerType has changed.
//
// Revision 1.5  2006/11/12 23:56:31  geconomos
// added smarter initialization of ribbon
//
// Revision 1.4  2006/11/07 21:36:48  geconomos
// handling changing of selected vessel
//
// Revision 1.3  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.2  2006/09/25 13:22:27  geconomos
// commented out unneeded  log debugging
//
// Revision 1.1  2006/09/14 20:06:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerRibbon.C,v 1.9 2007/11/15 22:32:17 romy Exp $
// $Id: vxControllerRibbon.C,v 1.9 2007/11/15 22:32:17 romy Exp $
