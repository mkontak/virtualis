// $Id: vxIntensityStatistic.C,v 1.2 2006/01/20 18:09:00 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxIntensityStatistic.h"
#include "Utilities.h"
#include <math.h>


// defines
#define FILE_REVISION "$Revision: 1.2 $"

  
/**
 * Default constructor
 * @param other statistic
 */
vxIntensityStatistic::vxIntensityStatistic()
//*******************************************************************
{
  m_iMinimum = 0;
  m_iMaximum = 0;
  m_fAverage = 0.0F;
  m_fStandardDeviation = 0.0F;
} // default constructor


/**
 * Copy constructor
 * @param other statistic
 */
vxIntensityStatistic::vxIntensityStatistic(const vxIntensityStatistic & other)
//*******************************************************************
{
  if (this != &other)
  {
    m_vDensities = other.m_vDensities;
    m_iMinimum   = other.m_iMinimum;
    m_iMaximum   = other.m_iMaximum;
    m_fAverage   = other.m_fAverage;
    m_fStandardDeviation = other.m_fStandardDeviation;
  }
} // copy constructor


/**
 * Assignment operator 
 * @param other statistic
 * @return const reference to this
 */
vxIntensityStatistic & vxIntensityStatistic::operator=(const vxIntensityStatistic & other)
//*******************************************************************
{
  if (this != &other)
  {
    m_vDensities = other.m_vDensities;
    m_iMinimum   = other.m_iMinimum;
    m_iMaximum   = other.m_iMaximum;
    m_fAverage   = other.m_fAverage;
    m_fStandardDeviation = other.m_fStandardDeviation;
  }

  return *this;
} // assignment operator


/**
 * Clear the values and statistics
 */
void vxIntensityStatistic::Clear()
//*******************************************************************
{
  m_vDensities.clear();
  m_iMinimum = 0;
  m_iMaximum = 0;
  m_fAverage = 0.0F;
  m_fStandardDeviation = 0.0F;
} // Clear()


/**
 * Add a value
 * @param new value
 */
void vxIntensityStatistic::Add(const int2 iValue)
//*******************************************************************
{
  m_vDensities.push_back(iValue);
}


/**
 * Get number of value
 * @return new value
 */
const uint4 vxIntensityStatistic::GetNumberEntries() const
//*******************************************************************
{
  return m_vDensities.size();
}
  

/**
 * Add a value
 */
void vxIntensityStatistic::Compute()
//*******************************************************************
{
  // clear values first
  m_iMinimum = 32767;
  m_iMaximum = -32768;
  m_fAverage = 0.0F;
  m_fStandardDeviation = 0.0F;
    
  if (m_vDensities.size() > 0)
  {
    // loop over all values once
    float4 fSum(0.0F);

    for (std::vector<int2>::const_iterator i=m_vDensities.begin(); i!=m_vDensities.end(); ++i)
    {
      // compute sum
      fSum += float4(*i);

      // compute min/max
      m_iMinimum = Min( * i, m_iMinimum );
      m_iMaximum = Max( * i, m_iMaximum );
    }
    
    // compute average
    m_fAverage = fSum / m_vDensities.size();
    
    // compute standard deviation
    float4 fSumDifferenceSquared(0.0F);

    for (std::vector<int2>::const_iterator i=m_vDensities.begin(); i!=m_vDensities.end(); ++i)
    {
      const float4 fA(*i - m_fAverage);
      fSumDifferenceSquared += fA * fA;
    }
    if (m_vDensities.size() > 1)
    {
      m_fStandardDeviation = sqrt(fSumDifferenceSquared / (GetNumberEntries()-1));
    }
    else
    {
      m_fStandardDeviation = 0.0F;
    }    
  } // if any data to process    
} // ComputeStatistics()
  
  
#undef FILE_REVISION


// Revision History:
// $Log: vxIntensityStatistic.C,v $
// Revision 1.2  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/01/23 16:31:51  frank
// computed and displayed Find Region with sampling for oblique & 3D views
//
// Revision 1.3  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.2  2003/05/06 14:56:45  michael
// code review
//
// Revision 1.1  2003/01/29 12:29:08  michael
// added reworked initial version of intensity statistic
//
// Revision 1.4  2002/09/25 21:23:08  ingmar
// replaced pow(x,2) with x*x
//
// Revision 1.3  2002/05/07 18:18:49  frank
// Removed dead code.
//
// Revision 1.2  2002/05/03 22:25:46  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.1  2002/03/21 18:38:20  manju
// Moved vxIntensityStatistic and IncrementalvxIntensityStatistic classes to
// 50_dataStruct.
//
// Revision 3.3  2001/12/18 18:46:39  ingmar
// added class Sec_70_algImgProc to structure the doxygen documentation
//
// Revision 3.2  2001/10/26 02:21:07  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.1  2001/10/18 18:28:32  geconomos
// removed stdafx.h
//
// Revision 3.0  2001/10/14 23:02:29  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 10 2001 20:14:44   frank
// #undef'd FILE_REVISION
// 
//    Rev 2.0   Sep 16 2001 23:49:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:12   ingmar
// Initial revision.
// Revision 1.4  2001/09/13 17:42:49  soeren
// added quotes to function name in logger macro
//
// Revision 1.3  2001/09/10 09:05:00  ingmar
// fixed use of logger defines
//
// Revision 1.2  2001/09/07 17:59:09  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/06 12:08:33  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxIntensityStatistic.C,v 1.2 2006/01/20 18:09:00 geconomos Exp $
// $Id: vxIntensityStatistic.C,v 1.2 2006/01/20 18:09:00 geconomos Exp $
