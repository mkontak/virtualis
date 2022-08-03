// $Id: vxRenderingNodeThread.C,v 1.4 2007/05/07 21:34:53 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRenderingNodeThread.h"
#include "vxRenderingNodeThreadArgs.h"
#include "vxRenderer.h"
#include "vxRenderingNodePlugin.h"
#include "vxRenderBuffer.h"

// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Constructor
 */
vxRenderingNodeThread::vxRenderingNodeThread( vxRenderingNodeThreadArgs & args ) :
  m_uThreadID( 0 ),
  m_bIsRendering(false),
  m_args( args ),
  m_iInitialTileIndex( 0 )
{
  CreateEvents();
} // constructor


///**
// * Constructor
// * @param parent renderer
// * @param thread id
// */
//vxRenderingNodeThread::vxRenderingNodeThread( vxRenderingNodePlugin * pPlugin, uint2 uThreadID )
//{
//  m_pPlugin = pPlugin;
//  m_uThreadID = uThreadID;
//  m_bIsRendering = false;
//  CreateEvents();
//} // constructor


/**
 * Destructor
 */
vxRenderingNodeThread::~vxRenderingNodeThread()
{
  ExitRendering();
  CloseHandle(m_hRenderEvent);
  CloseHandle(m_hKillEvent);
  CloseHandle(m_hCompleteEvent);
} // destructor


/**
 * Creates the thread synchronization objects
 */
void vxRenderingNodeThread::CreateEvents()
{
  m_hRenderEvent   = CreateEvent( NULL, true,  false, NULL );
  m_hKillEvent     = CreateEvent( NULL, false, false, NULL );
  m_hCompleteEvent = CreateEvent( NULL, true,  true, NULL );
} // CreateEvents()


/**
 * Pulses the wait event to render a frame
 */
void vxRenderingNodeThread::StartRendering( int4 iInitialTileIndex )
{
  m_iInitialTileIndex = iInitialTileIndex;
  m_bIsRendering = true;
  ResetEvent( m_hCompleteEvent );
  SetEvent( m_hRenderEvent );
} // StartRendering()


/**
 * Sets the kill event to exit the run loop
 */
void vxRenderingNodeThread::ExitRendering()
{
  SetEvent( m_hKillEvent);
  SetEvent( m_hCompleteEvent );
} // ExitRendering()


/**
 * Waits for the frame to be rendered
 * @param dwTimeout time(milliseconds) to wait for event to be signaled.
 */
void vxRenderingNodeThread::WaitForRenderingToComplete( DWORD dwTimeout )
{
  WaitForSingleObject( m_hCompleteEvent, dwTimeout );
} // WaitForRenderingToComplete( DWORD dwTimeout )


/**
 * Calls the rendering method with the given window
 */
void vxRenderingNodeThread::Run() 
{
  try
  {
    const DWORD dwNumberOfHandles = 2;

    // create the handles to wait on
    HANDLE pHandles[] = { m_hRenderEvent, m_hKillEvent };

    DWORD dwReturn = 0;
    while( ( dwReturn = WaitForMultipleObjects( dwNumberOfHandles, pHandles, false, INFINITE ) ) >= WAIT_OBJECT_0 )
    {
      // figure out the index of the event that got us here
      int iEventIndex = dwReturn - WAIT_OBJECT_0;

      // should we render another frame?
      if( iEventIndex == 0 )
      {
        ResetEvent( m_hRenderEvent );

        try
        {
          int4 iCurrentTile = m_iInitialTileIndex;
          int4 iTotalIterations = 0;
          do
          {
            const Triple<uint4> & imageSize = m_args.m_pPlugin->GetRenderBuffer().GetRenderSize();

            const uint2 xmin = ( iCurrentTile % m_args.m_iNumberOfTilesInRow ) * m_args.m_tileSize.m_x;
            const uint2 ymin = ( iCurrentTile / m_args.m_iNumberOfTilesInRow ) * m_args.m_tileSize.m_y;
            
            const uint2 xmax = min( imageSize.m_x, xmin + m_args.m_tileSize.m_x );
            const uint2 ymax = min( imageSize.m_y - 2, ymin + m_args.m_tileSize.m_y );
            
            BoundingRect< uint2 > window( xmin, ymin, xmax, ymax );  
           
            m_args.m_pPlugin->RenderWindow( window, m_uThreadID ); 
            
            static CriticalSection cs;
            {
              SingleLock< CriticalSection > lock( &cs, true );      
              
              iCurrentTile = (m_args.m_iNextTile)++;
            }
            iTotalIterations++;
          } while( iCurrentTile < m_args.m_iNumberOfTiles );
          
          m_args.m_pPlugin->GetRenderBuffer().GetThreadingStatistics()[ m_uThreadID ] 
            = static_cast< float4 >( iTotalIterations ) / m_args.m_iNumberOfTiles;
        }
        catch( ... )
        {
          LogWrn("thread did not finish rendering the window due to exception, will continue anyway", "vxRenderingNodeThread", "Run");
        }
        m_bIsRendering = false;

        SetEvent( m_hCompleteEvent );
      }
      else if( iEventIndex == 1 )
      {
        break; // outta here
      }
    } // WaitForMultipleObjects()
  }
  catch (...)
  {
    throw ex::VException(LogRec("thread terminated to early","vxRenderingNodeThread","Run"));
  }
} // Run()


// $Log: vxRenderingNodeThread.C,v $
// Revision 1.4  2007/05/07 21:34:53  gdavidson
// Modified the calculation for the maximum Y coordinate of the bounding box
//
// Revision 1.3  2007/05/03 21:32:41  gdavidson
// Changed the initial state of the complete event
//
// Revision 1.2  2007/04/12 14:13:41  geconomos
// switched threading to tile-based approach
//
// Revision 1.1  2007/02/05 19:27:53  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.3  2006/08/03 14:12:01  geconomos
// renamed vxRenderingEngineRenderer to vxRenderingNodePlugin
//
// Revision 1.2  2006/07/07 13:38:33  geconomos
// changed "complete" and "render" events to be manual reset
//
// Revision 1.1  2006/06/27 20:11:51  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderingNodeThread.C,v 1.4 2007/05/07 21:34:53 gdavidson Exp $
// $Id: vxRenderingNodeThread.C,v 1.4 2007/05/07 21:34:53 gdavidson Exp $
