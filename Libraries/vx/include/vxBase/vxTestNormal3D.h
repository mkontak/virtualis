// $Id: vxTestNormal3D.h,v 1.1 2005/05/10 20:03:15 frank Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: dongqing@viatronix.com

#ifndef vxTestNormal3D_h
#define vxTestNormal3D_h

// includes
#include "vxTest_10_math.h"


/**
 * Optimized Block Volume Neighbor Iterator Test.
 */
class vxTestNormal3D : public vxTest_10_math
{
// Member Functions
public:
  /// default constructor
  vxTestNormal3D() {};

  /// destructor
  virtual ~vxTestNormal3D() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestNormal3D"; }

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
  bool ValidateGetScaledAnIso(std::ostream & stream);
  /// Validate functionality
  bool ValidateDotAndCrossProducts(std::ostream & stream);
  /// Validate functionality
  bool ValidateUnaryPlusMinus(std::ostream & stream);
  /// Validate functionality
  bool ValidateScalarMultDiv(std::ostream & stream);
  /// Validate functionality
  bool ValidatePermute(std::ostream & stream);
  /// Validate functionality
  bool ValidateNormalize(std::ostream & stream);
  /// Validate functionality
  bool ValidateStreamIO(std::ostream & stream);

  ////////////////////////////////////////////////////////////////////////
  /// Performance test

  /// Test performance
  bool TestSpeedGetMaxAbsComponent(std::ostream & stream);
  /// Test performance
  bool TestSpeedGetMaxAbsComponentCoord(std::ostream & stream);

}; // vxTestNormal3D


#endif // vxTestNormal3D_h

// $Log: vxTestNormal3D.h,v $
// Revision 1.1  2005/05/10 20:03:15  frank
// revived from vx
//
// Revision 1.1  2003/05/20 14:53:51  dongqing
// initialization
//
// initial version
//
// $Id: vxTestNormal3D.h,v 1.1 2005/05/10 20:03:15 frank Exp $
