// $Id: IncrementalDensityStatistics.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "stdafx.h"
#include "IncrementalDensityStatistics.h"


/**
 * Constructor
 */
IncrementalDensityStatistics::IncrementalDensityStatistics()
:m_fMinimumValue(FLT_MAX),
m_fMaximumValue(-FLT_MAX),
m_fSquareValue(0.0F),
m_fSumOfValue(0.0F),
m_fAverageValue(0.0F),
m_fStandardDeviation(0.0F),
m_uNumSamples(0)
{
} // IncrementalDensityStatistics()


/**
 * Copy Constructor
 * @param rhs
 */
IncrementalDensityStatistics::IncrementalDensityStatistics(const IncrementalDensityStatistics & rhs) 
{ 
  m_fMinimumValue = rhs.GetMinimumValue();
  m_fMaximumValue = rhs.GetMaximumValue();
  m_fAverageValue = rhs.GetAverageValue();
  m_fStandardDeviation = rhs.GetStandardDeviation();
} // IncrementalDensityStatistics(rhs)


/**
 * Assignment operator
 * @param rhs
 */
IncrementalDensityStatistics & IncrementalDensityStatistics::operator =(const IncrementalDensityStatistics & rhs)
{
  m_fMinimumValue = rhs.m_fMinimumValue;
  m_fMaximumValue = rhs.m_fMaximumValue;
  m_fAverageValue = rhs.m_fAverageValue;
  m_fStandardDeviation = rhs.m_fStandardDeviation;
  return *this;
} // operator =()


/**
 * Clear the statsitics
 */
void IncrementalDensityStatistics::Clear()
{
  m_fMinimumValue = FLT_MAX;
  m_fMaximumValue = -FLT_MAX;
  m_fAverageValue = 0.0F;
  m_fStandardDeviation = 0.0F;    
  m_fSumOfValue = 0.0F;
  m_fSquareValue = 0.0F;
  m_uNumSamples = 0;
} // Clear()


/**
 * Calculate the sum of values and sum of square of values incrementally
 * @param uHU density value in Hounsfield Units
 */
void IncrementalDensityStatistics::ComputeStatisticsIncremental(const uint2 uHU)
{
  // find minimum
  if (uHU < m_fMinimumValue)
  {
    m_fMinimumValue = uHU;
  }

  // find maximum
  if (uHU > m_fMaximumValue)
  {
    m_fMaximumValue = uHU;
  }

  //incremental calculation for std. deviation.     
  m_fSquareValue = m_fSquareValue + (uHU * uHU);
  m_fSumOfValue+=uHU;
  m_uNumSamples++;

} // ComputeStatisticsIncremental(uint2)


/**
 * Calculate the sum of values and sum of square of values incrementally
 * @param fDensity raw density value
 */ 
void IncrementalDensityStatistics::ComputeStatisticsIncremental(const float4 fDensity)
{
  // find minimum
  if (fDensity < m_fMinimumValue)
  {
    m_fMinimumValue = fDensity;
  }     
   
  // find maximum
  if (fDensity > m_fMaximumValue)
  {
    m_fMaximumValue = fDensity;
  }    
   
  // incremental calculation for std. deviation.    
  m_fSquareValue = m_fSquareValue + (fDensity * fDensity);
  m_fSumOfValue+=fDensity;
  m_uNumSamples++;
       
} // ComputeStatisticsIncremental(float4)
  

/**
 * Calculate average and standard deviation from the values stored incrementally
 */
const void IncrementalDensityStatistics::Calculate()
{
  if (m_uNumSamples != 0)
  {
    m_fAverageValue = (float4)m_fSumOfValue/(m_uNumSamples);
    float8 temp(m_fSquareValue - ((float8)(m_fSumOfValue * m_fSumOfValue)/m_uNumSamples));
    if (m_uNumSamples == 1)
    {
      m_fStandardDeviation = sqrt(temp/(m_uNumSamples));
    }
    else
    {
      m_fStandardDeviation = sqrt(temp/(m_uNumSamples -1));
    }
  }    
} // Calculate()
  

// Revision History:
// $Log: IncrementalDensityStatistics.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/07/11 19:49:14  michael
// final code reviews
//
// Revision 1.5  2003/06/18 15:19:49  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.4  2003/05/20 17:17:18  dongqing
// code review
//
// Revision 1.3  2003/05/12 16:14:21  frank
// Fixed unit test
//
// Revision 1.2  2003/05/12 15:27:16  frank
// code review
//
// Revision 1.1  2002/06/03 14:26:55  manju
// Removed implementation from .h file to this .C file
//
// $Id: IncrementalDensityStatistics.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/IncrementalDensityStatistics.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
