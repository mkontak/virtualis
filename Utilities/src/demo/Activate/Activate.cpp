
// Activate.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Activate.h"
#include "ActivateDlg.h"
#include "CommandLineInfo.h"
#include "DemoRegistry.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CActivateApp

BEGIN_MESSAGE_MAP(CActivateApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CActivateApp construction

CActivateApp::CActivateApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CActivateApp object

CActivateApp theApp;


// CActivateApp initialization

BOOL CActivateApp::InitInstance()
{

  m_exitCode = 0;

  CommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);

  if (cmdInfo.IsParametersSpecified() )
  {

    m_exitCode = Activate((LPCTSTR)cmdInfo.GetApplication(), (LPCTSTR)cmdInfo.GetActivationKey());
 
    return FALSE;

  }



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

	CActivateDlg dlg;
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

int CActivateApp::ExitInstance()
{
  if (m_exitCode != 0)
    return m_exitCode;
  else
    return CWinApp::ExitInstance();
}

int CActivateApp::Activate(LPCSTR application, LPCTSTR activationKey)
{

  std::string app(application);

  std::string key(activationKey);

  if (app.empty())
    return -1;

  if (key.empty())
    return -2;

  sys::DemoRegistry64 demo(app);

  // Already installed
  if (demo.Exists())
  {
    // If the keys are equal we are already activated otherwise we are re-activating 
    if (demo.GetActivationKey() == key)
      return -100;
  }


  try
  {

    // sets the activation key received from viatronix
    demo.SetActivationKey(key);


    // Activate the demo
    demo.Activate();

    return 0;

  }
  catch (ex::DemoRegistryException &)
  {
    return -50;
  }
  catch (ex::DemoInvalidDataException &)
  {
    return -200;
  }
  catch (ex::DemoActivationKeyExpiredException &)
  {
    return -300;
  }
  catch (ex::DemoAlreadyInstalled &)
  {
    return -100;
  }
  catch (...)
  {
    return -200;
  }
}