// $Id: ImportLibraries.cpp,v 1.4 2006/10/09 14:24:01 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (george@viatronix.com)

#include "stdafx.h"

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#pragma comment( lib, "gdiplus.lib" )




#if defined(_WIN64) | defined(WIN64)

  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolumed.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxCADd.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBased.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )  
   

  
  #else

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxCAD.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBase.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )  

  
  #endif

#else
  
 
  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume32d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxCAD32d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBase32d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger32d.lib" )  

  
  #else

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume32.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxCAD32.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBase32.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger32.lib" )  
  
  #endif

#endif


// $Log: ImportLibraries.cpp,v $
// Revision 1.4  2006/10/09 14:24:01  romy
// gdiplus added for the graph
//
// Revision 1.3  2006/06/02 13:00:34  geconomos
// added headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/ImportLibraries.cpp,v 1.4 2006/10/09 14:24:01 romy Exp $
// $Id: ImportLibraries.cpp,v 1.4 2006/10/09 14:24:01 romy Exp $

