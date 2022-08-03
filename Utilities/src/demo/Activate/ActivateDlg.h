
// ActivateDlg.h : header file
//

#pragma once


// CActivateDlg dialog
class CActivateDlg : public CDialogEx
{
// Construction
public:
	CActivateDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ACTIVATE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:

  void CreateFont(int pointSize, LPCSTR fontName, CFont ** pFont);

  void EnableActivationButton();

private:

  COLORREF m_foreground;
  CComboBox * m_pComboBox;
  CFont * m_pFont;
  CToolTipCtrl* m_pToolTip;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
  afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnBnClickedActivate();

  afx_msg void OnEnChangeEditKey();
  afx_msg void OnCbnSelchangeComboApplication();
  afx_msg void OnNcDestroy();
};
