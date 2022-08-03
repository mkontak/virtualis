// $Id: vxRendererColormap.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (ingmar@viatronix.com)

/**
 * Define the color map for the reder
 */

#ifndef vxRendererColormap_h
#define vxRendererColormap_h


// includes
#include "vxRenderer.h"


// forward declarations
class vxControlPoint;


// class definition
class VX_VIEWER_DLL vxRendererColormap: public vxRenderer
{
// functions
public:

  // constructor
  vxRendererColormap(vxEnvironment & environment);

  // destructor
  virtual ~vxRendererColormap() {};

  /// called when the OpenGL drawing should take place
  virtual void Render();

}; // vxRendererColormap


#endif // vxRendererColormap_h


// Revision History:
// $Log: vxRendererColormap.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/05/19 14:38:02  frank
// Removed unused return parameter
//
// Revision 1.8  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.7  2003/05/16 13:22:14  frank
// formatting
//
// Revision 1.6  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.5  2003/05/14 18:30:04  dongqing
// code review
//
// Revision 1.4  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.3  2003/01/21 22:14:39  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.2  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.1  2002/10/03 21:11:50  michael
// Added renamed renderer ...
//
// Revision 1.6  2002/10/03 21:00:37  michael
// Added renamed renderer ...
//
// Revision 1.4  2002/09/10 20:37:56  michael
// Fixed "foreward" --> "forward"
//
// Revision 1.3  2002/09/10 15:36:35  michael
// removed redundant includes and added some formatting
//
// Revision 1.2  2002/09/09 21:31:59  frank
// Working on color map editor
//
// Revision 1.1  2002/09/06 23:06:18  ingmar
// new TFE without sampling effect renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererColormap.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxRendererColormap.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
