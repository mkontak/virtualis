// $Id: vxElements.C,v 1.7 2008/03/20 16:52:14 cqian Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxElement.h"
#include "vxElementBool.h"
#include "vxElementFloat4.h"
#include "vxElementInt4.h"
#include "vxElementPoint3D.h"
#include "vxElementPixelRGBA.h"
#include "vxElementVector.h"
#include "vxElementString.h"


template class VX_VIEWER_DLL vxElementPoint3D< float4 >;
template class VX_VIEWER_DLL vxElementVector< float4 >;


// $Log: vxElements.C,v $
// Revision 1.7  2008/03/20 16:52:14  cqian
// change the wrapper of elementsttring
//
// Revision 1.6  2008/02/20 19:07:18  gdavidson
// Removed vxElementPlane
//
// Revision 1.5  2008/01/29 14:33:47  cqian
// Modiify the cut plane
//
// Revision 1.4  2007/04/06 14:11:58  gdavidson
// Added vxElementVector
//
// Revision 1.3  2006/05/18 17:48:40  gdavidson
// implicit instantiation of a xElementPoint3D< float4 >
//
// Revision 1.2  2006/03/27 19:01:27  gdavidson
// included vxElementPixelRGBA
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxElements.C,v 1.7 2008/03/20 16:52:14 cqian Exp $
// $Id: vxElements.C,v 1.7 2008/03/20 16:52:14 cqian Exp $