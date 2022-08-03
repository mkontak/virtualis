// $Id: vxViewerTypeUndefined.h,v 1.4 2006/01/20 18:09:00 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner, (mailto:michael@viatronix.com)

#ifndef vxViewerTypeUndefined_h
#define vxViewerTypeUndefined_h


// includes
#include "vxViewerType.h"


// forward declarations


// class definition
class VX_VIEWER_DLL vxViewerTypeUndefined: public vxViewerType
{
// functions
public:

  /// constructor
  vxViewerTypeUndefined(vxEnvironment & environment);

  /// copy Constructor
  virtual ~vxViewerTypeUndefined() {};

  /// initializes the view
  virtual void InitializeView() { return; };

  /// initializes the OpenGL view setting
  virtual void SetupView() { return; };

  /// get the world position of the given screen position
  virtual bool ConvertScreenToWorld(const Point2D<int2> & screenCoord, Point<float4> & worldCoord, bool bConsiderRenderingMode = true) const { return false; }

  /// get the world position of the given screen position
  virtual bool ConvertWorldToScreen(const Point<float4> & worldCoord, Point2D<int2> & screenCoord) const  { return false; }
  
  /// is this annotation type supported by this viewer type?
  virtual bool IsAnnotationSupported(const vxHandlesObj & handlesObject) const { return false; }

  /// should it be drawn on the screen?
  virtual bool IsVisible(const vxHandlesObj & handlesObject) const { return false; }

  /// is this position visible in the current view?
  virtual bool IsVisible(const Point3Df & position) const { return false; }

  /// is the primary renderer is 2D
  virtual BOOL Is2D() const { return BOOL( false ); }

  /// is the primary renderer is 3D
  virtual BOOL Is3D() const { return BOOL( false ); }

  /// returns the name of the class for type checking (here till dynamic_cast works across DLLs)
  virtual const std::string GetClassString() const { return "vxViewerTypeUndefined"; }

private:

  /// copy constructor
  vxViewerTypeUndefined(const vxViewerTypeUndefined & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxViewerTypeUndefined & operator=(const vxViewerTypeUndefined & other); // should only be public if really implemented!

}; // class vxViewerTypeUndefined


#endif // vxViewerTypeUndefined_h


// Revision History:
// $Log: vxViewerTypeUndefined.h,v $
// Revision 1.4  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
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
// Revision 1.20  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.19  2003/04/29 14:31:18  frank
// Removed IsManipulatorSupported
//
// Revision 1.18  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.17  2003/04/24 13:49:29  frank
// code review
//
// Revision 1.16  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.15  2003/04/16 14:40:36  michael
// performed initial code review
//
// Revision 1.14  2003/04/16 14:34:51  michael
// performed initial code review
//
// Revision 1.13  2003/04/16 13:42:25  michael
// performed initial code review
//
// Revision 1.12  2003/03/30 21:03:23  michael
// formatting and cosmetics
//
// Revision 1.11  2003/03/25 17:31:33  frank
// Added virtual function to return class name until dynamic_cast problem across DLLs is resolved.
//
// Revision 1.10  2003/03/20 21:28:16  frank
// Individual IsVisible() for a single point, rather than for a whole annotation
//
// Revision 1.9  2003/03/18 21:23:02  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.8  2003/03/06 15:58:21  frank
// Moved Is2D and Is3D to vxViewerType
//
// Revision 1.7  2003/03/03 16:50:39  geconomos
// Added and implemented InitializeCamera method.
//
// Revision 1.6  2003/02/28 17:54:33  michael
// partially moved SetupView() from vxViewer over to vxViewerType ...
//
// Revision 1.5  2003/01/22 15:04:47  michael
// adjusted code to account for our Y-flip in the GetScreenCoordinates()
//
// Revision 1.4  2003/01/15 16:29:51  michael
// made methods const that do not alter the state
//
// Revision 1.3  2003/01/13 23:19:35  michael
// added get/set for the environment (protected member in base class)
//
// Revision 1.2  2003/01/09 14:56:03  michael
// adjusted methods to take new annotation classes
//
// Revision 1.1  2002/12/18 15:01:33  michael
// added initial version of undefined viewer type (default one)
//
// $Id: vxViewerTypeUndefined.h,v 1.4 2006/01/20 18:09:00 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerTypeUndefined.h,v 1.4 2006/01/20 18:09:00 geconomos Exp $
