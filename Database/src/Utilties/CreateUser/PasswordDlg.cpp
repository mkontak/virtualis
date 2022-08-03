// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CreateUser.h"
#include "PasswordDlg.h"
#include "afxdialogex.h"
#include "Exceptions.h"
#include "crypto\AESCrypt.h"


// CPasswordDlg dialog

IMPLEMENT_DYNAMIC(CPasswordDlg, CDialogEx)

CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CPasswordDlg::IDD, pParent)
{

}


void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialogEx)
  ON_BN_CLICKED(IDOK, &CPasswordDlg::OnOkClicked)
  ON_BN_CLICKED(IDCANCEL, &CPasswordDlg::OnCancelClicked)
END_MESSAGE_MAP()

BOOL CPasswordDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();


  m_pFont = new CFont();
  m_pFont->CreateFont(18, 0, 0, 0, FW_REGULAR, false, false,
    0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
    FIXED_PITCH | FF_MODERN, _T("Book Antiqua"));



  GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(m_pFont);
  GetDlgItem(IDOK)->SetFont(m_pFont);

  // ===========================
  // Set up the tooltip control
  // ===========================
  m_pToolTip = new CToolTipCtrl;
  if (!m_pToolTip->Create(this))
  {
    TRACE("Unable To create ToolTip\n");
    return TRUE;
  }


  if (m_pToolTip->AddTool(this, "Encryption Application"))
    TRACE("Unable to add Dialog to the tooltip\n");

  if (m_pToolTip->AddTool(GetDlgItem(IDC_EDIT_PASSWORD), "Enter the password to create user"))
    TRACE("Unable to add DECRYPT to the tooltip\n");


  m_pToolTip->Activate(TRUE);


  return TRUE;  // return TRUE  unless you set the focus to a control
}


CString CPasswordDlg::GetServicePassword()
{
  static CString sServicePassword(crypto::AESCrypt::Decrypt("BtMRulDkb8Uwe2cZDmJjSQ==").c_str());

  return sServicePassword;
}
void CPasswordDlg::ValidatePassword(CString sPassword)
{
  if (sPassword != GetServicePassword())
    throw InvalidPasswordException("Password is invalid");
}

void CPasswordDlg::OnOkClicked()
{

  CString password;

  GetDlgItemText(IDC_EDIT_PASSWORD, password);

  try
  {
    ValidatePassword(password);

    this->m_nModalResult = IDOK;

    this->OnOK();

  }
  catch (InvalidPasswordException ex)
  {

    MessageBox((LPCTSTR)ex.GetMessage(), "Invalid password", MB_OK | MB_ICONERROR);

    SetDlgItemText(IDC_EDIT_PASSWORD, "");
    GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
  }

}

void CPasswordDlg::OnCancelClicked()
{

  CString password;
  CString encryptedText;

  GetDlgItemText(IDC_EDIT_PASSWORD, password);

  this->m_nModalResult = IDCANCEL;

  this->OnCancel();


}

void CPasswordDlg::OnDestroy()
{
  CDialogEx::OnDestroy();

  if (m_pFont != __nullptr)
    delete m_pFont;
}


