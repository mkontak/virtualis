// $Id: BoundingRect.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Wei Li (liwei@viatronix.com)


/**
 * default constructor
 */
template <class T>
BoundingRect<T>::BoundingRect()
{
  m_minX = m_maxX = m_minY = m_maxY = 0;
} // BoundingRect()


/** 
 * Constructor
 *
 * @param minX the minimum x coordinate
 * @param minY the minimum y coordinate
 * @param maxX the maximum x coordinate
 * @param maxY the maximum y coordinate
 */
template <class T>
BoundingRect<T>::BoundingRect( const T & minX, const T & minY, const T & maxX, const T & maxY )
: m_minX(minX),
m_maxX(maxX),
m_minY(minY),
m_maxY(maxY)
{
} // BoundingRect()


/**
 * Constructor
 * @param dimX
 * @param dimY
 */
template <class T>
BoundingRect<T>::BoundingRect( const T & dimX, const T & dimY )
{
  m_minX = m_minY = 0;
  m_maxX = dimX;
  m_maxY = dimY;
} // BoundingRect()


/**
 * Scale the rectangle
 *
 * @param fScale the scaling factor
 */
template <class T>
void BoundingRect<T>::Scale( const T fScale )
{
  m_minX *= fScale;
  m_maxX *= fScale;
  m_minY *= fScale;
  m_maxY *= fScale;
} // Scale()


/**
 * Ensure that the minimum is minimum and the maximum is maximum
 */
template <class T>
void BoundingRect<T>::Normalize()
{
  if ( m_minX > m_maxX ) Swap(m_minX, m_maxX);
  if ( m_minY > m_maxY ) Swap(m_minY, m_maxY);
} // Normalize()


/**
 * Expand the bounding rectangle to include the given point.
 *
 * @param pointToInclude the point to include
 */
template <class T>
inline void BoundingRect<T>::Union( const Point3D<T> & pointToInclude )
{
  m_minX = Min( m_minX, pointToInclude.m_x );
  m_maxX = Max( m_maxX, pointToInclude.m_x );
  m_minY = Min( m_minY, pointToInclude.m_y );
  m_maxY = Max( m_maxY, pointToInclude.m_y );
} // Union()


/**
 * access the four corners by index (as in class box)
 *
 * @param i - the i'th corner
 * @return 3D coordinate of i'th corner
 */
template <class T>
inline Point3D<T> BoundingRect<T>::GetCorner( const uint2 & i )
{
  Point3D<T> corner( T(0), T(0), T(0) );

  switch ( i )
  {
    case 0:
      corner.m_x = GetMinX();
      corner.m_y = GetMinY();
    break;
    case 1:
      corner.m_x = GetMaxX();
      corner.m_y = GetMinY();
    break;
    case 2:
      corner.m_x = GetMinX();
      corner.m_y = GetMaxY();
    break;
    case 3:
      corner.m_x = GetMaxX();
      corner.m_y = GetMaxY();
    break;
    default:
      corner.m_x = GetMaxX();
      corner.m_y = GetMaxY();
    break;
  }
  return corner;
} // GetCorner()


/**
 * Convert to a string representation
 *
 * @return a string representation
 */
template <class T>
std::string BoundingRect<T>::ToString() const
{
  std::strstream ss;
  ss << "BoundingRect( "<<GetMinX()<<"<=x<="<<GetMaxX()<<", "<<GetMinY()<<"<=y<="<<GetMaxY()<<" )"<<'\0' ;
  return( std::string(ss.str()) );
} // ToString()


/**
 * set parameters
 *
 * @param minX the minimum x coordinate
 * @param minY the minimum y coordinate
 * @param maxX the maximum x coordinate
 * @param maxY the maximum y coordinate
 */
template <class T>
inline void BoundingRect<T>::Set( const T & minX, const T & minY, const T & maxX, const T & maxY )
{
  m_minX = minX;
  m_minY = minY;
  m_maxX = maxX;
  m_maxY = maxY;
} // Set()


/** 
 * Compute the intersection of itself and another rectangle (does not modify itself)
 *
 * @param other the other BoundingRect
 *
 * @return a new bounding rectangle
 */
template <class T>
BoundingRect<T> BoundingRect<T>::GetIntersection( const BoundingRect & other ) const
{
	return BoundingRect<T>(	max(m_minX, other.GetMinX()), max(m_minY, other.GetMinY()),
													min(m_maxX, other.GetMaxX()), min(m_maxY, other.GetMaxY()) );
} // GetIntersection()


/** 
 * Compute the union of itself and another rectangle (does not modify itself)
 *
 * @param other the other BoundingRect
 *
 * @return a new bounding rectangle
 */
template <class T>
BoundingRect<T> BoundingRect<T>::GetUnion( const BoundingRect & other ) const
{
	return BoundingRect<T>(	min(m_minX, other.GetMinX()), min(m_minY, other.GetMinY()),
													max(m_maxX, other.GetMaxX()), max(m_maxY, other.GetMaxY()) );
} // GetUnion()


/**
 * Whether the area of the bounding rect is 0
 *
 * @return true if the area of the bounding rect is 0
 */
template <class T>
const bool BoundingRect<T>::IsEmpty() const
{
  return m_minX >= m_maxX || m_minY >= m_maxY;
} // IsEmpty()


/**
 * Whether the position is inside the bounding rect.
 * It is inclusive of both start and end bounds.
 *
 * @param x  the x position
 * @param y  the x position
 *
 * @return true if the position is inside the bounding rect
 */
template <class T>
const bool BoundingRect<T>::IsInside( T x, T y ) const
{

  return x >= m_minX && x <= m_maxX &&
         y >= m_minY && y <= m_maxY;

} // IsInside()


/**
 * equality operator
 *
 * @param other  the other rectanlge
 *
 * @return true if ==
 */
template <class T>
bool BoundingRect<T>::operator ==( const BoundingRect<T> & other ) const
{
  return
    m_minX == other.m_minX &&
    m_maxX == other.m_maxX &&
    m_minY == other.m_minY &&
    m_maxY == other.m_maxY;
} // operator ==


/**
 * inequality operator
 *
 * @param other  the other rectanlge
 *
 * @return true if !=
 */
template <class T>
bool BoundingRect<T>::operator !=( const BoundingRect<T> & other ) const
{
  return
    m_minX != other.m_minX ||
    m_maxX != other.m_maxX ||
    m_minY != other.m_minY ||
    m_maxY != other.m_maxY;
} // operator !=


// $Log: BoundingRect.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.8  2003/12/16 20:23:03  frank
// Issue #3593: Restricted the segmentation operations to the visible volume including crop box and thin slab when using the shift key
//
// Revision 3.7  2003/05/20 16:13:50  dongqing
// code review
//
// Revision 3.6  2003/05/15 20:05:47  frank
// code review
//
// Revision 3.5  2002/08/21 18:29:38  michael
// added method to access the four corners by index (as in class box)
//
// Revision 3.4  2002/04/26 15:57:34  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.3  2002/02/25 21:23:12  ingmar
// added void to function union
//
// Revision 3.2.2.1  2002/04/08 17:14:28  frank
// Added void return type on Union().
//
// Revision 3.2  2001/11/06 01:11:50  frank
// Added Union()
//
// Revision 3.1  2001/11/01 16:44:03  frank
// Added extra constructors and output functions.
//
// Revision 3.0  2001/10/14 23:02:17  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:10   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:04   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:39  soeren
// renamed math directory
//
// Revision 1.2  2001/08/23 00:19:06  liwei
// Added Union().
//
// Revision 1.1  2001/08/09 21:11:58  liwei
// Box2D renamed to BoundingRect
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/BoundingRect.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: BoundingRect.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
