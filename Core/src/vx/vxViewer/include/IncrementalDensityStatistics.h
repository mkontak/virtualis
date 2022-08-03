// $Id: IncrementalDensityStatistics.h,v 1.2 2004/06/22 16:27:27 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)

/*
 Class to calculate statistical properties of a set of intensities efficiently.
 With this class, there is no need to store all the data in order to calculate
 statistics such as standard deviation.  You just have to stream the data
 through the Compute functions, then call calculate any time you want the data.
*/

#ifndef IncrementalDensityStatistics_h
#define IncrementalDensityStatistics_h



// class definition
class VX_VIEWER_DLL IncrementalDensityStatistics
{
// functions
public:

  /// constructor
  IncrementalDensityStatistics();

  /// destructor
  virtual ~IncrementalDensityStatistics() {}
      
  /// copy Constructor
  IncrementalDensityStatistics(const IncrementalDensityStatistics & rhs);

  /// assignment Operator
  IncrementalDensityStatistics & operator =(const IncrementalDensityStatistics & rhs);

  /// reset all the statistics values
  void Clear();

  /// compute the statistics using HU value of density
  void ComputeStatisticsIncremental(const uint2 iHU);

  /// compute the statistics using the raw density value
  void ComputeStatisticsIncremental(const float4 fDensity);

  /// calculate the statistics
  const void Calculate();

  /// return the average value
  const float4 & GetAverageValue() const { return m_fAverageValue; }

  /// get the minimum value
  const float4 & GetMinimumValue() const { return m_fMinimumValue; }

  /// get the maximum value stored
  const float4 & GetMaximumValue() const { return m_fMaximumValue; }

  /// get the standard deviation
  const float4 & GetStandardDeviation() const { return m_fStandardDeviation; }

  /// get the numebr of voxels considered
  const uint4 GetNumSamples() const { return m_uNumSamples; }

// data
private:
  
  // minimum value
  float4 m_fMinimumValue;
  
  // maximum value
  float4 m_fMaximumValue;
  
  // average value
  float4 m_fAverageValue;

  // standard deviation
  float4 m_fStandardDeviation;

  // square of values
  float4 m_fSquareValue; 

  // sum of values
  float4 m_fSumOfValue;

  // number of sample voxels
  uint4 m_uNumSamples;
}; // IncrementalDensityStatistics


#endif // IncrementalDensityStatistics_h


// Revision History:
// $Log: IncrementalDensityStatistics.h,v $
// Revision 1.2  2004/06/22 16:27:27  frank
// exported the class
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2003/07/11 19:49:14  michael
// final code reviews
//
// Revision 1.12  2003/06/18 15:19:49  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.11  2003/05/12 15:27:16  frank
// code review
//
// Revision 1.10  2003/04/29 21:55:37  michael
// some initial code reviews
//
// Revision 1.9  2002/06/03 14:27:55  manju
// Moved implementation to .C file
//
// Revision 1.8  2002/05/29 19:27:17  manju
// Coding standard.
//
// Revision 1.7  2002/05/07 18:19:08  frank
// Removed dead code.
//
// Revision 1.6  2002/05/03 18:29:55  manju
// Put a lower bound for number of samples in Calculate method.
//
// Revision 1.5  2002/05/03 13:35:23  manju
// Calculating the statistics only if the number of voxels considered is > 0.
//
// Revision 1.4  2002/05/01 20:24:34  manju
// Added method to clear the statistics object.
// Changed names of some variables as per the coding standards.
//
// Revision 1.3  2002/04/29 18:35:21  manju
// Added method to return number of samples.
//
// Revision 1.2  2002/04/18 14:28:03  manju
// Changed m_uMaximumValue to m_fMaximumValue.
//
// Revision 1.1  2002/03/21 18:38:20  manju
// Moved DensityStatistics and IncrementalDensityStatistics classes to
// 50_dataStruct.
//
// Revision 1.9  2001/12/18 18:46:39  ingmar
// added class Sec_70_algImgProc to structure the doxygen documentation
//
// Revision 1.8  2001/11/30 16:52:57  manju
// Added comments and cahnged the default max value.
//
// Revision 1.7  2001/11/23 18:21:32  manju
// Made numSamples unit4.
//
// Revision 1.6  2001/11/22 00:10:28  manju
// Added another function to compute with density values.
//
// Revision 1.5  2001/11/21 00:02:55  manju
// Modified calculations.
//
// Revision 1.4  2001/11/16 18:07:33  manju
// Using density in HU now for all calculations.
//
// Revision 1.3  2001/11/01 17:55:06  manju
// changed density to uint2.
//
// Revision 1.2  2001/11/01 14:50:12  manju
// Corercted standard deviation calculation.
//
// Revision 1.1  2001/10/31 22:40:24  manju
// Class to incrementally compute density statistics.
//
// $Id: IncrementalDensityStatistics.h,v 1.2 2004/06/22 16:27:27 frank Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/IncrementalDensityStatistics.h,v 1.2 2004/06/22 16:27:27 frank Exp $
