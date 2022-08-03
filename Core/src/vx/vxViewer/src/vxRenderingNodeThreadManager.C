// $Id: vxRenderingNodeThreadManager.C,v 1.10.2.2 2010/06/01 19:33:10 dongqing Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

// include declarations
#include "StdAfx.h"
#include "vxRenderingNodeThread.h"
#include "vxRenderBuffer.h"
#include "SystemInfo.h"
#include "Timer.h"
#include "vxRenderingNodePlugin.h"
#include "vxEnvironment.h"
#include "vxRenderingNodeThreadManager.h"
#include "vxVisualizationPreferences.H"


#include "vxRenderingNodePluginCurvetracer.h"

#define FILE_REVISION "$Revision: 1.10.2.2 $"


/**
 * Constructor
 */
vxRenderingNodeThreadManager::vxRenderingNodeThreadManager(): 
 m_bAbortRendering( false ),
 m_renderSize( 128, 128, 0 )
{
	m_pCurrentPlugin = NULL;
	Initialize();
} // vxRenderingNodeThreadManager()


/**
 * Destructor
 */
vxRenderingNodeThreadManager::~vxRenderingNodeThreadManager()
{
  vxRenderingNodeThreads::iterator iter = m_threads.begin();
  for( ;  iter != m_threads.end(); ++iter )
  {
    (*iter)->ExitRendering();
    (*iter)->Wait();    
    delete (*iter);
  }
}//~vxRenderingNodeThreadManager()


/**
 * Initializes the threading manager
 */
void vxRenderingNodeThreadManager::Initialize()
{
  /// allocate threads
  uint4 uAffinityMask( 0x0001 );
  const uint4 uNumberOfThreads = vxVisualizationPreferences::GetMaxRenderThreads();

  for( int i = 0; i < uNumberOfThreads; i++, uAffinityMask <<= 1 )
  {
    vxRenderingNodeThread * pThread = new vxRenderingNodeThread( m_threadArgs );
    pThread->SetId( i );
    pThread->SetPriority( Thread::HIGHEST );
   // pThread->SetAffinity( uAffinityMask );
    pThread->Start();

    m_renderingCompleteHandles.push_back( pThread->GetCompleteEvent() );
    
    m_threads.push_back( pThread );
  }  

  m_iNumberOfRenderThreads = uNumberOfThreads;
} // Initialize()


/**
 * Renders an image using the threads
 *
 * @param   pPlugin rendering node plugin
 */
void vxRenderingNodeThreadManager::Render( vxRenderingNodePlugin * pPlugin )
{
	if ( pPlugin == NULL )
		return;

	// cache the current Plugin Engine for Abort methods
	m_pCurrentPlugin = pPlugin;

	m_bAbortRendering = false;

  if( pPlugin->SupportsAdaptiveRendering() )
  {
    pPlugin->SetRenderSize( m_renderSize );
    pPlugin->GetRenderBuffer().SetRenderSize( m_renderSize );
  }
  
  pPlugin->PreRender();

  if( !pPlugin->SupportsAdaptiveRendering() ) 
  {
    m_renderSize = pPlugin->GetRenderBuffer().GetBufferSize();
    pPlugin->GetRenderBuffer().SetRenderSize( m_renderSize );
  }

  // initialize the shared thread arguments
  m_threadArgs.m_pPlugin = pPlugin;
  m_threadArgs.m_tileSize = pPlugin->GetTileSize();
  m_threadArgs.m_iNumberOfTilesInRow = ceil( m_renderSize.m_x / static_cast< float4 >( m_threadArgs.m_tileSize.m_x ) );
  m_threadArgs.m_iNumberOfRows       = ceil( m_renderSize.m_y / static_cast< float4 >( m_threadArgs.m_tileSize.m_y ) );
  m_threadArgs.m_iNumberOfTiles      = m_threadArgs.m_iNumberOfTilesInRow * m_threadArgs.m_iNumberOfRows;
  m_threadArgs.m_iNextTile           = m_iNumberOfRenderThreads;

  // TODO: support tile count based on thread count
  if( dynamic_cast< vxRenderingNodePluginCurvetracer * >( pPlugin ) )
  {
    m_threadArgs.m_iNumberOfTilesInRow = max( 1, m_iNumberOfRenderThreads / 2 );
    m_threadArgs.m_iNumberOfRows       = m_iNumberOfRenderThreads > 1? 2 : 1;

    uint2 x = m_renderSize.m_x  / m_threadArgs.m_iNumberOfTilesInRow;
    uint2 y = m_renderSize.m_y  / m_threadArgs.m_iNumberOfRows;
    m_threadArgs.m_tileSize = Triple<uint2>(x,y,0);
    m_threadArgs.m_iNumberOfTiles      = m_threadArgs.m_iNumberOfTilesInRow * m_threadArgs.m_iNumberOfRows;
    m_threadArgs.m_iNextTile           = m_iNumberOfRenderThreads;
  }
  
  for( uint4 i = 0; i < m_iNumberOfRenderThreads; ++i )
    m_threads[ i ]->StartRendering( i );

} // Render( vxRenderingNodePlugin * pPlugin )

  
/**
 * Renders and image blocks until finished
 *
 * @param pPlugin rendering node plugin
 */
bool vxRenderingNodeThreadManager::RenderAndWait( vxRenderingNodePlugin * pPlugin, const Thread::ThreadPriorityEnum & ePriority )
{
  for( int i = 0; i < m_threads.size(); ++i )
    m_threads[ i ]->SetPriority( ePriority );

	Timer timer;
  Render( pPlugin );
  Wait();
  pPlugin->GetRenderBuffer().SetRenderTime( timer.ElapsedTime() );
  // TODO: verify other instances of SetRendererThreadCount
  pPlugin->GetRenderBuffer().SetRenderThreadCount( m_iNumberOfRenderThreads );
	
	return m_bAbortRendering;
} // RenderAndWait( vxRenderingNodePlugin * pPlugin )


/**
 * Waits for all rendering threads to complete before returning.
 */
void vxRenderingNodeThreadManager::Wait()
{
  const DWORD dwNumberOfHandles =  m_renderingCompleteHandles.size();

  const HANDLE * pHandles = &m_renderingCompleteHandles[ 0 ];

  WaitForMultipleObjects( dwNumberOfHandles, pHandles, TRUE, INFINITE );

} // WaitForRenderingToComplete()


/**
 * Sets the thread priorities for the engine
 *
 * @param   ePriority     thread priority
 */
void vxRenderingNodeThreadManager::SetThreadPriority( const Thread::ThreadPriorityEnum ePriority )
{
  vxRenderingNodeThreads::iterator iter = m_threads.begin();
  for( ;  iter != m_threads.end(); ++iter )
    (*iter)->SetPriority( ePriority );
} // SetThreadPriority( const Thread::ThreadPriorityEnum ePriority )


/**
 * Aborts the threads from rendering
 */
void vxRenderingNodeThreadManager::AbortRendering()
{
  if( IsRendering() && m_pCurrentPlugin != NULL )
    m_pCurrentPlugin->Abort();
  m_bAbortRendering = true;

} // Abort()


/**
 * Aborts the threads from rendering
 */
void vxRenderingNodeThreadManager::AbortRenderingAndWait()
{
  AbortRendering();
  Wait();
} // AbortRenderingAndWait()


/**
 * Indicates if the threads are busy rendering
 *
 * @return  true if at least one thread is still rendering
 */
bool vxRenderingNodeThreadManager::IsRendering() const
{
  bool bRendering = false;
  
  // TODO: maybe be smarter about using render number?
  vxRenderingNodeThreads::const_iterator iter = m_threads.begin();
  for( ;  iter != m_threads.end(); ++iter )
  {
    if( bRendering = (*iter)->IsRendering() )
      break;
  }    
  return bRendering;
} // IsRendering() const


/**
 * Pauses the rendering engine
 */
void vxRenderingNodeThreadManager::Pause()
{
   // TODO: maybe be smarter about using render number?
 for( int4 i = 0; i < m_threads.size(); ++i )
  {
    if( m_threads[ i ]->IsRendering() )
      m_threads[ i ]->Suspend();
  }
} // Pause()


/**
 * Resumes the rendering engine
 */
void vxRenderingNodeThreadManager::Resume()
{
    // TODO: maybe be smarter about using render number?
 for( int4 i = 0; i < m_threads.size(); ++i )
  {
    if( m_threads[ i ]->IsRendering() )
      m_threads[ i ]->Resume();
  }
} // Resume()


// $Log: vxRenderingNodeThreadManager.C,v $
// Revision 1.10.2.2  2010/06/01 19:33:10  dongqing
// remove the maximum number of thread control capped by number of cores
//
// Revision 1.10.2.1  2008/08/01 22:00:09  jmeade
// new view
//
// Revision 1.10  2008/04/24 21:55:35  jmeade
// Merge from CON_3-0 on 080424
//
// Revision 1.9  2007/08/24 18:05:41  geconomos
// fixed slow rendering of curved images for large image sizes
//
// Revision 1.8  2007/06/29 20:21:27  geconomos
// added rendering tile size
//
// Revision 1.7  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.6  2007/04/12 14:22:29  geconomos
// corrected bad checkin
//
// Revision 1.5  2007/04/12 14:13:41  geconomos
// switched threading to tile-based approach
//
// Revision 1.4  2007/03/22 18:25:46  gdavidson
// Added GetNumberOfRenderThreads
//
// Revision 1.3.2.1  2007/04/10 14:47:02  geconomos
// added mechanism for setting the rendering threads priorities
//
// Revision 1.3  2007/03/05 16:45:16  geconomos
// corrected bad checkin
//
// Revision 1.2  2007/03/05 15:23:28  geconomos
// code review preparation
//
// Revision 1.1  2007/02/05 19:27:53  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.13  2007/01/24 23:08:40  gdavidson
// Rolled back changes (Threading problem)
//
// Revision 1.12  2007/01/24 20:30:27  gdavidson
// Removed setting of the buffer's render size
//
// Revision 1.11  2007/01/19 15:28:50  geconomos
// reorganized visualization preferences
//
// Revision 1.10  2006/12/04 20:49:18  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.9  2006/08/28 14:02:06  geconomos
// moved all frame rate related stuff to vxRenderingNodePlugin
//
// Revision 1.8  2006/08/23 17:54:23  geconomos
// + moved MaintainFramerate() to vxRenderingNode
// + added plugin data synchronization methods
// + changed RenderAndWait() to return if it was aborted
//
// Revision 1.7  2006/08/03 14:13:36  geconomos
// + renamed vxRenderingEngineRenderer to vxRenderingNodePlugin
// + non-square image support in MaintainFrameRate()
//
// Revision 1.6  2006/07/21 15:07:33  geconomos
// initial support for planetracer added
//
// Revision 1.5  2006/07/14 13:38:06  geconomos
// cleaned up RenderWindow method
//
// Revision 1.4  2006/07/07 18:02:05  geconomos
// modified Wait class method to use WaitForMultipleObjects function instead of multiple WaitForSingleObjects
//
// Revision 1.3  2006/07/07 13:41:50  geconomos
// added vxRenderingEngineSettings and vxRenderingEngineBuffers class variables
//
// Revision 1.2  2006/06/27 20:33:24  geconomos
// lowered desired frame rate range
//
// Revision 1.1  2006/06/27 20:11:51  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderingNodeThreadManager.C,v 1.10.2.2 2010/06/01 19:33:10 dongqing Exp $
// $Id: vxRenderingNodeThreadManager.C,v 1.10.2.2 2010/06/01 19:33:10 dongqing Exp $
