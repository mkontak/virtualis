// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: RemoteLauncherDlg.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


#include "LaunchRequest.h"


// CRemoteLauncherDlg dialog
class CRemoteLauncherDlg : public CDialogEx
{
// Construction
public:
	CRemoteLauncherDlg(CWnd* pParent = NULL);	// standard constructor

  ~CRemoteLauncherDlg() 
  {
    if (m_pToolTip != NULL )
      delete m_pToolTip;
  }

// Dialog Data
	enum { IDD = IDD_REMOTELAUNCHER_DIALOG };

 
protected:

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support



// Implementation
protected:
	HICON m_hIcon;
  LaunchRequest m_request;
  CToolTipCtrl* m_pToolTip;
  CFont * m_pFont;
  COLORREF m_background;
  COLORREF m_foreground; 

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg HCURSOR OnQueryDragIcon();
  afx_msg BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnServerChanged();
  afx_msg void OnPortChanged();
  afx_msg void OnContextChanged();
  afx_msg void OnUserNameChanged();
  afx_msg void OnPasswordChanged();
  afx_msg void OnDescriptionChanged();
  afx_msg void OnNewSessionClicked();
  afx_msg void OnApplicationChanged();
  afx_msg void OnOkClicked();
  afx_msg void OnUidsChanged();
  afx_msg void OnDestroy();
};
