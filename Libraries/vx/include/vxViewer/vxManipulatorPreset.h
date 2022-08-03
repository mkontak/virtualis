// $Id: vxManipulatorPreset.h,v 1.2 2004/04/26 19:04:42 michael Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxManipulatorPreset_h
#define vxManipulatorPreset_h


// includes
#include "vxManipulator.h"


/**
 * Manipulator to handle selection of camera and visualization presets
 */
class VX_VIEWER_DLL vxManipulatorPreset : public vxManipulator
{
// functions
public:

  /// constructor
  vxManipulatorPreset(vxEnvironment & environment);

  /// destructor
  ~vxManipulatorPreset() {}

  /// event handler can be reimplemented in a subclass to receive keyboard events for the viewer
  virtual bool OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState );

private:

  /// copy constructor
  vxManipulatorPreset(const vxManipulatorPreset & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxManipulatorPreset & operator =(const vxManipulatorPreset & other); // should only be public if really implemented!

}; // class vxManipulatorPreset


#endif // vxManipulatorPreset_h


// Revision History:
// $Log: vxManipulatorPreset.h,v $
// Revision 1.2  2004/04/26 19:04:42  michael
// Adjusted code to reflect the changes in zomm factor, roi not being part of
// camera anymore, etc.
//
// Revision 1.1  2004/04/15 15:51:31  frank
// split out the preset manipulation from the window/level manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorPreset.h,v 1.2 2004/04/26 19:04:42 michael Exp $
// $Id: vxManipulatorPreset.h,v 1.2 2004/04/26 19:04:42 michael Exp $