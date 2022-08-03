// $Id: vxManipulatorDataRendererSoftware3d.h,v 1.2 2007/01/16 17:04:18 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

/**
 *   manipulator for the data of render Cpu3D
 */

#ifndef vxManipulatorDataRendererSoftware3d_h
#define vxManipulatorDataRendererSoftware3d_h


// includes
#include "vxManipulator.h"


// class declaration
class VX_VIEWER_DLL vxManipulatorDataRendererSoftware3d : public vxManipulator
{
/// functions
public:

  /// default constructor
  vxManipulatorDataRendererSoftware3d(vxEnvironment & environment);

  /// default destructor
  virtual ~vxManipulatorDataRendererSoftware3d() {}

  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive keyboard events for the viewer
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

private:

  /// copy constructor
  vxManipulatorDataRendererSoftware3d(const vxManipulatorDataRendererSoftware3d & other);                     // should only be public if really implemented!

  /// assignment operator
  const vxManipulatorDataRendererSoftware3d & operator=(const vxManipulatorDataRendererSoftware3d & other); // should only be public if really implemented!

// data
private:

  /// dragging the light?
  bool m_bDraggingLightSource;
}; // vxManipulatorDataRendererSoftware3d


#endif // vxManipulatorDataRendererSoftware3d_h


// $Log: vxManipulatorDataRendererSoftware3d.h,v $
// Revision 1.2  2007/01/16 17:04:18  gdavidson
// Removed OnTimer
//
// Revision 1.1  2004/08/30 17:15:12  michael
// Data used by renderers is now polymorph as well. Adjusted manipulator, too.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/16 13:22:52  frank
// formatting
//
// Revision 1.3  2003/05/08 16:39:05  michael
// more code reviews (75 renderer Utils)
//
// Revision 1.2  2003/05/01 18:50:19  dongqing
// code review
//
// Revision 1.1  2003/04/29 21:54:17  michael
// initial version (renamed RenderQualitySpeedTradeoff to vxRendererCpu3dData)
//
// Revision 1.8  2002/10/23 23:13:09  ingmar
// RendererQualitySpeedTradeoff now only keeps if high or low framrerate is desired (fly vs examine) the Cpu3D rederer keeps internally what the curent image size is and decides no its own in PstRender() how to change the resolution. the "f" key toggles between the two frame rate modes
// I also cleand out the too general sliders of the RendererQualitySpeedTradeoff. The class now only contains concrete elements that are actually used
//
// Revision 1.7  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.6  2002/08/20 22:43:49  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.5  2002/08/05 16:20:36  dmitry
// Event name added to OnTimer.
//
// Revision 1.4  2002/07/11 16:34:45  ingmar
// added timer for QualitySpeedTradeoff
//
// Revision 1.3  2002/07/02 20:44:59  geconomos
// Exporting classes .
//
// Revision 1.2  2002/06/25 19:33:02  soeren
// added initial version of moveable light
//
// Revision 1.1  2002/03/29 01:39:56  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorDataRendererSoftware3d.h,v 1.2 2007/01/16 17:04:18 gdavidson Exp $
// $Id: vxManipulatorDataRendererSoftware3d.h,v 1.2 2007/01/16 17:04:18 gdavidson Exp $