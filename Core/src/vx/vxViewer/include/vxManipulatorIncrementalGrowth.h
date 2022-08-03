// $Id: vxManipulatorIncrementalGrowth.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxManipulator.h"


// forwards
class vxIncrementalGrowth;


/**
 * Manages the incremental growth operation
 */
class VX_VIEWER_DLL vxManipulatorIncrementalGrowth : public vxManipulator
{
// Member Functions
public:

  /// Constructor
  vxManipulatorIncrementalGrowth(vxEnvironment & environment);

  /// Destructor
  ~vxManipulatorIncrementalGrowth();

  /// This event handler can be reimplemented in a subclass to receive mouse move events for the viewer     
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive mouse down events for the viewer     
  virtual bool OnButtonDown(const Point2D<int2> & position, const vxManipulator::ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive mouse up events for the viewer     
  virtual bool OnButtonUp(const Point2D<int2> & position, const vxManipulator::ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive keyboard down events for the viewer
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive keyboard up events for the viewer
  virtual bool OnKeyReleased(const uint1 uKeyId, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive mouse enter events for the viewer     
  virtual bool OnSetCursor();

  /// Get the number of voxels that are grown per each pixel dragged on screen
  static const uint4 & GetVoxelsPerPixel() { return m_uVoxelsPerPixel; }

private:

  /// Start growing
  bool StartGrowth( const Point2D<int2> & mousePos, const Point3Df & worldPos );

  /// End growth
  void EndGrowth();

  /// Grow the vessel component some
  void GrowSome();

  /// Set the number of voxels that are segmented for each screen pixel
  void SetVoxelsPerStep( const uint4 uKeyboardState );

  /// Quit growing immediately
  void QuitGrowing();

  /// Get the seed point underneath the cursor, if available
  bool ComputeSeedPoint( Point<float4> & worldPosition, const Point2D<int2> & mousePosition );

// Data Members
private:

  /// How quickly to grow as you drag
  static uint4 m_uVoxelsPerPixel;
  
}; // class vxManipulatorIncrementalGrowth


// $Log: vxManipulatorIncrementalGrowth.h,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:34  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 14:59:24  vxconfig
// added easy select object
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2004/01/29 18:03:30  frank
// fixed the selection and display of easy select in 2D views
//
// Revision 1.7  2004/01/29 17:01:11  frank
// Added const access to static data
//
// Revision 1.6  2004/01/08 12:49:10  frank
// Changed the cursor when you can actually select something
//
// Revision 1.5  2003/12/08 20:22:43  frank
// Removed an extra data member
//
// Revision 1.4  2003/11/20 14:22:50  frank
// Made easy select work on all views simultaneously
//
// Revision 1.3  2003/08/04 20:11:29  frank
// Issue #3393: Cleaned up easy select mode after mode switch
//
// Revision 1.2  2003/06/06 14:57:17  frank
// Issue #3277: Disallowed growth if seedpoint is locked
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.7  2003/04/29 19:47:55  frank
// Handled the key up and key down message properly for shift and control
//
// Revision 1.6  2003/04/28 18:59:35  frank
// code review
//
// Revision 1.5  2003/04/18 16:01:12  frank
// Fixed growing, progress bar, and screen issues
//
// Revision 1.4  2003/04/08 19:51:39  geconomos
// updated for new graphics.
//
// Revision 1.3  2003/04/04 21:27:45  frank
// Made the number of voxels in the region interactive
//
// Revision 1.2  2003/04/04 15:55:08  frank
// Cleaned up class and file names and respected the component lock flag.
//
// Revision 1.1  2003/04/04 13:01:23  frank
// Added the ability to incrementally grow a new component.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorIncrementalGrowth.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxManipulatorIncrementalGrowth.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $