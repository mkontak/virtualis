// pragmas
#pragma once

// namespaces
using namespace System;

OPEN_GENMANIFESTBASE_NS

/**
 * Reads the version information from the file.
 */
public ref class VersionInfo
{
// methods
public:

  /// Retrieves file version information for the specified file.
  static Tuple<String ^, String ^> ^ RetrieveVersionInformation(String ^ mpFilename);

}; // class VersionInfo

CLOSE_GENMANIFESTBASE_NS
