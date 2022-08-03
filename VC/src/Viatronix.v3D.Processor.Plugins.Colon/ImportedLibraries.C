// $Id: ImportedLibraries.C,v 1.3 2007/03/06 16:34:01 dongqing Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (george@viatronix.com)

#include "stdafx.h"


#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "Netapi32.lib" )
#pragma comment( lib, "opengl32.lib" )

#ifdef _WIN64


  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\Libraries\\minilzo\\lib\\minilzo64d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\zlib\\lib\\zlib64d.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBased.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolumed.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxViewerd.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxColond.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxMessagingd.lib" )
 
  #else

    #pragma comment( lib, "..\\..\\..\\Libraries\\minilzo\\lib\\minilzo64.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\zlib\\lib\\zlib64.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBase.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxViewer.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxColon.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxMessaging.lib" )

  #endif

#else

  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\Libraries\\libs\\minilzo\\lib\\minilzo32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\libs\\zlib\\lib\\zlib32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBased.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolumed.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxViewerd.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxColond.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxMessaging32d.lib" )

  
  #else

    #pragma comment( lib, "..\\..\\..\\Libraries\\minilzo\\lib\\minilzo32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\zlib\\lib\\zlib32.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )  
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBase.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxViewer.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxColon.lib" )
    #pragma comment( lib, "..\\..\\..\\Libraries\\vx\\lib\\vxMessaging32.lib" )
  
  #endif

#endif


// $Log: ImportedLibraries.C,v $
// Revision 1.3  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.2  2006/06/07 14:30:00  geconomos
// added reference to vxcolon.lib
//
// Revision 1.1  2006/06/02 19:01:38  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ImportedLibraries.C,v 1.3 2007/03/06 16:34:01 dongqing Exp $
// $Id: ImportedLibraries.C,v 1.3 2007/03/06 16:34:01 dongqing Exp $
