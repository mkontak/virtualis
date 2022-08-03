// $Id: v3D_NodeGlobalController.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
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


// forward declarations
class vxNodeGlobalController;

// forward declarations
OPEN_VISUALIZATION_NS
interface class IGlobalControllerFactory;
CLOSE_VISUALIZATION_NS


// namespaces
OPEN_GRAPH_NS


// forward declarations
ref class VisualizationGraph;


public ref class NodeGlobalController : public IVisualizationGraphNode
{
public:

  // destructor
  ~NodeGlobalController();

  // finalizer
  !NodeGlobalController();

  /// gets the node identifier
  virtual property System::String ^ ID { System::String ^ get() { return NodeIDs::NodeGlobalController; } }

  /// gets or sets the visualization graph
  virtual property Viatronix::Visualization::Graph::VisualizationGraph ^ VisualizationGraph
  {
    Viatronix::Visualization::Graph::VisualizationGraph ^ get() { return m_mpVisualizationGraph; }
    void set( Viatronix::Visualization::Graph::VisualizationGraph ^ mpGraph ) { m_mpVisualizationGraph = mpGraph; }
  } // VisualizationGraph

  // gets the unmanaged graph pointer
  virtual System::IntPtr GetNodePtr() { return System::IntPtr( m_pNodeGlobalController ); }

  /// Add a GlobalControllers to the VisualizationGraph
  void AddGlobalController( System::Type ^ mpFactoryType );

  /// Remove a GlobalController from the VisualizationGraph
  void RemoveGlobalController( System::Type ^ mpFactoryType );

  /// Check if the VisualizationGraph contains a GlobalController
  bool HasGlobalController( System::Type ^ mpFactoryType );

  /// Removes all GlobalControllers
  void ClearGlobalControllers();

// construction
internal:

  // default constructor
  NodeGlobalController();

private:

  /// clears the global controllers
  void OnClear();

// members
private:

  /// gc handle for managed delegate
  System::Runtime::InteropServices::GCHandle  m_gcHandle;

  /// method delegate
  System::Windows::Forms::MethodInvoker ^ m_mpFunctionPtr;

  /// managed global controllers
  System::Collections::Generic::Dictionary< System::Type ^, IGlobalControllerFactory ^ > ^ m_mpGlobalControllers;

  /// visualization graph
  Viatronix::Visualization::Graph::VisualizationGraph ^ m_mpVisualizationGraph;

  /// unmanaged pointer
  vxNodeGlobalController * m_pNodeGlobalController;

}; // class NodeGlobalController


CLOSE_GRAPH_NS


// $Log: v3D_NodeGlobalController.h,v $
// Revision 1.1  2007/05/03 21:43:16  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_NodeGlobalController.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
// $Id: v3D_NodeGlobalController.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
