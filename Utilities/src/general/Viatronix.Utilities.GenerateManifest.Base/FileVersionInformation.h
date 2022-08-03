// pragmas
#pragma once

// namespaces
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;

OPEN_GENMANIFESTBASE_NS

/**
 * Provides version information about a file.
 */
public ref class FileVersionInformation
{
// construction
public:
  
  /// Constructor.
  FileVersionInformation(String ^ mpFilename);

// properties
public:

  /// Gets or sets the filename.
  property String ^ Filename
  {
    String ^ get() { return m_mpFilename; }
    void set(String ^ mpValue) { m_mpFilename = mpValue; }
  } // Filename


  /// Gets or sets the file version.
  property String ^ FileVersion
  {
    String ^ get() { return m_mpFileVersion; }
    void set(String ^ mpValue) { m_mpFileVersion = mpValue; }
  } // FileVersion


  /// Gets or sets the product version.
  property String ^ ProductVersion
  {
    String ^ get() { return m_mpProductVersion; }
    void set(String ^ mpValue) { m_mpProductVersion = mpValue; }
  } // ProductVersion


  /// Gets or sets the Pdf Keywords.
  property String ^ PdfKeywords
  {
    String ^ get() { return m_mpPdfKeywords; }
    void set(String ^ mpValue) { m_mpPdfKeywords = mpValue; }
  } // PdfKeywords


  /// Gets if the file has ANY version information.
  property bool IsVersioned
  {
    bool get() { return !String::IsNullOrEmpty(FileVersion) || !String::IsNullOrEmpty(ProductVersion) || !String::IsNullOrEmpty(PdfKeywords); }
  } // IsVersioned

// fields
private:

  /// The filename.
  String ^ m_mpFilename;

  /// The file version.
  String ^ m_mpFileVersion;

  /// The product version.
  String ^ m_mpProductVersion;

  /// The pdf keywords.
  String ^ m_mpPdfKeywords;

}; // ref class FileVersionInformation

CLOSE_GENMANIFESTBASE_NS
