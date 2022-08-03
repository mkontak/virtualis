// $Id: BackgroundProcess.h,v 1.1 2005/08/05 12:59:44 geconomos Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: BackgroundProcess.h
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


#if !defined(AFX_BACKGROUNDPROCESS_H__D3363C33_31DD_47C8_A25E_D61B9FC04384__INCLUDED_)
#define AFX_BACKGROUNDPROCESS_H__D3363C33_31DD_47C8_A25E_D61B9FC04384__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "typedef.h"

typedef uint4 (* BackgroundProcessFunction)(void * pData);

// mutually-exclusive, hide parameters while running background threads
enum HideParamsEnum { eBKHIDE_NONE = 0, eBKHIDE_MENU = 1, eBKHIDE_TABS = 2, eBKHIDE_VIEWWIN = 4, eBKHIDE_ALL = 7 };


/**
 * Class for typical background functions
 */
class BackgroundProcess  
{
// Messages
public:
  /// Message to handle start/finish of a background thread: WPARAM(Start?(1:0), HideParamsEnum), LPARAM(thread ID)
  static const uint4 WM_REG_BACKGROUNDPROC_STARTFINISH;

public:
  /// Constructor
  BackgroundProcess(BackgroundProcessFunction f, void* pData = 0, const HWND hwnd = HWND(0), const HideParamsEnum eHide = eBKHIDE_NONE);
  /// Run a background process
  static bool Run(const BackgroundProcessFunction func, void* pData, const std::string& sFuncName,
    const HideParamsEnum eHide = eBKHIDE_NONE, const int4 iPriority = THREAD_PRIORITY_NORMAL);

  /// Run a background process
  bool RunFunc(const BackgroundProcessFunction func, void* pData, const std::string& sFuncName,
    const HideParamsEnum eHide = eBKHIDE_NONE, const int4 iPriority = THREAD_PRIORITY_NORMAL);

  /// Set thread priority
  bool SetThreadPriority(const int4 iPriority);
  /// Suspend thread function
  bool SuspendThread();
  /// Resume thread function
  bool ResumeThread();

private:
  /// Runs a background process (invoked from BackgroundProcess::Run method)
  static ULONG WINAPI BackgroundProcessRunFunction(LPVOID pData);

public:
  /// Background process to run
  BackgroundProcessFunction m_func;
  /// Application-defined data passed to the process
  void* m_pData;
  /// Main application window
  HWND m_hAppWnd;
  /// Defines what user-interface elements will be hidden while running the process
  HideParamsEnum m_eHideParams;

  std::string m_sFuncName;

private:
  /// Handle to thread
  HANDLE m_hThread;
};


class BackgroundWaitProcess
{
};

#endif // !defined(AFX_BACKGROUNDPROCESS_H__D3363C33_31DD_47C8_A25E_D61B9FC04384__INCLUDED_)

// $Log: BackgroundProcess.h,v $
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.2.4.2.4.1  2005/04/21 19:15:10  jmeade
// Issue 3851: Add debugging info for process exceptions.
//
// Revision 1.2.4.2  2003/07/07 20:30:37  jmeade
// Code standards.
//
// Revision 1.2.4.1  2003/06/09 17:32:26  jmeade
// code review prep.
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
// Revision 1.1.2.2  2002/02/08 01:10:08  jmeade
// Comments.
//
// Revision 1.1.2.1  2002/02/07 22:20:27  jmeade
// Background processes get their own class and file.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/BackgroundProcess.h,v 1.1 2005/08/05 12:59:44 geconomos Exp $
// $Id: BackgroundProcess.h,v 1.1 2005/08/05 12:59:44 geconomos Exp $
