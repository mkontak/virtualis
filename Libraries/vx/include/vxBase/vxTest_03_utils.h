// $Id: vxTest_03_utils.h,v 1.1 2005/05/12 13:57:44 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/*
  This class implements the test interface for the logger classes.
*/

#ifndef vxTest_03_utils_h
#define vxTest_03_utils_h


// includes
#include "vxTest.h"


// class definition
class vxTest_03_utils : public vxTest
{
// functions
public:

  /// default constructor
  vxTest_03_utils() {};

  /// destructor
  virtual ~vxTest_03_utils() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTest_03_utils"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTest_03_utils(const vxTest_03_utils & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTest_03_utils & operator=(const vxTest_03_utils & other); // should only be public if really implemented!
}; // vxTest_03_utils


#endif // vxTest_03_utils_h


// $Log: vxTest_03_utils.h,v $
// Revision 1.1  2005/05/12 13:57:44  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/02 18:28:39  michael
// initial version
//
// Revision 1.1  2003/05/02 15:59:53  michael
// added initial version for testing
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTest_03_utils.h,v 1.1 2005/05/12 13:57:44 frank Exp $
// $Id: vxTest_03_utils.h,v 1.1 2005/05/12 13:57:44 frank Exp $
