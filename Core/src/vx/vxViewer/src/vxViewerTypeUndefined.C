// $Id: vxViewerTypeUndefined.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner, (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxViewerTypeUndefined.h"


/**
 * Constructor
 */
vxViewerTypeUndefined::vxViewerTypeUndefined(vxEnvironment & environment)
:vxViewerType(environment)
//*******************************************************************
{
}


// Revision History:
// $Log: vxViewerTypeUndefined.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.12  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.11  2003/04/16 13:42:25  michael
// performed initial code review
//
// Revision 1.10  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.9  2003/03/20 21:28:16  frank
// Individual IsVisible() for a single point, rather than for a whole annotation
//
// Revision 1.8  2003/03/18 21:23:02  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.7  2003/03/03 16:50:39  geconomos
// Added and implemented InitializeCamera method.
//
// Revision 1.6  2003/02/28 17:54:33  michael
// partially moved SetupView() from vxViewer over to vxViewerType ...
//
// Revision 1.5  2003/01/22 15:04:47  michael
// adjusted code to account for our Y-flip in the GetScreenCoordinates()
//
// Revision 1.4  2003/01/15 16:29:51  michael
// made methods const that do not alter the state
//
// Revision 1.3  2003/01/13 23:19:35  michael
// added get/set for the environment (protected member in base class)
//
// Revision 1.2  2003/01/09 14:56:03  michael
// adjusted methods to take new annotation classes
//
// Revision 1.1  2002/12/18 15:01:33  michael
// added initial version of undefined viewer type (default one)
//
// $Id: vxViewerTypeUndefined.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerTypeUndefined.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
