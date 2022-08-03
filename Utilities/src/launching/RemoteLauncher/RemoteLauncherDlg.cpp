// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: RemoteLauncherDlg.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "stdafx.h"
#include "RemoteLauncher.h"
#include "RemoteLauncherDlg.h"
#include "afxdialogex.h"
#include "Launcher.h"
#include "Exceptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRemoteLauncherDlg dialog



CRemoteLauncherDlg::CRemoteLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRemoteLauncherDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_pToolTip = NULL;


  m_foreground = RGB(49, 45, 130);


}

void CRemoteLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRemoteLauncherDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
  ON_WM_CTLCOLOR()
	ON_WM_QUERYDRAGICON()
  ON_EN_CHANGE(IDC_EDIT_SERVER, &CRemoteLauncherDlg::OnServerChanged)
  ON_EN_CHANGE(IDC_EDIT_PORT, &CRemoteLauncherDlg::OnPortChanged)
  ON_EN_CHANGE(IDC_EDIT_CONTEXT, &CRemoteLauncherDlg::OnContextChanged)
  ON_EN_CHANGE(IDC_EDIT_USER_NAME, &CRemoteLauncherDlg::OnUserNameChanged)
  ON_EN_CHANGE(IDC_EDIT_PASSWORD, &CRemoteLauncherDlg::OnPasswordChanged)
  ON_EN_CHANGE(IDC_EDIT_DESC, &CRemoteLauncherDlg::OnDescriptionChanged)
  ON_BN_CLICKED(IDC_CHECK_NEW_SESSION, &CRemoteLauncherDlg::OnNewSessionClicked)
  ON_CBN_SELCHANGE(IDC_COMBO_APPLICATION, &CRemoteLauncherDlg::OnApplicationChanged)
  ON_BN_CLICKED(IDOK, &CRemoteLauncherDlg::OnOkClicked)
  ON_EN_CHANGE(IDC_EDIT_UIDS, &CRemoteLauncherDlg::OnUidsChanged)
  ON_WM_DESTROY()
END_MESSAGE_MAP()


// CRemoteLauncherDlg message handlers

BOOL CRemoteLauncherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


  m_pFont = new CFont();
  m_pFont->CreateFont(18, 0, 0, 0, FW_REGULAR, false, false,
    0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
    FIXED_PITCH | FF_MODERN, _T("Book Antiqua"));



  // ===========================
  // Set up the tooltip control
  // ===========================
  m_pToolTip = new CToolTipCtrl;
  if (!m_pToolTip->Create(this))
  {
    TRACE("Unable To create ToolTip\n");
    return TRUE;
  }

  GetDlgItem(IDC_EDIT_URL)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_SERVER)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_PORT)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_CONTEXT)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_USER_NAME)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_DESC)->SetFont(m_pFont);
  GetDlgItem(IDC_CHECK_NEW_SESSION)->SetFont(m_pFont);
  GetDlgItem(IDOK)->SetFont(m_pFont);
  GetDlgItem(IDCANCEL)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_UIDS)->SetFont(m_pFont);
  GetDlgItem(IDC_COMBO_APPLICATION)->SetFont(m_pFont);

 


  if (m_pToolTip->AddTool(this, "V3D Remote Application Launcher"))
    TRACE("Unable to add Dialog to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_URL), "You may enter a custom URL, if you do not want to use the fields provided"))
    TRACE("Unable to add URL to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_SERVER), "Enter the server name of the server running the V3D Viewer Service"))
    TRACE("Unable to add Server to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_PORT), "Enter the port the V3D Viewer Service is listening on"))
    TRACE("Unable to add Port to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_CONTEXT), "Enter the V3D System authentication token/context id"))
    TRACE("Unable to add Context to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_USER_NAME), "Enter either a valid V3D System user name or an alias to be used"))
    TRACE("Unable to add User Name to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_PASSWORD), "Enter the password for the user name specified"))
    TRACE("Unable to add User Name to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_DESC), "Enter the session description to be used if a new session is created"))
    TRACE("Unable to add Description to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_CHECK_NEW_SESSION), "Enter a check in the check box if you would like a new session created"))
    TRACE("Unable to add Description to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDOK), "Click to attempt to remote launch the specified application"))
    TRACE("Unable to add OK Button to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDCANCEL), "Click to cancel the remote launch"))
    TRACE("Unable to add Cancel Button to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_COMBO_APPLICATION), "Select the application to remote launch"))
    TRACE("Unable to add APPLCIATION to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_UIDS), "Enter the list of UID(s) to launch delimited by the '|' character"))
    TRACE("Unable to add UIDS to the tooltip\n");


  m_pToolTip->Activate(TRUE);

  
  ((CComboBox *)GetDlgItem(IDC_COMBO_APPLICATION))->AddString("VC   Virtual Colonoscopy");
  ((CComboBox *)GetDlgItem(IDC_COMBO_APPLICATION))->SetCurSel(0);

  m_request.SetApplication("VC");


	return TRUE;  // return TRUE  unless you set the focus to a control
}


HBRUSH CRemoteLauncherDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{

  HBRUSH brush = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

  switch (nCtlColor)
  {

  case CTLCOLOR_STATIC:
    pDC->SetTextColor(m_foreground);
    break;

  case CTLCOLOR_EDIT:
  case CTLCOLOR_MSGBOX:
    pDC->SetTextColor(m_foreground);
    break;

  case CTLCOLOR_BTN:
    pDC->SetTextColor(m_foreground);
    break;

  }

  return brush;

}


void CRemoteLauncherDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRemoteLauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRemoteLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/**
 * Handles the server changed event
 */
void CRemoteLauncherDlg::OnServerChanged()
{

  CString server;

  GetDlgItemText(IDC_EDIT_SERVER, server);

  if (server.Find(' ') > -1)
  {
    MessageBox("Server name cannot have spaces in it", "Invalid Value", MB_OK | MB_ICONERROR);
    SetDlgItemText(IDC_EDIT_SERVER, "");
    GetDlgItem(IDC_EDIT_SERVER)->SetFocus();

  }
  else
  { 
    m_request.SetServer(server);

    SetDlgItemText(IDC_EDIT_URL, m_request.GetUrl());
  }
} // OnServerChanged()

/**
 * Handles the Port changed event
 */
void CRemoteLauncherDlg::OnPortChanged()
{
  CString sPort;

  GetDlgItemText(IDC_EDIT_PORT, sPort);

  char * end;

  long port = strtol(sPort, &end, 10);

  if (*end != 0)
  {
    MessageBox("Port must be an integer value", "Invalid Value", MB_OK | MB_ICONERROR);
    SetDlgItemText(IDC_EDIT_PORT, "");
    GetDlgItem(IDC_EDIT_PORT)->SetFocus();
  }
  else
  {
    m_request.SetPort((unsigned int)port);
    SetDlgItemText(IDC_EDIT_URL, m_request.GetUrl());

  }

  SetDlgItemText(IDC_EDIT_URL, m_request.GetUrl());
} // OnPortChanged()

/**
 * Handles the context changed event
 */
void CRemoteLauncherDlg::OnContextChanged()
{
  CString context;

  GetDlgItemText(IDC_EDIT_CONTEXT, context);

  if (context.Find(' ') > -1)
  {
    MessageBox("Context name cannot have spaces in it", "Invalid Value", MB_OK | MB_ICONERROR);
    SetDlgItemText(IDC_EDIT_CONTEXT, "");
    GetDlgItem(IDC_EDIT_CONTEXT)->SetFocus();

  }
  else
  {

    m_request.SetContext(context);

    if (context.IsEmpty())
    {
      CEdit * pUserName = (CEdit *)GetDlgItem(IDC_EDIT_USER_NAME);
      pUserName->EnableWindow(FALSE);
      pUserName->SetWindowText("");
      m_request.SetUserName("");

      CEdit * pPassword = (CEdit *)GetDlgItem(IDC_EDIT_PASSWORD);
      pPassword->EnableWindow(FALSE);
      pPassword->SetWindowText("");
      m_request.SetPassword("");


    }
    else
    {
      GetDlgItem(IDC_EDIT_USER_NAME)->EnableWindow(TRUE);
      GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(TRUE);
    }


    SetDlgItemText(IDC_EDIT_URL, m_request.GetUrl());

  }
} // OnContextChanged()

/**
 * Handles the user changed event
 */
void CRemoteLauncherDlg::OnUserNameChanged()
{
  CString username;

  GetDlgItemText(IDC_EDIT_USER_NAME, username);

  m_request.SetUserName(username);

  SetDlgItemText(IDC_EDIT_URL, m_request.GetUrl());
  
} // OnUserNameChanged()

/**
 * Handles the password changed event
 */
void CRemoteLauncherDlg::OnPasswordChanged()
{
  CString password;

  GetDlgItemText(IDC_EDIT_PASSWORD, password);

  if (password.Find(' ') > -1)
  {
    MessageBox("Password cannot have spaces in it", "Invalid Value", MB_OK | MB_ICONERROR);
    SetDlgItemText(IDC_EDIT_PASSWORD, "");
    GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();

  }
  else
  {

    m_request.SetUserName(password);

    SetDlgItemText(IDC_EDIT_URL, m_request.GetUrl());
  }

} // OnPasswordChanged()

/**
 * Handles the description changed event
 */
void CRemoteLauncherDlg::OnDescriptionChanged()
{
  CString desc;

  GetDlgItemText(IDC_EDIT_DESC, desc);

  m_request.SetDescription(desc);

  SetDlgItemText(IDC_EDIT_URL, m_request.GetUrl());
} // OnDescriptionChanged()

/**
 * Handles the New Session Clicked event
 */
void CRemoteLauncherDlg::OnNewSessionClicked()
{
  CButton * pNewSession = (CButton *)GetDlgItem(IDC_CHECK_NEW_SESSION);

  UINT uCheck = pNewSession->GetState();

  m_request.SetNewSession(((uCheck & BST_CHECKED) == BST_CHECKED ? true : false));

  SetDlgItemText(IDC_EDIT_URL, m_request.GetUrl());



} // OnNewSessionClicked()

/**
 * Handles the application changed event
 */
void CRemoteLauncherDlg::OnApplicationChanged()
{
  CComboBox * pApplication = (CComboBox *)GetDlgItem(IDC_COMBO_APPLICATION);

  CString sComboText;

  pApplication->GetWindowText(sComboText);

  CString sApplication = sApplication.Left(3).TrimRight();

  m_request.SetApplication(sApplication);

  SetDlgItemText(IDC_EDIT_URL, m_request.GetUrl());


} // OnApplicationChanged()

/**
 * Handles the PreTranslateMessage event so that toop tips can be displayed.
 *
 * @param pMsg      Message
 */
BOOL CRemoteLauncherDlg::PreTranslateMessage(MSG* pMsg)
{
  if (NULL != m_pToolTip)
    m_pToolTip->RelayEvent(pMsg);

  return CDialog::PreTranslateMessage(pMsg);
}

/**
 * Handles the OK Button click event
 */
void CRemoteLauncherDlg::OnOkClicked()
{
  
  SCOPE_EXIT()
  {
    GetDlgItem(IDC_EDIT_URL)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_SERVER)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
    GetDlgItem(IDC_COMBO_APPLICATION)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_USER_NAME)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_CONTEXT)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_DESC)->EnableWindow(TRUE);
    GetDlgItem(IDC_CHECK_NEW_SESSION)->EnableWindow(TRUE);
    GetDlgItem(IDOK)->EnableWindow(TRUE);
    GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
  };


  try
  {
    GetDlgItem(IDC_EDIT_URL)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_SERVER)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_APPLICATION)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_USER_NAME)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_CONTEXT)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_DESC)->EnableWindow(FALSE);
    GetDlgItem(IDC_CHECK_NEW_SESSION)->EnableWindow(FALSE);
    GetDlgItem(IDOK)->EnableWindow(FALSE);
    GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

    CString session("");

    Launcher::Launch(m_request, session);

    MessageBox("Session : " + session, "Launch Succeed", MB_OK | MB_ICONEXCLAMATION);

  }
  catch (LaunchException ex)
  {
    MessageBox((LPCSTR)ex.GetMessage(), "Launch Failed", MB_OK | MB_ICONERROR);
  }

}

/**
 * Handles the change event for the UIDs edit box
 */
void CRemoteLauncherDlg::OnUidsChanged()
{
  CString uids;

  GetDlgItemText(IDC_EDIT_UIDS, uids);

  if (uids.Find(' ') > -1)
  {
    MessageBox("UID(s) cannot have spaces in it, use the '|' as a delimiter", "Invalid Value", MB_OK);
    SetDlgItemText(IDC_EDIT_UIDS, "");
    GetDlgItem(IDC_EDIT_UIDS)->SetFocus();

  }
  else
  {

    m_request.SetUids(uids);

    SetDlgItemText(IDC_EDIT_URL, m_request.GetUrl());
  }
} // OnUidsChanged()


void CRemoteLauncherDlg::OnDestroy()
{
  CDialog::OnDestroy();

  if (m_pFont != __nullptr)
  {
    m_pFont->DeleteObject();
    delete m_pFont;
  }

}
