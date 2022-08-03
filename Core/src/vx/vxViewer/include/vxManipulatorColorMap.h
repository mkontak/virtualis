// $Id: vxManipulatorColorMap.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (ingmar@viatronix.com)

/**
 *  This class is for setting colormap
 */


#ifndef vxManipulatorColormap_h
#define vxManipulatorColormap_h



// include declaration
#include "vxManipulator.h"
#include "vxControlPoint.h"



// foreward declarations
template <class T> class Point2D;



// class definition
class VX_VIEWER_DLL vxManipulatorColormap: public vxManipulator
{
// member functions
public:

  // constructor
  vxManipulatorColormap(vxEnvironment & environment);

  // destructor
  virtual ~vxManipulatorColormap() {};

  /// This event handler receives mouse down events for the viewer.     
  virtual bool OnButtonDown(const Point2D<int2> & pos, const ButtonEnum eButton, const uint4 uState);

  /// This event handler receives mouse up events for the viewer.     
  virtual bool OnButtonUp(const Point2D<int2> & pos, const ButtonEnum eButton, const uint4 uState);

  /// This event handler receives mouse move events for the viewer.     
  virtual bool OnMouseMove(const Point2D<int2> & pos, const uint4 uState);

  /// This event handler can be reimplemented in a subclass to receive keyboard events for the viewer.     
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive mouse down events for the viewer.     
  virtual bool OnDoubleClick(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);
  
private:

  /// Finds the closest point, closer than the closest distance, if any (-1 if none).
  int4 GetClosestPoint( float4 & fClosestDistance, const Point2D<float4> & pos );

  /// Converts a point in world coordinates to local coordinates.
  Point2D<float4> ToLocal(const Point2D<int2> & pt);

  /// Returns true if coordinates were next to a point in local coordinates.
  bool IsClose(const Point2D<float4> & click, const Point2D<float4> & point) const;

  /// Performs check on all or selected points and returns index of the nearest point or -1.
  int4 NextTo(const Point2D<float4> & click, bool bAll = true) const;

  /// Handles the arrow key press command.
  void MoveAll(const Point2D<float4> & trans);

  /// Move a single control point to a specified location.
  void MoveSinglePoint(const int4 & iIndex, Point2D<float4> pt);

  /// Translate multiple points by a vector in 2D.
  void MoveMultiplePoints(const Point2D<float4> & pt);

  /// Move to maximum or minimum x value allowed.
  void MoveCurve(float4 fX, float4 fY);

  /// Move Window / Level controls.
  void MoveWindowLevel(float4 fX);

  /// Adds control points to the current configuration.
  void AddPoints(std::vector<vxControlPoint> & vShape);

  ///
  Point2D<float4> FindLocationInImage(const PixelRGBA<uint1> & color);

// member variables
private:              
  
  /// if we are dragging
  bool m_bDragging;

}; // vxManipulatorColorMap


#endif // vxManipulatorColorMap_h


// Revision History:
// $Log: vxManipulatorColorMap.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2003/06/19 20:21:21  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.12  2003/05/07 15:01:30  dongqing
// code walkthrough
//
// Revision 1.11  2003/05/07 11:44:02  michael
// initial code review
//
// Revision 1.10  2002/10/17 22:16:46  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.9  2002/10/04 14:37:07  michael
// fixed colormap
//
// Revision 1.8  2002/09/26 19:23:26  michael
// Fixed color selection to always update the lcoation in the color chooser
// corresponding to the currently selected point in the color map
//
// Revision 1.7  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.6  2002/09/17 13:01:02  michael
// adjustments for changes in transfer function
//
// Revision 1.5  2002/09/11 20:43:36  frank
// Working on transfer function event mechanism
//
// Revision 1.4  2002/09/10 22:59:59  frank
// Cleaned out dead code.
//
// Revision 1.3  2002/09/10 17:18:46  frank
// Added colormap point rendering and manipulation.
//
// Revision 1.2  2002/09/09 21:31:58  frank
// Working on color map editor
//
// Revision 1.1  2002/09/06 23:05:50  ingmar
// new TFE without sampling effect renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorColorMap.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxManipulatorColorMap.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $