// $Id: vxTestIntensityStatistics.h,v 1.1 2005/05/12 14:13:52 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

#ifndef vxTestIntensityStatistics_h
#define vxTestIntensityStatistics_h


// includes
#include "vxTest_70_vxUtils.h"


// class definition
class vxTestIntensityStatistics : public vxTest_70_vxUtils
{
// functions
public:

  /// default constructor
  vxTestIntensityStatistics() {};

  /// destructor
  virtual ~vxTestIntensityStatistics() {};

  /// get name of test
  virtual std::string GetName() const { return "vxTestIntensityStatistics"; }

  /// sets the modal flag
  virtual bool UnitTest(std::ostream & stream);

  /// 
  virtual bool PerformanceTest(std::ostream & stream);

  /// always returns the same volume dimension to be used by all performance tests
  static Triplei GetTestVolumeDimension();

private:

  /// copy constructor
  vxTestIntensityStatistics(const vxTestIntensityStatistics & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxTestIntensityStatistics & operator=(const vxTestIntensityStatistics & other); // should only be public if really implemented!

  /// validate statistics
  bool ValidateStatistics(std::ostream & stream);

}; // class vxTestIntensityStatistics


#endif // vxTestIntensityStatistics_h


// $Log: vxTestIntensityStatistics.h,v $
// Revision 1.1  2005/05/12 14:13:52  frank
// added more unit and performance tests
//
// Revision 1.2  2003/05/08 13:13:11  frank
// code review
//
// Revision 1.1  2003/05/08 12:16:08  frank
// Statistic -> Statistics
//
// Revision 1.1  2003/05/06 14:54:16  michael
// added test class(es) for 70_vxUtils and vxIntensityStatistic
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxTestIntensityStatistics.h,v 1.1 2005/05/12 14:13:52 frank Exp $
// $Id: vxTestIntensityStatistics.h,v 1.1 2005/05/12 14:13:52 frank Exp $
