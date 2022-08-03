// $Id: vxManipulatorCurrentVoxel.h,v 1.3.2.1 2009/07/28 15:38:23 kchalupa Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: jenny hizver (jenny@viatronix.org)


#pragma once

// includes
#include "vxManipulator.h"


/**
 * Class for setting the current voxel based on the current mouse position
 */
class VX_VIEWER_DLL vxManipulatorCurrentVoxel : public vxManipulator
{
// construction
public:

  //constructor
  vxManipulatorCurrentVoxel(vxEnvironment & environment);

  //destructor
  ~vxManipulatorCurrentVoxel() {};

private:

  /// copy constructor
  vxManipulatorCurrentVoxel(const vxManipulatorCurrentVoxel &);                   // should only be public if really implemented!

// member functions
public:

  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive wheel events for the viewer
  virtual bool OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse leave events
  virtual void OnMouseLeave();

private:

  /// set the current voxel in the environment
  void SetCurrentVoxel(const Point2D<int2> & position);
  
// operators
private:

  /// assignment operator
  const vxManipulatorCurrentVoxel & operator=(const vxManipulatorCurrentVoxel &); // should only be public if really implemented!
 
}; // class vxManipulatorCurrentVoxel


// Revision History:
// $Log: vxManipulatorCurrentVoxel.h,v $
// Revision 1.3.2.1  2009/07/28 15:38:23  kchalupa
// Code Walkthrough
//
// Revision 1.3  2008/04/24 21:55:35  jmeade
// Merge from CON_3-0 on 080424
//
// Revision 1.2  2007/10/31 14:35:58  gdavidson
// Issue #5835: Set current voxel to (-1,-1,-1) when mouse leaves the viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.9  2003/05/16 13:21:34  frank
// formatting
//
// Revision 1.8  2003/05/07 15:01:30  dongqing
// code walkthrough
//
// Revision 1.7  2003/05/07 11:44:02  michael
// initial code review
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorCurrentVoxel.h,v 1.3.2.1 2009/07/28 15:38:23 kchalupa Exp $
// $Id: vxManipulatorCurrentVoxel.h,v 1.3.2.1 2009/07/28 15:38:23 kchalupa Exp $
