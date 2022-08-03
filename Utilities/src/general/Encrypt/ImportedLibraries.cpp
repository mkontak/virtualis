// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ImportedLibraries.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "stdafx.h"


#pragma comment( lib, "gdiplus.lib" )
#pragma comment( lib, "Ws2_32.lib" )
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "wbemuuid.lib")



#ifdef _DEBUG


#pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
#pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBased.lib" )

#else


#pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
#pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBase.lib" )

#endif