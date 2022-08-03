// $Id: vxRendererPath3D.h,v 1.2 2004/07/28 19:37:33 michael Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxRendererPath3D_h
#define vxRendererPath3D_h


// includes
#include "vxRenderer.h"


// forward declarations
class vxVessel;
class RaySegment;
template <class Type> class Quad;
template <class Type> class vxPlaneOblique;


/**
 * Renders the vessel centerline and outline in 3D.
 */
class VX_VIEWER_DLL vxRendererPath3D : public vxRenderer
{
// functions
public:

  /// constructor
  vxRendererPath3D(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererPath3D();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// render
  virtual void Render();

private:

  /// copy constructor
  vxRendererPath3D(const vxRendererPath3D & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererPath3D & operator=(const vxRendererPath3D & other); // should only be public if really implemented!

  /// render the centerline
  void RenderCenterline() const;

  /// render resampled centerline
  void RenderResampledCenterline(const bool bBoundPlane, const vxPlaneOblique<float4> * pVisiblePlane, const float4 fPlaneDistance,
                                 const bool bBoundBox, const Box<float4> & visibleBox, const float4 fResampleInterval) const;

  /// retrieve the current path
  vxVessel * GetPath() const;

}; // class vxRendererPath3D 


#endif // vxRendererPath3D_h


// Revision History:
// $Log: vxRendererPath3D.h,v $
// Revision 1.2  2004/07/28 19:37:33  michael
// adjusted the path drawing to show it in a more understandable fashion
//
// Revision 1.1  2004/04/06 18:19:30  frank
// renamed vessel to path
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/01/16 13:19:14  frank
// Issue #3525: Added display of path on oblique slices/slabs
//
// Revision 1.3  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.2  2003/08/21 19:46:55  dongqing
// finialize the centerline renderer
//
// Revision 1.1  2003/08/12 16:57:31  frank
// Moved the vessel tools down to vxBase
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererPath3D.h,v 1.2 2004/07/28 19:37:33 michael Exp $
// $Id: vxRendererPath3D.h,v 1.2 2004/07/28 19:37:33 michael Exp $