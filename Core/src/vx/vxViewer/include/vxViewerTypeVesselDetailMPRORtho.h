// $Id: vxViewerTypeVesselDetailMPRORtho.h,v 1.1 2006/09/25 13:16:04 geconomos Exp $
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
#include "vxViewerTypeMPROrtho.h"


class VX_VIEWER_DLL vxViewerTypeVesselDetailMPROrtho : public vxViewerTypeMPROrtho
{
// functions
public:

  /// constructor
  vxViewerTypeVesselDetailMPROrtho( vxEnvironment & environment );

  /// destructor
  virtual ~vxViewerTypeVesselDetailMPROrtho() {}

  /// initializes the view
  virtual void InitializeView();

private:

  /// copy constructor
  vxViewerTypeVesselDetailMPROrtho( const vxViewerTypeVesselDetailMPROrtho & other );                    // should only be public if really implemented!

  /// assignment operator
  const vxViewerTypeVesselDetailMPROrtho & operator =( const vxViewerTypeVesselDetailMPROrtho & other ); // should only be public if really implemented!

}; // class vxViewerTypeVesselDetailMPROrtho


// $Log: vxViewerTypeVesselDetailMPRORtho.h,v $
// Revision 1.1  2006/09/25 13:16:04  geconomos
// moved from vascular and renamed
//
// Revision 1.3  2005/06/07 18:15:03  vxconfig
// removed the vessel array
//
// Revision 1.2  2004/12/22 14:13:08  frank
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
// Revision 1.2  2004/03/02 01:10:28  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.1.2.1  2004/02/25 16:08:28  frank
// initial version
//
// $Id: vxViewerTypeVesselDetailMPRORtho.h,v 1.1 2006/09/25 13:16:04 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerTypeVesselDetailMPRORtho.h,v 1.1 2006/09/25 13:16:04 geconomos Exp $
