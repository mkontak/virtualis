// $Id: vxTestValueRange.h,v 1.1 2005/05/12 13:57:32 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxTestValueRange_h
#define vxTestValueRange_h


// includes
#include "vxTest_03_utils.h"


/*
 * This class implements the test interface for the value range class
 */
class vxTestValueRange : public vxTest_03_utils
{
// member functions
public:

  /// default constructor
  vxTestValueRange() {};

  /// destructor
  virtual ~vxTestValueRange() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestValueRange"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTestValueRange(const vxTestValueRange & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestValueRange & operator=(const vxTestValueRange & other); // should only be public if really implemented!

  /// validate constructors
  bool ValidateConstructors( std::ostream & stream );

  /// validate add value
  bool ValidateAddValue( std::ostream & stream );

  /// validate get minimum
  bool ValidateGetMinimum( std::ostream & stream );

  /// validate get maximum
  bool ValidateGetMaximum( std::ostream & stream );

  /// validate get percentile
  bool ValidateGetPercentile( std::ostream & stream );

  /// validate equality operator
  bool ValidateEqualityOperator( std::ostream & stream );

}; // vxTestValueRange


#endif // vxTestValueRange_h


// $Log: vxTestValueRange.h,v $
// Revision 1.1  2005/05/12 13:57:32  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/15 15:03:26  frank
// added test for ValueRange
//
// Revision 1.1  2003/05/14 20:41:00  frank
// added Line unit test
//
// Revision 1.2  2003/05/14 19:33:24  frank
// code review
//
// Revision 1.1  2003/05/14 18:10:10  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestValueRange.h,v 1.1 2005/05/12 13:57:32 frank Exp $
// $Id: vxTestValueRange.h,v 1.1 2005/05/12 13:57:32 frank Exp $
