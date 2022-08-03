// $Id: vxRendererOverlayVesselSection.C,v 1.2 2007/03/07 17:04:39 geconomos Exp $
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
#include "vxRendererOverlayVesselSection.h"
#include "vxElementFloat4.h"
#include "ValueRange.h"
#include "vxOverlayStrings.h"
#include "vxVesselUtilities.h"
#include "vxEnvironment.h"
#include "vxVessel.h"

// Defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxRendererOverlayVesselSection::vxRendererOverlayVesselSection( vxEnvironment & environment )
: vxRendererOverlay( environment )
{
} // vxRendererOverlayVesselSection()


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererOverlayVesselSection::IsRedrawNecessary()
{

  const bool bRenderingModeChanged( GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE ) );
  const bool bVesselChanged( GetEnvironment().IsModified( vxIDs::VesselArray ) );

  return bRenderingModeChanged || bVesselChanged;

} // IsRedrawNecessary()


/**
 * Render 2D overlay
 */
void vxRendererOverlayVesselSection::Render()
{

  // make sure it should be shown at all
  if ( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION ) == false )
    return;

  // get the overlay strings object
  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );
  vxOverlayStrings::vxOverlayElement overlayElement( "nothing", vxOverlayStrings::vxOverlayElement::LowerLeft, Triple<float4>( 1, 1, 1 ) );

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogWrn( "Vessel pointer is NULL", "vxRendererOverlayVesselSection", "Render" );
    return;
  }

  // check for no data
  if ( pVessel->size() < 1 )
  {
    LogWrn( "Vessel currently has no data", "vxRendererOverlayVesselSection", "Render" );
    return;
  }

  // get the current vessel section
  vxShareableObject< vxElementFloat4 > * pCurrentVesselSection = dynamic_cast<vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ));
  if( pCurrentVesselSection == NULL )
  {
    LogWrn( "no current vessel section: ", "vxRendererOverlayVesselSection", "Render" );
    return;
  }
  int4 iCurrentVesselSection( pCurrentVesselSection->GetValue() + 0.5F );
  if ( uint4(iCurrentVesselSection) > pVessel->size()-1 )
  {
    LogWrn( "bad current vessel section: "+ToAscii(iCurrentVesselSection) , "vxRendererOverlayVesselSection", "Render" );
    return;
  }

  // display the tortuosity at this point
  const bool bShowTortuosity( false );
  if ( bShowTortuosity )
  {
    const float4 fTortuosity = pVessel->ResampleTortuosityDegree2MM( iCurrentVesselSection );
    overlayElement.SetText( "Tortuosity: " + ToAscii( RoundDecimals( fTortuosity*10.0F, 0 ) ) + " °/cm" );
    pOverlayStrings->AddElement( overlayElement );
  }

  // display the diameter range
  const bool bValidDiameters( pVessel->GetDiameterRange( pVessel->GetMeasurementDomain() ).GetMaximum() != 0.0F );
  const bool bValidAreas( pVessel->GetWallAreaRange( pVessel->GetMeasurementDomain() ).GetMaximum() != 0.0F );
  if ( bValidAreas && bValidDiameters )
  {
    std::string minMaxDiam = "Min/Mean/Max diam: ";
    minMaxDiam += ToAscii( RoundDecimals( pVessel->at( iCurrentVesselSection ).GetDiameterRange().GetMinimum(), 1 ) ) + "/";
    minMaxDiam += ToAscii( RoundDecimals( pVessel->at( iCurrentVesselSection ).GetMeanDiameter(), 1 ) ) + "/";
    minMaxDiam += ToAscii( RoundDecimals( pVessel->at( iCurrentVesselSection ).GetDiameterRange().GetMaximum(), 1 ) ) + " mm";
    overlayElement.SetText( minMaxDiam );
    pOverlayStrings->AddElement( overlayElement );
  }

  // display the area range
  if ( bValidAreas )
  {
    std::string area = "Area: " + ToAscii( RoundDecimals( pVessel->at( iCurrentVesselSection ).GetLumenArea() / 100.0F, 2 ) ) + " cm^2";
    overlayElement.SetText( area );
    pOverlayStrings->AddElement( overlayElement );
  }

  // display the vessel name
  const std::string & sName = GetEnvironment().GetComponents()->ArrayByLabel()[ pVessel->GetLabel() ]->GetName();
  overlayElement.SetText( "Vessel: " + sName );
  pOverlayStrings->AddElement( overlayElement );

} // Render()


// undefines
#undef FILE_REVISION


// $Log: vxRendererOverlayVesselSection.C,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.5  2006/08/17 15:52:30  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.4  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.1  2005/08/15 17:37:31  frank
// Issue #4353: Fixed vessel name
//
// Revision 1.3  2005/06/08 15:01:41  vxconfig
// formatting
//
// Revision 1.2  2005/06/07 18:12:01  vxconfig
// removed the vessel array
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.3  2004/06/17 14:00:15  frank
// avoided showing zero measurement values for simple curved MPR vessels
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.15  2004/03/02 01:10:30  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.14.2.1  2004/02/25 15:00:09  frank
// formatting
//
// Revision 1.14  2004/01/05 20:23:50  frank
// formatting
//
// Revision 1.13  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.12  2003/09/30 20:45:40  frank
// code review
//
// Revision 1.11  2003/08/25 15:18:24  frank
// Adjusted significant digits
//
// Revision 1.10  2003/08/13 16:37:06  michael
// accounted for change from OverlayStrings to vxOverlayStrings
//
// Revision 1.9  2003/07/09 19:19:32  frank
// added degree symbol for degrees
//
// Revision 1.8  2003/06/10 14:25:15  wenli
// Add ResampleTortuosity()
//
// Revision 1.7  2003/06/06 12:27:04  dongqing
// fix tortuosity computation bug
//
// Revision 1.6  2003/05/30 12:32:51  frank
// Moved the vessel data text to the lower left
//
// Revision 1.5  2003/05/27 14:03:49  frank
// Made the cross-section area more specific to lumen
//
// Revision 1.4  2003/05/27 13:23:54  frank
// Throttled the dislay of image information
//
// Revision 1.3  2003/05/21 13:26:18  frank
// code review
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.15  2003/04/30 18:03:27  frank
// code review
//
// Revision 1.14  2003/04/15 19:05:53  frank
// Moving over to overlay strings
//
// Revision 1.13  2003/03/21 15:43:59  frank
// Rounded for nearest contour
//
// Revision 1.12  2003/03/13 14:12:41  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.11  2003/02/26 21:41:10  frank
// Synchronized units of all overlays
//
// Revision 1.10  2003/02/18 21:50:29  frank
// Error handling for empty vessels.
//
// Revision 1.9  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.8  2003/02/11 16:08:41  frank
// Put back tortuosity display
//
// Revision 1.7  2003/02/10 17:15:43  frank
// took out tortuosity
//
// Revision 1.6.2.1  2003/02/12 23:10:18  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.6  2003/01/22 22:18:22  ingmar
// more detailed debug info
//
// Revision 1.5  2003/01/16 21:27:47  frank
// code review
//
// Revision 1.4  2003/01/14 16:17:37  dongqing
// Code review
//
// Revision 1.3  2002/11/21 19:35:03  frank
// Revised tortuosity computation
//
// Revision 1.2  2002/11/21 00:51:51  frank
// Let you go to vessel views without a vessel without a crash
//
// Revision 1.1  2002/11/19 20:14:05  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlayVesselSection.C,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlayVesselSection.C,v 1.2 2007/03/07 17:04:39 geconomos Exp $
