
// ActivateDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ActivateDemo.h"
#include "ActivateDemoDlg.h"
#include "afxdialogex.h"
#include "DemoRegistry.h"


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


// CActivateDemoDlg dialog



CActivateDemoDlg::CActivateDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CActivateDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  m_foreground = RGB(33, 7, 101);

}

void CActivateDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CActivateDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
  ON_WM_CTLCOLOR()
  ON_WM_DESTROY()
  ON_WM_ERASEBKGND()
	ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_BUTTON1, &CActivateDemoDlg::OnBnClickedGenerateActivationKey)
END_MESSAGE_MAP()


// CActivateDemoDlg message handlers

BOOL CActivateDemoDlg::OnInitDialog()
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



  GetDlgItem(IDC_EDIT_DAYS)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_SYSTEMID)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_DAYS)->SetWindowTextA(TEXT("30"));
  GetDlgItem(IDC_EDIT_KEY)->SetFont(m_pFont);
  GetDlgItem(IDC_COMBO_APPLICATION)->SetFont(m_pFont);



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

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_DAYS), "Enter the number of days to activate this DEMO"))
    TRACE("Unable to add DAYS to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_COMBO_APPLICATION), "Select the application to generate the key for"))
    TRACE("Unable to add APPLICATION to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_SYSTEMID), "Enter the system id of the system the DEMO is to be installed on"))
    TRACE("Unable to add SYSTEM ID to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_KEY), "Demo Activation Key for the system and application specified, copy and paste this to an email"))
    TRACE("Unable to add KEY ID to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_BUTTON_ACTIVATE), "Click to generate the activation key"))
    TRACE("Unable to add KEY ID to the tooltip\n");

  m_pToolTip->Activate(TRUE);

  std::vector<std::string> applications = sys::DemoRegistry64::GetApplicationList();

  m_pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO_APPLICATION);

  std::for_each(applications.begin(), applications.end(), [this](std::string app)
  {
    m_pComboBox->AddString(app.c_str());
  });


	return TRUE;  // return TRUE  unless you set the focus to a control
}

/**
* Handles the PreTranslateMessage event so that toop tips can be displayed.
*
* @param pMsg      Message
*/
BOOL CActivateDemoDlg::PreTranslateMessage(MSG* pMsg)
{
  if (NULL != m_pToolTip)
    m_pToolTip->RelayEvent(pMsg);

  return CDialog::PreTranslateMessage(pMsg);
}

void CActivateDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CActivateDemoDlg::OnPaint()
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
HCURSOR CActivateDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CActivateDemoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CActivateDemoDlg::OnDestroy()
{
  CDialog::OnDestroy();

  if (m_pFont != __nullptr)
  {
    m_pFont->DeleteObject();
    delete m_pFont;
  }

}

void CActivateDemoDlg::OnBnClickedGenerateActivationKey()
{
  CString systemId;

  if ( GetDlgItemText(IDC_EDIT_SYSTEMID,systemId) == 0 || systemId.IsEmpty())
  {
    AfxMessageBox(TEXT("No system id was specified"));
    GetDlgItem(IDC_EDIT_SYSTEMID)->SetFocus();
    return;
  }

  int days;
  BOOL bDaysTranslated;

  days = GetDlgItemInt(IDC_EDIT_DAYS, &bDaysTranslated);


  if (!bDaysTranslated)
  {
    AfxMessageBox(TEXT("Invalid days specified must be an integer value"));
    GetDlgItem(IDC_EDIT_DAYS)->SetFocus();
    return;
  }

  int app =  m_pComboBox->GetCurSel();


  

  GetDlgItem(IDC_EDIT_KEY)->SetWindowTextA( sys::DemoRegistry64::GenerateActivationKey((sys::Applications)app, (LPCSTR)systemId, days).c_str());

}
