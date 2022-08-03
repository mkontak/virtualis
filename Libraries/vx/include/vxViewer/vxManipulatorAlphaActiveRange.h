// $Id: vxManipulatorAlphaActiveRange.h,v 1.2 2005/05/24 15:32:18 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (ingmar@viatronix.com)

#ifndef vxManipulatorAlphaActiveRange_h
#define vxManipulatorAlphaActiveRange_h


// include declaration
#include "vxManipulator.h"
#include "vxControlPoint.h"


// forward declarations
template <class T> class Point2D;


// class definition
class VX_VIEWER_DLL vxManipulatorAlphaActiveRange : public vxManipulator
{
// functions
public:

  /// constructor
  vxManipulatorAlphaActiveRange(vxEnvironment & environment);

  /// destructor
  virtual ~vxManipulatorAlphaActiveRange() {};

  /// event handler receives mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButton, const uint4 uState);

  /// event handler receives mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButton, const uint4 uState);

  /// event handler receives mouse move events for the viewer 
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uState);

  /// event handler can be reimplemented in a subclass to receive keyboard events for the viewer
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse down events for the viewer
  virtual bool OnDoubleClick(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);
  
private:

  /// converts a point in world coordinates to local coordinates
  Point2D<float4> ToLocal(const Point2D<int2> & position);

  /// finds the closest point, closer than the closest distance, if any (-1 if none)
  int4 GetClosestPoint(const Point2D<float4> & position);

  /// returns check if coordinates were next to a point in local coordinates
  bool NextTo(const Point2D<float4> & click, const Point2D<float4>& point) const;

  /// move window / level controls
  void MoveWindowLevel(float4 fX);

// data
private:              

  /// if we are dragging
  bool m_bDragging;

  /// the number of pixels to use as the threshold for selection
  static const int m_selectionThresholdDistance = 7;

}; // class vxManipulatorAlphaActiveRange


#endif // vxManipulatorAlphaActiveRange_h


// $Log: vxManipulatorAlphaActiveRange.h,v $
// Revision 1.2  2005/05/24 15:32:18  frank
// made it easier to select and adjust points in the alpha transfer function
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2004/01/23 16:26:11  frank
// computed and displayed Find Region with sampling for oblique & 3D views
//
// Revision 1.7  2003/08/20 14:22:34  michael
// added "mailto:"
//
// Revision 1.6  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.5  2002/10/17 22:16:46  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.4  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.3  2002/09/17 13:01:02  michael
// adjustments for changes in transfer function
//
// Revision 1.2  2002/09/10 22:59:13  frank
// Made alpha curve manipulation possible.
//
// Revision 1.1  2002/09/06 23:05:50  ingmar
// new TFE without sampling effect renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorAlphaActiveRange.h,v 1.2 2005/05/24 15:32:18 frank Exp $
// $Id: vxManipulatorAlphaActiveRange.h,v 1.2 2005/05/24 15:32:18 frank Exp $
