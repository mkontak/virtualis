// $Id: v3D_Controllers.h,v 1.13.2.1 2009/03/31 21:00:35 gdavidson Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// pragmas
#pragma once


// includes
#include "stdafx.h"
#include "v3D_Interfaces.h"
#include "vxControllerRebox.h"
#include "vxControllerCursor3D.h"
#include "vxControllerRibbon.h"
#include "vxControllerVesselDetail.h"
#include "vxControllerGlobalActiveEnvironment.h"
#include "vxControllerGlobalSliceShadows.h"
#include "vxControllerMomentum.h"
#include "vxControllerCine2D.h"
#include "vxControllerCameraFly.h"
#include "vxControllerCADFindings.h"
#include "vxControllerGlobalSyncPlane.h"
#include "vxControllerGlobalSyncViewers.h"
#include "vxControllerVesselSeedpoints.h"


// namespaces
OPEN_VISUALIZATION_NS


// local controller definitions
DEFINE_LOCAL_CONTROLLER( ControllerRebox )
DEFINE_LOCAL_CONTROLLER( ControllerCursor3D )
DEFINE_LOCAL_CONTROLLER( ControllerRibbon )
DEFINE_LOCAL_CONTROLLER( ControllerVesselDetail )
DEFINE_LOCAL_CONTROLLER( ControllerMomentum )
DEFINE_LOCAL_CONTROLLER( ControllerCine2D )
DEFINE_LOCAL_CONTROLLER( ControllerCameraFly )
DEFINE_LOCAL_CONTROLLER( ControllerCADFindings )
DEFINE_LOCAL_CONTROLLER( ControllerVesselSeedpoints )



// global controller definitions
DEFINE_GLOBAL_CONTROLLER( ControllerGlobalActiveEnvironment )
DEFINE_GLOBAL_CONTROLLER( ControllerGlobalSliceShadows )
DEFINE_GLOBAL_CONTROLLER( ControllerGlobalSyncPlane )
DEFINE_GLOBAL_CONTROLLER( ControllerGlobalSyncViewers )


// namespaces
CLOSE_VISUALIZATION_NS


// $Log: v3D_Controllers.h,v $
// Revision 1.13.2.1  2009/03/31 21:00:35  gdavidson
// Added ControllerVesselSeedpoints
//
// Revision 1.13  2008/05/02 05:00:39  cqian
// add ControllerGlobalSyncViewers
//
// Revision 1.12  2007/10/01 14:49:28  gdavidson
// Added vxControllerGlobalSyncPlane
//
// Revision 1.11  2007/06/22 15:16:57  geconomos
// Removed vxGlobalControllerDatasets
//
// Revision 1.10  2007/04/11 20:56:43  romy
// added ControllerCADFindings
//
// Revision 1.9  2007/01/16 17:11:34  gdavidson
// Added ControllerCine2D, ControllerMomentum and ControllerCameraFly
//
// Revision 1.8  2006/11/09 15:01:48  gdavidson
// Added GlobalControllers
//
// Revision 1.7  2006/10/05 17:27:48  geconomos
// removed mpr curved controller
//
// Revision 1.6  2006/10/04 14:44:18  geconomos
// removed ControllerDatasets
//
// Revision 1.5  2006/09/14 20:24:22  geconomos
// added ControllerRibbon and ControllerVesselDetail
//
// Revision 1.4  2006/08/17 14:07:16  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.3  2004/10/27 18:38:02  frank
// moved cursor 3D functionality to a controller
//
// Revision 1.2  2004/10/04 15:07:30  frank
// added dataset controller to handle 4D data with multiple renderer data objs
//
// Revision 1.1  2004/05/20 14:14:53  frank
// initial version of controllers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Controllers.h,v 1.13.2.1 2009/03/31 21:00:35 gdavidson Exp $
// $Id: v3D_Controllers.h,v 1.13.2.1 2009/03/31 21:00:35 gdavidson Exp $
