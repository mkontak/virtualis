
// Encrypt.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CEncryptApp:
// See Encrypt.cpp for the implementation of this class
//

class CEncryptApp : public CWinApp
{
public:
	CEncryptApp();

// Overrides
public:
	virtual BOOL InitInstance();
  virtual int ExitInstance();


private:

  int m_exitCode;

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CEncryptApp theApp;