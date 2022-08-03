// $Id: vxViewerTypeGraph.h,v 1.5 2007/08/09 18:55:22 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Romy Sreedharan (mailto:romy@viatronix.com)

// pragmas
#pragma once


// includes
#include "vxViewerType.h"

class vxGraph;

// class definition
class VX_VIEWER_DLL vxViewerTypeGraph: public vxViewerType
{
// functions
public:

  /// constructor
  vxViewerTypeGraph(vxEnvironment & environment);

  /// copy constructor
  virtual ~vxViewerTypeGraph() {}

  /// initializes the view
  virtual void InitializeView();

  /// initializes the OpenGL view setting
  virtual void SetupView();

  /// is this annotation type supported by this viewer type?
  virtual bool IsAnnotationSupported(const vxHandlesObj & handlesObject) const;

  /// should it be drawn on the screen?
  virtual bool IsVisible(const vxHandlesObj & handlesObject) const;

	  /// is the primary renderer is 2D
  virtual BOOL Is2D() const { return false; }

  /// is the primary renderer is 3D
  virtual BOOL Is3D() const { return false; }

  /// Is this position visible in the current view?
  virtual bool IsVisible(const Point3Df & position) const;

  /// handles modified events
  virtual void OnModified(const type_info & type, const vxModifiedInfo & info);

  /// returns if the viewer supports dropping of the specified type
  virtual bool CanDragDrop(const DragDropEnum eSourceType);

  /// returns the name of the class for type checking (here till dynamic_cast works across DLLs)
  virtual const std::string GetClassString() const { return "vxViewerTypeGraph"; }

  ///Get the graph object
	virtual vxGraph* GetGraph(){ return NULL; };

	///Indicates whether the type has a rendering engine plugin or not
	virtual bool HasRenderingNodePlugin() { return true; }

	///Creates an instance of a rendering engine plugin for this particular viewer type
  vxRenderingNodePlugin * CreateRenderingEnginePlugin();

private:

  /// copy constructor
  vxViewerTypeGraph(const vxViewerTypeGraph & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxViewerTypeGraph & operator=(const vxViewerTypeGraph & other); // should only be public if really implemented!
};


// Revision History:
// $Log: vxViewerTypeGraph.h,v $
// Revision 1.5  2007/08/09 18:55:22  romy
// type definition expanded
//
// Revision 1.4  2007/08/07 14:57:46  romy
// added a dummy PluginNode to avoid VizualizationGraph initialization crash -
//
// Revision 1.3  2007/03/01 21:51:58  romy
// added code comments
//
// Revision 1.2  2006/11/02 03:47:33  romy
// added new viewer type
//
// Revision 1.1  2006/11/02 03:36:48  romy
// added new viewer type
//
// Revision 1.1  2006/11/02 03:31:05  romy
// added new viewer type
//
// Revision 1.1  2006/10/31 15:41:31  romy
// test version
//
// $Id: vxViewerTypeGraph.h,v 1.5 2007/08/09 18:55:22 romy Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerTypeGraph.h,v 1.5 2007/08/09 18:55:22 romy Exp $
