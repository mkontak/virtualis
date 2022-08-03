// $Id: RenderThread.C,v 1.6.6.1 2010/06/08 21:12:03 dongqing Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net, Kevin Kreeger  kevin@viatronix.net


// includes
#include "stdafx.h"
#include "RenderThread.h"
#include "SystemInfo.h"
#include "Dataset.h"

// defines
#define FILE_REVISION "$Revision: 1.6.6.1 $"


// namespaces
using namespace ReaderLib;


/**
 * Constructor 
 *
 * @param rVolRender   Volume render instance
 * @param rDataset     Dataset to be rendered
 * @param uId          ID
 */
RenderThread::RenderThread(VolumeRender& rVolRender, Dataset& rDataset, const uint4 uId)
 : m_rVolRender(rVolRender), m_rDataset(rDataset), m_uID(uId), m_hThread(NULL)
{
  m_hDoneRenderingEvent  = CreateEvent(NULL,true,false, NULL);
  m_hStartRenderingEvent = CreateEvent(NULL,true,false, NULL);
  m_hExitThreadEvent     = CreateEvent(NULL,true,false, NULL);

  // Create thread for rendering
  m_hThread = HANDLE(_beginthreadex(NULL, 0, ThreadControlFunction, ((LPVOID)(this)), 0, NULL));

  if (IsThreadValid())
  {
    SystemInfo systemInfo;  // To ensure that system information is initialized for use below
 
//    uint4 uCPUs = SystemInfo::GetCpuInfo().GetTotalThreads();  uCPUs = (uCPUs!=0) ? uCPUs : 1;
//    SetThreadIdealProcessor(m_hThread, (uId % uCPUs));
    SetThreadAffinityMask( m_hThread, 1 << uId );
    SetThreadPriority( m_hThread, THREAD_PRIORITY_HIGHEST );
  }
  else
  {
    LogWrn(((m_rDataset.GetOrientation()==DatasetOrientations::eSUPINE)?"Supine":"Prone")+ToAscii(uId)+" Thread failed with code:"+ToAscii(GetLastError()),
      "RenderThread","RenderThread");
  }
} // RenderThread()


/**
 * Destructor 
 */
RenderThread::~RenderThread()
{
  SetEvent(m_hExitThreadEvent);
}


/**
 * Indicates whether or not thread is valid
 *
 * @return bool   True if thread handle is non-null
 */
bool RenderThread::IsThreadValid()
{
  return (m_hThread != NULL);
}


/**
 * Signals the render event
 */
void RenderThread::StartRender()
{
  ResetEvent(m_hDoneRenderingEvent);
  SetEvent(m_hStartRenderingEvent);
}


/**
 * Waits for the rendering event to be set (i.e. for rendering for complete)
 */
void RenderThread::WaitForRender()
{
  WaitForSingleObject(m_hDoneRenderingEvent, INFINITE);
}


/**
 * Worker thread function, handles processing needed for rendering
 *
 * @param pParam   Pointer to a RenderThread instance.
 * @return         Always 1.
 */
UINT _stdcall RenderThread::ThreadControlFunction(LPVOID pParam)
{
  RenderThread * pRenderThreadData = ((RenderThread *)(pParam));
  HANDLE hEvents[2] = { pRenderThreadData->m_hExitThreadEvent, pRenderThreadData->m_hStartRenderingEvent };

  while(1)
  {
    if (WaitForMultipleObjects(2, hEvents, FALSE, INFINITE) == WAIT_OBJECT_0)
    {
      return 1;
    }

    pRenderThreadData->Render();
    ResetEvent(pRenderThreadData->m_hStartRenderingEvent);
    SetEvent(pRenderThreadData->m_hDoneRenderingEvent);
  }

  return 0;
} // ThreadControlFunction()


/**
 * Renders the object
 */
void RenderThread::Render()
{
  m_rVolRender.Raycast2(m_uID, m_rDataset,m_renderingBounds);
}


// $Log: RenderThread.C,v $
// Revision 1.6.6.1  2010/06/08 21:12:03  dongqing
// change code to allow spinning out 16 threads
//
// Revision 1.6  2006/02/02 20:01:46  geconomos
// updated for changes to SystemInfo
//
// Revision 1.5  2006/01/31 14:05:19  frank
// code review
//
// Revision 1.4  2005/10/28 13:32:22  geconomos
// added render window bounds
//
// Revision 1.3  2005/10/25 13:47:37  geconomos
// added rendering bounds
//
// Revision 1.2  2005/09/28 20:25:13  vxconfig
// renamed study files to dataset
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.5.2.4  2003/04/10 17:49:11  kevin
// Create the render threads with the same stack size as teh main thread.
// (Note we no longer link with a 50MB stack size, and the recursive growing
// has been replaced with a list method).
//
// Revision 3.5.2.3  2003/04/09 16:17:19  jmeade
// Comments, coding standards.
//
// Revision 3.5.2.2  2003/04/04 20:13:26  kevin
// Issue 2835: The threads were crashing on creatiion. 2 things were
// done to fix this:
// (1) Handle rendierng if the threads are non-existant by switching
// to sequential rendering (this should have been in there nayway)
// (2) Create the threads early when there is plenty of memory blocks
//
// Revision 3.5.2.1  2003/03/05 16:01:17  jmeade
// Issue 2835:  Put an upper limit on time to wait for render thread completion.
//
// Revision 3.5  2003/01/29 19:36:48  jmeade
// Use an event instead of a flag to end the thread.
//
// Revision 3.4  2003/01/29 18:53:38  jmeade
// Removed code duplication in RenderThread and derivatives.
//
// Revision 3.3  2003/01/07 15:05:53  kevin
// Render threads switched to worker-threads rather than windows-threads
//
// Revision 3.2  2002/12/23 22:29:15  kevin
// Fixed the VERY VERY VERY BAD sleep-busy-wait cycle that
// significantly effected the rendering speed and system performance
// on the newer faster processors
//
// Revision 3.1  2002/04/12 02:13:44  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 08 2001 21:37:48   jmeade
// Specify study to be rendered (removed legacy assumption of rdrGlobal.m_pStudy)
// Resolution for 1078: Registration support
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/RenderThread.C,v 1.6.6.1 2010/06/08 21:12:03 dongqing Exp $
// $Id: RenderThread.C,v 1.6.6.1 2010/06/08 21:12:03 dongqing Exp $
