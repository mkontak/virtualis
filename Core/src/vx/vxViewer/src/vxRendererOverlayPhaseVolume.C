// $Id: vxRendererOverlayPhaseVolume.C,v 1.4 2005/11/18 21:15:41 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "vxRendererOverlayPhaseVolume.h"
#include "vxUtils.h"
#include "vxEnvironment.h"
#include "GLFontGeorge.h"
#include "vxElementInt4.h"
#include "vxOverlayStrings.h"
#include "vxRenderingMode.h"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxRendererOverlayPhaseVolume::vxRendererOverlayPhaseVolume( vxEnvironment & environment )
: vxRendererOverlayMPROrtho( environment )
{
} // vxRendererOverlayPhaseVolume()


/**
 * Render 2D overlay
 */
void vxRendererOverlayPhaseVolume::Render()
{
  // call base class since we only "add" something to it
  vxRendererOverlayMPROrtho::Render();

  // now do the local stuff
  const bool bVolumeValid( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim() != Triple<int4>(2, 2, 2));
  const bool bShowPatientInfo( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION ) );

  // see if we need to render patient information
  if ( bVolumeValid )
  {
    vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >(  GetEnvironment().GetElement( vxIDs::OverlayStrings ) );

    // Slice Number at top-right
    vxOverlayStrings::vxOverlayElement element( "", vxOverlayStrings::vxOverlayElement::UpperRight );
    element.SetText(GetSliceNumberString());
    pOverlayStrings->AddElement( element );

    // Window Level Text
    element.SetText( GetWindowLevelString() );
    pOverlayStrings->AddElement( element );

    // Zoom Text - only show if different from 100%, top right
    const int4 iZoomFactor( static_cast<vxZoomFactor *>(GetEnvironment().GetZoomFactor())->GetSize() );
    if( iZoomFactor != 100 )
    {
      int1 sTmpStr[201];
      sprintf(sTmpStr, "Zoom: %d%%%%", iZoomFactor );
      element.SetText( sTmpStr );
      pOverlayStrings->AddElement( element );
    } // show only if not 100% zoom
    
    // render the phase information
    element.SetText( GetPhaseInfoString() );
    pOverlayStrings->AddElement( element );

    if ( bShowPatientInfo )
    {
      element.SetLocation( vxOverlayStrings::vxOverlayElement::UpperLeft );
      element.SetText( GetPatientNameString() );
      pOverlayStrings->AddElement( element );
      element.SetText( GetPatientIdString  () );
      pOverlayStrings->AddElement( element );
    }

  } // if valid volume

} // Render()


/**
 * Get the phase information as a string
 *
 * @return the phase information as a string
 */
std::string vxRendererOverlayPhaseVolume::GetPhaseInfoString()
{

  vxElementInt4 * pPhaseNumber = dynamic_cast< vxElementInt4 * >( GetEnvironment().GetElement( vxIDs::PhaseNumber ) );
  vxElementInt4 * pNumPhases   = dynamic_cast< vxElementInt4 * >( GetEnvironment().GetElement( vxIDs::NumPhases   ) );

  if( pPhaseNumber != NULL && pNumPhases != NULL )
  {
    std::stringstream ss;
    ss << "Phase " << pPhaseNumber->GetValue() << "/" << pNumPhases->GetValue();
    return ss.str();
  }
  else
  {
    return "";
  }

} // GetPhaseInfoString()


// $Log: vxRendererOverlayPhaseVolume.C,v $
// Revision 1.4  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.6.1  2005/07/19 11:49:45  frank
// missing include
//
// Revision 1.3  2004/10/23 10:23:34  frank
// made some functions not static
//
// Revision 1.2  2004/04/06 16:27:32  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.20  2003/11/04 15:47:40  michael
// accounting for the changes in the overlay renderers (inheritance and naming)
//
// Revision 1.19  2003/08/13 12:44:43  michael
// accounted for change from OverlayStrings to vxOverlayStrings
//
// Revision 1.18  2003/05/22 13:04:29  frank
// code review
//
// Revision 1.17  2003/05/19 14:40:22  frank
// Removed unused return parameter
//
// Revision 1.16  2003/05/05 16:04:35  frank
// Refactored overlay rendering code so it can be easily re-used
//
// Revision 1.15  2003/05/01 14:35:29  frank
// reflected parameter change in vxRendererOverlay
//
// Revision 1.14  2003/04/18 15:21:02  frank
// Removed HUconverter
//
// Revision 1.13  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.12  2003/01/22 22:02:38  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.11  2003/01/21 22:15:53  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.10  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.9  2002/09/30 17:07:46  frank
// Issue #2695: Cleaned up phase number and patient information opn phase renderer overlay display.
//
// Revision 1.8  2002/09/24 19:41:14  frank
// fixed issue #2675
//
// Revision 1.7  2002/09/24 15:32:49  frank
// Added explanation for missing images when the slab or slice is outside of the current crop box.
//
// Revision 1.6  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.5  2002/09/19 14:26:02  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.4  2002/09/16 20:00:22  dmitry
// New vxID (s) used.
//
// Revision 1.3  2002/08/17 12:31:56  frank
// Centralized display of strings in the upper two corners. Now you just add strings to the two lists and they are rendered for you by RenderCornerStrings.  No more hand formatting.
//
// Revision 1.2  2002/08/06 20:25:14  frank
// Hid zoom when 100%.
//
// Revision 1.1  2002/08/02 22:37:57  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlayPhaseVolume.C,v 1.4 2005/11/18 21:15:41 frank Exp $
// $Id: vxRendererOverlayPhaseVolume.C,v 1.4 2005/11/18 21:15:41 frank Exp $
