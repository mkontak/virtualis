
// CreateUserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CreateUser.h"
#include "CreateUserDlg.h"
#include "afxdialogex.h"
#include "Database.h"
#include "Utilities.h"

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


// CCreateUserDlg dialog



CCreateUserDlg::CCreateUserDlg(CWnd* pParent /*=NULL*/) :
CDialogEx(CCreateUserDlg::IDD, pParent)
{
  m_foreground = RGB(33, 7, 101);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCreateUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCreateUserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_CHECK_CREATE_NEW, &CCreateUserDlg::OnCreateNewClicked)
  ON_BN_CLICKED(IDOK, &CCreateUserDlg::OnOkClicked)

END_MESSAGE_MAP()


// CCreateUserDlg message handlers

BOOL CCreateUserDlg::OnInitDialog()
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



  GetDlgItem(IDC_EDIT_LOGIN)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(m_pFont);
  GetDlgItem(IDC_CHECK_CREATE_NEW)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_NAME)->SetFont(m_pFont);
  GetDlgItem(IDC_COMBO_ROLE)->SetFont(m_pFont);
  GetDlgItem(IDOK)->SetFont(m_pFont);
  GetDlgItem(IDCANCEL)->SetFont(m_pFont);
  GetDlgItem(IDC_CHECK_CHANGE)->SetFont(m_pFont);

  ((CButton *)GetDlgItem(IDC_CHECK_CREATE_NEW))->SetCheck(BST_CHECKED);

  // ===========================
  // Set up the tooltip control
  // ===========================
  m_pToolTip = new CToolTipCtrl;
  if (!m_pToolTip->Create(this))
  {
    TRACE("Unable To create ToolTip\n");
    return TRUE;
  }


  if (m_pToolTip->AddTool(this, "V3D Create User"))
    TRACE("Unable to add Dialog to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_LOGIN), "Enter the login you want to use or a new login"))
    TRACE("Unable to add LOGIN to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_PASSWORD), "Enter the password"))
    TRACE("Unable to add PASSWORD to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_NAME), "Enter users name"))
    TRACE("Unable to add NAME to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_COMBO_ROLE), "Select the role for the user"))
    TRACE("Unable to add ROLE to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_CHECK_CHANGE), "Check this item if you require a password change"))
    TRACE("Unable to add CHANGE to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_CHECK_CREATE_NEW), "Check this item if you want to create a new user, unchecked will only validate"))
    TRACE("Unable to add CHANGE to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDOK), " Click to validate or add the specified user"))
    TRACE("Unable to add OK Button to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDCANCEL), "Click to cancel the application"))
    TRACE("Unable to add Cancel Button to the tooltip\n");



  m_pToolTip->Activate(TRUE);

  try
  {
    std::vector<std::string> roles = Database::GetRoles();

    CComboBox * pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_ROLE);
    m_iPhysicianIndex = 0;
    int count = 0;

    std::for_each(roles.begin(), roles.end(), [pComboBox, this, &count](std::string role)
    {
      pComboBox->AddString(role.c_str());
      if (role == "Physician")
        m_iPhysicianIndex = count;
      count++;
        
    });

    pComboBox->SetCurSel(m_iPhysicianIndex);

  }
  catch (ex::VException & ex)
  {
    AfxMessageBox(ex.GetLogRecord().GetTextMessage().c_str());
  }



	return TRUE;  // return TRUE  unless you set the focus to a control
}


/**
* Handles the PreTranslateMessage event so that toop tips can be displayed.
*
* @param pMsg      Message
*/
BOOL CCreateUserDlg::PreTranslateMessage(MSG* pMsg)
{
  if (NULL != m_pToolTip)
    m_pToolTip->RelayEvent(pMsg);

  return CDialog::PreTranslateMessage(pMsg);
}

HBRUSH CCreateUserDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CCreateUserDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCreateUserDlg::OnPaint()
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
HCURSOR CCreateUserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCreateUserDlg::OnCreateNewClicked()
{
  CButton * pCreateNew = (CButton *)GetDlgItem(IDC_CHECK_CREATE_NEW);

  UINT uCheck = pCreateNew->GetState();


  if ((uCheck & BST_CHECKED) == BST_CHECKED )
  {
    SetDlgItemText(IDOK, "Create");
    GetDlgItem(IDC_COMBO_ROLE)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_NAME)->EnableWindow(TRUE);
    GetDlgItem(IDC_CHECK_CHANGE)->EnableWindow(TRUE);
  }
  else
  {
    SetDlgItemText(IDOK, "Validate");
    GetDlgItem(IDC_COMBO_ROLE)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_NAME)->EnableWindow(FALSE);
    SetDlgItemText(IDC_EDIT_NAME, "");
    GetDlgItem(IDC_CHECK_CHANGE)->EnableWindow(FALSE);
    ((CButton *)GetDlgItem(IDC_CHECK_CHANGE))->SetCheck(BST_UNCHECKED);

  }
}

/**
 * OK Click handler
 */
void CCreateUserDlg::OnOkClicked()
{
  CButton * pCreateNew = (CButton *)GetDlgItem(IDC_CHECK_CREATE_NEW);

  UINT uCheck = pCreateNew->GetState();
  CString sLogin;
  CString sPassword;
  GetDlgItemText(IDC_EDIT_LOGIN, sLogin);
  GetDlgItemText(IDC_EDIT_PASSWORD, sPassword);

  // ===================================
  // Make sure the login was specified
  // ===================================
  if (sLogin.IsEmpty())
  {
    MessageBox("No login id was sepcified", "Missing data", MB_OK | MB_ICONERROR);
    GetDlgItem(IDC_EDIT_LOGIN)->SetFocus();

  }

  // =====================================
  // make sure the password was specified
  // =======================================
  if (sPassword.IsEmpty() )
  {
    MessageBox("No password was sepcified", "Missing data", MB_OK | MB_ICONERROR);
    GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();

  }

  if ((uCheck & BST_CHECKED) == BST_CHECKED)
  {
    CString sName;
    CString sRole;

    GetDlgItemText(IDC_EDIT_NAME, sName);

    // =====================================
    // make sure the user name was specified
    // =======================================
    if (sName.IsEmpty())
    {
      MessageBox("No user name was sepcified", "Missing data", MB_OK | MB_ICONERROR);
      GetDlgItem(IDC_EDIT_NAME)->SetFocus();

    }

    GetDlgItemText(IDC_COMBO_ROLE, sRole);

    // =====================================
    // make sure the user role was specified
    // =======================================
    if (sRole.IsEmpty())
    {
      MessageBox("No user role was sepcified", "Missing data", MB_OK | MB_ICONERROR);
      GetDlgItem(IDC_COMBO_ROLE)->SetFocus();

    }

    CButton * pChange = (CButton *)GetDlgItem(IDC_CHECK_CHANGE);
    bool bChangePassword = (  (pChange->GetState() & BST_CHECKED) == BST_CHECKED ? true : false );

    try
    {
      Database::AddUser(sLogin, sPassword, sName, sRole, bChangePassword);

      CString message;
      message.Format("User %s - %s was successfully added", (LPCTSTR)sLogin, (LPCTSTR)sName);

      MessageBox((LPCTSTR)message, "User Added", MB_OK | MB_ICONINFORMATION);

      SetDlgItemText(IDC_EDIT_LOGIN, "");
      SetDlgItemText(IDC_EDIT_PASSWORD, "");
      pChange->SetCheck(BST_UNCHECKED);
      SetDlgItemText(IDC_EDIT_NAME, "");

      ((CComboBox *)GetDlgItem(IDC_COMBO_ROLE))->SetCurSel(m_iPhysicianIndex);

    }
    catch (ex::VException & ex)
    {

      Error error = Utilities::ParseMessage(ex.GetLogRecord().GetTextMessage());
      
      CString message;
      message.Format("[%d] %s", error.Code, (LPCTSTR)error.Message);

      MessageBox(message, "Add User Failed", MB_OK | MB_ICONERROR);
      GetDlgItem(IDC_EDIT_LOGIN)->SetFocus();

    }
  }
  else
  {
    try
    {
      Database::ReleaseContext(Database::AcquireContext(sLogin, sPassword));

    }
    catch (ex::VException & ex)
    {
      Error error = Utilities::ParseMessage(ex.GetLogRecord().GetTextMessage());

      CString message;
      message.Format("[%d] %s", error.Code, (LPCSTR)error.Message);

      MessageBox(message, "Validation Failed", MB_OK | MB_ICONERROR);
      GetDlgItem(IDC_EDIT_LOGIN)->SetFocus();

    }

  }

}
