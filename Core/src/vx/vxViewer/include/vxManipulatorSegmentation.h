//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

#ifndef vxManipulatorSegmentation_h
#define vxManipulatorSegmentation_h


// includes
#include "vxManipulator.h"


/**
 *  This is the class definition for segmentation
 */
class VX_VIEWER_DLL vxManipulatorSegmentation : public vxManipulator
{
// functions
public:

  /// constructor
  vxManipulatorSegmentation(vxEnvironment & environment);

  /// destructor
  virtual ~vxManipulatorSegmentation() {};

  /// process button down event
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// Event handler if a key is pressed.
  virtual bool OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive mouse move events for the viewer.     
  virtual bool OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState );

  /// called when the mouse cursor is to be set
  virtual bool OnSetCursor();

private:

  /// copy constructor
  vxManipulatorSegmentation(vxManipulatorSegmentation & other); // should only be public if really implemented!

  /// assignment operator
  const vxManipulatorSegmentation & operator=(vxManipulatorSegmentation & other); // should only be public if really implemented!

}; // vxManipulatorSegmentation


#endif // #ifdnef vxManipulatorSegmentation_h


// $Log: vxManipulatorSegmentation.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2004/01/16 21:22:44  frank
// Prevented the oblique-shift warning from flashing as you move the mouse
//
// Revision 1.9  2004/01/12 14:18:36  frank
// fixed problem with manipulating on oblique with shift held down
//
// Revision 1.8  2003/12/22 12:53:39  frank
// Issue #3593: Restricted segmentation to the visible volume on the shift key
//
// Revision 1.7  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.6  2003/05/16 13:21:35  frank
// formatting
//
// Revision 1.5  2003/05/07 21:00:21  dongqing
// code walkthrough
//
// Revision 1.4  2003/05/07 13:17:31  michael
// initial code review
//
// Revision 1.3  2003/04/15 21:10:43  geconomos
// Issue# 3046: MessageBox causing focus problems.
//
// Revision 1.2  2002/11/25 13:44:56  geconomos
// New mouse cursor handling.
//
// Revision 1.1  2002/11/20 18:34:05  frank
// Renamed from vxManipulatorInterSeg
//
// Revision 1.6  2002/10/17 22:16:46  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.5  2002/08/20 22:36:42  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.4  2002/07/02 20:44:59  geconomos
// Exporting classes .
//
// Revision 1.3  2002/04/23 20:50:46  dmitry
// Cursors implemented.
//
// Revision 1.2  2002/03/29 22:32:54  jaddonisio
// Moved worl coordinate function ouside to base class.
// Also, corrected usage of volume index coordinates.
//
// Revision 1.1  2002/03/29 01:42:54  jaddonisio
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorSegmentation.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxManipulatorSegmentation.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
