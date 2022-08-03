
#include "stdafx.h"
#include "CommandLineInfo.h"


/**
* Parses the command line
*/
void CommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
  static CString lastParameter("");

  if (bFlag)
  {
    lastParameter = lpszParam;
  }
  else
  {
    if (lastParameter == "k")
      m_sActivationKey = lpszParam;
    else if (lastParameter == "a")
      m_sApplication = lpszParam;
    
    
  }


  // Call the base class to ensure proper command line processing
  CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
}
