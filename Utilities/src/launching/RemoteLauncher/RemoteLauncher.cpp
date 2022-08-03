// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: RemoteLauncher.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "stdafx.h"
#include "RemoteLauncher.h"
#include "RemoteLauncherDlg.h"
#include "CommandLineInfo.h"
#include "LaunchRequest.h"
#include "Exceptions.h"
#include "Launcher.h"
#include <streambuf>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRemoteLauncherApp


BEGIN_MESSAGE_MAP(CRemoteLauncherApp, CWinApp)
  ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CRemoteLauncherApp construction

CRemoteLauncherApp::CRemoteLauncherApp()
{
  // support Restart Manager
  m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

  // TODO: add construction code here,
  // Place all significant initialization in InitInstance
}


// The one and only CRemoteLauncherApp object

CRemoteLauncherApp theApp;


// CRemoteLauncherApp initialization

BOOL CRemoteLauncherApp::InitInstance()
{

  // InitCommonControlsEx() is required on Windows XP if an application
  // manifest specifies use of ComCtl32.dll version 6 or later to enable
  // visual styles.  Otherwise, any window creation will fail.
  INITCOMMONCONTROLSEX InitCtrls;
  InitCtrls.dwSize = sizeof(InitCtrls);
  // Set this to include all the common control classes you want to use
  // in your application.
  InitCtrls.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&InitCtrls);

  CWinApp::InitInstance();


  AfxEnableControlContainer();

  // Create the shell manager, in case the dialog contains
  // any shell tree view or shell list view controls.
  CShellManager *pShellManager = new CShellManager;

  // Activate "Windows Native" visual manager for enabling themes in MFC controls
  CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

  // Standard initialization
  // If you are not using these features and wish to reduce the size
  // of your final executable, you should remove from the following
  // the specific initialization routines you do not need
  // Change the registry key under which our settings are stored
  // TODO: You should modify this string to be something appropriate
  // such as the name of your company or organization
  SetRegistryKey(_T("Local AppWizard-Generated Applications"));

  CommandLineInfo cmdInfo;


  try
  {

    // Parse the command line
    ParseCommandLine(cmdInfo);

    // ===========================================================
    // If there are parameters specified then attempt the launch
    // ===========================================================
    if (cmdInfo.IsParametersSpecified())
    {

      CString session; 

      Launcher::Launch(cmdInfo.GetLaunchRequest(), session );

      return FALSE;

    }
  }
  catch (UsageException e)
  {
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
      int osfh = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), 8);
      if ((HANDLE)osfh != INVALID_HANDLE_VALUE)
      {
        *stdout = *_tfdopen(osfh, _T("a"));
      }

      fprintf(stdout, "%s", (LPCTSTR)e.GetMessage());


    }
    return FALSE;

  }
  catch (InvalidDataException e)
  {
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
      int osfh = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), 8);
      if ((HANDLE)osfh != INVALID_HANDLE_VALUE)
      {
        *stdout = *_tfdopen(osfh, _T("a"));
      }

      fprintf(stdout, "Failed to launch : %s", (LPCTSTR)e.GetMessage());


    }
    return FALSE;

  }
  catch (LaunchException e)
  {

    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
      int osfh = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), 8);
      if ((HANDLE)osfh != INVALID_HANDLE_VALUE)
      {
        *stdout = *_tfdopen(osfh, _T("a"));
      }

      fprintf(stdout, "Failed to launch : %s", (LPCTSTR)e.GetMessage());

      
    }

    CString message;

    message.Format("Launch failed: %s", e.GetMessage());

    MessageBox(NULL, message, "Launch Failed", MB_OK | MB_ICONERROR);

    return FALSE;

  }


  CRemoteLauncherDlg dlg;
  m_pMainWnd = &dlg;
  INT_PTR nResponse = dlg.DoModal();
  if (nResponse == IDOK)
  {
    // TODO: Place code here to handle when the dialog is
    //  dismissed with OK
  }
  else if (nResponse == IDCANCEL)
  {
    // TODO: Place code here to handle when the dialog is
    //  dismissed with Cancel
  }
  else if (nResponse == -1)
  {
    TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
    TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
  }

  // Delete the shell manager created above.
  if (pShellManager != NULL)
  {
    delete pShellManager;
  }

  // Since the dialog has been closed, return FALSE so that we exit the
  //  application, rather than start the application's message pump.
  return FALSE;
}


