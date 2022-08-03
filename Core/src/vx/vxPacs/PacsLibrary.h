// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PacsLibrary.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


#include <string>
#include <map>
#include <vector>
#include <comdef.h>
#include <gdiplus.h>


#include "Logger.h"
#include "vxDicomLibrary.h"


#ifdef PACS_BUILD
#define PACS_DLL __declspec(dllexport)
#else
#define PACS_DLL __declspec(dllimport)
#endif