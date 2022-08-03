// $Id: vxManipulatorVesselNavigation.h,v 1.2 2006/09/25 13:22:06 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * define manipulator for the vessel mavigation
 */


// pragmas
#pragma once


// includes
#include "vxManipulator.h"


class VX_VIEWER_DLL vxManipulatorVesselNavigation : public vxManipulator
{
// Member Functions
public:

  /// Constructor.
  vxManipulatorVesselNavigation(vxEnvironment & environment);

  /// Destructor.
  ~vxManipulatorVesselNavigation() {}

  /// This event handler can be reimplemented in a subclass to receive wheel events for the viewer. 
  virtual bool OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState);

  /// This event handler can be reimplemented in a subclass to receive keyboard events for the viewer.     
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

// Utility Functions
public:

  /// Set the current section index
  static void SetVesselSectionIndex( vxEnvironment & environment, const float4 fSectionIndex, const bool bLookForward );

private:

  /// Translate the section by a certain distance
  void TranslateSection( const float4 fDistance );

}; // class vxManipulatorVesselNavigation


// $Log: vxManipulatorVesselNavigation.h,v $
// Revision 1.2  2006/09/25 13:22:06  geconomos
// + commented out unneeded  log debugging
// + removed "DetailViewMode" extended element
//
// Revision 1.3  2005/06/08 15:00:29  vxconfig
// formatting
//
// Revision 1.2  2004/12/22 14:11:15  frank
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
// Revision 1.2  2003/05/20 18:18:20  dongqing
// code review
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.6  2003/04/30 15:51:25  frank
// code review
//
// Revision 1.5  2003/03/14 18:02:54  frank
// Fixed view direction along vessel
//
// Revision 1.4  2003/03/14 16:06:30  frank
// Organized code better
//
// Revision 1.3  2003/02/14 22:34:52  frank
// Made it work
//
// Revision 1.2  2003/02/14 19:25:02  frank
// Working on endoluminal flight
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorVesselNavigation.h,v 1.2 2006/09/25 13:22:06 geconomos Exp $
// $Id: vxManipulatorVesselNavigation.h,v 1.2 2006/09/25 13:22:06 geconomos Exp $
