// $Id: vxViewerTypeVesselDetailMPROblique.h,v 1.1 2006/09/25 13:16:04 geconomos Exp $
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
#include "vxViewerTypeMPROblique.h"


class VX_VIEWER_DLL vxViewerTypeVesselDetailMPROblique : public vxViewerTypeMPROblique
{
// functions
public:

  /// constructor
  vxViewerTypeVesselDetailMPROblique( vxEnvironment & environment );

  /// destructor
  virtual ~vxViewerTypeVesselDetailMPROblique() {}

  /// initializes the view
  virtual void InitializeView();

  /// initializes the view/object in view based on an annotation
  virtual bool InitializeFrom( vxAnnotation & annotation );

  /// returns the name of the class for type checking (here till dynamic_cast works across DLLs)
  //virtual const std::string GetClassString() const { return "vxViewerTypeVesselDetailMPROblique"; }

private:

  /// copy constructor
  vxViewerTypeVesselDetailMPROblique( const vxViewerTypeMPROblique & other );                    // should only be public if really implemented!

  /// assignment operator
  const vxViewerTypeVesselDetailMPROblique & operator =( const vxViewerTypeMPROblique & other ); // should only be public if really implemented!

}; // class vxViewerTypeVesselDetailMPROblique


// $Log: vxViewerTypeVesselDetailMPROblique.h,v $
// Revision 1.1  2006/09/25 13:16:04  geconomos
// moved from vascular and renamed
//
// Revision 1.4  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.3  2005/06/08 15:02:49  vxconfig
// formatting
//
// Revision 1.2  2004/12/22 14:13:00  frank
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
// Revision 1.3.2.1  2004/02/25 15:00:57  frank
// formatting
//
// Revision 1.3  2004/01/14 14:18:16  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.2  2003/12/11 20:08:36  frank
// Issue #3569: Showed the annotation on the oblique view when you click on them in the pane
//
// Revision 1.1  2003/05/29 14:04:01  frank
// Initialized the oblique slice on the first vessel cross-section
//
// $Id: vxViewerTypeVesselDetailMPROblique.h,v 1.1 2006/09/25 13:16:04 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerTypeVesselDetailMPROblique.h,v 1.1 2006/09/25 13:16:04 geconomos Exp $
