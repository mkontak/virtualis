// $Id: BackgroundProcess.C,v 1.3 2007/03/08 22:21:42 romy Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: BackgroundProcess.C
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "BackgroundProcess.h"
//#include "Logger.h"
//#include "Exception.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

#define FILE_REVISION "$Revision: 1.3 $"

/// Message to handle start/finish of a background thread: WPARAM(Start?(1:0), HideParamsEnum), LPARAM(thread ID)
const uint4 BackgroundProcess::WM_REG_BACKGROUNDPROC_STARTFINISH(RegisterWindowMessage(TEXT("Viatronix.v3D.Message.WM_APP_BACKGROUNDPROC_STARTFINISH")));


/**
 * Constructor 
 *
 * @param f       Background process to run.
 * @param pData   Application-defined data for function.
 * @param hwnd    Main application window.
 * @param eHide   Defines what user-interface elements will be hidden while running the process.
 */
BackgroundProcess::BackgroundProcess(BackgroundProcessFunction f, void* pData, const HWND hwnd, const HideParamsEnum eHide)
  : m_func(f), m_pData(pData), m_hAppWnd(hwnd), m_eHideParams(eHide), m_hThread(HANDLE(NULL))
{
}


/**
 * Creates thread for, and runs, a background process.
 *
 * @param func        Function to run.
 * @param pData       Application-defined data.
 * @param sFuncName   Name of the function being run, for debugging/logging purposes.
 * @param eHide       Identifies what user interface controls to hide during process run, if any.
 * @param iPriority   Priority of the thread.
 * @return            Whether or not the thread could be created.
 */
bool BackgroundProcess::Run(const BackgroundProcessFunction func, void* pData, const std::string& sFuncName, const HideParamsEnum eHide, const int4 iPriority)
{
  BackgroundProcess * pBPS = new BackgroundProcess(func, pData, AfxGetMainWnd()->GetSafeHwnd(), eHide);
  pBPS->m_sFuncName = sFuncName;

  DWORD uThreadID(0);
  HANDLE hThread = CreateThread(NULL, 0, BackgroundProcessRunFunction, pBPS, CREATE_SUSPENDED, &uThreadID);

  if (hThread == 0)
  {
    LogErr("Could not create background thread to run process:" + sFuncName, "BackgroundProcess", "Run");
    return false;
  }

  ::SetThreadPriority(hThread, iPriority);
  ::ResumeThread(hThread);
  return true;
}


/**
 * Creates thread for, and runs, a background process.
 *
 * @param func        Function to run.
 * @param pData       Application-defined data.
 * @param sFuncName   Name of the function being run, for debugging/logging purposes.
 * @param eHide       Identifies what user interface controls to hide during process run, if any.
 * @param iPriority   Priority of the thread.
 * @return            Whether or not the thread could be created.
 */
bool BackgroundProcess::RunFunc(const BackgroundProcessFunction func, void* pData, const std::string& sFuncName, const HideParamsEnum eHide, const int4 iPriority)
{
  if (m_hThread != 0)
  {
    // thread already running
    return false;
  }

  BackgroundProcess * pBPS = new BackgroundProcess(func, pData, AfxGetMainWnd()->GetSafeHwnd(), eHide);
  pBPS->m_sFuncName = sFuncName;

  DWORD uThreadID(0);
  m_hThread = CreateThread(NULL, 0, BackgroundProcessRunFunction, pBPS, CREATE_SUSPENDED, &uThreadID);

  if (m_hThread == 0)
  {
    LogErr("Could not create background thread to run process:" + sFuncName, "BackgroundProcess", "Run");
    return false;
  }

  ::SetThreadPriority(m_hThread, iPriority);
  ::ResumeThread(m_hThread);
  return true;
}


/**
 * Set thread priority
 * @param iPriority   Priority of the thread.
 * @return            Whether or not the thread priority could be set.
 */
bool BackgroundProcess::SetThreadPriority(const int4 iPriority)
{
  if (m_hThread == HANDLE(0))
  {
    // no thread running
    return false;
  }
  return true;
}


/**
 * Suspend thread function
 * @return            Whether or not the thread could be suspended.
 */
bool BackgroundProcess::SuspendThread()
{
  if (m_hThread == HANDLE(0))
  {
    // no thread running
    return false;
  }

  return (::SuspendThread(m_hThread) != DWORD(-1));
}


/**
 * Resume thread function
 * @return            Whether or not the thread could be resumed.
 */
bool BackgroundProcess::ResumeThread()
{
  if (m_hThread == HANDLE(0))
  {
    // no thread running
    return false;
  }

  // decrease thread suspend count all the way to 0
  DWORD iPrevCount(100);
  for (; iPrevCount > 1; iPrevCount = ::ResumeThread(m_hThread));

  return iPrevCount != DWORD(-1);
}


/**
 * Runs the background process.
 *
 * @param pData   Pointer to a BkgdProcStruct instance (which must be deleted).
 * @return        Value returned by background function.
 */
ULONG WINAPI BackgroundProcess::BackgroundProcessRunFunction(LPVOID pData)
{
  try
  {
    // Initialize any COM for new thread
    CoInitialize(NULL);

    const BackgroundProcess* pBPS = static_cast<BackgroundProcess*>(pData);
    DWORD uThreadID = GetCurrentThreadId();
    
    ULONG uReturnVal = 0;
    try
    {
      // Send message to application window that thread is starting
      ::SendMessage(pBPS->m_hAppWnd, WM_REG_BACKGROUNDPROC_STARTFINISH, MAKEWPARAM(TRUE, uint4(pBPS->m_eHideParams)), uThreadID);

      // Run the function
      uReturnVal = pBPS->m_func(pBPS->m_pData);
    }
    catch (...)
    {
      LogErr("Unhandled exception caught in BackgroundProcess function: " + pBPS->m_sFuncName, "BackgroundProcess", "BackgroundProcessFunctionRun");
      // Error::WarnLog("Exception caught in BackgroundProcess function");
      uReturnVal = 0;
    }

    // Send message to application window that thread is completing
    ::SendMessage(pBPS->m_hAppWnd, WM_REG_BACKGROUNDPROC_STARTFINISH, MAKEWPARAM(FALSE, uint4(pBPS->m_eHideParams)), uThreadID);

    // delete struct allocated in RunBackgroundProcess(...)
    delete pData;

    // Close COM
    CoUninitialize();

    // Return the value returned by the background function
    return uReturnVal;
  }
  catch ( ... )
  {
    const BackgroundProcess* pBPS = static_cast<BackgroundProcess*>(pData);
    LogErr( "unhandled exception: " + pBPS->m_sFuncName, "BackgroundProcess", "BackgroundProcessRunFunction" );
    return 0;
  }
}

#undef FILE_REVISION

// $Log: BackgroundProcess.C,v $
// Revision 1.3  2007/03/08 22:21:42  romy
// code review - header files adjusted
//
// Revision 1.2  2006/06/01 20:19:11  frank
// updated to visual studio 2005
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.3.2.4.4.2  2005/04/21 19:15:10  jmeade
// Issue 3851: Add debugging info for process exceptions.
//
// Revision 1.3.2.4.4.1  2005/04/13 14:51:53  frank
// Issue #4112: Improved exception handling
//
// Revision 1.3.2.4  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 1.3.2.3  2004/02/03 19:01:37  mkontak
// Missing #undef FILE_REVISION
//
// Revision 1.3.2.2  2003/07/07 20:30:37  jmeade
// Code standards.
//
// Revision 1.3.2.1  2003/06/09 17:32:26  jmeade
// code review prep.
//
// Revision 1.3  2002/11/21 01:07:31  jmeade
// Message naming conventions.
//
// Revision 1.2  2002/07/19 22:37:37  jmeade
// Using registered windows messaging for all messages.
//
// Revision 1.1  2002/07/19 16:07:51  jmeade
// Moved background thread and function progress classes.
//
// Revision 3.1  2002/03/13 22:01:15  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 1.1.2.2  2002/02/08 01:10:07  jmeade
// Comments.
//
// Revision 1.1.2.1  2002/02/07 22:20:27  jmeade
// Background processes get their own class and file.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/BackgroundProcess.C,v 1.3 2007/03/08 22:21:42 romy Exp $
// $Id: BackgroundProcess.C,v 1.3 2007/03/08 22:21:42 romy Exp $
