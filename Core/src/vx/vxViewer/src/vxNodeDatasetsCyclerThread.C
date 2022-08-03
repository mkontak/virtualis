// $Id: vxNodeDatasetsCyclerThread.C,v 1.6 2007/10/24 20:12:33 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxNodeDatasetsCyclerThread.h"
#include "vxDatasetsInfo.h"
#include "vxViewer.h"
#include "vxRenderingNodeThreadManager.h"
#include "vxRenderingNodePluginRaytracer.h"
#include "SingleLock.h"
#include "CriticalSection.h"
#include "vxNodeRendering.h"
#include "vxNodeDatasetsCycler.h"
#include "vxNodeIDs.h"
#include "vxCycleSettings.h"
#include "vxVisualizationGraph.h"
#include "vxDataset.h"
#include "vxViewerControl.h"
 
// defines
#define FILE_REVISION "$Revision: 1.6 $"


/**
 * Constructor 
 */
vxNodeDatasetsCyclerThread::vxNodeDatasetsCyclerThread() :
  m_pNodeRendering( NULL ),
  m_pNodeDatasets( NULL ),
  m_bAbortRendering( false ),
  m_bIsRendering( false )
{
  m_hRenderViewersEvent     = CreateEvent( NULL, TRUE, FALSE, NULL );
  m_hAbortThreadEvent       = CreateEvent( NULL, TRUE, FALSE, NULL );
  m_hRenderingCompleteEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

} // ( vxControllerGlobalDatasets * pDatasetsController ) :


/**
 * Destructor
 */
vxNodeDatasetsCyclerThread::~vxNodeDatasetsCyclerThread()
{
  // abort thread loop
  SetEvent( m_hAbortThreadEvent );

  // wait for the thread to terminate
  WaitForSingleObject( GetThreadHandle(), INFINITE );

  // free event handles
  CloseHandle( m_hAbortThreadEvent );
  CloseHandle( m_hRenderViewersEvent );
  CloseHandle( m_hRenderingCompleteEvent );

} // ~vxNodeDatasetsCyclerThread()


/**
 * Begins rendering. Adds the specified viewers to the internal list of viewers
 * to be rendered.
 *
 * @param   viewers     viewers to add to rendering
 */
void vxNodeDatasetsCyclerThread::BeginRendering()
{
  // reset control event
  ResetEvent( m_hRenderingCompleteEvent );
  
  /// release the rendering thread
  SetEvent( m_hRenderViewersEvent );

} // BeginRendering( const std::vector< vxViewer * > & viewers )


/**
 * Stops rendering
 */
void vxNodeDatasetsCyclerThread::StopRendering()
{
  // are we rendering?
  if( m_bIsRendering )
  {
    m_bAbortRendering = true;
    m_pNodeRendering->Abort();
    WaitForSingleObject( m_hRenderingCompleteEvent, INFINITE );
  }
} // StopRendering()


/**
 * Thread function responsible for generating the final high-quality image
 */
void vxNodeDatasetsCyclerThread::Run()
{ 
  // TODO: clean this up one day ;)

  // create the handles to wait on
  const DWORD dwNumberOfHandles = 2;
  HANDLE pHandles[] = { m_hRenderViewersEvent, m_hAbortThreadEvent };

  DWORD dwReturn = 0;
  while( ( dwReturn = WaitForMultipleObjects( dwNumberOfHandles, pHandles, false, INFINITE )) >= WAIT_OBJECT_0)
  {
    // figure out the index of the event that got us here
    int iEventIndex = dwReturn - WAIT_OBJECT_0;

    if( iEventIndex == 0 )
    {
      // reset control flags
      ResetEvent( m_hRenderViewersEvent );
      m_bIsRendering = true;
      m_bAbortRendering = false;

      vxNodeDatasetsCycler::vxCacheBuffers & cache = m_pNodeDatasets->GetCache();
    
      for( int4 i = 0; i < cache.size(); ++i )
      {
        vxViewer * pViewer = m_pNodeDatasets->GetVisualizationGraph()->GetViewerControl(i)->GetViewer( vxViewerControl::MainViewer );

        if( !dynamic_cast< vxCycleSettings * >(  pViewer->GetEnvironment().GetElement( vxIDs::CycleSettings ) )->IsEnabled() )
          continue;

        if( !pViewer->GetViewerType()->HasRenderingNodePlugin() )
          continue;
        
        vxRenderingNodePluginRaytracer * pPlugin = dynamic_cast< vxRenderingNodePluginRaytracer * >( &pViewer->GetViewerType()->GetRenderingNodePlugin());
        if( pPlugin == NULL )
          continue;
  
        for( int4 j = 0; j < cache[ i ].size(); ++j )
        {
          vxNodeDatasetsCycler::vxCacheBuffer * pBuffer = cache[ i ][ j ];
          if( pBuffer->m_eState != vxNodeDatasetsCycler::HQ )
          {
            vxDataset & dataset = *vxDatasetsInfo::GetDataset( j );
            pPlugin->GetRaytracer().GetSettings().SetComponents( *dataset.GetComponents() );
            pPlugin->GetRaytracer().GetSettings().SetIntensityVolume( *dataset.GetIntensityVolume());
            pPlugin->GetRaytracer().GetSettings().SetLabelVolume( *dataset.GetLabelVolume() );
            pPlugin->GetRaytracer().GetSettings().UpdateMaskForIntensities( *dataset.GetIntensityVolume(), *dataset.GetColormap() );
            pPlugin->GetRaytracer().GetSettings().UpdateMaskForLabels( *dataset.GetComponents() );
            pPlugin->GetRaytracer().GetSettings().UpdateSpaceLeapVolume();

            m_pNodeRendering->GetThreadManager()->SetRenderSize( pPlugin->GetMaxRenderSize() );

            if( m_bAbortRendering |= m_pNodeRendering->GetThreadManager()->RenderAndWait( pPlugin, Thread::LOWEST ) )
              break;

            // lock when updating the buffer
            {
              SingleLock< CriticalSection > lock( &(pBuffer->m_buffer.GetSyncObject()), true );
              pBuffer->m_buffer = pPlugin->GetRenderBuffer();
            }
          }

          if( m_bAbortRendering )
            break;

        } // for( int4 j = 0; j < cache[ i ].size(); ++j )

        if( m_bAbortRendering )
          break;

        for( int4 j = 0; j < cache[ i ].size(); ++j )
          cache[i][j]->m_eState = vxNodeDatasetsCycler::HQ;

      }

      // indicate that processing has completed
      m_bIsRendering = false;
      SetEvent( m_hRenderingCompleteEvent );
    }
    else // aborted
    {
      break;
    }
  } // WaitForMultipleObjects
} // Run()


// $Log: vxNodeDatasetsCyclerThread.C,v $
// Revision 1.6  2007/10/24 20:12:33  gdavidson
// Added vxViewerControl
//
// Revision 1.5  2007/09/19 17:41:07  geconomos
// updating components in raytracer
//
// Revision 1.4  2007/09/17 18:50:45  geconomos
// removed double buffers from vxCacheBuffer
//
// Revision 1.3  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.2  2007/06/22 19:46:57  geconomos
// Corrected case when thread was aborted all non-generated cached images were marked as high quality
//
// Revision 1.1  2007/06/22 15:15:24  geconomos
// - renamed vxDatasetsThread => vxNodeDatasetsCyclerThread
// - reverted to RSNA version
//
// Revision 1.15  2007/05/29 15:11:44  gdavidson
// Added a CriticalSection
//
// Revision 1.14  2007/05/03 21:30:31  gdavidson
// Refactored vxVisualizationGraph
//
// Revision 1.13  2007/04/18 17:06:53  gdavidson
// Modified the beating of datasets
//
// Revision 1.12  2007/04/02 21:29:59  gdavidson
// Modified switching of the Datasets
//
// Revision 1.11  2007/03/02 17:40:47  geconomos
// code review preparation
//
// Revision 1.10  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.9  2007/02/22 16:07:37  geconomos
// code cleanup
//
// Revision 1.8  2007/02/05 20:17:20  geconomos
// change vxViewerType::GetRenderingEnginePlugin => GetRenderingNodePlugin
//
// Revision 1.7  2007/02/05 19:28:40  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.6  2007/01/08 17:14:31  gdavidson
// Refactored vxVisualizationGraph code
//
// Revision 1.5  2006/12/05 22:13:29  romy
// bug fix
//
// Revision 1.4  2006/12/05 18:25:27  romy
// fixed the Crash in Caridac Viewer. Checks for HasRenderingEngine flag
//
// Revision 1.3  2006/12/04 20:44:25  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.2  2006/09/20 14:28:42  geconomos
// storing the active rendering engine in an instance variable
//
// Revision 1.1  2006/09/14 20:06:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxNodeDatasetsCyclerThread.C,v 1.6 2007/10/24 20:12:33 gdavidson Exp $
// $Id: vxNodeDatasetsCyclerThread.C,v 1.6 2007/10/24 20:12:33 gdavidson Exp $
