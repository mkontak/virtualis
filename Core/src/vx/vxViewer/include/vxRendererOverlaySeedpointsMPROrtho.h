// $Id: vxRendererOverlaySeedpointsMPROrtho.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

/**
 * Renders the vessel seedpoints in 2D orthographic views
 */


// pragmas
#pragma once


// includes
#include "vxRendererOverlay.h"


// forward declarations
class vxVessel;
template <class T> class PixelRGB;


class VX_VIEWER_DLL vxRendererOverlaySeedpointsMPROrtho : public vxRendererOverlay
{
// Member Functions
public:

  /// Constructor
  vxRendererOverlaySeedpointsMPROrtho(vxEnvironment & environment);

  /// Destructor
  virtual ~vxRendererOverlaySeedpointsMPROrtho();

  /// Render
  virtual void Render();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

private:

  /// Get the translation necessary to move a point from original world to detail world coordinates.
  Vector3Df GetDetailTranslation() const;

  /// Render the seedpoints
  void RenderSeedpoints() const;

  /// Render a circle
  void RenderCircle( const Point3Df & worldPos, const float4 fRadius, const PixelRGB<float4> & color ) const;

  /// Render a seedpoint and label
  void RenderSeedpoint( const int4 uSeedNumber, const Point3Df & worldPos,
                        const float4 fRadius, const PixelRGB<float4> & color ) const;

// Data Members
private:

  /// The translation necessary to move a point from original world to detail world coordinates
  Vector3Df m_vDetailTransform;

  /// The OpenGL viewport
  GLint m_viewport[4];

  /// The OpenGL modelview matrix
  GLdouble m_mModelviewMatrix[16];

  /// The OpenGL projection matrix
  GLdouble m_mProjectionMatrix[16];

}; // class vxRendererOverlaySeedpointsMPROrtho 


// $Log: vxRendererOverlaySeedpointsMPROrtho.h,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:34  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 15:01:32  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1  2004/05/26 12:23:15  frank
// initial version
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2004/01/13 19:14:35  frank
// code review
//
// Revision 1.5  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.4  2003/10/30 17:20:31  wenli
// Issue # 3544 : The centerline numbers can hide from the 2D images on Vessel select layout..
//
// Revision 1.3  2003/10/07 17:04:16  frank
// Issue #3533: Added seedpoint numbers
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.3  2003/05/01 20:25:57  frank
// code review
//
// Revision 1.2  2003/03/28 12:55:32  frank
// Clarified parameter name
//
// Revision 1.1  2003/03/20 21:29:44  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlaySeedpointsMPROrtho.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlaySeedpointsMPROrtho.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
