// $Id: vxCursor3d.h,v 1.2 2005/03/21 20:48:58 michael Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner, meissner@viatronix.com

/*
 This is kind of silly because we need a new type to be able to have
 Modified() work. If we would simply take a camera, then the rendering
 would always be triggered because the Modified checks for type()
*/

#ifndef vxCursor3d_h
#define vxCursor3d_h


// includes
#include "vxCamera.h"


// class definition
class VX_VIEWER_DLL vxCursor3d : public vxCamera
{
// functions
public:

  /// default constructor
  vxCursor3d(): vxCamera() {}

  /// default constructor
  vxCursor3d(const vxCamera & camera): vxCamera(camera) {}

  /// destructor
  ~vxCursor3d() {};

  /// get position
  Point<float4> GetPosition() { return GetEye(); }

  /// set position
  void SetPosition(const Point<float4> & position) { SetEye(position); }

  /// get reference
  Point<float4> GetReference() { return GetViewReference(); }

  /// set reference
  void SetReference(const Point<float4> & reference);

  /// get view direction
  Normal3Df GetDirection() { return GetViewDirection(); }

  /// set view direction
  void SetDirection(const Normal3Df & viewDirection) { SetViewDirection( viewDirection ); }

  /// get view up direction
  Normal3Df GetViewUp() { return vxCamera::GetViewUp(); }

  /// set view up direction
  void SetViewUp(const Normal3Df & viewUpDirection) { vxCamera::SetViewUp( viewUpDirection ); }

  /// assignment operator
  const vxCursor3d & operator=(const vxCursor3d & other);

  // get display mode
  bool IsCrossHairEnabled() { return m_bShowCrossHair; }

  // set cross hair on/off
  void EnableCrossHair(bool bFlag) { m_bShowCrossHair = bFlag; }

private:

  /// copy constructor
  vxCursor3d(const vxCursor3d & other);                   // should only be public if implemented

  // display mode for cross hair 
  // (BAD, should be a data in the environment. Do not mix model
  // and GUI display mode properties in ONE class. Bad style!!!)
  bool m_bShowCrossHair;

}; // vxCursor3D


#endif // vxCursor3d_h


// Revision History:
// $Log: vxCursor3d.h,v $
// Revision 1.2  2005/03/21 20:48:58  michael
// Added code to trigger the display of the cross hair
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/10/01 21:23:52  michael
// cosmetics
//
// Revision 1.4  2003/06/25 20:24:59  frank
// Added direction functions
//
// Revision 1.3  2003/05/06 15:40:14  michael
// initial code reviews
//
// Revision 1.2  2003/05/06 15:39:26  michael
// initial code reviews
//
// Revision 1.1  2003/04/24 14:01:06  michael
// added vxCursor3d class
//
// $Id: vxCursor3d.h,v 1.2 2005/03/21 20:48:58 michael Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxCursor3d.h,v 1.2 2005/03/21 20:48:58 michael Exp $
