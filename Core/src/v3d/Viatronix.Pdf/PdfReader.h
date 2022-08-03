// pragmas
#pragma once

// includes
#include "scoped_ptr.h"

// namespaces
using namespace System;
using namespace System::Drawing;
using namespace System::Collections::Generic;

OPEN_PDF_NS

/**
 * A class to read a pdf document.
 */
public ref class PdfReader
{
// construction
private:

  /// Constructor
  PdfReader();

// properties
public:

  /// Gets the number of pages in the report.
  property int PageCount
  {
    int get();
  } // PageCount


  /// Gets the list of pages in the document.
  property cli::array<Bitmap ^> ^ Pages
  {
    cli::array<Bitmap ^> ^ get();
  } // Pages

// methods
public:

  /// Creates an instance from the specified file.
  static PdfReader ^ Create(String ^ mpFilename);

// fields
private:

  /// The object to wrap.
  scoped_ptr<std::shared_ptr<pdf::Pdf>> m_pdf;

}; // class PdfReader

CLOSE_PDF_NS