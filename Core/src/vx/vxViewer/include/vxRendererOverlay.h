// $Id: vxRendererOverlay.h,v 1.6.8.1 2009/03/23 15:42:52 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxRendererOverlay_h
#define vxRendererOverlay_h


// Includes
#include "vxRenderer.h"


// Forwards
class vxEnvironment;


/*
 * Base class for the rendering of overlay text.  Holds functionality
 * common to 2D and 3D overlays.
 */
class VX_VIEWER_DLL vxRendererOverlay : public vxRenderer
{
// functions
public:

  /// constructor
  vxRendererOverlay( vxEnvironment & environment );

  /// destructor
  virtual ~vxRendererOverlay() {}
  
  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// Render the overlay
  virtual void Render();

// utility functions
public:

	std::string GetVoxelString( Vector3D<int4> pos );

	/// return the World cordinate as String
	std::string GetWorldCoordinateAsString();

	/// return the Volume coordinate as string.
	std::string GetVolCoordinateAsString();

  /// return the patient name as a string
  std::string GetPatientNameString();

  /// return the patient id as a string
  std::string GetPatientIdString();

  /// return the patient birth date as a string
  std::string GetPatientBirthDateString();

  /// return the patient age as a string
  std::string GetPatientAgeString();

  /// return the patient sex as a string
  std::string GetPatientSexString();

  /// return the peak voltage as a string
  std::string GetPeakVoltageString();

  /// return the current as a string
  std::string GetCurrentAsString();

  /// return the slice thickness as a string
  std::string GetSliceThicknessString();

  /// return the window/level as a string
  std::string GetWindowLevelString();

  /// return the field of view as a string
  std::string GetFieldOfViewString();

  /// return the value of the current voxel as a string.
  std::string GetCurrentVoxelString();

  /// return the study date as a string.
  std::string GetStudyDateString();

  /// get a string that represents the RAO or LAO angulation as if it was a C-arm
  std::string GetRaoLaoString();

  /// get a string that represents the cranial or caudal angulation as if it was a C-arm
  std::string GetCranCaudString();

  /// setup the OpenGL environment for overlay drawing
  void SetupRenderState();

  /// restore the OpenGL environment after overlay drawing
  static void RestoreRenderState();

  /// renders the orientation letters for a 3D
  void Render3DOrientationLettering();

  /// render the patient information in the upper left
  void RenderPatientInformation();

  /// render modality-specific information in the lower left
  void RenderModalityInformation();

  /// display the rendering modes for debugging purposes
  void DisplayRenderingModes();

private:

  /// copy constructor
  vxRendererOverlay( const vxRendererOverlay & );                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererOverlay & operator=( const vxRendererOverlay & ); // should only be public if really implemented!

// data
protected:

  /// spacing between consecutive lines of text
  float4 m_fInterlineSpacing;

  /// how much to multiply the font height to derive the interline spacing
  static const float4 m_fFontHeightFactor;

  /// offset from the sides of the window
  float4 m_fSideSpace;
  
  /// color of normal text around the edges
  Triple<float4> m_textColorNormal;

  /// color of the highlighted text
  Triple<float4> m_textColorHighlighted;

  /// height of the viatronix logo
  static const int4 m_iLogoHeight;

}; // class vxRendererOverlay


#endif // vxRendererOverlay_h


// $Log: vxRendererOverlay.h,v $
// Revision 1.6.8.1  2009/03/23 15:42:52  gdavidson
// Replaced the Viatronix text overlay with an image
//
// Revision 1.6  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.5  2006/10/27 07:44:54  romy
// HU display for neighbours added for Zoom > 250
//
// Revision 1.4  2006/10/26 17:40:02  romy
// added Methods to return volume and world cordinates as strings
//
// Revision 1.3  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.6.1  2005/08/04 18:33:32  frank
// debugging code
//
// Revision 1.2.12.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.2  2004/10/23 10:23:52  frank
// made some functions not static
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.32  2004/01/07 18:30:17  frank
// Removed dead code
//
// Revision 1.31  2004/01/05 15:41:12  michael
// fixed missing redraw sensitivity of the overlay renderers. Added base class
// to be returning true in RedrawNecessary() if font has changed and made
// derived classes call base class plus their additional stuff.
//
// Revision 1.30  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.29  2003/11/19 01:36:54  frank
// Centralized a rendering constant to avoid duplication
//
// Revision 1.28  2003/11/04 15:47:40  michael
// accounting for the changes in the overlay renderers (inheritance and naming)
//
// Revision 1.27  2003/09/04 17:29:45  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.26  2003/08/28 16:50:57  frank
// Fixed orientation lettering in 3D for some cases.
//
// Revision 1.25  2003/08/22 19:24:01  frank
// Issue #3460: Revised orientation lettering method - unified between 2D and 3D
//
// Revision 1.24.2.1  2003/08/29 15:20:11  frank
// Fixed orientation lettering in 3D
//
// Revision 1.24  2003/07/09 19:08:51  frank
// Added string for cran/caud and rao/lao angulation
// we do not yet have a definitive definition of the angles so it is still subject to change
//
// Revision 1.23  2003/05/22 13:04:29  frank
// code review
//
// Revision 1.22  2003/05/19 14:40:22  frank
// Removed unused return parameter
//
// Revision 1.21  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.20  2003/05/05 16:04:35  frank
// Refactored overlay rendering code so it can be easily re-used
//
// Revision 1.19  2003/05/01 13:35:37  frank
// Made two utility functions static so others can use them as well.
//
// Revision 1.18  2003/04/10 12:04:18  frank
// Added Render method to render all overlay strings (eventually)
//
// Revision 1.17  2003/01/13 23:25:46  frank
// Removed old parameters, fixed text layout
//
// Revision 1.16  2003/01/08 21:48:27  frank
// Added additional study info to overlays.
//
// Revision 1.15  2002/11/22 01:46:51  frank
// Added current HU/Raw display as string function to remove tons of redundant code.
//
// Revision 1.14  2002/09/30 17:07:46  frank
// Issue #2695: Cleaned up phase number and patient information opn phase renderer overlay display.
//
// Revision 1.13  2002/09/24 19:41:14  frank
// fixed issue #2675
//
// Revision 1.12  2002/09/24 15:32:49  frank
// Added explanation for missing images when the slab or slice is outside of the current crop box.
//
// Revision 1.11  2002/08/22 15:02:57  frank
// update
//
// Revision 1.10  2002/08/17 12:31:56  frank
// Centralized display of strings in the upper two corners. Now you just add strings to the two lists and they are rendered for you by RenderCornerStrings.  No more hand formatting.
//
// Revision 1.9  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.8  2002/06/12 23:19:45  frank
// Moved DICOM string functions back to RendererOverlay.
//
// Revision 1.7  2002/05/22 19:59:04  jenny
// fixed orientation lettering and removed a bunch of functions to process the age, patient id, sex and so on, put them in VolumeHeader
//
// Revision 1.6  2002/04/29 18:10:50  frank
// Added patient age.
//
// Revision 1.5  2002/04/27 19:47:03  frank
// Added some scan information.
//
// Revision 1.4  2002/04/27 04:20:54  frank
// Began centralizing the patient information block.
//
// Revision 1.3  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.2  2002/04/25 19:39:58  frank
// Centralized orientation lettering in vxRendererOverlay class.
//
// Revision 1.1  2002/04/24 19:20:57  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlay.h,v 1.6.8.1 2009/03/23 15:42:52 gdavidson Exp $
// $Id: vxRendererOverlay.h,v 1.6.8.1 2009/03/23 15:42:52 gdavidson Exp $
