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

    if (lastParameter == "nomsg")
      m_bNoMessageWindows = true;
    else if (lastParameter == "?" || lastParameter == "help")
    {
      CString message("\r\nRemoteLauncher <-app APPLICATION> <-server SERVER> <-port PORT> <-user USERNAME> [-pwd PASSWORD] \r\n");
      message.Append( "             [-desc DESCRIPTION] [-cid CONTEXT][-new 1 or 0][-nomsg]\r\n");

      message.Append("\r\nwhere:\r\n\r\n");
      message.Append("<-app APPLICATION>         Specifies the application to be launched ( VC, CAR, ... )\r\n");
      message.Append("<-server SERVER>           Specifies the name of the server running the V3D Viewer Service\r\n");
      message.Append("<-port PORT>               Specifies the port the service is listening on\r\n");
      message.Append("<-user USERNAME>           Specifies either a valid V3D user name or and alias\r\n");
      message.Append("[-pwd PASSWORD]            Required only if the -user is a V3D User\r\n");
      message.Append("[-desc DESCRIPTION]        Allows the session description to be specified, only used if a new session is created\r\n");
      message.Append("[-cid CONTEXT]             Specifies a V3D Authentication context token (Not Used)\r\n");
      message.Append("[-new 1 or 0]              If 1 then a new session is created. If 0 a new session is only created if one does not already exists (default 0)\r\n");
      message.Append("[-nomsg]                   Indicates that no Message Boxes are to be displayed\r\n");



      throw UsageException(message);
    }
    else if (lastParameter != "uids" && lastParameter != "server" &&
              lastParameter != "app" && lastParameter != "port" &&
              lastParameter != "user" && lastParameter != "pwd" &&  
              lastParameter != "cid" &&  lastParameter != "new" &&
              lastParameter != "desc" )
    { 
      CString message;
      message.Format("Invalid parameter %s", lpszParam);
      throw InvalidDataException(message);
    }
  }
  else
  {

    CString message;

    if (lastParameter == "uids")
      m_request.SetUids(lpszParam);
    else if (lastParameter == "server")
      m_request.SetServer(lpszParam);
    else if (lastParameter == "app")
      m_request.SetApplication(lpszParam);
    else if (lastParameter == "port")
      m_request.SetPort((unsigned int)atoi(lpszParam));
    else if (lastParameter == "user")
      m_request.SetUserName(lpszParam);
    else if (lastParameter == "pwd")
      m_request.SetPassword(lpszParam);
    else if (lastParameter == "desc")
      m_request.SetDescription(lpszParam);
    else if (lastParameter == "context")
      m_request.SetContext(lpszParam);
    else if (lastParameter == "new")
      m_request.SetNewSession(strncmp(lpszParam, "1", 1) == 0);
    else
    {
      message.Format("Invalid parameter %s", lpszParam);
      throw InvalidDataException(message);
    }



  }


  // Call the base class to ensure proper command line processing
  CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
}


/**
 * Determines if parameters were specified
 */
bool CommandLineInfo::IsParametersSpecified()
{
  return (! m_request.IsEmpty() );
}