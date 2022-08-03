// $Id: vxManipulatorUtils.h,v 1.1 2001/01/03 19:05:27 michael Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

/**
 * Define vxManipulator utilities 
 */

#ifndef vxManipulatorUtils_h
#define vxManipulatorUtils_h


// includes
#include "vxEnvironment.h"


// forward declarations
class vxEnvironment;


// class definition
class VX_VIEWER_DLL vxManipulatorUtils
{
// functions
public:

  /// default constructor
  vxManipulatorUtils() {};

  /// destructor
  ~vxManipulatorUtils() {};

  /// Warn user if pressing shift key on an oblique view (has no effect)
  static void WarnShiftOblique(const vxEnvironment & environment, const uint4 uKeyboardState);

}; // vxRenderUtils


#endif // vxManipulatorUtils_h


// Revision History:
// $Log: vxManipulatorUtils.h,v $
// Revision 1.1  2001/01/03 19:05:27  michael
// adding class with helper function for some manipulators to display message
// in case user wants to use "SHIFT" in oblique views which is not supported
// by all viewer types.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorUtils.h,v 1.1 2001/01/03 19:05:27 michael Exp $
// $Id: vxManipulatorUtils.h,v 1.1 2001/01/03 19:05:27 michael Exp $
