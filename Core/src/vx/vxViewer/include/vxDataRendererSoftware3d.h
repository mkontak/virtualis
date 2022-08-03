// $Id: vxDataRendererSoftware3d.h,v 1.6 2004/11/17 22:34:17 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner {meissner@viatronix.com)

/*
  Encapsulate the data a renderer might need
*/

#ifndef vxDataRendererSoftware3d_h
#define vxDataRendererSoftware3d_h


// includes
#include "vxDataRendererSoftware.h"


// class declaration 
class VX_VIEWER_DLL vxDataRendererSoftware3d : public vxDataRendererSoftware
{
// functions
public:

  /// default constructor
  vxDataRendererSoftware3d();

  /// destructor
  virtual ~vxDataRendererSoftware3d() {}

  /// copy constructor
  vxDataRendererSoftware3d(const vxDataRendererSoftware3d & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxDataRendererSoftware3d & operator=(const vxDataRendererSoftware3d & other); // should only be public if really implemented!

  /// get depth image
  Image<float4> & GetDepthImage() { return m_fDepthImage; }

// data
private:

  /// image that stores the depth per ray generated in software raycasting
  Image<float4> m_fDepthImage;

}; // class vxDataRendererSoftware3d


#endif // vxDataRendererSoftware3d_h


// $Log: vxDataRendererSoftware3d.h,v $
// Revision 1.6  2004/11/17 22:34:17  geconomos
// made destructor virtual
//
// Revision 1.5  2004/10/18 14:45:15  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.4  2004/10/05 14:29:12  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.3  2004/09/21 15:51:14  frank
// added shadow flag
//
// Revision 1.2  2004/09/09 14:55:24  michael
// Added frame-time to vxDataRenderer
//
// Revision 1.1  2004/08/30 17:15:13  michael
// Data used by renderers is now polymorph as well. Adjusted manipulator, too.
//
// Revision 1.4  2004/05/18 14:57:44  vxconfig
// cosmetics
//
// Revision 1.3  2004/05/12 19:26:27  frank
// improved movies, added more profiling
//
// Revision 1.2  2004/03/30 18:18:20  frank
// fixed old cut-paste error
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2004/02/07 03:43:30  michael
// fixed light source/direction issue in 3d rendering by making the mouse position
// relative to the viewport size instead of absolut. Now the change in vxWinRect
// has correctly no impact on shading.
//
// Revision 1.8  2003/11/17 13:22:33  michael
// changed the initialization of the quality speed factor
//
// Revision 1.7  2003/11/16 23:39:02  michael
// added ability to controle the rendering speed/quality ...
//
// Revision 1.6  2003/09/22 15:53:02  michael
// added jittering, shading, and preintegration flags as well as sampling rate
//
// Revision 1.5  2003/08/13 12:43:42  michael
// added derivation for 70_vxUtils
//
// Revision 1.4  2003/07/29 18:34:29  michael
// added mode to be able to prevent progressive refinement and set image size fixed.
//
// Revision 1.3  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.2  2003/05/01 18:51:01  dongqing
// code review
//
// Revision 1.1  2003/04/29 21:54:17  michael
// initial version (renamed RenderQualitySpeedTradeoff to vxDataRendererSoftware3d)
//
// Revision 1.13  2002/11/19 19:05:46  ingmar
// added autoFlyMode
//
// Revision 1.12  2002/11/08 19:00:23  ingmar
// moved the visible data bounding box into the environment to be accessable by the manip CamFly
//
// Revision 1.11  2002/11/08 17:19:28  ingmar
// added depth image
//
// Revision 1.10  2002/10/23 23:13:08  ingmar
// RendererQualitySpeedTradeoff now only keeps if high or low framrerate is desired (fly vs examine) the Cpu3D rederer keeps internally what the curent image size is and decides no its own in PstRender() how to change the resolution. the "f" key toggles between the two frame rate modes
// I also cleand out the too general sliders of the RendererQualitySpeedTradeoff. The class now only contains concrete elements that are actually used
//
// Revision 1.9  2002/09/30 14:51:20  ingmar
// removed vector of bool toggle modes
//
// Revision 1.8  2002/07/16 20:48:54  ingmar
// added mode array
//
// Revision 1.7  2002/07/12 21:21:00  ingmar
// fixed space leaoing, current framerate 512^3 256^2 => 5.3
//
// Revision 1.6  2002/07/11 22:59:10  ingmar
// added event for modification of light
//
// Revision 1.5  2002/07/11 16:34:11  ingmar
// added timer for QualitySpeedTradeoff
//
// Revision 1.4  2002/07/03 13:16:30  geconomos
// Exported class.
//
// Revision 1.3  2002/06/25 19:33:01  soeren
// added initial version of moveable light
//
// Revision 1.2  2002/05/01 13:49:21  michael
// cosmetics
//
// Revision 1.1  2002/03/29 01:40:15  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxDataRendererSoftware3d.h,v 1.6 2004/11/17 22:34:17 geconomos Exp $
// $Id: vxDataRendererSoftware3d.h,v 1.6 2004/11/17 22:34:17 geconomos Exp $