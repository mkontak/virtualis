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


/**
* Parses the command line
*/
void CommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
	static CString lastParameter("");

	if (bFlag)
	{

		lastParameter = lpszParam;

		if (lastParameter == "d")
		{
			m_bDecrypt = true; 
			m_bEncrypt = false;
			m_bParametersSpecified = true;

		}
		else if (lastParameter == "?" || lastParameter == "help")
		{
			CString message("\r\nEncrypt [-d] [-p PASSWORD] [-f FILE] text\r\n");

			message.Append("\r\nwhere:\r\n\r\n");
			message.Append("[-d]                    Indicates that we are to decrypt the text, requires PASSWORD\r\n");
      message.Append("[-p PASSWORD]           Password required to decrypt\r\n");
      message.Append("[-f FILE]               Specifies file to write results to\r\n");


			throw UsageException(message);
		}
    else if (lastParameter != "d" && lastParameter != "p" && lastParameter != "f")
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
		{
			m_sPassword = lpszParam;
			m_bParametersSpecified = true;
		}
    else if (lastParameter == "f")
    {
      m_sFilePath = lpszParam;
      m_bParametersSpecified = true;
    }
		else
		{

      if (!m_sText.IsEmpty())
      {
        message.Format("Invalid parameter %s, cannot specify multiple text patterns", lpszParam);
        throw InvalidDataException(message);
      }

      m_sText = lpszParam;
      m_bParametersSpecified = true;

		}

    lastParameter = "";



	}


	// Call the base class to ensure proper command line processing
	CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
}

