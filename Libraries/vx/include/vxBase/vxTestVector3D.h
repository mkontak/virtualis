// $Id: vxTestVector3D.h,v 1.1 2005/05/10 20:03:15 frank Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: dongqing@viatronix.com

#ifndef vxTestVector3D_h
#define vxTestVector3D_h

// includes
#include "vxTest_10_math.h"


/**
 * Optimized Block Volume Neighbor Iterator Test.
 */
class vxTestVector3D : public vxTest_10_math
{
// Member Functions
public:
  /// default constructor
  vxTestVector3D() {};

  /// destructor
  virtual ~vxTestVector3D() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestVector3D"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// get smallest unit of mouse wheel movement; corresponds to one 'click' of the mouse wheel
  virtual bool PerformanceTest(std::ostream & stream);

private:
  ////////////////////////////////////////////////////////////////////////
  /// Demo

  /// demonstrate basic class functionality
  bool Demo(std::ostream & stream);


  ////////////////////////////////////////////////////////////////////////
  /// Validation

  /// Validate functionality
  bool ValidateConstructors(std::ostream & stream);
  /// Validate functionality
  bool ValidateAssignment(std::ostream & stream);
  /// Validate functionality
  bool ValidateIndexAccess(std::ostream & stream);
  /// Validate functionality
  bool ValidateComparators(std::ostream & stream);
  /// Validate functionality
  bool ValidateGetMaxComponent(std::ostream & stream);
  /// Validate functionality
  bool ValidateGetMaxAbsComponent(std::ostream & stream);
  /// Validate functionality
  bool ValidateGetMaxComponentCoord(std::ostream & stream);
  /// Validate functionality
  bool ValidateGetMaxAbsComponentCoord(std::ostream & stream);
  /// Validate functionality
  bool ValidateGetScaledDividedUnIso(std::ostream & stream);
  /// Validate functionality
  bool ValidateDotAndCrossProducts(std::ostream & stream);
  /// Validate functionality
  bool ValidateDistances(std::ostream & stream);
  /// Validate functionality
  bool ValidateUnaryPlusMinus(std::ostream & stream);
  /// Validate functionality
  bool ValidateScalarMultDiv(std::ostream & stream);
  /// Validate functionality
  bool ValidateVectorPlusMinus(std::ostream & stream);
  /// Validate functionality
  bool ValidateClamp(std::ostream & stream);
  /// Validate functionality
  bool ValidateNormalize(std::ostream & stream);
  /// Validate functionality
  bool ValidatePermute(std::ostream & stream);
  /// Validate functionality
  bool ValidateScaleDivideAnIso(std::ostream & stream);
  /// Validate functionality
  bool ValidateNonConstOperators(std::ostream & stream);
  /// Validate functionality
  bool ValidateStreamIO(std::ostream & stream);


  ////////////////////////////////////////////////////////////////////////
  /// Performance test

  /// Test performance
  bool TestSpeedGetMaxAbsComponent(std::ostream & stream);
  /// Test performance
  bool TestSpeedGetMaxAbsComponentCoord(std::ostream & stream);

private:
  ////////////////////////////////////////////////////////////////////////
  /// private helper functions

}; // vxTestVector3D


#endif // Vector3D_Test_h

// $Log: vxTestVector3D.h,v $
// Revision 1.1  2005/05/10 20:03:15  frank
// revived from vx
//
// Revision 1.1  2003/05/20 14:53:51  dongqing
// initialization
//
// Revision 1.2  2003/05/16 11:48:48  michael
// code review
//
// Revision 1.1  2003/01/22 21:10:50  ingmar
// initial version
//
// Revision 3.3  2001/01/02 19:38:18  ingmar
// better documentation structure
//
// Revision 3.2  2001/01/02 18:52:08  ingmar
// better documentation structure
//
// Revision 3.1  2001/01/02 16:18:08  ingmar
// initial version
//
// Revision 1.3  2002/12/23 17:18:14  ingmar
// added performance tests
//
// Revision 1.2  2002/12/23 16:53:15  ingmar
// added Validation tests
//
// Revision 1.1  2002/12/21 00:37:05  ingmar
// initial version
//
// $Id: vxTestVector3D.h,v 1.1 2005/05/10 20:03:15 frank Exp $
