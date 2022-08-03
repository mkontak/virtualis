
#include "stdafx.h"


#pragma comment( lib, "gdiplus.lib" )

#if defined(_WIN64 ) || defined(WIN64)

  #pragma comment( lib, "..\\..\\..\\Libraries\\xerces\\lib\\xerces64.lib" )

  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxbased.lib" )  
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxloggerd.lib" )  
   
  #else

    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxbase.lib" )  
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxlogger.lib" )  
   
  #endif

#else

   #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\xerces\\lib\\xerces32.lib" )

  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxbase32d.lib" )  
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxlogger32d.lib" )  
   
  #else

    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxbase32.lib" )  
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxlogger32.lib" )  
   
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
