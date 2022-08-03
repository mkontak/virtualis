
// GetSystemIdDlg.h : header file
//

#pragma once


// CGetSystemIdDlg dialog
class CGetSystemIdDlg : public CDialogEx
{

// Construction
public:
	CGetSystemIdDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_GETSYSTEMID_DIALOG };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:

	HICON m_hIcon;


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:

  CBrush * m_pEditBkBrush;
  CFont * m_pFont;


};
