// $Id: v3D_GraphTimerEx.cpp,v 1.1.2.2 2009/07/29 16:43:29 kchalupa Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "StdAfx.h"
#include "v3D_GraphTimerEx.h"
#include "v3D_VisualizationGraph.h"

USING_GRAPH_NS

/**
 * Constructor.
 */
GraphTimerEx::GraphTimerEx() : m_bRunning( false )
{
  m_mpApplicationIdleHandler = 
    gcnew System::EventHandler( this, &GraphTimerEx::OnApplicationIdleHandler );
} // GraphTimerEx()


/**
 * Starts the timer.
 */
void GraphTimerEx::Start()
{
  if( !m_bRunning )
  {
    System::Windows::Forms::Application::Idle += m_mpApplicationIdleHandler;
    m_bRunning = true;
  }
} // Start()


/**
 * Stops the timer.
 */
void GraphTimerEx::Stop()
{
  if( m_bRunning )
  {
    System::Windows::Forms::Application::Idle -= m_mpApplicationIdleHandler;
    m_bRunning = false;
  }
} // Stop()


/**
 * Application.Idle event handler. Called when the message queue is empty.
 *
 * @param mpSender  Sender of this event.
 * @param mpArgs  EventArgs for this event.
 */
void GraphTimerEx::OnApplicationIdleHandler( System::Object ^ mpSender, System::EventArgs ^ mpArgs )
{
    MSG msg;
    while( m_bRunning && !PeekMessage( &msg, NULL, 0, 0, 0 ) )
    {
      // keep running as long as there isn't any messages on the queue
      Viatronix::Visualization::Graph::VisualizationGraph::Instance->Run();
    }
} // OnApplicationIdleHandler( mpSender, mpArgs )


// $Log: v3D_GraphTimerEx.cpp,v $
// Revision 1.1.2.2  2009/07/29 16:43:29  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.1  2009/01/27 17:26:19  gdavidson
// Initial revision - Moved from Colon plugin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_GraphTimerEx.cpp,v 1.1.2.2 2009/07/29 16:43:29 kchalupa Exp $
// $Id: v3D_GraphTimerEx.cpp,v 1.1.2.2 2009/07/29 16:43:29 kchalupa Exp $
