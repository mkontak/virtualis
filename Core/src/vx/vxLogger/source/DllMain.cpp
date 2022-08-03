#include "stdafx.h"
#include "Logger.h"

/** 
 * DllMain
 * @param module sHandle
 * @param calling reason
 * @param unused
 * @return true/false
 */
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  dwReason, LPVOID lpReserved )
{

    switch (dwReason)
   {
     case DLL_PROCESS_ATTACH:
       {
        xmlInitParser();
       }
       break;
     case DLL_PROCESS_DETACH:
       {
        xmlCleanupParser();
        vxLogFactory::ReleaseLoggers();
       }
       break;
     case DLL_THREAD_ATTACH:
       break;
     case DLL_THREAD_DETACH:
       break;
   }
   return TRUE;


} // DllMain()
