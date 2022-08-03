

#pragma once

#include <afxwin.h>

class CommandLineInfo : public CCommandLineInfo
{

public:
  CommandLineInfo() : m_sFilePath("") { }

public:

  CString GetFilePath() { return m_sFilePath; }

  bool IsFileSpecified() { return ! m_sFilePath.IsEmpty();}

public:


  void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

private:

  CString m_sFilePath; 


};