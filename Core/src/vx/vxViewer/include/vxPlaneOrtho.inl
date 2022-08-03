// $Id: vxPlaneOrtho.inl,v 1.3 2007/07/20 19:58:12 cqian Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)


// includes
#include "Vector.h"


/**
 * Constructor, generating plane with given normal and distance.
 * @param normal    Normal of plane.
 * @param fDistance Normal distance to plane.
 */
template <class T>
vxPlaneOrtho<T>::vxPlaneOrtho(const Normal<T> & normal, const T & distance)
: vxPlaneOblique<T>(normal, distance)
//*************************************************************
{
  // make sure it is a ortho normal (parallel to one axis)
  SetNormal(normal);
} // constructor


/**
 * Constructor, generating plane with given normal and a point on the plane.
 * @param normal Normal to plane.
 * @param point  Point on the plane.
 */
template <class T>
vxPlaneOrtho<T>::vxPlaneOrtho(const Normal<T> & normal, 
                                 const Point3D<T> & point)
: vxPlaneOblique<T>(normal, point)
//*************************************************************
{
  // make sure it is a ortho normal (parallel to one axis)
  SetNormal(normal);
} // constructor


/**
 * Constructor, generating plane with given three scalars of a nornal and distance.
 * @param x Scalar on x-m_longAxis.
 * @param y Scalar on y-m_longAxis.
 * @param z Scalar on z-m_longAxis.
 */
template <class T>
vxPlaneOrtho<T>::vxPlaneOrtho(const T & x, const T & y, const T & z, const T & distance)
: vxPlaneOblique<T>(x, y, z, distance)
//*************************************************************
{
  // make sure it is a ortho normal (parallel to one axis)
  SetNormal(Normal<T>(x, y, z));
} // constructor


/**
 * Copy constructor
 * @param other Orthonormal plane.
 */
template <class T>
vxPlaneOrtho<T>::vxPlaneOrtho(const vxPlaneOrtho<T> & other)
//*************************************************************
{
  if (& other != this)
  {
    SetDistance(other.GetDistance());
    vxPlaneOblique<T>::SetNormal(other.GetNormal());
  }
} // Copy constructor


/**
 * Assignment operator =
 * @param other orthonormal plane.
 * @return Orthonormal plane.
 */
template <class T>
const vxPlaneOrtho<T> & vxPlaneOrtho<T>::operator=(const vxPlaneOrtho<T> & other)
//*************************************************************
{
  if (& other != this)
  {
    SetDistance(other.GetDistance());
    vxPlaneOblique<T>::SetNormal(other.GetNormal());
  }

  return * this;
} // operator=()


/**
 * equality operator
 *
 * @param other the other plane
 *
 * @return if equal
 */
template <class T>
bool vxPlaneOrtho<T>::operator ==( const vxPlaneOrtho<T> & other ) const
{

  return GetNormal() == other.GetNormal() && GetDistance() == other.GetDistance();

} // operator ==()


/**
 * Public method to set the normal of the plane.
 * @param normal Reference to a normal.
 */
template <class T>
void vxPlaneOrtho<T>::SetNormal(const Normal<T> & normal)
//*************************************************************
{ 
  vxPlaneOblique<T>::SetNormal(GetAxisAlignedNormal(normal));
} // SetNormal()



/**
 * Private method to get a normal that is axis aligned
 * @param normal Reference to a normal.
 * @return Axis aligned normal.
 */
template <class T>
Normal<T> vxPlaneOrtho<T>::GetAxisAlignedNormal(const Normal<T> & normal) const
//*************************************************************
{
  int4 index(normal.GetMaxAbsComponentCoord());
  Vector<T> vector3D(0,0,0);

  vector3D[index] = (normal[index] > 0) ? static_cast<T>(1) : static_cast<T>(-1);

  return Normal<T>(vector3D[0], vector3D[1], vector3D[2]);
} // GetAxisAlignedNormal()


/**
 * Increment one slice
 * @return new distance
 */
template <class T>
float vxPlaneOrtho<T>::Increment()
//*************************************************************
{
  SetDistance(GetDistance() + 1.0F);
  return GetDistance();
} // Increment()


/**
 * Decrement one slice
 * @return new distance
 */
template <class T>
float vxPlaneOrtho<T>::Decrement()
//*************************************************************
{
  SetDistance(GetDistance() - 1.0F);
  return GetDistance();
} // Decrement()


// Revision History:
// $Log: vxPlaneOrtho.inl,v $
// Revision 1.3  2007/07/20 19:58:12  cqian
// check in for testing
//
// Revision 1.2  2006/01/18 20:37:46  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/09/09 13:30:17  frank
// added equality operator
//
// Revision 1.6  2003/08/20 14:18:58  michael
// added doxygen @param
//
// Revision 1.5  2003/05/15 20:09:15  frank
// Moving away from old point class
//
// Revision 1.4  2003/05/15 11:52:13  michael
// code review
//
// Revision 1.3  2003/05/15 11:03:27  michael
// added inl files for templatized class
//
// Revision 1.4  2003/05/14 20:31:23  dongqing
// code review
//
// Revision 1.3  2003/05/14 19:07:04  dongqing
// code review
//
// Revision 1.2  2003/01/22 21:08:36  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.1  2002/11/18 16:12:37  michael
// added renamed versions of the plane objects using vx prefix
//
// Revision 1.1  2002/11/18 15:53:08  michael
// added renamed versions of the plane objects using vx prefix
//
// Revision 1.5  2002/11/06 22:21:04  michael
// took out the Get/SetFloatDistance() bool which has nothing to do with this class!!!
//
// Revision 1.4  2002/10/03 19:40:30  frank
// Added Increment() and Decrement().
//
// Revision 1.3  2002/05/21 15:25:52  manju
// Hack - Added an indicator for type of plane distance.
//
// Revision 1.2  2002/03/06 15:38:43  jaddonisio
// Fixed UnitTest().
//
// Revision 1.1  2002/03/06 00:20:49  jaddonisio
// Initial version.
//
// Revision 1.2  2002/03/04 20:33:22  michael
// adjusted all vxPlaneOrtho and PlaneOblique instantiations with the new
// templetized types ...
//
// Revision 1.1  2002/03/04 20:27:39  michael
// added inl files since plane classes are now templetized
//
// Revision 1.8  2002/02/27 19:27:03  jaddonisio
// Call base constructor with parameters.
//
// Revision 1.7  2002/02/27 15:02:34  michael
// added explicit parameter call of base class constructor
//
// Revision 1.6  2002/02/26 19:46:43  michael
// normalize vector in constructor only once
//
// Revision 1.5  2002/02/26 17:38:13  michael
// fixed copy constructor, assignment operator and made methods const that do not alter member data
//
// Revision 1.4  2002/02/26 17:19:22  michael
// now correclty handles the normal vector in the constructors
//
// Revision 1.3  2002/02/25 23:12:14  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 1.2  2002/02/25 13:20:46  michael
// made vxPlaneOrtho a real derivative of planeOblique
//
// Revision 1.1  2002/02/20 17:37:52  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxPlaneOrtho.inl,v 1.3 2007/07/20 19:58:12 cqian Exp $
// $Id: vxPlaneOrtho.inl,v 1.3 2007/07/20 19:58:12 cqian Exp $
