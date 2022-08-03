// $Id: vxManipulatorSubcube.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxManipulator.h"


/**
 * This manipulator is used to grow or shrink the subcube
 * around the center as the mouse wheel or page-up/down is used.
 */
class VX_VIEWER_DLL vxManipulatorSubcube : public vxManipulator
{
// Member Functions
public:

  /// Constructor.
  vxManipulatorSubcube(vxEnvironment & environment);

  /// Destructor.
  virtual ~vxManipulatorSubcube() {}

  /// This event handler can be reimplemented in a subclass to receive keyboard events for the viewer.     
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

private:

  /// Grow the subcube by a number of voxels
  void GrowSubcube( const float4 fVoxels );

}; // class vxManipulatorSubcube


// $Log: vxManipulatorSubcube.h,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:34  geconomos
// moved from vascular
//
// Revision 1.3  2005/06/08 14:59:51  vxconfig
// added easy select object
//
// Revision 1.2  2004/12/10 14:54:46  frank
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
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.3  2003/05/01 16:38:49  geconomos
// Coding standards.
//
// Revision 1.2  2003/04/28 20:13:12  frank
// code review
//
// Revision 1.1  2003/02/14 23:31:18  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorSubcube.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxManipulatorSubcube.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
