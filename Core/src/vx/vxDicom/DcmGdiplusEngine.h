
#pragma once

#include <GdiPlus.h>

namespace dcm
{

/**
 * Class to manage the GDI plus engine
 */
class DCM_DLL DcmGdiplusEngine
{
// member functions
public:
  
  /// default constructor
  DcmGdiplusEngine();
  
  /// destructor
  ~DcmGdiplusEngine();

  /// Gets the encoder
  static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

// data members
private:
  
  /// Gdiplus initialization token
  ULONG_PTR m_pToken;

}; // class DcmGdiplusEngine

}
