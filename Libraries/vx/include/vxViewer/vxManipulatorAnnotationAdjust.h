// $Id: vxManipulatorAnnotationAdjust.h,v 1.3 2006/08/03 22:18:48 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/*
  This class inherits from Manipulator but also provides the
  interface to a HandlesSet. All operations on a given
  HandlesObj should be implemented as an implementation
  (derivation) of this class.
*/

#ifndef vxManipulatorAnnotationAdjust_h
#define vxManipulatorAnnotationAdjust_h


// includes
#include "vxManipulator.h"
#include "vxPlaneOrtho.h"
#include "vxPlaneOblique.h"
#include "vxPlaneCurved.h"


// class definition
class VX_VIEWER_DLL vxManipulatorAnnotationAdjust : public vxManipulator
{
// functions
public:
  
  /// constructor
  vxManipulatorAnnotationAdjust(vxEnvironment & environment);
  
  /// destructor
  ~vxManipulatorAnnotationAdjust() {};

  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse down events for the viewer
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// event handler can be reimplemented in a subclass to receive mouse up events for the viewer
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

	///Event handler checks for the del key
	virtual bool OnKeyReleased(const uint1 uKeyId, const uint4 uKeyboardState);
  
  /// called when the mouse cursor is to be set
  virtual bool OnSetCursor();

  /// get dragging
  bool IsDragging() { return m_bDragging; };

  /// get dragging
  void SetDragging(bool bFlag) { m_bDragging = bFlag; };

private:
  
  /// copy constructor
  vxManipulatorAnnotationAdjust(vxManipulatorAnnotationAdjust & other);             // should only be public if really implemented!

  /// assignment operator
  vxManipulatorAnnotationAdjust & operator=(vxManipulatorAnnotationAdjust & other); // should only be public if really implemented!

  /// get the selected handle object index
  int4 GetSelectedHandleObjIndex() { return m_iSelectedHandleObjIndex; };

  /// set the selected handle object index
  void SetSelectedHandleObjIndex(int iIndex) { m_iSelectedHandleObjIndex = iIndex; };

  /// get the selected handle index
  int4 GetSelectedHandleIndex() { return m_iSelectedHandleIndex; };

  /// set the selected handle index
  void SetSelectedHandleIndex(int iIndex) { m_iSelectedHandleIndex = iIndex; };

  /// is label selected
  bool IsLabelSelected() { return m_bSelectedLabel; };

  /// set label selected
  void SetLabelSelected(bool bSelected) { m_bSelectedLabel = bSelected; };

  /// is label selected
  bool IsTextSelected() { return m_bSelectedText; };

  /// set label selected
  void SetTextSelected(bool bSelected) { m_bSelectedText = bSelected; };
  
  /// indicates if adjustment is allowed
  bool CanAdjust() const;

  /// set the plane ortho pointer
  void SetPlaneOrtho(vxPlaneOrtho<float4> * pPlaneOrtho) { m_planeOrtho = *pPlaneOrtho; }

  /// get the plane ortho plane pointer
  vxPlaneOrtho<float4> & GetPlaneOrtho() { return m_planeOrtho; }

  /// set the plane ortho pointer
  void SetPlaneOblique(vxPlaneOblique<float4> * pPlaneOblique) { m_planeOblique = *pPlaneOblique; }

  /// get the plane ortho plane pointer
  vxPlaneOblique<float4> & GetPlaneOblique() { return m_planeOblique; }

  /// set the plane curved pointer
  void SetPlaneCurved(vxPlaneCurved * pPlaneCurved) { m_planeCurved = *pPlaneCurved; }

  /// get the plane curved pointer
  vxPlaneCurved & GetPlaneCurved() { return m_planeCurved; }

  /// test whether we need to let a selected handle go while dragging
  bool DiscardSelection();

  /// cache planes
  void CachePlane();

// data
private:

  /// are we dragging something
  bool m_bDragging;

  /// selected handle object index
  int4 m_iSelectedHandleObjIndex;

  /// selected handle index
  int4 m_iSelectedHandleIndex;

  /// is label selected
  bool m_bSelectedLabel;

  /// is text selected
  bool m_bSelectedText;

  // plane ortho
  mutable vxPlaneOrtho<float4> m_planeOrtho;

  // plane oblique
  mutable vxPlaneOblique<float4> m_planeOblique;

  // plane curved
  mutable vxPlaneCurved m_planeCurved;
}; // ManipulatorMoveHandlesObj


#endif // vxManipulatorAnnotationAdjust_h


// Revision History:
// $Log: vxManipulatorAnnotationAdjust.h,v $
// Revision 1.3  2006/08/03 22:18:48  romy
// added the delete Key Press - testing
//
// Revision 1.2  2005/03/08 19:09:24  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.1  2005/02/16 20:28:16  michael
// Fixed issue #3964 by checking the plane object while dragging so that in case
// the plane changes in the meantime, the dragging of a handle/text/label is
// suspended.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/06/19 20:21:21  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.6  2003/06/04 13:27:30  geconomos
// Disabled annotation support when in MIP, thin slab and X-Ray rendering modes.
//
// Revision 1.5  2003/05/16 13:21:33  frank
// formatting
//
// Revision 1.4  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.3  2003/01/17 00:33:22  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.2  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.1  2003/01/14 23:25:07  michael
// added initial version of annotation adjust manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorAnnotationAdjust.h,v 1.3 2006/08/03 22:18:48 romy Exp $
// $Id: vxManipulatorAnnotationAdjust.h,v 1.3 2006/08/03 22:18:48 romy Exp $
