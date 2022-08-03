// $Id: vxRendererMPROrthoPanZoom.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Uday Chebrolu (mailto:uday@viatronix.com)

#ifndef vxRendererMPROrthoPanZoom_h
#define vxRendererMPROrthoPanZoom_h


// includes
#include "vxRenderer.h"


// forward declarations
class vxEnvironment;


// class definition
class VX_VIEWER_DLL vxRendererMPROrthoPanZoom : public vxRenderer
{
// functions
public:

  /// constructor
  vxRendererMPROrthoPanZoom(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererMPROrthoPanZoom() {}; 

  /// perform rendering operations
  virtual void Render();

private:

  /// copy constructor
  vxRendererMPROrthoPanZoom(const vxRendererMPROrthoPanZoom & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererMPROrthoPanZoom & operator=(const vxRendererMPROrthoPanZoom & other); // should only be public if really implemented!
}; // vxRendererMPROrthoPanZoom


#endif // vxRendererMPROrthoPanZoom_h


// Revision History:
// $Log: vxRendererMPROrthoPanZoom.h,v $
// Revision 1.2  2007/02/15 00:08:24  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.1.18.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.1  2004/04/26 21:25:07  vxconfig
// added renamed classes
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/07/11 19:49:15  michael
// final code reviews
//
// Revision 1.9  2003/05/19 14:40:40  frank
// Removed unused return parameter
//
// Revision 1.8  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.7  2003/05/16 13:22:14  frank
// formatting
//
// Revision 1.6  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.5  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.4  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.3  2002/05/17 14:58:42  dmitry
// Removed Prerender(), PostRender().
//
// Revision 1.2  2002/04/08 13:34:47  michael
// cosmetics and added author
//
// Revision 1.1  2002/03/26 21:15:28  uday
// Initial Version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererMPROrthoPanZoom.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
// $Id: vxRendererMPROrthoPanZoom.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
