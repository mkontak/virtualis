
// Encrypt.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Encrypt.h"
#include "EncryptDlg.h"
#include "Exceptions.h"
#include "CommandLineInfo.h"
#include <streambuf>
#include <io.h>
#include "crypto\AESCrypt.h"
#include "PasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEncryptApp

BEGIN_MESSAGE_MAP(CEncryptApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CEncryptApp construction

CEncryptApp::CEncryptApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

  m_exitCode = 0;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CEncryptApp object

CEncryptApp theApp;


// CEncryptApp initialization

BOOL CEncryptApp::InitInstance()
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

      m_exitCode = 0;

      std::string text((LPCTSTR)cmdInfo.GetText());
      std::string results("");

      if (cmdInfo.IsEncrypt())
        results = crypto::AESCrypt::Encrypt(text);
      else
      {

        CPasswordDlg::ValidatePassword(cmdInfo.GetPassword());

        results = crypto::AESCrypt::Decrypt(text);

      }

      if (cmdInfo.GetFilePath().IsEmpty())
      {
        if (AttachConsole(ATTACH_PARENT_PROCESS))
        {
          int osfh = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), 8);
          if ((HANDLE)osfh != INVALID_HANDLE_VALUE)
          {
            *stdout = *_tfdopen(osfh, _T("a"));
          }

          fprintf(stdout, "%s", results.c_str());


        }
      }
      else
      {
        HANDLE fh = CreateFile((LPCTSTR)cmdInfo.GetFilePath(), GENERIC_ALL, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (SUCCEEDED(fh))
        {
          DWORD bytesWritten(0);

          if (!WriteFile(fh, (LPVOID)results.c_str(), results.size(), &bytesWritten, NULL))
          {
            CString error;
            error.Format("could not write to %s [%d]", (LPCTSTR)cmdInfo.GetFilePath(), GetLastError());

            throw EncryptionException(error);

          }

          CloseHandle(fh);


        }
        else
        {
          CString error;
          error.Format("could not open %s to write results [%d]", (LPCTSTR)cmdInfo.GetFilePath(), GetLastError());

          throw EncryptionException(error);

        }
      }

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
  catch (ex::CryptException & ex)
  {
    m_exitCode = -1;

    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
      int osfh = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), 8);
      if ((HANDLE)osfh != INVALID_HANDLE_VALUE)
      {
        *stdout = *_tfdopen(osfh, _T("a"));
      }

      fprintf(stdout, "Failed to encrypt : %s\r\n",  ex.GetLogRecord().GetTextMessage().c_str());



    }
    return FALSE;

  }
	catch (EncryptionException e)
	{
    m_exitCode = -1;

		if (AttachConsole(ATTACH_PARENT_PROCESS))
		{
			int osfh = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), 8);
			if ((HANDLE)osfh != INVALID_HANDLE_VALUE)
			{
				*stdout = *_tfdopen(osfh, _T("a"));
			}

			fprintf(stdout, "Failed to encrypt : %s\r\n", (LPCTSTR)e.GetMessage());



		}
    return FALSE;

	}


	CEncryptDlg dlg;
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



int CEncryptApp::ExitInstance()
{
  if (m_exitCode != 0)
    return m_exitCode;
  else
    return CWinApp::ExitInstance();
}


