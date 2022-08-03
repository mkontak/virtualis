// $Id: vxRendererSoftwareMPR.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
//
// Copyright � 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (michael@viatronix.com)

/**
 * This class defines the render software MPR
 */

#ifndef vxRendererSoftwareMPR_h
#define vxRendererSoftwareMPR_h


// includes
#include "vxRendererSoftware.h"


// class definition
class VX_VIEWER_DLL vxRendererSoftwareMPR : public vxRendererSoftware
{
// functions
public:

  /// constructor
  vxRendererSoftwareMPR(vxEnvironment & environment);
 
  /// destructor
  virtual ~vxRendererSoftwareMPR() {}

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// perform pre-rendering operations
  virtual int4 PreRender();
  
  /// perform rendering operations
  virtual void Render();
  
  /// called right before CreateTexture()
  virtual void PreCreateTexture( const BoundingRect<uint2> & window ) {}

  /// set the texture of the image generated by the algorithm
  virtual void DrawTexture();

private:

  /// copy constructor
  vxRendererSoftwareMPR(const vxRendererSoftwareMPR & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererSoftwareMPR & operator=(const vxRendererSoftwareMPR & other); // should only be public if really implemented!
}; // vxRendererSoftwareMPR


#endif // vxRendererSoftwareMPR_h


// Revision History:
// $Log: vxRendererSoftwareMPR.h,v $
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
// Revision 1.24  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.23  2003/05/19 14:42:20  frank
// Removed unused return parameter
//
// Revision 1.22  2003/05/17 15:44:51  michael
// reformatting
//
// Revision 1.21  2003/05/16 13:22:15  frank
// formatting
//
// Revision 1.20  2003/05/14 18:30:04  dongqing
// code review
//
// Revision 1.19  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.18  2002/12/06 21:29:00  michael
// removed old fashioned Set/GetRGBATablePtr(); it was only copy of what is in
// the colormap in the environment.
//
// Revision 1.17  2002/11/19 21:25:31  michael
// cosmetics
//
// Revision 1.16  2002/11/15 18:12:28  frank
// Fixed border clearing fiasco, re-implemented PreCreateTexture()
//
// Revision 1.15  2002/11/12 21:26:35  ingmar
// moved default CreateTexture() implementation from vxRendererSoftwareMPR() to vxRendererSoftware()
//
// Revision 1.14  2002/11/08 13:53:18  michael
// removed redundant method
//
// Revision 1.13  2002/09/28 02:20:33  frank
// Working on PreCreateTexture - a better way to fix image borders.
//
// Revision 1.12  2002/07/08 22:03:33  frank
// Removed unnecesary function.
//
// Revision 1.11  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.10  2002/05/09 17:36:14  uday
// Removed SliceOrientationEnum
//
// Revision 1.9  2002/05/07 18:37:24  frank
// Removed unneccessary includes.
//
// Revision 1.8  2002/04/29 16:07:46  michael
// Now uses slab thickness as defined in vxRenderingMode
//
// Revision 1.7  2002/04/25 01:39:48  michael
// no more VLI table, only ColorTableCpu!
//
// Revision 1.6  2002/04/23 21:34:58  jenny
// no more create volume based on data type.
//
// Revision 1.5  2002/04/19 15:28:46  binli
// MIP rendering/Sliding Thin Slice
//
// Revision 1.4  2002/04/14 00:51:41  michael
// made GetVolumeType pure virtual. Compile timer error is preferable to run time error
//
// Revision 1.3  2002/04/01 21:24:58  michael
// added missing public to the enum (default is private!!!)
//
// Revision 1.2  2002/03/15 21:19:42  michael
// further progress on the 2D renderers
//
// Revision 1.1  2002/03/15 01:27:55  michael
// added initial version of software renderers (still buggy)
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererSoftwareMPR.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
// $Id: vxRendererSoftwareMPR.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
