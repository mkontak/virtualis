#pragma once

#include "Windows.h"
#include <string>

#define OPEN_GENMANIFESTBASE_NS namespace Viatronix { namespace Utilities { namespace GenerateManifest { namespace Base {
#define CLOSE_GENMANIFESTBASE_NS } } } }
#define USING_GENMANIFESTBASE_NS using namespace Viatronix::Utilities::GenerateManifest::Base;

/**
 * Returns an unmanaged std::string
 * @param string
 */
inline const std::string GetUnManagedStdString( System::String ^ mpString )
{
  using namespace System::Runtime::InteropServices;

  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( mpString );

  std::string converted = ( const char* ) buffer.ToPointer();
	
  Marshal::FreeHGlobal( buffer );

  return converted;
} // GetUnManagedStdString()


/**
 * Gets the unmanaged string.
 */
inline const std::string ss( System::String ^ mpString )
{
  return GetUnManagedStdString( mpString );
} // ss( mpString )
