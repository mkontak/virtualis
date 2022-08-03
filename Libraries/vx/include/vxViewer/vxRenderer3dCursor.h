// $Id: vxRenderer3dCursor.h,v 1.3 2007/02/15 00:08:23 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Ingmar Bitter (ingmar@viatronix.com)
  
/**
 * This class defines the 3D cursor
 */

#ifndef vxRendererCursor3d_h
#define vxRendererCursor3d_h


// includes
#include "vxRenderer.h"


// class definition
class  VX_VIEWER_DLL vxRendererCursor3d : public vxRenderer
{
// functions
public:
  
  /// constructor
  vxRendererCursor3d(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererCursor3d() {}

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// perform rendering operations
  virtual void Render();
  
protected:

  /// copy constructor
  vxRendererCursor3d(const vxRendererCursor3d & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererCursor3d & operator=(const vxRendererCursor3d & other); // should only be public if really implemented!

  /// render the Cursor3d
  virtual void RenderCursor3d();

  /// render the Cursor3d
  virtual void RenderCrossHair();
}; // vxRendererCursor3d


#endif // vxRendererCursor3d_h


// Revision History:
// $Log: vxRenderer3dCursor.h,v $
// Revision 1.3  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.2.8.1  2006/05/05 13:05:40  geconomos
// added virtual destructors
//
// Revision 1.2  2005/03/21 20:48:14  michael
// Added function to render a cross hair on the MPR Ortho and MPR Oblique
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/12/17 18:02:55  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.9  2003/05/19 14:37:24  frank
// Removed unused return parameter
//
// Revision 1.8  2003/05/17 15:44:49  michael
// reformatting
//
// Revision 1.7  2003/05/16 13:22:13  frank
// formatting
//
// Revision 1.6  2003/05/14 18:30:03  dongqing
// code review
//
// Revision 1.5  2003/05/14 16:10:30  michael
// code review
//
// Revision 1.4  2002/11/14 14:49:00  geconomos
// Check of SHOW_3D_CURSOR rendering mode before rendering.
//
// Revision 1.3  2002/11/08 18:02:58  michael
// fixed world/index coordinates
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderer3dCursor.h,v 1.3 2007/02/15 00:08:23 jmeade Exp $
// $Id: vxRenderer3dCursor.h,v 1.3 2007/02/15 00:08:23 jmeade Exp $
