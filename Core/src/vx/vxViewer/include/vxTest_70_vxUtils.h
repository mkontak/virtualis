// $Id: vxTest_70_vxUtils.h,v 1.1 2005/05/12 13:58:50 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/*
  This class implements the test interface for the vx utilitiy classes.
*/

#ifndef vxTest_70_vxUtils_h
#define vxTest_70_vxUtils_h


// includes
#include "vxTest.h"


// class definition
class vxTest_70_vxUtils : public vxTest
{
// functions
public:

  /// default constructor
  vxTest_70_vxUtils() {};

  /// destructor
  virtual ~vxTest_70_vxUtils() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTest_70_vxUtils"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTest_70_vxUtils(const vxTest_70_vxUtils & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTest_70_vxUtils & operator=(const vxTest_70_vxUtils & other); // should only be public if really implemented!
}; // vxTest_70_vxUtils


#endif // vxTest_70_vxUtils_h


// Revision History:
// $Log: vxTest_70_vxUtils.h,v $
// Revision 1.1  2005/05/12 13:58:50  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/06 14:54:16  michael
// added test class(es) for 70_vxUtils and vxIntensityStatistic
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxTest_70_vxUtils.h,v 1.1 2005/05/12 13:58:50 frank Exp $
// $Id: vxTest_70_vxUtils.h,v 1.1 2005/05/12 13:58:50 frank Exp $
