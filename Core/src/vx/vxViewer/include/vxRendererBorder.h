// $Id: vxRendererBorder.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: jenny hizver (jenny@viatronix.com)

/**
 * This class defines the render border
 */


#ifndef vxRendererBorder_h
#define vxRendererBorder_h


// includes
#include "vxRenderer.h"


// class definition
class VX_VIEWER_DLL vxRendererBorder : public vxRenderer
{
// functions
public:

  /// constructor
  vxRendererBorder(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererBorder() {}
  
public:
  
  /// renders a border
  virtual void Render();
  
private:

  /// copy constructor
  vxRendererBorder(const vxRendererBorder & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererBorder & operator=(const vxRendererBorder & other); // should only be public if really implemented!
}; // vxRendererBorder


#endif // vxRendererBorder_h


// Revision History:
// $Log: vxRendererBorder.h,v $
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
// Revision 1.12  2003/05/19 14:38:02  frank
// Removed unused return parameter
//
// Revision 1.11  2003/05/17 15:44:49  michael
// reformatting
//
// Revision 1.10  2003/05/16 13:22:14  frank
// formatting
//
// Revision 1.9  2003/05/14 18:30:04  dongqing
// code review
//
// Revision 1.8  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.7  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.6  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.5  2002/04/26 13:39:56  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.4  2002/04/08 13:35:38  michael
// added missing privately non implemented copy constructor and assignment operator
//
// Revision 1.3  2002/03/21 20:13:36  jenny
// put comments
//
// Revision 1.2  2002/03/21 19:16:41  michael
// added log history lines
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererBorder.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
// $Id: vxRendererBorder.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $1.2
