// $Id: vxHighQualityThread.C,v 1.17.2.2 2009/03/23 15:42:16 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxHighQualityThread.h"
#include "vxRenderingNodeThreadManager.h"
#include "vxViewer.h"
#include "vxViewerType.h"
#include "vxNodeRendering.h"
#include "vxEnvironment.h"
#include "vxWindowHandle.h"

#define FILE_REVISION "$Revision: 1.17.2.2 $"

/**
 * Constructor 
 *
 * @param   pRenderingManager   vxRenderingNode owning this instance
 */
vxHighQualityThread::vxHighQualityThread( vxNodeRendering * pRenderingNode ) :
  m_pRenderingNode( pRenderingNode ),
  m_bAbortRendering( false ),
  m_bIsRendering( false ),
  m_iCurrentViewerIndex( 0 )
{
  m_hRenderViewersEvent     = CreateEvent( NULL, FALSE, FALSE, NULL );
  m_hAbortThreadEvent       = CreateEvent( NULL, TRUE, FALSE, NULL );
  m_hRenderingCompleteEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
 
} // ( vxRenderingNode * pRenderingNode ) :


/**
 * Destructor
 */
vxHighQualityThread::~vxHighQualityThread()
{
  // abort thread loop
  SetEvent( m_hAbortThreadEvent );
  
  // wait for the thread to terminate
  WaitForSingleObject( GetThreadHandle(), INFINITE );
  
  // free event handles
  CloseHandle( m_hAbortThreadEvent );
  CloseHandle( m_hRenderViewersEvent );
  CloseHandle( m_hRenderingCompleteEvent );

} // ~vxHighQualityThread()


/**
 * Begins rendering. Adds the specified viewers to the internal list of viewers
 * to be rendered.
 *
 * @param   viewers     viewers to add to rendering
 */
void vxHighQualityThread::BeginRendering( const std::vector< vxViewer * > & viewers )
{
  // TODO: investigate std::set()
  
  // the viewer can have 2 levels of rendering and the m_currentViewerIndex is used to indicate the level of quality
  // when m_currentViewerIndex < m_viewers.size() the viewers are rendering intermediate quality 
  // when m_currentViewerIndex > m_viewers.size() the viewers are rendering high quality 
  int4 iIndex = ( m_iCurrentViewerIndex > m_viewers.size() ) ? m_iCurrentViewerIndex - m_viewers.size() : 0;
  std::vector< vxViewer * > tmp;

  // store all viewers that haven't been rendered yet
  for( int i = iIndex; i < m_viewers.size(); ++i )
    tmp.push_back( m_viewers[ i ] );

  // update master list of viewers to be rendered
  m_viewers = tmp;

  // iterate over incoming viewers
  for( int i = 0; i < viewers.size(); ++i )
  {
    bool bPriority = ( &viewers[i]->GetEnvironment() == viewers[i]->GetEnvironment().GetActiveEnvironment() );
    std::vector< vxViewer * >::iterator iter = std::find( m_viewers.begin(), m_viewers.end(), viewers[ i ] );

    // remove the viewer if it is the priority rendering
    if( iter != m_viewers.end() && bPriority )
      m_viewers.erase( iter );

    // insert priority rendering at beginning or add non-priority to the end of the queue
    if( bPriority )
      m_viewers.insert( m_viewers.begin(), viewers[i] );
    else if( iter == m_viewers.end() )
      m_viewers.push_back( viewers[ i ] );
  }

  // reset control event
  ResetEvent( m_hRenderingCompleteEvent );
  m_bIsRendering = true;
  m_bAbortRendering = false;
  
  // release the rendering thread
  PulseEvent( m_hRenderViewersEvent );
} // BeginRendering( const std::vector< vxViewer * > & viewers )


/**
 * Stops rendering
 */
void vxHighQualityThread::StopRendering()
{
  try
  {
    // are we rendering?
    if( m_bIsRendering )
    {
      m_bAbortRendering = true;
	    m_pRenderingNode->GetThreadManager()->AbortRenderingAndWait();
      WaitForSingleObject( m_hRenderingCompleteEvent, INFINITE );
    }
  }
  catch( ... )
  {
    LogDbg( "+++ UNHANDLED EXCEPTION CAUGHT +++", "vxHighQualityThread", "StopRendering" );
  }
} // StopRendering()


/**
 * Thread function responsible for generating the final high-quality image
 */
void vxHighQualityThread::Run()
{ 
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
      try
      {
        // two possible levels of rendering
        // when m_currentViewerIndex < m_viewers.size() the viewers are rendering intermediate quality 
        // when m_currentViewerIndex > m_viewers.size() the viewers are rendering high quality 
        for( m_iCurrentViewerIndex = 0; m_iCurrentViewerIndex < m_viewers.size() * 2; ++m_iCurrentViewerIndex )
        {
          const int4 iIndex = m_iCurrentViewerIndex % m_viewers.size();
          vxViewer * pViewer = m_viewers[ iIndex ];

          vxRenderingNodePlugin & renderingEnginePlugin = pViewer->GetViewerType()->GetRenderingNodePlugin();
          const Triple< uint4 > & maxSize = renderingEnginePlugin.GetMaxRenderSize();
          Triple< uint4 > size = renderingEnginePlugin.GetRenderBuffer().GetRenderSize();
          vxRenderBuffer::BufferStatEnum eBufferState = vxRenderBuffer::EMPTY;

          // only render HQ if window is larger than 110% of the currently rendered image
          if( pViewer->GetWinRect().GetViewportWidth() <= ( size.m_x + ( size.m_x >> 3 )) || pViewer->GetWinRect().GetViewportHeight() <= ( size.m_y + ( size.m_y >> 3 )))
          {
            const_cast< vxRenderBuffer &>( pViewer->GetEnvironment().GetRenderBuffer() ).SetState( vxRenderBuffer::HIGH );
          }
          else
          {
            // render at half max size
            if( m_iCurrentViewerIndex < m_viewers.size() )
            {
              if( size.m_x < ( maxSize.m_x / 2 ) && size.m_y < ( maxSize.m_y / 2 ))
              {
                size.m_x = maxSize.m_x / 2;
                size.m_y = maxSize.m_y / 2;
                eBufferState = vxRenderBuffer::INTERMEDIATE;
              }
              else
                continue;
            }
            else if( size.m_x == maxSize.m_x || size.m_y == maxSize.m_y ) // avoid additional rendering
              continue;
            else
            {
              // render at max size
              size = maxSize;
              eBufferState = vxRenderBuffer::HIGH;
            }
            
            if( m_bAbortRendering |= m_pRenderingNode->RenderViewer( pViewer, Triple< uint4 >( size.m_x, size.m_y, 0 ), eBufferState, Thread::LOWEST ))   
              break;
          }

          m_pRenderingNode->RedrawViewer( pViewer );
        }

        if( !m_bAbortRendering )
        {
          m_viewers.clear();
          m_iCurrentViewerIndex = 0;
        }
      }
      catch( ... )
      {
        LogDbg( "+++ UNHANDLED EXCEPTION CAUGHT +++", "vxHighQualityThread", "Run" );
      }

      // indicate that processing has completed
      SetEvent( m_hRenderingCompleteEvent );
      m_bIsRendering = false;

    }
    else // aborted
    {
      break;
    }
  } // WaitForMultipleObjects
} // Run()


// $Log: vxHighQualityThread.C,v $
// Revision 1.17.2.2  2009/03/23 15:42:16  gdavidson
// Added code to prevent renderering higher than the window can display
//
// Revision 1.17.2.1  2008/11/11 20:39:16  gdavidson
// Issue #6170: Position active viewer at beginning of rendering queue
//
// Revision 1.17  2007/06/29 19:20:20  gdavidson
// Corrected merge error
//
// Revision 1.16  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.15  2007/05/03 21:33:29  gdavidson
// Changed vxRenderingNode -> vxNodeRendering
//
// Revision 1.14.2.3  2007/04/10 14:47:02  geconomos
// added mechanism for setting the rendering threads priorities
//
// Revision 1.14.2.2  2007/03/27 21:54:33  gdavidson
// Rolled back
//
// Revision 1.14.2.1  2007/03/22 21:33:13  gdavidson
// Issue #5505: Modified the render loop
//
// Revision 1.14  2007/02/05 20:17:20  geconomos
// change vxViewerType::GetRenderingEnginePlugin => GetRenderingNodePlugin
//
// Revision 1.13  2007/02/05 19:28:40  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.12  2007/01/25 22:44:11  gdavidson
// Added an intermediate render level
//
// Revision 1.11  2007/01/25 18:33:05  gdavidson
// Fixed two level high quality rendering
//
// Revision 1.10  2007/01/24 23:08:40  gdavidson
// Rolled back changes (Threading problem)
//
// Revision 1.9  2007/01/24 20:31:25  gdavidson
// Fixed bug which halted high quality when the user interacts
//
// Revision 1.8  2007/01/23 21:43:19  gdavidson
// Added two levels of render quality
//
// Revision 1.7  2006/12/04 20:45:47  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.6  2006/10/11 18:29:58  geconomos
// issue #5045:  one or more viewers do not update to the final high quality image
//
// Revision 1.5  2006/10/05 19:42:17  geconomos
// added try/catch block around rendering to silently log unhandled exceptions
//
// Revision 1.4  2006/09/28 18:30:06  geconomos
// Issue 4963: added indicator for non-optimal image quality
//
// Revision 1.3  2006/09/15 18:36:23  geconomos
// added "try/catch" to StopRendering
//
// Revision 1.2  2006/08/28 14:02:18  geconomos
// moved all frame rate related stuff to vxRenderingNodePlugin
//
// Revision 1.1  2006/08/23 17:52:20  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxHighQualityThread.C,v 1.17.2.2 2009/03/23 15:42:16 gdavidson Exp $
// $Id: vxHighQualityThread.C,v 1.17.2.2 2009/03/23 15:42:16 gdavidson Exp $
