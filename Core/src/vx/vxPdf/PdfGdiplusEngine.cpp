#include "Stdafx.h"
#include "PdfGdiplusEngine.h"
#include <GdiPlus.h>


using namespace pdf;

/**
* Default constructor
*/
PdfGdiplusEngine::PdfGdiplusEngine()
{
  Gdiplus::GdiplusStartupInput gsi;
  Gdiplus::GdiplusStartup(&m_pToken, &gsi, NULL);
}

/// destructor
PdfGdiplusEngine::~PdfGdiplusEngine()
{
  Gdiplus::GdiplusShutdown(m_pToken);
}


int PdfGdiplusEngine::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
  UINT  num = 0;          // number of image encoders
  UINT  size = 0;         // size of the image encoder array in bytes

  Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

  Gdiplus::GetImageEncodersSize(&num, &size);
  if (size == 0)
    return -1;  // Failure

  pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
  if (pImageCodecInfo == NULL)
    return -1;  // Failure

  Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

  for (UINT j = 0; j < num; ++j)
  {
    if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
    {
      *pClsid = pImageCodecInfo[j].Clsid;
      free(pImageCodecInfo);
      return j;  // Success
    }
  }

  free(pImageCodecInfo);
  return -1;  // Failure
}
