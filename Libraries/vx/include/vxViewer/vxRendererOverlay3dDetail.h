// $Id: vxRendererOverlay3dDetail.h,v 1.1 2004/10/26 13:17:29 vxconfig Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

#ifndef vxRendererOverlay3dDetail_h
#define vxRendererOverlay3dDetail_h


// includes
#include "vxRendererOverlay3D.h"


/**
 * Render the 3D overlay information.
 */
class VX_VIEWER_DLL vxRendererOverlay3dDetail: public vxRendererOverlay3D
{
// functions
public:

  /// constructor
  vxRendererOverlay3dDetail(vxEnvironment & environment);
  
  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// renders 3D Overlay 
  virtual void Render();

private:

  /// copy constructor
  vxRendererOverlay3dDetail(const vxRendererOverlay3dDetail &);                    // should only be public if really implemented!

  /// assignment operator
  const vxRendererOverlay3dDetail & operator =(const vxRendererOverlay3dDetail &); // should only be public if really implemented!

}; // class vxRendererOverlay3dDetail


#endif


// Revision History:
// $Log: vxRendererOverlay3dDetail.h,v $
// Revision 1.1  2004/10/26 13:17:29  vxconfig
// moved from colon plugin
//
// Revision 1.2  2004/10/25 23:42:57  michael
// now included in vxViewer ...
//
// Revision 1.1  2004/10/21 17:54:04  michael
// added detail overlay renderer
//
// $Id: vxRendererOverlay3dDetail.h,v 1.1 2004/10/26 13:17:29 vxconfig Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlay3dDetail.h,v 1.1 2004/10/26 13:17:29 vxconfig Exp $
