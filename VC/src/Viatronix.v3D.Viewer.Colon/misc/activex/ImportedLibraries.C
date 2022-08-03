// $Id: ImportedLibraries.C,v 1.2 2007/02/20 14:35:40 geconomos Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
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

#ifdef _WIN64

  #pragma comment( lib, "..\\..\\..\\..\\libs\\minilzo\\lib\\minilzo64.lib" )

  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxLogger64d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxBase64d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxBlockVolume64d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxViewer64d.lib" )
  
  #else

    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxLogger64.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxBase64.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxBlockVolume64.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxViewer64.lib" )
  
  #endif

#else
  
  #pragma comment( lib, "..\\..\\..\\..\\libs\\minilzo\\lib\\minilzo32.lib" )

  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxLogger32d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxBase32d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxBlockVolume32d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxCAD32d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxColon32d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxViewer32d.lib" )

  
  #else

    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxLogger32.lib" )  
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxBase32.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxBlockVolume32.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxCAD32.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxColon32.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\libs\\vx\\lib\\vxViewer32.lib" )
  
  #endif

#endif


// $Log: ImportedLibraries.C,v $
// Revision 1.2  2007/02/20 14:35:40  geconomos
// including logging
//
// Revision 1.1  2007/02/16 14:43:41  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/misc/activex/ImportedLibraries.C,v 1.2 2007/02/20 14:35:40 geconomos Exp $
// $Id: ImportedLibraries.C,v 1.2 2007/02/20 14:35:40 geconomos Exp $
