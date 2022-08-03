
// ActivateDemoDlg.h : header file
//

#pragma once


// CActivateDemoDlg dialog
class CActivateDemoDlg : public CDialogEx
{
// Construction
public:
	CActivateDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ACTIVATEDEMO_DIALOG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support




private:

  COLORREF m_foreground;
  CFont * m_pFont;
  CComboBox * m_pComboBox;
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
  afx_msg void OnBnClickedGenerateActivationKey();



};
