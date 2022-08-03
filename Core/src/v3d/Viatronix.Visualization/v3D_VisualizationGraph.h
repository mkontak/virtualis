// $Id: v3D_VisualizationGraph.h,v 1.13.2.2 2009/07/29 19:54:07 kchalupa Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson(mailto:gdavidson@viatronix.com)

#pragma once

// includes
#include "vxModifiedQueue.h"
#include "vxVisualizationGraph.h"
#include "v3D_Enums.h"


// forward declarations
OPEN_VISUALIZATION_NS
interface class IGlobalControllerFactory;
ref class Viewer;
ref class RenderingNode;
ref class CpuInfo;
ref class Dataset;
CLOSE_VISUALIZATION_NS


// Namespaces
OPEN_GRAPH_NS


// forward declarations
ref class GraphTimerEx;
interface class IVisualizationGraphNode;


public ref class VisualizationGraph
{
// events
public:

  /// raised when the graph is resetting
  event System::EventHandler ^  Resetting;

// construction
public:

  /// static constructor
  static VisualizationGraph()
  {  
    // TODO: work out this mess
    m_mpInstance = gcnew VisualizationGraph();
    
    vxModifiedQueue::SetVisualizationGraph( m_mpInstance->m_pVisualizationGraph );

    System::Configuration::ConfigurationSettings::GetConfig( "visualizationPreferences" );

    m_mpInstance->CreateDefaultNodes();
  } // VisualizationGraph()

  /// default constructor
  VisualizationGraph();

  /// destructor
  virtual ~VisualizationGraph();

  /// finalizer
  !VisualizationGraph();

// member functions
public:

  /// creates the default nodes in the graph.
  void CreateDefaultNodes();

  /// creates the node for the graph.
  void CreateNode( System::String ^ mpID );

  /// returns a pointer to the vxVisualizationGraph
  System::IntPtr GetElementPtr() { return System::IntPtr( m_pVisualizationGraph ); }

  /// adds the node to the graph
  void AddNode( System::String ^ mpID );

  /// adds the node to the graph
  void RemoveNode( System::String ^ mpID );

  /// checks if the graph contains the node
  bool HasNode( System::String ^ mpID );

  /// gets a node
  IVisualizationGraphNode ^ GetNode( System::String ^ mpID );

  /// gets a node of a specific type
  generic <typename T>
  where T : IVisualizationGraphNode
  T GetNode( System::String ^ mpID );

  /// clears all nodes
  void ClearNodes();

  /// initializes the visualization graph
  void Initialize();

  /// Resets the VisualizationGraph
  void Reset();

  /// runs the VisualizationGraph
  void Run();

  /// Aborts the visualization graph
  void Abort();

  /// Starts the timer
  void StartTimer();

  /// Stops the timer
  void StopTimer();

  /// Stops the timer
  void HaltTimer();

  /// creates a collection of viewers
  array< Viatronix::Visualization::Viewer ^ > ^ AllocateViewers( int4 iCount );

// properties
public:
	
	/// gets or sets user interacting
  property bool ViewChanging
  {
    //bool get() { return m_pVisualizationGraph->GetViewChanging(); }
    void set( bool bViewChanging ) { m_pVisualizationGraph->SetViewChanging( bViewChanging ); }
  } // SetViewChanging

  /// gets or sets user interacting
  property bool UserInteracting
  {
    bool get() { return m_pVisualizationGraph->GetUserInteracting(); }
    void set( bool bInteracting ) { m_pVisualizationGraph->SetUserInteracting( bInteracting ); }
  } // UserInteracting

  /// returns a CpuInfo object
  property Viatronix::Visualization::CpuInfo ^ CpuInfo
  { 
    Viatronix::Visualization::CpuInfo ^ get() { return m_mpCpuInfo; }
  } // CpuInfo

  /// returns a collection of viewers
  property System::Collections::Generic::List< Viatronix::Visualization::Viewer ^ > ^ Viewers
  { 
    System::Collections::Generic::List< Viatronix::Visualization::Viewer ^ > ^ get() { return m_mpViewers; }
  } // Viewers

  /// gets or sets the rendering context mode
  property Viatronix::Visualization::RenderingContextMode RenderingContextMode
  {
    Viatronix::Visualization::RenderingContextMode get();
    void set( Viatronix::Visualization::RenderingContextMode eMode );
  } // RenderingContextMode

  /// returns the rendering manager
  static property Viatronix::Visualization::Graph::VisualizationGraph ^ Instance
  {
    Viatronix::Visualization::Graph::VisualizationGraph ^ get() { return m_mpInstance; }
  } // RenderingNode

  /// returns a viewer cache
  property System::Collections::Generic::List< Viatronix::Visualization::Viewer ^ > ^ ViewerCache
  {
    System::Collections::Generic::List< Viatronix::Visualization::Viewer ^ > ^ get() { return m_mpViewerCache; }
  } // ViewerCache

// member variables
private:

  /// visualization graph
  vxVisualizationGraph * m_pVisualizationGraph;

  /// cpu information
  Viatronix::Visualization::CpuInfo ^ m_mpCpuInfo;

  /// collection of available nodes
  System::Collections::Generic::Dictionary< System::String ^, IVisualizationGraphNode ^ > ^ m_mpAvailableNodes;

  /// collection of viewers
  System::Collections::Generic::List< Viatronix::Visualization::Viewer ^ > ^ m_mpViewers;

  /// cache of viewers
  System::Collections::Generic::List< Viatronix::Visualization::Viewer ^ > ^ m_mpViewerCache;

  /// graph timer
  Viatronix::Visualization::Graph::GraphTimerEx ^ m_mpTimer;

  /// number of items using timer
  int4 m_iItemsUsingTimer;

  /// singleton instance
  static Viatronix::Visualization::Graph::VisualizationGraph ^ m_mpInstance;

}; // class VisualizationGraph


CLOSE_GRAPH_NS


// $Log: v3D_VisualizationGraph.h,v $
// Revision 1.13.2.2  2009/07/29 19:54:07  kchalupa
// Code Walkthrough
//
// Revision 1.13.2.1  2009/01/27 17:24:41  gdavidson
// Changed to GraphTimerEx for visualization graph
//
// Revision 1.13  2008/01/22 21:03:39  gdavidson
// Issue #5966: Added CreateNode to the VisualizationGraph
//
// Revision 1.12  2007/12/05 15:52:25  geconomos
// + moved addition of default nodes from constructor to AddDefaultNodes()
// + moved calling of visualization settings to static initializer of visualization graph
//
// Revision 1.11  2007/10/24 20:06:16  gdavidson
// Moved viewer's lifecycle ownership to the graph
//
// Revision 1.10  2007/09/04 14:26:07  romy
// Added a flag to indicate that the ViewChanging is happening. This flag aids to suspend the ModifiedQueue flush operation until the view change is over.
//
// Revision 1.9  2007/06/28 19:34:14  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.8  2007/05/03 21:29:09  gdavidson
// Refactored VisualizationGraph
//
// Revision 1.7  2007/04/18 17:02:31  gdavidson
// Added a method that aborts the DatasetThread
//
// Revision 1.6  2007/03/22 18:23:36  gdavidson
// Added property for accessing CpuInfo
//
// Revision 1.5.2.2  2007/03/27 21:54:33  gdavidson
// Rolled back
//
// Revision 1.5.2.1  2007/03/22 21:34:46  gdavidson
// Issue #5505: Added ResetUserInteracting method
//
// Revision 1.5  2007/02/05 19:29:12  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.4  2007/01/16 21:19:33  gdavidson
// Added UserInteracting methods
//
// Revision 1.3  2007/01/16 17:09:23  gdavidson
// Added StartTimer and StopTimer methods
//
// Revision 1.2  2007/01/08 17:15:19  gdavidson
// Refactored VisualizationGraph code
//
// Revision 1.1  2006/11/09 15:07:05  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_VisualizationGraph.h,v 1.13.2.2 2009/07/29 19:54:07 kchalupa Exp $
// $Id: v3D_VisualizationGraph.h,v 1.13.2.2 2009/07/29 19:54:07 kchalupa Exp $
