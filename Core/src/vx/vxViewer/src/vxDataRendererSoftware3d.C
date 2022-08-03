// $Id: vxDataRendererSoftware3d.C,v 1.4 2004/10/18 14:49:16 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// includes
#include "StdAfx.h"
#include "vxDataRendererSoftware3d.h"


// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Default constructor
 */
vxDataRendererSoftware3d::vxDataRendererSoftware3d()
:m_fDepthImage(0)
//*******************************************************************
{
} // vxDataRendererSoftware3d()


/**
 * Copy constructor
 * @param other the other data
 */
vxDataRendererSoftware3d::vxDataRendererSoftware3d(const vxDataRendererSoftware3d & other)
:m_fDepthImage(0)
//*******************************************************************
{
  * this = other;
} // vxDataRendererSoftware3d(other)


/**
 * Assignment operator =
 * @param other the other data
 * @return const this reference
 */
const vxDataRendererSoftware3d & vxDataRendererSoftware3d::operator =(const vxDataRendererSoftware3d & other)
//*******************************************************************
{
  if (this != & other)
  {
    m_fDepthImage = other.m_fDepthImage;
  }

  return * this;
} // operator =()


// undefines
#undef FILE_REVISION


// $Log: vxDataRendererSoftware3d.C,v $
// Revision 1.4  2004/10/18 14:49:16  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.3  2004/10/05 14:29:34  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.2  2004/09/21 15:51:14  frank
// added shadow flag
//
// Revision 1.1  2004/08/30 17:15:51  michael
// Data used by renderers is now polymorph as well. Adjusted manipulator, too.
//
// Revision 1.3  2004/05/12 19:26:27  frank
// improved movies, added more profiling
//
// Revision 1.2  2004/03/30 18:18:20  frank
// fixed old cut-paste error
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2004/02/10 19:22:53  michael
// fixed light to be initialized correctly
//
// Revision 1.7  2004/02/07 03:43:30  michael
// fixed light source/direction issue in 3d rendering by making the mouse position
// relative to the viewport size instead of absolut. Now the change in vxWinRect
// has correctly no impact on shading.
//
// Revision 1.6  2003/11/17 13:22:33  michael
// changed the initialization of the quality speed factor
//
// Revision 1.5  2003/11/17 01:04:15  michael
// changed default value for speed quality scale factor
//
// Revision 1.4  2003/11/16 23:39:02  michael
// added ability to controle the rendering speed/quality ...
//
// Revision 1.3  2003/09/22 15:53:02  michael
// added jittering, shading, and preintegration flags as well as sampling rate
//
// Revision 1.2  2003/07/29 18:34:29  michael
// added mode to be able to prevent progressive refinement and set image size fixed.
//
// Revision 1.1  2003/04/29 21:54:17  michael
// initial version (renamed RenderQualitySpeedTradeoff to vxDataRendererSoftware3d)
//
// Revision 1.10  2002/12/20 19:01:15  ingmar
// removed single scalar constructor from PixelRGBA
//
// Revision 1.9  2002/11/19 19:05:45  ingmar
// added autoFlyMode
//
// Revision 1.8  2002/11/11 16:07:18  ingmar
// changed inital value of high framerate demand to low (false)
//
// Revision 1.7  2002/11/05 20:43:39  geconomos
// Initailized m_bStartBenchmark in constructor.
//
// Revision 1.6  2002/10/23 23:13:08  ingmar
// RendererQualitySpeedTradeoff now only keeps if high or low framrerate is desired (fly vs examine) the Cpu3D rederer keeps internally what the curent image size is and decides no its own in PstRender() how to change the resolution. the "f" key toggles between the two frame rate modes
// I also cleand out the too general sliders of the RendererQualitySpeedTradeoff. The class now only contains concrete elements that are actually used
//
// Revision 1.5  2002/09/30 14:51:20  ingmar
// removed vector of bool toggle modes
//
// Revision 1.4  2002/07/16 20:48:54  ingmar
// added mode array
//
// Revision 1.3  2002/07/11 16:34:11  ingmar
// added timer for QualitySpeedTradeoff
//
// Revision 1.2  2002/06/25 19:33:01  soeren
// added initial version of moveable light
//
// Revision 1.1  2002/03/29 01:40:15  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxDataRendererSoftware3d.C,v 1.4 2004/10/18 14:49:16 frank Exp $
// $Id: vxDataRendererSoftware3d.C,v 1.4 2004/10/18 14:49:16 frank Exp $
