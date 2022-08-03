// $Id: vxVisualizationGraph.C,v 1.20.2.2 2009/05/01 21:46:25 gdavidson Exp $
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
#include "vxVisualizationGraph.h"
#include "vxVisualizationGraphNode.h"
#include "vxModifiedQueue.h"
#include "vxModifiedSet.h"
#include "vxObserver.h"
#include "vxVisualizationPreferences.h"
#include "vxEnvironment.h"
#include "vxViewer.h"
#include "vxCine.h"
#include "vxElementInt4.h"
#include "vxDatasetsInfo.h"
#include "vxNodeDatasetsCycler.h"
#include "vxNodeIDs.h"
#include "vxRenderingContextDIB.h"
#include "vxRenderingContextPbuffer.h"
#include "vxRenderingContextGL.h"
#include "vxViewerControl.h"


// defines
#define FILE_REVISION "$Revision: 1.20.2.2 $"


// static member initialization
bool vxVisualizationGraph::m_bFlushing( false );
bool vxVisualizationGraph::m_bViewChanging( false );


/**
 * default constructor
 */
vxVisualizationGraph::vxVisualizationGraph() :
  m_bTimerRunning( false ),
  m_eRenderingContext( vxIRenderingContext::WINDOWED )
{
} // vxVisualizationGraph()


/**
 * destructor
 */
vxVisualizationGraph::~vxVisualizationGraph()
{
  // clean up fonts
  for( std::map< vxEnvironment *, GLFontGeorge * >::iterator iter = m_fonts.begin(); iter != m_fonts.end(); ++iter )
  {
    delete iter->second;
  }
  m_fonts.clear();

  // clean up contexts
  for( std::map< vxEnvironment *, std::pair< vxIRenderingContext *, GLPaintDevice * >>::iterator iter = m_contexts.begin(); iter != m_contexts.end(); ++iter )
  {
    // clean up rendering context
    if( iter->second.first != NULL )
    {
      iter->second.first->Destroy();
      delete iter->second.first;

      iter->second.first = NULL;
    }

    // clean up device
    if( iter->second.second != NULL )
    {
      delete iter->second.second;
      iter->second.second = NULL;
    }
  }
  m_contexts.clear();
} // ~vxVisualizationGraph()


/**
 * Adds the node to the graph
 *
 * @param         nodeID            identifier of the node
 * @param         pNode             vxVisualizationGraphNode object
 */
void vxVisualizationGraph::AddNode( std::string sNodeID, vxVisualizationGraphNode * pNode )
{
  m_nodes.push_back( pNode );
  m_nodeMap[ sNodeID ] = pNode;
} // AddNode( nodeID, pNode )


/**
 * Removes a node from a graph
 *
 * @param        nodeID            identifier of the node
 */
void vxVisualizationGraph::RemoveNode( const std::string & sNodeID )
{
  vxVisualizationGraphNode * pNode = m_nodeMap[ sNodeID ];
  m_nodeMap.erase( sNodeID );
  m_nodes.erase( std::find( m_nodes.begin(), m_nodes.end(), pNode ));
} // RemoveNode( nodeID )


/**
 * checks if the graph contains the node
 *
 * @param        nodeID            identifier of the node
 * @return       true if the node is in the graph, otherwise false
 */
bool vxVisualizationGraph::HasNode( const std::string & sKey )
{
  return m_nodeMap.find( sKey ) != m_nodeMap.end();
} // HasNode( sKey )


/** 
 * clears the nodes
 */
void vxVisualizationGraph::ClearNodes()
{
  m_nodes.clear();
  m_nodeMap.clear();
} // ClearNodes()


/**
 * initializes the graph
 *
 * @param         viewers               collection of viewers
 */
void vxVisualizationGraph::Initialize( std::vector< vxViewerControl * > & viewerControls )
{
  m_viewerControls = viewerControls;

  // initialize each graph node
  for( std::vector< vxVisualizationGraphNode * >::iterator iter = m_nodes.begin(); iter != m_nodes.end(); ++iter )
  {
    (*iter)->SetVisualizationGraph( this );
    (*iter)->Initialize();
  }
} // Initialize( viewers )


/**
 * Resets the graph
 */
void vxVisualizationGraph::Reset()
{
  Abort();

  for( std::vector< vxVisualizationGraphNode * >::iterator iter = m_nodes.begin(); iter != m_nodes.end(); ++iter )
    (*iter)->Reset();

  m_viewerControls.clear();
} // Reset()


/**
 * Aborts the graph
 */
void vxVisualizationGraph::Abort()
{
  for( std::vector< vxVisualizationGraphNode * >::iterator iter = m_nodes.begin(); iter != m_nodes.end(); ++iter )
    (*iter)->Abort();
} // Abort()


/**
 * Sends all of the modified messages to the sink clients
 */
void vxVisualizationGraph::Run()
{
  try
  {
		//If in the middle of  view changing don't do a flush
		if ( m_bViewChanging )
			return;

    InternalFlush();
  
    if( m_bFlushing || m_viewerControls.size() == 0 )
      return;

    // run each graph node
    for( std::vector< vxVisualizationGraphNode * >::iterator iter = m_nodes.begin(); iter != m_nodes.end(); ++iter )
    {
      (*iter)->Run();
      if( (*iter)->IsFlushNecessary() )
        InternalFlush();
    }
  }
  catch( ... )
  {
    LogDbg( "+++ UNHANDLED EXCEPTION CAUGHT +++", "vxVisualizationGraph", "Run" );
  }
} // Run()



/**
 * Internal flush
 */
void vxVisualizationGraph::InternalFlush()
{
  // set to true if this method has been called recursively
  static bool bFlush = false;

  if( m_bFlushing )
  {
    // flush called while flushing
    bFlush = true;
  }
  else
  {
    // send all modified messages to the clients
    m_bFlushing = true;
		register int4 i;
		for( i=0; i<vxModifiedQueue::GetMessages().size(); ++i )
			vxModifiedQueue::GetMessages()[i]->GetClient()->OnModified( vxModifiedQueue::GetMessages()[i]->GetMessages() );
    m_bFlushing = false;

    // release all objects
    vxModifiedQueue::Clear();

    // check if the buffer is empty
    if( !vxModifiedQueue::GetBuffer().empty() )
    {
      // transfer messages from the buffer
			for( i=0; i<vxModifiedQueue::GetBuffer().size(); ++i )
				vxModifiedQueue::GetMessages().push_back( vxModifiedQueue::GetBuffer()[i] );
      vxModifiedQueue::GetBuffer().clear();

      // check if flush was called
      if( bFlush )
      {
        bFlush = false;
        InternalFlush();
      }
    } // if
  }
} // InternalFlush()


/**
 * Sets the View Changing flag
 */
void vxVisualizationGraph::SetViewChanging( bool bViewChanging )
{
	m_bViewChanging = bViewChanging; 

	//call an explicit flush to clear all the last modified bits
	vxModifiedQueue::Flush();

	//Is the view changing so reset and clear all those nodes.
	if ( m_bViewChanging == true )
	{
		Reset();
		ClearNodes();
	}

}//SetViewChanging( bool bViewChanging )


/**
 * Draws the statistics for the specified environment
 *
 * @param   environment       environment to retrieve statistics from
 */
void vxVisualizationGraph::DrawStatistics( vxEnvironment & environment )
{
  static char s[100];
 
  vxShareableObject<vxOverlayStrings> * pOverlayStrings = dynamic_cast<vxShareableObject<vxOverlayStrings> *>( environment.GetElement( vxIDs::OverlayStrings ) );

  vxRenderBuffer & buffer = environment.GetRenderBuffer();


  // threading statistics
  //std::stringstream ss;
  //for( int4 i = 0; i < buffer.GetThreadingStatistics().size(); ++i )
  //{
  //  ss << "T " << i << ": " << std::setprecision(2) << buffer.GetThreadingStatistics()[i];
  //  if( i < buffer.GetRenderThreadCount() - 1 )
  //    ss << "   ";
  //}
  //pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( ss.str(), vxOverlayStrings::vxOverlayElement::LowerRight, Triple<float4>(1, 1, 0) ) );



  // flags
  char sFlags[20];
  const bool bUserInteracting = environment.GetUserInteracting();
  const bool bGraphRunning = vxModifiedQueue::GetVisualizationGraph().IsRunning();
  if( bUserInteracting || bGraphRunning )
  {
    sprintf_s( sFlags, 20,"%s%s", bGraphRunning? "R" : "-", bUserInteracting? "I" : "-"  );
  }
  else
    strcpy_s( sFlags, 20, "--" );

  const float fps = 1.0F / environment.GetFrameRateTimer().ElapsedTime();   environment.GetFrameRateTimer().Reset();
  const Triple< uint4 > & renderSize = buffer.GetRenderSize();

  // only 1 dataset loaded
  if( vxDatasetsInfo::GetNumberOfDatasets() == 1 )
    sprintf_s( s, 100,"%.1f fps: [%d,%d] : %s : %d", fps,  renderSize.m_x, renderSize.m_y, sFlags, buffer.GetRenderThreadCount() );
  else
  {
    int4 iDatasetIndex = 1;
    vxNodeDatasetsCycler * pNodeDatasets = dynamic_cast< vxNodeDatasetsCycler * >( vxModifiedQueue::GetVisualizationGraph().GetNode( vxNodeIDs::NodeDatasets ));
    if( pNodeDatasets != NULL )
      iDatasetIndex = pNodeDatasets->GetDatasetIndex() + 1;
    const int4 iDatasetCount = vxDatasetsInfo::GetNumberOfDatasets();
    sprintf_s( s, 100,"%.1f fps: [%d,%d] : %d/%d : %s : %d", fps,  renderSize.m_x, renderSize.m_y, iDatasetIndex, 
                                                             iDatasetCount, sFlags, buffer.GetRenderThreadCount() );
  }


  pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( s, vxOverlayStrings::vxOverlayElement::LowerRight, Triple<float4>(1, 1, 0) ) );

} // DrawStatistics( vxEnvironment & environment )


/**
 * gets the rendering context
 *
 * @param       pEnvironment          associated environment
 * @returns     associated rendering context
 */
vxIRenderingContext * vxVisualizationGraph::GetRenderingContext( vxEnvironment * pEnvironment )
{
  if( m_eRenderingContext == vxIRenderingContext::SHARED )
  {
    if( m_contexts.empty() )
    {
      vxRenderingContextPBuffer * pContext = new vxRenderingContextPBuffer();
      pContext->Create( 2048, 2048 );
      m_contexts[ NULL ] = std::pair< vxIRenderingContext *, GLPaintDevice * >( pContext, __nullptr );
    }

    return m_contexts.begin()->second.first;
  }
  else if( m_eRenderingContext == vxIRenderingContext::DIB )
  {
    if( m_contexts.empty() )
    {
      vxRenderingContextDIB * pContext = new vxRenderingContextDIB();
      pContext->Create( 2048, 2048 );
      m_contexts[ NULL ] = std::pair< vxIRenderingContext *, GLPaintDevice * >( pContext, __nullptr );
    }

    return m_contexts.begin()->second.first;
  }
  else if( m_eRenderingContext == vxIRenderingContext::WINDOWED )
  {
    if( m_contexts.find( pEnvironment ) == m_contexts.end() )
    {
      HWND hWnd = dynamic_cast< vxWindowHandle * >( pEnvironment->GetElement( vxIDs::WindowHandle ) )->GetHandle();
  
      // initialize the paint device
      GLPaintDevice * pDevice = new GLPaintDevice();
      pDevice->SetHDC( GetDC( hWnd ));
      pDevice->SetHWND( hWnd );

      vxRenderingContextGL * pContext = new vxRenderingContextGL( GLFormat(), pDevice );
      pContext->Create();
      pContext->SetInitialized( true );
      m_contexts[ pEnvironment ] = std::pair< vxIRenderingContext *, GLPaintDevice * >( pContext, pDevice );
    }

    return m_contexts[ pEnvironment ].first;
  }
  else
  {
    return NULL;
  }
} // GetRenderingContext( pEnvironment )


/**
 * gets the rendering context
 *
 * @param       pEnvironment          associated environment
 * @returns     associated rendering context
 */
GLFontGeorge * vxVisualizationGraph::GetFont( vxEnvironment * pEnvironment )
{
  if( m_eRenderingContext == vxIRenderingContext::SHARED || m_eRenderingContext == vxIRenderingContext::DIB )
  {
    if( m_fonts.empty() )
    {
      GLFontGeorge * pFont = new GLFontGeorge();
      pEnvironment->UpdateFont( pFont, GetRenderingContext( pEnvironment )); 
      m_fonts[ NULL ] = pFont;
    }

    return  m_fonts.begin()->second;
  }
  else if( m_eRenderingContext == vxIRenderingContext::WINDOWED )
  {
    if( m_fonts.find( pEnvironment ) == m_fonts.end() )
    {
      GLFontGeorge * pFont = new GLFontGeorge();
      pEnvironment->UpdateFont( pFont, GetRenderingContext( pEnvironment )); 
      m_fonts[ pEnvironment ] = pFont;
    }

    return m_fonts[ pEnvironment ];
  }
  else 
  {
    return NULL;
  }
} // GetFont( pEnvironment )


// $Log: vxVisualizationGraph.C,v $
// Revision 1.20.2.2  2009/05/01 21:46:25  gdavidson
// Temporarily removed threading statistics
//
// Revision 1.20.2.1  2009/01/27 17:23:56  gdavidson
// Changed to GraphTimerEx for visualization graph
//
// Revision 1.20  2007/12/05 15:53:14  geconomos
// code cleanup
//
// Revision 1.19  2007/10/24 20:15:42  gdavidson
// Removed vxViewer collection and replaced with vxViewerControl collection
//
// Revision 1.18  2007/09/04 19:03:12  romy
// Flush suspend added to improve performance. ClearNodes during the switch views
//
// Revision 1.17  2007/09/04 14:25:41  romy
// Added a flag to indicate that the ViewChanging is happening. This flag aids to suspend the ModifiedQueue flush operation until the view change is over.
//
// Revision 1.16  2007/06/22 15:14:42  geconomos
// Renamed vxNodeDatasets => vxNodeDatasetsCycler
//
// Revision 1.15  2007/05/03 21:30:07  gdavidson
// Refactored vxVisualizationGraph
//
// Revision 1.14  2007/04/12 14:22:29  geconomos
// corrected bad checkin
//
// Revision 1.13  2007/04/12 14:13:41  geconomos
// switched threading to tile-based approach
//
// Revision 1.12  2007/04/03 15:15:37  gdavidson
// Added dataset index to the statistics
//
// Revision 1.11  2007/04/02 21:30:20  gdavidson
// Added IsModified to the vxRenderBuffer
//
// Revision 1.10  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.9  2007/02/12 22:46:14  gdavidson
// Issue #5354: Modified the ordering of operations in the reset
//
// Revision 1.8  2007/02/05 19:28:17  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.7  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.6  2007/01/24 14:41:55  gdavidson
// Moved creation of hidden window to the constructor of the VisualizationGraphTimer
//
// Revision 1.5  2007/01/22 15:33:34  geconomos
// updated statistics output
//
// Revision 1.4  2007/01/17 22:09:57  gdavidson
// Issue #5303: Added check for the flushing flag
//
// Revision 1.3  2007/01/17 16:41:52  geconomos
// exposed timer IsRunning
//
// Revision 1.2  2007/01/17 14:09:30  geconomos
// removed m_bFlushing check in Run (not needed)
//
// Revision 1.1  2007/01/16 16:27:28  gdavidson
// Changed extension
//
// Revision 1.1  2007/01/08 17:02:48  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxVisualizationGraph.C,v 1.20.2.2 2009/05/01 21:46:25 gdavidson Exp $
// $Id: vxVisualizationGraph.C,v 1.20.2.2 2009/05/01 21:46:25 gdavidson Exp $
