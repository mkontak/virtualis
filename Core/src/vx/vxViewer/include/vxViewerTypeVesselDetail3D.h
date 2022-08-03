// $Id: vxViewerTypeVesselDetail3D.h,v 1.1 2006/09/25 13:16:04 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

/**
 * The oblique viewer specialized for vessel cross-sections.
 * It basically works the same except that it initializes itself
 * at the first cross-section of the vessel.
 */


// pragmas
#pragma once


// includes
#include "vxViewerType3D.h"


class VX_VIEWER_DLL vxViewerTypeVesselDetail3D : public vxViewerType3D
{
// functions
public:

  /// constructor
  vxViewerTypeVesselDetail3D( vxEnvironment & environment );

  /// destructor
  virtual ~vxViewerTypeVesselDetail3D() {}


private:

  /// copy constructor
  vxViewerTypeVesselDetail3D( const vxViewerTypeVesselDetail3D & other );                    // should only be public if really implemented!

  /// assignment operator
  const vxViewerTypeVesselDetail3D & operator =( const vxViewerTypeVesselDetail3D & other ); // should only be public if really implemented!

}; // class vxViewerTypeVesselDetail3D


// $Log: vxViewerTypeVesselDetail3D.h,v $
// Revision 1.1  2006/09/25 13:16:04  geconomos
// moved from vascular and renamed
//
// $Id: vxViewerTypeVesselDetail3D.h,v 1.1 2006/09/25 13:16:04 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerTypeVesselDetail3D.h,v 1.1 2006/09/25 13:16:04 geconomos Exp $
