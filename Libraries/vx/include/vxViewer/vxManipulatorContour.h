// $Id: vxManipulatorContour.h,v 1.4.10.1 2009/08/31 13:15:59 kchalupa Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Manjushree Nulkar (manju@viatronix.com)

/**
 * Manipulator to create a restricted lasso.
 */


// pragmas
#pragma once


// includes
#include "vxManipulator.h"
#include "Point3D.h"


// forward declarations
class vxContour;
class vxContourList;


// class definition
class VX_VIEWER_DLL vxManipulatorContour : public vxManipulator
{
// functions
public:

  //constructor
  vxManipulatorContour(vxEnvironment & environment);

  //destructor
  ~vxManipulatorContour();

  /// event handler can be reimplemented in a subclass to receive mouse move events for the viewer.     
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);

  /// process button down event
  virtual bool OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// process button up event
  virtual bool OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState);

  /// process key pressed
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

  /// called when the mouse cursor is to be set
  virtual bool OnSetCursor();
  
  /// create a new component.
  void CreateNewComponent( const uint4 uKeyboardState );

private:

  /// copy constructor
  vxManipulatorContour(const vxManipulatorContour & other);                   // should only be public if really implemented!
  
  /// assignment operator
  const vxManipulatorContour & operator=(const vxManipulatorContour & other); // should only be public if really implemented!
  
  /// check if a lasso was already created on the current slice
  void CheckForLassoOnSameSlice(Point3Df & mouseWorldPos, vxContourList & pLassoElement);

  /// insert a newly created lasso in the correct position in the list
  void InsertionSortList( vxContour * pContour, vxContourList & pLassoElement );
  
  /// returns the current plane distance
  float4 GetCurrentPlaneDistance();
  
  /// returns index of the lasso to be copied
  void GetCurrentLasso();

  /// copy the selected lasso on the desired slice
  void CopyLasso();

protected:

  /// Gets the active state of the tool.
  bool GetActive() { return m_bActive; }

  /// Sets the active state of the tool.
  void SetActive(bool bValue) { m_bActive = bValue; }

  /// clear out all the data and reset the state to defaults
  void Clear();

// data
private:

  // pointer to the current lasso created before it is put into the list
  vxContour * m_pCurrentLasso;

  // index of lasso to be copied
  int2 m_iLassoIndex;

  // indicates if the restricted lasso is being manipulated
  bool m_bActive;

}; // vxManipulatorContour


// $Log: vxManipulatorContour.h,v $
// Revision 1.4.10.1  2009/08/31 13:15:59  kchalupa
// Expose active variable to derived classes to enable correct out of bounds handling.
//
// Revision 1.4  2006/03/13 13:53:24  frank
// formatting
//
// Revision 1.3  2004/03/12 19:53:53  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.2  2004/03/12 18:03:37  frank
// working on removing annotation ROI - first renaming contour to contour list
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2004/01/14 17:15:51  frank
// cleared the old contour in the destructor
//
// Revision 1.9  2003/12/16 20:21:14  frank
// Issue #3593: Restricted the segmentation operations to the visible volume including crop box and thin slab when using the shift key
//
// Revision 1.8  2003/05/15 20:07:52  frank
// Moving away from old point class
//
// Revision 1.7  2003/05/07 15:01:30  dongqing
// code walkthrough
//
// Revision 1.6  2003/05/07 11:44:02  michael
// initial code review
//
// Revision 1.5  2003/04/08 20:34:23  frank
// Issue #3062: Trying to reproduce problem with buggy contour drawing
//
// Revision 1.4  2003/02/25 22:11:48  frank
// Fixed problem with pan/zoom window killing selection points
//
// Revision 1.3  2003/02/25 16:38:18  frank
// Believe to have fixed the contour missing slices
//
// Revision 1.2  2002/11/25 13:44:56  geconomos
// New mouse cursor handling.
//
// Revision 1.1  2002/11/06 18:29:38  frank
// Copied RestrictedLasso from CS and renamed Contour
//
// Revision 1.7  2002/09/26 18:08:57  dmitry
// Cleaned up mouse input handlers and orientation change handled.
//
// Revision 1.6  2002/09/18 20:17:33  dmitry
// Cursors added.
//
// Revision 1.5  2002/09/16 20:20:39  dmitry
// New vxID (s) used.
//
// Revision 1.4  2002/08/20 22:44:25  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.3  2002/07/11 21:09:11  manju
// Speed up.
//
// Revision 1.2  2002/07/10 15:13:31  manju
// Added methods to implement copy-paste of a lasso and creation of
// new plaque.
//
// Revision 1.1  2002/07/10 13:49:01  manju
// Initial revision.
//
// Revision 1.3  2002/06/27 12:07:49  manju
// Added OnKeyPressed method.
//
// Revision 1.2  2002/06/26 17:24:58  manju
// Modified the code to get it working,
//
// Revision 1.1  2002/06/19 14:24:20  manju
// Manipulator for restricted lasso.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorContour.h,v 1.4.10.1 2009/08/31 13:15:59 kchalupa Exp $
// $Id: vxManipulatorContour.h,v 1.4.10.1 2009/08/31 13:15:59 kchalupa Exp $
