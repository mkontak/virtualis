// $Id: vxManipulatorPathCurvedCreate.h,v 1.2 2006/02/27 20:11:58 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)

#ifndef vxManipulatorPathCurvedCreate_h
#define vxManipulatorPathCurvedCreate_h


// Includes
#include "vxManipulator.h"


class VX_VIEWER_DLL vxManipulatorPathCurvedCreate : public vxManipulator
{
// Member Functions
public:

  /// Constructor.
  vxManipulatorPathCurvedCreate(vxEnvironment & environment);

  /// Destructor.
  ~vxManipulatorPathCurvedCreate() {};

  /// This event handler can be reimplemented in a subclass to receive mouse down events for the viewer.     
  virtual bool OnButtonDown(const Point2D<int2> & position, const vxManipulator::ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive keyboard events for the viewer.     
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

  /// called when the mouse cursor is to be set
  virtual bool OnSetCursor();

private:

  /// One time initialization
  void OneTimeInitialize();

// Data Members
private:
  
  /// true before initialization
  bool m_bFirstTime;
  
}; // class vxManipulatorPathCurvedCreate


#endif // vxManipulatorPathCurvedCreate_h


// $Log: vxManipulatorPathCurvedCreate.h,v $
// Revision 1.2  2006/02/27 20:11:58  gdavidson
// Added an OnSetCursor method
//
// Revision 1.1  2004/04/06 18:19:30  frank
// renamed vessel to path
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.1  2003/08/12 15:47:31  frank
// Moved the vessel data structure down to vxBase
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.3  2003/04/30 14:16:50  frank
// code review
//
// Revision 1.2  2003/04/11 18:40:55  frank
// Working on new curved MPR creation
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.1  2002/10/23 23:05:57  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorPathCurvedCreate.h,v 1.2 2006/02/27 20:11:58 gdavidson Exp $
// $Id: vxManipulatorPathCurvedCreate.h,v 1.2 2006/02/27 20:11:58 gdavidson Exp $