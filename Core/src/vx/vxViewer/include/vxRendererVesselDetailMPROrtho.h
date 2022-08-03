// $Id: vxRendererVesselDetailMPROrtho.h,v 1.1 2006/09/25 13:17:34 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxRendererOverlay.h"


// forward declarations
class vxVessel;
class Quadrilateral;
class RaySegment;


class VX_VIEWER_DLL vxRendererVesselDetailMPROrtho : public vxRendererOverlay
{
// Member Functions
public:

  /// Constructor.
  vxRendererVesselDetailMPROrtho(vxEnvironment & environment);

  /// Destructor.
  virtual ~vxRendererVesselDetailMPROrtho();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// Render
  virtual void Render();

private:

  /// Render the centerline
  void RenderCenterline();

  /// Transform OpenGL for the detail volume
  Vector3Df GetDetailTransform() const;

}; // class vxRendererVesselDetailMPROrtho


// $Log: vxRendererVesselDetailMPROrtho.h,v $
// Revision 1.1  2006/09/25 13:17:34  geconomos
// moved from vascular
//
// Revision 1.4  2005/06/08 15:02:26  vxconfig
// formatting
//
// Revision 1.3  2005/06/07 18:13:20  vxconfig
// removed the vessel array
//
// Revision 1.2  2004/10/23 10:26:29  frank
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
// Revision 1.3  2003/05/05 13:58:19  frank
// code review
//
// Revision 1.2  2003/04/18 15:30:50  frank
// Working on vessel centerline
//
// Revision 1.1  2003/04/04 13:50:07  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererVesselDetailMPROrtho.h,v 1.1 2006/09/25 13:17:34 geconomos Exp $
// $Id: vxRendererVesselDetailMPROrtho.h,v 1.1 2006/09/25 13:17:34 geconomos Exp $
