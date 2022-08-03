// $Id: vxViewerTypeMPRCurved.h,v 1.7 2007/08/09 19:24:40 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxViewerType.h"


// forwards
class vxRibbon;


/**
 * a viewer type for the curved MPR views
 */
class VX_VIEWER_DLL vxViewerTypeMPRCurved : public vxViewerType
{
// functions
public:

  /// constructor
  vxViewerTypeMPRCurved(vxEnvironment & environment);

  /// copy constructor
  virtual ~vxViewerTypeMPRCurved();

  /// initializes the OpenGL view setting
  virtual void SetupView();

  /// initializes the view
  virtual void InitializeView();

  /// initializes the view/object in view based on an annotation
  virtual bool InitializeFrom( vxAnnotation & annotation );

  /// convert from mouse to screen
  virtual Point2D<int2> ConvertMouseToScreen(const Point2D<int2> & mouseCoord) const;

  /// convert screen coordinates to world coordinates
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
  virtual bool IsVisible( const Point3Df & position ) const;

  /// returns if the viewer supports dropping of the specified type
  virtual bool CanDragDrop(const DragDropEnum eSourceType);


  /// Compute the nearest point on the curved plane.
  static bool ComputeNearestCurvedPoint( vxRibbon * pRibbon, const Point<float4> & queryPosition,
                                         float4 & fNearestScanline, float4 & fPercentAlongScanline,
                                         float4 & fNearestDistance );

  /// is the primary renderer is 2D
  virtual BOOL Is2D() const { return true; }

  /// is the primary renderer is 3D
  virtual BOOL Is3D() const { return false; }

  /// returns the name of the class for type checking (here till dynamic_cast works across DLLs)
  virtual const std::string GetClassString() const { return "vxViewerTypeMPRCurved"; }

 	///Indicates whether the type has a rendering engine plugin or not
	virtual bool HasRenderingNodePlugin() { return true; }

	///// creates the instance of a rendering engine plugin
	virtual vxRenderingNodePlugin * CreateRenderingEnginePlugin();

private:

  /// copy constructor
  vxViewerTypeMPRCurved(const vxViewerTypeMPRCurved & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxViewerTypeMPRCurved & operator=(const vxViewerTypeMPRCurved & other); // should only be public if really implemented!

  /// compute the sum of all distances of the handle with the given scanline
  float4 GetSumDistance( const vxHandlesObj & handlesObject ) const;

}; // class vxViewerTypeMPRCurved


// $Log: vxViewerTypeMPRCurved.h,v $
// Revision 1.7  2007/08/09 19:24:40  romy
// added drag and drop support
//
// Revision 1.6  2007/03/05 15:23:27  geconomos
// code review preparation
//
// Revision 1.5  2007/03/02 21:40:54  geconomos
// cleanup
//
// Revision 1.4  2007/02/05 19:28:17  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.3  2006/12/04 20:35:32  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.2  2006/09/14 20:10:50  geconomos
// added rendering engine
//
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.3  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.2  2005/06/08 15:02:49  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.3  2004/04/26 19:03:14  michael
// replaced "bool" by "BOOL"
//
// Revision 1.2  2004/03/02 19:19:17  frank
// renamed method in viewer type to more accurately describe it
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2004/03/02 01:10:29  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.7.2.1  2004/02/25 15:00:49  frank
// formatting
//
// Revision 1.7  2004/01/14 14:18:16  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.6  2004/01/07 16:50:27  wenli
// Issue # 3570 : Adjusting an arrow annotation from the Curved MPR is increasing the annotation size
//
// Revision 1.5  2003/12/23 23:02:09  wenli
// Add GetBoundingRect() to slove the ssue #3167: missing ablility to move the annotation text in curved
//
// Revision 1.4  2003/12/15 16:09:10  frank
// Issue #3569: Initialized the oblique and curved views from annotations
//
// Revision 1.3  2003/11/17 19:20:34  frank
// Preparing to initialize the view from an annotation
//
// Revision 1.2  2003/10/22 14:47:46  frank
// Added camera initialization to the viewer type
//
// Revision 1.1  2003/05/08 19:59:28  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.12  2003/04/29 14:31:31  frank
// Removed IsManipulatorSupported
//
// Revision 1.11  2003/04/21 15:54:44  dongqing
// code review
//
// Revision 1.10  2003/03/25 17:31:33  frank
// Added virtual function to return class name until dynamic_cast problem across DLLs is resolved.
//
// Revision 1.9  2003/03/20 21:28:46  frank
// Individual IsVisible() for a single point, rather than for a whole annotation
//
// Revision 1.8  2003/03/18 21:27:50  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.7  2003/03/06 15:59:00  frank
// Moved Is2D and Is3D to vxViewerType
//
// Revision 1.6  2003/03/05 12:51:56  frank
// Extended interface to include the ability to render in either screen or world space.
//
// Revision 1.5  2003/03/03 18:31:08  geconomos
// Empty implementaion for InitializeCamera ( temporary )
//
// Revision 1.4  2003/03/03 18:28:24  frank
// got projection working
//
// Revision 1.3  2003/02/28 20:55:15  frank
// Updated to reflect changes in base class
//
// Revision 1.2  2003/02/28 19:24:56  frank
// Added projection function
//
// Revision 1.1  2003/02/18 22:32:10  frank
// Initial version
//
// $Id: vxViewerTypeMPRCurved.h,v 1.7 2007/08/09 19:24:40 romy Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerTypeMPRCurved.h,v 1.7 2007/08/09 19:24:40 romy Exp $
