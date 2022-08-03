// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: ImportedLibraries.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)



/// includes
#include "stdafx.h"

#pragma comment( lib, "Ws2_32.lib" )


#if defined(WIN64) || defined(_WIN64)
#ifdef _DEBUG

  
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )


#else

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )


#endif

#else

#ifdef _DEBUG

  
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger32d.lib" )


#else

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger32.lib" )

#endif

#endif