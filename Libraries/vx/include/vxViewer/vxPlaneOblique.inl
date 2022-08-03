// $Id: vxPlaneOblique.inl,v 1.3 2006/01/18 20:37:46 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)

/*
  This class provides an oblique plane description
*/


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Default constructor, assumes a plane in the origin oriented along the x axis
 */
template <class T>
vxPlaneOblique<T>::vxPlaneOblique()
//*************************************************************
{
  SetNormal(Normal<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)));
  SetDistance(static_cast<T>(0));
} // default constructor


/**
 * Constructor generating plane with given normal and distance
 * @param Normal<T> & normal
 * @param T & fDistance
 */
template <class T>
vxPlaneOblique<T>::vxPlaneOblique(const Normal<T> & normal, 
                                     const T & distance)
//*************************************************************
{
  SetNormal(normal);
  SetDistance(distance);
} // Constructor


/**
 * Constructor, generating plane with given normal and distance
 * @param normal the plane normal
 * @param point  point on plane
 */
template <class T>
vxPlaneOblique<T>::vxPlaneOblique(const Normal<T> & normal, 
                                     const Point3D<T> & point)
//*************************************************************
{
  SetNormal(normal);
  SetDistance(static_cast<T>(0));
  Move(GetDistance(point));
} // Constructor


/**
 * Constructor, generating plane with given normal (three scalars) and distance
 * @param T rdX
 * @param T rdY
 * @param T rdZ
 * @param T distance
 */
template <class T>
vxPlaneOblique<T>::vxPlaneOblique(const T & rdX, 
                                     const T & rdY, 
                                     const T & rdZ, 
                                     const T & distance)
//*************************************************************
{
  SetNormal(Normal<T>(rdX, rdY, rdZ));
  SetDistance(distance);
} // Constructor


/**
 * Copy constructor
 * @param plane
 */
template <class T>
vxPlaneOblique<T>::vxPlaneOblique(const vxPlaneOblique<T> & plane)
//*************************************************************
{
  *this = plane;
} // Copy constructor


/**
 * assignment operator
 * @param plane
 */
template <class T>
const vxPlaneOblique<T> & vxPlaneOblique<T>::operator =(const vxPlaneOblique<T> & plane)
//*************************************************************
{
  if (this != & plane)
  {  
    SetNormal(plane.GetNormal());
    SetDistance(plane.GetDistance());
  }  
  return *this;
} // operator =


/**
 * Equality operator
 * @param other the other plane
 * @return true if ==
 */
template <class T>
bool vxPlaneOblique<T>::operator ==(const vxPlaneOblique<T> & other) const
//*************************************************************
{
  return m_normal == other.m_normal && m_distance == other.m_distance;
} // operator ==()


/**
 * Inequality operator
 * @param other the other plane
 * @return true if !=
 */
template <class T>
bool vxPlaneOblique<T>::operator !=(const vxPlaneOblique<T> & other) const
//*************************************************************
{
  return ! (* this == other);
} // operator !=()


/**
 * Returns the distance of the point to the plane
 * @param point (x,y,z) of point
 * @return Distance of point to oblique plane
 */
template <class T>
T vxPlaneOblique<T>::GetDistance(const Point3D<T> & point) const
//*************************************************************
{
  return m_normal.Dot(Vector3D<T>(point)) + m_distance;
} // GetDistance()


/**
 * Public method to get the absolute distance of the plane from the origin
 * @return Absolute distance
 */
template <class T>
T vxPlaneOblique<T>::GetAbsDistance() const
//*************************************************************
{
  return Abs(m_distance);
} // GetAbsDistance()


/**
 * Get the absolute distance of the plane from the origin
 * @param point XYZ of a point
 * @return Absolute distance
 */
template <class T>
T vxPlaneOblique<T>::GetAbsDistance(const Point3D<T> & point) const
//*************************************************************
{

  return Abs( GetDistance( point ) );

} // GetAbsDistance()


/**
 * Moves the plane by the given offset
 * @param distance offset distance
 */
template <class T>
void vxPlaneOblique<T>::Move(const T & distance)
//*************************************************************
{
  m_distance -= distance;
} // Move()


/**
 * Returns the intersection of the given ray and the plane
 * @param rayPoint     point on ray
 * @param rayNormal    normal of ray
 * @param intersection intersection
 * @return true if intersection exists
 */
template <class T>
bool vxPlaneOblique<T>::GetIntersection(const Point3D<T> & rayPoint, 
                                        const Normal<T> & rayNormal,
                                        Point3D<T> & intersection) const
//*************************************************************
{
  float4 fDistanceToIntersection(0.0F);
  if (! GetDistanceToIntersection(rayPoint, rayNormal, fDistanceToIntersection)) return false;

  for (int i(0); i<3; ++i)
    intersection[i] = rayPoint[i] + fDistanceToIntersection * rayNormal[i];

  return true;

} // GetIntersection()


/**
 * Returns the intersection of the given ray and the plane
 * @param rayPoint  point on ray
 * @param rayNormal normal of ray
 * @param fDistanceToIntersection distance
 * @return true if intersection exists
 */
template <class T>
bool vxPlaneOblique<T>::GetDistanceToIntersection(const Point3D<T> & rayPoint, 
                                                  const Normal<T> & rayNormal,
                                                  float4 & fDistanceToIntersection) const
//*************************************************************
{
  float fResult1, fResult2;

  fResult1  = GetNormal().X() * rayPoint.X();
  fResult1 += GetNormal().Y() * rayPoint.Y();
  fResult1 += GetNormal().Z() * rayPoint.Z();
  fResult1 += GetDistance();

  fResult2 =  GetNormal().X() * rayNormal.X();
  fResult2 += GetNormal().Y() * rayNormal.Y();
  fResult2 += GetNormal().Z() * rayNormal.Z();

  if (fResult2 == 0.0) 
  {
    return false;
  }

  fDistanceToIntersection = -fResult1 / fResult2;

  return true;
} // GetDistanceToIntersection()


#undef FILE_REVISION


// Revision History:
// $Log: vxPlaneOblique.inl,v $
// Revision 1.3  2006/01/18 20:37:46  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2  2005/03/08 19:09:24  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.1  2005/02/16 20:27:18  michael
// copy constructor now uses assignment operator
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2004/01/16 12:44:03  frank
// fixed abs distance function to return distance of point rather than plane
//
// Revision 1.8  2003/09/09 13:29:57  frank
// width is unused
//
// Revision 1.7  2003/08/20 14:18:58  michael
// added doxygen @param
//
// Revision 1.6  2003/05/20 17:52:38  frank
// Added equality and inequality operators
//
// Revision 1.5  2003/05/15 20:08:44  frank
// Moving away from old point class
//
// Revision 1.4  2003/05/15 11:52:13  michael
// code review
//
// Revision 1.3  2003/05/15 11:03:27  michael
// added inl files for templatized class
//
// Revision 1.9  2003/05/14 20:31:22  dongqing
// code review
//
// Revision 1.8  2003/05/14 19:07:04  dongqing
// code review
//
// Revision 1.7  2003/01/28 23:22:53  frank
// Coding standards: const reference parameters!
//
// Revision 1.6  2003/01/24 23:31:39  ingmar
// code walkthrough changes
//
// Revision 1.5  2003/01/20 19:12:04  michael
// cosmetics
//
// Revision 1.4  2002/11/26 18:09:53  ingmar
// added GetDistanceToIntersection()
//
// Revision 1.3  2002/11/21 22:51:55  michael
// cleanup
//
// Revision 1.2  2002/11/19 21:26:43  michael
// added intersection method to plane oblique
//
// Revision 1.1  2002/11/18 16:12:37  michael
// added renamed versions of the plane objects using vx prefix
//
// Revision 1.1  2002/11/18 15:53:08  michael
// added renamed versions of the plane objects using vx prefix
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
// Revision 1.2  2002/03/04 20:33:22  michael
// adjusted all PlaneOrtho and vxPlaneOblique instantiations with the new
// templetized types ...
//
// Revision 1.1  2002/03/04 20:27:39  michael
// added inl files since plane classes are now templetized
//
// Revision 1.10  2002/02/27 19:30:04  jaddonisio
// Changed float variable name from 'dist' to 'fDistance'.
//
// Revision 1.9  2002/02/26 20:14:37  jaddonisio
// New Distance function use Dot method of Normal class.
//
// Revision 1.8  2002/02/26 17:38:13  michael
// fixed copy constructor, assignment operator and made methods const that do not alter member data
//
// Revision 1.7  2002/02/25 23:12:14  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 1.6  2002/02/25 13:20:46  michael
// made planeOrtho a real derivative of vxPlaneOblique
//
// Revision 1.5  2002/02/15 15:54:44  michael
// clean-up, all member data is private, dragging slice is not anywmore in Manipulator
// base class, and PlaneOrtho manipulator got simplified.
//
// Revision 1.4  2002/02/14 18:13:00  uday
// plane stuff replaces orientation and slicenumber
//
// Revision 1.3  2001/12/14 00:04:18  frank
// Added comment.
//
// Revision 1.2  2001/12/11 22:14:31  michael
// new 3dTexMapRenderer compiles but still enough to do ...
//
// Revision 1.1  2001/12/11 19:35:58  michael
// added class vxPlaneOblique
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxPlaneOblique.inl,v 1.3 2006/01/18 20:37:46 geconomos Exp $
// $Id: vxPlaneOblique.inl,v 1.3 2006/01/18 20:37:46 geconomos Exp $
