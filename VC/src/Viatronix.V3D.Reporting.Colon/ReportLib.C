#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "ReportLib.h"
#include "ReportLib_i.c"
#include "Image.h"
#include "Report.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_Image, Image)
OBJECT_ENTRY(CLSID_Report, Report)
END_OBJECT_MAP()

/**
 * DLL Entry Point
 *
 * @param   hInstance       handle to dll
 * @param   dwReason        why we all calling this function
 * @param   lpReserved      reserved
 * @return                  TRUE upon success
 */
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD_PTR dwReason, LPVOID /*lpReserved*/)
{
  if (dwReason == DLL_PROCESS_ATTACH)
  {
    _Module.Init(ObjectMap, hInstance, &LIBID_ReportLib);
    DisableThreadLibraryCalls(hInstance);
  }
  else if (dwReason == DLL_PROCESS_DETACH)
  {
    _Module.Term();
  }

  return TRUE;
}

/**
 *  used to determine whether the DLL can be unloaded by OLE
 */
STDAPI DllCanUnloadNow(void)
{
  return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/**
 * returns a class factory to create an object of the requested type
 *
 * @param   rclsid      CLSID for the class object
 * @param   riid        identifier of the interface that communicates with the class object
 * @param   ppv         receives the interface pointer requested in riid 
 * @return              S_OK upon success
 */
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/**
 *  adds entries to the system registry
 *
 * @return S_OK upon success
 */
STDAPI DllRegisterServer(void)
{
  // registers object, typelib and all interfaces in typelib
  return _Module.RegisterServer(TRUE);
}


/**
 *  removes entries from the system registry
 *
 * @return S_OK upon success
*/
STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}