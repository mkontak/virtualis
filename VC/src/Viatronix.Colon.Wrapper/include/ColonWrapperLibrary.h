#pragma once

#ifdef VX_WRAPPER_BUILD
#   define  VX_WRAPPER_DLL   __declspec( dllexport )
#elif defined(VX_TEST)
#   define  VX_WRAPPER_DLL
#else
#   define  VX_WRAPPER_DLL   __declspec( dllimport )
#endif

#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x0501
#endif


#define OPEN_VC_NS namespace Viatronix { namespace Colon {  
#define CLOSE_VC_NS } }
#define USING_VC_NS using namespace Viatronix::Colon;