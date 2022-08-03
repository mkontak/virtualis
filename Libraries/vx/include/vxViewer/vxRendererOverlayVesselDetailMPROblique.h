// $Id: vxRendererOverlayVesselDetailMPROblique.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxRendererOverlayMPROblique.h"


// forward declarations
class vxEnvironment;


class VX_VIEWER_DLL vxRendererOverlayVesselDetailMPROblique : public vxRendererOverlayMPROblique
{
// Member Functions
public:

  /// Constructor
  vxRendererOverlayVesselDetailMPROblique( vxEnvironment & environment );

public:

  /// Render 2D overlay
  virtual void Render();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// return the field of view as a string
  virtual std::string GetFieldOfViewString() const;

  /// return the current slice number as a string
  virtual std::string GetSliceNumberString();

private:

  /// Copy constructor
  vxRendererOverlayVesselDetailMPROblique( const vxRendererOverlayVesselDetailMPROblique & );

  /// Assignment operator
  const vxRendererOverlayVesselDetailMPROblique & operator =( const vxRendererOverlayVesselDetailMPROblique & );

}; // class vxRendererOverlayVesselDetailMPROblique


// $Log: vxRendererOverlayVesselDetailMPROblique.h,v $
// Revision 1.2  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.1  2006/09/25 13:17:34  geconomos
// moved from vascular
//
// Revision 1.2  2005/06/08 15:00:59  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2004/01/14 18:18:41  frank
// Updated the current slice number to indicate the distance along the vessel
//
// Revision 1.1  2004/01/07 18:28:24  frank
// initial version
//
// Revision 1.3  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.3  2003/04/23 14:02:29  dongqing
// code review
//
// Revision 1.2  2003/04/22 12:29:32  frank
// code review
//
// Revision 1.1  2002/11/15 18:28:07  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlayVesselDetailMPROblique.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxRendererOverlayVesselDetailMPROblique.h,v 1.2 2007/03/07 17:04:39 geconomos Exp $
