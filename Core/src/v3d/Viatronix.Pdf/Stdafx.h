// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

// pragmas
#pragma once

// includes
#include "Logger.h"
#include "vxPdfLibrary.h"

// defines
#define OPEN_PDF_NS namespace Viatronix { namespace Pdf {
#define CLOSE_PDF_NS } }
#define USING_PDF_NS using namespace Viatronix::Pdf;

/**
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

/**
 * Converts a managed System::String to a std::string 
 *
 * @param string      String to convert to an unmanaged std::string
 *
 * @returns Returns the std::string representation of the String supplied.
 */
inline const std::wstring ws( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;

  System::IntPtr buffer = Marshal::StringToHGlobalUni( string );

  std::wstring converted = ( const wchar_t * ) buffer.ToPointer();
	
  Marshal::FreeHGlobal( buffer );

  return converted;
} // sws()
