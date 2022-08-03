// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once



#include <string>
#include <exception>


#include "Logger.h"


#ifdef HTTP_BUILD
#define HTTP_DLL __declspec(dllexport)
#else
#define HTTP_DLL __declspec(dllimport)
#endif