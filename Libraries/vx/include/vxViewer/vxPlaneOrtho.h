// $Id: vxPlaneOrtho.h,v 1.3 2006/08/07 01:26:31 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)

#ifndef vxPlaneOrtho_h
#define vxPlaneOrtho_h


// includes
#include "vxPlaneOblique.h"
#include "vxElement.h"


/**
 * This class provides an orthonormal (to xyz axis) plane description.
 */
template <class T>
class vxPlaneOrtho : public vxElement, public vxPlaneOblique<T>
{
// functions
public:

  /// default constructor
  vxPlaneOrtho<T>() {};

  /// constructor providing a normal and distance
  vxPlaneOrtho<T>(const Normal<T> & normal, const T & distance);

  /// constructor providing a normal and a point on the plane
  vxPlaneOrtho<T>(const Normal<T> & normal, const Point3D<T> & point);

  /// constructor providing a three scalars of a normal and distance
  vxPlaneOrtho<T>(const T & x, const T & y, const T & z, const T & distance);

  /// copy constructor
  vxPlaneOrtho<T>(const vxPlaneOrtho<T> & other);

  /// destructor
  virtual ~vxPlaneOrtho<T>() {};

  /// assignment operator
  const vxPlaneOrtho<T> & operator=(const vxPlaneOrtho<T> & other);

  /// equality operator
  bool operator ==( const vxPlaneOrtho<T> & other ) const;

  /// sets the normal of the plane.
  void SetNormal(const Normal<T> & normal);

  /// increment one slice
  float Increment();

  /// decrement one slice
  float Decrement();

private:

  /// aligns a normal to the most perpendicular axis
  Normal<T> GetAxisAlignedNormal(const Normal<T> & normal) const;

}; // vxPlaneOrtho


#include "vxPlaneOrtho.inl"


#endif // ifndef vxPlaneOrtho_h


// $Log: vxPlaneOrtho.h,v $
// Revision 1.3  2006/08/07 01:26:31  gdavidson
// Included vxElement
//
// Revision 1.2  2006/08/05 19:31:03  gdavidson
// Extends vxElement
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.11  2003/09/09 13:30:17  frank
// added equality operator
//
// Revision 1.10  2003/08/20 14:18:58  michael
// added doxygen @param
//
// Revision 1.9  2003/05/16 13:08:50  frank
// code formatting
//
// Revision 1.8  2003/05/15 20:08:58  frank
// Moving away from old point class
//
// Revision 1.7  2003/05/15 11:52:13  michael
// code review
//
// Revision 1.6  2003/05/14 20:31:23  dongqing
// code review
//
// Revision 1.5  2003/05/14 19:07:04  dongqing
// code review
//
// Revision 1.4  2003/04/24 13:46:28  michael
// initial code reviews
//
// Revision 1.3  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.2  2002/11/18 16:35:34  michael
// added source file for include (inline)
//
// Revision 1.6  2002/11/06 22:21:04  michael
// took out the Get/SetFloatDistance() bool which has nothing to do with this class!!!
//
// Revision 1.5  2002/10/03 19:40:30  frank
// Added Increment() and Decrement().
//
// Revision 1.4  2002/05/21 15:25:52  manju
// Hack - Added an indicator for type of plane distance.
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
// Revision 1.10  2002/03/04 23:53:53  jaddonisio
// Comment change.
//
// Revision 1.9  2002/03/04 20:33:22  michael
// adjusted all PlaneOrtho and PlaneOblique instantiations with the new
// templetized types ...
//
// Revision 1.8  2002/02/27 20:20:46  jaddonisio
// Remove some spaces.
//
// Revision 1.7  2002/02/27 19:20:57  jaddonisio
// Removed dead code.
//
// Revision 1.6  2002/02/26 17:38:13  michael
// fixed copy constructor, assignment operator and made methods const that do not alter member data
//
// Revision 1.5  2002/02/26 17:19:22  michael
// now correclty handles the normal vector in the constructors
//
// Revision 1.4  2002/02/25 23:12:14  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 1.3  2002/02/25 22:06:59  michael
// formatting
//
// Revision 1.2  2002/02/25 13:20:46  michael
// made planeOrtho a real derivative of planeOblique
//
// Revision 1.1  2002/02/20 17:37:52  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxPlaneOrtho.h,v 1.3 2006/08/07 01:26:31 gdavidson Exp $
// $Id: vxPlaneOrtho.h,v 1.3 2006/08/07 01:26:31 gdavidson Exp $
