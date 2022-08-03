#include "Stdafx.h"
#include "DcmGdiplusEngine.h"
#include <GdiPlus.h>


using namespace dcm;

/**
 * Default constructor
 */
DcmGdiplusEngine::DcmGdiplusEngine()
{
  Gdiplus::GdiplusStartupInput gsi;
  Gdiplus::GdiplusStartup( & m_pToken, & gsi, NULL );
} 
  
/// destructor
DcmGdiplusEngine::~DcmGdiplusEngine()
{
  Gdiplus::GdiplusShutdown(m_pToken);
}

/**
 * Gets the encoder class id using the format specified
 *
 * @param format      Format
 *                            image/bmp 
 *                            image/jpeg 
 *                            image/gif 
 *                            image/tiff 
 *                            image/png 
 * 
 * @param pClsid      Parameter to be filled in with the class id
 *
 * @return  -1 if failed, 0 otherwise
 */
int DcmGdiplusEngine::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

   Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}
