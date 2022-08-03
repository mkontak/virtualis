// $Id: ImportedLibraries.C,v 1.3 2007/02/27 19:01:02 jmeade Exp $
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
#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#pragma comment( lib, "Vfw32.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "Version.lib" )


// 64-bit
#if defined(_WIN64 ) || defined(WIN64)

  

  // 64-bit debug
  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\Libraries\\minilzo\\lib\\minilzo64d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\zlib\\lib\\zlib64d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBased.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolumed.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxViewerd.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxColond.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxCADd.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\zlib\\lib\\zlib64d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\libpng\\lib\\libpng64d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxPdfd.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxPacsd.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxDicomd.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\xerces\\lib\\xerces64d.lib" )

  // 64-bit release
  #else

    #pragma comment( lib, "..\\..\\..\\Libraries\\minilzo\\lib\\minilzo64.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\zlib\\lib\\zlib64.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBase.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxViewer.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxColon.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxCAD.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\libpng\\lib\\libpng64.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxPdf.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxPacs.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxDicom.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\xerces\\lib\\xerces64.lib" )

  #endif

// 32-bit
#else
  
  #pragma comment( lib, "..\\..\\..\\Libraries\\Libraries\\xerces\\lib\\xerces32.lib" )

  // 64-bit debug
  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\Libraries\\Libraries\\minilzo\\lib\\minilzo32d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\Libraries\\zlib\\lib\\zlib32d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxLogger32d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBase32d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume32d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxViewer32d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxColon32d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxCAD32d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vc\\lib\\vxColonWrapperd.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxPdf32d.lib" )
  
  // 64-bit release
  #else

    #pragma comment( lib, "..\\..\\..\\Libraries\\Libraries\\minilzo\\lib\\minilzo32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\Libraries\\zlib\\lib\\zlib32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxLogger32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBase32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxViewer32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxColon32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxCAD32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vc\\lib\\vxColonWrapperd.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxPdf32.lib" )

  #endif

#endif

// $Log: ImportedLibraries.C,v $
// Revision 1.3  2007/02/27 19:01:02  jmeade
// comment blocks.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ImportedLibraries.C,v 1.3 2007/02/27 19:01:02 jmeade Exp $
// $Id: ImportedLibraries.C,v 1.3 2007/02/27 19:01:02 jmeade Exp $
