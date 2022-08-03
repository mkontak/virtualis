// $Id: vxTest_70_algoImageProcessing.h,v 1.1 2005/05/12 13:58:50 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Dongqing Chen (dongqing@viatronix.com)

/**
 *  This class implements the test interface for 70_algoImageProcessing module.
 */

#ifndef vxTest_70_algoImageProcessing_h
#define vxTest_70_algoImageProcessing_h


// includes
#include "vxTest.h"


// class definition
class vxTest_70_algoImageProcessing : public vxTest
{
// functions
public:

  /// default constructor
  vxTest_70_algoImageProcessing() {};

  /// destructor
  virtual ~vxTest_70_algoImageProcessing() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTest_70_algoImageProcessing"; }

  /// sets the unit test
  virtual bool UnitTest(std::ostream & stream);

  /// get the performance test
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTest_70_algoImageProcessing(const vxTest_70_algoImageProcessing & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTest_70_algoImageProcessing & operator=(const vxTest_70_algoImageProcessing & other); // should only be public if really implemented!

}; // vxTest_70_algoImageProcessing


#endif // vxTest_70_algoImageProcessing_h


// $Log: vxTest_70_algoImageProcessing.h,v $
// Revision 1.1  2005/05/12 13:58:50  frank
// added more unit and performance tests
//
// Revision 1.2  2003/07/29 20:05:54  dongqing
// add performance test definition
//
// Revision 1.1  2003/07/29 19:55:18  dongqing
// initializ the vxTestVQsegmentation
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxTest_70_algoImageProcessing.h,v 1.1 2005/05/12 13:58:50 frank Exp $
// $Id: vxTest_70_algoImageProcessing.h,v 1.1 2005/05/12 13:58:50 frank Exp $
