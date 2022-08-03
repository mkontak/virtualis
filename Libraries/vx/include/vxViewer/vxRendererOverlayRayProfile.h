// $Id: vxRendererOverlayRayProfile.h,v 1.4 2007/02/15 00:08:23 jmeade Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxRendererOverlayRayProfile_h
#define vxRendererOverlayRayProfile_h


// Includes
#include "vxRendererOverlay.h"


// Forward declarations
class vxEnvironment;


/**
 * Render the alpha intensity profile of the current ray in 3D.  This
 * is useful for debugging 3D picking operations, for instance.
 */
class VX_VIEWER_DLL vxRendererOverlayRayProfile : public vxRendererOverlay
{
// Member Functions
public:

  /// Constructor
  vxRendererOverlayRayProfile( vxEnvironment & environment );

  /// destructor
  virtual ~vxRendererOverlayRayProfile() {}
  
public:
  
  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// Render 2D overlay
  virtual void Render();

private:

  /// Copy constructor
  vxRendererOverlayRayProfile( const vxRendererOverlayRayProfile & other);                    // should only be public if really implemented!

  /// Assignment operator
  const vxRendererOverlayRayProfile & operator =( const vxRendererOverlayRayProfile & other); // should only be public if really implemented!

}; // class vxRendererOverlayRayProfile


#endif // vxRendererOverlayRayProfile_h


// $Log: vxRendererOverlayRayProfile.h,v $
// Revision 1.4  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.3  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.6.1  2005/08/05 13:06:14  frank
// added rendering necessary function
//
// Revision 1.2.12.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.2  2004/10/23 10:23:52  frank
// made some functions not static
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/27 14:12:00  michael
// completed code review
//
// Revision 1.4  2003/05/27 13:18:42  frank
// code review
//
// Revision 1.3  2003/05/19 14:40:22  frank
// Removed unused return parameter
//
// Revision 1.2  2003/01/21 22:16:23  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.1  2001/01/09 12:56:10  frank
// Added ray profile data used for sampling along a ray for 3D picking.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlayRayProfile.h,v 1.4 2007/02/15 00:08:23 jmeade Exp $
// $Id: vxRendererOverlayRayProfile.h,v 1.4 2007/02/15 00:08:23 jmeade Exp $
