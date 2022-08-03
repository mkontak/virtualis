// $Id: vxManipulatorVesselCurvedProjected.h,v 1.2 2006/09/14 20:07:25 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxManipulator.h"


/**
 * Manipulator for Curved Projected MPR
 */
class VX_VIEWER_DLL vxManipulatorVesselCurvedProjected : public vxManipulator
{
public:

  /// Constructor.
  vxManipulatorVesselCurvedProjected(vxEnvironment & environment);

  /// Destructor.
  ~vxManipulatorVesselCurvedProjected() {}

  /// This event handler can be reimplemented in a subclass to receive wheel events for the viewer. 
  virtual bool OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive mouse move events for the viewer.     
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive mouse down events for the viewer.     
  virtual bool OnButtonDown(const Point2D<int2> & position, const vxManipulator::ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive mouse up events for the viewer.     
  virtual bool OnButtonUp(const Point2D<int2> & position, const vxManipulator::ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive keyboard events for the viewer.     
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

private:

  /// get vessel index
  int2 GetVesselIndex( const Point2D<int2> & position );

  /// set navigation parameters
  void SetNavigationPars( const Point2D<int2> & position );

private:

  /// dragging indicator
  bool m_bDragging;
  
}; // class vxManipulatorVesselCurvedProjected


// $Log: vxManipulatorVesselCurvedProjected.h,v $
// Revision 1.2  2006/09/14 20:07:25  geconomos
// moved OrientCameratoVessel to vxVesselUtilities
//
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.3  2005/06/08 15:00:14  vxconfig
// added easy select object
//
// Revision 1.2  2005/02/24 19:36:06  frank
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/12/22 20:00:31  frank
// removed dead code
//
// Revision 1.3  2003/05/22 14:56:32  dongqing
// code review
//
// Revision 1.2  2003/05/21 20:48:11  wenli
// code review
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.5  2003/04/30 16:56:47  frank
// code reviews
//
// Revision 1.4  2003/03/13 14:29:48  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.3  2003/02/14 19:25:55  frank
// Moved zoom function out to curved
//
// Revision 1.2  2003/02/13 17:25:16  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.1.2.1  2003/02/12 23:06:52  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorVesselCurvedProjected.h,v 1.2 2006/09/14 20:07:25 geconomos Exp $
// $Id: vxManipulatorVesselCurvedProjected.h,v 1.2 2006/09/14 20:07:25 geconomos Exp $
