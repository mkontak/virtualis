// $Id: vxViewerTypeVesselDetailEndoluminal.h,v 1.1 2006/09/25 13:16:04 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxViewerType3D.h"


/**
* The 3D viewer specialized for endoluminal viewing.
* It basically works the same except that it initializes itself
* at the start of the vessel centerline.
*/
class VX_VIEWER_DLL vxViewerTypeVesselDetailEndoluminal : public vxViewerType3D
{
// functions
public:

  /// constructor
  vxViewerTypeVesselDetailEndoluminal( vxEnvironment & environment );

  /// destructor
  virtual ~vxViewerTypeVesselDetailEndoluminal() {}

  /// initializes the view
  virtual void InitializeView();

private:

  /// copy constructor
  vxViewerTypeVesselDetailEndoluminal( const vxViewerTypeVesselDetailEndoluminal & other );                    // should only be public if really implemented!

  /// assignment operator
  const vxViewerTypeVesselDetailEndoluminal & operator =( const vxViewerTypeVesselDetailEndoluminal & other ); // should only be public if really implemented!

}; // class vxViewerTypeVesselDetailEndoluminal


// $Log: vxViewerTypeVesselDetailEndoluminal.h,v $
// Revision 1.1  2006/09/25 13:16:04  geconomos
// moved from vascular and renamed
//
// Revision 1.3  2005/06/08 15:02:49  vxconfig
// formatting
//
// Revision 1.2  2004/12/10 14:55:16  frank
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/03/02 19:19:17  frank
// renamed method in viewer type to more accurately describe it
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/03/02 01:10:29  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.3.2.1  2004/02/25 15:00:49  frank
// formatting
//
// Revision 1.3  2004/01/14 14:18:16  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.2  2003/09/30 20:45:23  frank
// code review
//
// Revision 1.1  2003/07/09 14:15:10  frank
// Added 3D endoluminal viewer type
//
// $Id: vxViewerTypeVesselDetailEndoluminal.h,v 1.1 2006/09/25 13:16:04 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerTypeVesselDetailEndoluminal.h,v 1.1 2006/09/25 13:16:04 geconomos Exp $
