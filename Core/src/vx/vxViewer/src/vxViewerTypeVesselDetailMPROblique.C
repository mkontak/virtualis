// $Id: vxViewerTypeVesselDetailMPROblique.C,v 1.1 2006/09/25 13:16:23 geconomos Exp $
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
#include "vxViewerTypeVesselDetailMPROblique.h"
#include "vxEnvironment.h"
#include "vxManipulatorVesselDetailMPROblique.h"
#include "vxVesselUtilities.h"
#include "vxVessel.h"


// defines
#define FILE_REVISION "$Revision: 1.1 $"


/**
 * Constructor
 */
vxViewerTypeVesselDetailMPROblique::vxViewerTypeVesselDetailMPROblique( vxEnvironment & environment )
: vxViewerTypeMPROblique( environment )
{
} // vxViewerTypeVesselDetailMPROblique()


/**
 * Initializes the view
 */
void vxViewerTypeVesselDetailMPROblique::InitializeView()
{
  vxViewerTypeMPROblique::InitializeView();
 
} // InitializeView()


/**
 * initializes the view/object in view based on an annotation
 *
 * @param annotation the annotation to initialize on
 */
bool vxViewerTypeVesselDetailMPROblique::InitializeFrom( vxAnnotation & annotation )
{

  // keep track of the original section index to restore it if we can't find a more suitable one
  float4 fDesiredSectionIndex( vxManipulatorVesselDetailMPROblique::GetVesselSectionIndex( GetEnvironment() ) );
  bool bSuccess( false );

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxViewerTypeVesselDetailMPROblique", "InitializeFrom" );
    return bSuccess;
  }

  // try to find a suitable index
  const float4 fMaxSectionIndex( pVessel->size()-1 );
  const float4 fSectionIndexIncrement( 0.25F );
  for ( float4 fIndex( 0.0F ); fIndex <= fMaxSectionIndex; fIndex += fSectionIndexIncrement )
  {

    // make sure all the handles are visible
    vxManipulatorVesselDetailMPROblique::SetVesselSectionIndex( GetEnvironment(), fIndex );
    bool bVisible( true );
    Triple<float4> units( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
    float fDelta( 0.5F * Min( units[0], Min( units[1], units[2] ) ) );
    const vxHandlesObj & handlesObject( annotation );
    for ( int i(0); i<handlesObject.GetNumHandles(); i++ )
    {
      if ( fabs( GetEnvironment().GetPlaneOblique()->GetDistance( handlesObject.GetHandle(i).GetPosition() ) ) > fDelta )
      {
        bVisible = false;
      }
    }

    if ( bVisible == true)
    {
      fDesiredSectionIndex = fIndex;
      bSuccess = true;
    }
  }

  // set it to the desired section (either the original one or a visible one)
  vxManipulatorVesselDetailMPROblique::SetVesselSectionIndex( GetEnvironment(), fDesiredSectionIndex );

  return bSuccess;

} // InitializeFrom()


// $Log: vxViewerTypeVesselDetailMPROblique.C,v $
// Revision 1.1  2006/09/25 13:16:23  geconomos
// moved from vascular and renamed
//
// Revision 1.5  2006/09/21 15:05:19  geconomos
// reworked initialize view
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
// Revision 1.4  2004/04/26 19:03:48  michael
// Need to pass in the ROI to setup the camera
//
// Revision 1.3  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.2  2004/03/02 19:19:35  frank
// renamed method in viewer type to more accurately describe it
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2004/03/02 01:10:29  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.5.2.1  2004/02/25 15:00:57  frank
// formatting
//
// Revision 1.5  2004/01/23 03:58:06  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.4  2004/01/14 14:18:16  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.3  2003/12/11 20:08:36  frank
// Issue #3569: Showed the annotation on the oblique view when you click on them in the pane
//
// Revision 1.2  2003/05/30 12:36:15  frank
// Initialized the oblique zoom
//
// Revision 1.1  2003/05/29 14:04:01  frank
// Initialized the oblique slice on the first vessel cross-section
//
// $Id: vxViewerTypeVesselDetailMPROblique.C,v 1.1 2006/09/25 13:16:23 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerTypeVesselDetailMPROblique.C,v 1.1 2006/09/25 13:16:23 geconomos Exp $
