// $Id: vxViewerTypeMPROrthoPanZoom.h,v 1.8 2006/12/04 20:37:13 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner, (michael@viatronix.com)

#ifndef vxViewerTypeMPROrthoPanZoom_h
#define vxViewerTypeMPROrthoPanZoom_h


// includes
#include "vxViewerTypeMPROrtho.h"


// forward declarations


// class definition
class VX_VIEWER_DLL vxViewerTypeMPROrthoPanZoom: public vxViewerTypeMPROrtho
{
// functions
public:

  /// constructor
  vxViewerTypeMPROrthoPanZoom(vxEnvironment & environment);

  /// copy constructor
  virtual ~vxViewerTypeMPROrthoPanZoom() {};

  /// initializes the view
  virtual void InitializeView() { return; };

  /// initializes the OpenGL view setting
  virtual void SetupView();

  /// is this annotation type supported by this viewer type?
  virtual bool IsAnnotationSupported(const vxHandlesObj & handlesObject) const { return false; };

  /// should it be drawn on the screen?
  virtual bool IsVisible(const vxHandlesObj & handlesObject) const { return false; };

  /// is this position visible in the current view?
  virtual bool IsVisible(const Point3Df & position) const { return false; }

  /// is the primary renderer 2D?
  virtual BOOL Is2D() const { return BOOL( true ); }

  /// is the primary renderer 3D?
  virtual BOOL Is3D() const { return BOOL( false ); }

  /// returns the name of the class for type checking (here till dynamic_cast works across DLLs)
  virtual const std::string GetClassString() const { return "vxViewerTypeMPROrthoPanZoom"; }

private:

  /// copy constructor
  vxViewerTypeMPROrthoPanZoom(const vxViewerTypeMPROrthoPanZoom & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxViewerTypeMPROrthoPanZoom & operator=(const vxViewerTypeMPROrthoPanZoom & other); // should only be public if really implemented!
};


#endif // vxViewerTypeMPROrthoPanZoom_h


// Revision History:
// $Log: vxViewerTypeMPROrthoPanZoom.h,v $
// Revision 1.8  2006/12/04 20:37:13  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.7  2006/10/05 21:09:35  geconomos
// Issue #5027: image disappers while panning
//
// Revision 1.6  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.5  2004/08/05 13:28:01  michael
// took out code that was commented out anyway
//
// Revision 1.4  2004/04/26 19:01:27  michael
// moved GetLeft() / GetRight() / etc. from vxCamera to vxViewerType
//
// Revision 1.3  2004/04/23 16:50:07  frank
// workaround the virtual bool bug
//
// Revision 1.2  2004/03/02 19:19:17  frank
// renamed method in viewer type to more accurately describe it
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.8  2003/04/29 14:31:18  frank
// Removed IsManipulatorSupported
//
// Revision 1.7  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.6  2003/04/18 16:04:09  michael
// initial code review
//
// Revision 1.5  2003/03/26 22:54:53  michael
// moved empty method implementation to header file
//
// Revision 1.4  2003/03/25 17:31:33  frank
// Added virtual function to return class name until dynamic_cast problem across DLLs is resolved.
//
// Revision 1.3  2003/03/20 21:28:16  frank
// Individual IsVisible() for a single point, rather than for a whole annotation
//
// Revision 1.2  2003/03/19 16:16:31  michael
// fixed compile issues and added it to DEFINES in managed code
//
// Revision 1.1  2003/03/19 16:05:01  michael
// Added initial version
//
// $Id: vxViewerTypeMPROrthoPanZoom.h,v 1.8 2006/12/04 20:37:13 romy Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerTypeMPROrthoPanZoom.h,v 1.8 2006/12/04 20:37:13 romy Exp $
