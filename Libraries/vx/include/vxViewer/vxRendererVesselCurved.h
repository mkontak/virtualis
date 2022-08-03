// $Id: vxRendererVesselCurved.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
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
#include "vxRendererOverlay.h"


// forward declarations
class vxVessel;
class RaySegment;
class vxRibbon;


class VX_VIEWER_DLL vxRendererVesselCurved : public vxRendererOverlay
{
// Member Functions
public:

  /// Constructor
  vxRendererVesselCurved( vxEnvironment & environment );

  /// Destructor
  virtual ~vxRendererVesselCurved();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// Render
  virtual void Render();

private:

  /// Render the centerline
  void RenderCenterline() const;

  /// Render the contours
  void RenderContours() const;

  /// Get the intersection of the centerline with a screen scanline.
  float4 GetCenterlineXLocationFromScreenY( vxRibbon * pRibbon, const uint4 uScreenY ) const;

  /// Get the intersection of the centerline with an image scanline.
  float4 GetCenterlineXLocationFromImageY( vxRibbon * pRibbon, const uint4 uImageY ) const;

  /// Render the selection handles
  void RenderSelectionHandles() const;

  /// Render a single selection handle
  void RenderSelectionHandle( vxRibbon * pRibbon, const float4 fSelectionIndex, const std::string & sDisplayName ) const;

  /// Get the screen coordinate corresponding to an image position
  float4 GetScreenYFromImageY( vxRibbon * pRibbon, const float4 imageY ) const;

  /// Get the image coordinate corresponding to a screen position
  float4 GetImageYFromScreenY( vxRibbon * pRibbon, const float4 screenY ) const;

}; // class vxRendererVesselCurved


// $Log: vxRendererVesselCurved.h,v $
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.4  2005/06/08 15:02:17  vxconfig
// formatting
//
// Revision 1.3  2005/06/07 18:13:20  vxconfig
// removed the vessel array
//
// Revision 1.2  2004/10/23 10:26:01  frank
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
// Revision 1.3  2003/12/18 17:05:32  frank
// Optimizing redraws
//
// Revision 1.2  2003/05/19 17:24:58  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.7  2003/05/02 18:11:59  frank
// code review
//
// Revision 1.6  2003/03/13 14:11:04  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.5  2003/02/18 17:58:06  frank
// removed dead code
//
// Revision 1.4  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.3  2003/02/11 21:47:37  frank
// Rendered selection handles
//
// Revision 1.2  2003/02/11 18:15:45  frank
// Cleaned up centerline computation.
//
// Revision 1.1.2.1  2003/02/12 23:10:19  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.1  2003/01/28 15:06:00  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererVesselCurved.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
// $Id: vxRendererVesselCurved.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
