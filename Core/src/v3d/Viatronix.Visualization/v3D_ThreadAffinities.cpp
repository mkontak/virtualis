// $Id: v3D_ThreadAffinities.cpp,v 1.1 2008/01/28 15:13:33 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


#include "stdafx.h"
#include "CpuInfo.h"

/**
 * A class for
 */
public ref class ThreadAffinities
{
// member variables
public:

 /**
  * This is to guarantee that the timer calls reference the same processor timer core.
  * Problem manifiests iteself on AMD systems (usually single processor, dual core systems).
  */
  static void Initialize()
  {
    CpuInfo ci;
    if ( ci.GetVendor() == "AuthenticAMD" && ci.GetCoresPerProcessor() == 2 && ci.GetTotalCores() == 2 )
      SetThreadAffinityMask( GetCurrentThread(), 1 );
  } // Initialize()

}; // class ThreadAffinities


// $Log: v3D_ThreadAffinities.cpp,v $
// Revision 1.1  2008/01/28 15:13:33  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ThreadAffinities.cpp,v 1.1 2008/01/28 15:13:33 geconomos Exp $
// $Id: v3D_ThreadAffinities.cpp,v 1.1 2008/01/28 15:13:33 geconomos Exp $
