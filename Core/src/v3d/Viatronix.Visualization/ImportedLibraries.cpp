// $Id: ImportedLibraries.cpp,v 1.4 2006/06/02 13:00:34 geconomos Exp $
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
#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "Netapi32.lib" )


  

  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\minilzo\\lib\\minilzo64d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBased.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolumed.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxViewerd.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxCADd.lib" )
  
  #else

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\minilzo\\lib\\minilzo64.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBase.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxViewer.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxCAD.lib" )
  
  #endif




// $Log: ImportedLibraries.cpp,v $
// Revision 1.4  2006/06/02 13:00:34  geconomos
// added headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/ImportedLibraries.cpp,v 1.4 2006/06/02 13:00:34 geconomos Exp $
// $Id: ImportedLibraries.cpp,v 1.4 2006/06/02 13:00:34 geconomos Exp $
