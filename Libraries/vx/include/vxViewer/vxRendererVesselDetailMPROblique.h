// $Id: vxRendererVesselDetailMPROblique.h,v 1.2 2007/02/12 14:23:56 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxRendererOverlay.h"
#include "Quad.h"


// forward declarations
class vxVessel;
class vxVesselSection;
class Quadrilateral;
class RaySegment;


class VX_VIEWER_DLL vxRendererVesselDetailMPROblique : public vxRendererOverlay
{
// Member Functions
public:

  /// Constructor.
  vxRendererVesselDetailMPROblique(vxEnvironment & environment);

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// Render
  virtual void Render();

private:

  /// Render the centerline
  void RenderCenterline();

  /// Render the diameter glyphs
  void RenderDiameterGlyphs() const;

  /// Render one diameter glyph
  void RenderDiameterGlyph( const RaySegment & diameter ) const;

  /// Render a contour as lines
  void RenderContourLines( const float4 fSectionIndex, const Quad<float4> & baseColor, const bool bDrawLumen ) const;

  /// Retrieve the current vessel section
  vxVesselSection * GetCurrentVesselSection() const;

  /// Retrieve the oblique view quadrilateral
  Quadrilateral GetObliqueQuadrilateral() const;

  /// Transform OpenGL for the detail volume
  void TransformDetail() const;

private:
 
}; // class vxRendererVesselDetailMPROblique 


// $Log: vxRendererVesselDetailMPROblique.h,v $
// Revision 1.2  2007/02/12 14:23:56  geconomos
// added support for screen space rendering for oblique views
//
// Revision 1.1  2006/09/25 13:17:34  geconomos
// moved from vascular
//
// Revision 1.4  2005/06/08 15:02:17  vxconfig
// formatting
//
// Revision 1.3  2005/06/07 18:13:20  vxconfig
// removed the vessel array
//
// Revision 1.2  2004/10/23 10:26:15  frank
// made some functions not static
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/12/18 17:05:32  frank
// Optimizing redraws
//
// Revision 1.3  2003/05/19 17:24:58  frank
// Removed unused return parameter
//
// Revision 1.2  2003/05/14 13:54:28  frank
// Added the wall contour
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.8  2003/05/02 20:15:17  frank
// code review
//
// Revision 1.7  2003/05/02 19:50:49  jmeade
// Code walkthru.
//
// Revision 1.6  2003/04/11 18:39:45  frank
// Interpolated the oblique lumen cross section
//
// Revision 1.5  2003/01/24 20:48:34  frank
// Fixed vessel overlays.
//
// Revision 1.4  2003/01/21 22:18:04  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.3  2003/01/20 12:20:22  frank
// Fixed Point2D/Point problem.
//
// Revision 1.2  2002/11/13 22:07:20  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.1  2002/11/12 16:13:59  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererVesselDetailMPROblique.h,v 1.2 2007/02/12 14:23:56 geconomos Exp $
// $Id: vxRendererVesselDetailMPROblique.h,v 1.2 2007/02/12 14:23:56 geconomos Exp $
