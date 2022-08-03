#ifndef vxBlockVolumeLibrary_h
#define vxBlockVolumeLibrary_h

#ifdef VX_BLOCK_VOLUME_BUILD
#   define  VX_BLOCK_VOLUME_DLL   __declspec( dllexport )
#elif defined(VX_TEST)
#   define  VX_BLOCK_VOLUME_DLL
#else
#   define  VX_BLOCK_VOLUME_DLL   __declspec( dllimport )
#endif

// include vxBase headers
#include "Logger.h"
#include "vxBaseLibrary.h"


#endif // #ifndef vxBlockVolumeLibrary_h
