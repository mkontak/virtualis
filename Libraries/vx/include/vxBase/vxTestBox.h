// $Id: vxTestBox.h,v 1.1 2005/05/12 13:57:32 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxTestBox_h
#define vxTestBox_h


// includes
#include "vxTest_03_utils.h"


/*
 * This class implements the test interface for the value range class
 */
class vxTestBox : public vxTest_03_utils
{
// member functions
public:

  /// default constructor
  vxTestBox() {};

  /// destructor
  virtual ~vxTestBox() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestBox"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:

  /// copy constructor
  vxTestBox(const vxTestBox & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestBox & operator=(const vxTestBox & other); // should only be public if really implemented!

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

  /// validate is inside
  bool ValidateIsInside( std::ostream & stream );

  /// validate scale
  bool ValidateScale( std::ostream & stream );

  /// validate normalize
  bool ValidateNormalize( std::ostream & stream );

  /// validate to string
  bool ValidateToString( std::ostream & stream );

  /// validate equality
  bool ValidateEquality( std::ostream & stream );

  /// validate inequality
  bool ValidateInequality( std::ostream & stream );

  /// validate inner radius
  bool ValidateInnerRadius( std::ostream & stream );

  /// validate outer radius
  bool ValidateOuterRadius( std::ostream & stream );

}; // vxTestBox


#endif // vxTestBox_h


// $Log: vxTestBox.h,v $
// Revision 1.1  2005/05/12 13:57:32  frank
// added more unit and performance tests
//
// Revision 1.2  2003/05/15 20:11:56  frank
// code review
//
// Revision 1.1  2003/05/15 16:00:36  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxTestBox.h,v 1.1 2005/05/12 13:57:32 frank Exp $
// $Id: vxTestBox.h,v 1.1 2005/05/12 13:57:32 frank Exp $
