
// CreateUser.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CreateUser.h"
#include "CreateUserDlg.h"
#include "Exceptions.h"
#include "CommandLineInfo.h"
#include "Database.h"
#include "PasswordDlg.h"
#include "Utilities.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCreateUserApp

BEGIN_MESSAGE_MAP(CCreateUserApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCreateUserApp construction

CCreateUserApp::CCreateUserApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

  m_exitCode = 0;

}


// The one and only CCreateUserApp object

CCreateUserApp theApp;


// CCreateUserApp initialization

BOOL CCreateUserApp::InitInstance()
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

  m_exitCode = 0;
  CString create;

  try
  {

    // Parse the command line
    ParseCommandLine(cmdInfo);

    CString message;

    // ===========================================================
    // If there are parameters specified then attempt the launch
    // ===========================================================
    if (cmdInfo.IsParametersSpecified())
    {

      m_exitCode = 0;

      if (cmdInfo.GetLogin().IsEmpty())
        throw InvalidDataException("No login specified");

      if (cmdInfo.GetPassword().IsEmpty())
        throw InvalidDataException("No password specified");

      if (cmdInfo.IsCreateUser())
      {
        if (cmdInfo.GetName().IsEmpty())
          throw InvalidDataException("No user name specified");

        if (cmdInfo.GetRole().IsEmpty())
          throw InvalidDataException("No user role specified");

        Database::AddUser(cmdInfo.GetLogin(), cmdInfo.GetPassword(), cmdInfo.GetName(), cmdInfo.GetRole(), cmdInfo.IsPasswordChangeRequired() );
        

        message.Format("User %s - %s was added successfully", (LPCTSTR)cmdInfo.GetLogin(), (LPCTSTR)cmdInfo.GetName());

      }
      else
      {
        Database::ReleaseContext(Database::AcquireContext(cmdInfo.GetLogin(), cmdInfo.GetPassword()));

        message.Format("User %s was validated", (LPCTSTR)cmdInfo.GetLogin());

      }

      if (AttachConsole(ATTACH_PARENT_PROCESS))
      {
        int osfh = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), 8);
        if ((HANDLE)osfh != INVALID_HANDLE_VALUE)
        {
          *stdout = *_tfdopen(osfh, _T("a"));
        }

        fprintf(stdout, "%s\r\n", (LPCTSTR)message);


      }

      if (cmdInfo.DisplayMessages())
        MessageBox(NULL, (LPCTSTR)message, "Create User Failed", MB_OK | MB_ICONINFORMATION);

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

      fprintf(stdout, "%s\r\n", (LPCTSTR)e.GetMessage());


    }
    return FALSE;

  }
  catch (ex::VException & ex)
  {



    Error error = Utilities::ParseMessage(ex.GetLogRecord().GetTextMessage());
    m_exitCode = error.Code;


    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
      int osfh = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), 8);
      if ((HANDLE)osfh != INVALID_HANDLE_VALUE)
      {
        *stdout = *_tfdopen(osfh, _T("a"));
      }

      fprintf(stdout, "Failed to create user : [%d] %s\r\n", error.Code, (LPCTSTR)error.Message);



    }


    if (cmdInfo.DisplayMessages())
    {
      CString errorMessage;
      errorMessage.Format("[%d] %s", error.Code, (LPCTSTR)error.Message);

      MessageBox(NULL, (LPCTSTR)errorMessage, "Create User Failed", MB_OK | MB_ICONERROR);
    }

    return FALSE;

  }
  catch (SecurityException e)
  {

    Error error = Utilities::ParseMessage((LPCTSTR)e.GetMessage());
    m_exitCode = error.Code;

    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
      int osfh = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), 8);
      if ((HANDLE)osfh != INVALID_HANDLE_VALUE)
      {
        *stdout = *_tfdopen(osfh, _T("a"));
      }

      fprintf(stdout, "Failed to create user : [%d] %s\r\n", error.Code, (LPCTSTR)error.Message);



    }

    if (cmdInfo.DisplayMessages())
    {
      CString errorMessage;
      errorMessage.Format("[%d] %s", error.Code, (LPCTSTR)error.Message);

      MessageBox(NULL, (LPCTSTR)errorMessage, "Create User Failed", MB_OK | MB_ICONERROR);
    }

    return FALSE;

  }


  
  CPasswordDlg passwordDlg;
  if (passwordDlg.DoModal() != IDOK)
    return FALSE;

	CCreateUserDlg dlg;
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


int CCreateUserApp::ExitInstance()
{
  if (m_exitCode != 0)
    return m_exitCode;
  else
    return CWinApp::ExitInstance();
}

