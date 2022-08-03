
#pragma once


class MainDialog : public CDialog
{

public:

  MainDialog( CWnd* pParent = NULL );	

	enum { IDD = IDD_RDGEN_DIALOG };

protected:

	virtual BOOL OnInitDialog();

  DECLARE_MESSAGE_MAP()

  afx_msg void OnClickedGenerate();

  afx_msg void OnShowFileOpen();

  void GenerateRegistryFile( const CString & sFilename, const CString & sDisclaimerText, bool b64bit );
  
};
