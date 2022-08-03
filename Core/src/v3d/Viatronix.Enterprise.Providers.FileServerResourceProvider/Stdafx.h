// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include <WinSock2.h>
#include <Windows.h> 

#include "Logger.h"
#include "vxBaseLibrary.h"

#include <string>


#pragma warning(disable : 4251)
#pragma warning(disable : 4482)



#define USING_PROVIDER_NS   using namespace Viatronix::Enterprise::Providers;
#define OPEN_PROVIDERS_NS  namespace Viatronix { namespace Enterprise { namespace Providers {
#define CLOSE_PROVIDERS_NS  } } };




/*
 * Converts a managed System::String to a std::string 
 *
 * @param string      String to convert to an unmanaged std::string
 *
 * @returns Returns the std::string representation of the String supplied.
 */
inline const std::string ss( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;

  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );

  std::string converted = ( const char* ) buffer.ToPointer();
	
  Marshal::FreeHGlobal( buffer );

  return converted;
} // ss()

