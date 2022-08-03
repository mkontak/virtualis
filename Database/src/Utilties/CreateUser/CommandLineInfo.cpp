// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CommandLineInfo.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "stdafx.h"
#include "CommandLineInfo.h"
#include "Exceptions.h"


bool CommandLineInfo::IsParametersSpecified()
{
  return (!m_sLogin.IsEmpty() || !m_sPassword.IsEmpty() || !m_sName.IsEmpty() || !m_sRole.IsEmpty());
}


/**
* Parses the command line
*/
void CommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
  static CString lastParameter("");

  if (bFlag)
  {

    lastParameter = lpszParam;

    if (lastParameter == "?" || lastParameter == "help")
    {
      CString message("\r\nCreateUser [-msg 1|0] [-create] [-chgpwd 0|1] [-u USERNAME/LOGIN] [-p PASSWORD] [-r ROLE]  [-n NAME]\r\n");

      message.Append("\r\nwhere:\r\n\r\n");
      
      message.Append("[-create]             Specifies that the user is to be created, otherwise only validation occurs\r\n");
      message.Append("[-chgpwd 0|1]         Specifies that the user will be required to change their password upon first login (Default is 1)\r\n");
      message.Append("[-u USERNAME/LOGIN]   Specifies the users login\r\n");
      message.Append("[-p PASSWORD]         Specifies the users password\r\n");
      message.Append("[-r ROLE]             Specifies the users Role\r\n");
      message.Append("[-n NAME]             Specifies the users name\r\n");
      message.Append("[-msg 0|1]            Specifies whether messages will be displayed (Default is 0)\r\n");



      throw UsageException(message);
    }
    else if (lastParameter == "create")
      m_bCreateUser = true;
    else if (lastParameter != "u" && lastParameter != "p" && lastParameter != "r" && lastParameter != "n" && lastParameter != "chgpwd" && lastParameter != "msg")
    {
      CString message;
      message.Format("Invalid parameter %s", lpszParam);
      throw InvalidDataException(message);
    }

  }
  else
  {

    CString message;

    if (lastParameter == "p")
      m_sPassword = lpszParam;
    else if (lastParameter == "u")
      m_sLogin = lpszParam;
    else if (lastParameter == "n")
      m_sName = lpszParam;
    else if (lastParameter == "r")
      m_sRole = lpszParam;
    else if (lastParameter == "chgpwd")
      m_bChangePassword = (strncmp(lpszParam, "0", 1) == 0 ? false : true);
    else if (lastParameter == "msg")
      m_bDisplayMessages = (strncmp(lpszParam, "1", 1) == 0 ? true : false);
    else
    {
      message.Format("Invalid parameter %s, cannot specify multiple text patterns", lpszParam);
      throw InvalidDataException(message);
    }

    lastParameter = "";



  }


  // Call the base class to ensure proper command line processing
  CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
}

