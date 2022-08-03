// $Id: vxTestTriple.h,v 1.1 2005/05/10 20:03:15 frank Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: dongqing@viatronix.com

#ifndef vxTestTriple_h
#define vxTestTriple_h

// includes
#include "vxTest_10_math.h"


/**
 * Optimized Block Volume Neighbor Iterator Test.
 */
class vxTestTriple : public vxTest_10_math
{
// Member Functions
public:
  /// default constructor
  vxTestTriple() {};

  /// destructor
  virtual ~vxTestTriple() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestTriple"; }

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
  bool ValidateUnaryPlusMinus(std::ostream & stream);
  /// Validate functionality
  bool ValidateScalarMultDiv(std::ostream & stream);
  /// Validate functionality
  bool ValidateClamp(std::ostream & stream);
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

}; // vxTestTriple

#endif // vxTestTriple_h

// $Log: vxTestTriple.h,v $
// Revision 1.1  2005/05/10 20:03:15  frank
// revived from vx
//
// Revision 1.1  2003/05/20 15:53:04  dongqing
// initialization
//
// initial version
//
// $Id: vxTestTriple.h,v 1.1 2005/05/10 20:03:15 frank Exp $
