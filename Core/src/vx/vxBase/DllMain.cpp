#include "stdafx.h"
#include "vxDOM.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason, LPVOID lpvReserved )
{

  if ( fdwReason == DLL_PROCESS_ATTACH )
    ;//vxDOMDocument::Initialize();
  else if ( fdwReason == DLL_PROCESS_DETACH )
    ;//vxDOMDocument::Terminate();

  return TRUE;
}
