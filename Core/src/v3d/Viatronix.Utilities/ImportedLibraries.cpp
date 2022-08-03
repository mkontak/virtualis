// $Id: ImportedLibraries.cpp,v 1.5 2007/03/09 21:07:03 mkontak Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (george@viatronix.com)

//includes
#include "stdafx.h"

// pragmas
#pragma comment( lib, "gdiplus.lib" )
#pragma comment( lib, "kernel32.lib" )
#pragma comment( lib, "Netapi32.lib" )
#pragma comment( lib, "vfw32.lib" )


#ifdef _DEBUG
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\wmsdk\\lib\\wmvcore64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBased.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
#else
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\wmsdk\\lib\\wmvcore64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBase.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
#endif

