// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: ImportLibraries.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#include "stdafx.h"


#pragma comment(lib, "tdh.lib")
#pragma comment(lib, "ws2_32.lib")  

#if defined(WIN64) || defined(_WIN64)

#ifdef _DEBUG
 #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libiconv\\lib\\libiconv64d.lib" )
//pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml64d.lib" )
 #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml2_a" )
#else
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libiconv\\lib\\libiconv64.lib" )
  //#pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml2_a" )
#endif

#else

#ifdef _DEBUG
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libiconv\\lib\\libiconv32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml32d.lib" )
#else
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libiconv\\lib\\libiconv32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libxml\\lib\\libxml32.lib" )
#endif

#endif