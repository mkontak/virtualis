// $Id: vxTest_70_vxUtils.C,v 1.3 2006/03/30 15:13:21 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxTest_70_vxUtils.h"
#include "vxTestIntensityStatistics.h"


// namespaces
using namespace std;


/**
 * Unit test of vxVolume stuff
 * @param out stream
 * @return success
 */
bool vxTest_70_vxUtils::UnitTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  string sLabel("Class");
  std::vector<vxTest *> vpTest;

  vpTest.push_back(new vxTestIntensityStatistics);

  bOk &= PerformAndFormatTest(stream, vpTest, vxTest::UNIT, sLabel);
  CleanVector(vpTest);

  return bOk;
} // UnitTest()


/**
 * Performance test of vxVolume stuff
 * @param out stream
 * @return success
 */
bool vxTest_70_vxUtils::PerformanceTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  string sLabel("Class");
  std::vector<vxTest *> vpTest;

  vpTest.push_back(new vxTestIntensityStatistics);

  bOk &= PerformAndFormatTest(stream, vpTest, vxTest::PERFORMANCE, sLabel);
  CleanVector(vpTest);

  return bOk;
} // PerformanceTest()


// Revision History:
// $Log: vxTest_70_vxUtils.C,v $
// Revision 1.3  2006/03/30 15:13:21  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/05/17 18:21:33  frank
// unit test now pays attention to the test results
//
// Revision 1.1  2005/05/12 13:58:50  frank
// added more unit and performance tests
//
// Revision 1.2  2003/05/08 13:13:35  frank
// Statistic -> Statistics
//
// Revision 1.1  2003/05/06 14:54:16  michael
// added test class(es) for 70_vxUtils and vxIntensityStatistic
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxTest_70_vxUtils.C,v 1.3 2006/03/30 15:13:21 geconomos Exp $
// $Id: vxTest_70_vxUtils.C,v 1.3 2006/03/30 15:13:21 geconomos Exp $
