// $Id: ImportedLibraries.C,v 1.2.2.1 2008/01/18 17:14:43 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (george@viatronix.com)

#include "stdafx.h"

#pragma comment( lib, "Netapi32.lib" )
#pragma comment( lib, "wsock32.lib" )


  #ifdef _DEBUG

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBased.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolumed.lib" )
	  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxCADd.lib" )
	  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxMessagingd.lib" )
  #else

    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBase.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxCAD.lib" )
    #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxMessaging.lib" )

  #endif



// $Log: ImportedLibraries.C,v $
// Revision 1.2.2.1  2008/01/18 17:14:43  geconomos
// remove unsued libs
//
// Revision 1.2  2007/03/01 20:40:36  geconomos
// code review preparation
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/ImportedLibraries.C,v 1.2.2.1 2008/01/18 17:14:43 geconomos Exp $
// $Id: ImportedLibraries.C,v 1.2.2.1 2008/01/18 17:14:43 geconomos Exp $
