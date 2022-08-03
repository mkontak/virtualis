// $Id: Plane.inl,v 1.5 2008/01/29 17:36:37 gdavidson Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


// Includes
#include "Normal3D.h"
#include "PrimitiveUtilities.h"
#include <math.h>


/**
 * Constructor for the class Plane.
 */
inline Plane::Plane()
: m_fA(1),
m_fB(0),
m_fC(0),
m_fD(0)
{
} // Plane()


/**
 * Copy constructor.
 * @param other
 */
Plane::Plane( const Plane & other )
: m_fA( other.m_fA ),
m_fB( other.m_fB ),
m_fC( other.m_fC ),
m_fD( other.m_fD )
{
} // Plane()


/**
 * Constructor for the class Plane
 *
 * @param pointInPlane
 * @param normalToPlane
 */
inline Plane::Plane( const Point3Df & pointInPlane, const Normal3Df & normalToPlane )
{
  m_fA = normalToPlane.X();
  m_fB = normalToPlane.Y();
  m_fC = normalToPlane.Z();
  m_fD = -1 * Vector3Df( pointInPlane ).Dot( Vector3Df( normalToPlane ) );
  NormalizeCoefficients();
} // Plane( point, normal )


/**
 * Constructor from three points
 *
 * @param point1 the first point
 * @param point2 the second point
 * @param point3 the third point
 */
Plane::Plane( const Point3Df & point1, const Point3Df & point2, const Point3Df & point3 )
{
  Normal3Df normal( Normal3Df( point2 - point1 ).Cross( Normal3Df( point3 - point1 ) ) );
  * this = Plane( point1, normal );
} // Plane( point, point, point )


/**
 * Constructor for the class Plane
 *
 * @param fCoefficientA plane coefficient A
 * @param fCoefficientB plane coefficient B
 * @param fCoefficientC plane coefficient C
 * @param fCoefficientD plane coefficient D
 */
inline Plane::Plane( const float4 & fCoefficientA, const float4 & fCoefficientB, const float4 & fCoefficientC, const float4 & fCoefficientD )
: m_fA( fCoefficientA ),
m_fB( fCoefficientB ),
m_fC( fCoefficientC ),
m_fD( fCoefficientD )
{
  NormalizeCoefficients();
} // Plane( a, b, c, d )


/**
 * Assignment Operator: from Plane
 *
 * @param other the other plane
 * @return reference to this plane
 */
inline Plane & Plane::operator=( const Plane & other )
{
  m_fA = other.m_fA;
  m_fB = other.m_fB;
  m_fC = other.m_fC;
  m_fD = other.m_fD;
  return * this;
} // operator=()


/**
 * Returns the signed distance from this plane
 *
 * @param point  the point in question
 *
 * @return the signed distance from this plane
 */
inline float4 Plane::GetDistance( const Point3D<float4> & point ) const
{
  return point.m_x * m_fA + point.m_y * m_fB + point.m_z * m_fC + m_fD;
} // GetDistance()


/**
 * Returns whether or not the point is on the positive side of the plane.
 *
 * @param point  the point in question
 *
 * @return true if on positive side
 */
inline bool Plane::IsOnPositiveSide( const Point3D<float4> & point ) const
{
  return GetDistance(point) >= 0;
} // IsOnPositiveSide()


/**
 * Projects a vector so that it becomes parallel to the plane.
 *
 * @param inVector the input vector
 *
 * @return the projected vector
 */
inline Vector3Df Plane::GetProjectionParallelToPlane( const Vector3Df & inVector ) const
{

  const Normal3Df normalVector( m_fA, m_fB, m_fC );

  return PrimitiveUtilities::GetProjectionParallelToPlane( inVector, normalVector );

} // ProjectVectorOntoPlane()


/**
 * Projects a point so that it lies within the plane.
 */
inline Point3D<float4> Plane::GetProjectionOntoPlane( const Point3D<float4> & inPoint ) const
{
  // find out how far away it is and on what side
  const float fDistanceFromPlane( GetDistance( inPoint ) );

  // translate it back to lie within the plane, translate along the normal direction
  return Point3D<float4>( Vector<float4>(inPoint) - Vector<float4>(m_fA, m_fB, m_fC) * fDistanceFromPlane );

} // GetProjectionOntoPlane()


/** 
 * Returns the value of the first coefficient (m_fA) of a Normal<float>
 *
 * @return the plane coefficient A
 */
inline float4 Plane::GetA()const
{
  return m_fA;
} // end of A() const


/** 
 * Returns the value of the second coefficient (m_fB) of a Normal<float4>
 *
 * @return the plane coefficient B
 */
inline float4 Plane::GetB()const
{
  return m_fB;
} // end of B() const


/** 
 * Returns the value of the third coefficient (m_fC) of a Normal<float4>
 *
 * @return the plane coefficient C
 */
inline float4 Plane::GetC()const
{
  return m_fC;
} // end of C() const


/** 
 * Returns the value of the fourth coefficient (m_fD) of a Normal<float4>
 *
 * @return the plane coefficient D
 */
inline float4 Plane::GetD()const
{
  return m_fD;
} // end of D() const


/**
 * Returns a point on the plane.
 */
inline Point3D<float4> Plane::GetPointOnPlane() const
{
  return GetProjectionOntoPlane( Point3D<float4>( 0,0,0 ) );
} // GetPointOnPlane()


/**
 * Normalize the coefficients.
 */
inline void Plane::NormalizeCoefficients()
{
  const float4 fDenominator( static_cast< float4 >( sqrt(m_fA*m_fA + m_fB*m_fB + m_fC*m_fC)) );
  if (fDenominator != 0.0F)
  {
    m_fA /= fDenominator;
    m_fB /= fDenominator;
    m_fC /= fDenominator;
    m_fD /= fDenominator;
  }
} // NormalizeCoefficients()


/**
 * Overloaded << Operator.
 * Directs the three data members to the ostream, reference of which is passed
 * AsOstream member function is called which is virtual, hence the appropriate
 * function is called, which shows the class of the object of which it is an 
 * instance of.
 */
inline std::ostream & operator<<(std::ostream & os, const Plane & plane)
{
  os << "Plane:(" ;
  os << plane.GetA() << "," ;
  os << plane.GetB() << "," ;
  os << plane.GetC() << "," ;
  os << plane.GetD();
  os << ")"; 
  return os;
} // operator<<()


/**
 * operator >>
 *
 * @param is
 * @param plane
 * @return if
 */
inline std::istream & operator>>(std::istream & is, Plane & plane)
{
  // skip "Plane:("
  char iOneChar='x';
  for( ; iOneChar!= '('; is >> iOneChar ) 
    ; // noop
  
  is >> plane.m_fA >> iOneChar; //","
  is >> plane.m_fB >> iOneChar; //","
  is >> plane.m_fC >> iOneChar; //","
  is >> plane.m_fD >> iOneChar; //")"
  
  return is;
} // operator>>()

inline Vector3Df GetProjectionParallelToPlane( const Vector3Df & vector3D, const Normal3Df & normalToPlane )
{
 // Project the vector so that it's perpendicular to the normal vector.
 
 // The dot product between them should be zero.
 float4 fDotProduct(normalToPlane.Dot(vector3D));

 // move the tip of the vector by the amount of the dotproduct against he normal direction.
 return Vector3Df(vector3D) - Vector3Df(normalToPlane) * fDotProduct;
 // now computing the dot product again would result in a zero value
} // GetProjectionParallelToPlane() 



// $Log: Plane.inl,v $
// Revision 1.5  2008/01/29 17:36:37  gdavidson
// Changed any reference of Point to Point3D
//
// Revision 1.4  2005/08/05 13:03:50  geconomos
// added GetProjectionParallelToPlane()
//
// Revision 1.3  2005/05/11 12:58:23  frank
// fixed include file
//
// Revision 1.2  2005/05/11 12:36:41  frank
// added necessary include file
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.14  2003/08/31 03:53:33  frank
// Added constructor from three points
//
// Revision 3.13  2003/05/23 12:35:31  frank
// Moved utility functions to PrimitiveUtilities
//
// Revision 3.12  2003/05/20 16:04:44  dongqing
// code review
//
// Revision 3.11  2003/05/14 19:33:58  frank
// code review
//
// Revision 3.10  2003/01/24 20:25:42  jmeade
// Inline method (GetProjectParallelToPlane) needs to be in the .inl file.
//
// Revision 3.9  2003/01/24 18:19:01  frank
// Removed Plane template parameter
//
// Revision 3.8  2003/01/22 20:58:59  ingmar
// moved GetProjectionParallelToPlane() to .C
//
// Revision 3.7  2001/01/02 18:00:11  ingmar
// added independent function GetProjectionParallelToPlane()
//
// Revision 3.6  2001/01/02 16:16:15  ingmar
// added independent function GetProjectionParallelToPlane()
//
// Revision 3.5  2002/11/21 19:26:32  frank
// added point projection
//
// Revision 3.4  2002/08/15 18:11:57  wenli
// Moved NormalizeCoefficients to inline file.
//
// Revision 3.3  2002/06/21 16:18:17  frank
// Normalized the coefficients.
//
// Revision 3.2.2.1  2002/05/28 20:25:04  frank
// Normalized the coefficients.
//
// Revision 3.2  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.1  2002/03/05 19:50:43  ingmar
// added missing braces that Intel C++ compiler complained about
//
// Revision 3.0  2001/10/14 23:02:17  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
// 
//    Rev 2.1   Sep 19 2001 14:33:14   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:52   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:10   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.6  2001/08/20 14:39:48  frank
// Added vector projection onto the plane.
//
// Revision 1.5  2001/05/31 21:32:36  ana
// changes regarding code review
//
// Revision 1.4  2001/05/30 20:29:43  ana
// changed the members access function
//
// Revision 1.3  2001/05/29 18:39:29  ana
// took care of the warnings
//
// Revision 1.2  2001/05/25 20:52:34  ana
// Plane.C added and TestByTyple method created
//
// Revision 1.1  2001/04/30 14:38:17  frank
// New plane class.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Plane.inl,v 1.5 2008/01/29 17:36:37 gdavidson Exp $
// $Id: Plane.inl,v 1.5 2008/01/29 17:36:37 gdavidson Exp $
