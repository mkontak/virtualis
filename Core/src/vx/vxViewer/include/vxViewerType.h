// $Id: vxViewerType.h,v 1.17 2007/07/31 16:00:57 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Michael Meissner, (mailto:michael@viatronix.com)

/*
 This class is an abstract base class for any view type. To support
 the rendering as well as annotations, there is a minimal interface
 required to be supported. Thus, any view type developed in the future
 can still make use of annotations by specifying whether they are
 supported and whether they are visible (to be drawn) in the current
 view.
*/

#pragma once

// includes
#include "GLFontGeorge.h"
#include "BoundingRect.H"
#include "vxRenderingNodePlugin.h"


// forward declarations
class vxEnvironment;
class vxHandlesObj;
class vxModifiedInfoList;
class vxModifiedInfo;
class vxManipulator;
class vxAnnotation;

template <class T> class vxBlockVolumeRayIterator;

// class definition
class VX_VIEWER_DLL vxViewerType
{
// enums
public:

  /// coordinate system to use when rendering visual elements on the screen such as annotations
  enum RenderCoordinateSystemEnum 
  { 
    eWorldSpace, 
    eScreenSpace 
  }; // RenderCoordinateSystemEnum

  /// available drag and drop items
  enum DragDropEnum 
  { 
    DATASET, 
    CAMERA, 
    COLORMAP,
    PRESET
  }; // DragDropEnum

// functions
public:

  /// constructor
  vxViewerType(vxEnvironment & environment);

  /// destructor
  virtual ~vxViewerType();

  /// initializes the view
  virtual void InitializeView() = 0;

  /// initializes the view/object in view based on an annotation
  virtual bool InitializeFrom(vxAnnotation & annotation) { return false; };

  /// initializes the OpenGL view setting
  virtual void SetupView() = 0;

  /// helper to convert from mouse to world
  bool ConvertMouseToWorld(const Point2D<int2> & mouseCoord, Point<float4> & worldCoord, bool bConsiderRenderingMode = true) const;

  /// convert from mouse to screen
  virtual Point2D<int2> ConvertMouseToScreen(const Point2D<int2> & mouseCoord) const;

  /// convert from screen to world
  virtual bool ConvertScreenToWorld(const Point2D<int2> & screenCoord, Point<float4> & worldCoord, bool bConsiderRenderingMode = true) const;

  /// convert from world to screen
  virtual bool ConvertWorldToScreen(const Point<float4> & worldCoord, Point2D<int2> & screenCoord) const;

  /// helper to convert from world to screen using given OpenGL parameters (NOT SO NICE OF A SOLUTION!!!)
  bool ConvertWorldToScreen(const Point<float4> & worldCoord, Point2D<int2> & screenCoord,
                            const GLint * piViewport, const GLdouble * pfModelviewMatrix, 
                            const GLdouble * pfPorjectionMatrix) const ;

	 /// get the world coordinate of the given screen coordinate
  bool ConvertScreenToWorldUsingOpenGL(const Point2D<int2> & screenCoord, Point<float4> & worldCoord) const;

  /// get the screen coordinate of the given world coordinate
  bool ConvertWorldToScreenUsingOpenGL(const Point<float4> & worldCoord, Point2D<int2> & screenCoord) const;

  /// is this annotation type supported by this viewer type?
  virtual bool IsAnnotationSupported(const vxHandlesObj & handlesObject) const = 0;

  /// should it be drawn on the screen?
  virtual bool IsVisible(const vxHandlesObj & handlesObject) const = 0;

  /// is this coordinate visible in the current view?
  virtual bool IsVisible(const Point3Df & worldCoord) const = 0;

  /// processes the modified queue
  virtual void Modified(vxModifiedInfoList & infoList);

  /// called for each modified list item
  virtual void OnModified(const type_info & type, const vxModifiedInfo & info) { };

  /// get the rendering coordinate system
  const RenderCoordinateSystemEnum & GetRenderCoordinateSystem() const { return m_eRenderCoordinateSystem; }

  /// set the rendering coordinate system
  void SetRenderCoordinateSystem(const RenderCoordinateSystemEnum & eRenderCoordinateSystem) { m_eRenderCoordinateSystem = eRenderCoordinateSystem; }

  /// is the primary renderer 2D?
  virtual BOOL Is2D() const = 0;

  /// is the primary renderer 3D?
  virtual BOOL Is3D() const = 0;

  ///Indicates whether the type has a rendering engine plugin or not
	virtual bool HasRenderingNodePlugin() { return false; }

  /// gets the rendering engine plugin
  vxRenderingNodePlugin & GetRenderingNodePlugin();

  /// returns if the viewer supports dropping of the specified type
  virtual bool CanDragDrop(const DragDropEnum eSourceType) { return false; }

  /// returns the name of the class for type checking (here till dynamic_cast works across DLLs)
  virtual const std::string GetClassString() const = 0;

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
 
protected:
	
  /// creates the instance of a rendering engine plugin
	virtual vxRenderingNodePlugin * CreateRenderingEnginePlugin(){ return NULL;}

  /// get the environment
  vxEnvironment & GetEnvironment() const { return m_environment; };


  /// get the world coordinate given a set of ray iterators
  bool ConvertRayToWorld(vxBlockVolumeRayIterator<uint2> & dataIter, vxBlockVolumeRayIterator<uint2> & labelIter, Point3Df & worldPos) const;

private:

  /// copy constructor
  vxViewerType(const vxViewerType & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxViewerType & operator=(const vxViewerType & other); // should only be public if really implemented!

// data
private:

  /// environment, this variable doesn't really define the state of the object -> mutable
  vxEnvironment & m_environment; 

  /// coordinate system to use when rendering visual elements on the screen such as annotations
  RenderCoordinateSystemEnum m_eRenderCoordinateSystem;

protected:

	// plugin
  vxRenderingNodePlugin * m_pRenderingEnginePlugin;

}; // class vxViewerType


// $Log: vxViewerType.h,v $
// Revision 1.17  2007/07/31 16:00:57  romy
// exposed ConvertScreenToWorldUsingOpenGL methods
//
// Revision 1.16  2007/03/23 14:34:38  romy
// new DragDropEnum added
//
// Revision 1.15  2007/03/05 15:23:27  geconomos
// code review preparation
//
// Revision 1.14  2007/02/05 20:17:20  geconomos
// change vxViewerType::GetRenderingEnginePlugin => GetRenderingNodePlugin
//
// Revision 1.13  2007/02/05 19:28:17  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.12  2006/12/05 18:26:18  romy
// optiomization for PluginMethods
//
// Revision 1.11  2006/12/04 20:28:40  romy
// Eliminated the Rendering Engine's excess Threads
//
// Revision 1.10  2006/12/04 20:27:05  romy
// Eliminated the Rendering Engine's excess Threads
//
// Revision 1.9  2006/10/05 17:00:13  geconomos
// promoted the GetXXXPlane to public
//
// Revision 1.8  2006/08/03 14:07:50  geconomos
// added "rendering engine" support
//
// Revision 1.7  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.6  2001/01/03 19:18:44  michael
// Made GetEnvironment() function "protected", not "public". Was used to cheat ...
//
// Revision 1.5  2004/08/05 13:54:46  michael
// reduced code to e.g. have GetLeftPlane() return -GetRightPlane() instead of
// having the code twice with *-1.
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
// Revision 1.33  2004/01/14 14:18:17  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.32  2003/12/23 23:02:38  wenli
// Add GetBoundingRect() to slove the ssue #3167: missing ablility to move the annotation text in curved
//
// Revision 1.31  2003/09/22 15:54:04  michael
// added "mailto:"
//
// Revision 1.30  2003/09/15 14:57:46  frank
// exposed the environment so viewers can see each other's data
//
// Revision 1.29  2003/08/21 13:09:48  michael
// added functions for OpenGL setup. Needs to move out of camera and become
// part of the viewer type
//
// Revision 1.28  2003/06/18 15:19:53  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.27  2003/04/29 14:31:18  frank
// Removed IsManipulatorSupported
//
// Revision 1.26  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.25  2003/04/24 14:01:45  frank
// code review
//
// Revision 1.24  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.23  2003/04/16 13:42:26  michael
// performed initial code review
//
// Revision 1.22  2003/04/11 17:59:51  frank
// Refactored 2D/3D selection code so that the same algorithm is used regardless of 2D or 3D and the mode (x-ray, MIP, thin slab, normal)
//
// Revision 1.21  2003/03/30 21:03:23  michael
// formatting and cosmetics
//
// Revision 1.20  2003/03/25 17:31:33  frank
// Added virtual function to return class name until dynamic_cast problem across DLLs is resolved.
//
// Revision 1.19  2003/03/20 21:28:16  frank
// Individual IsVisible() for a single point, rather than for a whole annotation
//
// Revision 1.18  2003/03/18 21:23:55  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.17  2003/03/18 20:42:33  michael
// added method to initialize the view/plane based on a given annotation
//
// Revision 1.16  2003/03/17 19:47:34  geconomos
// Issue #2914. Added DropType property.
//
// Revision 1.15  2003/03/06 15:57:52  frank
// Moved Is2D and Is3D to vxViewerType
//
// Revision 1.14  2003/03/05 12:51:08  frank
// Extended interface to include the ability to render in either screen or world space.
//
// Revision 1.13  2003/03/04 15:34:17  geconomos
// Added methods for processing parent environment's modified queue.
//
// Revision 1.12  2003/03/03 16:50:39  geconomos
// Added and implemented InitializeCamera method.
//
// Revision 1.11  2003/02/28 17:54:33  michael
// partially moved SetupView() from vxViewer over to vxViewerType ...
//
// Revision 1.10  2003/02/24 14:57:07  frank
// Added local coordinate transformation for non-OpenGL projections
//
// Revision 1.9  2003/01/22 21:48:29  ingmar
// disabled forward declarations
//
// Revision 1.8  2003/01/22 15:07:02  michael
// adjusted code to account for our Y-flip in the GetScreenCoordinates()
//
// Revision 1.7  2003/01/17 00:33:21  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.6  2003/01/15 16:29:52  michael
// made methods const that do not alter the state
//
// Revision 1.5  2003/01/13 23:19:35  michael
// added get/set for the environment (protected member in base class)
//
// Revision 1.4  2003/01/09 14:56:04  michael
// adjusted methods to take new annotation classes
//
// Revision 1.3  2002/12/18 18:22:02  michael
// moved OpenGL frame buffer reading to base class
//
// Revision 1.2  2002/12/18 14:52:15  geconomos
// added virtual specifier to destructor
//
// Revision 1.1  2002/12/17 14:41:41  michael
// initial version of viewer type base class and the MPROrtho implementation
//
// $Id: vxViewerType.h,v 1.17 2007/07/31 16:00:57 romy Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerType.h,v 1.17 2007/07/31 16:00:57 romy Exp $
