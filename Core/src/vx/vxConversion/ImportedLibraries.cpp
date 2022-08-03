//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] (mailto:mkontak@viatronix.com)

/// includes
#include "stdafx.h"

#pragma comment( lib, "gdiplus.lib" )
#pragma comment( lib, "Ws2_32.lib" )


#if defined(WIN64) || defined(_WIN64)

#ifdef _DEBUG
  
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ofstd64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdata64d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\oflog64d.lib" )

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLoggerd.lib" )
   #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBased.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolumed.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxDicomd.lib" )

#else

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ofstd64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdata64.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\oflog64.lib" )

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBase.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxDicom.lib" ) 

#endif

#else

#ifdef _DEBUG
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ofstd32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdata32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\oflog32d.lib" )

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBase32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxDicom32d.lib" )

#else


  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\ofstd32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\dcmdata32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\dcmtk\\lib\\oflog32.lib" )

  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxLogger32d.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBase32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxBlockVolume32.lib" )
  #pragma comment( lib, "..\\..\\..\\..\\Libraries\\vx\\lib\\vxDicom32.lib" ) 

#endif

#endif

