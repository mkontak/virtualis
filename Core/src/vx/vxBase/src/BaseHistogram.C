// $Id: BaseHistogram.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
// 
// Author : Michael (michael@viatronix.com), George (geconomos@viatronix.com)


// includes
#include "stdafx.h"
#include "BaseHistogram.h"


/**
 * Default constructor
 */
BaseHistogram::BaseHistogram()
:m_fMinValue(0.0F), 
m_fMaxValue(0.0F), 
m_fNonZeroBinLow(0.0F), 
m_fNonZeroBinHigh(0.0F)
//*******************************************************************
{
}


/**
 * Returns the value of the Data represented at the specified index
 * @param the index into the histogram vector
 * @return count at that index
 */
uint4 BaseHistogram::GetCountAt(const uint2 & uBinIndex) const
//*******************************************************************
{
  if (uBinIndex >= GetNumOfBins())
  {
    return(0);
  }
  else
  {
    return(m_vBins[uBinIndex]);
  }
} // GetCountAt()


/**
 * Returns the number of bins of the Histogram
 * @return number of bins
 */
const uint2 BaseHistogram::GetNumOfBins() const
//*******************************************************************
{
  return static_cast< uint2 >( m_vBins.size() );
}


/**
 * Set the number of bins of the histogram
 * @param number of bins
 */
void BaseHistogram::SetNumOfBins(const uint2 uNumOfBins)
//*******************************************************************
{
  m_vBins.resize(uNumOfBins);
}


/**
 * Returns the count of the bin with the highest count
 * @return maximum count
 */
uint4 BaseHistogram::GetMaxCount() const
//*******************************************************************
{
  return *(std::max_element(m_vBins.begin(), m_vBins.end()));
}


// $Log: BaseHistogram.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.11  2003/05/16 13:08:19  frank
// code formatting
//
// Revision 3.10  2003/05/15 11:52:35  michael
// code review
//
// Revision 3.9  2003/05/13 19:09:07  michael
// code review
//
// Revision 3.8  2003/02/13 16:33:02  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 3.6.2.1  2003/02/12 22:44:20  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 3.7  2003/02/11 16:04:11  michael
// removed member number of bins that cached the vector size
//
// Revision 3.6  2002/11/12 22:38:10  geconomos
// Moved consturctor to .C file.
//
// Revision 3.5  2002/07/22 23:17:20  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 3.4  2002/07/22 18:24:09  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 3.3  2002/07/08 14:33:06  dmitry
// fixed compilation errors.
//
// Revision 3.2  2002/07/08 13:23:35  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/BaseHistogram.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: BaseHistogram.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
