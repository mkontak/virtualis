// $Id: vxRendererOverlayCArm.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
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
 * Class to display the text messages of the measured vessel
 */
class VX_VIEWER_DLL vxRendererOverlayCArm : public vxRendererOverlay
{

// Member Functions
public:

  /// Constructor
  vxRendererOverlayCArm(vxEnvironment & environment);

  /// Render overlay text
  virtual void Render();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

}; // class vxRendererOverlayVesselCArm


// $Log: vxRendererOverlayCArm.h,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:34  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 15:00:39  vxconfig
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
// Revision 1.2  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.1  2003/09/08 19:03:56  frank
// Moved C-arm style information text to separate renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlayCArm.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlayCArm.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
