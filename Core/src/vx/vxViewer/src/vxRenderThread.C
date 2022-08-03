// $Id: vxRenderThread.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderThread.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRenderThread.h"
#include "vxRenderer.h"


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


/**
 * Constructor
 */
vxRenderThread::vxRenderThread()
:m_pRenderer(NULL),
m_uThreadID(-1),
m_bRendering(false)
//*******************************************************************
{
  CreateEvents();
} // constructor


/**
 * Constructor
 * @param parent renderer
 * @param thread id
 */
vxRenderThread::vxRenderThread(vxRenderer * pRenderer, uint1 uThreadID)
//*******************************************************************
{
  m_pRenderer = pRenderer;
  m_uThreadID = uThreadID;
  m_bRendering = false;
  CreateEvents();
} // constructor


/**
 * Destructor
 */
vxRenderThread::~vxRenderThread()
//*******************************************************************
{
  ExitRendering();
  CloseHandle(m_hRenderEvent);
  CloseHandle(m_hKillEvent);
  CloseHandle(m_hCompleteEvent);
} // destructor


/**
 * Creates the thread synchronization objects
 */
void vxRenderThread::CreateEvents()
//*******************************************************************
{
  m_hRenderEvent   = CreateEvent(NULL, false, false, NULL);
  m_hKillEvent     = CreateEvent(NULL, false, false, NULL);
  m_hCompleteEvent = CreateEvent(NULL, false, false, NULL);
} // CreateEvents()


/**
 * Pulses the wait event to render a frame
 */
void vxRenderThread::StartRendering()
//*******************************************************************
{
  m_bRendering = true;
  ResetEvent(m_hCompleteEvent);
  SetEvent(m_hRenderEvent);
} // StartRendering()


/**
 * Sets the kill event to exit the run loop
 */
void vxRenderThread::ExitRendering()
//*******************************************************************
{
  SetEvent(m_hKillEvent);
  SetEvent(m_hCompleteEvent);
} // ExitRendering()


/**
 * Waits for the frame to be rendered
 * @param dwTimeout time(milliseconds) to wait for event to be signaled.
 */
void vxRenderThread::WaitForRenderingToComplete(DWORD dwTimeout)
//*******************************************************************
{
  if (IsRendering() == true)
    WaitForSingleObject(m_hCompleteEvent, dwTimeout);
}


/**
 * Calls the rendering method with the given window
 */
void vxRenderThread::Run() 
//*******************************************************************
{
  try
  {
    const DWORD dwNumberOfHandles = 2;

    // create the handles to wait on
    HANDLE pHandles[] = { m_hRenderEvent, m_hKillEvent};

    DWORD dwReturn = 0;
    while ((dwReturn = WaitForMultipleObjects(dwNumberOfHandles, pHandles, false, INFINITE)) >= WAIT_OBJECT_0)
    {
      // figure out the index of the event that got us here
      int iEventIndex = dwReturn - WAIT_OBJECT_0;

      // should we render another frame?
      if (iEventIndex == 0)
      {
        ResetEvent(m_hRenderEvent);

        try
        {
          m_pRenderer->RenderWindow(m_uWindow, m_uThreadID); 
        }
        catch (...)
        {
          LogWrn("thread did not finish rendering the window due to exception, will continue anyway", "vxRenderThread", "Run");
        }
        m_bRendering = false;

        SetEvent(m_hCompleteEvent);
      }
      else if (iEventIndex == 1)
      {
        break; // outta here
      }
    } // WaitForMultipleObjects()
  }
  catch (...)
  {
    throw ex::VException(LogRec("thread terminated to early","vxRenderThread","Run"));
  }
} // Run()

#undef FILE_REVISION


// Revision History:
// $Log: vxRenderThread.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/07/25 12:07:24  geconomos
// Spelling in comments.
//
// Revision 1.9  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.8  2003/05/17 15:44:51  michael
// reformatting
//
// Revision 1.7  2003/05/16 13:22:15  frank
// formatting
//
// Revision 1.6  2003/05/16 11:45:27  michael
// code review
//
// Revision 1.5  2003/05/14 18:30:04  dongqing
// code review
//
// Revision 1.4  2003/05/14 16:10:20  michael
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderThread.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxRenderThread.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
