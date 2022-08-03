// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#include "logger.h"
#include "vxBaseLibrary.h"
#include "ColonWrapperLibrary.h"
#include <string>


/**
 * Converts a System::String to a std::string
 *
 * @param string input System::String *
 * @return converted std::string
 */
inline const std::string ss( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;

  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );

  std::string converted = ( const char * ) buffer.ToPointer();
	
  Marshal::FreeHGlobal( buffer );

  return converted;
}