// $Id: vxRendererSliceShadowsMPRCurved.h,v 1.3 2007/03/02 21:19:53 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// pragma declarations
#pragma once


// include declarations
#include "vxRendererSliceShadows.h"


/** 
 * Renders shadows for ortho/oblique detail planes for ortho mpr views
 */
class VX_VIEWER_DLL vxRendererSliceShadowsMPRCurved : public vxRendererSliceShadows
{
// member functions
public:

  /// constructor
  vxRendererSliceShadowsMPRCurved( vxEnvironment & environment );

  ///  renders the slice shadows
  virtual void Render();
 
private:

  /// renders the endoluminal camera position and direction 
  void RenderVesselDetailEndoluminal( const vxEnvironment & otherEnvironment );

  /// renders the the position and direction projection of an oblique or ortho plane
  void RenderVesselDetailPlane( const vxEnvironment & otherEnvironment );

}; // class vxRendererSliceShadowsMPRCurved


// $Log: vxRendererSliceShadowsMPRCurved.h,v $
// Revision 1.3  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.2  2006/11/13 14:39:08  geconomos
// made sure that slice shadows always update
//
// Revision 1.1  2006/10/03 20:02:17  geconomos
// continued reworking of slice shadows
//
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.2  2005/06/08 15:01:52  vxconfig
// formatting
//
// Revision 1.1  2001/01/03 20:29:49  michael
// Renamed vxRenderer{*}SliceShadows to vxRenderersliceShadows{*} so that
// it is consistent with the overall hierarchical class naming convention.
//
// Revision 1.2  2004/10/23 10:25:01  frank
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
// Revision 1.5  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.4  2003/06/12 20:09:56  wenli
// render the camera and oblique plane to solve the Track#2792
//
// Revision 1.3  2003/05/22 15:06:48  frank
// Changed inheritance
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.1  2003/01/28 23:20:57  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererSliceShadowsMPRCurved.h,v 1.3 2007/03/02 21:19:53 geconomos Exp $
// $Id: vxRendererSliceShadowsMPRCurved.h,v 1.3 2007/03/02 21:19:53 geconomos Exp $