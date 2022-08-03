

#include "stdafx.h"


#pragma comment( lib, "gdiplus.lib" )
#pragma comment( lib, "Ws2_32.lib" )

#ifdef _WIN64

  //#pragma comment( lib, "..\\..\\libs\\minilzo\\lib\\minilzo64.lib" )
  //#pragma comment( lib, "..\\..\\libs\\zlib\\lib\\zlib64.lib" )

  #ifdef _DEBUG


  #else


  #endif

#else
  
  #pragma comment( lib, "..\\..\\libs\\minilzo\\lib\\minilzo32.lib" )
  #pragma comment( lib, "..\\..\\libs\\zlib\\lib\\zlib32.lib" )

  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\libs\\v3d\\lib\\vxLogger32d.lib" )
    #pragma comment( lib, "..\\..\\libs\\v3d\\lib\\vxBase32d.lib" )
    #pragma comment( lib, "..\\..\\libs\\v3d\\lib\\vxBlockVolume64d.lib" )

  #else

    #pragma comment( lib, "..\\..\\libs\\v3d\\lib\\vxLogger32.lib" )  
    #pragma comment( lib, "..\\..\\libs\\v3d\\lib\\vxBase32.lib" )
    #pragma comment( lib, "..\\..\\libs\\v3d\\lib\\vxBlockVolume32.lib" )

  #endif

#endif

