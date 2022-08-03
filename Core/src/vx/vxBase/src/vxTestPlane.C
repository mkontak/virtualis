// $Id: vxTestPlane.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxTestPlane.h"
#include "Plane.h"
#include "Utilities.h"


// namespaces
using namespace std;


/**
 * Unit test of Plane
 *
 * @param stream out stream
 * @return success
 */
bool vxTestPlane::UnitTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  VXTEST_VALIDATE_FUNCTION( stream, "vxTestPlane", "ValidateConstructors", ValidateConstructors, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestPlane", "ValidateGetDistance", ValidateGetDistance, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestPlane", "ValidateIsOnPositiveSide" , ValidateIsOnPositiveSide, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestPlane", "ValidateGetDistance" , ValidateGetDistance, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestPlane", "ValidateGetNormal" , ValidateGetNormal, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestPlane", "ValidateGetProjectionOntoPlane" , ValidateGetProjectionOntoPlane, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestPlane", "ValidateGetProjectionParallelToPlane" , ValidateGetProjectionParallelToPlane, bOk );

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;

} // UnitTest()


/**
 * Performance test of vxVolume stuff
 *
 * @param stream out stream
 * @return success
 */
bool vxTestPlane::PerformanceTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  stream << "No performance test is necessary at this time." << endl;

  return bOk;

} // PerformanceTest()


/**
 * Test the plane constructor
 *
 * @param stream out stream
 * @return success
 */
bool vxTestPlane::ValidateConstructors( std::ostream & stream )
{
  // test constructor from point and normal
  Plane plane1( Point3Df( 1, 2, 3 ), Normal3Df( 1, 1, 1 ) );
  // d = -6 * sqrt(3)
  const float4 fEpsilon( 1e-5F );
  const float4 fActualD( -6 / float4( sqrt(3.0f) ) );
  if ( fabs( plane1.GetA() - 1 / float4( sqrt(3.0f) ) ) > fEpsilon ) return false;
  if ( fabs( plane1.GetB() - 1 / float4( sqrt(3.0f) ) ) > fEpsilon ) return false;
  if ( fabs( plane1.GetC() - 1 / float4( sqrt(3.0f) ) ) > fEpsilon ) return false;
  if ( fabs( plane1.GetD() - fActualD ) > fEpsilon ) return false;

  // test constructor from 4 coefficients
  Normal3Df normal( 1, 2, 3 );
  Plane plane2( normal.X(), normal.Y(), normal.Z(), 4 );
  if ( fabs( plane2.GetA() - normal.X() ) > fEpsilon ) return false;
  if ( fabs( plane2.GetB() - normal.Y() ) > fEpsilon ) return false;
  if ( fabs( plane2.GetC() - normal.Z() ) > fEpsilon ) return false;
  if ( fabs( plane2.GetD() - 4 ) > fEpsilon ) return false;

  // test copy constructor
  Plane plane3( plane2 );
  if ( fabs( plane3.GetA() - normal.X() ) > fEpsilon ) return false;
  if ( fabs( plane3.GetB() - normal.Y() ) > fEpsilon ) return false;
  if ( fabs( plane3.GetC() - normal.Z() ) > fEpsilon ) return false;
  if ( fabs( plane3.GetD() - 4 ) > fEpsilon ) return false;

  // test assignment operator
  Plane plane4 = plane2;
  if ( fabs( plane4.GetA() - normal.X() ) > fEpsilon ) return false;
  if ( fabs( plane4.GetB() - normal.Y() ) > fEpsilon ) return false;
  if ( fabs( plane4.GetC() - normal.Z() ) > fEpsilon ) return false;
  if ( fabs( plane4.GetD() - 4 ) > fEpsilon ) return false;

  // test constructor from three points
  Point3Df point1( 0, 0, 0 );
  Point3Df point2( 1, 0, 0 );
  Point3Df point3( 0, 1, 0 );
  Plane plane5( point1, point2, point3 );
  if ( fabs( plane5.GetA() - 0 ) > fEpsilon ) return false;
  if ( fabs( plane5.GetB() - 0 ) > fEpsilon ) return false;
  if ( fabs( plane5.GetC() - 1 ) > fEpsilon ) return false;
  if ( fabs( plane5.GetD() - 0 ) > fEpsilon ) return false;

  return true;

} // ValidateConstructors()


/**
* validate get distance
*
* @param stream out stream
*
* @return success
*/
bool vxTestPlane::ValidateGetDistance( std::ostream & stream )
{
  Point3Df point( 5, 6, 7 );
  Plane plane( point, Normal3Df( 3, 5, -6 ) );
  const float4 fEpsilon( 1e-5F );

  if ( plane.GetDistance( point ) > fEpsilon ) return false;

  return true;
} // ValidateGetDistance()


/**
* validate is on positive side
*
* @param stream out stream
*
* @return success
*/
bool vxTestPlane::ValidateIsOnPositiveSide( std::ostream & stream )
{
  Plane plane( Point3Df( 1, 1, 1 ), Normal3Df( 1, 1, 1 ) );

  if ( plane.IsOnPositiveSide( Point3Df( 50, 20, 30 ) ) != true ) return false;
  if ( plane.IsOnPositiveSide( Point3Df( -50, -20, -30 ) ) != false ) return false;

  return true;
} // ValidateGetDistance()


/**
 * validate get point on plane
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestPlane::ValidateGetPointOnPlane( std::ostream & stream )
{
  Plane plane( Point3Df( 54, -84, 25 ), Normal3Df( -52, 13, -53 ) );
  const float4 fEpsilon( 1e-5F );

  if ( plane.GetDistance( plane.GetPointOnPlane() ) > fEpsilon ) return false;

  return true;
} // ValidateGetDistance()


/**
 * validate get normal
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestPlane::ValidateGetNormal( std::ostream & stream )
{
  Normal3Df normal( -52, 13, -53 );
  Plane plane( Point3Df( 54, -84, 25 ), normal );
  const float4 fEpsilon( 1e-5F );

  if ( fabs( normal.X() - plane.GetNormal().X() +
             normal.Y() - plane.GetNormal().Y() +
             normal.Z() - plane.GetNormal().Z() ) > fEpsilon )
  {
    return false;
  }

  return true;
} // ValidateGetDistance()


/**
* validate get projection onto plane
*
* @param stream out stream
*
* @return success
*/
bool vxTestPlane::ValidateGetProjectionOntoPlane( std::ostream & stream )
{
  Plane plane( Point3Df( 54, -84, 25 ), Normal3Df( -52, 13, -53 ) );
  const float4 fEpsilon( 1e-5F );
  Point3Df point( 4, 5, -6 );

  if ( plane.GetDistance( plane.GetProjectionOntoPlane( point ) ) > fEpsilon ) return false;

  return true;
} // ValidateGetProjectionOntoPlane()


/**
 * validate get projection parallel to plane
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestPlane::ValidateGetProjectionParallelToPlane( std::ostream & stream )
{
  Plane plane( Point3Df( 54, -84, 25 ), Normal3Df( -52, 13, -53 ) );
  const float4 fEpsilon( 1e-5F );
  Vector3Df originalVector( 4, 5, -6 );
  Vector3Df projectedVector( plane.GetProjectionParallelToPlane( originalVector ) );
  Point3Df pointOnTipOfProjectedVector( plane.GetPointOnPlane() + projectedVector );

  if ( plane.GetDistance( pointOnTipOfProjectedVector ) > fEpsilon ) return false;

  return true;
} // ValidateGetProjectionOntoPlane()


// $Log: vxTestPlane.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/10 20:03:33  frank
// revived from vx
//
// Revision 1.3  2003/08/31 03:50:08  frank
// Added test for new plane constructor
//
// Revision 1.2  2003/05/14 19:33:24  frank
// code review
//
// Revision 1.1  2003/05/14 18:10:10  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestPlane.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestPlane.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
