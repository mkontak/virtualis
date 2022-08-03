// $Id: vxTestValueRange.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTestValueRange.h"
#include "Utilities.h"
#include "ValueRange.h"


// namespaces
using namespace std;


/**
 * Unit test of ValueRange
 *
 * @param stream out stream
 * @return success
 */
bool vxTestValueRange::UnitTest(std::ostream & stream)
{

  bool bOk( true );
  Timer timer;

  VXTEST_VALIDATE_FUNCTION( stream, "vxTestValueRange", "ValidateConstructors", ValidateConstructors, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestValueRange", "ValidateAddValue", ValidateAddValue, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestValueRange", "ValidateGetMinimum", ValidateGetMinimum, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestValueRange", "ValidateGetMaximum", ValidateGetMaximum, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestValueRange", "ValidateGetPercentile", ValidateGetPercentile, bOk );
  VXTEST_VALIDATE_FUNCTION( stream, "vxTestValueRange", "ValidateEqualityOperator", ValidateEqualityOperator, bOk );

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;

} // UnitTest()


/**
 * Performance test of ValueRange
 *
 * @param stream out stream
 * @return success
 */
bool vxTestValueRange::PerformanceTest(std::ostream & stream)
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
bool vxTestValueRange::ValidateConstructors( std::ostream & stream )
{
  // test default constructor
  ValueRange<float4> range1;
  const float4 fValue( 57.3452F );
  range1.AddValue( fValue );
  if ( range1.GetMinimum() != fValue ) return false;
  if ( range1.GetMaximum() != fValue ) return false;

  // test convenience constructor
  ValueRange<float4> range2( fValue );
  if ( range2.GetMinimum() != fValue ) return false;

  return true;
} // ValidateConstructors()


/**
 * validate add value
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestValueRange::ValidateAddValue( std::ostream & stream )
{
  ValueRange<float4> range;
  const float4 fValue1( 57.3452F );
  range.AddValue( fValue1 );

  if ( range.GetMinimum() != fValue1 ) return false;
  if ( range.GetMaximum() != fValue1 ) return false;

  const float4 fValue2( -234.12235F );
  range.AddValue( fValue2 );

  if ( range.GetMinimum() != fValue2 ) return false;
  if ( range.GetMaximum() != fValue1 ) return false;

  return true;
} // ValidateAddValue()


/**
 * validate get minimum
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestValueRange::ValidateGetMinimum( std::ostream & stream )
{
  ValueRange<int4> range;
  range.AddValue( 6 );
  range.AddValue( 7 );
  range.AddValue( 8 );

  if ( range.GetMinimum() != 6 ) return false;

  return true;
} // ValidateGetMinimum()


/**
 * validate get maximum
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestValueRange::ValidateGetMaximum( std::ostream & stream )
{
  ValueRange<int4> range;
  range.AddValue( 6 );
  range.AddValue( 7 );
  range.AddValue( 8 );

  if ( range.GetMaximum() != 8 ) return false;

  return true;
} // ValidateGetMaximum()


/**
 * validate get percentile
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestValueRange::ValidateGetPercentile( std::ostream & stream )
{
  ValueRange<int4> range;
  range.AddValue( 0 );
  range.AddValue( 14 );
  range.AddValue( 78 );
  range.AddValue( 100 );

  if ( range.GetPercentile( 0   ) != 0.0F ) return false;
  if ( range.GetPercentile( 50  ) != 0.5F ) return false;
  if ( range.GetPercentile( 100 ) != 1.0F ) return false;

  return true;
} // ValidateGetPercentile()


/**
 * validate equality operator
 *
 * @param stream out stream
 *
 * @return success
 */
bool vxTestValueRange::ValidateEqualityOperator( std::ostream & stream )
{
  ValueRange<int4> range1;
  ValueRange<int4> range2;

  const float4 fValue1( 322423.123F );
  const float4 fValue2( 723.239F );

  range1.AddValue( fValue1 );
  range1.AddValue( fValue2 );
  range2.AddValue( fValue1 );
  range2.AddValue( fValue2 );

  return range1 == range2;
} // ValidateEqualityOperator()


// $Log: vxTestValueRange.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/12 13:57:14  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/15 15:03:26  frank
// added test for ValueRange
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTestValueRange.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTestValueRange.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
