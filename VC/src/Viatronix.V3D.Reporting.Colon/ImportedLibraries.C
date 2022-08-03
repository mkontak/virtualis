// $Id: ImportedLibraries.C,v 1.1 2006/06/02 15:04:03 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"


// imports for all configurations
#pragma comment( lib, "gdiplus.lib" )


#ifdef WIN64

  #pragma comment( lib, "..\\..\\..\\Libraries\\minilzo\\lib\\minilzo64d.lib" )
  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\Libraries\\zlib\\lib\\zlib64d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )  
    #pragma comment( lib, "..\\..\\..\\Libraries\\xerces\\lib\\xerces64d.lib" )


  #else

    #pragma comment( lib, "..\\..\\..\\Libraries\\zlib\\lib\\zlib64.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )  
    #pragma comment( lib, "..\\..\\..\\Libraries\\xerces\\lib\\xerces64.lib" )

  #endif

#else
  
  #pragma comment( lib, "..\\..\\..\\Libraries\\libs\\minilzo\\lib\\minilzo32d.lib" )
  #pragma comment( lib, "..\\..\\..\\Libraries\\libs\\xerces\\lib\\xerces32.lib" )

  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\Libraries\\libs\\zlib\\lib\\zlib32d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxLogger32d.lib" )  
  
  #else

    #pragma comment( lib, "..\\..\\..\\Libraries\\libs\\zlib\\lib\\zlib32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxLogger32.lib" )  
   
  #endif

#endif

// $Log: ImportedLibraries.C,v $
// Revision 1.1  2006/06/02 15:04:03  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ImportedLibraries.C,v 1.1 2006/06/02 15:04:03 frank Exp $
// $Id: ImportedLibraries.C,v 1.1 2006/06/02 15:04:03 frank Exp $
