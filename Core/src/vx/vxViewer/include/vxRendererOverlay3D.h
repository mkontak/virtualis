// $Id: vxRendererOverlay3D.h,v 1.3 2007/02/15 00:08:24 jmeade Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxRendererOverlay3D_h
#define vxRendererOverlay3D_h


// includes
#include "vxRendererOverlay.h"


/**
 * Render the 3D overlay information.
 */
class VX_VIEWER_DLL vxRendererOverlay3D: public vxRendererOverlay
{
// functions
public:

  /// constructor
  vxRendererOverlay3D(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererOverlay3D() {}
  
  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// renders 3D Overlay 
  virtual void Render();

  /// Return the slice type text string
  virtual std::string GetSliceTypeString();


private:

  /// copy constructor
  vxRendererOverlay3D(const vxRendererOverlay3D &);                    // should only be public if really implemented!

  /// assignment operator
  const vxRendererOverlay3D & operator =(const vxRendererOverlay3D &); // should only be public if really implemented!

}; // class vxRendererOverlay3D


#endif


// Revision History:
// $Log: vxRendererOverlay3D.h,v $
// Revision 1.3  2007/02/15 00:08:24  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.2.18.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.2  2004/04/28 15:51:12  frank
// added projection type in 3D view
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.8  2003/09/23 14:57:59  michael
// removed temporary code to compute Euler Angles for Shimadzu. It is now
// part of the V3D Angio plugin code.
//
// Revision 1.7  2003/05/22 13:04:29  frank
// code review
//
// Revision 1.6  2003/05/19 14:40:22  frank
// Removed unused return parameter
//
// Revision 1.5  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.4  2003/01/21 22:15:40  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.3  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.2  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.1  2002/04/26 13:10:50  michael
// Added renamed version of the overlay renderers by moving {2,3}D to the end
//
// Revision 1.10  2002/04/25 19:39:58  frank
// Centralized orientation lettering in vxRendererOverlay class.
//
// Revision 1.9  2002/04/24 03:03:31  michael
// added test if rendering is necessary (false if no volume available)
//
// Revision 1.8  2002/04/08 13:35:21  michael
// added missing privately non implemented copy constructor and assignment operator
//
// Revision 1.7  2002/04/01 23:26:01  michael
// Simplified function call by passing one instead if 4 parameters
//
// Revision 1.6  2002/03/21 20:10:53  jenny
// put comments
//
// Revision 1.5  2002/03/19 23:42:04  michael
// fixed window size using the winRect
//
// Revision 1.4  2002/03/18 19:30:34  jenny
// orientation letters
//
// Revision 1.3  2002/03/14 17:29:38  jenny
// worked on GetOrientationLettering
//
// Revision 1.2  2002/03/11 20:51:24  michael
// less compiler errors
//
// Revision 1.1  2002/03/11 20:15:59  michael
// added initial vxRenderer classes
//
// Revision 1.1  2002/03/11 19:54:00  michael
// added frist draft of the 3D overlay renderer
//
// Revision 1.2  2001/12/15 17:39:33  dmitry
// Changed Font Size
//
// Revision 1.1  2001/12/12 15:09:35  dmitry
// New OverLay Render instad of hack in Viewer
//
// $Id: vxRendererOverlay3D.h,v 1.3 2007/02/15 00:08:24 jmeade Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlay3D.h,v 1.3 2007/02/15 00:08:24 jmeade Exp $
