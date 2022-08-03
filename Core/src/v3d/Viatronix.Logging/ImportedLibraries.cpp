/// includes
#include "stdafx.h"



#pragma comment(lib, "tdh.lib")
#pragma comment(lib, "ws2_32.lib")  


#ifdef _DEBUG

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
 
#else

   #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
 
#endif