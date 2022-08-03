// $Id: vxTestTetrahedron.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTestTetrahedron.h"
#include "vxTetrahedron.h"
#include "Utilities.h"
#include "Matrix.h"


// namespaces
using namespace std;


/**
 * Unit test of vxTetrahedron
 *
 * @param stream out stream
 * @return success
 */
bool vxTestTetrahedron::UnitTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  VXTEST_VALIDATE_FUNCTION( stream, "vxTestTetrahedron", "ValidateVolume", ValidateVolume, bOk );

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;

} // UnitTest()


/**
 * Performance test of vxTetrahedron
 *
 * @param stream out stream
 * @return success
 */
bool vxTestTetrahedron::PerformanceTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  stream << "No performance test is necessary at this time." << endl;

  return bOk;

} // PerformanceTest()


/**
 * Test the vxTetrahedron volume
 *
 * @param stream out stream
 * @return success
 */
bool vxTestTetrahedron::ValidateVolume( std::ostream & stream )
{
  // test many times at random transformations
  const uint4 uNumTests( 50 );
  for ( uint4 i(0); i<uNumTests; ++i )
  {

    // design a simple known-volume tetrahedron
    Point3Df p[4] = { Point<float4>(0,0,0), Point<float4>(1,0,0), Point<float4>(0,1,0), Point<float4>(0,0,1) };

    // affine transform it somewhere random
    Matrix<float4> m;
    m.Translate( Vector3Df( Drand48(-10, 10), Drand48(-10, 10), Drand48(-10, 10) ) );
    m.Rotate( Normal3Df( Drand48(-1, 1), Drand48(-1, 1), Drand48(-1, 1) ), Drand48( -M_PI, M_PI ) );
    for ( int i(0); i<4; ++i )
      p[i] = m * p[i];
    vxTetrahedron tetra( p[0], p[1], p[2], p[3] );

    // calculate it
    const float4 fCalculatedVolume( tetra.GetVolume() );
    const float4 fActualVolume( 1.0F / 6.0F );
    const float4 fEpsilon( 1e-5F );

    // check it
    if ( fabs( fCalculatedVolume - fActualVolume ) > fEpsilon )
      return false;

  } // end random transformations

  return true;

} // ValidateVolume()


// $Log: vxTestTetrahedron.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/12 13:57:14  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/21 14:21:58  frank
// Added test for vxTetrahedron
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestTetrahedron.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestTetrahedron.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
