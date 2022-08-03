// $Id: vxRendererPlaneObliqueCreate.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

/*
  Renders the vessel centerline and outline in 3D
*/

#ifndef vxRendererPlaneObliqueCreate_h
#define vxRendererPlaneObliqueCreate_h


// includes
#include "vxRenderer.h"
#include "vxAnnotation.h"


// class definition
class VX_VIEWER_DLL vxRendererPlaneObliqueCreate : public vxRenderer
{
// functions
public:

  /// constructor
  vxRendererPlaneObliqueCreate(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererPlaneObliqueCreate() {}

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// perform rendering operations
  virtual int4 PreRender();

  /// render
  virtual void Render();
}; // class vxRendererPlaneObliqueCreate 


#endif // vxRendererPlaneObliqueCreate_h


// Revision History:
// $Log: vxRendererPlaneObliqueCreate.h,v $
// Revision 1.2  2007/02/15 00:08:24  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.1.1.1.18.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/01/14 14:18:20  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.3  2003/09/22 21:12:34  michael
// changed points to two handles that can be rendered
//
// Revision 1.2  2003/09/22 15:55:43  michael
// added "mailto:"
//
// Revision 1.1  2003/08/19 15:22:34  michael
// added inital verions of renderer for creating an oblique plane
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererPlaneObliqueCreate.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
// $Id: vxRendererPlaneObliqueCreate.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
