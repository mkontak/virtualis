// $Id: vxViewerTypeMPROblique.h,v 1.10 2007/03/05 15:23:27 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner, (mailto:michael@viatronix.com)

#ifndef vxViewerTypeMPROblique_h
#define vxViewerTypeMPROblique_h


// includes
#include "vxViewerType.h"


// forward declarations


// class definition
class VX_VIEWER_DLL vxViewerTypeMPROblique: public vxViewerType
{
// functions
public:

  /// constructor
  vxViewerTypeMPROblique(vxEnvironment & environment);

  /// copy constructor
  virtual ~vxViewerTypeMPROblique() {};

  /// initializes the view
  virtual void InitializeView();

  /// initializes the view/object in view based on an annotation
  virtual bool InitializeFrom(vxAnnotation & annotation);

  /// initializes the OpenGL view setting
  virtual void SetupView();

  /// convert from mouse to screen
  virtual Point2D<int2> ConvertMouseToScreen(const Point2D<int2> & mouseCoord) const;  

  /// convert from screen to world
  virtual bool ConvertScreenToWorld(const Point2D<int2> & screenCoord, Point<float4> & worldCoord, bool bConsiderRenderingMode = true) const;

  /// convert world coordinates to screen coordinates
  virtual bool ConvertWorldToScreen(const Point<float4> & worldCoord, Point2D<int2> & screenCoord) const;

  /// convert world coordinates to screen coordinates
  bool ConvertWorldToScreen(const Point<float4> & worldCoord, Point2D<int2> & screenCoord,
                            const GLint * piViewport, const GLdouble * pfModelviewMatrix,
                            const GLdouble * pfPorjectionMatrix) const;

  /// is this annotation type supported by this viewer type?
  virtual bool IsAnnotationSupported(const vxHandlesObj & handlesObject) const;

  /// should it be drawn on the screen?
  virtual bool IsVisible(const vxHandlesObj & handlesObject) const;

  /// Is this position visible in the current view?
  virtual bool IsVisible(const Point3Df & position) const;

  /// handles modified events
  virtual void OnModified(const type_info & type, const vxModifiedInfo & info);

  /// is the primary renderer is 2D
  virtual BOOL Is2D() const { return BOOL( true ); }

  /// is the primary renderer is 3D
  virtual BOOL Is3D() const { return BOOL( false ); }

 	///Indicates whether the type has a rendering engine plugin or not
	virtual bool HasRenderingNodePlugin() { return true; }

	/// creates the instance of a rendering engine plugin
	virtual vxRenderingNodePlugin * CreateRenderingEnginePlugin();

  /// returns if the viewer supports dropping of the specified type
  virtual bool CanDragDrop(const DragDropEnum eSourceType);

  /// returns the name of the class for type checking (here till dynamic_cast works across DLLs)
  virtual const std::string GetClassString() const { return "vxViewerTypeMPROblique"; }

protected:

  //// FIXME: the implementation for the following six methods as well as
  ////        for SetupView() is identical in vxViewerType3D. Should
  ////        be consolidated, possibly in base class? Check with Vascular
  ////        viewer types if that would break anything there.

  /// get right plane
  virtual float4 GetRightPlane() const;

  /// get left plane
  virtual float4 GetLeftPlane() const;

  /// get top plane
  virtual float4 GetTopPlane() const;

  /// get bottom plane
  virtual float4 GetBottomPlane() const;

  /// get near plane
  virtual float4 GetNearPlane() const;

  /// get far plane
  virtual float4 GetFarPlane() const;

private:

  /// copy constructor
  vxViewerTypeMPROblique(const vxViewerTypeMPROblique & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxViewerTypeMPROblique & operator=(const vxViewerTypeMPROblique & other); // should only be public if really implemented!
};


#endif // vxViewerTypeMPROblique_h


// Revision History:
// $Log: vxViewerTypeMPROblique.h,v $
// Revision 1.10  2007/03/05 15:23:27  geconomos
// code review preparation
//
// Revision 1.9  2007/02/12 14:24:31  geconomos
// reworked MouseToScreen, WorldToScreen and ScreenToWorld methods
//
// Revision 1.8  2007/02/05 19:28:17  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.7  2006/12/04 20:36:20  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.6  2006/08/03 14:07:50  geconomos
// added "rendering engine" support
//
// Revision 1.5  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.4  2004/08/05 13:53:51  michael
// reworked (OpenGL) setup of 3D
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
// Revision 1.22  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.21  2003/08/21 13:10:34  michael
// added functions for OpenGL setup; oblique now does not anymore use the
// camera's functions
//
// Revision 1.20  2003/04/29 14:31:18  frank
// Removed IsManipulatorSupported
//
// Revision 1.19  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.18  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.17  2003/04/18 16:04:09  michael
// initial code review
//
// Revision 1.16  2003/03/30 21:03:23  michael
// formatting and cosmetics
//
// Revision 1.15  2003/03/25 17:31:33  frank
// Added virtual function to return class name until dynamic_cast problem across DLLs is resolved.
//
// Revision 1.14  2003/03/22 16:04:09  michael
// oblique now initializes from point annotations
//
// Revision 1.13  2003/03/20 21:28:16  frank
// Individual IsVisible() for a single point, rather than for a whole annotation
//
// Revision 1.12  2003/03/18 21:23:02  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.11  2003/03/17 19:47:34  geconomos
// Issue #2914. Added DropType property.
//
// Revision 1.10  2003/03/06 15:58:46  frank
// Moved Is2D and Is3D to vxViewerType
//
// Revision 1.9  2003/03/04 15:34:17  geconomos
// Added methods for processing parent environment's modified queue.
//
// Revision 1.8  2003/03/03 16:50:39  geconomos
// Added and implemented InitializeCamera method.
//
// Revision 1.7  2003/02/28 17:54:33  michael
// partially moved SetupView() from vxViewer over to vxViewerType ...
//
// Revision 1.6  2003/01/22 15:07:02  michael
// adjusted code to account for our Y-flip in the GetScreenCoordinates()
//
// Revision 1.5  2003/01/15 16:29:52  michael
// made methods const that do not alter the state
//
// Revision 1.4  2003/01/13 23:19:35  michael
// added get/set for the environment (protected member in base class)
//
// Revision 1.3  2003/01/09 14:56:04  michael
// adjusted methods to take new annotation classes
//
// Revision 1.2  2002/12/18 18:22:02  michael
// moved OpenGL frame buffer reading to base class
//
// Revision 1.1  2002/12/18 18:12:46  michael
// added initial version of vxViewerTypeMPROblique
//
// Revision 1.2  2002/12/18 14:52:15  geconomos
// added virtual specifier to destructor
//
// Revision 1.1  2002/12/17 14:41:40  michael
// initial version of viewer type base class and the MPROrtho implementation
//
// $Id: vxViewerTypeMPROblique.h,v 1.10 2007/03/05 15:23:27 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerTypeMPROblique.h,v 1.10 2007/03/05 15:23:27 geconomos Exp $
