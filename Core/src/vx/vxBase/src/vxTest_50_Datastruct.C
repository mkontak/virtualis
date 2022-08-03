// $Id: vxTest_50_Datastruct.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
// 
// Copyright © 2003, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Owner: Frank Dachille (dachille@viatronix.com)


// includes
#include "stdafx.h"
#include "vxTest_50_datastruct.h"


// namespaces
using namespace std;


/**
 * Unit test of datastruct stuff
 * @param out stream
 * @return success
 */
bool vxTest_50_datastruct::UnitTest(std::ostream & stream)
{
  string sLabel("Class");
  std::vector<vxTest *> vpTest;

  //vpTest.push_back(new vxTestIncrementalIntensityStatistics);

  bool bOk = PerformAndFormatTest(stream, vpTest, vxTest::UNIT, sLabel);
  CleanVector(vpTest);

  return bOk;
} // UnitTest()


/**
 * Performance test of vxUtils stuff
 * @param out stream
 * @return success
 */
bool vxTest_50_datastruct::PerformanceTest(std::ostream & stream)
{
  string sLabel("Class");
  std::vector<vxTest *> vpTest;

  //vpTest.push_back(new vxTestIncrementalIntensityStatistics);

  bool bOk = PerformAndFormatTest(stream, vpTest, vxTest::PERFORMANCE, sLabel);
  CleanVector(vpTest);

  return bOk;
} // PerformanceTest()


// $Log: vxTest_50_Datastruct.C,v $
// Revision 1.3  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/05/17 18:21:32  frank
// unit test now pays attention to the test results
//
// Revision 1.1  2005/05/12 13:57:44  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/12 16:19:35  frank
// Added 50_datastruct tests
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTest_50_Datastruct.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTest_50_Datastruct.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
