// $Id: vxManipulatorCenterSelection.h,v 1.1.2.2 2009/07/28 15:38:23 kchalupa Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#pragma once


// includes
#include "vxManipulator.h"


/**
 * Manipulator for the center selection
 */
class VX_VIEWER_DLL vxManipulatorCenterSelection : public vxManipulator
{
// construction
public:

  /// constructor
  vxManipulatorCenterSelection(vxEnvironment & environment);

  /// default destructor
  virtual ~vxManipulatorCenterSelection() {}

private:

  /// Copy constructor
  vxManipulatorCenterSelection( vxManipulatorCenterSelection & other ); // should only be public if really implemented!

// member functions
public:

  /// event handler can be reimplemented in a subclass to receive mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// called when the mouse cursor is to be set
  virtual bool OnSetCursor();

// operators
private:

  /// Assignment operator
  const vxManipulatorCenterSelection & operator=( vxManipulatorCenterSelection & other); // should only be public if really implemented!

// member variables
private:

  /// tracks the activity of the manipulator
  bool m_bActive;

}; // class vxManipulatorCenterSelection


// $Log: vxManipulatorCenterSelection.h,v $
// Revision 1.1.2.2  2009/07/28 15:38:23  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.1  2008/11/18 19:33:19  gdavidson
// Issue #6203: Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorCenterSelection.h,v 1.1.2.2 2009/07/28 15:38:23 kchalupa Exp $
// $Id: vxManipulatorCenterSelection.h,v 1.1.2.2 2009/07/28 15:38:23 kchalupa Exp $