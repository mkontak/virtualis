// $Id: vxManipulatorAnnotationCreate.h,v 1.2 2005/05/24 13:50:02 michael Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)

/*
  This class inherits from Manipulator but also provides the
  interface to a HandlesSet. All operations on a given
  HandlesObj should be implemented as an implementation
  (derivation) of this class.
*/

#ifndef vxManipulatorAnnotationCreate_h
#define vxManipulatorAnnotationCreate_h


// includes
#include "vxManipulator.h"


// forward declarations
template <class T> class vxShareableObject;
class vxDataAnnotationCreate;


// class definition
class VX_VIEWER_DLL vxManipulatorAnnotationCreate : public vxManipulator
{
// functions
public:
  
  /// constructor
  vxManipulatorAnnotationCreate(vxEnvironment & environment);
  
  /// destructor
  ~vxManipulatorAnnotationCreate();

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
  
  /// get status on dragging a point
  bool IsDragging() { return m_bDragging; };

  /// set status on dragging a point
  void SetDragging(bool bValue) { m_bDragging = bValue; };

private:
  
  /// copy constructor
  vxManipulatorAnnotationCreate(vxManipulatorAnnotationCreate & other);             // should only be public if really implemented!
  
  /// assignment operator
  vxManipulatorAnnotationCreate & operator=(vxManipulatorAnnotationCreate & other); // should only be public if really implemented!

  /// checks if the active annotation is supported for the viewer type
  bool IsAnnotationSupported() const;

// data:
private:

  /// are we dragging the mouse
  bool m_bDragging;

  /// cached pointer to the data object in environment
  vxDataAnnotationCreate * m_pDataAC;

  /// cached pointer to shareable data object in environment
  vxShareableObject<vxDataAnnotationCreate> * m_pShareableObjectDataAC;

}; // ManipulatorCreateHandlesObj


#endif // ManipulatorCreateHandlesObj_h


// Revision History:
// $Log: vxManipulatorAnnotationCreate.h,v $
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
// Revision 1.7  2004/01/05 19:06:10  michael
// added "mailto:"
//
// Revision 1.6  2003/06/19 20:21:21  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.5  2003/05/16 13:21:33  frank
// formatting
//
// Revision 1.4  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.3  2003/03/25 15:35:25  geconomos
// Issue # 2949: Call to vxViewerType::IsAnnotationSupported on every OnXXX function.
//
// Revision 1.2  2003/03/04 01:43:24  michael
// fixed 'ESC' on creating annotations
//
// Revision 1.1  2003/01/09 14:57:53  michael
// added new manipulator to creat the new annotation objects
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorAnnotationCreate.h,v 1.2 2005/05/24 13:50:02 michael Exp $
// $Id: vxManipulatorAnnotationCreate.h,v 1.2 2005/05/24 13:50:02 michael Exp $