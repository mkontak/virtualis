// $Id: vxRendererSoftwareMPRCurvedLuminal.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxRendererSoftwareMPRCurved.h"


/**
 * Renderer for the curved luminal view (the straightened curved MPR)
 */
class VX_VIEWER_DLL vxRendererSoftwareMPRCurvedLuminal : public vxRendererSoftwareMPRCurved
{
// member functions
public:

  /// constructor
  vxRendererSoftwareMPRCurvedLuminal( vxEnvironment & environment );
  
  /// destructor
  virtual ~vxRendererSoftwareMPRCurvedLuminal();
  
  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// perform pre-rendering operations
  virtual int4 PreRender();

  /// this is ONLY for image space parallel algorithms
  virtual int4 RenderWindow( const BoundingRect<uint2> & window, const uint1 & uThreadID );
  
protected:
  
  /// render a MIP slab
  int4 RenderMIP( const BoundingRect<uint2> & renderWindow, vxEnvironment & renderEnv, bool bSlab = false );  

  /// render a normal thin slice
  int4 RenderSingleLayer( const BoundingRect<uint2> & renderWindow, vxEnvironment & renderEnv );

  /// render a MIP slab
  int4 RenderXRay( const BoundingRect<uint2> & renderWindow, vxEnvironment & renderEnv );

  /// setup the ends of the scanline
  virtual void SetupScanlines();

}; // class vxRendererSoftwareMPRCurvedLuminal 


// $Log: vxRendererSoftwareMPRCurvedLuminal.h,v $
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.3  2005/06/08 15:02:09  vxconfig
// formatting
//
// Revision 1.2  2004/12/22 14:12:41  frank
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1  2004/05/26 12:23:14  frank
// initial version
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/12/18 17:05:32  frank
// Optimizing redraws
//
// Revision 1.4  2003/11/18 15:54:03  wenli
// 3D Biconvex Slab MIP
//
// Revision 1.3  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.2  2003/06/24 11:50:24  frank
// formatting
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.2  2002/08/19 14:37:43  wenli
// initial version - Vascular
//
// Revision 1.1  2002/08/15 14:26:58  wenli
// initial version - Vascular
//
// Revision 1.1.2.4  2002/05/23 18:45:13  frank
// Moved to Ribbon data structure.
//
// Revision 1.1.2.3  2002/05/15 16:22:17  frank
// Cleaned up image sizes.
//
// Revision 1.1.2.2  2002/05/13 20:37:25  frank
// Initial version.
//
// Revision 1.1.2.1  2002/05/07 21:57:37  frank
// Initial version.
//
// Revision 1.3  2002/04/23 21:35:32  jenny
// not a template class any more
//
// Revision 1.2  2002/04/11 14:38:30  michael
// added missing private copy constructor and assignment operator
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererSoftwareMPRCurvedLuminal.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
// $Id: vxRendererSoftwareMPRCurvedLuminal.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
