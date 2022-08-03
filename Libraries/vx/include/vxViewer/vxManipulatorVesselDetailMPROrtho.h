// $Id: vxManipulatorVesselDetailMPROrtho.h,v 1.2 2007/01/16 17:04:06 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxManipulator.h"


/**
 * Manipulator the the orthogonal MPR view along a vessel
 */
class VX_VIEWER_DLL vxManipulatorVesselDetailMPROrtho : public vxManipulator
{
public:

  /// Constructor.
  vxManipulatorVesselDetailMPROrtho( vxEnvironment & environment );

  /// Destructor.
  ~vxManipulatorVesselDetailMPROrtho() {}

  /// This event handler can be reimplemented in a subclass to receive wheel events for the viewer. 
  virtual bool OnMouseWheel( const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive mouse move events for the viewer.     
  virtual bool OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive mouse down events for the viewer.     
  virtual bool OnButtonDown( const Point2D<int2> & position, const vxManipulator::ButtonEnum eButtonId, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive mouse up events for the viewer.     
  virtual bool OnButtonUp( const Point2D<int2> & position, const vxManipulator::ButtonEnum eButtonId, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive keyboard events for the viewer.
  virtual bool OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState );

private:

  /// Translate the section by a certain distance
  void TranslateSection( const float4 fDistance );

  /// Retrieve the current section index
  float4 GetVesselSectionIndex() const;

  /// Retrieve the maximum possible section index
  float4 GetMaximumVesselSectionIndex() const;

// Utility Functions
public:

  /// Set the current section index
  static void SetVesselSectionIndex( vxEnvironment & environment, const float4 fSectionIndex );

// Data Members
private:

  /// If we are currently dragging with the mouse button
  bool m_bDragging;

  /// swing direction
  bool m_bSwingForward;

}; // class vxManipulatorVesselDetailMPROrtho


// $Log: vxManipulatorVesselDetailMPROrtho.h,v $
// Revision 1.2  2007/01/16 17:04:06  gdavidson
// Removed OnTimer
//
// Revision 1.1  2006/09/25 13:17:34  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 15:00:39  vxconfig
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
// Revision 1.2  2004/01/15 14:23:20  frank
// Issue #3645: Added cine mode for orthogonal MPR vessel views
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.3  2003/05/01 18:49:45  dongqing
// code review
//
// Revision 1.2  2003/04/30 17:31:08  frank
// code review
//
// Revision 1.1  2003/03/24 16:23:59  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorVesselDetailMPROrtho.h,v 1.2 2007/01/16 17:04:06 gdavidson Exp $
// $Id: vxManipulatorVesselDetailMPROrtho.h,v 1.2 2007/01/16 17:04:06 gdavidson Exp $
