// $Id: ImportedLibraries.C,v 1.0 2011/06/21 dongqing Exp $
//
// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Dongqing (dongqing@viatronix.com)

#include "stdafx.h"


#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "Netapi32.lib" )
#pragma comment( lib, "opengl32.lib" )


  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\minilzo\\lib\\minilzo64d.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\zlib\\lib\\zlib64d.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxLoggerd.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxBased.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxBlockVolumed.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxViewerd.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxColond.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vc\\lib\\vxColonWrapper.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vc\\lib\\Viatronix.v3D.Processor.Plugins.Colond.lib" )
  
  #else

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\minilzo\\lib\\minilzo64.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\zlib\\lib\\zlib64.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxLogger.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxBase.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxBlockVolume.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxViewer.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vx\\lib\\vxColon.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vc\\lib\\vxColonWrapper.lib" )
    #pragma comment( lib, "..\\..\\..\\libs\\vc\\lib\\Viatronix.v3D.Processor.Plugins.Colon.lib" )

  #endif


// $Log: ImportedLibraries.C,v $
// Revision 1.0 2011/06/21  dongqing
//
// $Header: /VCpipeline/ImportedLibraries.C,v 1.0 2011/06/21 dongqing Exp $
// $Id: ImportedLibraries.C,v 1.0 2011/06/21 dongqing Exp $
