#pragma once

// namespaces
OPEN_MANIFEST_NS
using namespace System;
using namespace System::Collections::Generic;

/**
 * A class to read the file versions in a directory.
 */
public ref class FileVersionReader
{
// construction
public:

  /// Constructor.
  FileVersionReader();

// properties
public:

  /// Gets a mapping from the filename to the version string.
  property List<Tuple<String ^, String ^, String ^> ^> ^ FileVersions
  {
    List<Tuple<String ^, String ^, String ^> ^> ^ get() { return m_mpFileVersions; }
  } // FileVersions

// methods
public:

  /// Reads the directory
  void ReadDirectory(String ^ mpInputDirectory);

  /// Writes out the manifest.
  void WriteManifest(String ^ mpInputDirectory, String ^ mpOutputFile);

// fields
private:

  /// The list of files mapped with their versions.
  List<Tuple<String ^, String ^, String ^> ^> ^ m_mpFileVersions;

}; // class FileVersionReader

// namespaces
CLOSE_MANIFEST_NS