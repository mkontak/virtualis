// $Id: ImportedLibraries.C,v 1.1 2007/03/01 15:45:14 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:george@viatronix.com)

#include "stdafx.h"


#pragma comment( lib, "gdiplus.lib" )
#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "crypt32.lib")
#pragma comment( lib, "wbemuuid.lib")
#pragma comment( lib, "WinHttp.lib" )
#pragma comment ( lib, "msi.lib" )

#if defined(_WIN64 ) || defined(WIN64)


  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\minilzo\\lib\\minilzo64d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\zlib\\lib\\zlib64d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\xerces\\lib\\xerces64d.lib" )

  #else

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\minilzo\\lib\\minilzo64.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\zlib\\lib\\zlib64.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\xerces\\lib\\xerces64.lib" )

  #endif

#else

   #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\xerces\\lib\\xerces32.lib" )


   #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\minilzo\\lib\\minilzo32d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\zlib\\lib\\zlib32d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger32d.lib" )

  #else

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\minilzo\\lib\\minilzo32.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\zlib\\lib\\zlib32.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger32.lib" )

  #endif

#endif




// $Log: ImportedLibraries.C,v $
// Revision 1.1  2007/03/01 15:45:14  geconomos
// changed extension from .cpp to .C
//
// Revision 1.3  2006/06/02 13:00:34  geconomos
// added headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/ImportedLibraries.C,v 1.1 2007/03/01 15:45:14 geconomos Exp $
// $Id: ImportedLibraries.C,v 1.1 2007/03/01 15:45:14 geconomos Exp $
