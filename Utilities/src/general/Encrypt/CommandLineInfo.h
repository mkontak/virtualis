// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CommandLineInfo.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include <afxwin.h>




class CommandLineInfo : public CCommandLineInfo
{

public:
	CommandLineInfo() : m_bParametersSpecified(false), m_bEncrypt(true), m_bDecrypt(false), m_sText(""), m_sPassword(""), m_sFilePath("") {}



public:


	CString GetPassword() { return m_sPassword;  }
	CString GetText() { return m_sText; }

  /// Get the specified file path
  CString GetFilePath() { return m_sFilePath; }


	bool IsEncrypt() { return m_bEncrypt; }
	bool IsDecrypt() { return m_bDecrypt; }

	bool IsParametersSpecified() { return m_bParametersSpecified; }


public:


	virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

private:

	CString m_sText;

  CString m_sFilePath;
	

	CString m_sPassword;

	bool m_bEncrypt;

	bool m_bDecrypt;

	bool m_bParametersSpecified;
};