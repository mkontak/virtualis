
// CreateUserDlg.h : header file
//

#pragma once


// CCreateUserDlg dialog
class CCreateUserDlg : public CDialogEx
{
// Construction
public:
	CCreateUserDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CREATEUSER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
  HICON m_hIcon;
  CToolTipCtrl* m_pToolTip;
  COLORREF m_foreground;
  CFont * m_pFont;
  int m_iPhysicianIndex;


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnCreateNewClicked();
  afx_msg void OnOkClicked();
};
