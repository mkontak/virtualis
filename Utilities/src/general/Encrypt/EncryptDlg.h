
// EncryptDlg.h : header file
//

#pragma once


// CEncryptDlg dialog
class CEncryptDlg : public CDialogEx
{
// Construction
public:
	CEncryptDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ENCRYPT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CToolTipCtrl* m_pToolTip;
  bool m_bUpdating;
  COLORREF m_foreground;
  CFont * m_pFont;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClearTextChanged();
	afx_msg void OnEncryptedTextChanged();
  afx_msg void OnOkClicked();
  afx_msg void OnDestroy();
};
