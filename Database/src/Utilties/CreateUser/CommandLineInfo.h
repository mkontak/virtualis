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
  CommandLineInfo() : m_sLogin(""), m_sPassword(""), m_sRole("Physician"), m_sName(""), m_bCreateUser(false), m_bChangePassword(true), m_bDisplayMessages(false) { }



public:


  /// Gets the password specified
  CString GetPassword() { return m_sPassword; }

  /// Gets the login specified
  CString GetLogin() { return m_sLogin; }

  /// Gets the role specified
  CString GetRole() { return m_sRole; }

  /// Gets the name specified
  CString GetName() { return m_sName; }

  bool DisplayMessages() { return m_bDisplayMessages; }

  bool IsCreateUser() { return m_bCreateUser;  }

  bool IsPasswordChangeRequired() { return m_bChangePassword;  }

  bool IsParametersSpecified();


public:


  virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

private:

  CString m_sLogin;

  CString m_sPassword;

  CString m_sName;

  CString m_sRole;

  bool m_bCreateUser;

  bool m_bChangePassword;

  bool m_bDisplayMessages;

};