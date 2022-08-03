// $Id: vxRendererOverlayVesselDetailMPROblique.C,v 1.4 2007/05/21 21:07:32 romy Exp $
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
#include "vxRendererOverlayVesselDetailMPROblique.h"
#include "vxEnvironment.h"
#include "vxOverlayStrings.h"
#include "Quadrilateral.h"
#include "vxViewerTypeVesselDetailMPROblique.h"
#include "vxElementFloat4.h"
#include "vxVesselUtilities.h"
#include "vxVessel.h"


// Defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Constructor.
 *
 * @param environment the environment
 */
vxRendererOverlayVesselDetailMPROblique::vxRendererOverlayVesselDetailMPROblique( vxEnvironment & environment )
: vxRendererOverlayMPROblique( environment )
{
} // Constructor


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererOverlayVesselDetailMPROblique::IsRedrawNecessary()
{

  const bool bBaseClassNeedsRedraw( vxRendererOverlayMPR::IsRedrawNecessary() );
  const bool bRenderingModeChanged( GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE ) );
  const bool bCurrentVoxelChanged( GetEnvironment().IsModified( vxEnvironment::CURRENT_VOXEL ) );
  const bool bPlaneChanged( GetEnvironment().IsModified( vxEnvironment::PLANE_MPR ) );

  return
    bBaseClassNeedsRedraw ||
    bRenderingModeChanged ||
    bCurrentVoxelChanged ||
    bPlaneChanged;

} // IsRedrawNecessary()


/**
 * Render 2D overlay.
 */
void vxRendererOverlayVesselDetailMPROblique::Render()
{

  // see if we are in an oblique MPR view
  if ( dynamic_cast< vxViewerTypeVesselDetailMPROblique * >( GetEnvironment().GetViewerType() ) == NULL )
  {
    std::string sViewerType( typeid( * GetEnvironment().GetViewerType() ).name() );
    throw ex::UnsupportedDataTypeException( LogRec( "Wrong viewer type in environment: " + sViewerType,
                                                    "vxRendererOverlayMPROblique", "Render" ) );
  }
  else
  {
    vxRendererOverlayMPROblique::Render();
  }

	// don't the FOV is the user don't want to see Image Information.
	if ( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION ) == false )
    return;

  // Add the field of view
  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );
  pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( GetFieldOfViewString(), vxOverlayStrings::vxOverlayElement::UpperRight ) );

} // Render()


/**
 * Return the current slice number as a string
 *
 * @param environment the environment
 *
 * @return the current field of view as a string
 */
std::string vxRendererOverlayVesselDetailMPROblique::GetFieldOfViewString() const
{

  // compute the field of view
  Quadrilateral quad( vxUtils::ComputeObliqueQuadrilateralAnalytical( GetEnvironment().GetCamera(), GetEnvironment().GetPlaneOblique(), GetEnvironment().GetViewerType() ) );
  float4 fFov( ( quad.GetPoint( Quadrilateral::TOP_LEFT ) - quad.GetPoint( Quadrilateral::TOP_RIGHT ) ).GetLength() );
  return "FOV: " + ToAscii( RoundDecimals( fFov, 1 ) ) + " mm^2";

} // GetFieldOfViewString()


/**
* Return the current slice number as a string
*
* @param environment the environment
*
* @return the current slice number as a string
*/
std::string vxRendererOverlayVesselDetailMPROblique::GetSliceNumberString()
{

  const std::string sDefault( "Image: ----/----" );

  // compose the slab thickness prefix
  const bool bMipMode( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::MIP );
  const float fThicknessMM( GetEnvironment().GetRenderingMode()->GetSlabThickness() );
  std::string sPrefix = bMipMode ? "MIP" : "Image";
  if ( bMipMode ) sPrefix = ToAscii( int4( fThicknessMM ) ) + "mm " + sPrefix;

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogWrn( "Vessel pointer is NULL", "vxRendererOverlayVesselDetailMPROblique", "GetSliceNumberString" );
    return sDefault;
  }

  // get the current vessel section
  vxShareableObject< vxElementFloat4 > * pCurrentVesselSection = dynamic_cast<vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ) );
  if( pCurrentVesselSection == NULL )
  {
    //LogWrn( "no current vessel section: ", "vxRendererOverlayVesselDetailMPROblique", "GetSliceNumberString" );
    return sDefault;
  }

  // compute the lengths
  const float4 fCurrentLength( pVessel->GetLength( vxVessel::MeasurePair( 0, pCurrentVesselSection->GetValue() ) ) );
  const float4 fOverallLength( pVessel->GetLength( pVessel->GetEntireDomain() ) );

  // render the string
  int1 sOutputString [201];
  sprintf( sOutputString, "%s: %3.1f/%3.1fmm", sPrefix.c_str(), RoundDecimals( fCurrentLength, 1 ), RoundDecimals( fOverallLength, 1 ) );
  return sOutputString;

} // GetSliceNumberString()


// undefines
#undef FILE_REVISION


// $Log: vxRendererOverlayVesselDetailMPROblique.C,v $
// Revision 1.4  2007/05/21 21:07:32  romy
// fov displays only if IMAGE_INFORMATION flag is true
//
// Revision 1.3  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.2  2006/10/05 17:01:05  geconomos
// updated for changes to ComputeIntersection
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.7  2006/08/17 15:52:30  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.6  2006/06/05 15:07:38  frank
// updated for visual studio 2005
//
// Revision 1.5  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.4.2.1  2005/08/19 14:14:05  frank
// spelling
//
// Revision 1.4  2005/06/08 15:00:59  vxconfig
// formatting
//
// Revision 1.3  2005/06/07 18:10:42  vxconfig
// removed the vessel array
//
// Revision 1.2  2004/10/05 14:43:29  frank
// moving the rendering mode out of the data renderer software 3D class
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
// Revision 1.3  2004/03/02 01:10:31  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.2.2.1  2004/02/25 15:00:01  frank
// formatting
//
// Revision 1.2  2004/01/14 18:18:41  frank
// Updated the current slice number to indicate the distance along the vessel
//
// Revision 1.1  2004/01/07 18:28:23  frank
// initial version
//
// Revision 1.8  2004/01/05 20:24:10  frank
// formatting
//
// Revision 1.7  2003/12/22 20:03:34  frank
// updated slice shadow data exchange
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
// Revision 1.3  2003/05/23 16:03:17  frank
// Displayed the field of view
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.12  2003/05/06 16:21:13  frank
// Removed HUConverter
//
// Revision 1.11  2003/05/05 16:05:53  frank
// Fixed double display of HU
//
// Revision 1.10  2003/04/23 14:02:29  dongqing
// code review
//
// Revision 1.9  2003/04/14 19:44:23  frank
// Moving over to overlay strings
//
// Revision 1.8  2003/03/01 13:20:26  frank
// GetWinRect() was removed from vxManipulator
//
// Revision 1.7  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.6  2003/02/10 17:15:56  frank
// removed position
//
// Revision 1.5.2.1  2003/02/12 23:10:18  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.5  2003/02/03 19:59:53  frank
// Removed duplicate text overlay
//
// Revision 1.4  2003/01/20 19:34:40  wenli
// New Version with updated centerline selection and Hessian Filter
//
// Revision 1.3  2002/11/22 01:46:12  frank
// Removed redundant code
//
// Revision 1.2  2002/11/19 20:20:53  frank
// fixed current voxel
//
// Revision 1.1  2002/11/15 18:28:07  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlayVesselDetailMPROblique.C,v 1.4 2007/05/21 21:07:32 romy Exp $
// $Id: vxRendererOverlayVesselDetailMPROblique.C,v 1.4 2007/05/21 21:07:32 romy Exp $
