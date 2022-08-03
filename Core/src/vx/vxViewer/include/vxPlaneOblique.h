// $Id: vxPlaneOblique.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)

/**
 *  This class provides an oblique plane description
 */

#ifndef vxPlaneOblique_h
#define vxPlaneOblique_h


// includes
#include "vxPlane.h"
#include "Normal.h"


// forward declarations
template <class T> class Normal;
template <class T> class Point;


// class definition
template<class T>
class vxPlaneOblique : public vxPlane
{ 
// functions
public:

  /// default constructor for plane in origin and x axis as normal
  vxPlaneOblique<T>();
  
  /// constructor providing a normal (three scalars) and distance
  vxPlaneOblique<T>(const Normal<T> & normal, const T & distance);
  
  /// constructor providing a normal (three scalars) and distance
  vxPlaneOblique<T>(const T & rdX, const T & rdY, const T & rdZ, const T & distance);
  
  /// constructor providing a normal and a point
  vxPlaneOblique<T>(const Normal<T> & normal, const Point3D<T> & point);
  
  /// copy constructor
  vxPlaneOblique<T>(const vxPlaneOblique<T> & other);
  
  /// default Destrutor
  virtual ~vxPlaneOblique<T>() {};
  
  /// assignment operator
  const vxPlaneOblique<T> & operator=(const vxPlaneOblique<T> & other);

  /// equality operator
  bool operator ==(const vxPlaneOblique<T> & other) const;

  /// inequality operator
  bool operator !=(const vxPlaneOblique<T> & other) const;

  /// return the normal of the plane
  Normal<T> GetNormal() const { return m_normal; };
  
  /// sets the normal of the plane
  virtual void SetNormal(const Normal<T> & normal) { m_normal = normal; };
  
  /// return the distance of the plane to the origin
  T GetDistance() const { return m_distance; };
  
  /// sets the distance of the plane to the origin
  void SetDistance(const T & distance) { m_distance = distance; };
  
  /// return the (orthogonal) distance of the given point to the plane
  T GetDistance(const Point3D<T> & point) const;

  /// return the absolute distance of the plane to the origin.
  T GetAbsDistance() const;

  /// return the absolute distance of a point to the plane.
  T GetAbsDistance(const Point3D<T> & point) const;

  /// intersects the ray with the plane
  bool GetIntersection(const Point3D<T> & rayPoint, const Normal<T> & rayNormal, Point3D<T> & intersection) const;

  /// intersects the ray with the plane
  bool GetDistanceToIntersection(const Point3D<T> & rayPoint, const Normal<T> & rayNormal, float4 & fDistanceToIntersection) const;

  /// move the plane along the normal direction by the given distance
  void Move(const T & distance);

// data
private:

  /// normal of the plane
  Normal<T> m_normal;
  
  /// distance of the plane to the origin
  T m_distance;

}; // vxPlaneOblique


#include "vxPlaneOblique.inl"


#endif // vxPlaneOblique_h


// $Log: vxPlaneOblique.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.19  2004/01/16 12:44:02  frank
// fixed abs distance function to return distance of point rather than plane
//
// Revision 1.18  2003/09/09 13:29:57  frank
// width is unused
//
// Revision 1.17  2003/08/26 19:47:34  michael
// fixed typo
//
// Revision 1.16  2003/08/20 14:18:58  michael
// added doxygen @param
//
// Revision 1.15  2003/06/04 18:33:23  michael
// tookout namespace declaration, should not be in header file!!!
//
// Revision 1.14  2003/05/20 17:52:38  frank
// Added equality and inequality operators
//
// Revision 1.13  2003/05/16 11:48:48  michael
// code review
//
// Revision 1.12  2003/05/15 20:08:14  frank
// Moving away from old point class
//
// Revision 1.11  2003/05/15 11:52:13  michael
// code review
//
// Revision 1.10  2003/05/14 20:31:23  dongqing
// code review
//
// Revision 1.9  2003/05/14 19:07:04  dongqing
// code review
//
// Revision 1.8  2003/04/24 13:46:28  michael
// initial code reviews
//
// Revision 1.7  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.6  2003/01/28 23:22:53  frank
// Coding standards: const reference parameters!
//
// Revision 1.5  2002/11/26 18:09:53  ingmar
// added GetDistanceToIntersection()
//
// Revision 1.4  2002/11/21 22:51:55  michael
// cleanup
//
// Revision 1.3  2002/11/19 21:26:43  michael
// added intersection method to plane oblique
//
// Revision 1.2  2002/11/18 16:35:34  michael
// added source file for include (inline)
//
// Revision 1.4  2002/10/17 15:20:18  frank
// Added width.
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
// Revision 1.10  2002/03/04 23:57:48  jaddonisio
// Comment change.
//
// Revision 1.9  2002/03/04 20:33:23  michael
// adjusted all PlaneOrtho and PlaneOblique instantiations with the new
// templetized types ...
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxPlaneOblique.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxPlaneOblique.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
