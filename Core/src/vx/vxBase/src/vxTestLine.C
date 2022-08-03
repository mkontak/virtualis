// $Id: vxTestLine.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTestLine.h"
#include "Line.h"
#include "Timer.h"
#include "Utilities.h"
#include "Vector3D.h"


// namespaces
using namespace std;


/**
 * Unit test of Plane
 *
 * @param stream out stream
 * @return success
 */
bool vxTestLine::UnitTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  VXTEST_VALIDATE_FUNCTION( stream, "vxTestLine", "ValidateConstructors", ValidateConstructors, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestLine", "ValidateGetDistance", ValidateGetDistance, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestLine", "ValidateGetPoint", ValidateGetPoint, bOk );

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
bool vxTestLine::PerformanceTest(std::ostream & stream)
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
bool vxTestLine::ValidateConstructors( std::ostream & stream )
{
  Line line1( Point3Df( 0, 0, 0 ), Point3Df( 1, 2, 3 ) );
  const float4 fEpsilon( 1e-5F );
  const Point3Df point( line1.GetPoint() );

  if ( point.m_x * 2 != point.m_y || point.m_x * 3 != point.m_z ) return false;

  return true;
} // ValidateConstructors()


/**
 * validate get point
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestLine::ValidateGetPoint( std::ostream & stream )
{
  Line line1( Point3Df( 1, 2, 3 ), Point3Df( 5, 2, 3 ) );
  const float4 fEpsilon( 1e-5F );
  const Point3Df point( line1.GetPoint() );

  if ( point.m_y != 2 || point.m_z != 3 ) return false;

  return true;
} // ValidateGetPoint()


/**
* validate get direction
*
* @param stream out stream
*
* @return success
*/
bool vxTestLine::ValidateGetDistance( std::ostream & stream )
{
  Line line1( Point3Df( 0, 0, 0 ), Point3Df( 1, 2, 3 ) );
  const float4 fEpsilon( 1e-5F );
  const Normal3Df actualDirection( 1, 2, 3 );
  if ( fabs( line1.GetDirection().X() - actualDirection.X() ) > fEpsilon ) return false;
  if ( fabs( line1.GetDirection().Y() - actualDirection.Y() ) > fEpsilon ) return false;
  if ( fabs( line1.GetDirection().Z() - actualDirection.Z() ) > fEpsilon ) return false;

  return true;
} // ValidateGetDistance()


/**
 * validate get intersection
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestLine::ValidateGetIntersection( std::ostream & stream )
{
  Point3Df point( 2, -4, 2 );
  Line line1( point, Point3Df( 5, 2, 5 ) );
  Line line2( point, Point3Df( -5, 34, 23 ) );
  const float4 fEpsilon( 1e-5F );

  Point3Df intersection( line1.GetIntersection( line2 ) );

  if ( ( intersection - point ).GetLength() > fEpsilon ) return false;

  return true;
} // ValidateGetIntersection()


// $Log: vxTestLine.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/12 13:57:14  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/14 20:41:00  frank
// added Line unit test
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestLine.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestLine.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
