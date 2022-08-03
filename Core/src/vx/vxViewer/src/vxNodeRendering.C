// $Id: vxNodeRendering.C,v 1.6.2.4 2009/03/23 15:42:16 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxViewer.h"
#include "vxEnvironment.h"
#include "vxRenderingNodeThreadManager.h"
#include "vxWindowHandle.h"
#include "vxVisualizationPreferences.h"
#include "vxHighQualityThread.h"
#include "vxElementInt4.h"
#include "SingleLock.h"
#include "vxRenderingNodePlugin.h"
#include "vxVisualizationGraph.h"
#include "vxVisualizationGraphNode.h"
#include "vxNodeRendering.h"
#include "vxViewerControl.h"


#define FILE_REVISION "$Revision: 1.6.2.4 $"


// registered redraw message
UINT vxNodeRendering::VX_REDRAW = ::RegisterWindowMessage( "VX_REDRAW" ); 

/**
 * Constructor
 * 
 * @param   
 */
vxNodeRendering::vxNodeRendering() :
  m_bPreviousUserInteractionFlag( false ),
  m_bHighQualityThreadEnabled( vxVisualizationPreferences::GetHighQualityEnabled() ),
  m_bFixedSizeRenderingEnabled( false ),
  m_fixedRenderingSize( 512, 512, 0 )
{
  // thread manager control rendering threads
	m_pThreadManager = new vxRenderingNodeThreadManager();
  
  // start the final image generation thread
  m_pHighQualityThread = new vxHighQualityThread( this );
  m_pHighQualityThread->Start();
  m_pHighQualityThread->SetPriority( Thread::LOWER );

  // rednering used when multiple viewers need to render
  m_renderingWieghts.push_back( 1.00F );
  m_renderingWieghts.push_back( 0.75F );
  m_renderingWieghts.push_back( 0.50F );
} // vxRenderingNode()


/**
 * Destructor
 */
vxNodeRendering::~vxNodeRendering()
{
	delete m_pHighQualityThread;
	m_pHighQualityThread = NULL;

	delete m_pThreadManager;
	m_pThreadManager = NULL;
} // ~vxRenderingNode()


/**
 * Initializes the rendering manager with the specified viewers
 *
 * @param   viewers     viewers to associate with manager
 */
void vxNodeRendering::Initialize()
{
  for( int4 i = 0; i < GetVisualizationGraph()->GetViewerControls().size(); ++i )
  {
    vxViewer * pViewer = GetVisualizationGraph()->GetViewerControl(i)->GetViewer( vxViewerControl::MainViewer );

    vxRenderingInfo * pInfo = new vxRenderingInfo();
    pInfo->m_pViewer = pViewer;
    pInfo->m_fRenderTime = 0.0F;
    pInfo->m_bNeedsRendering = false;
    pInfo->m_pViewerType = NULL;
    pInfo->m_bInitialRendering = true;
    pInfo->m_renderSize = pViewer->GetViewerType()->GetRenderingNodePlugin().GetInitialRenderSize();

    m_renderings.push_back( pInfo );
  }
} // Initialize( std::list< vxViewer * > & viewers )


/**
 * Resets the rendering manager to a default state
 */
void vxNodeRendering::Reset() 
{ 
  if( m_pThreadManager->IsRendering() )
    m_pHighQualityThread->StopRendering();
 
  for( int4 i = 0; i < m_renderings.size(); ++i )
    delete m_renderings[ i ];
  m_renderings.clear(); 
  
  // clear the viewers list from the HighQualityThread object.
  m_pHighQualityThread->Reset();

} // Reset() 


/**
 * Aborts the rendering manager
 */
void vxNodeRendering::Abort() 
{ 
  if( m_pHighQualityThread->IsRendering() )
    m_pHighQualityThread->StopRendering();
} // Abort() 


/** 
 * Runs the rendering manager
 */
void vxNodeRendering::Run()
{
  //////////////////
  // dummy check
  
  if( m_renderings.size() == 0 )
    return;


  //////////////////////////////////////////
  // figure out who needs to render
  
  uint4 uNeedsRenderingCount = 0;
  for( int4 i = 0; i < m_renderings.size(); ++i )
  {
    m_renderings[ i ]->m_bNeedsRendering = false;
    
    // get viewer 
    vxViewer * pViewer = m_renderings[ i ]->m_pViewer;

    // set the viewer type in the environment to be the main viewer type
    pViewer->GetEnvironment().SetViewerType( pViewer->GetViewerType() );

    // grab the rendering engine associated with the viewer
    if( pViewer->GetViewerType()->HasRenderingNodePlugin() )
    {
      vxRenderingNodePlugin & plugin = pViewer->GetViewerType()->GetRenderingNodePlugin();

			m_renderings[ i ]->m_bNeedsRendering = plugin.IsRenderingNecessary();
			
      // increment uNeedsRenderingCount for anyone that needs rendering
      if( m_renderings[ i ]->m_bNeedsRendering )
        uNeedsRenderingCount++;

      // did the viewer type change for this viewer, if so reset it so a low quality image will be generated
      if( m_renderings[ i ]->m_pViewerType != pViewer->GetViewerType() )
      {
        m_renderings[ i ]->m_fRenderTime = (std::numeric_limits< float4>::max)();
        m_renderings[ i ]->m_pViewerType = pViewer->GetViewerType();
      }
    }
  } // for( int4 i = 0; i < m_renderings.size(); ++i )
  
  

  ////////////////////////////////////////////
  // compute rendering weights
  
  float4 fActiveViewerWeight = 1.0F;
  float4 fSecondaryViewerWeight = 1.0F;
  if( uNeedsRenderingCount > 0 )
  {
    fActiveViewerWeight = m_renderingWieghts[ min( uNeedsRenderingCount - 1, m_renderingWieghts.size() -1 ) ];

    if( uNeedsRenderingCount > 1 )
      fSecondaryViewerWeight = ( 1.0F - fActiveViewerWeight ) / ( uNeedsRenderingCount - 1 );
  }

  const bool bUserInteracting = vxEnvironment::GetUserInteracting();
  
  /////////////////////////////////////////////////////////////////////////
  // stop the high quality thread if we need to render at least one
  
  if(( uNeedsRenderingCount > 0 && m_bHighQualityThreadEnabled ) || bUserInteracting)
    m_pHighQualityThread->StopRendering();

  /////////////////////////////
  // perform rendering

  // to contain viewers that are to be sent off to the HQ thread
  std::vector< vxViewer * > viewers;

  for( int i = 0; i < m_renderings.size(); ++i )
  {
    // get viewer 
    vxViewer * pViewer = m_renderings[ i ]->m_pViewer;

    // is rendering necessary?
    if( m_renderings[ i ]->m_bNeedsRendering )
    {
      if( pViewer->GetViewerType()->HasRenderingNodePlugin() == false )
        continue;

      // update the engine plugin's environment
      vxRenderingNodePlugin & plugin = pViewer->GetViewerType()->GetRenderingNodePlugin();
      plugin.UpdateFromEnvironment();

      // set the last render size and time for low quality
      Triple< uint4 > size = plugin.GetRenderBuffer().GetRenderSize();

      if( plugin.SupportsAdaptiveRendering() )
      {
        // is this viewer the active one?
        bool bActiveViewer = pViewer->GetEnvironment().GetActiveEnvironment() == & pViewer->GetEnvironment();

        if( !m_bFixedSizeRenderingEnabled )
        {
          // if only one rendering use engine's framerates; otherwise use the global framerate
					float4 fMinFrameRate = plugin.GetMinFramerate();
          float4 fMaxFrameRate = plugin.GetMaxFramerate();

          // adjust the framerates using the found weights
          fMinFrameRate /= bActiveViewer? fActiveViewerWeight : fSecondaryViewerWeight;
          fMaxFrameRate /= bActiveViewer? fActiveViewerWeight : fSecondaryViewerWeight;

          // adjust the framerate by the quality scale
          const float4 fQualityScaleFactor = pViewer->GetEnvironment().GetRenderingMode()->GetQualityScaleFactor();
          fMinFrameRate *= fQualityScaleFactor;
          fMaxFrameRate *= fQualityScaleFactor;

          // compute adjusted rendering size
          size = AdjustRenderSize( m_renderings[ i ], fMinFrameRate, fMaxFrameRate );
        }
        else
        {
          size = m_fixedRenderingSize;
        }
      } // if( m_renderings[ i ]->m_bNeedsRendering )
     
      
      // we need to generate a high quality only if the current render size is less than viewport size and the maximum allowed
      bool bGenerateHighQuality = ( pViewer->GetWinRect().GetViewportWidth() > size.m_x && pViewer->GetWinRect().GetViewportHeight() > size.m_y )
                               && ( size.m_x < plugin.GetMaxRenderSize().m_x && size.m_y < plugin.GetMaxRenderSize().m_y );

      // set the buffer state
      vxRenderBuffer::BufferStatEnum eBufferState = 
        ( !bGenerateHighQuality || m_bFixedSizeRenderingEnabled || !plugin.SupportsAdaptiveRendering() ) 
        ? vxRenderBuffer::HIGH : vxRenderBuffer::LOW;

      // now render
      RenderViewer( pViewer, size, eBufferState, Thread::HIGHEST );

      // update statistics
			m_renderings[ i ]->m_renderSize  = size;
      m_renderings[ i ]->m_fRenderTime = plugin.GetRenderBuffer().GetRenderTime();

      // if engine supports adaptive rendering and user is NOT interacting add to HQ thread
      if( plugin.SupportsAdaptiveRendering() && !bUserInteracting && !m_bFixedSizeRenderingEnabled && bGenerateHighQuality )
        viewers.push_back( pViewer );
    
    } // if( m_renderings[ i ]->m_bNeedsRendering )
    else
    {
      if( pViewer->GetViewerType()->HasRenderingNodePlugin() &&  
          pViewer->GetViewerType()->GetRenderingNodePlugin().SupportsAdaptiveRendering()  )
      {
        // check if high quality is necessary
        vxRenderingNodePlugin & plugin = pViewer->GetViewerType()->GetRenderingNodePlugin();
        const Triple< uint4 > & size = plugin.GetRenderBuffer().GetRenderSize();
        const bool bGenerateHighQuality = ( size.m_x < plugin.GetMaxRenderSize().m_x &&
                                            size.m_y < plugin.GetMaxRenderSize().m_y );

        if( !pViewer->GetEnvironment().GetUserInteracting() && m_bPreviousUserInteractionFlag && bGenerateHighQuality )
          viewers.push_back( pViewer );
      }
    }
  } // for( int i = 0; i < m_viewers.size(); ++i )

  // store the user interaction flag
  m_bPreviousUserInteractionFlag = bUserInteracting;
  
  ///////////////////////////////////////
  // start HQ thread

  bool bStartHQ = !m_pHighQualityThread->IsRendering() && ( m_pHighQualityThread->HasUnfinishedRendering() || ( viewers.size() > 0 )); 
  if( m_bHighQualityThreadEnabled && !m_bFixedSizeRenderingEnabled && !bUserInteracting && bStartHQ )
    m_pHighQualityThread->BeginRendering( viewers );

} // Run()


/**
 * Adjusts the image size in order to keep framerate bewteen min and max.
 *
 * @param   pInfo           current rendering info
 * @param   fMinFramerate   minimum allowable framerate
 * @param   fMaxFramerate   maximum allowable framerate
 * @return  adjusted image size
 */
Triple< uint4 > vxNodeRendering::AdjustRenderSize( vxRenderingInfo * pInfo, float4 fMinFrameRate, float4 fMaxFrameRate )
{
  const int4 iRenderSizeGranularity = 16;        

  vxViewer * pViewer = pInfo->m_pViewer;

  // first time through use the initial render size
  Triple< uint4 > adjustedRenderSize = pInfo->m_renderSize;
  if( pInfo->m_bInitialRendering )
  {
    pInfo->m_bInitialRendering = false;
    return pViewer->GetViewerType()->GetRenderingNodePlugin().GetInitialRenderSize();
  }

  //  simulate square image
  const int4 iCurrentRenderSize = sqrt( static_cast< float4 >( adjustedRenderSize.m_x * adjustedRenderSize.m_y ) );
  
  // get current fps based on the last render time  
  float4 fCurrentFps = ( pInfo->m_fRenderTime > 0.0F ) ? 1.0F / pInfo->m_fRenderTime : 0.0F;
        
  
  const vxRenderingNodePlugin & plugin = pViewer->GetViewerType()->GetRenderingNodePlugin();
     
  // are rendering too slow?
  int4 iDesiredSize( iCurrentRenderSize );
  if( fCurrentFps < fMinFrameRate )
  {
    iDesiredSize = sqrt( fCurrentFps / fMinFrameRate ) * iCurrentRenderSize;  
		iDesiredSize = max( plugin.GetMinRenderSize().m_x, iDesiredSize );
  }  
  
  // are rendering too fast?
  else if( fCurrentFps > fMaxFrameRate ) 
  {
    iDesiredSize = sqrt( fCurrentFps / fMaxFrameRate ) * iCurrentRenderSize;  
    iDesiredSize = min( plugin.GetMaxRenderSize().m_x, iDesiredSize );
  }

  // should we try and squeeze out some more quality?
  else
  {
    iDesiredSize = iCurrentRenderSize + iRenderSizeGranularity + 1;  
    iDesiredSize = min( plugin.GetMaxRenderSize().m_x, iDesiredSize );
  }

  std::pair<uint2,uint2> size = pViewer->GetEnvironment().GetWinRect().GetWindowSize();
    
  // convert to actual aspect ratio image
  const float4 fAspectRatio = static_cast<float4>(size.second) / size.first;
  const int4 iDesiredSizeX = iDesiredSize *sqrt( 1.0F / fAspectRatio ) + 0.5F;
  const int4 iDesiredSizeY = iDesiredSizeX * fAspectRatio + 0.5F;

  if( iDesiredSizeX != adjustedRenderSize.m_x || iDesiredSizeY != adjustedRenderSize.m_y  )
  {
		const uint4 uNewSizeX = min( ( iDesiredSizeX / iRenderSizeGranularity ) * iRenderSizeGranularity, plugin.GetMaxRenderSize().m_x );
    const uint4 uNewSizeY = min( ( iDesiredSizeY / iRenderSizeGranularity ) * iRenderSizeGranularity, plugin.GetMaxRenderSize().m_y );
    adjustedRenderSize.SetXYZ( uNewSizeX, uNewSizeY, 0 );
  }
  return adjustedRenderSize;
} // AdjustRenderSize( vxRenderingInfo * pInfo, float4 fMinFrameRate, float4 fMaxFrameRate )


/**
 * Renders the specifed viewer
 *
 * @param   pViewer         viewer to render
 * @param   renderSize      render size
 * @param   eBufferState    buffer state
 * @return  true if aborted
 */
bool vxNodeRendering::RenderViewer( vxViewer * pViewer, const Triple< uint4 > & renderSize, vxRenderBuffer::BufferStatEnum eBufferState, const Thread::ThreadPriorityEnum & ePriority )
{
  const Triple< uint4 > & originalSize = pViewer->GetEnvironment().GetRenderBuffer().GetRenderSize();
  m_pThreadManager->SetRenderSize( renderSize );

	vxRenderingNodePlugin & plugin = pViewer->GetViewerType()->GetRenderingNodePlugin();
  
  // Mark KOntak
  //   const bool bAborted = m_pThreadManager->RenderAndWait( &plugin );

  const bool bAborted = m_pThreadManager->RenderAndWait( &plugin , ePriority );
  if( bAborted )
  {
    plugin.GetRenderBuffer().SetRenderSize( originalSize );
  }
  else
  {
    SingleLock<CriticalSection> lock(&pViewer->GetEnvironment().GetRenderBuffer().GetSyncObject(), true );
    
    pViewer->GetEnvironment().SetRenderBuffer( plugin.GetRenderBuffer() );
	  
    const_cast< vxRenderBuffer &>( pViewer->GetEnvironment().GetRenderBuffer() ).SetState( eBufferState );
    
    pViewer->GetEnvironment().GetRenderBuffer().SetModified( true );
  }

  return bAborted;
} // RenderViewer( vxViewer * pViewer, const Triple< uint4 > & renderSize, vxRenderBuffer::BufferStatEnum eBufferState )


/**
 * Sends a VX_REDRAW message to the specified viewer
 *
 * @param   pViewer   viewer to redraw
 */
void vxNodeRendering::RedrawViewer( vxViewer * pViewer )
{
  // get associated HWND handle for viewer
  HWND hWnd = 
    dynamic_cast< vxWindowHandle * >( pViewer->GetEnvironment().GetElement( vxIDs::WindowHandle ) )->GetHandle();

  PostMessage( hWnd, VX_REDRAW, 0, 0 );
} // RedrawViewer( vxViewer * pViewer )


// $Log: vxNodeRendering.C,v $
// Revision 1.6.2.4  2009/03/23 15:42:16  gdavidson
// Added code to prevent renderering higher than the window can display
//
// Revision 1.6.2.3  2009/03/06 16:12:21  gdavidson
// Corrected problem that prevented all viewers from reaching HQ
//
// Revision 1.6.2.2  2008/11/12 17:55:32  gdavidson
// Allow HQ thread to execute, even if no viewers have been changed.
// Allows HQ thread to empty its queue.
//
// Revision 1.6.2.1  2008/11/11 20:38:05  gdavidson
// Issue #6170: Added check to finish rendering viewers in HQ queue
//
// Revision 1.6  2008/04/09 22:00:20  gdavidson
// In the first call to AdjustRenderSize, return the plugin's initial render size
//
// Revision 1.5  2007/10/24 20:13:58  gdavidson
// Added vxViewerControl
//
// Revision 1.4  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.3  2007/05/29 19:52:24  gdavidson
// Stop HQ rendering if the user interacting flag is set
//
// Revision 1.2  2007/05/07 21:33:54  gdavidson
// Don't start HQ thread if no viewers require it
//
// Revision 1.1  2007/05/03 21:42:49  gdavidson
// Initial revision
//
// Revision 1.4  2007/04/02 21:28:56  gdavidson
// Added IsModified to the vxRenderBuffer
//
// Revision 1.3  2007/03/05 15:23:27  geconomos
// code review preparation
//
// Revision 1.2  2007/02/05 20:17:20  geconomos
// change vxViewerType::GetRenderingEnginePlugin => GetRenderingNodePlugin
//
// Revision 1.1  2007/02/05 19:27:52  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.37  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.36  2007/01/25 22:43:14  gdavidson
// Fixed the initial rendering size
//
// Revision 1.35  2007/01/25 18:33:05  gdavidson
// Fixed two level high quality rendering
//
// Revision 1.34  2007/01/24 23:08:40  gdavidson
// Rolled back changes (Threading problem)
//
// Revision 1.33  2007/01/24 20:29:18  gdavidson
// Used the initial redering size when computing the size
//
// Revision 1.32  2007/01/23 21:44:25  gdavidson
// Removed unnecessary high quality generation
//
// Revision 1.31  2007/01/18 21:48:17  gdavidson
// Removed clear of Environment's modified bits
//
// Revision 1.30  2007/01/17 22:09:19  gdavidson
// Issue #5303: Changed SendMessage to PostMessage
//
// Revision 1.29  2007/01/17 16:42:19  geconomos
// moved DrawStatistic to vxVisualizationGraph
//
// Revision 1.28  2007/01/16 17:01:37  gdavidson
// Changed redraw call from PostMessage to SendMessage
//
// Revision 1.27  2007/01/08 17:05:12  gdavidson
// Refactored vxVisualizationGraph code
//
// Revision 1.26  2007/01/05 18:53:52  geconomos
// Issue 5250: Application becomes sluggish after switching views
//
// Revision 1.25  2006/12/12 21:22:14  romy
// Cancels the Rendering threads during the Viewe Change
//
// Revision 1.24  2006/12/12 20:50:47  romy
// Reset method modified to fix issue #5232
//
// Revision 1.23  2006/12/11 15:51:20  geconomos
// reworked  quality scale factor to be comptaible with new rendering manager
//
// Revision 1.22  2006/12/08 19:03:04  romy
// The check modified for "increment uNeedsRenderingCount only if imagegeneration is costly"
//
// Revision 1.21  2006/12/06 15:28:30  geconomos
// corrected problem with setting render buffer state in RenderViewer()
//
// Revision 1.20  2006/12/05 18:25:51  romy
// fixed the Crash in Caridac Viewer. Checks for HasRenderingEngine flag
//
// Revision 1.19  2006/12/04 20:54:29  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.18  2006/10/12 14:57:33  geconomos
// + getting viewer type from vxViewer
// + setting atcive viewer type to main viewer when rendering
// + sanity checks for null rendering engines
//
// Revision 1.17  2006/10/11 18:29:58  geconomos
// issue #5045:  one or more viewers do not update to the final high quality image
//
// Revision 1.16  2006/10/05 20:18:09  geconomos
// proper handling of render buffer state if "fixed size rendering" is enabled
//
// Revision 1.15  2006/10/05 19:44:29  geconomos
// added sychronization for access to render buffer
//
// Revision 1.14  2006/10/04 14:35:55  geconomos
// Issue # 5033: Default the "last render time" to max_float to ensure a low quality image is generated when when the viewer type changes for given viewer
//
// Revision 1.13  2006/09/28 18:28:56  geconomos
// added method to dislpay rendering manager statistics given a vxEnvironment
//
// Revision 1.12  2006/09/27 16:08:14  geconomos
// added support for fixed size rendering
//
// Revision 1.11  2006/09/25 14:26:04  geconomos
// always calling plugin's UpdateFromEnvironment during run
//
// Revision 1.10  2006/09/21 15:03:49  geconomos
// changed spot where the rendering engine updates from the environment
//
// Revision 1.9  2006/09/20 14:38:11  geconomos
// + removed ProcessLocalControllers
// + moved UpdateFromEnvironment (always called )
//
// Revision 1.8  2006/09/15 14:27:25  geconomos
// Changed SendMessage to PostMessage to force a viewer to redraw
//
// Revision 1.7  2006/09/14 20:11:39  geconomos
// removed local controller processing
//
// Revision 1.6  2006/09/01 13:40:34  geconomos
// clearing viewer collection in Reset()
//
// Revision 1.5  2006/08/31 14:13:14  geconomos
// +moved vxRenderingInfo as nested class
// + added viewers accessor
//
// Revision 1.4  2006/08/28 21:03:47  geconomos
// added ability to disable the high quality image thread
//
// Revision 1.3  2006/08/28 14:02:35  geconomos
// moved all frame rate related stuff to vxRenderingNodePlugin
//
// Revision 1.2  2006/08/23 17:58:21  geconomos
// + initial implementation of high quality thread
// + refactored rendering and drawing code
//
// Revision 1.1  2006/08/03 13:58:32  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxNodeRendering.C,v 1.6.2.4 2009/03/23 15:42:16 gdavidson Exp $
// $Id: vxNodeRendering.C,v 1.6.2.4 2009/03/23 15:42:16 gdavidson Exp $
