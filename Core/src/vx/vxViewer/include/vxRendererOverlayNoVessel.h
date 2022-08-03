// $Id: vxRendererOverlayNoVessel.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxRendererOverlay.h"


// forwards
class vxEnvironment;


/**
 * To display messages whether there is no avaliable vessel.
 */
class VX_VIEWER_DLL vxRendererOverlayNoVessel : public vxRendererOverlay
{

// Member Functions
public:

  /// Constructor.
  vxRendererOverlayNoVessel(vxEnvironment & environment);

  /// Render 2D overlay.
  virtual void Render();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

}; // class vxRendererOverlayNoVessel


// $Log: vxRendererOverlayNoVessel.h,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/21 15:09:11  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 15:01:32  vxconfig
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
// Revision 1.3  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.2  2003/05/01 17:48:55  wenli
// code reviewed
//
// Revision 1.1  2002/11/21 02:31:55  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlayNoVessel.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlayNoVessel.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
