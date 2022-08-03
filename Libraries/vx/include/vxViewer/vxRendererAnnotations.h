// $Id: vxRendererAnnotations.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/**
 * Renderer for displaying annotations with their labels and statistics.
 */

#ifndef vxRendererAnnotations_h
#define vxRendererAnnotations_h


// includes
#include "vxRenderer.h"


// class definition
class VX_VIEWER_DLL vxRendererAnnotations : public vxRenderer
{
// functions
public:
  
  /// constructor
  vxRendererAnnotations(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererAnnotations() {}

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// perform rendering operations
  virtual int4 PreRender();

  /// perform rendering operations
  virtual void Render();
  
private:

  /// copy constructor
  vxRendererAnnotations(const vxRendererAnnotations & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererAnnotations & operator=(const vxRendererAnnotations & other); // should only be public if really implemented!
}; // class vxRendererAnnotations


#endif // vxRendererAnnotations_h


// Revision History:
// $Log: vxRendererAnnotations.h,v $
// Revision 1.2  2007/02/15 00:08:24  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.1.1.1.18.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.32  2003/12/17 18:02:55  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.31  2003/05/19 14:37:44  frank
// Removed unused return parameter
//
// Revision 1.30  2003/05/17 15:44:49  michael
// reformatting
//
// Revision 1.29  2003/05/16 13:22:14  frank
// formatting
//
// Revision 1.28  2003/05/14 18:30:03  dongqing
// code review
//
// Revision 1.27  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.26  2003/03/04 17:57:58  frank
// Cleaned out unused functions
//
// Revision 1.25  2003/01/22 22:00:29  ingmar
// added missing includes
//
// Revision 1.24  2003/01/15 16:30:13  michael
// calling PreRender() to determine the handle size in world space
//
// Revision 1.23  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.22  2002/11/22 17:24:47  michael
// fixed drawing of arrows and points in oblique
//
// Revision 1.21  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.20  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.19  2002/06/12 13:30:33  manju
// Moved GetScreenCoordinates method completely to vxUtils.
//
// Revision 1.18  2002/06/03 20:44:45  manju
// Made the render functions virtual, to be derived by
// vxRendererAnnotations3D.
//
// Revision 1.17  2002/05/29 19:28:46  manju
// Coding standard.
//
// Revision 1.16  2002/05/21 15:28:06  manju
// Using screen coordinates to position labels and statistics for annotations.
//
// Revision 1.15  2002/04/29 18:32:11  manju
// Added rectangle annotation.
//
// Revision 1.14  2002/04/25 19:19:43  manju
// Modified drawing of arc of angle annotation and labels and text for linear
// and angle annotation, to look better for large datasets and also when
// zoomed.
//
// Revision 1.13  2002/04/24 20:15:40  manju
// Using PixelRGBA to represent color, in RenderArrow method.
//
// Revision 1.12  2002/04/08 21:20:41  manju
// Modified rendering of annotations to take into account all three orientations,
// Axial, Sagittal and Coronal.
//
// Revision 1.11  2002/04/08 13:58:37  manju
// Added rendering of volume annotation and of histogrmas for volume, ellipse,
// lasso and linear annotations.
//
// Revision 1.10  2002/04/02 22:17:21  manju
// Displaying statistics of annotations.
//
// Revision 1.9  2002/04/02 20:55:46  michael
// removed redundant include
//
// Revision 1.8  2002/04/01 19:04:24  manju
// Removed passing of font as argument to the various rendering methods.
//
// Revision 1.7  2002/03/27 18:15:51  manju
// The annotations list is now made of HandlesObj and not Annotation objects.
//
// Revision 1.6  2002/03/27 14:25:35  michael
// cosmetics
//
// Revision 1.5  2002/03/25 16:29:12  manju
// Added creation of font for the annotation text to appear. This will be
// removed later.
//
// Revision 1.4  2002/03/21 18:34:28  manju
// cleaned the includes.
//
// Revision 1.3  2002/03/21 16:16:18  manju
// Modified rendering of annotations.
//
// Revision 1.2  2002/03/15 19:31:46  manju
// Changed RendererAnnotations to vxRendererAnnotations in the ifdef
// statement.
//
// Revision 1.1  2002/03/15 18:52:56  manju
// Renderer for Annotations.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererAnnotations.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
// $Id: vxRendererAnnotations.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
