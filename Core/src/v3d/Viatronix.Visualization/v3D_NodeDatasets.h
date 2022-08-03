// $Id: v3D_NodeDatasets.h,v 1.6 2007/09/19 14:34:09 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson(mailto:gdavidson@viatronix.com)

#pragma once


// includes
#include "v3D_NodeIDs.h"
#include "vxNodeDatasetsCycler.h"


// forward declarations
OPEN_VISUALIZATION_NS
ref class Viewer;
ref class Dataset;
CLOSE_VISUALIZATION_NS


// namespaces
OPEN_GRAPH_NS


// forward declarations
ref class VisualizationGraph;


public ref class NodeDatasets : public IVisualizationGraphNode
{
public:

  // destructor
  ~NodeDatasets();

  // finalizer
  !NodeDatasets();

  /// gets the node identifier
  virtual property System::String ^ ID { System::String ^ get() { return NodeIDs::NodeDatasets; } }

  /// gets or sets the visualization graph
  virtual property Viatronix::Visualization::Graph::VisualizationGraph ^ VisualizationGraph
  {
    Viatronix::Visualization::Graph::VisualizationGraph ^ get() { return m_mpVisualizationGraph; }
    void set( Viatronix::Visualization::Graph::VisualizationGraph ^ mpGraph ) { m_mpVisualizationGraph = mpGraph; }
  } // VisualizationGraph

  // gets the unmanaged graph pointer
  virtual System::IntPtr GetNodePtr() { return System::IntPtr( m_pNodeDatasets ); }

  /// syncs the viewer with the current dataset
  void SyncViewer( Viewer ^ mpViewer );

  /// syncs the viewer with the current dataset
  static void SyncViewer( Viewer ^ mpViewer, Dataset ^ mpDataset );

  /// gets the current dataset index
  property int4 CurrentDatasetIndex 
  { 
    int4 get() { return m_pNodeDatasets->GetDatasetIndex(); }
    void set( int4 iIndex ) { m_pNodeDatasets->SetDatasetIndex( iIndex ); }
  }

  /// gets or sets the node enabled
  property bool Enabled
  {
    bool get() { return m_pNodeDatasets->IsEnabled(); }
    void set( bool enabled ) { m_pNodeDatasets->SetEnabled( enabled ); }
  }

// event
public:

  /// dataset index changed event
  event System::EventHandler ^ DatasetChanged;

private:

  /// raises index changed event
  void OnIndexChanged();

// construction
internal:

  // default constructor
  NodeDatasets();

// members
private:

  /// gc handle for managed delegate
  System::Runtime::InteropServices::GCHandle  m_gcHandle;

  /// method delegate
  System::Windows::Forms::MethodInvoker ^ m_mpFunctionPtr;

  /// visualization graph
  Viatronix::Visualization::Graph::VisualizationGraph ^ m_mpVisualizationGraph;

  /// unmanaged pointer
  vxNodeDatasetsCycler * m_pNodeDatasets;

}; // class NodeDatasets


CLOSE_GRAPH_NS


// $Log: v3D_NodeDatasets.h,v $
// Revision 1.6  2007/09/19 14:34:09  gdavidson
// Added static SyncViewer method
//
// Revision 1.5  2007/06/22 15:16:41  geconomos
// Renamed vxNodeDatasets => vxNodeDatasetsCycler
//
// Revision 1.4  2007/05/31 18:47:12  gdavidson
// Changed the implementation for switching to a specific dataset
//
// Revision 1.3  2007/05/30 19:52:10  gdavidson
// Added an Enabled property
//
// Revision 1.2  2007/05/17 15:12:41  romy
// wrapper added for SwapInDataset
//
// Revision 1.1  2007/05/03 21:43:16  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_NodeDatasets.h,v 1.6 2007/09/19 14:34:09 gdavidson Exp $
// $Id: v3D_NodeDatasets.h,v 1.6 2007/09/19 14:34:09 gdavidson Exp $
