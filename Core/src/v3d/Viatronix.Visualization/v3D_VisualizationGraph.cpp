// $Id: v3D_VisualizationGraph.cpp,v 1.17.2.2 2009/07/29 19:54:07 kchalupa Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_VisualizationGraph.h"
#include "vxVisualizationGraph.h"
#include "v3D_Viewer.h"
#include "v3D_CpuInfo.h"
#include "v3D_NodeRendering.h"
#include "v3D_NodeGlobalController.h"
#include "v3D_NodeDatasets.h"
#include "v3D_Nodes.h"
#include "v3D_Dataset.h"
#include "vxIRenderingContext.h"
#include "v3D_Environment.h"
#include "v3D_GraphTimerEx.h"


// namespaces
USING_GRAPH_NS
USING_VISUALIZATION_NS
using namespace System::Collections::Generic;


/**
 * default constructor
 */
VisualizationGraph::VisualizationGraph() :
  m_pVisualizationGraph( new vxVisualizationGraph() ),
  m_mpCpuInfo( gcnew Viatronix::Visualization::CpuInfo() ),
  m_mpViewers( gcnew System::Collections::Generic::List< Viewer ^ >() ),
  m_mpViewerCache( gcnew System::Collections::Generic::List< Viewer ^ >() ),
  m_mpAvailableNodes( gcnew Dictionary< System::String ^, IVisualizationGraphNode ^ >() ),
  m_mpTimer( gcnew Viatronix::Visualization::Graph::GraphTimerEx() ),
  m_iItemsUsingTimer( 0 )
{
} // VisualizationGraph()


/**
 * destructor
 */
VisualizationGraph::~VisualizationGraph()
{
  // clean up each viewer
  for each( Viewer ^ mpViewer in m_mpViewerCache )
    delete mpViewer;
   
  this->!VisualizationGraph();
} // ~VisualizationGraph()


/**
 * Finalizer
 */
VisualizationGraph::!VisualizationGraph()
{
  delete m_pVisualizationGraph; 
  m_pVisualizationGraph = NULL;
} // !VisualizationGraph()


/**
 * Creates the default nodes for the graph.
 */
void VisualizationGraph::CreateDefaultNodes()
{
 // TODO: Move to XML
  IVisualizationGraphNode ^ mpNode = gcnew NodeDatasets();
  mpNode->VisualizationGraph = this;
  m_mpAvailableNodes->Add( mpNode->ID, mpNode );

  mpNode = gcnew NodeGlobalController();
  mpNode->VisualizationGraph = this;
  m_mpAvailableNodes->Add( mpNode->ID, mpNode );

  mpNode = gcnew NodeLocalController();
  mpNode->VisualizationGraph = this;
  m_mpAvailableNodes->Add( mpNode->ID, mpNode );

  mpNode = gcnew NodeRendering();
  mpNode->VisualizationGraph = this;
  m_mpAvailableNodes->Add( mpNode->ID, mpNode );

  mpNode = gcnew NodeDrawing();
  mpNode->VisualizationGraph = this;
  m_mpAvailableNodes->Add( mpNode->ID, mpNode );

  mpNode = gcnew NodeDispatcher();
  mpNode->VisualizationGraph = this;
  m_mpAvailableNodes->Add( mpNode->ID, mpNode );
} // CreateDefaultNodes()


/** 
 * creates the node for the graph.
 *
 * @param mpID  The desired id
 */
void VisualizationGraph::CreateNode( System::String ^ mpID )
{
  if( mpID == NodeIDs::NodeDatasets )
  {
    IVisualizationGraphNode ^ mpNode = gcnew NodeDatasets();
    mpNode->VisualizationGraph = this;
    m_mpAvailableNodes->Add( mpNode->ID, mpNode );
  }
  else if( mpID == NodeIDs::NodeGlobalController )
  {
    IVisualizationGraphNode ^ mpNode = gcnew NodeGlobalController();
    mpNode->VisualizationGraph = this;
    m_mpAvailableNodes->Add( mpNode->ID, mpNode );
  }
  else if( mpID == NodeIDs::NodeLocalController )
  {
    IVisualizationGraphNode ^ mpNode = gcnew NodeLocalController();
    mpNode->VisualizationGraph = this;
    m_mpAvailableNodes->Add( mpNode->ID, mpNode );
  }
  else if( mpID == NodeIDs::NodeRendering )
  {
    IVisualizationGraphNode ^ mpNode = gcnew NodeRendering();
    mpNode->VisualizationGraph = this;
    m_mpAvailableNodes->Add( mpNode->ID, mpNode );
  }
  else if( mpID == NodeIDs::NodeDrawing )
  {
    IVisualizationGraphNode ^ mpNode = gcnew NodeDrawing();
    mpNode->VisualizationGraph = this;
    m_mpAvailableNodes->Add( mpNode->ID, mpNode );
  }
  else if( mpID == NodeIDs::NodeDispatcher )
  {
    IVisualizationGraphNode ^ mpNode = gcnew NodeDispatcher();
    mpNode->VisualizationGraph = this;
    m_mpAvailableNodes->Add( mpNode->ID, mpNode );
  }
  else
  {
    throw gcnew System::ApplicationException( "The node is not available [NODE=" + mpID + "]" );
  }
} // CreateNode( mpID )


/**
 * adds the node to the graph
 *
 * @param mpID  node id
 */
void VisualizationGraph::AddNode( System::String ^ mpID )
{
  if( HasNode( mpID ))
  {
    throw gcnew System::ArgumentException( "The graph already contains the node. [ID=" + mpID + "]" );
  }
 
  IVisualizationGraphNode ^ mpNode( nullptr );
  if( m_mpAvailableNodes->TryGetValue( mpID, mpNode ))
  {
    m_pVisualizationGraph->AddNode( ss( mpID ), reinterpret_cast< vxVisualizationGraphNode * >( mpNode->GetNodePtr().ToPointer() ));
  }
  else
  {
    throw gcnew System::NullReferenceException( "The node is not available [NODE=" + mpID + "]" );
  }
} // AddNode( mpID )


/**
 * adds the node to the graph
 *
 * @param             mpID                node id
 */
void VisualizationGraph::RemoveNode( System::String ^ mpID )
{
  IVisualizationGraphNode ^ mpNode( nullptr );
  if( m_mpAvailableNodes->TryGetValue( mpID, mpNode ))
  {
    m_pVisualizationGraph->RemoveNode( ss( mpID ));
    reinterpret_cast< vxVisualizationGraphNode * >( mpNode->GetNodePtr().ToPointer() )->Reset();
  }
} // RemoveNode( mpID )


/**
 * checks if the graph contains the node
 *
 * @param        mpID                node id
 * @return       true if the node is in the graph, otherwise false
 */
bool VisualizationGraph::HasNode( System::String ^ mpID )
{
  return m_pVisualizationGraph->HasNode( ss( mpID ));
} // HasNode( mpID )


/**
 * gets the node with the provided id
 *
 * @param        mpID                node id
 * @return       IVisualizationGraphNode object
 */
IVisualizationGraphNode ^ VisualizationGraph::GetNode( System::String ^ mpID )
{
  if( HasNode( mpID ))
  {
    IVisualizationGraphNode ^ mpNode( nullptr );
    m_mpAvailableNodes->TryGetValue( mpID, mpNode );
    return mpNode;
  }

  return nullptr;
} // GetNode( mpID )


/**
 * gets the node with the provided id
 *
 * @param        mpID                node id
 * @return       IVisualizationGraphNode object
 */
generic <typename T>
where T : IVisualizationGraphNode
T VisualizationGraph::GetNode( System::String ^ mpID )
{
  return safe_cast< T >( GetNode( mpID ));
} // GetNode( mpID )
  

/** 
 * clears all nodes
 */
void VisualizationGraph::ClearNodes()
{
  m_pVisualizationGraph->ClearNodes();
} // ClearNodes()



/**
 * Initializes the visualization graph
 */
void VisualizationGraph::Initialize()
{
  std::vector< vxViewerControl * > viewers;
  for each( Viewer ^ mpViewer in m_mpViewers )
  {
    viewers.push_back( reinterpret_cast< vxViewerControl * >( mpViewer->GetViewerPtr().ToPointer() ));
  }

  m_pVisualizationGraph->Initialize( viewers );
} // Initialize()


/**
 * Resets the visualization graph.
 */ 
void VisualizationGraph::Reset()
{
  // raise Resetting event
  this->Resetting( this, System::EventArgs::Empty );

  // reset all viewers in the cache
  for each( Viewer ^ mpViewer in m_mpViewerCache )
  {
    mpViewer->Reset();
    mpViewer->Visible = false;
    mpViewer->Environment->IgnoreModifiedEvents = true;
    mpViewer->BorderColor = System::Drawing::Color::Black;
  }

  m_pVisualizationGraph->Reset();
  m_mpViewers->Clear();
} // Reset()


/**
 * Runs the visualization graph.
 */ 
void VisualizationGraph::Run()
{
  m_pVisualizationGraph->Run();
} // Run()


/**
 * Aborts the visualization graph.
 */ 
void VisualizationGraph::Abort()
{
  m_pVisualizationGraph->Abort();
} // Abort()


/**
 * Starts the graph timer
 */
void VisualizationGraph::StartTimer()
{
  System::Threading::Interlocked::Increment( m_iItemsUsingTimer );
  m_pVisualizationGraph->SetRunning( true );
  m_mpTimer->Start();
} // StartTimer()


/**
 * Stops the graph timer
 */
void VisualizationGraph::StopTimer()
{
  if( System::Threading::Interlocked::Decrement( m_iItemsUsingTimer ) <= 0 )
  {
    m_mpTimer->Stop();
    m_pVisualizationGraph->SetRunning( false );
    System::Threading::Interlocked::Exchange( m_iItemsUsingTimer, 0 );
  }
} // StopTimer()


/**
 * Stops the graph timer
 */
void VisualizationGraph::HaltTimer()
{
  m_iItemsUsingTimer = 0;
  m_pVisualizationGraph->SetRunning( false );
  m_mpTimer->Stop();
} // HaltTimer()


/**
 * Gets the rendering context mode
 */
Viatronix::Visualization::RenderingContextMode VisualizationGraph::RenderingContextMode::get()
{ 
  return static_cast< Viatronix::Visualization::RenderingContextMode >( m_pVisualizationGraph->GetRenderingContextMode() ); 
} // get_RenderingContextMode


/** 
 * Sets the rendering context mode
 */
void VisualizationGraph::RenderingContextMode::set( Viatronix::Visualization::RenderingContextMode eMode )
{ 
  m_pVisualizationGraph->SetRenderingContextMode( static_cast< vxIRenderingContext::RenderingContextEnum >( eMode )); 
} // set_RenderingContextMode( eMode )


/** 
 * creates a collection of viewers
 *
 * @param         iCount            number of viewers to be created
 * @return        a collection of viewers
 */
array< Viatronix::Visualization::Viewer ^ > ^ VisualizationGraph::AllocateViewers( int4 iCount )
{
  // create any additional viewers for this layout and add to the cache
  const int4 iNum = System::Math::Max( 0, iCount - m_mpViewerCache->Count );
  for( int4 i = 0; i < iNum; ++i )
  {
    Viewer ^ mpViewer = gcnew Viewer();
    m_mpViewerCache->Add( mpViewer );
    mpViewer->Environment->VisualizationGraph = this;
  }

  m_mpViewers->Clear();
  for( int4 i = 0; i < iCount; ++i )
  {
    m_mpViewers->Add( m_mpViewerCache[i] );
  }

  return m_mpViewers->ToArray();
} // AllocateViewers( iCount )



// $Log: v3D_VisualizationGraph.cpp,v $
// Revision 1.17.2.2  2009/07/29 19:54:07  kchalupa
// Code Walkthrough
//
// Revision 1.17.2.1  2009/01/27 17:24:40  gdavidson
// Changed to GraphTimerEx for visualization graph
//
// Revision 1.17  2008/01/22 21:03:39  gdavidson
// Issue #5966: Added CreateNode to the VisualizationGraph
//
// Revision 1.16  2007/12/05 15:52:25  geconomos
// + moved addition of default nodes from constructor to AddDefaultNodes()
// + moved calling of visualization settings to static initializer of visualization graph
//
// Revision 1.15  2007/10/24 20:06:16  gdavidson
// Moved viewer's lifecycle ownership to the graph
//
// Revision 1.14  2007/06/28 19:34:14  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.13  2007/05/03 21:29:09  gdavidson
// Refactored VisualizationGraph
//
// Revision 1.12  2007/04/18 17:02:31  gdavidson
// Added a method that aborts the DatasetThread
//
// Revision 1.11  2007/03/22 18:23:36  gdavidson
// Added property for accessing CpuInfo
//
// Revision 1.10.2.2  2007/03/27 21:54:33  gdavidson
// Rolled back
//
// Revision 1.10.2.1  2007/03/22 21:34:46  gdavidson
// Issue #5505: Added ResetUserInteracting method
//
// Revision 1.10  2007/03/01 20:33:43  gdavidson
// Commented code
//
// Revision 1.9  2007/02/12 22:46:14  gdavidson
// Issue #5354: Modified the ordering of operations in the reset
//
// Revision 1.8  2007/02/05 19:29:12  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.7  2007/01/16 17:09:35  gdavidson
// Added StartTimer and StopTimer methods
//
// Revision 1.6  2007/01/08 17:15:19  gdavidson
// Refactored VisualizationGraph code
//
// Revision 1.1  2006/11/09 15:07:05  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_VisualizationGraph.cpp,v 1.17.2.2 2009/07/29 19:54:07 kchalupa Exp $
// $Id: v3D_VisualizationGraph.cpp,v 1.17.2.2 2009/07/29 19:54:07 kchalupa Exp $
