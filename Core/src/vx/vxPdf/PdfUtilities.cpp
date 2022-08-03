// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PdfUtilities.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "stdafx.h"
#include "PdfUtilities.h"
#include "PdfExceptions.h"


using namespace pdf;

/**
 * Gets the encoder class id
 *
 * @param format      Image format
 *                      image/bmp 
 *                      image/jpeg 
 *                      image/gif 
 *                      image/tiff 
 *                      image/png 
 *
 * @param pClsid      Class id to be filled in
 */
void PdfUtilities::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::GetImageEncodersSize(&num, &size);
   if( size == 0 )
     throw PdfException(LogRec("Failed to get the encoder size, make sure the Gdiplus has been started","PdfUtilities","GetEncoderClsid"));


   Gdiplus::ImageCodecInfo * pImageCodecInfo((Gdiplus::ImageCodecInfo*)(malloc(size)));
   if( pImageCodecInfo == __nullptr )
      throw PdfException(LogRec("Failed to allocated codec info","PdfUtilities","GetEncoderClsid"));

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return;  
      }    
   }

   free(pImageCodecInfo);

   throw PdfException(LogRec("Failed to find specified format","PdfUtilities","GetEncoderClsid"));
} // GetEncoderClsid(const WCHAR* format, CLSID* pClsid)


/**
 * Converts the guid to a string
 *
 *  @param sString
 *  @return the wide string
 */
std::wstring PdfUtilities::ToWString(const std::string & sString)
{
  std::wstring sWide;

  for ( std::string::const_iterator it = sString.cbegin(); it != sString.cend(); it++ )
  {
    wchar_t wch = static_cast<wchar_t>(*it);
    sWide.push_back(wch);
  }

  return sWide;
} // ToWString(const std::string & sString)


/**
 * Converts std::string to wchar_t
 *
 *  @param sString
 *  @return the wchar_t string
 */
wchar_t * PdfUtilities::ToWChar(const std::string & sString)
{  

	std::string sss = sString;


  const size_t bufferSize(sss.size() + 1);
  wchar_t * psWide = new wchar_t[bufferSize];
	if( psWide == nullptr )
  {
    LogErr("Failed to allocate wide buffer for conversion", "PdfUtilities", "ToWChar");
    return nullptr;
  }
  wmemset(psWide, 0, bufferSize);

  size_t size;
	errno_t error = mbstowcs_s(&size, psWide, bufferSize, sss.c_str(), sss.size() );

  return psWide;
} // ToWChar(const std::string & sString)

