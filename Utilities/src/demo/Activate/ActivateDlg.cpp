
// ActivateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Activate.h"
#include "ActivateDlg.h"
#include "afxdialogex.h"
#include "Wmi.h"


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


// CActivateDlg dialog



CActivateDlg::CActivateDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CActivateDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  m_foreground = RGB(33, 7, 101);
}

void CActivateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CActivateDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_CTLCOLOR()
  ON_WM_DESTROY()
  ON_WM_ERASEBKGND()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_BUTTON_ACTIVATE, &CActivateDlg::OnBnClickedActivate)
  ON_EN_CHANGE(IDC_EDIT_KEY, &CActivateDlg::OnEnChangeEditKey)
  ON_CBN_SELCHANGE(IDC_COMBO_APPLICATION, &CActivateDlg::OnCbnSelchangeComboApplication)
  ON_WM_NCDESTROY()
END_MESSAGE_MAP()


// CActivateDlg message handlers

BOOL CActivateDlg::OnInitDialog()
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


  GetDlgItem(IDC_EDIT_KEY)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_SYSTEMID)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_SYSTEMID)->SetWindowText(sys::Wmi::GetSystemId().c_str());

  m_pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_APPLICATION);
  m_pComboBox->SetFont(m_pFont);
  


  // ===========================
  // Set up the tooltip control
  // ===========================
  m_pToolTip = new CToolTipCtrl;
  if (!m_pToolTip->Create(this))
  {
    TRACE("Unable To create ToolTip\n");
    return TRUE;
  }

  if (m_pToolTip->AddTool(this, "Activate Demo Application"))
    TRACE("Unable to add Dialog to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_KEY), "Enter activation key received from Viatronix"))
    TRACE("Unable to add DAYS to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_SYSTEMID), "System Id"))
    TRACE("Unable to add SYSTEM ID to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_COMBO_APPLICATION), "Select the Application DEMO you are activating"))
    TRACE("Unable to add APPLICATION to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_ACTIVATE), "Click to activate the application DEMO"))
    TRACE("Unable to add ACTIVATE to the tooltip\n");

  m_pToolTip->Activate(TRUE);


  std::vector<std::string> applications = sys::DemoRegistry64::GetApplicationList();

  std::for_each(applications.begin(), applications.end(), [this](std::string app)
  {
    m_pComboBox->AddString(app.c_str());
  });



  SetFont(m_pFont);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CActivateDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CActivateDlg::OnPaint()
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
HCURSOR CActivateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CActivateDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

/**
* Handles the PreTranslateMessage event so that toop tips can be displayed.
*
* @param pMsg      Message
*/
BOOL CActivateDlg::PreTranslateMessage(MSG* pMsg)
{
  if (NULL != m_pToolTip)
    m_pToolTip->RelayEvent(pMsg);

  return CDialog::PreTranslateMessage(pMsg);
}

void CActivateDlg::OnDestroy()
{
  CDialog::OnDestroy();

  if (m_pFont != __nullptr)
  {
    m_pFont->DeleteObject();
    delete m_pFont;
  }
}


void CActivateDlg::OnBnClickedActivate()
{

  CString app;
  if (GetDlgItemText(IDC_COMBO_APPLICATION, app) == 0 || app.IsEmpty() )
  {
    AfxMessageBox(TEXT("No application was specified"));
    GetDlgItem(IDC_COMBO_APPLICATION)->SetFocus();
    return;
  }

  CString key;
  if (GetDlgItemText(IDC_EDIT_KEY, key) == 0 || key.IsEmpty() )
  {
    AfxMessageBox(TEXT("No activation key specified"));
    GetDlgItem(IDC_EDIT_KEY)->SetFocus();
    return;
  }


  int iRC(0);
  if ( (iRC = CActivateApp::Activate((sys::Applications)m_pComboBox->GetCurSel(), (LPCTSTR)key)) < 0)
  {
    switch (iRC)
    {
    case -1:
      AfxMessageBox(TEXT("Failed to activate, missing application!"));
      break;

    case -2:
      AfxMessageBox(TEXT("Failed to activate, missing activation key!"));
      break;

    case -100:
      AfxMessageBox(TEXT("Failed to activate, " + app + " already activated!"));
      break;

    case -50:
      AfxMessageBox(TEXT("Failed to activate, could not write to the registry!"));
      break;


    case -200:
      AfxMessageBox(TEXT("Failed to activate, invalid activation key!"));
      break;

    case -300:
      AfxMessageBox(TEXT("Failed to activate, key has expired!"));
      break;


    default:
      AfxMessageBox(TEXT("Failed to activate!"));
      break;


    };
  
    EndDialog(0);

  }
  else
  {
    AfxMessageBox(TEXT("Activation was successful!"));
    EndDialog(0);
  }


}



void CActivateDlg::OnEnChangeEditKey()
{

  EnableActivationButton();
}


void CActivateDlg::OnCbnSelchangeComboApplication()
{
  EnableActivationButton();
}

void CActivateDlg::EnableActivationButton()
{
  CString key;
  CString app;

  if (GetDlgItemText(IDC_EDIT_KEY, key) > 0)
  {
    if (GetDlgItemText(IDC_COMBO_APPLICATION, app) > 0)
    {
      GetDlgItem(IDC_BUTTON_ACTIVATE)->EnableWindow();
    }
  }

}


void CActivateDlg::OnNcDestroy()
{
  CDialogEx::OnNcDestroy();

  // TODO: Add your message handler code here
}
