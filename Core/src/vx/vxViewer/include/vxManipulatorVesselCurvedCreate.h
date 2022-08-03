// $Id: vxManipulatorVesselCurvedCreate.h,v 1.3 2007/06/28 19:34:09 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxManipulator.h"

class vxVessel;


class VX_VIEWER_DLL vxManipulatorVesselCurvedCreate : public vxManipulator
{
// Member Functions
public:

  /// Constructor.
  vxManipulatorVesselCurvedCreate(vxEnvironment & environment);

  /// Destructor.
  ~vxManipulatorVesselCurvedCreate() {}

  /// This event handler can be reimplemented in a subclass to receive mouse down events for the viewer.     
  virtual bool OnButtonDown(const Point2D<int2> & position, const vxManipulator::ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive keyboard events for the viewer.     
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

  /// Called when the mouse cursor is to be set
  bool OnSetCursor();

private:

  /// One time initialization
  void OneTimeInitialize();

// Data Members
private:

  vxVessel * m_pVessel;
  
  /// true before initialization
  static bool m_bFirstTime;
  
}; // class vxManipulatorVesselCurvedCreate


// $Log: vxManipulatorVesselCurvedCreate.h,v $
// Revision 1.3  2007/06/28 19:34:09  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.2.2.1  2007/04/16 18:25:55  geconomos
// corrected component handling
//
// Revision 1.2  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.5  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.4.2.1  2005/10/05 15:24:08  frank
// Issue #4452: Added special cursor for new curved MPR tool
//
// Revision 1.4  2005/06/08 15:00:14  vxconfig
// added easy select object
//
// Revision 1.3  2005/03/08 19:09:48  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2  2005/02/24 19:36:06  frank
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
// Revision 1.2  2003/12/23 16:38:07  wenli
// Issue #3571: new curved MPR selection points ....
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorVesselCurvedCreate.h,v 1.3 2007/06/28 19:34:09 jmeade Exp $
// $Id: vxManipulatorVesselCurvedCreate.h,v 1.3 2007/06/28 19:34:09 jmeade Exp $
