// $Id: vxRendererOverlayMPROrtho.h,v 1.3 2007/02/15 00:08:23 jmeade Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: jenny hizver (mailto:jenny@viatronix.com)

#ifndef vxRendererOverlayMPROrtho_h
#define vxRendererOverlayMPROrtho_h


// includes
#include "vxRendererOverlayMPR.h"


// forward declarations
class vxEnvironment;


/**
 * Renders the overlay on orthogonal MPR views
 */
class VX_VIEWER_DLL vxRendererOverlayMPROrtho : public vxRendererOverlayMPR
{
// functions
public:

  /// constructor
  vxRendererOverlayMPROrtho(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererOverlayMPROrtho() {}
  
  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// render 2D overlay
  virtual void Render();

protected:

  /// return the slice type text string
  virtual std::string GetSliceTypeString();

  /// return the current slice number as a string
  virtual std::string GetSliceNumberString();

  /// determine if the current slice (or slab) intersects the crop box and display message if not
  virtual void ExplainMissingImage();

private:
	/// return the axial slice dicom z position string
	std::string GetAxialSliceDicomPositionString();

	///Adds the HU overlay string element
	void AddHUOverlayElement( Vector3Df voxelPos );

	///Finds and Adds the HU overlay string elements
	void AddHUOverlayElements();
	
  /// copy constructor
  vxRendererOverlayMPROrtho(const vxRendererOverlayMPROrtho & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererOverlayMPROrtho & operator=(const vxRendererOverlayMPROrtho & other); // should only be public if really implemented!

  /// display the ruler unit length information on top of the 2D ruler
	void RenderRulerUnitLength();


// data members
protected:

  /// timer for cine mode
  Timer m_cineTimer;

}; // class vxRendererOverlayMPROrtho


#endif // vxRendererOverlayMPROrtho_h


// $Log: vxRendererOverlayMPROrtho.h,v $
// Revision 1.3  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.2  2006/10/27 07:44:54  romy
// HU display for neighbours added for Zoom > 250
//
// Revision 1.1.1.1.18.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.2  2003/12/09 16:08:18  frank
// Fixed cine frames per second
//
// Revision 1.1  2003/11/04 15:46:40  michael
// added new overlay classes that follow the same inheritance hierachy as the
// view types, manipulators, and renderers.
//
// Revision 1.9  2003/05/19 14:39:21  frank
// Issue #3249: Slice out of bounds message improperly shown
//
// Revision 1.8  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.7  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.6  2003/01/21 22:15:10  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.5  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.4  2002/09/24 15:32:49  frank
// Added explanation for missing images when the slab or slice is outside of the current crop box.
//
// Revision 1.3  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.2  2002/06/06 18:01:30  dmitry
// Cine speed (fps) displayed when it is active.
//
// Revision 1.1  2002/04/26 13:10:50  michael
// Added renamed version of the overlay renderers by moving {2,3}D to the end
//
// Revision 1.7  2002/04/25 19:39:58  frank
// Centralized orientation lettering in vxRendererOverlay class.
//
// Revision 1.6  2002/04/24 03:03:31  michael
// added test if rendering is necessary (false if no volume available)
//
// Revision 1.5  2002/04/08 13:35:21  michael
// added missing privately non implemented copy constructor and assignment operator
//
// Revision 1.4  2002/03/21 20:08:17  jenny
// put comments
//
// Revision 1.3  2002/03/21 19:16:41  michael
// added log history lines
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlayMPROrtho.h,v 1.3 2007/02/15 00:08:23 jmeade Exp $
// $Id: vxRendererOverlayMPROrtho.h,v 1.3 2007/02/15 00:08:23 jmeade Exp $
