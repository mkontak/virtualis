// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: stdafx.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once

#define _CRT_SECURE_NO_DEPRECATE


#pragma warning(disable:4251)
#pragma warning(disable:4482)

#include <algorithm>
#include "Logger.h"


#define FILE_REVISION "1.0"

#ifdef UNICODE
  typedef std::wstring  STRING;
  typedef std::wostringstream STRINGSTREAM;
#else
  typedef std::string   STRING;
  typedef std::ostringstream STRINGSTREAM;
#endif


