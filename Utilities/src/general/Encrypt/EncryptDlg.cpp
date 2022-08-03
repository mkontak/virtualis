
// EncryptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Encrypt.h"
#include "EncryptDlg.h"
#include "afxdialogex.h"
#include "crypto\AESCrypt.h"
#include "PasswordDlg.h"

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


// CEncryptDlg dialog



CEncryptDlg::CEncryptDlg(CWnd* pParent /*=NULL*/) :
m_bUpdating(false),
CDialogEx(CEncryptDlg::IDD, pParent)
{
  m_foreground = RGB(33, 7, 101);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEncryptDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
  ON_WM_CTLCOLOR()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT_CLEAR_TEXT, &CEncryptDlg::OnClearTextChanged)
	ON_EN_CHANGE(IDC_EDIT_ENCRYPTED_TEXT, &CEncryptDlg::OnEncryptedTextChanged)
  ON_BN_CLICKED(IDOK, &CEncryptDlg::OnOkClicked)
  ON_WM_DESTROY()
END_MESSAGE_MAP()


// CEncryptDlg message handlers

BOOL CEncryptDlg::OnInitDialog()
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



  GetDlgItem(IDC_EDIT_CLEAR_TEXT)->SetFont(m_pFont);
  GetDlgItem(IDC_EDIT_ENCRYPTED_TEXT)->SetFont(m_pFont);
  GetDlgItem(IDOK)->SetFont(m_pFont);
  GetDlgItem(IDCANCEL)->SetFont(m_pFont);

	// ===========================
	// Set up the tooltip control
	// ===========================
	m_pToolTip = new CToolTipCtrl;
	if (!m_pToolTip->Create(this))
	{
		TRACE("Unable To create ToolTip\n");
		return TRUE;
	}


	if (m_pToolTip->AddTool(this, "V3D Remote Application Launcher"))
		TRACE("Unable to add Dialog to the tooltip\n");

	if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_CLEAR_TEXT), "You may enter any clear text that you want encrypted"))
		TRACE("Unable to add ENCRYPT to the tooltip\n");

	if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_ENCRYPTED_TEXT), "You may enter any encrypted text you want decrypted"))
		TRACE("Unable to add DECRYPT to the tooltip\n");


	if (m_pToolTip->AddTool(GetDlgItem(IDOK), "Click to attempt to encrypt or decrypt the text"))
		TRACE("Unable to add OK Button to the tooltip\n");

	if (m_pToolTip->AddTool(GetDlgItem(IDCANCEL), "Click to cancel the application"))
		TRACE("Unable to add Cancel Button to the tooltip\n");



	m_pToolTip->Activate(TRUE);


	return TRUE;  // return TRUE  unless you set the focus to a control
}


/**
* Handles the PreTranslateMessage event so that toop tips can be displayed.
*
* @param pMsg      Message
*/
BOOL CEncryptDlg::PreTranslateMessage(MSG* pMsg)
{
  if (NULL != m_pToolTip)
    m_pToolTip->RelayEvent(pMsg);

  return CDialog::PreTranslateMessage(pMsg);
}

HBRUSH CEncryptDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CEncryptDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CEncryptDlg::OnPaint()
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
HCURSOR CEncryptDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CEncryptDlg::OnClearTextChanged()
{
  if (!m_bUpdating)
  {
 
    CString text;
    GetDlgItemText(IDC_EDIT_CLEAR_TEXT, text);

    if (!text.IsEmpty())
    {
      SetDlgItemText(IDC_EDIT_ENCRYPTED_TEXT, "");
      ((CButton *)GetDlgItem(IDOK))->SetWindowText("Encrypt");
      ((CButton *)GetDlgItem(IDOK))->EnableWindow(TRUE);
    }
    else
    {
      GetDlgItemText(IDC_EDIT_ENCRYPTED_TEXT, text);

      if (text.IsEmpty())
        ((CButton *)GetDlgItem(IDOK))->EnableWindow(FALSE);
      else
      {
        ((CButton *)GetDlgItem(IDOK))->SetWindowText("Decrypt");
        ((CButton *)GetDlgItem(IDOK))->EnableWindow(TRUE);

      }

    }
  }

}


void CEncryptDlg::OnEncryptedTextChanged()
{
  if (!m_bUpdating)
  {


	  CString text;
	  GetDlgItemText(IDC_EDIT_ENCRYPTED_TEXT, text);

	  if (!text.IsEmpty())
	  {
		  SetDlgItemText(IDC_EDIT_CLEAR_TEXT, "");
      ((CButton *)GetDlgItem(IDOK))->SetWindowText("Decrypt");
      ((CButton *)GetDlgItem(IDOK))->EnableWindow(TRUE);
	  }
    else
    {
      GetDlgItemText(IDC_EDIT_CLEAR_TEXT, text);

      if (text.IsEmpty())
        ((CButton *)GetDlgItem(IDOK))->EnableWindow(FALSE);
      else
      {
        ((CButton *)GetDlgItem(IDOK))->SetWindowText("Encrypt");
        ((CButton *)GetDlgItem(IDOK))->EnableWindow(TRUE);

      }

    }
  }
}


void CEncryptDlg::OnOkClicked()
{

  CString clearText;
  CString encryptedText;

  GetDlgItemText(IDC_EDIT_ENCRYPTED_TEXT, encryptedText);
  GetDlgItemText(IDC_EDIT_CLEAR_TEXT, clearText);
  GetDlgItem(IDC_EDIT_ENCRYPTED_TEXT)->EnableWindow(FALSE);
  GetDlgItem(IDC_EDIT_CLEAR_TEXT)->EnableWindow(FALSE);

  m_bUpdating = true;

  SCOPE_EXIT
  {
    m_bUpdating = false;
    GetDlgItem(IDC_EDIT_ENCRYPTED_TEXT)->EnableWindow(TRUE);
    GetDlgItem(IDC_EDIT_CLEAR_TEXT)->EnableWindow(TRUE);

  };

  try
  {
    std::string results;
    if (!clearText.IsEmpty())
    {
      results = crypto::AESCrypt::Encrypt((LPCSTR)clearText);

      encryptedText = results.c_str();
      SetDlgItemText(IDC_EDIT_ENCRYPTED_TEXT, encryptedText);
    }
    else
    {

      CPasswordDlg dlg;

      INT_PTR nResponse = dlg.DoModal();
      if (nResponse == IDOK)
      {

        results = crypto::AESCrypt::Decrypt((LPCSTR)encryptedText);
        clearText = results.c_str();
        SetDlgItemText(IDC_EDIT_CLEAR_TEXT, clearText);
      }
      else
      {
        GetDlgItem(IDC_EDIT_ENCRYPTED_TEXT)->SetFocus();
      }
    }
  }
  catch (ex::VException & ex)
  {
    MessageBox(ex.GetLogRecord().GetTextMessage().c_str(), "Encryption Failed", MB_OK | MB_ICONERROR);
  }
}
 




void CEncryptDlg::OnDestroy()
{
  CDialog::OnDestroy();

  if (m_pFont != __nullptr)
  {
    m_pFont->DeleteObject();
    delete m_pFont;
  }
}
