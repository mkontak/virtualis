// $Id: vxRendererOverlay3D.C,v 1.13.8.1 2009/03/23 15:42:35 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererOverlay3D.h"
#include "vxEnvironment.h"
#include "GLEnv.h"
#include "GLFontGeorge.h"
#include "vxOverlayStrings.h"
#include "Vector3D.h"
#include "vxRenderingMode.h"


// defines
#define FILE_REVISION "$Revision: 1.13.8.1 $"


/**
 * Constructor
 *
 * @param environment the environemnt
 */
vxRendererOverlay3D::vxRendererOverlay3D( vxEnvironment & environment )
: vxRendererOverlay( environment )
//******************************************************************* 
{
} // vxRendererOverlay3D()


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererOverlay3D::IsRedrawNecessary()
//******************************************************************* 
{
  bool bRedrawNecessary(vxRendererOverlay::IsRedrawNecessary());

  bool bChangedDataset(GetEnvironment().IsModified(vxEnvironment::VOLUME_INTENSITY));
  bool bChangedColormap(GetEnvironment().IsModified(vxEnvironment::COLORMAP));

  bRedrawNecessary |= bChangedColormap;
  bRedrawNecessary |= bChangedDataset;

  return bRedrawNecessary;
} // IsRedrawNecessary()


/**
 * Render 3D Overlay
 */  
void vxRendererOverlay3D::Render()
{

  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >(  GetEnvironment().GetElement( vxIDs::OverlayStrings ) );

  if( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION ) )
  {
    pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( GetSliceTypeString(), vxOverlayStrings::vxOverlayElement::UpperRight ) );
    pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( GetWindowLevelString(), vxOverlayStrings::vxOverlayElement::UpperRight ) );
  }

  Render3DOrientationLettering();

  if( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION ) )
  {
    RenderPatientInformation();
  }

  if( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION ) )
  {
    RenderModalityInformation();
  }

} // Render()


/**
 * Return the slice type text string
 * @return the slice type text string
 */
std::string vxRendererOverlay3D::GetSliceTypeString()
{

  std::string sProjectionType( "<undefined>" );

  switch ( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() )
  {
  case vxRenderingMode::DEFAULT:
    sProjectionType = "3D VR";
    break;
  case vxRenderingMode::MIP:
    sProjectionType = "3D MIP";
    break;
  case vxRenderingMode::THIN_SLAB:
    sProjectionType = "3D VR";
    break;
  case vxRenderingMode::XRAY:
    sProjectionType = "3D x-ray";
    break;
  default:
    throw ex::IllegalArgumentException( LogRec( "Unknown projection type: " + ToAscii( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() ),
      "vxRendererOverlay3D", "Render" ) );
    break;
  }

  return sProjectionType;

} // GetSliceTypeString()


#undef FILE_REVISION


// $Log: vxRendererOverlay3D.C,v $
// Revision 1.13.8.1  2009/03/23 15:42:35  gdavidson
// Removed the Viatronix text overlay
//
// Revision 1.13  2007/02/05 19:28:40  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.12  2006/08/03 14:08:52  geconomos
// removed display of rendering engine diagnostic data
//
// Revision 1.11  2006/07/07 18:00:43  geconomos
// formatting of statistical information for rendering engine
//
// Revision 1.10  2006/07/07 13:42:45  geconomos
// updated for changes to vxRenderingNodeThreadManager interface
//
// Revision 1.9  2006/06/27 20:19:17  geconomos
// temporary code for analyzing render engine buffers
//
// Revision 1.8  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.7.2.1  2005/07/19 11:49:31  frank
// missing include
//
// Revision 1.7  2005/05/23 12:48:20  frank
// fixed file revision definition so that we get correct information in log files
//
// Revision 1.6  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.5.2.1  2005/02/08 16:16:46  frank
// fixed field of view display
//
// Revision 1.5  2004/10/23 10:23:34  frank
// made some functions not static
//
// Revision 1.4  2004/10/05 14:32:18  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.3  2004/04/28 15:51:12  frank
// added projection type in 3D view
//
// Revision 1.2  2004/04/06 16:26:41  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.41  2004/01/14 17:05:52  frank
// removed debug
//
// Revision 1.40  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.39  2004/01/05 15:41:12  michael
// fixed missing redraw sensitivity of the overlay renderers. Added base class
// to be returning true in RedrawNecessary() if font has changed and made
// derived classes call base class plus their additional stuff.
//
// Revision 1.38  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.37  2003/09/23 14:57:59  michael
// removed temporary code to compute Euler Angles for Shimadzu. It is now
// part of the V3D Angio plugin code.
//
// Revision 1.36  2003/09/02 17:55:53  michael
// made distance between lines (m_fInterlineSpacing) font size dependent
//
// Revision 1.35  2003/08/29 16:57:57  michael
// adjusted code to reflect discussion with Takeshi Ozaki (Shimadzu) but commented
// it out since it should not really be here, and it will soon be moved
//
// Revision 1.34  2003/08/13 12:55:44  michael
// accounted for change from OverlayStrings to vxOverlayStrings
//
// Revision 1.33  2003/08/07 19:38:33  michael
// added some stuff for Shimadzu (temporarily) but it stays commented out ...
//
// Revision 1.32  2003/06/18 15:19:58  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.31  2003/05/23 20:13:20  michael
// added image information display flag
//
// Revision 1.30  2003/05/23 18:56:41  frank
// Moved "show patient information" logic out of render function
//
// Revision 1.29  2003/05/22 13:04:29  frank
// code review
//
// Revision 1.28  2003/05/19 14:40:22  frank
// Removed unused return parameter
//
// Revision 1.27  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.26  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.25  2003/05/05 16:04:35  frank
// Refactored overlay rendering code so it can be easily re-used
//
// Revision 1.24  2003/05/02 19:01:09  frank
// Cleaned up dead code
//
// Revision 1.23  2003/05/02 18:27:06  frank
// Moved 2D viewer to more flexible overlay strings
//
// Revision 1.22  2003/05/01 19:59:47  frank
// Moved to using the more flexible overlay strings
//
// Revision 1.21  2003/05/01 14:35:29  frank
// reflected parameter change in vxRendererOverlay
//
// Revision 1.20  2003/04/18 15:21:02  frank
// Removed HUconverter
//
// Revision 1.19  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.18  2003/02/14 19:00:20  frank
// Fixed overlay for MR datasets
//
// Revision 1.17  2003/01/21 22:15:40  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.16  2003/01/08 21:48:28  frank
// Added additional study info to overlays.
//
// Revision 1.15  2001/01/09 13:03:25  frank
// Fixed text alignment
//
// Revision 1.14  2002/10/05 02:37:44  frank
// Fixed orientation lettering so it lines up and matches the rest of the overlays.
//
// Revision 1.13  2002/09/24 15:32:49  frank
// Added explanation for missing images when the slab or slice is outside of the current crop box.
//
// Revision 1.12  2002/09/09 20:48:03  michael
// Fixed clearing of text to be displayed
//
// Revision 1.11  2002/08/17 12:31:56  frank
// Centralized display of strings in the upper two corners. Now you just add strings to the two lists and they are rendered for you by RenderCornerStrings.  No more hand formatting.
//
// Revision 1.10  2002/06/17 22:39:06  soeren
// renamed GetSignificantBitPerVoxel to GetSignificantBitsPerVoxel
//
// Revision 1.9  2002/05/09 21:05:06  jenny
// corrected window/level display
//
// Revision 1.8  2002/05/08 22:10:27  jenny
// set font style to SHADOW_AROUND, got rid of rasterPos
//
// Revision 1.7  2002/05/07 19:57:21  michael
// fixed patient age and sex display in the viewers
//
// Revision 1.6  2002/05/01 19:37:55  jenny
// getting rid of volumeset
//
// Revision 1.5  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.4  2002/04/30 15:18:02  michael
// Moved display of window level to same position as in 2D overlay.
// It was currently hidden behind the orientation cube.
//
// Revision 1.3  2002/04/27 04:21:06  frank
// Began centralizing the patient information block.
//
// Revision 1.2  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.1  2002/04/26 13:10:50  michael
// Added renamed version of the overlay renderers by moving {2,3}D to the end
//
// Revision 1.24  2002/04/25 19:39:58  frank
// Centralized orientation lettering in vxRendererOverlay class.
//
// Revision 1.23  2002/04/24 03:03:31  michael
// added test if rendering is necessary (false if no volume available)
//
// Revision 1.22  2002/04/23 20:12:23  michael
// replaced Density by Intensity
//
// Revision 1.21  2002/04/23 15:39:57  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.20  2002/04/21 21:59:08  michael
// Re-fixed window level AGAIN since checked in code does not work
//
// Revision 1.19  2002/04/20 03:46:01  michael
// Added patient info to 3D overlay, as in 2D overlay
//
// Revision 1.18  2002/04/16 19:05:21  michael
// introduced vxWindowLevel class and use it throughout the code
//
// Revision 1.17  2002/04/01 23:26:01  michael
// Simplified function call by passing one instead if 4 parameters
//
// Revision 1.16  2002/04/01 16:37:09  michael
// push and pop of attributes happens once in Render()
//
// Revision 1.15  2002/03/27 16:10:05  michael
// cosmetics
//
// Revision 1.14  2002/03/27 15:07:56  michael
// adjusted Get.SetSize() to Get/SetViewportSize()
//
// Revision 1.13  2002/03/21 20:11:15  jenny
// put comments
//
// Revision 1.12  2002/03/21 18:52:32  michael
// less methods and other adjustments
//
// Revision 1.11  2002/03/20 16:40:38  jenny
// put some comments
//
// Revision 1.10  2002/03/20 15:49:29  michael
// fixed OpenGL matrixes. Push matrixes first and pop them back after rendering
//
// Revision 1.9  2002/03/20 11:45:48  michael
// font is nor anymore re-created
//
// Revision 1.8  2002/03/19 23:42:04  michael
// fixed window size using the winRect
//
// Revision 1.7  2002/03/18 19:30:42  jenny
// orientation letters
//
// Revision 1.6  2002/03/18 15:51:45  jenny
// rearranged the letters being mapped
//
// Revision 1.5  2002/03/14 17:30:49  jenny
// worked on GetOrientationLettering
//
// Revision 1.4  2002/03/13 18:56:37  michael
// removed spaces between glBegin and ()
//
// Revision 1.3  2002/03/11 20:59:49  michael
// removed compiler errors
//
// Revision 1.2  2002/03/11 20:51:24  michael
// less compiler errors
//
// Revision 1.1  2002/03/11 19:54:00  michael
// added frist draft of the 3D overlay renderer
//
// $Id: vxRendererOverlay3D.C,v 1.13.8.1 2009/03/23 15:42:35 gdavidson Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlay3D.C,v 1.13.8.1 2009/03/23 15:42:35 gdavidson Exp $
