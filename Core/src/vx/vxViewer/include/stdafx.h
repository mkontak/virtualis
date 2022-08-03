#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#include "Logger.h"
#include "vxViewerLibrary.h"



inline HMODULE GetModuleHandle()
{
  static int dummy = 0;
  
  MEMORY_BASIC_INFORMATION mbi;
  VirtualQuery( &dummy, &mbi, sizeof(mbi) );
  
  return reinterpret_cast<HMODULE>( mbi.AllocationBase );
}


#include <wglext.h>
#include <atlbase.h>
#include <atlwin.h>