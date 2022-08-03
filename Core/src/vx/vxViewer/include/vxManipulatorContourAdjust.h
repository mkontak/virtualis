// $Id: vxManipulatorContourAdjust.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Manjushree Nulkar (mailto:manju@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxManipulator.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"


/**
 * Manipulator to adjust the edge of a component.
 */
class VX_VIEWER_DLL vxManipulatorContourAdjust : public vxManipulator
{
// enumerations
private:

  /// the current stage of drawing
  enum DrawingStage
  {

    /// before the sculpting operation has exited the selected component
    BEFORE_ADJUST,

    /// after the sculpting operation has exited the selected component - actively modifying the contour at this point
    DURING_ADJUST,

    /// after the sculpting operation has completed - no further adjustment now until the 
    AFTER_ADJUST,

  }; // enum DrawingStage

// functions
public:

  /// constructor
  vxManipulatorContourAdjust( vxEnvironment & environment );

  /// destructor
  ~vxManipulatorContourAdjust();

  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer.     
  virtual bool OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState );

  /// process button down event
  virtual bool OnButtonDown( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState );

  /// process button up event
  virtual bool OnButtonUp( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState );

  /// process key pressed
  virtual bool OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState );

  /// called when the mouse cursor is to be set
  virtual bool OnSetCursor();

private:

  /// copy constructor
  vxManipulatorContourAdjust( const vxManipulatorContourAdjust & other );                    // should only be public if really implemented!
  
  /// assignment operator
  const vxManipulatorContourAdjust & operator =( const vxManipulatorContourAdjust & other ); // should only be public if really implemented!

  /// search for the nearest edge point
  Vector3Di vxManipulatorContourAdjust::GetNearestEdgePoint( const Vector3Di & startPointVolume );

  /// extend the edge contour to be nearer to the given point
  void ExtendEdgeContour( const Vector3Di & startPointVolume );

  /// create a contour around the existing component
  void GenerateContour( const Vector3Di & startPointVolume );

  /// perform one pixel's movement worth of work
  void OnePixelMove( const Vector3Di & volumePosition );

  /// get the proper neighborhood for processing (either 4 or 8-connected)
  enum vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum GetNeighborhood( const bool bFourConnected );

  /// determines if the given voxel position is on the outside edge of a component
  bool IsOnOutsideEdgeOfComponent( vxBlockVolumeIterator<uint2> & labelIter, const Vector3Di & queryPositionVolume,
                                   const uint2 & uComponentLabel );

// data
private:

  /// if currently active
  bool m_bActive;

  /// what stage of activity are we in
  enum DrawingStage m_eDrawingStage;

  /// the label index of the component that we started in
  uint2 m_uStartLabelIndex;

  /// the point at which we first exited the component
  Vector3Di m_iStartPositionVolume;

  /// a set of points that track the edge of the contour nearest to the cursor
  std::vector< Point3Di > m_edgeTrackVolume;

  /// the contour of the current component
  std::vector< Point3Di > m_existingContourVolume;

  /// the contour that the user has drawn
  std::vector< Vector3Di > m_drawnContourVolume;

  /// a cache of the most recently used position (in volume coordinates)
  Vector3Di m_lastVolumePosition;

}; // vxManipulatorContourAdjust


// $Log: vxManipulatorContourAdjust.h,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/03/13 13:55:19  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorContourAdjust.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxManipulatorContourAdjust.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
