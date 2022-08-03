// $Id: vxRendererSliceShadowsMPROrtho.h,v 1.4 2007/03/02 21:20:23 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: George Economos (mailto:george@viatronix.com)


// pragma declarations
#pragma once


//  include delcarations
#include "vxRendererSliceShadows.h"


/** 
 * Renders shadows for ortho/oblique/ curved planes for ortho mpr views
 */
class VX_VIEWER_DLL vxRendererSliceShadowsMPROrtho : public vxRendererSliceShadows
{
// member functions
public:

  /// constructor
  vxRendererSliceShadowsMPROrtho( vxEnvironment & environment );

  /// performs rendering
  virtual void Render();

private:

  /// renders the projection of an oblique or ortho plane
  void RenderPlane( const vxEnvironment & environment );

  /// renders the projection of a curved plane
  void RenderCurvedPlane( const vxEnvironment & environment );

}; // class vxRendererSliceShadowsMPROrtho


// $Log: vxRendererSliceShadowsMPROrtho.h,v $
// Revision 1.4  2007/03/02 21:20:23  geconomos
// corrected checkin
//
// Revision 1.2  2006/10/23 01:38:54  geconomos
// code cleanup
//
// Revision 1.1  2006/10/03 20:02:17  geconomos
// continued reworking of slice shadows
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererSliceShadowsMPROrtho.h,v 1.4 2007/03/02 21:20:23 geconomos Exp $
// $Id: vxRendererSliceShadowsMPROrtho.h,v 1.4 2007/03/02 21:20:23 geconomos Exp $