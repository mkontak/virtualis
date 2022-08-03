
// GetSystemIdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GetSystemId.h"
#include "GetSystemIdDlg.h"
#include "afxdialogex.h"
#include "Wmi.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGetSystemIdDlg dialog



CGetSystemIdDlg::CGetSystemIdDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGetSystemIdDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

 

  // Instantiate and initialize the background brush to black.
  m_pEditBkBrush = new CBrush(RGB(192, 192, 192));
}

void CGetSystemIdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGetSystemIdDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_WM_CTLCOLOR()
  ON_WM_DESTROY()
  ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CGetSystemIdDlg message handlers

BOOL CGetSystemIdDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


  LOGFONT lf;
  CFont font;

  // Creates a 12-point-Courier-font
  font.CreatePointFont(120, _T("Courier"));
  font.GetLogFont(&lf);
  lf.lfWeight = FW_BOLD;

  m_pFont = CFont::FromHandle(::CreateFontIndirect(&lf));

  // Without a member variable
  GetDlgItem(IDC_EDIT2)->SetFont(m_pFont);
  SetDlgItemText(IDC_EDIT2,sys::Wmi::GetSystemId().c_str());
  SetFont(m_pFont);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGetSystemIdDlg::OnPaint()
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
HCURSOR CGetSystemIdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HBRUSH CGetSystemIdDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  switch (nCtlColor)
  {

  case CTLCOLOR_STATIC:
  case CTLCOLOR_EDIT:
  case CTLCOLOR_MSGBOX:
    // Set the background mode for text to transparent  
    // so background will show thru.
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(RGB(0, 0, 128));
    //pDC->SetBkColor(RGB(255, 255, 255));
    return (HBRUSH)(m_pEditBkBrush->GetSafeHandle());

  default:
    return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
  }
}

void CGetSystemIdDlg::OnDestroy()
{
  CDialog::OnDestroy();

  // Free the space allocated for the background brush
  delete m_pEditBkBrush;
  m_pFont->DeleteObject();
}




BOOL CGetSystemIdDlg::OnEraseBkgnd(CDC* pDC)
{
  CRect rect;
  GetClientRect(&rect);
  CBrush myBrush(RGB(192,192,192));    // dialog background color
  CBrush *pOld = pDC->SelectObject(&myBrush);
  BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
  pDC->SelectObject(pOld);    // restore old brush
  return bRes;                       // CDialog::OnEraseBkgnd(pDC);
}



