// $Id: vxTestMath.h,v 1.1 2005/05/10 20:03:15 frank Exp $
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

#ifndef vxTestMath_h
#define vxTestMath_h


// includes
#include "vxTest.h"


// class definition
class vxTestMath : public vxTest
{
// functions
public:

  /// default constructor
  vxTestMath() {};

  /// destructor
  virtual ~vxTestMath() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestMath"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTestMath(const vxTestMath & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestMath & operator=(const vxTestMath & other); // should only be public if really implemented!
}; // vxTestMath


#endif // vxTestMath_h


// $Log: vxTestMath.h,v $
// Revision 1.1  2005/05/10 20:03:15  frank
// revived from vx
//
// Revision 1.1  2003/05/02 15:59:53  michael
// added initial version for testing
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestMath.h,v 1.1 2005/05/10 20:03:15 frank Exp $
// $Id: vxTestMath.h,v 1.1 2005/05/10 20:03:15 frank Exp $
