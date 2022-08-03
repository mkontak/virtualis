/// includes
#include "stdafx.h"

#pragma comment( lib, "gdiplus.lib" )

#ifdef _DEBUG

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBased.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolumed.lib" )
 
#else

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBase.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume.lib" )
 
#endif