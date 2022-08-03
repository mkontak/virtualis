//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] (mailto:mkontak@viatronix.com)

/// includes
#include "stdafx.h"

#pragma comment( lib, "gdiplus.lib" )
#pragma comment( lib, "Ws2_32.lib" )


#if defined(WIN64) || defined(_WIN64)
#ifdef _DEBUG

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdata64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdsig64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmimage64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmimgle64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmjpeg64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmnet64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmpstat64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmqrdb64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmsr64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmtls64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmwlm64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ijg864d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ijg1264d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ijg1664d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ofstd64d.lib" )

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\oflog64d.lib" )


  //#pragma comment( lib, "..\\..\\..\\..\\Libraries\\libtiff\\lib\\libtiff64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libpng\\lib\\libpng64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libjpeg\\lib\\libjpeg64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\zlib\\lib\\zlib64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libiconv\\lib\\libiconv64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml64d.lib" )
  
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\openjpeg\\lib\\openjp2.lib" )
 




  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxDicomd.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBased.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxPacsd.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxPdfd.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxMessagingd.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxConversiond.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolumed.lib" )

#else

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdata64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdsig64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmimage64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmimgle64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmjpeg64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmnet64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmpstat64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmqrdb64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmsr64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmtls64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmwlm64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ijg864.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ijg1264.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ijg1664.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ofstd64.lib" )

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdata64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\oflog64.lib" )

  //#pragma comment( lib, "..\\..\\..\\..\\Libraries\\libtiff\\lib\\libtiff64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libpng\\lib\\libpng64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libjpeg\\lib\\libjpeg64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\zlib\\lib\\zlib64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libiconv\\lib\\libiconv64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml64.lib" )

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxDicom.lib" )

#endif

#else

#ifdef _DEBUG

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdata32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdsig32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmimage32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmimgle32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmjpeg32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmnet32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmpstat32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmqrdb32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmsr32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmtls32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmwlm32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ijg832d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ijg1232d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ijg1632d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ofstd32d.lib" )


  //#pragma comment( lib, "..\\..\\..\\..\\Libraries\\libtiff\\lib\\libtiff32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libpng\\lib\\libpng32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libjpeg\\lib\\libjpeg32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\zlib\\lib\\zlib32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libiconv\\lib\\libiconv32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml32d.lib" )
  
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxDicom32d.lib" )

#else

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdata32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdsig32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmimage32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmimgle32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmjpeg32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmnet32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmpstat32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmqrdb32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmsr32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmtls32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmwlm32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ijg832.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ijg1232.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ijg1632.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ofstd32.lib" )


  //#pragma comment( lib, "..\\..\\..\\..\\Libraries\\libtiff\\lib\\libtiff32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libpng\\lib\\libpng32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libjpeg\\lib\\libjpeg32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\zlib\\lib\\zlib32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libiconv\\lib\\libiconv32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml32.lib" )

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxDicom32.lib" )

#endif

#endif