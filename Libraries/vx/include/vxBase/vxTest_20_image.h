// $Id: vxTest_20_image.h,v 1.1 2005/05/12 13:57:44 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


#ifndef vxTest_20_image_h
#define vxTest_20_image_h


// includes
#include "vxTest.h"


///
/// This class implements the test interface for the image module.
///
class vxTest_20_image : public vxTest
{
// member functions
public:

  /// default constructor
  vxTest_20_image() {};

  /// destructor
  virtual ~vxTest_20_image() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTest_20_image"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTest_20_image(const vxTest_20_image & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTest_20_image & operator=(const vxTest_20_image & other); // should only be public if really implemented!

}; // class vxTest_20_image


#endif // vxTest_20_image_h


// $Log: vxTest_20_image.h,v $
// Revision 1.1  2005/05/12 13:57:44  frank
// added more unit and performance tests
//
// Revision 1.2  2003/05/20 14:01:56  frank
// Moved V3DFile test to test environment
//
// Revision 1.1  2003/05/02 18:28:39  michael
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTest_20_image.h,v 1.1 2005/05/12 13:57:44 frank Exp $
// $Id: vxTest_20_image.h,v 1.1 2005/05/12 13:57:44 frank Exp $
