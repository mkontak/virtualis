

#pragma once

#include <afxwin.h>

class CommandLineInfo : public CCommandLineInfo
{

public:
  CommandLineInfo() : m_sActivationKey(""), m_sApplication("") { }

public:

  CString GetActivationKey() { return m_sActivationKey; }

  CString GetApplication() { return m_sApplication; }

  bool IsParametersSpecified() { return (!m_sApplication.IsEmpty() && ! m_sActivationKey.IsEmpty());  }

public:


  void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

private:

  CString m_sActivationKey;

  CString m_sApplication;


};