// $Id: vxRendererOverlayMPRCurved.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxRendererOverlayMPR.h"


/**
 * This renderer displays graphs of lumen area, diameter, and measurements for the curved views.
 */
class VX_VIEWER_DLL vxRendererOverlayMPRCurved : public vxRendererOverlayMPR
{
// Member Functions
public:

  /// Constructor
  vxRendererOverlayMPRCurved( vxEnvironment & environment );
  
  /// Destructor
  virtual ~vxRendererOverlayMPRCurved();
  
  /// Perform the rendering
  virtual void Render();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

protected:

  /// return the slice type text string
  virtual std::string GetSliceTypeString();

  /// Get the field of view
  virtual std::string GetFieldOfViewString();

  /// Return the current slice number as a string
  virtual std::string GetSliceNumberString();
  
  /// Determine if the current slice (or slab) intersects the crop box and display message if not.
  virtual void ExplainMissingImage() {}

}; // class vxRendererOverlayMPRCurvedGraph


// $Log: vxRendererOverlayMPRCurved.h,v $
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.3  2005/06/08 15:00:49  vxconfig
// formatting
//
// Revision 1.2  2004/12/22 14:11:43  frank
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
// Revision 1.2  2003/12/18 17:05:31  frank
// Optimizing redraws
//
// Revision 1.1  2003/11/24 16:42:10  frank
// Renamed the class
//
// Revision 1.2  2003/11/06 20:44:04  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.1  2003/05/22 16:20:59  frank
// changed name to reflect class derivation
//
// Revision 1.2  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.4  2003/05/05 16:57:48  frank
// code review
//
// Revision 1.3  2003/01/16 21:26:56  frank
// code review
//
// Revision 1.2  2002/11/22 04:27:50  frank
// Added field of view
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.2  2002/08/23 15:56:53  ana
// Percent stenosis.
//
// Revision 1.1  2002/08/22 21:28:56  ana
// Luminal graph.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlayMPRCurved.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
// $Id: vxRendererOverlayMPRCurved.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
