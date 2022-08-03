// $Id: vxManipulatorCropBox.h,v 1.3 2004/03/25 18:36:53 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxManipulatorCropBox_h
#define vxManipulatorCropBox_h


// includes
#include "vxManipulator.h"


/**
 * This class modifies a crop box
 */
class VX_VIEWER_DLL vxManipulatorCropBox : public vxManipulator
{
// functions
public:

  /// constructor
  vxManipulatorCropBox( vxEnvironment & environment );

  /// destructor
  virtual ~vxManipulatorCropBox() {}
  
  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & mousePosition, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & mousePosition, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & mousePosition, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse enter events for the viewer
  virtual bool OnSetCursor();

  /// event handler can be reimplemented in a subclass to receive mouse leave events
  virtual void OnMouseLeave();

private:

  /// Sets the cursor for a specific handle
  void UpdateCursor( int4 iHandleIndex ) const;

  /// get the index of the handle under the cursor, if any exists
  int4 GetTouchingHandle( const Point2D<int2> & mousePosition ) const;

  /// copy constructor
  vxManipulatorCropBox( vxManipulatorCropBox & other );
  
  /// assignment operator
  vxManipulatorCropBox & operator =( vxManipulatorCropBox & );

}; // class vxManipulatorCropBox


#endif // vxManipulatorCropBox_h


// $Log: vxManipulatorCropBox.h,v $
// Revision 1.3  2004/03/25 18:36:53  frank
// added special cursors to the crop box handles
//
// Revision 1.2  2004/03/11 17:39:53  frank
// reworked crop box to eliminate handles set and annotation ROI
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.14  2003/06/03 20:30:54  michael
// added check to see if volume is valid (each dimension > 0)
//
// Revision 1.13  2003/05/16 20:46:26  dongqing
// code review
//
// Revision 1.12  2003/05/16 18:38:39  michael
// code review
//
// Revision 1.11  2003/05/16 13:21:33  frank
// formatting
//
// Revision 1.10  2003/01/06 17:13:55  michael
// formatting
//
// Revision 1.9  2002/11/25 13:44:56  geconomos
// New mouse cursor handling.
//
// Revision 1.8  2002/10/17 22:16:46  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.7  2002/09/25 22:34:09  ingmar
// added missing return types
//
// Revision 1.6  2002/09/17 21:42:37  dmitry
// Cursors added.
//
// Revision 1.5  2002/09/16 19:59:03  dmitry
// New vxID (s) used.
//
// Revision 1.4  2002/08/20 22:31:34  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.3  2002/07/30 20:22:18  dmitry
// bunch of stuff changed to vx
//
// Revision 1.2  2002/07/02 20:44:59  geconomos
// Exporting classes .
//
// Revision 1.1  2002/06/11 14:55:38  manju
// Manipulator for crop box.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorCropBox.h,v 1.3 2004/03/25 18:36:53 frank Exp $
// $Id: vxManipulatorCropBox.h,v 1.3 2004/03/25 18:36:53 frank Exp $
