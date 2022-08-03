// $Id: vxPlane.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Michael Meissner (michael@viatronix.com)

/**
 * This class is the common plane base class
 */

#ifndef vxPlane_h
#define vxPlane_h

// class definition
class VX_VIEWER_DLL vxPlane 
{
// functions
public:

  /// default constructor
  vxPlane() {};

  /// destructor
  virtual ~vxPlane() {};

private:

  /// copy constructor
  vxPlane(const vxPlane & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxPlane & operator=(const vxPlane & other); // should only be public if really implemented!

  /// needed to have this class being polymorphic (at least one virtual method)
  virtual void DoNothing() {};
}; // vxPlane


#endif // vxPlane_h


// $Log: vxPlane.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/06/04 19:18:28  jmeade
// code review.
//
// Revision 1.8  2003/05/16 13:08:50  frank
// code formatting
//
// Revision 1.7  2003/05/16 11:48:48  michael
// code review
//
// Revision 1.6  2003/05/14 19:07:04  dongqing
// code review
//
// Revision 1.5  2003/04/24 13:46:28  michael
// initial code reviews
//
// Revision 1.4  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.3  2003/01/02 19:39:45  ingmar
// better documentation structure
//
// Revision 1.2  2001/01/02 18:53:35  ingmar
// better documentation structure
//
// Revision 1.1  2002/11/18 15:53:08  michael
// added renamed versions of the plane objects using vx prefix
//
// Revision 1.8  2002/11/13 14:48:23  geconomos
// Removed EVENT_FLOAT_DISTANCE
//
// Revision 1.7  2002/08/21 15:49:07  frank
// Exported classes to DLL.
//
// Revision 1.6  2002/05/08 18:22:35  dmitry
// EVENT_FLOAT_DISTANCE added.
//
// Revision 1.5  2002/04/29 20:30:11  uday
// added enum EVENT_MODIFIED
//
// Revision 1.4  2002/04/11 15:26:19  dmitry
// Added event enum.
//
// Revision 1.3  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 1.2  2002/03/06 15:38:43  jaddonisio
// Fixed UnitTest().
//
// Revision 1.1  2002/03/06 00:20:49  jaddonisio
// Initial version.
//
// Revision 1.8  2002/03/04 20:33:22  michael
// adjusted all PlaneOrtho and PlaneOblique instantiations with the new
// templetized types ...
//
// Revision 1.7  2002/03/04 16:43:28  michael
// fomatting and coding guidelines
//
// Revision 1.6  2002/02/25 22:07:29  michael
// added one virtual function to get polymorphism and to be able to perform dynamic_cast<>()
//
// Revision 1.5  2002/02/25 13:20:46  michael
// made planeOrtho a real derivative of planeOblique
//
// Revision 1.4  2001/12/18 17:19:00  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 1.3  2001/12/12 23:55:17  michael
// 3d texture mapping is now compiling through
//
// Revision 1.2  2001/12/07 17:25:25  michael
// Removed plane pointer since this a only an interface, cintaining no
// implementation at all.
//
// Revision 1.1  2001/12/07 14:28:47  michael
// Added a simple plane object which has only get/set methods for the plane.
// Ortho, Oblique, and Curved should be derived from it. Furthermore, Ortho
// could be used in 2D TexMap and Oblique in 3D TexMap.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxPlane.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxPlane.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
