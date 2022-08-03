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
#include "LaunchRequest.h"



class CommandLineInfo : public CCommandLineInfo
{

public:
  CommandLineInfo() : m_bNoMessageWindows(false) { }


  bool IsParametersSpecified();

public:


  bool NoMessageWindows() { return m_bNoMessageWindows;  }

  LaunchRequest & GetLaunchRequest() { return m_request; }


public:


  void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

private:

  LaunchRequest m_request;

  bool m_bNoMessageWindows;
};