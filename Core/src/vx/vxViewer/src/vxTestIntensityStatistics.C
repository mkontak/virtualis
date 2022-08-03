// $Id: vxTestIntensityStatistics.C,v 1.3 2006/03/30 15:13:21 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxTestIntensityStatistics.h"
#include "vxIntensityStatistics.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Unit test of vxIntensityStatistics
 * @param out stream
 * @return success
 */
bool vxTestIntensityStatistics::UnitTest(std::ostream & stream)
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestIntensityStatistics", "ValidateStatistics", ValidateStatistics, bOk);

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;
} // UnitTest()


/**
 * Performance test of vxVolume stuff
 * @param out stream
 * @return success
 */
bool vxTestIntensityStatistics::PerformanceTest(std::ostream & stream)
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestIntensityStatistics", "ValidateStatistics", ValidateStatistics, bOk);

  return bOk;
} // PerformanceTest()


/**
 * Default constructor
 */
bool vxTestIntensityStatistics::ValidateStatistics(std::ostream & stream)
{
  vxIntensityStatistics stats;
  stats.Add(1);
  stats.Add(2);
  stats.Add(3);
  stats.Add(4);
  stats.Add(5);
  stats.Add(6);
  stats.Add(7);
  stats.Add(8);
  stats.Add(9);
  stats.Add(10);
  stats.Compute();

  if ( stats.GetAverage() != float4( 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 ) / 10.0F )
  {
    stream << "Average failed with value of " << stats.GetAverage() << endl;
    return false;
  }

  if ( stats.GetMaximum() != 10 )
  {
    stream << "Maximum failed with value of " << stats.GetMaximum() << endl;
    return false;
  }

  if ( stats.GetMinimum() != 1 )
  {
    stream << "Minimum failed with value of " << stats.GetMinimum() << endl;
    return false;
  }

  if ( stats.GetNumberEntries() != 10 )
  {
    stream << "GetNumberEntries() failed with value of " << stats.GetNumberEntries() << endl;
    return false;
  }

  // average is correct by now, so use that as truth
  float4 fSumDifferenceSquared(0.0F);
  for ( int4 i(1); i<=10; ++i )
  {
    const float4 fA( i - stats.GetAverage() );
    fSumDifferenceSquared += fA * fA;
  }
  const float4 fStandardDeviation = sqrt( fSumDifferenceSquared / ( stats.GetNumberEntries()-1 ) );
  const float4 fEpsilon( static_cast< float4 >( 1e-8 ) );
  if ( abs( stats.GetStandardDeviation() - fStandardDeviation ) > fEpsilon )
  {
    stream << "GetStandardDeviation() failed with value of " << stats.GetStandardDeviation() << endl;
    stream << "It should have been: " << fStandardDeviation << endl;
    return false;
  }

  return true;
} // ValidateStatistics()


#undef FILE_REVISION


// $Log: vxTestIntensityStatistics.C,v $
// Revision 1.3  2006/03/30 15:13:21  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/05/17 18:31:31  frank
// avoided floating point comparison
//
// Revision 1.1  2005/05/12 14:13:52  frank
// added more unit and performance tests
//
// Revision 1.2  2003/05/08 13:13:11  frank
// code review
//
// Revision 1.1  2003/05/08 12:16:08  frank
// Statistic -> Statistics
//
// Revision 1.2  2003/05/06 15:08:38  michael
// fixed compiler error
//
// Revision 1.1  2003/05/06 14:54:16  michael
// added test class(es) for 70_vxUtils and vxIntensityStatistic
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxTestIntensityStatistics.C,v 1.3 2006/03/30 15:13:21 geconomos Exp $
// $Id: vxTestIntensityStatistics.C,v 1.3 2006/03/30 15:13:21 geconomos Exp $
