// $Id: Triangle.inl,v 1.2 2006/02/17 15:09:13 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


/**
 * Constructor
 * @param p1
 * @param p2
 * @param p3
 */
template <typename T>
Triangle<T>::Triangle( const T & p1, const T & p2, const T & p3 )
: m_v1( p1 ), m_v2( p2 ), m_v3( p3 )
{
} // Triangle()


/**
 * Copy constructor
 *
 * @param other the other triangle
 */
template <typename T>
Triangle<T>::Triangle( const Triangle<T> & other )
  : m_v1( other.m_v1 ), m_v2( other.m_v2 ), m_v3( other.m_v3 )
{
} // Triangle( other )


/**
 * Assignment operator
 *
 * @param other the other triangle
 *
 * @return reference to itself
 */
template <typename T>
Triangle<T> & Triangle<T>::operator =( const Triangle<T> & other )
{ 
  if ( & other != this )
  {
    m_v1 = other.m_v1; 
    m_v2 = other.m_v2; 
    m_v3 = other.m_v3;
  }    
  return * this;
} // operator =()


/**
 * Access the vertices
 *
 * @param iIndex the desired index
 *
 * @return the vertex of interest
 */
template <typename T>
const T & Triangle<T>::GetVertex( const uint4 iIndex ) const
{ 
  switch ( iIndex )
  {
  case 1:
    return m_v1;
  case 2:
    return m_v2;
  case 3:
    return m_v3;
  default:
    return m_v1;
  }
} // GetVertex()


/**
  * Compute the triangle area.
  * See http://mathworld.wolfram.com/TriangleArea.html for details
  *
  * @return the area
  */
template <typename T>
float8 Triangle<T>::GetArea() const
{
  // we can take half the magnitude of the cross product of any two edge vectors as the area
  const Vector3Df vA( m_v2 - m_v1 );
  const Vector3Df vB( m_v3 - m_v1 );
  const Vector3Df vCross( vA.Cross( vB ) );
  return vCross.GetLength() / 2.0F;
} // GetArea()


// $Log: Triangle.inl,v $
// Revision 1.2  2006/02/17 15:09:13  frank
// improved the accuracy of area estimates
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/05/21 13:11:37  dongqing
// code review
//
// Revision 1.1  2003/05/20 20:23:58  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Triangle.inl,v 1.2 2006/02/17 15:09:13 frank Exp $
// $Id: Triangle.inl,v 1.2 2006/02/17 15:09:13 frank Exp $
