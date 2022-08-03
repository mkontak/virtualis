// $Id: v3D_ViewerTypes.h,v 1.7 2007/08/09 21:01:28 romy Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "v3D_Environment.h"
#include "v3D_Interfaces.h"
#include "vxViewerTypeUndefined.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerType3D.h"
#include "vxViewerTypeMPROrthoPanZoom.h"
#include "vxViewerTypeMPRCurved.h"
#include "vxViewerTypeMPRCurvedLuminal.h"
#include "vxViewerTypeVesselDetailEndoluminal.h"
#include "vxViewerTypeVesselDetailMPROblique.h"
#include "vxViewerTypeVesselDetailMPROrtho.h"
#include "vxViewerTypeVesselDetail3D.h"
#include "vxViewerTypeMesh.h"

// for the virtual bool bug
#pragma warning( disable : 4800 )


// namespaces
OPEN_VISUALIZATION_NS

  DEFINE_VIEWER_TYPE( ViewerTypeUndefined )

  DEFINE_VIEWER_TYPE( ViewerTypeMPROrtho )

  DEFINE_VIEWER_TYPE( ViewerTypeMPROrthoPanZoom )

  DEFINE_VIEWER_TYPE( ViewerTypeMPROblique )

  DEFINE_VIEWER_TYPE( ViewerType3D )

  DEFINE_VIEWER_TYPE( ViewerTypeMPRCurved )

  DEFINE_VIEWER_TYPE( ViewerTypeMPRCurvedLuminal )

  DEFINE_VIEWER_TYPE( ViewerTypeVesselDetailEndoluminal )

  DEFINE_VIEWER_TYPE( ViewerTypeVesselDetailMPROblique )

  DEFINE_VIEWER_TYPE( ViewerTypeVesselDetailMPROrtho )

  DEFINE_VIEWER_TYPE( ViewerTypeVesselDetail3D )

  DEFINE_VIEWER_TYPE( ViewerTypeMesh )

	

CLOSE_VISUALIZATION_NS

#pragma warning( default : 4800 )

// $Log: v3D_ViewerTypes.h,v $
// Revision 1.7  2007/08/09 21:01:28  romy
// added new viewertype vxMeshViewerType
//
// Revision 1.6  2006/09/25 13:27:58  geconomos
// moving shared code for vascular and cardiac to visualization dll
//
// Revision 1.5  2006/09/14 20:25:45  geconomos
// added ViewerTypeCurvedLuminal
//
// Revision 1.4  2006/08/17 14:07:16  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.3  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.2  2004/04/27 12:24:55  frank
// workaround the virtual bool bug
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/08/20 14:28:56  michael
// added "mailto:"
//
// Revision 1.6  2003/05/16 13:13:57  frank
// formatting
//
// Revision 1.5  2003/05/13 19:08:32  michael
// code review
//
// Revision 1.4  2003/05/13 13:52:11  geconomos
// More coding standards.
//
// Revision 1.3  2003/05/13 13:50:04  geconomos
// Coding standards.
//
// Revision 1.2  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.1  2003/03/20 13:34:57  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ViewerTypes.h,v 1.7 2007/08/09 21:01:28 romy Exp $
// $Id: v3D_ViewerTypes.h,v 1.7 2007/08/09 21:01:28 romy Exp $
