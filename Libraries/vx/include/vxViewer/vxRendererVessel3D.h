// $Id: vxRendererVessel3D.h,v 1.3 2007/08/04 03:38:36 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * Renders the vessel centerline and outline in 3D.
 */


// pragmas
#pragma once


// includes
#include "vxRenderer.h"
#include "Quad.h"


// forward declarations
class vxVessel;
class RaySegment;


class VX_VIEWER_DLL vxRendererVessel3D : public vxRenderer
{
// Member Functions
public:

  /// Constructor
  vxRendererVessel3D(vxEnvironment & environment);

  /// Destructor
  virtual ~vxRendererVessel3D();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// Render
  virtual void Render();

private:

	//Render the arrow 
	void RendererArrow() const;

  /// Render the centerline
  void RenderCenterline() const;

  /// Render the contours
  void RenderContours() const;

  /// Render the area glyphs
  void RenderAreaGlyphs() const;

  /// Render the diameter glyphs
  void RenderDiameterGlyphs() const;

  /// Render one diameter glyph
  void RenderDiameterGlyph( const RaySegment & segment ) const;

  /// Render the maximum tortuosity glyph
  void RenderTortuosityGlyph() const;

  /// Render a contour as lines
  void RenderContourLines( const uint4 uSectionIndex, const Quad<float4> & baseColor ) const;

  /// Render a contour as points
  void RenderContourPoints( const uint4 uSectionIndex, const Quad<float4> & baseColor ) const;

  /// Render the connection between two contours
  void RenderContourConnection( const uint4 uSectionIndex, const Quad<float4> & baseColor ) const;

  /// Transform OpenGL for the detail volume
  void TransformDetail() const;

  /// Render the seedpoints
  void RenderSeedpoints() const;

  /// Render sphere wireframe
  void RenderSphereWireframe( const Point3Df & position, const float4 fRadius ) const;

}; // class vxRendererVessel3D 


// $Log: vxRendererVessel3D.h,v $
// Revision 1.3  2007/08/04 03:38:36  romy
// added RenderArrow method
//
// Revision 1.2  2006/09/20 14:33:43  geconomos
// moved from vascular
//
// Revision 1.3  2005/06/08 15:02:17  vxconfig
// formatting
//
// Revision 1.2  2005/06/07 18:13:20  vxconfig
// removed the vessel array
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/12/18 17:05:32  frank
// Optimizing redraws
//
// Revision 1.2  2003/08/21 13:08:37  michael
// added newline at end of file
//
// Revision 1.1  2003/08/13 16:07:46  frank
// Specialized for vascular
//
// Revision 1.3  2003/06/09 13:42:46  frank
// Added individual slice shadow renderer
//
// Revision 1.2  2003/05/19 17:24:58  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.10  2003/03/17 18:43:20  frank
// Rendered the seedpoints
//
// Revision 1.9  2003/03/13 14:12:22  frank
// Working on rendering the camera in 3D
//
// Revision 1.8  2003/01/24 20:50:00  frank
// Fixed vessel overlays.
//
// Revision 1.7  2003/01/21 22:17:20  frank
// code review
//
// Revision 1.6  2003/01/14 16:17:15  dongqing
// Code review
//
// Revision 1.5  2002/11/15 00:05:46  frank
// Rendered the curved/luminal shadow
//
// Revision 1.4  2002/11/08 03:05:51  frank
// Rendered the entire vessel contour as points
//
// Revision 1.3  2002/11/05 19:53:22  frank
// Rendered oblique slice shadow
//
// Revision 1.2  2002/11/04 14:54:10  frank
// Working on blending colors more
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.3  2002/10/30 23:13:37  frank
// Cleaned up vessel rendering.
//
// Revision 1.2  2002/10/29 18:27:28  frank
// Added the contour rendering
//
// Revision 1.1  2002/10/29 16:54:14  frank
// added vxRendererVessel3D
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererVessel3D.h,v 1.3 2007/08/04 03:38:36 romy Exp $
// $Id: vxRendererVessel3D.h,v 1.3 2007/08/04 03:38:36 romy Exp $
