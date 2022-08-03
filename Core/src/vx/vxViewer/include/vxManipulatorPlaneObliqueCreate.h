// $Id: vxManipulatorPlaneObliqueCreate.h,v 1.2 2005/05/24 13:50:02 michael Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

/*
  This class implements a manipulator to create a PlaneOblique
*/

#ifndef vxManipulatorPlaneObliqueCreate_h
#define vxManipulatorPlaneObliqueCreate_h


// includes
#include "vxManipulator.h"


// forward declarations
template <class T> class vxShareableObject;
class vxDataPlaneObliqueCreate;


// class definition
class VX_VIEWER_DLL vxManipulatorPlaneObliqueCreate : public vxManipulator
{
// functions
public:
  
  /// default constructor
  vxManipulatorPlaneObliqueCreate(vxEnvironment & environment);
  
  /// default destructor
  ~vxManipulatorPlaneObliqueCreate();
  
  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);
  
  /// event handler can be reimplemented in a subclass to receive keyboard events for the viewer
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

  /// called when the mouse cursor is to be set
  virtual bool OnSetCursor();

private:
  
  /// copy constructor
  vxManipulatorPlaneObliqueCreate(const vxManipulatorPlaneObliqueCreate &);                   // should only be public if really implemented!
  
  /// assignment operator
  const vxManipulatorPlaneObliqueCreate & operator=(const vxManipulatorPlaneObliqueCreate &); // should only be public if really implemented!

  /// are we in dragging mode?
  bool IsDragging() { return m_bDragging; }

  /// set dragging mode
  void SetDragging(bool bFlag) { m_bDragging = bFlag; }

// data
private:

  /// create the oblique plane from two points and camera
  void CreatePlaneObliqueAndCamera(vxDataPlaneObliqueCreate & data);

  /// dragging state
  bool m_bDragging;

  /// cached pointer to the data object in environment
  vxDataPlaneObliqueCreate * m_pDataPOC;

  /// cached pointer to shareable data object in environment
  vxShareableObject<vxDataPlaneObliqueCreate> * m_pShareableObjectDataPOC;

}; // class vxManipulatorPlaneObliqueCreate


#endif // vxManipulatorPlaneObliqueCreate_h


// Revision History:
// $Log: vxManipulatorPlaneObliqueCreate.h,v $
// Revision 1.2  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/09/23 22:10:22  michael
// took out redundant code
//
// Revision 1.4  2003/08/20 14:21:56  michael
// added dragging and OnMouseEnter() and OnMouseLeave()
//
// Revision 1.3  2003/08/13 19:24:54  michael
// plane oblique creating, added environment to dynamic environment data
//
// Revision 1.2  2003/05/15 20:07:52  frank
// Moving away from old point class
//
// Revision 1.1  2002/10/17 15:27:03  frank
// Initial versions.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorPlaneObliqueCreate.h,v 1.2 2005/05/24 13:50:02 michael Exp $
// $Id: vxManipulatorPlaneObliqueCreate.h,v 1.2 2005/05/24 13:50:02 michael Exp $
