
// Activate.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CActivateApp:
// See Activate.cpp for the implementation of this class
//

class CActivateApp : public CWinApp
{
public:
	CActivateApp();

// Overrides
public:

  
	virtual BOOL InitInstance();

  virtual int ExitInstance();

  /// Performs the activation
  static int Activate(LPCTSTR application, LPCTSTR key);

  /// Performs the activation
  static int Activate(sys::Applications application, LPCTSTR key)
  {
    return Activate(sys::DemoRegistry64::ToString(application).c_str(), key);
  }

private:


  int m_exitCode;

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CActivateApp theApp;