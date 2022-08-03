// $Id: vxRendererOverlayIncrementalGrowth.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

// pragmas
#pragma once


// includes
#include "vxRendererOverlay.h"
#include "vxIncrementalGrowthImage.h"
#include "Point3D.h"


// forwards
class vxEnvironment;


/**
 * Class to render the incremental growth interface
 */
class VX_VIEWER_DLL vxRendererOverlayIncrementalGrowth: public vxRendererOverlay
{

// Member Functions
public:

  /// Constructor
  vxRendererOverlayIncrementalGrowth( vxEnvironment & environment );
  
  /// Render 2D overlay
  virtual void Render();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

private:

  /// Render the incremental growth image
  void RenderIncrementalGrowthImage();

  /// Render the slider
  void RenderSlider();

  /// Process all the voxels computed so far and splat to image
  void ProcessVoxels();

// Data Members
private:

  /// A count of the number of voxels processed into the image so far
  uint4 m_uNumVoxelsProcessed;

  /// The incremental growth image
  vxIncrementalGrowthImage m_growthImage;

}; // class RendererOverlayVessel


// $Log: vxRendererOverlayIncrementalGrowth.h,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:34  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 15:00:49  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.3  2003/11/20 14:22:50  frank
// Made easy select work on all views simultaneously
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.3  2003/05/01 20:08:43  frank
// code review
//
// Revision 1.2  2003/04/04 21:27:45  frank
// Made the number of voxels in the region interactive
//
// Revision 1.1  2003/04/04 13:01:29  frank
// Added the ability to incrementally grow a new component.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlayIncrementalGrowth.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlayIncrementalGrowth.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
