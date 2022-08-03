// $Id: vxManipulatorMultiClick.h,v 1.1.2.2 2009/07/30 19:23:42 kchalupa Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Jeff Meade (mailto:jmeade@viatronix.com)

/**
 * This class inherits all inputs and has interfaces to provide
 * the current mouse position and window size to perform certain
 * manipulations of data that is relative or absolute to the mouse
 * movement, click, etc.
 */

#ifndef vxManipulatorMultiClick_h
#define vxManipulatorMultiClick_h


// includes
#include "vxInput.h"
#include "Point2D.h"
#include "vxWinRect.h"
#include "vxUtils.h"


// forwards
class vxEnvironment;


// class definition
class VX_VIEWER_DLL vxManipulatorMultiClick :  public vxInput
{
// construction
public:
  
  /// default constructor
  vxManipulatorMultiClick(vxEnvironment & environment);
  
  /// destructor
  virtual ~vxManipulatorMultiClick() {};
  
private:
  
  /// default constructor
  vxManipulatorMultiClick();  

  /// Copy constructor
  vxManipulatorMultiClick(const vxManipulatorMultiClick & other);

// member functions
public:

  /// get mouse position
  const Point2D<int2> & GetMousePosition() const { return m_mousePosition; };
  
  /// get last selected mouse position
  const Point2D<int2> & GetLastMousePosition() const { return m_lastMousePosition; };

  /// set the environment
  void SetEnvironment(vxEnvironment & environment) { m_pEnvironment = &environment; };

  /// get the environment
  vxEnvironment & GetEnvironment() const { return *m_pEnvironment; };

protected:
  
  /// set last mouse position, done internally
  void SetMousePosition(const Point2D<int2> & position) { m_mousePosition = position; };
  
  /// set last mouse position, done internally
  void SetLastMousePosition(const Point2D<int2> & position) { m_lastMousePosition = position; };

  /// Get the multiplier for the shift/alt/control keys when manipulating a linear variable (e.g., slice number).
  static int4 GetKeyMultiplier( const uint4 uKeyboardState );

// operators
private:
  
  /// Assignment operator
  const vxManipulatorMultiClick & operator=(const vxManipulatorMultiClick & other);

// member variables
private:

  /// environment
  vxEnvironment * m_pEnvironment;

  /// mouse position inside the window
  Point2D<int2> m_mousePosition;
  
  /// last mouse position inside the window
  Point2D<int2> m_lastMousePosition;

}; // vxManipulatorMultiClick


#endif // vxManipulatorMultiClick_h


// $Log: vxManipulatorMultiClick.h,v $
// Revision 1.1.2.2  2009/07/30 19:23:42  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.1  2009/02/20 22:28:52  jmeade
// initial version.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorMultiClick.h,v 1.1.2.2 2009/07/30 19:23:42 kchalupa Exp $
// $Id: vxManipulatorMultiClick.h,v 1.1.2.2 2009/07/30 19:23:42 kchalupa Exp $
