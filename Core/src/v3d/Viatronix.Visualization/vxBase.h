// $Id: vxBase.h,v 1.3 2006/03/29 20:08:42 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: george econmos (george@viatronix.com)

// pragmas
#pragma once

// includes
#include "Exception.h"
#include "vxShareableObject.h"
#include "vxBlockVolume.h"
#include "vxTimeVolume.h"
#include "vxModifiedInfo.h"
#include "vxModifiedInfoList.h"
#include "vxModifiedSet.h"
#include "vxRenderingMode.h"
#include "vxComponentArray.h"
#include "BaseHistogram.h"
#include "Image.h"
#include "Triple.h"
#include "Point3D.h"
#include "Vector3D.h"
#include "Normal3D.h"
#include "vxElementPoint3D.h"
#include "BoundingRect.h"
#include <list>


// defines
#if defined(VXBUILD)
#     define EXP_TEMPLATE 
#elif  defined(VXIMPORT)
#     define EXP_TEMPLATE extern
#endif



#if defined(VXBUILD) || defined(VXIMPORT)
// export template classes
//EXP_TEMPLATE template class VXDLL vxShareableObject< vxComponentArray >;
//EXP_TEMPLATE template class VXDLL vxShareableObject< vxBlockVolume< uint1 > >;
//EXP_TEMPLATE template class VXDLL vxShareableObject< vxBlockVolume< uint2 > >;
//EXP_TEMPLATE template class VXDLL vxShareableObject< vxBlockVolume< uint2 > >;
//EXP_TEMPLATE template class VXDLL vxShareableObject< vxBlockVolume< bool > >;
//EXP_TEMPLATE template class VXDLL vxShareableObject< vxBlockVolume< float4 > >;
//EXP_TEMPLATE template class VXDLL vxShareableObject< vxRenderingMode >;
//EXP_TEMPLATE template class VXDLL Triple<uint1 >;
//EXP_TEMPLATE template class VXDLL Triple<uint2 >;
//EXP_TEMPLATE template class VXDLL Triple<uint4 >;
//EXP_TEMPLATE template class VXDLL Triple< int1 >;
//EXP_TEMPLATE template class VXDLL Triple< int2 >;
//EXP_TEMPLATE template class VXDLL Triple< int4 >;
//EXP_TEMPLATE template class VXDLL Triple<float4>;
//EXP_TEMPLATE template class VXDLL Triple<float8>;
//EXP_TEMPLATE template class VXDLL Point3D<uint1 >;
//EXP_TEMPLATE template class VXDLL Point3D<uint2 >;
//EXP_TEMPLATE template class VXDLL Point3D< int1 >;
//EXP_TEMPLATE template class VXDLL Point3D< int2 >;
//EXP_TEMPLATE template class VXDLL Point3D< int4 >;
//EXP_TEMPLATE template class VXDLL Point3D<float4>;
//EXP_TEMPLATE template class VXDLL Point3D<float8>;
//EXP_TEMPLATE template class VXDLL Vector3D<uint1 >;
//EXP_TEMPLATE template class VXDLL Vector3D<uint2 >;
//EXP_TEMPLATE template class VXDLL Vector3D< int1 >;
//EXP_TEMPLATE template class VXDLL Vector3D< int2 >;
//EXP_TEMPLATE template class VXDLL Vector3D< int4 >;
//EXP_TEMPLATE template class VXDLL Vector3D<float4>;
//EXP_TEMPLATE template class VXDLL Vector3D<float8>;
//EXP_TEMPLATE template class VXDLL Normal3D<float4>;
//EXP_TEMPLATE template class VXDLL Normal3D<float8>;
//EXP_TEMPLATE template class VXDLL Image< bool >;
//EXP_TEMPLATE template class VXDLL Image< bool >;
//EXP_TEMPLATE template class VXDLL Image< uint1 >;
//EXP_TEMPLATE template class VXDLL Image< int2 >;
//EXP_TEMPLATE template class VXDLL Image< float4 >;
//EXP_TEMPLATE template class VXDLL vxElementPoint3D< float4 >;
//EXP_TEMPLATE template class VXDLL BoundingRect< int2 >;



#endif // VXBUILD || VXIMPO


// $Log: vxBase.h,v $
// Revision 1.3  2006/03/29 20:08:42  geconomos
// removed vxBlockVolumeBinaryHistogram related code
//
// Revision 1.2  2004/04/06 12:10:06  frank
// removed unused includes
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/12/23 23:05:41  wenli
// Add GetBoundingRect() to slove the ssue #3167: missing ablility to move the annotation text in curved
//
// Revision 1.8  2003/06/18 15:19:46  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.7  2003/05/16 13:14:29  frank
// formatting
//
// Revision 1.6  2003/05/13 19:08:33  michael
// code review
//
// Revision 1.5  2003/05/12 16:31:48  michael
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/vxBase.h,v 1.3 2006/03/29 20:08:42 geconomos Exp $
// $Id: vxBase.h,v 1.3 2006/03/29 20:08:42 geconomos Exp $
