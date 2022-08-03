// $Id: vxTest_15_dicom.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTest_15_dicom.h"


// namespaces
using namespace std;


/**
 * Unit test of logger stuff
 * @param out stream
 * @return success
 */
bool vxTest_15_dicom::UnitTest(std::ostream & stream)
//*******************************************************************
{
  string sLabel("Class");
  std::vector<vxTest *> vpTest;

  bool bOk = PerformAndFormatTest(stream, vpTest, vxTest::UNIT, sLabel);
  CleanVector(vpTest);

  return bOk;
} // UnitTest()


/**
 * Performance test of logger stuff
 * @param out stream
 * @return success
 */
bool vxTest_15_dicom::PerformanceTest(std::ostream & stream)
//*******************************************************************
{
  stream << "There is currently no performance test implemented in module: " << GetName() << endl;

  return true;
} // PerformanceTest()


// $Log: vxTest_15_dicom.C,v $
// Revision 1.3  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/05/17 18:21:32  frank
// unit test now pays attention to the test results
//
// Revision 1.1  2005/05/12 14:15:47  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/02 18:28:39  michael
// initial version
//
// Revision 1.1  2003/05/02 15:59:53  michael
// added initial version for testing
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTest_15_dicom.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTest_15_dicom.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
