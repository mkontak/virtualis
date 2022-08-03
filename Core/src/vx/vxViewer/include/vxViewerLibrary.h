#ifndef vxViewerLibrary_h
#define vxViewerLibrary_h

#ifdef VX_VIEWER_BUILD
#   define  VX_VIEWER_DLL   __declspec( dllexport )
#elif defined(VX_TEST)
#   define  VX_VIEWER_DLL
#else
#   define  VX_VIEWER_DLL   __declspec( dllimport )
#endif

#include "vxBaseLibrary.h"
#include "vxBlockVolumeLibrary.h"

#include "Triple.h"
#include "vxDOMElement.h"
#include "vxDOMDocument.h"
#include "vxDOMNodeList.h"

#endif // #ifndef vxViewerLibrary_h
