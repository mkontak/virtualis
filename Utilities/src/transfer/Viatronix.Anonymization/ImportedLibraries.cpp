// $Id: ImportedLibraries.cpp,v 1.2 2007/03/12 14:43:20 mkontak Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] (mailto:mkontak@viatrinix.com)

// includes
#include "stdafx.h"

// pragmas
#pragma comment( lib, "gdiplus.lib" )


// ====================================
// Only 64 bit code is being generated
// ====================================

#ifdef _DEBUG

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBased.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolumed.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
  
#else

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBase.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )

#endif

