// $Id: vxTest_70_algoImageProcessing.C,v 1.3 2006/03/30 15:13:21 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Dongqing Chen (dongqing@viatronix.com)


// includes
#include "stdafx.h"
#include "vxTest_70_algoImageProcessing.h"


// namespaces
using namespace std;


/**
 * Unit test of logger stuff
 * @param out stream
 * @return success
 */
bool vxTest_70_algoImageProcessing::UnitTest(std::ostream & stream)
//*******************************************************************
{
  string sLabel("Class");
  std::vector<vxTest *> vpTest;
 
  //vpTest.push_back( new vxTestVQsegmentation );
  //vpTest.push_back( new vxTestFeatureRegionGrow );

  bool bOk = PerformAndFormatTest(stream, vpTest, vxTest::UNIT, sLabel);

  CleanVector(vpTest);
 
  return bOk;
} // UnitTest()


/**
* Performance test of logger stuff
* @param out stream
* @return success
*/
bool vxTest_70_algoImageProcessing::PerformanceTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  string sLabel("Class");
  std::vector<vxTest *> vpTest;

  CleanVector(vpTest);

  return bOk;
} // PerformanceTest()


// $Log: vxTest_70_algoImageProcessing.C,v $
// Revision 1.3  2006/03/30 15:13:21  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/05/17 18:21:33  frank
// unit test now pays attention to the test results
//
// Revision 1.1  2005/05/12 13:58:50  frank
// added more unit and performance tests
//
// Revision 1.4  2003/08/25 20:43:40  dongqing
// add FeatureRegionGrow class
//
// Revision 1.3  2003/07/31 17:03:43  dongqing
// comments
//
// Revision 1.2  2003/07/29 20:05:54  dongqing
// add performance test definition
//
// Revision 1.1  2003/07/29 19:55:18  dongqing
// initializ the vxTestVQsegmentation
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxTest_70_algoImageProcessing.C,v 1.3 2006/03/30 15:13:21 geconomos Exp $
// $Id: vxTest_70_algoImageProcessing.C,v 1.3 2006/03/30 15:13:21 geconomos Exp $
