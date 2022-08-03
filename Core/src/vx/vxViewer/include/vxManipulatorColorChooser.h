// $Id: vxManipulatorColorChooser.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (ingmar@viatronix.com)

/**
 *   This class should define the color chooser.
 */


#ifndef vxManipulatorColorChooser_h
#define vxManipulatorColorChooser_h


// includes
#include "vxManipulator.h"
#include "vxControlPoint.h"


// forward declarations
template <class T> class Point2D;


// class definiction
class VX_VIEWER_DLL vxManipulatorColorChooser: public vxManipulator
{
// functions
public:

  // constructor
  vxManipulatorColorChooser(vxEnvironment & environment);

  // destructor
  virtual ~vxManipulatorColorChooser() {};

  /// event handler receives mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButton, const uint4 uKeyboardState);

  /// event handler receives mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButton, const uint4 uKeyboardState);

  /// event handler receives mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse leave events for the viewer
  virtual void OnMouseLeave();

// data
private:

  /// set the color from the colormap image to the transfer function
  bool SetColor( const Point2D<int2> & pos );

  /// copy of the colormap image
  Image<PixelRGBA<uint1> > * m_pColormapImage;

  /// are dragging
  bool m_bDragging;
}; // vxManipulatorColorChooser


#endif // vxManipulatorColorChooser_h


// Revision History:
// $Log: vxManipulatorColorChooser.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/06/19 20:21:21  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.7  2003/05/07 15:01:30  dongqing
// code walkthrough
//
// Revision 1.6  2003/05/07 11:44:01  michael
// initial code review
//
// Revision 1.5  2002/10/17 22:16:46  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.4  2002/10/14 18:43:20  michael
// fixed type bug that ocurred during the const-transition of vxCamera and vxColormap
//
// Revision 1.3  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.2  2002/09/10 17:18:46  frank
// Added colormap point rendering and manipulation.
//
// Revision 1.1  2002/09/06 23:05:50  ingmar
// new TFE without sampling effect renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorColorChooser.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxManipulatorColorChooser.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $