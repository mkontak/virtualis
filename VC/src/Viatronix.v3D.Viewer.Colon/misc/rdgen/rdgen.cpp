
#include "stdafx.h"
#include "rdgen.h"
#include "MainDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



BEGIN_MESSAGE_MAP(Application, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


Application theApp;

/**
 *
 */
Application::Application()
{
} // Application()



/**
 *
 */
BOOL Application::InitInstance()
{
	CWinApp::InitInstance();

	MainDialog dlg;
	dlg.DoModal();
	return FALSE;
} // InitInstance()
