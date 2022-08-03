// $Id: vxRendererSoftwareMPRCurvedProjected.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


/**
 *	render the Curved Projected MPR image
 */

// includes
#include "vxRendererSoftwareMPRCurved.h"



class VX_VIEWER_DLL vxRendererSoftwareMPRCurvedProjected : public vxRendererSoftwareMPRCurved
{
// Member Functions
public:

  /// Constructor.
  vxRendererSoftwareMPRCurvedProjected(vxEnvironment & environment);

  /// Destructor
  virtual ~vxRendererSoftwareMPRCurvedProjected();

  /// Perform pre-rendering operations
  virtual int4 PreRender();

  virtual int4 RenderWindow(const BoundingRect<uint2> & window, const uint1 & uThreadID);

protected:

  /// Setup the ends of the scanline.
  virtual void SetupScanlines();

  /// Render the centerline for one scanline.
  void RenderCenterlineSpan( const int4 iScanline, const float4 fDepthZ, Image<float4> & zBuffer, const std::pair< float4, float4 > & ySpan );

// Data Members
private:

}; // class vxRendererSoftwareMPRCurvedProjected


// $Log: vxRendererSoftwareMPRCurvedProjected.h,v $
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.2  2005/06/08 15:02:09  vxconfig
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
// Revision 1.4  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.3  2003/05/22 14:56:32  dongqing
// code review
//
// Revision 1.2  2003/05/21 20:44:48  wenli
// code review
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.3  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.2  2003/02/11 20:07:25  frank
// Moved function to CurvedProjected
//
// Revision 1.1.2.1  2003/02/12 23:10:19  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.1  2003/01/21 20:24:48  frank
// Initial version.
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererSoftwareMPRCurvedProjected.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
// $Id: vxRendererSoftwareMPRCurvedProjected.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
