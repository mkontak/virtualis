// $Id: vxTest.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
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
#include "vxTest.h"


// static member initialization
std::string vxTest::m_sPath = "C:/v-system/test/Visualization";


// namespaces
using namespace std;


/**
 * Unit test of logger stuff
 * @param out stream
 * @param test vector
 * @param test type
 * @param test label
 * @return success
 */
bool vxTest::PerformAndFormatTest(std::ostream & stream,
                                  std::vector<vxTest *> & vpTest,
                                  vxTestEnum eTest,
                                  string & sLabel)
//*******************************************************************
{

  bool bOkAll(true);

  for (std::vector<vxTest *>::iterator iter=vpTest.begin(); iter != vpTest.end(); ++iter)
  {
    stream << "================================" << endl;
    stream << sLabel << ": " << (*iter)->GetName().c_str() << endl;

    bool bOk( true );
    string sTestName( "<UNKNOWN>" );

    // perform actual test
    if (eTest == vxTestEnum::UNIT)
    {
      bOk = (*iter)->UnitTest(stream);
      sTestName = "unit test";
    }
    else if (eTest == vxTestEnum::PERFORMANCE)
    {
      bOk = (*iter)->PerformanceTest(stream);
      sTestName = "performance test";
    }

    stream << "... completed " << sTestName << " " << (*iter)->GetName().c_str() << " (" << ( bOk ? "succesful" : "FAILED" ) << ")" << endl;

    bOkAll &= bOk;
  }

  return bOkAll;

} // UnitTest()


/**
 * Clean a vector
 * @param vector
 */
void vxTest::CleanVector(std::vector<vxTest *> & vpTest)
//*******************************************************************
{
  std::vector<vxTest *>::iterator iter;
  while (vpTest.begin() != vpTest.end())
  {
    vxTest * pTest = *vpTest.begin();
    delete pTest;
    vpTest.erase(vpTest.begin());
  }

  return;
}


// $Log: vxTest.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/10 20:03:33  frank
// revived from vx
//
// Revision 1.4  2003/08/31 03:51:39  frank
// Fixed logic so overall result is carried through to end
//
// Revision 1.3  2003/05/02 18:28:52  michael
// added more stuff
//
// Revision 1.2  2003/05/02 15:59:52  michael
// added initial version for testing
//
// Revision 1.1  2003/05/01 20:44:33  michael
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxTest.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
// $Id: vxTest.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
