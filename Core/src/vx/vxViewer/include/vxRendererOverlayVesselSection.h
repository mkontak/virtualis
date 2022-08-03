// $Id: vxRendererOverlayVesselSection.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
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


// forwards
class vxEnvironment;


/**
 * Tells information about the current vessel section.
 */
class VX_VIEWER_DLL vxRendererOverlayVesselSection: public vxRendererOverlay
{
// Member Functions
public:

  /// Constructor
  vxRendererOverlayVesselSection( vxEnvironment & environment );

  /// Render 2D overlay
  virtual void Render();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

}; // class RendererOverlayVessel


// $Log: vxRendererOverlayVesselSection.h,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:34  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 15:01:41  vxconfig
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
// Revision 1.5  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.4  2003/09/30 20:45:40  frank
// code review
//
// Revision 1.3  2003/05/21 13:26:18  frank
// code review
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.2  2003/01/16 21:27:47  frank
// code review
//
// Revision 1.1  2002/11/19 20:14:05  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlayVesselSection.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlayVesselSection.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
