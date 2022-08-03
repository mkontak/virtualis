// $Id: vxRendererOverlayVessel.C,v 1.3 2007/03/07 17:04:39 geconomos Exp $
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
#include "vxRendererOverlayVessel.h"
#include "vxEnvironment.h"
#include "GLFontGeorge.h"
#include "vxIDs.h"
#include "vxOverlayStrings.h"
#include "ValueRange.h"
#include "vxVessel.h"
#include "vxVesselUtilities.h"


// Defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxRendererOverlayVessel::vxRendererOverlayVessel(vxEnvironment & environment)
:vxRendererOverlay(environment)
{
} // Constructor


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererOverlayVessel::IsRedrawNecessary()
{

  const bool bVesselArrayModified( GetEnvironment().IsModified( vxIDs::VesselArray ) );
  const bool bComponentNameChanged( GetEnvironment().IsModified( vxEnvironment::COMPONENT_NAME ) );
  const bool bComponentSelectionChanged( GetEnvironment().IsModified( vxEnvironment::COMPONENT_SELECTION ) );
  const bool bRenderingModeChanged( GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE ) );

  return
    bVesselArrayModified ||
    bComponentNameChanged ||
    bComponentSelectionChanged ||
    bRenderingModeChanged;

} // IsRedrawNecessary()


/**
 * Render vessel overlay
 */  
void vxRendererOverlayVessel::Render()
{
  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );

  // warn if displaying vesselness data
  if ( GetEnvironment().GetElement( vxIDs::IsVesselnessLoaded ) != NULL )
  {
    vxOverlayStrings::vxOverlayElement warningElement( "Vessel Enhanced Data - for segmentation only, not for diagnosis",
                                                       vxOverlayStrings::vxOverlayElement::LowerLeft, Triple<float4>( 1, 1, 0 ) );
    pOverlayStrings->AddElement( warningElement );
  }

  // don't show if the image should be "clean"
  if ( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION ) == false )
    return;

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( pVessel != NULL )
  {
    // make sure there is some data
    if ( pVessel->size() < 1 )
    {
      LogWrn( "Vessel has no data", "vxRendererOverlayVessel", "Render" );
      return;
    }

    // display the maximum tortuosity
    vxOverlayStrings::vxOverlayElement overlayElement( "", vxOverlayStrings::vxOverlayElement::LowerLeft, Triple<float4>( 1, 1, 1 ) );
    const bool bDisplayTortuosity( false );
    if ( bDisplayTortuosity )
    {
      uint4 uMaxTortuosityLocation( 0 );
      vxVessel::MeasurePair tortuosityMeasurePair;
      const float4 fMaxTortuosity( pVessel->ComputeMaximumTortuosity( uMaxTortuosityLocation, pVessel->GetMeasurementDomain() ) );
      const float4 fMinTortuosity( pVessel->ComputeMinimumTortuosity( uMaxTortuosityLocation, pVessel->GetMeasurementDomain() ) );
      const float4 fAvgTortuosity( pVessel->ComputeAverageTortuosity( uMaxTortuosityLocation, pVessel->GetMeasurementDomain() ) );
      overlayElement.SetText( "Min/Avg/Max tortuosity: " + ToAscii( RoundDecimals( fMinTortuosity * 10.0F, 1 ) ) +  "/" +
                              ToAscii( RoundDecimals( fAvgTortuosity * 10.0F, 1 ) ) +  "/" + 
                              ToAscii( RoundDecimals( fMaxTortuosity * 10.0F, 1 ) ) + " °/cm" );
      pOverlayStrings->AddElement( overlayElement );
    }

    // display the diameter range
    const bool bValidDiameters( pVessel->GetDiameterRange( pVessel->GetMeasurementDomain() ).GetMaximum() != 0.0F );
    if ( bValidDiameters )
    {
      std::string minMaxDiam = "Min/Max diam: ";
      minMaxDiam += ToAscii( RoundDecimals( pVessel->GetDiameterRange( pVessel->GetMeasurementDomain() ).GetMinimum(), 1 ) ) + "/";
      minMaxDiam += ToAscii( RoundDecimals( pVessel->GetDiameterRange( pVessel->GetMeasurementDomain() ).GetMaximum(), 1 ) ) + " mm";
      overlayElement.SetText( minMaxDiam );
      pOverlayStrings->AddElement( overlayElement );
    }

    // display the area range
    const bool bValidAreas( pVessel->GetWallAreaRange( pVessel->GetMeasurementDomain() ).GetMaximum() != 0.0F );
    if ( bValidAreas )
    {
      std::string minMaxArea = "Min/Max area: ";
      minMaxArea += ToAscii( RoundDecimals( pVessel->GetLumenAreaRange( pVessel->GetMeasurementDomain() ).GetMinimum() / 100.0F, 2 ) ) + "/";
      minMaxArea += ToAscii( RoundDecimals( pVessel->GetLumenAreaRange( pVessel->GetMeasurementDomain() ).GetMaximum() / 100.0F, 2 ) ) + " cm^2";
      overlayElement.SetText( minMaxArea );
      pOverlayStrings->AddElement( overlayElement );

      // display the measured volume
      overlayElement.SetText( "Volume: " + ToAscii( RoundDecimals( pVessel->GetVolume( pVessel->GetMeasurementDomain() ) / 1000.0F, 1 ) ) + " cm^3" );
      pOverlayStrings->AddElement( overlayElement );
    }

    // display the measured length
    overlayElement.SetText( "Length: " + ToAscii( RoundDecimals( pVessel->GetLength( pVessel->GetMeasurementDomain() ), 1 ) ) + " mm" );
    pOverlayStrings->AddElement( overlayElement );

    // display the vessel name
    const std::string & sName = GetEnvironment().GetComponents()->ArrayByLabel()[ pVessel->GetLabel() ]->GetName();
    overlayElement.SetText( "Vessel: " + sName );
    pOverlayStrings->AddElement( overlayElement );
  }
} // Render()


// undefines
#undef FILE_REVISION


// $Log: vxRendererOverlayVessel.C,v $
// Revision 1.3  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.2  2006/09/21 15:00:30  geconomos
// moved code that displays "vessel not selected" to it's own renderer
//
// Revision 1.1  2006/09/14 20:09:32  geconomos
// moved from vascular project
//
// Revision 1.6  2006/08/17 15:52:30  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.5  2005/06/08 15:01:41  vxconfig
// formatting
//
// Revision 1.4  2005/06/07 18:12:01  vxconfig
// removed the vessel array
//
// Revision 1.3  2004/09/02 17:58:18  frank
// decreased the granularity of component array modified bits
//
// Revision 1.2  2004/09/02 16:24:53  frank
// decreased the granularity of component array modified bits
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
// Revision 1.19  2004/03/02 01:10:30  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.18.2.2  2004/03/01 22:21:34  wenli
// Add min/max and Avg Tortuosity.
//
// Revision 1.18.2.1  2004/02/25 15:00:09  frank
// formatting
//
// Revision 1.18  2004/02/02 15:08:36  frank
// moved the text to the lower left
//
// Revision 1.17  2004/01/05 20:24:10  frank
// formatting
//
// Revision 1.16  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.15  2003/12/18 14:21:36  frank
// Renamed the element ID filename to match class name
//
// Revision 1.14  2003/10/02 12:02:59  frank
// Removed tortuosity display
//
// Revision 1.13  2003/09/08 19:04:55  frank
// Moved C-arm style information text to separate renderer
//
// Revision 1.12  2003/09/08 18:36:55  frank
// Moved pre-processing information text to separate renderer
//
// Revision 1.11  2003/08/29 18:13:19  dongqing
// show parameter setting name on the top of the image
//
// Revision 1.10  2003/08/25 15:50:36  frank
// Added warning string to vessel enhanced view
//
// Revision 1.9  2003/08/25 15:18:24  frank
// Adjusted significant digits
//
// Revision 1.8  2003/08/13 16:37:06  michael
// accounted for change from OverlayStrings to vxOverlayStrings
//
// Revision 1.7  2003/07/09 19:20:20  frank
// Showed rao/lao and cran/caud degrees
//
// Revision 1.6  2003/06/02 18:35:59  frank
// Removed extra space at top-right of overlay
//
// Revision 1.5  2003/05/30 12:32:34  frank
// Moved the vessel data text to the lower left
//
// Revision 1.4  2003/05/28 17:45:57  frank
// Displayed the vessel wall area on the graph.
//
// Revision 1.3  2003/05/27 13:23:54  frank
// Throttled the dislay of image information
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.30  2003/05/06 16:21:13  frank
// Removed HUConverter
//
// Revision 1.29  2003/05/05 16:06:27  frank
// Refactored overlay rendering code so it can be easily re-used
//
// Revision 1.28  2003/05/02 20:12:58  frank
// Removed "Measurements in Range"
//
// Revision 1.27  2003/05/01 17:10:00  frank
// typo
//
// Revision 1.26  2003/05/01 14:39:32  frank
// Clarified the results
//
// Revision 1.25  2003/04/30 18:03:27  frank
// code review
//
// Revision 1.24  2003/04/14 19:44:37  frank
// Moving over to overlay strings
//
// Revision 1.23  2003/04/09 13:59:54  frank
// Reduced the number of debug messages
//
// Revision 1.22  2003/02/26 21:40:43  frank
// Fixed volume display
//
// Revision 1.21  2003/02/24 16:10:29  frank
// Added back in volume display
//
// Revision 1.20  2003/02/18 21:50:44  frank
// Error handling for empty vessels.
//
// Revision 1.19  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.18  2003/02/11 16:08:41  frank
// Put back tortuosity display
//
// Revision 1.17  2003/02/10 12:41:16  frank
// commented out slow part of rendering
//
// Revision 1.16.2.1  2003/02/12 23:10:18  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.16  2003/02/04 23:52:25  frank
// volume is negative?
//
// Revision 1.15  2003/01/24 20:17:57  frank
// Added HU for curved
//
// Revision 1.14  2003/01/23 23:21:02  frank
// changed units of measurement
//
// Revision 1.13  2003/01/16 21:27:32  frank
// code review
//
// Revision 1.12  2003/01/14 16:17:37  dongqing
// Code review
//
// Revision 1.11  2002/11/28 02:14:52  frank
// removed volume display
//
// Revision 1.10  2002/11/22 04:28:47  frank
// removed max tortuosity location
//
// Revision 1.9  2002/11/22 04:28:10  frank
// Adjusted precision of vessel information
//
// Revision 1.8  2002/11/21 19:31:08  frank
// Rendering tortuosity
//
// Revision 1.7  2002/11/21 00:51:51  frank
// Let you go to vessel views without a vessel without a crash
//
// Revision 1.6  2002/11/20 17:30:45  frank
// Computed volume and displayed on vessel overlay
//
// Revision 1.5  2002/11/19 20:22:15  frank
// Added maximum tortuosity
//
// Revision 1.4  2002/11/13 01:10:10  frank
// Showed less significant digits.
//
// Revision 1.3  2002/11/08 17:51:23  frank
// Simplified text
//
// Revision 1.2  2002/11/08 02:34:48  frank
// Removed debug text
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.14  2002/10/30 23:11:37  frank
// More output text
//
// Revision 1.13  2002/10/29 16:51:21  frank
// revamped measurement
//
// Revision 1.12  2002/10/29 15:41:23  geconomos
// Added inclusion of header file.
//
// Revision 1.11  2002/10/25 01:51:49  frank
// Moved working vessel into the VesselArray
//
// Revision 1.10  2002/09/25 15:25:24  wenli
// v3D vessel component list
//
// Revision 1.9  2002/09/16 20:22:14  dmitry
// New vxID (s) used.
//
// Revision 1.8  2002/08/23 17:30:23  frank
// Text output.
//
// Revision 1.7  2002/08/23 15:25:09  wenli
// initial version - Vascular
//
// Revision 1.6  2002/08/23 14:20:34  ana
// GetAreaRangeMeasure for the measrue and
// GetAreaRangeVessel fot the whole vessel.
//
// Revision 1.5  2002/08/23 14:12:45  frank
// Rounded significant digits
//
// Revision 1.4  2002/08/22 18:01:47  wenli
// initial version - Vascular
//
// Revision 1.3  2002/08/22 17:18:17  frank
// working on live updates
//
// Revision 1.2  2002/08/22 03:34:12  frank
// cleaned up display
//
// Revision 1.1  2002/08/22 02:58:44  frank
// Intial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlayVessel.C,v 1.3 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlayVessel.C,v 1.3 2007/03/07 17:04:39 geconomos Exp $
