// $Id: vxRendererOverlayVessel.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
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
class VX_VIEWER_DLL vxRendererOverlayVessel : public vxRendererOverlay
{

// Member Functions
public:

  /// Constructor
  vxRendererOverlayVessel(vxEnvironment & environment);
  
  /// Render 2D overlay
  virtual void Render();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

}; // class RendererOverlayVessel


// $Log: vxRendererOverlayVessel.h,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/14 20:09:32  geconomos
// moved from vascular project
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
// Revision 1.4  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.3  2003/09/08 18:36:55  frank
// Moved pre-processing information text to separate renderer
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.5  2003/05/01 17:48:55  wenli
// code reviewed
//
// Revision 1.4  2003/01/28 15:09:09  frank
// comments
//
// Revision 1.3  2003/01/16 21:27:32  frank
// code review
//
// Revision 1.2  2002/11/19 20:21:31  frank
// Added maximum tortuosity
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.2  2002/08/23 14:12:46  frank
// Rounded significant digits
//
// Revision 1.1  2002/08/22 02:58:44  frank
// Intial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlayVessel.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlayVessel.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
