#include "stdafx.h"
#include "resource.h"

[ module( EXE, name = "DummyCadPlugin",  resource_name = "IDR_DUMMYCADPLUGIN", 
  uuid = "{04C5E9B1-2B6E-4ADC-B83A-F494E5B887A8}" ) // TODO: Replace with your own guid
]
class PluginModule
{
public:
  HRESULT RegisterClassObjects(DWORD dwClsContext, DWORD dwFlags) throw()
  {
    dwFlags &= ~REGCLS_MULTIPLEUSE;
    dwFlags |= REGCLS_SINGLEUSE;
    return __super::RegisterClassObjects( dwClsContext, dwFlags );
  }
}; // class PluginModule

/// import the viatronix com cad idl file
[importlib( "vxCOMCad.tlb" )];
		 
