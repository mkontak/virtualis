// $Id: v3D_NodeIDs.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson(mailto:gdavidson@viatronix.com)

#pragma once


// namespaces
OPEN_GRAPH_NS


// forward declarations
ref class VisualizationGraph;


public ref class NodeIDs sealed
{
public:

  /// gets the "NodeRendering" element id
  property static System::String ^ NodeRendering { System::String ^ get(); }  

  /// gets the "NodeDatasets" element id
  property static System::String ^ NodeDatasets { System::String ^ get(); }  

  /// gets the "NodeGlobalController" element id
  property static System::String ^ NodeGlobalController { System::String ^ get(); }  

  /// gets the "NodeLocalController" element id
  property static System::String ^ NodeLocalController { System::String ^ get(); }  

  /// gets the "NodeDrawing" element id
  property static System::String ^ NodeDrawing { System::String ^ get(); }  

  /// gets the "NodeDispatcher" element id
  property static System::String ^ NodeDispatcher { System::String ^ get(); }  


private:

  /// default constructor
  NodeIDs() {}

}; // class Nodes


public interface class IVisualizationGraphNode
{

  /// gets the node identifier
  property System::String ^ ID { System::String ^ get(); }

  /// gets or sets the visualization graph
  property Viatronix::Visualization::Graph::VisualizationGraph ^ VisualizationGraph
  {
    Viatronix::Visualization::Graph::VisualizationGraph ^ get();
    void set( Viatronix::Visualization::Graph::VisualizationGraph ^ );
  } // VisualizationGraph

  /// gets the implementing vxVisualizationGraphNode pointer
  System::IntPtr GetNodePtr();

}; // interface IVisualizationGraphNode


CLOSE_GRAPH_NS


// $Log: v3D_NodeIDs.h,v $
// Revision 1.1  2007/05/03 21:43:16  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_NodeIDs.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
// $Id: v3D_NodeIDs.h,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
