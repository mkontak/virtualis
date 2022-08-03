// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: MessagingLibrary.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#include <WinSock2.h>
#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <exception>
#include <functional>
#include <algorithm>
#include <memory>

#ifdef MSG_BUILD
#define MSG_DLL __declspec(dllexport)
#else
#define MSG_DLL __declspec(dllimport)
#endif