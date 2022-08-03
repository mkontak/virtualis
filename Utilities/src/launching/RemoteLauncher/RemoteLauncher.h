// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: RemoteLauncher.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// forward declarations
class LaunchRequest;

/**
 * Implements a remote launcher application
 */
class CRemoteLauncherApp : public CWinApp
{
public:
	CRemoteLauncherApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CRemoteLauncherApp theApp;