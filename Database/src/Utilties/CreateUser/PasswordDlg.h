#pragma once


// CPasswordDlg dialog

class CPasswordDlg : public CDialogEx
{
  DECLARE_DYNAMIC(CPasswordDlg)

public:
  CPasswordDlg(CWnd* pParent = NULL);   // standard constructor

  // Dialog Data
  enum { IDD = IDD_DIALOG_PASSWORD };


  CString GetPassword() { return m_sPassword; }

  static void ValidatePassword(CString sPassword);
  static CString GetServicePassword();

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
  CToolTipCtrl* m_pToolTip;
  COLORREF m_foreground;
  CFont * m_pFont;
  CString m_sPassword;


  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnClearTextChanged();
  afx_msg void OnEncryptedTextChanged();
  afx_msg void OnOkClicked();
  afx_msg void OnCancelClicked();
  afx_msg void OnDestroy();
};
