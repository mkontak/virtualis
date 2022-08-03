// $Id: vxManipulatorVesselCurved.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxManipulator.h"


class VX_VIEWER_DLL vxManipulatorVesselCurved : public vxManipulator
{
public:

  /// Constructor
  vxManipulatorVesselCurved( vxEnvironment & environment );

  /// Zoom the view to fit the entire volume
  static void ZoomEntireVolume( vxEnvironment & environment, const Triple<uint4> & imageDim );
  
}; // class vxManipulatorVesselCurved


// $Log: vxManipulatorVesselCurved.h,v $
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.2  2005/06/08 15:00:04  vxconfig
// added easy select object
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.5  2003/02/14 19:26:15  frank
// Moved zoom function out to curved
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorVesselCurved.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
// $Id: vxManipulatorVesselCurved.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
