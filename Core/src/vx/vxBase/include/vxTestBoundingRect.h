// $Id: vxTestBoundingRect.h,v 1.1 2005/05/12 13:57:32 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxTestBoundingRect_h
#define vxTestBoundingRect_h


// includes
#include "vxTest_12_mathGFX.h"


/*
 * This class implements the test interface for the value range class
 */
class vxTestBoundingRect : public vxTest_12_mathGFX
{
// member functions
public:

  /// default constructor
  vxTestBoundingRect() {};

  /// destructor
  virtual ~vxTestBoundingRect() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestBoundingRect"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTestBoundingRect(const vxTestBoundingRect & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestBoundingRect & operator=(const vxTestBoundingRect & other); // should only be public if really implemented!

  /// validate constructors
  bool ValidateConstructors( std::ostream & stream );

  /// validate set
  bool ValidateSet( std::ostream & stream );

  /// validate get intersection
  bool ValidateGetIntersection( std::ostream & stream );

  /// validate get union
  bool ValidateGetUnion( std::ostream & stream );

  /// validate union
  bool ValidateUnion( std::ostream & stream );

  /// validate get min/mid/max
  bool ValidateGetMinMidMax( std::ostream & stream );

  /// validate set min/max
  bool ValidateSetMinMax( std::ostream & stream );

  /// validate get dim
  bool ValidateGetDim( std::ostream & stream );

  /// validate get corner
  bool ValidateGetCorner( std::ostream & stream );

  /// validate is empty
  bool ValidateIsEmpty( std::ostream & stream );

  /// validate is inside
  bool ValidateIsInside( std::ostream & stream );

  /// validate scale
  bool ValidateScale( std::ostream & stream );

  /// validate get area
  bool ValidateGetArea( std::ostream & stream );

  /// validate normalize
  bool ValidateNormalize( std::ostream & stream );

  /// validate to string
  bool ValidateToString( std::ostream & stream );

  /// validate equality
  bool ValidateEquality( std::ostream & stream );

  /// validate inequality
  bool ValidateInequality( std::ostream & stream );

}; // vxTestBoundingRect


#endif // vxTestBoundingRect_h


// $Log: vxTestBoundingRect.h,v $
// Revision 1.1  2005/05/12 13:57:32  frank
// added more unit and performance tests
//
// Revision 1.1  2003/05/15 16:00:36  frank
// initial version
//
// Revision 1.2  2003/05/15 15:56:43  frank
// completed tests
//
// Revision 1.1  2003/05/15 15:24:16  frank
// intial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestBoundingRect.h,v 1.1 2005/05/12 13:57:32 frank Exp $
// $Id: vxTestBoundingRect.h,v 1.1 2005/05/12 13:57:32 frank Exp $
