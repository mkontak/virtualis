// $Id: vxManipulatorVesselLuminal.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


/**
 *  Manipulator for the luminal MPR view
 */

// pragmas
#pragma once


// includes
#include "vxManipulatorVessel.h"


// forwards
class vxVessel;


/**
 * Manipulates a vessel either luminal or curved MPR.
 * Lets you do things like measure length or synchronize the cross-section.
 */
class VX_VIEWER_DLL vxManipulatorVesselLuminal : public vxManipulatorVessel
{
// Member Functions
public:

  /// Constructor.
  vxManipulatorVesselLuminal( vxEnvironment & environment );

  /// Destructor.
  ~vxManipulatorVesselLuminal();

  /// This event handler can be reimplemented in a subclass to receive mouse move events for the viewer.     
  virtual bool OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive mouse down events for the viewer.     
  virtual bool OnButtonDown( const Point2D<int2> & position, const vxManipulator::ButtonEnum eButtonId, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive mouse up events for the viewer.     
  virtual bool OnButtonUp( const Point2D<int2> & position, const vxManipulator::ButtonEnum eButtonId, const uint4 uKeyboardState );
  
  /// Event handler if key is pressed
  virtual bool OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive mouse enter events for the viewer.     
  virtual bool OnSetCursor();

  /// called when the mouse enters the view
  virtual void OnMouseEnter();

public:

  /// Synchronize all the other views to the position clicked on the screen
  static void SetNavigationPars( vxEnvironment & environment, const Point2D<int2> & position );

private:

  /// Set the endoluminal camera to the proper position and orientation
  static void SetEndoluminalCamera( vxEnvironment & environment, const float4 fVesselSection,
                                    const bool bLookForward, const Vector3Df & positionTranslation );

  /// Set the view parameters for the 3D detail view
  static void SetDetail3DView( vxEnvironment & environment, const Box< float4 > cutBox );

  /// Check if any selection lines are nearby and if so, which is closer
  vxManipulatorVessel::SelectedSection::SelectionEnum GetNearbySelection( const Point2D<int2> & position ) const;

// Data Members
private:

  /// if we are currently dragging the mouse
  bool m_bDragging;

  /// if we are dragging the mouse forward or not
  static bool m_bDraggingForward;

  /// the selection state
  SelectedSection m_selection;

}; // class vxManipulatorVesselLuminal


// $Log: vxManipulatorVesselLuminal.h,v $
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.3  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.2.1  2005/08/31 20:04:53  frank
// Issue #4407: Initialized the 3D camera arrow when new vessel is selected.
//
// Revision 1.2  2005/06/07 18:10:42  vxconfig
// removed the vessel array
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/03/12 16:10:10  frank
// Added auto-zoom on A key press
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/12/22 20:02:09  frank
// updated slice shadow data exchange
//
// Revision 1.6  2003/11/04 18:31:49  frank
// Issue #3558: fixed endoluminal flight direction on mouse up
//
// Revision 1.5  2003/06/09 13:37:10  frank
// Added individual slice shadow renderers
//
// Revision 1.4  2003/05/20 17:42:29  frank
// code review
//
// Revision 1.3  2003/05/20 17:40:32  dongqing
// code review
//
// Revision 1.2  2003/05/19 14:24:25  frank
// working on oblique view
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.14  2003/05/02 18:11:44  frank
// Issue #3167: Added up/down arrow when dragging or near selection lines
//
// Revision 1.13  2003/05/01 15:10:38  frank
// Added function to relocate the axial plane
//
// Revision 1.12  2003/04/30 15:49:47  frank
// code review
//
// Revision 1.11  2003/04/16 14:51:03  frank
// Separated out function for the new oblique plane
//
// Revision 1.10  2003/03/24 14:29:33  frank
// Moved some common code to a base class
//
// Revision 1.9  2003/03/14 17:28:43  frank
// Refactored code
//
// Revision 1.8  2003/03/14 16:06:16  frank
// Organized code better
//
// Revision 1.7  2003/03/07 20:41:12  geconomos
// Implementation of destructor move to .C file.
//
// Revision 1.6  2003/02/18 18:00:03  frank
// Refactored code
//
// Revision 1.5  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.4  2003/02/12 16:35:54  frank
// Enabled selection of the cross-section directly.
//
// Revision 1.3.2.1  2003/02/12 23:10:18  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.3  2003/01/28 15:08:44  frank
// Added vxRendererVesselCurved
//
// Revision 1.2  2002/11/12 16:14:10  frank
// Added synchronized oblique view with vessel section.
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.3  2002/10/21 22:33:22  frank
// Working on linking up the oblique view
//
// Revision 1.2  2002/09/25 22:10:24  wenli
// Add BoxOfInterest
//
// Revision 1.1  2002/08/22 02:58:44  frank
// Intial version.
//
// Revision 1.1  2002/08/12 15:52:58  wenli
// initial version - Vascular
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorVesselLuminal.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
// $Id: vxManipulatorVesselLuminal.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
