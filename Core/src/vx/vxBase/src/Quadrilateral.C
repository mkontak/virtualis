// $Id: Quadrilateral.C,v 1.2 2005/05/23 12:47:48 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachillle (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "Quadrilateral.h"
#include "Plane.h"
#include "Utilities.h"


// Defines
#define FILE_REVISION "$Revision: 1.2 $"

/**
 * Default constructor
 */
Quadrilateral::Quadrilateral()
{
  * this = Quadrilateral( Point<float4>(0,0,0), Normal<float4>(1,0,0), Normal<float4>(0,1,0), 1.0F );
} // Quadrilateral()


/**
 * Copy constructor
 *
 * @param other the other quadrilateral
 */
Quadrilateral::Quadrilateral( const Quadrilateral & other )
{
  if( &other != this )
  {
    for( uint4 i(0); i<4; ++i )
      m_vertices[i] = other.m_vertices[i];
  }
} // Quadrilatera( other )


/**
 * Assignment operator
 *
 * @param other the other quadrilateral
 */
Quadrilateral & Quadrilateral::operator =( const Quadrilateral & other )
{
  if( &other != this )
  {
    for( uint4 i(0); i<4; ++i )
      m_vertices[i] = other.m_vertices[i];
  }
  return * this;
} // operator =()


/**
 * Convenience constructor from a center, size, and orientation
 *
 * @param centerPoint  the centerPoint
 * @param upNormal     the normal which defines the up direction
 * @param rightNormal  the normal which defines the right direction
 * @param fRadius      half the size of the quadrilateral
 */
Quadrilateral::Quadrilateral(const Point<float4> & centerPoint, const Normal<float4> & upNormal, const Normal<float4> & rightNormal, const float4 fRadius)
{
  m_vertices[ BOTTOM_LEFT  ] = centerPoint + Vector<float4>(upNormal) *  fRadius + Vector<float4>(rightNormal) * -fRadius;
  m_vertices[ BOTTOM_RIGHT ] = centerPoint + Vector<float4>(upNormal) *  fRadius + Vector<float4>(rightNormal) *  fRadius;
  m_vertices[ TOP_LEFT     ] = centerPoint + Vector<float4>(upNormal) * -fRadius + Vector<float4>(rightNormal) * -fRadius;
  m_vertices[ TOP_RIGHT    ] = centerPoint + Vector<float4>(upNormal) * -fRadius + Vector<float4>(rightNormal) *  fRadius;
} // Quadrilateral()


/**
 * Convenience constructor from four points
 *
 * @param topleftPoint     the top left point
 * @param topRightPoint    the top right point
 * @param bottomleftPoint  the bottom left point
 * @param bottomRightPoint the bottom right point
 */
Quadrilateral::Quadrilateral( const Point3Df & topLeftPoint, const Point3Df & topRightPoint, const Point3Df & bottomLeftPoint, const Point3Df & bottomRightPoint )
{
  m_vertices[ BOTTOM_LEFT  ] = bottomLeftPoint;
  m_vertices[ BOTTOM_RIGHT ] = bottomRightPoint;
  m_vertices[ TOP_LEFT     ] = topLeftPoint;
  m_vertices[ TOP_RIGHT    ] = topRightPoint;
} // Quadrilateral()


/**
 * Transform the points by a float matrix
 *
 * @param transformationMatrix  the matrix which transforms each point
 */
void Quadrilateral::Transform( const Matrix<float4> & transformationMatrix )
{
  for( uint4 i(0); i<4; ++i )
    m_vertices[i] = transformationMatrix * m_vertices[i];
} // Transform()


/**
 * Compute and return the area of the shape
 *
 * @return the area
 */
float4 Quadrilateral::GetArea() const
{
  // Area formula from "The Geometry Center" - http://www.geom.umn.edu/
  // quadrilateral ABCD (0123)
  const float4 fDiag1 = ( m_vertices[1] - m_vertices[3] ).GetLength(); // line BD
  const float4 fDiag2 = ( m_vertices[0] - m_vertices[2] ).GetLength(); // line AC
  const float4 fLenA  = ( m_vertices[0] - m_vertices[1] ).GetLength(); // line AB
  const float4 fLenB  = ( m_vertices[1] - m_vertices[2] ).GetLength(); // line BC
  const float4 fLenC  = ( m_vertices[2] - m_vertices[3] ).GetLength(); // line CD
  const float4 fLenD  = ( m_vertices[3] - m_vertices[0] ).GetLength(); // line DA
  const float4 fA = 4.0F * fDiag1 * fDiag1 * fDiag2 * fDiag2;
  const float4 fB = fLenB*fLenB + fLenD*fLenD - fLenA*fLenA - fLenC*fLenC;
  return sqrt(fA - fB*fB) / 4.0F;
} // GetArea()


/**
 * Compute the projected position in normalized coordinates { (0-1), (0-1) }
 *
 * @param worldPos  the input world position
 *
 * @return the projected position in normalized coordinates { (0-1), (0-1) }
 */
Point2D<float4> Quadrilateral::GetProjectedPosition( const Point<float4> & worldPos )
{
  const Vector<float4> rightVector( m_vertices[ TOP_RIGHT ] - m_vertices[ TOP_LEFT ] );
  const Plane leftPlane( m_vertices[ TOP_LEFT ], Normal<float4>( rightVector ) );
  const float4 fDistanceX( leftPlane.GetDistance( worldPos ) );
  const float4 fX( fDistanceX / rightVector.GetLength() );

  const Vector<float4> upVector( m_vertices[ TOP_LEFT ] - m_vertices[ BOTTOM_LEFT ] );
  const Plane bottomPlane( m_vertices[ BOTTOM_LEFT ], Normal<float4>( upVector ) );
  const float4 fDistanceY( bottomPlane.GetDistance( worldPos ) );
  const float4 fY( fDistanceY / upVector.GetLength() );

  return Point2D<float4>( fX, fY );
} // GetProjectedPosition()


/**
 * Compute the projected position in world coordinates given normalized screen coordinates { (0-1), (0-1) }
 *
 * @param normalizedPos  the position within the quadrilateral in normalized coordinates { (0-1), (0-1) }
 *
 * @return the world position
 */
Point<float4> Quadrilateral::GetBackProjectedPosition( const Point2D<float4> & normalizedPos )
{
  // interpolate first in y
  const Point<float4> leftPoint ( m_vertices[ BOTTOM_LEFT  ] + ( m_vertices[ TOP_LEFT  ] - m_vertices[ BOTTOM_LEFT  ] ) * normalizedPos.m_y );
  const Point<float4> rightPoint( m_vertices[ BOTTOM_RIGHT ] + ( m_vertices[ TOP_RIGHT ] - m_vertices[ BOTTOM_RIGHT ] ) * normalizedPos.m_y );

  // then interpolate in x
  return leftPoint + ( rightPoint - leftPoint ) * normalizedPos.m_x;

} // GetBackProjectedPosition()

/**
 * Retrieve the point data
 *
 * @param eLocation the desired point specifier
 *
 * @return the position of the desired point
 */
Point<float4> Quadrilateral::GetPoint( const enum PointLocationEnum eLocation ) const
{
  switch ( eLocation )
  {
  case BOTTOM_LEFT:
  case BOTTOM_RIGHT:
  case TOP_LEFT:
  case TOP_RIGHT:
    return m_vertices[ uint4( eLocation ) ];
  	break;
  case MIDDLE:
    return Point3Df( ( Vector3Df( m_vertices[0] ) +
                       Vector3Df( m_vertices[1] ) +
                       Vector3Df( m_vertices[2] ) +
                       Vector3Df( m_vertices[3] ) ) / 4.0F );
  	break;
  default:
    std::string sLocation( ToAscii( 5 ) );
    throw ex::IllegalArgumentException( LogRec( "bad location: " + ToAscii( uint4( eLocation ) ), "Quadrilateral", "GetPoint" ) );
  	break;
  }

  return Point3Df();

} // GetPoint()


/**
 * Set the point data
 *
 * @param eLocation the desired point specifier
 * @param point     the new location
 */
void Quadrilateral::SetPoint( const enum PointLocationEnum eLocation, const Point<float4> & point )
{
  if ( uint4( eLocation ) < uint4( MIDDLE ) )
    m_vertices[ uint4( eLocation ) ]= point;
  else
    throw ex::IllegalArgumentException( LogRec( "bad location: " + ToAscii( eLocation ), "Quadrilateral", "SetPoint" ) );
} // SetPoint()


/**
 * Retrieve the plane normal
 *
 * @return the plane normal
 */
Normal3Df Quadrilateral::GetNormal() const
{
  const Normal3Df normal1( m_vertices[1] - m_vertices[0] );
  const Normal3Df normal2( m_vertices[2] - m_vertices[0] );
  return Normal3Df( normal1.Cross( normal2 ) );
} // GetNormal()


#undef FILE_REVISION


// $Log: Quadrilateral.C,v $
// Revision 1.2  2005/05/23 12:47:48  frank
// fixed file revision definition so that we get correct information in log files
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.10  2003/05/23 13:01:56  frank
// Additional constructors
//
// Revision 3.9  2003/05/20 17:51:10  frank
// code review
//
// Revision 3.8  2003/05/16 11:44:21  michael
// code review
//
// Revision 3.7  2003/03/04 17:59:21  frank
// Additional functionality
//
// Revision 3.6  2003/01/24 18:20:58  frank
// Removed Plane template parameter
//
// Revision 3.5  2003/01/22 21:12:42  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.4  2002/12/16 23:46:32  frank
// Changed to be a vector instead of four individual points.
//
// Revision 3.3  2002/11/15 18:27:38  frank
// Added back projection function
//
// Revision 3.2  2002/11/12 16:12:40  frank
// Added projection function.
//
// Revision 3.1  2002/10/17 16:14:49  frank
// Removed Quadrilateral template parameter.
//
// Revision 3.0  2001/10/14 23:02:18  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
//    Rev 2.1   Sep 19 2001 14:33:16   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:14   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.1  2001/08/27 12:32:04  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Quadrilateral.C,v 1.2 2005/05/23 12:47:48 frank Exp $
// $Id: Quadrilateral.C,v 1.2 2005/05/23 12:47:48 frank Exp $
