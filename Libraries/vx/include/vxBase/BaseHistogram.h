// $Id: BaseHistogram.h,v 1.3 2007/03/22 01:09:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
// 
// Author : Michael (michael@viatronix.com), George (geconomos@viatronix.com)


/** 
 *  Description: Non-templatized base class for Histogram<T>. BaseHistogram is responsible for handling histogram of AnyVolume. 
 *  Created: July 13, 2001
 */

#ifndef BaseHistogram_h
#define BaseHistogram_h




// forward declarations
class BaseHistogram;
template <class VoxelType> class vxBlockVolume;


// class definition
class VX_BASE_DLL BaseHistogram 
{
// functions
public:

	/// default constructor
  BaseHistogram();

	/// destructor
  ~BaseHistogram() {};

  /// returns the count for the specified bin
  uint4 GetCountAt(const uint2 & binIndex) const;

  /// returns the Max count of any one bin
  uint4 GetMaxCount() const;
	
  /// returns the number of bins of the histogram
  const uint2 GetNumOfBins() const;

  /// set the number of bins of the histogram
  void SetNumOfBins(const uint2 uNumOfBins);

  /// teturns the width of each bin of the histogram
  const float4 GetBinWidth() const { return m_fBinWidth; }

  /// set the width of bins of the histogram
  void SetBinWidth(const float fBinWidth) { m_fBinWidth = fBinWidth; }
	
  /// returns the max value of the volume
  const float4 GetMaxValue() const { return m_fMaxValue; }

  /// set the max value of the volume
  void SetMaxValue(const float4 fMaxValue) { m_fMaxValue = fMaxValue; }

  /// returns the min value of the volume
  const float4 GetMinValue() const { return m_fMinValue; }

  /// set the minimum value of the volume
  void SetMinValue(const float4 fMinValue) { m_fMinValue = fMinValue; }

  /// returns the highest value of the nonzero bins
  const float4 GetNonZeroBinHigh() const { return m_fNonZeroBinHigh; }

  /// sets the highest value of the nonzero bins
  void SetNonZeroBinHigh(const float4 & fValue) {  m_fNonZeroBinHigh = fValue; }

  /// returns the lowest value of the nonzero bins
  const float4 GetNonZeroBinLow() const { return m_fNonZeroBinLow; }

  /// sets the highest value of the nonzero bins
  void SetNonZeroBinLow(const float4 & fValue) { m_fNonZeroBinLow = fValue; }

  /// get the bins
  std::vector<uint4> & GetBinsVector() { return m_vBins; }
	
// data
public:

  /// the vector of histogram bins.
  std::vector<uint4> m_vBins;

  /// the width (or interval) of each bin.
  float4 m_fBinWidth;

	/// min value
  float4 m_fMinValue;

  /// max value
  float4 m_fMaxValue;

  // low nonzero bins
  float4 m_fNonZeroBinLow;

  // high nonzero bins
  float4 m_fNonZeroBinHigh;
}; // BaseHistogram


#endif // BaseHistogram_h


// $Log: BaseHistogram.h,v $
// Revision 1.3  2007/03/22 01:09:41  geconomos
// removed dead code
//
// Revision 1.2  2004/03/04 20:57:26  geconomos
// No longer exporting std::vector from dll.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.15  2003/05/16 13:08:19  frank
// code formatting
//
// Revision 3.14  2003/05/13 21:10:57  dongqing
// code review
//
// Revision 3.13  2003/05/13 19:09:07  michael
// code review
//
// Revision 3.12  2003/02/13 16:33:02  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 3.10.2.1  2003/02/12 22:44:20  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 3.11  2003/02/11 16:04:11  michael
// removed member number of bins that cached the vector size
//
// Revision 3.10  2002/11/12 21:29:21  geconomos
// Made vxContour  a first class citizen of vxEnvironment ( frank really did it ).
//
// Revision 3.9  2002/09/19 19:25:33  geconomos
// Fixed problem with vc6 build
//
// Revision 3.8  2002/09/19 17:56:52  geconomos
// Exported contained STL instantiations.
//
// Revision 3.7  2002/07/22 23:17:20  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 3.6  2002/07/22 18:24:09  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 3.5  2002/07/08 13:23:46  geconomos
// exported class
//
// Revision 3.4.2.1  2002/07/11 14:02:33  jaddonisio
// New functions to get low and high non-zero bins.
//
// Revision 3.4  2002/05/09 20:54:11  jenny
// added bunch of get methods
//
// Revision 3.3  2002/03/08 17:15:17  michael
// further removed include dependencies
//
// Revision 3.2  2002/02/20 01:55:01  michael
// spacing/formatting
//
// Revision 3.1  2001/12/20 14:59:25  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:14  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:29:50   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:06   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:24   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:28  soeren
// renamed utils directory
//
// Revision 1.2  2001/07/17 19:46:28  liwei
// Added exception handling.
//
// Revision 1.1  2001/07/13 23:26:47  liwei
// Added BaseHistogram, A non-templatized class for histogram.
// Now the templatized classes Histogram<T> are derived from BaseHistogram.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/BaseHistogram.h,v 1.3 2007/03/22 01:09:41 geconomos Exp $
// $Id: BaseHistogram.h,v 1.3 2007/03/22 01:09:41 geconomos Exp $
