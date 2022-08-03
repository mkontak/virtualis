#pragma once

#define _CRT_SECURE_NO_DEPRECATE


#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#define _ATL_APARTMENT_THREADED



#include <atlbase.h>
extern CComModule _Module;

#include <atlcom.h>
#include <atldef.h>
#include <comdef.h>
#include <gdiplus.h>


#include "Logger.h"
#include "vxBaseLibrary.h"






#include "ReportLibGlobals.h"
#include "Vector.h"
#include "Point.h"