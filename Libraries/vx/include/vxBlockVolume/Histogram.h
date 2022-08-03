// $Id: Histogram.h,v 1.3 2006/02/03 15:22:11 frank Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/Histogram.h,v 1.3 2006/02/03 15:22:11 frank Exp $
//
// Copyright 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created:
// Author: Jay
//
// Complete history at bottom of file.

/**
 * Template class for Histograms. <BR>
 * It is defined in terms of a template parameter VolIter
 * which represents a VxVolIterator or any Iterator that
 * supports operator++ , End() and the copy contructor.
 * This iterator is used to analyse and bin the data.
 */

#ifndef Histogram_h
#define Histogram_h



// pragma declarations
#pragma warning (disable : 4800)



// include declarations
#include "BaseHistogram.h"




// forward declarations
template <class T> class Triple;



// class definition
template <class T>
class Histogram : public BaseHistogram
{
// public member functions
public:

  /// CONSTRUCTORS

  /**
   * Constructor a Histogram given a volume iterator.  <BR>
   * VC++ 6 requires you to put such constructors to be defined in the
   * header file where it is declared inside the class.  <BR>
   * gcc is obviously a better compiler and it doesn't require that.
   *
   * @param T             The type of the data to make the histogram from.
   * @param Iter          The type of the iterator the traverse the data.
   * @param iter          The iterator to the traverse the data.
   * @param maxBuckets    The maximum number of buckets
   */
  template <class Iter>
  Histogram(const Iter & iter)
  {
    CreateHistogram(iter, 4096, false, false);
  } // constructor

  /**
   * Constructor a Histogram given a volume iterator <BR>
   * and given a target histogram width.  <BR>
   * VC++ 6 requires you to put such constructors to be defined in the
   * header file where it is declared inside the class.  <BR>
   * gcc is obviously a better compiler and it doesn't require that.
   * 
   * @param Iter       The type of the iterator the traverse the data.
   * @param Iter       The type of the iterator the traverse the data.
   * @param iter       The iterator to the traverse the data.
   * @param iter       The iterator to the traverse the data.
   * @param maxBuckets The maximum number of buckets
   * @param maxBuckets The maximum number of buckets
   * @param forceNumMaxBins
   *                   True  guarantees a histogramsize of exactly maxBuckets,<BR>
   *                   False allows also for smaller histograms.
   * @param forceNumMaxBins
   *                   True  guarantees a histogramsize of exactly maxBuckets,<BR>
   *                   False allows also for smaller histograms.
   * @param forceIntergerBinsZeroToMax
   *                   needed to ensure that transfer function editor background histogram 
   *                   has zero count areas at the extremas.
   *                   
   * @param forceIntergerBinsStartingAtZero
   */
  template <class Iter>
  Histogram(const Iter & iter, uint2 uMaxBins, bool bForceNumMaxBins, bool bForceIntergerBinsZeroToMax=false)
  {
    CreateHistogram(iter, uMaxBins, bForceNumMaxBins, bForceIntergerBinsZeroToMax);
  } // constructor

  /**
   * Actually create the histogram by traversing the iterator
   * and putting the values into bins.
   */
  template <class Iter>
  void CreateHistogram(const Iter & iter, uint2 uMaxBins, bool bForceNumMaxBins, bool bForceIntergerBinsZeroToMax)
  {
    if ( bForceIntergerBinsZeroToMax )
    {
      m_valMin     = T(0);
      m_valMinNext = T(1);
      m_valMax     = T(uMaxBins);    // compiler warning C4800 forcing value to bool 'true' or 'false' (performance warning)
      m_valMaxPrev = T(m_valMax-1);  // fix through partial template specialization
                                     // will be supported in VC 7.1
      m_vBins.resize(uMaxBins);
      m_fBinWidth = 1.0;
    }
    else
    {
      /// find min & max
      // consider that the data could be all zeros or just 0 and 1
      m_valMin = m_valMinNext = m_valMax = m_valMaxPrev = iter.GetVoxel();
      for ( Iter it(iter); !it.End(); ++it )
      {
        const T vox(it.GetVoxelRaw());
        if ( m_valMin > vox )
        {
          m_valMinNext = m_valMin;
          m_valMin = vox;
        } // find min
        if ( m_valMax < vox )
        {
          m_valMaxPrev = m_valMax;
          m_valMax = vox;
        } // find max
        if ( (m_valMinNext > vox && vox > m_valMin) || (m_valMinNext == m_valMin) )
        {
          m_valMinNext = vox;
        } // find next smallest
        if ( (m_valMaxPrev < vox && vox < m_valMax) || (m_valMaxPrev == m_valMax) )
        {
          m_valMaxPrev = vox;
        } // find next largest
      } // for all voxels

      m_vBins.resize(GetBestNumberOfBins(uMaxBins, bForceNumMaxBins));
    }

    /// now put all values in their bins
    for ( Iter it(iter); !it.End(); ++it )
    {
      ++m_vBins[GetBin(it.GetVoxelRaw())];
    } // for all voxels

    m_fMaxValue = m_valMax;
    m_fMinValue = m_valMin;
  } // CreateHistogram()


  /// Copy Constructor.
  Histogram(const Histogram<T> & other);

  /// Overloaded Assignment Operator.
  inline Histogram<T> & operator =(const Histogram<T> & other);

  /// DATA VALUE INFO
  /// Returns the index of the bin for the specified data value.
  inline uint2  GetBin(const T & dataValue) const;

  /// Returns the maximum value of the data in the same type.
  inline const T & GetMaxDataValue() const;

  /// Returns the minimum value of the data in the same type.
  inline const T & GetMinDataValue() const;

  /// Returns the value of the data at binIndex
  inline const T GetDataValueAt(const uint2 & binIndex) const;


  /// BIN INFO

  /// Returns the number of bins of the Histogram.
  inline const uint2 GetNumOfBins() const;

  /// Returns the width of each bin of the Histogram.
  inline const float GetBinWidth() const;


  /// PRINTING

  /// make a histogram image.
  //inline void MakeHistogramImage(PNGImage & image) const;

  /// make a histogram image with given color ramp
  //inline void MakeHistogramImage(PNGImage & image, const std::vector< Triple<uint1> > & colorRamp) const;

  /// Append all histogram information to an ostream.
  std::ostream & AsOstream(std::ostream & os) const;

  /// VERIFICATION

  /// Test function for the Histogram Class.
  static bool Test(std::ostream & os = std::cout);

  /// Overloaded << Operator.
  inline friend std::ostream & operator<<(std::ostream & os, const Histogram<T> & histogram);

private:
  /// find the best number of bins considering the data distribution and desired maximum number of bins
  uint2 GetBestNumberOfBins(uint2 uMaxBins, const bool bForceNumMaxBins);

protected: // member viariables

  uint2 m_numBins;

// private data members
private:

  /// the smallest and second smallest data values.
  T m_valMin, m_valMinNext;

  /// the largest and second largest data values.
  T m_valMax, m_valMaxPrev;
}; // template class Histogram



#include "Histogram.inl"



#endif // Histogram_h



// $Log: Histogram.h,v $
// Revision 1.3  2006/02/03 15:22:11  frank
// removed png library
//
// Revision 1.2  2005/04/27 17:36:30  geconomos
// added inline specifier to operator <<
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/02/13 16:33:02  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.4.2.1  2003/02/12 22:45:10  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.5  2003/02/11 21:06:14  dongqing
// add protected uint2 m_numBins to resolve conflicts against the new BaseHistogram
//
// Revision 1.4  2003/01/02 16:46:24  ingmar
// added missing include
//
// Revision 1.3  2002/03/12 20:34:49  michael
// added needed include to PNGimage (instead of forward declaration)
//
// Revision 1.2  2002/03/12 20:02:52  michael
// fixed wrong include
//
// Revision 1.1  2002/03/08 22:34:22  michael
// added Histogram to 20_image to avoid forward dependencies
//
// Revision 1.2  2002/03/08 17:15:18  michael
// further removed include dependencies
//
// Revision 1.1  2002/02/20 01:39:28  michael
// moved histogram from 03_utils to 20_image since PNGimage is needed
//
// Revision 3.4  2002/01/09 15:44:45  soeren
// disabled performance warning
//
// Revision 3.3  2001/12/20 15:00:15  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.2  2001/11/12 23:55:28  ingmar
// towards coding standards compliance
//
// Revision 3.1  2001/11/09 00:19:35  ingmar
// made volume histogram appear at full 4096 density range in transfer function editor
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 01 2001 16:10:14   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.1   Sep 19 2001 14:29:58   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:20   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:36   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:30  soeren
// renamed utils directory
//
// Revision 1.26  2001/07/27 23:54:11  liwei
// Added a couple of lines for debugging.
//
// Revision 1.25  2001/07/13 23:26:47  liwei
// Added BaseHistogram, A non-templatized class for histogram.
// Now the templatized classes Histogram<T> are derived from BaseHistogram.
//
// Revision 1.24  2001/04/12 18:51:38  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.23  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.22  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.21  2001/04/09 22:24:29  dave
// added exceptions instead of Abort
//
// Revision 1.20  2001/04/09 16:25:34  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.19  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.18  2001/03/30 18:47:22  dmitry
// Updated datatypes.
//
// Revision 1.17  2000/11/14 01:21:40  sarang
// fixed 2 ANSI C++ bugs (thanks to gcc)
// 1. #include<typeinfo> missing
// 2. function defined without return type
//
// Revision 1.16  2000/11/13 18:07:43  ingmar
// much improved rewrite of the Histogram class
//
// Revision 1.15  2000/11/06 03:57:05  ingmar
// upper limit for table size
//
// Revision 1.14  2000/11/06 01:15:25  ingmar
// better constructor for float
//
// Revision 1.13  2000/11/05 22:42:54  ingmar
// added GetValueAt
//
// Revision 1.12  2000/11/02 19:58:39  frank
// Added bounds on array access
//
// Revision 1.11  2000/10/24 02:56:46  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.10  2000/09/26 01:17:48  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.9  2000/09/25 18:22:16  kevin
// Made Static Libraries
//
// Revision 1.8.2.1  2000/09/25 18:56:35  kevin
// Made Static Libraries
//
// Revision 1.8  2000/09/21 06:48:33  dave
// cosmetics
//
// Revision 1.7  2000/09/17 21:22:00  ingmar
// back to better generic Histogram of version 1.4
//
// Revision 1.6  2000/09/17 21:13:49  ingmar
// towards ignore of lowest entry
//
// Revision 1.5  2000/09/17 04:33:26  ingmar
// now possible to ignore min value
//
// Revision 1.4  2000/09/11 17:41:19  kevin
// added author
//
// Revision 1.3  2000/09/11 13:46:10  kevin
// added header file include
//
// Revision 1.2  2000/09/11 04:12:01  antonio
// got rid of a few more warnings
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
//
// 12    8/28/00 10:51a Antonio
// changed ulong to unsigned long
//
// 11    8/22/00 5:37p Jay
// Added GetMaxCount function which returns the maximum frequency of the
// data
//
// 10    8/21/00 2:30p Antonio
// added a few comments and spaces
//
// 9     8/21/00 2:43a Sarang
// modified << operator function
//
// *****************  Version 8  *****************
// User: Sarang       Date: 8/20/00    Time: 6:49p
// Updated in $/v2k/src/Utils
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
//
// *****************  Version 7  *****************
// User: Jay          Date: 8/18/00    Time: 12:23p
// Updated in $/vx/src/Utils
// Default arg added to Histogram::Test()
//
// *****************  Version 6  *****************
// User: Antonio      Date: 8/11/00    Time: 10:24a
// Updated in $/vx/src/Utils
// updated header in file
//
// *****************  Version 5  *****************
// User: Jay          Date: 8/11/00    Time: 1:38a
// Updated in $/v2k/src/Utils
//
// *****************  Version 7  *****************
// User: Jay          Date: 8/10/00    Time: 7:16p
// Updated in $/TemplateVol/src/Utils
// Added copy constructor and assignment operator
//
// *****************  Version 6  *****************
// User: Jay          Date: 8/08/00    Time: 3:03p
// Updated in $/TemplateVol/src/Utils
//
// *****************  Version 4  *****************
// User: Antonio      Date: 8/02/00    Time: 10:20p
// Updated in $/vx/src/Utils
// added header and complete history to end of file
//
// *****************  Version 3  *****************
// User: Jay             Date:  7/22/00  Time:  2:46a
// Checked in $/v2k/src/Utils
// Comment:
//   exception handling included for vector.resize ..
// Remove later
//
// *****************  Version 2  *****************
// User: Frank           Date:  7/18/00  Time:  3:56p
// Checked in $/Viatron1000/src/Utils
// Comment:
//   Removed "using namespace *" from header files to prevent ambiguities
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Histogram.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/Histogram.h,v 1.3 2006/02/03 15:22:11 frank Exp $
// $Id: Histogram.h,v 1.3 2006/02/03 15:22:11 frank Exp $
