#pragma once

#include <WinSock2.h>
#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <exception>
#include <functional>
#include <comutil.h>
#include <comdef.h>
#include <unordered_map>

#include "Logger.h"
#include "vxBaseLibrary.h"
#include "vxBlockVolumeLibrary.h"
#include "vxDicomLibrary.h"


#ifdef CONVERSION_BUILD
  #define CVN_DLL   __declspec( dllexport )
#else
  #define CVN_DLL   __declspec( dllimport )
#endif