// $Id: vxManipulatorResearch.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)

/*
 * Manipulator for research use only.
 */


#ifndef vxManipulatorResearch_h
#define vxManipulatorResearch_h


// Includes
#include "vxManipulator.h"
#include "Point.h"


// Class declaration
class VX_VIEWER_DLL vxManipulatorResearch : public vxManipulator
{
// functions
public:

  /// constructor
  vxManipulatorResearch(vxEnvironment & environment);

  /// destructor
  ~vxManipulatorResearch() {};

  /// event handler can be reimplemented in a subclass to receive wheel events for the viewer
  virtual bool OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState);

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
  vxManipulatorResearch(const vxManipulatorResearch & other);                   // should only be public if really implemented!
  
  /// assignment operator
  const vxManipulatorResearch & operator=(const vxManipulatorResearch & other); // should only be public if really implemented!

  /// load a label volume
  void LoadLabelVolume(const std::string & sFilename);
  
  /// write the label volume
  void WriteLabelVolume(const std::string & sFilename);

// data
private:
  
  // last selected point
  Point<float4> m_lastSelectedPoint;
}; // vxManipulatorResearch


#endif // vxManipulatorResearch_h


// Revision History:
// $Log: vxManipulatorResearch.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/07/11 19:49:15  michael
// final code reviews
//
// Revision 1.9  2002/12/17 14:38:55  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.8  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.7  2002/08/20 22:44:07  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.6  2002/08/05 16:20:36  dmitry
// Event name added to OnTimer.
//
// Revision 1.5  2002/07/02 20:44:59  geconomos
// Exporting classes .
//
// Revision 1.4  2002/06/26 20:15:11  frank
// Added two click vessel segmentation interface.
//
// Revision 1.3  2002/06/26 17:32:41  frank
// Implemented reading and writing label volume.
//
// Revision 1.2  2002/06/21 19:17:55  frank
// cleaned out
//
// Revision 1.1  2002/06/21 19:03:20  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorResearch.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxManipulatorResearch.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
