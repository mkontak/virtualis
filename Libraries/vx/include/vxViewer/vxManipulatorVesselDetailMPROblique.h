// $Id: vxManipulatorVesselDetailMPROblique.h,v 1.3 2007/11/23 19:37:20 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


/**
 *  manipulator for the vessel oblique
 */


// pragmas
#pragma once


// includes
#include "vxManipulator.h"


class VX_VIEWER_DLL vxManipulatorVesselDetailMPROblique : public vxManipulator
{
// Member Functions
public:

  /// Constructor
  vxManipulatorVesselDetailMPROblique(vxEnvironment & environment);

  /// Destructor
  ~vxManipulatorVesselDetailMPROblique() {}

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

  /// called when the mouse enters the view
  virtual void OnMouseEnter();

private:

  /// Translate the section by a certain distance
  void TranslateSection( float4 fDistance );

  void SaveAVIClip();

// Utility Functions
public:

  /// Retrieve the current section index
  static float4 GetVesselSectionIndex( vxEnvironment & environment );

  /// Set the current section index
  static void SetVesselSectionIndex( vxEnvironment & environment, const float4 fSectionIndex );

  /// Set the old oblique view
  static void SetOldObliqueView( vxEnvironment & environment, const Point3Df & position, const Normal3Df & viewDirection, const Normal3Df & upDirection );

	///Indicates whether the Zooming was applied by the user or not
	static bool IsZoomFactorModified();

	//Reset the zoom factor
	static void ResetZoomFactor() { m_bZoomModified = false; }

// Data Members
private:

	/// Indicates whether the user selected Zooming
	static bool m_bZoomModified;

  /// If we are currently dragging with the mouse button
  bool m_bDragging;

}; // class vxManipulatorVesselDetailMPROblique


// $Log: vxManipulatorVesselDetailMPROblique.h,v $
// Revision 1.3  2007/11/23 19:37:20  romy
// reset the Eye when it switches from Endo to Oblique
//
// Revision 1.2  2007/07/30 19:23:25  romy
// added a flag to indicate that the Zoom level was modified
//
// Revision 1.1  2006/09/25 13:17:34  geconomos
// moved from vascular
//
// Revision 1.3  2005/06/08 15:00:29  vxconfig
// formatting
//
// Revision 1.2  2004/12/10 14:55:04  frank
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
// Revision 1.5  2003/12/11 20:08:19  frank
// Issue #3569: Showed the annotation on the oblique view when you click on them in the pane
//
// Revision 1.4  2003/06/30 15:23:35  wenli
// #  3279  : Oblique view is oriented like the axial with the spine at the bottom
//
// Revision 1.3  2003/06/09 13:37:24  frank
// Added individual slice shadow renderer
//
// Revision 1.2  2003/05/12 20:07:05  frank
// Moved to alternate oblique viewer
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.5  2003/05/01 18:49:45  dongqing
// code review
//
// Revision 1.4  2003/04/30 17:34:12  frank
// code review
//
// Revision 1.3  2003/03/13 14:13:33  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.2  2003/02/12 17:46:26  frank
// Added Home/End/PageUp/PageDown navigation
//
// Revision 1.1  2002/12/18 18:24:21  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorVesselDetailMPROblique.h,v 1.3 2007/11/23 19:37:20 romy Exp $
// $Id: vxManipulatorVesselDetailMPROblique.h,v 1.3 2007/11/23 19:37:20 romy Exp $
