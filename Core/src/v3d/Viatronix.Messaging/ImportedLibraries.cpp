/// includes
#include "stdafx.h"

#pragma comment( lib, "gdiplus.lib" )
#pragma comment( lib, "Ws2_32.lib" )


#if defined(WIN64) || defined(_WIN64) 

#ifdef _DEBUG

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libiconv\\lib\\libiconv64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxMessagingd.lib" )

#else


  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libiconv\\lib\\libiconv64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxMessaging.lib" )


#endif

#else

#ifdef _DEBUG

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libiconv\\lib\\libiconv32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxMessaging32d.lib" )

#else


  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libiconv\\lib\\libiconv32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxMessaging32.lib" )

#endif


#endif