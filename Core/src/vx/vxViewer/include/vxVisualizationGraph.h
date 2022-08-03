// $Id: vxVisualizationGraph.h,v 1.9.2.2 2009/07/28 16:32:09 kchalupa Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#pragma once

// includes
#include "vxVisualizationGraphTimer.h"
#include "vxEnvironment.h"
#include "vxRenderingContextDIB.h"


// forward declaration
class vxControllerGlobal;
class vxViewer;
class vxRenderingNode;
class vxVisualizationGraphNode;
class vxViewerControl;
class vxEnvironment;
class GLPaintDevice;
class GLFontGeorge;


/**
 * Represents a visualization graph
 */
class VX_VIEWER_DLL vxVisualizationGraph
{
// construction
public:

  /// default constructor
  vxVisualizationGraph();

  /// destructor
  virtual ~vxVisualizationGraph();

// member functions
public:

  /// determines if the graph is flushing
  inline bool IsFlushing() const { return m_bFlushing; }

  /// adds a node to the graph
  void AddNode( std::string sNodeID, vxVisualizationGraphNode * pNode );

  /// removes a node to the graph
  void RemoveNode( const std::string & sKey );

  /// checks if the graph contains the node
  bool HasNode( const std::string & sKey );

  /// clears the nodes
  void ClearNodes();

  /// intializes the graph
  void Initialize( std::vector< vxViewerControl * > & viewerControls );

  /// resets the graph
  void Reset();

  /// aborts the graph
  void Abort();

  /// sends all of the modified messages to the sink clients
  void Run();

  /// internal flush
  void InternalFlush();

  /// indicates if the graph is running
  bool IsRunning() { return m_bTimerRunning; }

  /// draws the statistics for the specified environment
  static void DrawStatistics( vxEnvironment & environment );

  /// returns a reference to a collection of viewers
  inline std::vector< vxViewerControl * > & GetViewerControls() { return m_viewerControls; }

  /// returns a references to a viewer
  inline vxViewerControl * GetViewerControl( int4 iIndex ) { return m_viewerControls[ iIndex ]; }

  /// gets a visualization node
  inline vxVisualizationGraphNode * GetNode( const std::string key ) { return m_nodeMap[ key ]; }

  /// sets the timer as running
  void SetRunning( bool bRunning ) { m_bTimerRunning = bRunning; }

    /// returns user interacting
  inline bool GetUserInteracting() const { return vxEnvironment::GetUserInteracting(); }

  /// sets user interacting
  inline void SetUserInteracting( bool bInteracting ) { vxEnvironment::SetUserInteracting( bInteracting ); }

  /// get the rendering context mode
  inline vxIRenderingContext::RenderingContextEnum GetRenderingContextMode() const { return m_eRenderingContext; }

  /// set the rendering context mode
  inline void SetRenderingContextMode( const vxIRenderingContext::RenderingContextEnum eMode ) { m_eRenderingContext = eMode; }

  /// gets the rendering context
  vxIRenderingContext * GetRenderingContext( vxEnvironment * pEnvironment );

  /// gets the font
  GLFontGeorge * GetFont( vxEnvironment * pEnvironment );

	///Sete the view changing event.
	void SetViewChanging( bool bViewChanging );

// member variables
private:

  /// collection of viewers
  std::vector< vxViewerControl * > m_viewerControls;

  /// collection of rendering contexts
  std::map< vxEnvironment *, std::pair< vxIRenderingContext *, GLPaintDevice * >> m_contexts;

  /// collection of rendering contexts
  std::map< vxEnvironment *, GLFontGeorge * > m_fonts;

  /// collection of nodes
  std::vector< vxVisualizationGraphNode * > m_nodes;

  /// map of strings to nodes
  std::map< std::string, vxVisualizationGraphNode * > m_nodeMap;

  /// is the timer running
  bool m_bTimerRunning;
  
  /// rendering context enum
  vxIRenderingContext::RenderingContextEnum m_eRenderingContext;

  /// indicates if the queue is being flushed
  static bool m_bFlushing;

	/// flag indicates whether the view is changing or not
	static bool m_bViewChanging;

}; // class vxVisualizationGraph


// $Log: vxVisualizationGraph.h,v $
// Revision 1.9.2.2  2009/07/28 16:32:09  kchalupa
// Code Walkthrough
//
// Revision 1.9.2.1  2009/01/27 17:23:57  gdavidson
// Changed to GraphTimerEx for visualization graph
//
// Revision 1.9  2007/10/24 20:15:35  gdavidson
// Removed vxViewer collection and replaced with vxViewerControl collection
//
// Revision 1.8  2007/09/04 14:25:41  romy
// Added a flag to indicate that the ViewChanging is happening. This flag aids to suspend the ModifiedQueue flush operation until the view change is over.
//
// Revision 1.7  2007/06/28 19:34:09  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.6  2007/05/03 21:30:07  gdavidson
// Refactored vxVisualizationGraph
//
// Revision 1.5.2.2  2007/03/27 21:54:33  gdavidson
// Rolled back
//
// Revision 1.5.2.1  2007/03/22 21:35:31  gdavidson
// Issue #5505: Added ResetUserInteracting method
//
// Revision 1.5  2007/02/05 19:28:17  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.4  2007/01/17 16:41:52  geconomos
// exposed timer IsRunning
//
// Revision 1.3  2007/01/16 21:19:33  gdavidson
// Added UserInteracting methods
//
// Revision 1.2  2007/01/16 16:50:52  gdavidson
// Added vxVisualizationGraphTimer
//
// Revision 1.1  2007/01/08 17:02:35  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxVisualizationGraph.h,v 1.9.2.2 2009/07/28 16:32:09 kchalupa Exp $
// $Id: vxVisualizationGraph.h,v 1.9.2.2 2009/07/28 16:32:09 kchalupa Exp $
