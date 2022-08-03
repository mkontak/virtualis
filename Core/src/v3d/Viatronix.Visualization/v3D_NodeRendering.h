// $Id: v3D_NodeRendering.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

// pragmas
#pragma once


// includes
#include "v3D_NodeIDs.h"


// forward declarations
class vxNodeRendering;


// forward declarations
OPEN_VISUALIZATION_NS
ref class Triplet;
CLOSE_VISUALIZATION_NS


// namespaces
OPEN_GRAPH_NS


// forward declarations
ref class VisualizationGraph;


// class declarations
public ref class NodeRendering sealed : IVisualizationGraphNode
{
// member functions
public:

  // destructor
  ~NodeRendering();

  // finalizer
  !NodeRendering();

  /// gets the node identifier
  virtual property System::String ^ ID { System::String ^ get() { return NodeIDs::NodeRendering; } }

  /// gets or sets the visualization graph
  virtual property Viatronix::Visualization::Graph::VisualizationGraph ^ VisualizationGraph
  {
    Viatronix::Visualization::Graph::VisualizationGraph ^ get() { return m_mpVisualizationGraph; }
    void set( Viatronix::Visualization::Graph::VisualizationGraph ^ mpGraph ) { m_mpVisualizationGraph = mpGraph; }
  } // VisualizationGraph

  // gets the unmanaged graph pointer
  virtual System::IntPtr GetNodePtr() { return System::IntPtr( m_pRenderingNode ); }

  /// gets the is rendering flag
  property bool IsRendering { bool get(); }

  /// gets the high quality thread enabled flag
  property bool EnableHighQualityThread{ bool get(); void set( bool bEnabled ); }

  /// gets the fixed size rendering flag
  property bool EnableFixedSizeRendering{ bool get(); void set( bool bEnabled ); }

  /// gets the fixed rendering size
  property Viatronix::Visualization::Triplet ^ FixedRenderingSize 
  { 
    Viatronix::Visualization::Triplet ^ get();  
    void set( Viatronix::Visualization::Triplet ^ mpSize ); 
  }

  /// gets or sets the number of threads available for rendering
  property int4 NumberOfRenderThreads { int4 get(); void set( int4 iThreadCount ); }

// construction
internal:

  /// constructor
  NodeRendering();

// member variables
private: 

  /// visualization graph
  Viatronix::Visualization::Graph::VisualizationGraph ^ m_mpVisualizationGraph;
  
  /// unmanaged singleton rendering manager
  vxNodeRendering * m_pRenderingNode;

}; // class RenderingNode


CLOSE_GRAPH_NS

// $Log: v3D_NodeRendering.h,v $
// Revision 1.1  2007/05/03 21:43:16  gdavidson
// Initial revision
//
// Revision 1.3  2007/03/22 18:24:01  gdavidson
// Added property for NumberOfRenderThreads
//
// Revision 1.2  2007/03/06 19:36:47  geconomos
// code review
//
// Revision 1.1  2007/02/05 19:28:00  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.8  2007/01/08 17:15:19  gdavidson
// Refactored VisualizationGraph code
//
// Revision 1.7  2006/12/11 19:14:09  geconomos
// issue #5152 - crashing when application is minimized or maximized
//
// Revision 1.6  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.5  2006/09/27 16:08:14  geconomos
// added support for fixed size rendering
//
// Revision 1.4  2006/08/31 14:14:22  geconomos
// changes to rendering manager
//
// Revision 1.3  2006/08/28 21:07:55  geconomos
// added ability to disable the high quality image thread
//
// Revision 1.2  2006/08/28 14:04:07  geconomos
// added Reset() method
//
// Revision 1.1  2006/08/03 14:19:50  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_NodeRendering.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
// $Id: v3D_NodeRendering.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
