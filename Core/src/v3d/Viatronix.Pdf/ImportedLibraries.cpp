#include "Stdafx.h"

#pragma comment( lib, "tdh.lib")
#pragma comment( lib, "gdiplus.lib" )
#pragma comment( lib, "Ws2_32.lib" )

#ifdef _DEBUG

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxpdfd.lib" )

#else

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxpdf.lib" )

#endif
