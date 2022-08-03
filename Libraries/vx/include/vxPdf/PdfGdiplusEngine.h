
#pragma once

#include <GdiPlus.h>

namespace pdf
{

  /**
  * Class to manage the GDI plus engine
  */
  class PdfGdiplusEngine
  {
    // member functions
  public:

    /// default constructor
    PdfGdiplusEngine();

    /// destructor
    ~PdfGdiplusEngine();

    /// Gets the encoder
    static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

    // data members
  private:

    /// Gdiplus initialization token
    ULONG_PTR m_pToken;

  }; // class PdfGdiplusEngine

}
