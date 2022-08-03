
// GetSystemId.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CGetSystemIdApp:
// See GetSystemId.cpp for the implementation of this class
//

class CGetSystemIdApp : public CWinApp
{
public:
	CGetSystemIdApp();

// Overrides
public:
	virtual BOOL InitInstance();
  virtual int ExitInstance();

private:

  bool m_failed;

public:
// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CGetSystemIdApp theApp;