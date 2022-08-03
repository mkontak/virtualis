// $Id: vxRendererSliceShadowsMPROblique.h,v 1.3 2007/03/02 21:20:23 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: George  Economos (mailto:george@viatronix.com)


// pragma declarations
#pragma once


// include declarations
#include "vxRendererSliceShadowsMPROrtho.h"

/** 
 * Renders shadows for ortho/oblique/ curved planes for oblique mpr views
 */
class VX_VIEWER_DLL vxRendererSliceShadowsMPROblique : public vxRendererSliceShadowsMPROrtho
{
// member functions
public:

  /// constructor
  vxRendererSliceShadowsMPROblique( vxEnvironment & environment );

}; // class vxRendererSliceShadowsCurvedOnOblique


// $Log: vxRendererSliceShadowsMPROblique.h,v $
// Revision 1.3  2007/03/02 21:20:23  geconomos
// corrected checkin
//
// Revision 1.1  2006/10/03 20:02:17  geconomos
// continued reworking of slice shadows
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererSliceShadowsMPROblique.h,v 1.3 2007/03/02 21:20:23 geconomos Exp $
// $Id: vxRendererSliceShadowsMPROblique.h,v 1.3 2007/03/02 21:20:23 geconomos Exp $
