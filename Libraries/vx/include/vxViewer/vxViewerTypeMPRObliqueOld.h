// $Id: vxViewerTypeMPRObliqueOld.h,v 1.2 2007/03/05 15:23:27 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner, (mailto:michael@viatronix.com)

#ifndef vxViewerTypeMPRObliqueOld_h
#define vxViewerTypeMPRObliqueOld_h


// includes
#include "vxViewerType.h"


// forward declarations


// class definition
class VX_VIEWER_DLL vxViewerTypeMPRObliqueOld: public vxViewerType
{
// functions
public:

  /// constructor
  vxViewerTypeMPRObliqueOld(vxEnvironment & environment);

  /// copy constructor
  virtual ~vxViewerTypeMPRObliqueOld() {};

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
  virtual const std::string GetClassString() const { return "vxViewerTypeMPRObliqueOld"; }

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
  vxViewerTypeMPRObliqueOld(const vxViewerTypeMPRObliqueOld & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxViewerTypeMPRObliqueOld & operator=(const vxViewerTypeMPRObliqueOld & other); // should only be public if really implemented!
};


#endif // vxViewerTypeMPRObliqueOld_h


// Revision History:
// $Id: vxViewerTypeMPRObliqueOld.h,v 1.2 2007/03/05 15:23:27 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerTypeMPRObliqueOld.h,v 1.2 2007/03/05 15:23:27 geconomos Exp $
