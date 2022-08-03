// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include <string>

#include "Logger.h"
#include "vxBaseLibrary.h"
#include "vxBlockVolumeLibrary.h"

#define OPEN_IMPBASE_NS  namespace Viatronix { namespace Utilities { namespace Importer { namespace Base {
#define CLOSE_IMPBASE_NS } } } }
#define USING_IMPBASE_NS using namespace Viatronix::Utilities::Importer::Base;

using namespace System::Runtime::InteropServices;

/*
 * Converts a mananged System::String to a std::string 
 *
 * @param string      String to convert to an unmanaged std::string
 *
 * @returns Returns the std::string represenetatioon of the String supplied.
 */
inline const std::string ss( System::String ^ string )
{
  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );
  std::string converted = ( const char* ) buffer.ToPointer();
  Marshal::FreeHGlobal( buffer );
  return converted;
} // ss()
