// $Id: vxNodeDatasetsCycler.C,v 1.5.2.1 2009/04/07 19:56:53 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

// includes
#include "StdAfx.h"
#include "vxNodeDatasetsCycler.h"
#include "vxVisualizationGraph.h"
#include "vxViewer.h"
#include "vxViewerControl.h"
#include "vxDatasetsInfo.h"
#include "vxDataset.h"
#include "vxCycleSettings.h"
#include "vxNodeRendering.h"
#include "vxNodeIDs.h"
#include "vxNodeDatasetsCyclerThread.h"


#define FILE_REVISION "$Revision: 1.5.2.1 $"


/**
 * constructor
 */
vxNodeDatasetsCycler::vxNodeDatasetsCycler() : vxVisualizationGraphNode()
{
  m_bThreadRunning = false;
  m_iCurrentDatasetIndex = 0;
  m_pIndexChangedFuncPtr = NULL;
  m_pDatasetsCyclerThread = new vxNodeDatasetsCyclerThread();
  m_pDatasetsCyclerThread->Start();
  m_pDatasetsCyclerThread->SetPriority( Thread::LOWER );
} // vxNodeDatasets()


/**
 * constructor
 *
 * @param           funcPtr             callback function  
 */
vxNodeDatasetsCycler::vxNodeDatasetsCycler( IndexChangedFuncPtr pFuncPtr ) : vxVisualizationGraphNode()
{
  m_iCurrentDatasetIndex = 0;
  m_bThreadRunning = false;
  m_pIndexChangedFuncPtr = pFuncPtr;

  m_pDatasetsCyclerThread = new vxNodeDatasetsCyclerThread();
  m_pDatasetsCyclerThread->Start();
  m_pDatasetsCyclerThread->SetPriority( Thread::LOWER );

} // vxNodeDatasets( pFuncPtr )


/**
 * destructor
 */
vxNodeDatasetsCycler::~vxNodeDatasetsCycler()
{
  if( m_pDatasetsCyclerThread != NULL )
  {
    delete m_pDatasetsCyclerThread;
    m_pDatasetsCyclerThread = NULL;
  }
} // ~vxNodeDatasets()



/**
 * Performs initialization
 */
void vxNodeDatasetsCycler::Initialize()
{
  // cache viewers
  m_viewers.clear();
  m_cycleSettings.clear();
  for( int i = 0; i < GetVisualizationGraph()->GetViewerControls().size(); ++i )
  {
    vxViewer * pViewer = GetVisualizationGraph()->GetViewerControl( i )->GetViewer( vxViewerControl::MainViewer );
    m_viewers.push_back( pViewer );
    m_cycleSettings.push_back( dynamic_cast< vxCycleSettings * >(  pViewer->GetEnvironment().GetElement( vxIDs::CycleSettings ) ) );
  }
  
    
  // free buffers
  for( int4 i = 0; i < m_cache.size(); ++i )
  {
    for( int4 j = 0; j < m_cache[ i ].size(); ++j )
      delete m_cache[ i ][ j ];
    m_cache[ i ].clear();
  }
  m_cache.clear();

  // create new buffers
  m_cache.resize( m_viewers.size() );
  for( int4 i = 0; i < m_cache.size(); ++i )
  {
    m_cache[ i ].resize( vxDatasetsInfo::GetNumberOfDatasets() );
    for( int4 j = 0; j < m_cache[ i ].size(); ++j )
    {
      vxCacheBuffer * pBuffer = new vxCacheBuffer();
      pBuffer->m_eState =  vxNodeDatasetsCycler::EMPTY;
      m_cache[ i ][ j ] = pBuffer;
    }
  }  

  m_pDatasetsCyclerThread->SetRenderingNode( dynamic_cast< vxNodeRendering * >( GetVisualizationGraph()->GetNode( vxNodeIDs::NodeRendering )));
  m_pDatasetsCyclerThread->SetDatasetsNode( this );
} // Initialize()


/**
 * Resets the rendering manager to a default state
 */
void vxNodeDatasetsCycler::Reset() 
{ 
  if( m_pDatasetsCyclerThread->IsRendering() )
    m_pDatasetsCyclerThread->StopRendering();
} // Reset() 


/**
 * Aborts the node
 */
void vxNodeDatasetsCycler::Abort()
{
  m_pDatasetsCyclerThread->StopRendering();
} // Abort()


/** 
 * Runs the rendering manager
 */
void vxNodeDatasetsCycler::Run()
{
  ////////////////////////////////////////////////////////////////////////////////
  // Step 1 
  //   - accumulate environment changes for all environments regardless if cycling or not
  //   - determine if any viewer is cycling
  //   - determine if any viewer needs rendering
  
  bool bAnyViewerCycling = false;
  bool bAnyViewerNeedsRendering  = false;
  for( int4 iViewerIndex = 0; iViewerIndex < m_cache.size(); ++iViewerIndex )
  {
    const vxViewer * pViewer = m_viewers[ iViewerIndex ];

    bAnyViewerCycling |= m_cycleSettings[ iViewerIndex ]->IsEnabled();

    if( pViewer->GetEnvironment().IsModified( vxEnvironment::ALL ) )
    {
      vxViewerType * pType = pViewer->GetEnvironment().GetViewerType();
      bool bNeedsRendering = pType->HasRenderingNodePlugin() && pType->GetRenderingNodePlugin().IsRenderingNecessary();
      for( int4 j = 0; j < m_cache[ iViewerIndex ].size(); ++j )
      {
        m_cache[ iViewerIndex ][ j ]->m_bits |= pViewer->GetEnvironment().GetModifiedBits();
        if( bNeedsRendering )
          m_cache[ iViewerIndex ][ j ]->m_eState = vxNodeDatasetsCycler::EMPTY;
      }
      bAnyViewerNeedsRendering |= bNeedsRendering;
    }
  }

  // break out at this point if the node is disabled
  if( m_bNodeEnabled == false )
    return;


  /////////////////////////////////////////////////////////
  // Step 2 
  //   -  abort all rendering if any viewer is cycling or needs rendering
  //   -  enable / diable the high quality thread in rendering node 

  vxNodeRendering * pRenderingNode =  dynamic_cast< vxNodeRendering * >( GetVisualizationGraph()->GetNode( vxNodeIDs::NodeRendering ));
  
  // abort rendering?
  if( bAnyViewerCycling || bAnyViewerNeedsRendering )
    pRenderingNode->Abort();

  // enable/disable high quality thread accordingly
  pRenderingNode->SetHighQualityThreadEnabled( !bAnyViewerCycling );

  // disable datasets thread if need be
  if( bAnyViewerNeedsRendering  )
  {
    if( m_pDatasetsCyclerThread->IsRendering() )
      m_pDatasetsCyclerThread->StopRendering();
    m_bThreadRunning = false;
  }

  // at this point we can bail if the user is interacting or there aren't any viewers cycling
  if( vxEnvironment::GetUserInteracting() || !bAnyViewerCycling )
    return;

  // enforce specified cycles per minute
  const float4 fSecondsInMinute = 60.0F;
  const float4 fCyclesPerMinute = 60.F;
  const float4 fDeltaForOneCycle = fSecondsInMinute / ( vxDatasetsInfo::GetNumberOfDatasets() * fCyclesPerMinute  );
  if( m_timer.ElapsedTime() < fDeltaForOneCycle )
    return;
  m_timer.Reset();

  
  //////////////////////////////
  // Step 3 
  //   - increment the current dataset index
  //   - swap in the new dataset for all viewers that are cycling

  m_iCurrentDatasetIndex = ( m_iCurrentDatasetIndex + 1 ) % vxDatasetsInfo::GetNumberOfDatasets();

  for( int4 iViewerIndex = 0; iViewerIndex < m_viewers.size(); ++iViewerIndex )
  {
    if( m_cycleSettings[ iViewerIndex ]->IsEnabled() )
    {
      vxEnvironment & environment = m_viewers[ iViewerIndex ]->GetEnvironment();

      vxDataset & dataset = *vxDatasetsInfo::GetDataset( m_iCurrentDatasetIndex );

      environment.SetIntensityVolume( dataset.GetIntensityVolume() );
      environment.SetHistogram( &dataset.GetIntensityVolume()->GetHistogram() );
      environment.SetLabelVolume( dataset.GetLabelVolume() );
      environment.SetComponents( dataset.GetComponents() );
      environment.SetDataset( &dataset );
    }
  }

  
  //////////////////////////////
  // Step 4 
  // - Cache the current rendered image in the buffer cache
  // - swap in buffered image for current viewer/dataset if exists or
  //   dirty environment so new image is generated in rendering node
  //

  for( int4 iViewerIndex = 0; iViewerIndex < m_viewers.size(); ++iViewerIndex )
  {
    // nothing to do if viewer isn't cycling
    if( !m_cycleSettings[ iViewerIndex ]->IsEnabled() )
      continue;

    vxViewer * pViewer = m_viewers[ iViewerIndex ];

    vxCacheBuffer * pCurrentCacheBuffer = m_cache[ iViewerIndex ][ m_iCurrentDatasetIndex ];

    /// bit of a hack for viewers that don't have a rendering node
    if( !pViewer->GetEnvironment().GetViewerType()->HasRenderingNodePlugin() )
    {
      // just set it to HQ to take it out of the equation
      pCurrentCacheBuffer->m_eState = vxNodeDatasetsCycler::HQ;
      continue;
    }

    // cache the current rendered image
    int4 iUpdateCacheIndex = ( m_iCurrentDatasetIndex - 1 ) < 0 ? vxDatasetsInfo::GetNumberOfDatasets() -1 : ( m_iCurrentDatasetIndex - 1 );
    if( m_cache[ iViewerIndex ] [ iUpdateCacheIndex ]->m_eState == vxNodeDatasetsCycler::EMPTY )
    {
      vxCacheBuffer * pPreviousCache = m_cache[ iViewerIndex ] [ iUpdateCacheIndex ];
      {
        SingleLock< CriticalSection > lock( &(pPreviousCache->m_buffer.GetSyncObject()), true );
        pPreviousCache->m_buffer = pViewer->GetEnvironment().GetRenderBuffer();
        
        if( pViewer->GetEnvironment().GetViewerType()->GetRenderingNodePlugin().SupportsAdaptiveRendering() && 
            pViewer->GetEnvironment().GetRenderBuffer().GetState() != vxRenderBuffer::HIGH )
        {
          pPreviousCache->m_eState = vxNodeDatasetsCycler::LQ;
        }
        else
        {
          pPreviousCache->m_eState = vxNodeDatasetsCycler::HQ;
        }
      }
    }

    // does this viewer/dataset have a cached image?
    if( pCurrentCacheBuffer->m_eState != vxNodeDatasetsCycler::EMPTY )
    {
      SingleLock< CriticalSection > lock( &(pCurrentCacheBuffer->m_buffer.GetSyncObject()), true );
	    // copy the image from the cache to the enviroment's render buffer
        pViewer->GetEnvironment().SetRenderBuffer( pCurrentCacheBuffer->m_buffer );

      if( pCurrentCacheBuffer->m_eState == vxNodeDatasetsCycler::LQ )
      {
        pViewer->GetEnvironment().GetRenderBuffer().SetState( vxRenderBuffer::LOW );
      }
      else
      {
        pViewer->GetEnvironment().GetRenderBuffer().SetState( vxRenderBuffer::HIGH );
      }
      
      pViewer->GetEnvironment().GetRenderBuffer().SetModified( true );
    }
    else // otherwise the cache buffer doesn't contain an image for the current viewer/dataset
    {
	    // dirty the environment so that an image is generated in the rendering node
      pViewer->GetEnvironment().SetModified( vxEnvironment::VOLUME_INTENSITY, true );
      pViewer->GetEnvironment().SetModified( vxEnvironment::VOLUME_LABEL, true );
      pViewer->GetEnvironment().SetModified( vxEnvironment::COMPONENT_VISUAL, true );
      pViewer->GetEnvironment().SetModified( vxEnvironment::COLORMAP, true );
	
	    // add in any accumulated bits
      pViewer->GetEnvironment().SetModifiedBits( pCurrentCacheBuffer->m_bits |= pViewer->GetEnvironment().GetModifiedBits() );
      pCurrentCacheBuffer->m_bits.reset(); // clear out, bits transferred
    }
  }
  
  
  ///////////////////////////////////////////////////////////////////
  // Step 5
  // Determine if it is possible to launch the datasets thread. The thread can only 
  // can be launched if there aren't any  viewers that need to be rendered
  // AND at the thread isn't already running

  if( !bAnyViewerNeedsRendering && !m_bThreadRunning )
  {

    bool bRunDatasetsThread = true;
    for( int4 i = 0; i < m_cache.size(); ++i )
    {
      if( !m_cycleSettings[ i ]->IsEnabled() )
        continue;

      for( int4 j = 0; j < m_cache[ i ].size(); ++j )
      {
        if( m_cache[ i ][ j ]->m_eState == vxNodeDatasetsCycler::EMPTY )
        {
          bRunDatasetsThread = false;
          break;
        }
      }
      if( !bRunDatasetsThread )
        break;
    }
    
    if( bRunDatasetsThread )
    {
      m_pDatasetsCyclerThread->BeginRendering();
      m_bThreadRunning = true;
    }
  }

  // callback so gui can update current dataset index
  if( m_pIndexChangedFuncPtr && bAnyViewerCycling )
    m_pIndexChangedFuncPtr();
    
} // Run()



// $Log: vxNodeDatasetsCycler.C,v $
// Revision 1.5.2.1  2009/04/07 19:56:53  gdavidson
// Added check for already existing HQ render buffers
//
// Revision 1.5  2007/10/24 20:12:21  gdavidson
// Added vxViewerControl
//
// Revision 1.4  2007/09/19 14:38:26  gdavidson
// Set the vxDataset when beating
//
// Revision 1.3  2007/09/17 18:50:45  geconomos
// removed double buffers from vxCacheBuffer
//
// Revision 1.2  2007/07/02 18:30:11  geconomos
// added "cycles per minute" support
//
// Revision 1.1  2007/06/22 15:15:49  geconomos
// - renamed vxNodeDatasets => vxNodeDatasetsCycler
// - reverted to RSNA version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxNodeDatasetsCycler.C,v 1.5.2.1 2009/04/07 19:56:53 gdavidson Exp $
// $Id: vxNodeDatasetsCycler.C,v 1.5.2.1 2009/04/07 19:56:53 gdavidson Exp $
