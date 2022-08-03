// $Id: Box.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Wei Li (liwei@viatronix.com)

/* 
 * default constructor
 */
template<class T>
Box<T>::Box()
: m_minZ(0),
m_maxZ(0)
{
} // Box()


/* 
 * constructor
 *
 * @pararm minX  the minimum x value
 * @pararm minY  the minimum y value
 * @pararm minZ  the minimum z value
 * @pararm maxX  the maximum x value
 * @pararm maxY  the maximum y value
 * @pararm maxZ  the maximum z value
 */
template<class T>
Box<T>::Box(const T minX, const T minY, const T minZ, const T maxX, const T maxY, const T maxZ)
: BoundingRect<T>( minX, minY, maxX, maxY ),
m_minZ( minZ ),
m_maxZ( maxZ )
{
} // Box()


/**
 * Min/max point constructor
 *
 * @param minPoint  the minimum point
 * @param maxPoint  the maximum point
 */
template <class T>
Box<T>::Box( const Point3D<T> minPoint, const Point3D<T> maxPoint )
: BoundingRect<T>( minPoint.m_x, minPoint.m_y, maxPoint.m_x, maxPoint.m_y ),
m_minZ( minPoint.m_z ),
m_maxZ( maxPoint.m_z )
{
} // Box()


/** 
 * Constructor
 *
 * @param boundingRect  the bounding rect
 * @param minZ          the minimum z value
 * @param maxZ          the maximum z value
 * @param eOrientation  the orientation
 */
template<class T>
Box<T>::Box( const BoundingRect<T>& boundingRect, const T minZ, const T maxZ, const nsORIENTATION::ORIENTATION eOrientation )
{
  Set( boundingRect, eOrientation );

  switch ( eOrientation )
  {
	case X_ORIENTATION:
		SetMinX(minZ);
		SetMaxX(maxZ);
		break;
	case Y_ORIENTATION:
		SetMinY(minZ);
		SetMaxY(maxZ);
		break;
	case Z_ORIENTATION:
		SetMinZ(minZ);
		SetMaxZ(maxZ);
		break;
	} // switch eOrientation
} // Box()


/**
 * set parameters
 *
 * @pararm minX  the minimum x value
 * @pararm minY  the minimum y value
 * @pararm minZ  the minimum z value
 * @pararm maxX  the maximum x value
 * @pararm maxY  the maximum y value
 * @pararm maxZ  the maximum z value
 */
template<class T>
void Box<T>::Set( const T minX, const T minY, const T minZ, const T maxX, const T maxY, const T maxZ )
{
  BoundingRect<T>::Set( minX, minY, maxX, maxY );
  m_minZ = minZ;
  m_maxZ = maxZ;
} // Set()


/* 
 * set the bounding rect
 *
 * @param boundingRect  the bounding rect
 * @param eOrientation  the orientation
 */
template<class T>
void Box<T>::Set( const BoundingRect<T> & boundingRect, const nsORIENTATION::ORIENTATION eOrientation )
{
	switch ( eOrientation )
  {
	case X_ORIENTATION:
		SetMinY(boundingRect.GetMinX());
		SetMaxY(boundingRect.GetMaxX());
		SetMinZ(boundingRect.GetMinY());
		SetMaxZ(boundingRect.GetMaxY());
		break;
	case Y_ORIENTATION:
		SetMinX(boundingRect.GetMinY());
		SetMaxX(boundingRect.GetMaxY());
		SetMinZ(boundingRect.GetMinX());
		SetMaxZ(boundingRect.GetMaxX());
		break;
	case Z_ORIENTATION:
		SetMinX(boundingRect.GetMinX());
		SetMaxX(boundingRect.GetMaxX());
		SetMinY(boundingRect.GetMinY());
		SetMaxY(boundingRect.GetMaxY());
		break;
	} // switch eOrientation
} // Set()


/* 
 * change the dimension
 *
 * @param dim  the new dimensions
 */
template<class T>
void Box<T>::SetDim( const Triple<T> & dim )
{
	SetMaxX( GetMinX() + dim.m_x );
	SetMaxY( GetMinY() + dim.m_y );
	SetMaxZ( GetMinZ() + dim.m_z );
} // SetDim()


/* 
 * set new min corner
 *
 * @param point the new min corner
 */
template<class T>
void Box<T>::SetMinPoint( const Point3D<T> point )
{
  SetMinX( point.m_x );
  SetMinY( point.m_y );
  SetMinZ( point.m_z );
} // SetMinPoint()


/* 
 * set new max corner
 *
 * @param point the new max corner
 */
template<class T>
void Box<T>::SetMaxPoint( const Point3D<T> point )
{
  SetMaxX( point.m_x );
  SetMaxY( point.m_y );
  SetMaxZ( point.m_z );
} // SetMaxPoint()


/**
 * Expand the bounding box to include the given point.
 *
 * @param pointToInclude  the other point to include
 */
template <class T>
inline void Box<T>::Union(const Point3D<T> & pointToInclude)
{
  m_minX = Min(m_minX, pointToInclude.m_x); m_maxX = Max(m_maxX, pointToInclude.m_x);
  m_minY = Min(m_minY, pointToInclude.m_y); m_maxY = Max(m_maxY, pointToInclude.m_y);
  m_minZ = Min(m_minZ, pointToInclude.m_z); m_maxZ = Max(m_maxZ, pointToInclude.m_z);
} // Union()


/**
 * Expand the bounding box to include the given point.
 *
 * @param pointToInclude  the other point to include
 */
template <class T>
inline void Box<T>::Union( const Vector3D<T> & pointToInclude )
{
  m_minX = Min(m_minX, pointToInclude.X()); m_maxX = Max(m_maxX, pointToInclude.X());
  m_minY = Min(m_minY, pointToInclude.Y()); m_maxY = Max(m_maxY, pointToInclude.Y());
  m_minZ = Min(m_minZ, pointToInclude.Z()); m_maxZ = Max(m_maxZ, pointToInclude.Z());
} // Union()


/**
 * Returns true if the two boxes intersect each other
 * Please note: Touching Boxes are considered to be overlapping
 */
template <class T>
inline bool Box<T>::IfOverlap( const Box<T> & other ) const
{
  // boxes outside each other
  if ( ( m_minX > other.GetMaxX() || m_maxX < other.GetMinX() ) ||
       ( m_minY > other.GetMaxY() || m_maxY < other.GetMinY() ) ||
       ( m_minZ > other.GetMaxZ() || m_maxZ < other.GetMinZ() ) )
  {
    return false;
  }
  else
  {
    return true;
  }
} // IfOverlap()


/**
 * access the eight corners by index
 *
 * @param i - the i'th corner.
 * @return 3D coordinate of i'th corner.
 */
template <class T>
inline Point3D<T> Box<T>::GetCorner( const uint2 & i ) const
{
  Point3D<T> corner( T(0), T(0), T(0) );

  switch ( i )
  {
    case 0:
      corner.m_x = GetMinX();
      corner.m_y = GetMinY();
      corner.m_z = GetMinZ();
    break;
    case 1:
      corner.m_x = GetMaxX();
      corner.m_y = GetMinY();
      corner.m_z = GetMinZ();
    break;
    case 2:
      corner.m_x = GetMinX();
      corner.m_y = GetMaxY();
      corner.m_z = GetMinZ();
    break;
    case 3:
      corner.m_x = GetMaxX();
      corner.m_y = GetMaxY();
      corner.m_z = GetMinZ();
    break;
    case 4:
      corner.m_x = GetMinX();
      corner.m_y = GetMinY();
      corner.m_z = GetMaxZ();
    break;
    case 5:
      corner.m_x = GetMaxX();
      corner.m_y = GetMinY();
      corner.m_z = GetMaxZ();
    break;
    case 6:
      corner.m_x = GetMinX();
      corner.m_y = GetMaxY();
      corner.m_z = GetMaxZ();
    break;
    case 7:
    default:
      corner.m_x = GetMaxX();
      corner.m_y = GetMaxY();
      corner.m_z = GetMaxZ();
    break;
  } // switch i

  return corner;

} // GetCorner()


/**
 * Get the length of the sizes
 *
 * @return Length of sides
 */
template <class T>
inline Triple<T> Box<T>::GetLength() const
{
  return Triple<T>( Abs( m_maxX - m_minX ),
                    Abs( m_maxY - m_minY ),
                    Abs( m_maxZ - m_minZ ) );
} // GetLength()


/**
 * assignment operator
 *
 * @param other  the other box
 */
template <class T>
inline void Box<T>::operator =( const Box<T>& other )
{
  m_minX = other.GetMinPoint().m_x;
  m_minY = other.GetMinPoint().m_y;
  m_minZ = other.GetMinPoint().m_z;
  m_maxX = other.GetMaxPoint().m_x;
  m_maxY = other.GetMaxPoint().m_y;
  m_maxZ = other.GetMaxPoint().m_z;
} // operator =


/**
 * operator ==
 *
 * @param  the box in question
 *
 * @return true if ==
 */
template <class T>
inline bool Box<T>::operator == (const Box<T>& other) const
{
  return m_minX == other.m_minX &&
         m_minY == other.m_minY &&
         m_minZ == other.m_minZ &&
         m_maxX == other.m_maxX &&
         m_maxY == other.m_maxY &&
         m_maxZ == other.m_maxZ;
} // operator ==


/**
 * operator !=
 *
 * @param  the box in question
 *
 * @return true if !=
 */
template <class T>
inline bool Box<T>::operator != ( const Box<T> & other ) const
{
  return m_minX != other.m_minX ||
         m_minY != other.m_minY ||
         m_minZ != other.m_minZ ||
         m_maxX != other.m_maxX ||
         m_maxY != other.m_maxY ||
         m_maxZ != other.m_maxZ;
} // operator !=


/**
 *  return the radius of the enclosing minimum bounding sphere
 *
 *  @return the outer radius
 */
template <class T>
inline T Box<T>::GetOuterRadius() const
{
  return ( GetMinPoint() - GetMaxPoint() ).GetLength() / 2.0F;
} // GetOuterRadius()


/**
 *  return the radius of the enclosed maximum sphere
 *
 *  @return the inner radius
 */
template <class T>
inline T Box<T>::GetInnerRadius() const
{
  return GetDim().GetMinComponent() / 2;
} // GetInnerRadius()


/**
 * get the intersection between two boxes
 *
 * @param other the other box
 *
 * @return the intersection of the two boxes
 */
template <class T>
Box<T> Box<T>::GetIntersection( const Box<T> & other ) const
{
  return Box<T>
  (
    Max( m_minX, other.m_minX ),
    Max( m_minY, other.m_minY ),
    Max( m_minZ, other.m_minZ ),
    Min( m_maxX, other.m_maxX ),
    Min( m_maxY, other.m_maxY ),
    Min( m_maxZ, other.m_maxZ )
  );
} // GetIntersection()


/**
 * get the union between two boxes
 *
 * @param other the other box
 *
 * @return the union of the two boxes
 */
template <class T>
Box<T> Box<T>::GetUnion( const Box<T> & other ) const
{
  return Box<T>
  (
    Min( m_minX, other.m_minX ),
    Min( m_minY, other.m_minY ),
    Min( m_minZ, other.m_minZ ),
    Max( m_maxX, other.m_maxX ),
    Max( m_maxY, other.m_maxY ),
    Max( m_maxZ, other.m_maxZ )
  );
} // GetIntersection()


/**
 *  return the radius of the enclosed maximum sphere
 *
 *  @return the inner radius
 */
template <class T>
std::string Box<T>::ToString() const
{
  return std::string( "Box( " + ToAscii( m_minX ) + "<=x<=" + ToAscii( m_maxX )+
                         ", " + ToAscii( m_minY ) + "<=y<=" + ToAscii( m_maxY )+
                         ", " + ToAscii( m_minZ ) + "<=z<=" + ToAscii( m_maxZ ) + " )" );
} // ToString()


/**
 * Get if the position is inside the box.
 * It is inclusive of both start and end bounds.
 *
 * @param position  the position in question
 *
 * @return true if inside the box
 */
template <class T>
bool Box<T>::IsInside( const Triple<T> & position ) const
{

  return position.m_x >= m_minX && position.m_x <= m_maxX &&
         position.m_y >= m_minY && position.m_y <= m_maxY &&
         position.m_z >= m_minZ && position.m_z <= m_maxZ;

} // IsInside()


/**
 * Scale the box
 *
 * @param fScale  the scale factor
 */
template <class T>
void Box<T>::Scale( const T fScale )
{
  BoundingRect<T>::Scale(fScale);
  m_minZ *= fScale;
  m_maxZ *= fScale;
} // Scale()


/**
 * Scale the box anisotropically
 *
 * @param fScaleVec  the scale factors
 */
template <class T>
void Box<T>::ScaleAnIso( const Triple<float4> & fScaleVec )
{
  m_minX *= fScaleVec.X();  m_maxX *= fScaleVec.X();  
  m_minY *= fScaleVec.Y();  m_maxY *= fScaleVec.Y();  
  m_minZ *= fScaleVec.Z();  m_maxZ *= fScaleVec.Z();  
} // ScaleAnIso()


/**
 * Ensures that the min coordinates are <= compared to the max coordinates
 */
template <class T>
void Box<T>::Normalize()
{

  if ( m_minX > m_maxX ) Swap( m_minX, m_maxX );
  if ( m_minY > m_maxY ) Swap( m_minY, m_maxY );
  if ( m_minZ > m_maxZ ) Swap( m_minZ, m_maxZ );

} // Normalize()


// $Log: Box.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.21  2003/12/22 12:46:22  frank
// Removed redundant functions
//
// Revision 3.20  2003/12/16 20:23:03  frank
// Issue #3593: Restricted the segmentation operations to the visible volume including crop box and thin slab when using the shift key
//
// Revision 3.19  2003/05/15 20:05:47  frank
// code review
//
// Revision 3.18  2002/11/20 21:17:51  michael
// made const methods const
//
// Revision 3.17  2002/11/15 23:04:45  ingmar
// added operators == and !=
//
// Revision 3.16  2002/11/14 20:46:28  ingmar
// added ToString()
//
// Revision 3.15  2002/10/14 15:33:05  michael
// maded GetInner/OuterRadius() const
//
// Revision 3.14  2002/09/25 20:58:37  ingmar
// implemented the constructor inside the classs declaration to make Intel 6.0 compiler happy
// added missing include
//
// Revision 3.13  2002/08/21 22:06:31  ingmar
// code beautify and
// allow to mix int and float boxes (i.e. for BoundWithBox)
//
// Revision 3.12  2002/08/09 20:53:08  uday
// Added BoundWithBox().
//
// Revision 3.11  2002/05/02 19:08:28  frank
// Added Vector3D interface.
//
// Revision 3.10  2002/04/26 15:57:34  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.9  2002/03/22 19:44:14  uday
// Set Min/Max points
//
// Revision 3.8  2002/03/18 23:32:16  jaddonisio
// Call base cllass copy constructor.
//
// Revision 3.7  2002/03/16 22:13:39  jaddonisio
// Copy constructor.
//
// Revision 3.6  2002/02/28 21:37:11  uday
// Inner and outer radius functions added
//
// Revision 3.5  2002/02/26 18:58:10  jaddonisio
// New functions:
//  i. GetCorner - gets i'th corner of box.
// ii. GetLength - gets length of sides.
// iii. GetMinCoord - gets coordinates of corner with min (x,y,z).
// iv. GetMaxCoord - gets coordinates of corner with max (x,y,z).
//
// Revision 3.4  2002/02/26 01:03:45  ingmar
// added return values to union
//
// Revision 3.3.2.1  2002/04/08 17:19:05  frank
// Added void return type on Union().
//
// Revision 3.3  2002/01/24 23:44:01  uday
// IfOverlap() added
// checks whether two boxes overlap
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
//    Rev 2.2   Sep 24 2001 17:15:08   liwei
// Added Scale() functions.
// 
//    Rev 2.1   Sep 19 2001 14:33:10   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:06   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:39  soeren
// renamed math directory
//
// Revision 1.3  2001/08/14 21:00:23  liwei
// Comitted for huamin to compile.
//
// Revision 1.2  2001/08/09 21:13:13  liwei
// Directly store min , max values in BoundingRect and Box, without using
// Triple and Point. The constructors are modified accordingly.
//
// Revision 1.1  2001/08/06 23:19:10  liwei
// Added Box<T> class.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Box.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Box.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
