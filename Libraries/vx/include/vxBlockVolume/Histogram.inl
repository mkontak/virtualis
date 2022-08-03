// $Id: Histogram.inl,v 1.4 2006/02/03 15:22:11 frank Exp $
//
// Copyright 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Jay & Ingmar
// Owner: michael

// define declarations
#define FILE_REVISION "$Revision: 1.4 $"



/**
 * Utility function to find the best number of bins.
 *
 * @param T             The type of the data the histogram is made for.
 */
template <class T>
uint2 Histogram<T>::GetBestNumberOfBins(uint2 uMaxBins, const bool bForceNumMaxBins)
{
  // default one-to-one mapping with jump after first and before last bin.
  uint2 uBestNumOfBins(m_valMaxPrev - m_valMinNext + 3);
  m_fBinWidth = 1.0;

  // scaled to use exactly as many bins as specified in maxBin.
  // still with jumps after the first and before the last bin.
  if ( uBestNumOfBins > uMaxBins || bForceNumMaxBins ||
       typeid(T) == typeid(float4) || typeid(T) == typeid(float8) )
  {
    m_fBinWidth = (m_valMaxPrev - m_valMinNext) / static_cast<float4>(uMaxBins-2);
    uBestNumOfBins = uMaxBins;
  }

  return uBestNumOfBins;
} // GetBestNumberOfBins


/**
 * Copy Constructor.
 *
 * @param T             The type of the data the histogram is made for.
 *
 * @param other         const Histogram<T> & which is to be copy.
 */
template <class T>
Histogram<T>::Histogram(const Histogram<T> & other)
: 
m_vBins(other.m_vBins),
m_fBinWidth(other.m_fBinWidth),
m_valMin(other.m_valMin),
m_valMinNext(other.m_valMinNext),
m_valMax(other.m_valMax),
m_valMaxPrev(other.m_valMaxPrev)
{
}

/**
 * Overloaded Assignment Operator.
 *
 * @param T             The type of the data the histogram is made for.
 *
 * @param other         const Histogram<T> & which is to be assigned.
 * @return Histogram<T> &
 */
template <class T>
Histogram<T> & Histogram<T>::operator =(const Histogram<T> & other)
{
  m_vBins = other.m_vBins;
  m_fBinWidth = other.m_fBinWidth;
  m_valMin = other.m_valMin;
  m_valMinNext = other.m_valMinNext;
  m_valMax = other.m_valMax;
  m_valMaxPrev = other.m_valMaxPrev;
  return(*this);
}


/**
 * Returns the value of the data represented at the specified bin.
 *
 * @param T             Type of the data the histogram is made for.
 * @param binIndex      the index into the histogram vector.
 *
 * @return the first value of the data interval at that index.
 */
template <class T>
const T Histogram<T>::GetDataValueAt(const uint2 & binIndex) const
{
  if ( binIndex == 0 )
  {
    return(m_valMin);
  }
  else if ( binIndex == GetNumOfBins()-1 )
  {
    return(m_valMax);
  }
  else
  {
    return((binIndex-1) * m_fBinWidth + m_valMinNext);
  }
} // GetDataValueAt


/**
 * Returns the index of the bin for the specified data value.
 *
 * @param T             Type of the data the histogram is made for.
 * @param value         the data value needed to identify the bin.
 * @return the first value of the data interval of the matching bin.
 */
template <class T>
uint2 Histogram<T>::GetBin(const T & dataValue) const
{
  if ( dataValue < m_valMinNext )
  {
    return(0);
  }
  else if ( dataValue > m_valMaxPrev )
  {
    return(GetNumOfBins()-1);
  }
  else
  {
    // extra 0.5 to avoid truncation errors
    uint2 bin = static_cast<uint2>( (dataValue - m_valMinNext) / m_fBinWidth + 1.5 );
    if ( bin >= GetNumOfBins() )
    {
      throw ex::IllegalArgumentException(LogRec("Attempted access outside array.", "Histogram<T>", "GetBin"));
    }
    return(bin);
  }
} // GetBin


/**
 * Returns the maximum value of the data in the same type.
 *
 * @param T             The type of the data the histogram is made for.
 *
 * @return const T &
 */
template <class T>
const T & Histogram<T>::GetMaxDataValue() const
{
  return(m_valMax);
} // GetMaxDataValue


/**
 * Returns the minimum value of the Data in the same type.
 *
 * @param T             The type of the data the histogram is made for.
 * @return const T &
 */
template <class T>
const T & Histogram<T>::GetMinDataValue() const
{
  return(m_valMin);
} // GetMinDataValue


////////////////////////////////////////////////////////////////////////
/// BIN INFO


/**
 * Returns the number of bins of the Histogram.
 *
 * @param T             The type of the data the histogram is made for.
 * @return const T &
 */
template <class T>
const uint2 Histogram<T>::GetNumOfBins() const
{
  return BaseHistogram::GetNumOfBins();
} // GetNumOfBins


/**
 * Returns the width of each bin of the Histogram.
 *
 * @param T             The type of the data the histogram is made for.
 * @return const T &
 */
template <class T>
const float4 Histogram<T>::GetBinWidth() const
{
  return BaseHistogram::GetBinWidth();
} // GetBinWidth


/**
 * make a histogram image.
 *
 * @param T             The type of the data the histogram is made for.
 * @param image         The already properly sized image that just needs new pixel colors.
 */
//template <class T>
//inline void Histogram<T>::MakeHistogramImage(PNGImage & image) const
//{
//  float4 maxCount=0;
//  if ( GetNumOfBins() < 20 )
//  {
//    // maximum of all entries
//    maxCount = GetMaxCount();
//  }
//  else
//  {
//    // maximum excluding first and last entry
//    for ( uint2 k=1; k < GetNumOfBins()-1; ++k )
//    {
//      if ( maxCount < GetCountAt(k) )
//      {
//        maxCount = GetCountAt(k);
//      }
//    }
//  }
//  Triple<uint1> barColor(255,100,255);
//  Triple<uint1> backgroundColor(0,0,0);
//  uint2 maxY = image.GetHeight() / 2;
//  if ( GetNumOfBins() == image.GetWidth() )
//  {
//    {
//
//      for ( uint2 x=0; x<image.GetWidth(); ++x )
//      {
//        float4 barHeight = maxY * GetCountAt(x)/maxCount;
//        for ( uint2 y=0; y<maxY; ++y )
//        {
//          if ( (maxY-y) > barHeight )
//          {
//            image.SetPixel(x,y, backgroundColor);
//          }
//          else
//          {
//            image.SetPixel(x,y, barColor);
//          }
//        }
//      }
//    }
//    barColor.m_x = 255;
//    barColor.m_y = 255;
//    barColor.m_z = 100;
//    backgroundColor.m_x = 40;
//    backgroundColor.m_y = 40;
//    backgroundColor.m_z = 40;
//    {
//      for ( uint2 x=0; x<image.GetWidth(); ++x )
//      {
//        float4 barHeight = maxY * pow( static_cast<float4>(GetCountAt(x)/maxCount), 0.3F);
//        for ( uint2 y=0; y<maxY; ++y )
//        {
//          if ( (maxY-y) > barHeight )
//          {
//            image.SetPixel(x,y+maxY, backgroundColor);
//          }
//          else
//          {
//            image.SetPixel(x,y+maxY, barColor);
//          }
//        }
//      }
//    }
//  }
//  else
//  {
//    std::stringstream tt;
//    tt<<"<Histogram<T>::MakeHistogramImage> not yet implemented for image width different from histogram bins.";
//    LogWrn(tt.str(),"Histogram<T>","MakeHistogramImage");
//    throw ex::IllegalArgumentException(LogRec("Image width different from histogram bins.","Histogram<T>","MakeHistogramImage"));
//  }
//} // MakeHistogramImage


/**
 * make a histogram image.
 *
 * @param T             The type of the data the histogram is made for.
 * @param image         The already properly sized image that just needs new pixel colors.
 */
//template <class T>
//inline void Histogram<T>::MakeHistogramImage(PNGImage & image, const std::vector< Triple<uint1> > & colorRamp) const
//{
//  float4 maxCount=0;
//  if ( GetNumOfBins() < 20 )
//  {
//    // maximum of all entries
//    maxCount = GetMaxCount();
//  }
//  else
//  {
//    // maximum excluding first and last entry
//    for ( uint2 k=1; k < GetNumOfBins()-1; ++k )
//    {
//      if ( maxCount < GetCountAt(k) )
//      {
//        maxCount = GetCountAt(k);
//      }
//    }
//  }
//  Triple<uint1> backgroundColor(0,0,0);
//  uint2 maxY = image.GetHeight() / 2;
//  if ( GetNumOfBins() == image.GetWidth() )
//  {
//    {
//
//      for ( uint2 x=0; x<image.GetWidth(); ++x )
//      {
//        float4 barHeight = maxY * GetCountAt(x)/maxCount;
//        for ( uint2 y=0; y<maxY; ++y )
//        {
//          if ( (maxY-y) > barHeight )
//          {
//            image.SetPixel(x,y, backgroundColor);
//          }
//          else
//          {
//            image.SetPixel(x,y, colorRamp[x]);
//          }
//        }
//      }
//    }
//    backgroundColor.m_x = 40;
//    backgroundColor.m_y = 40;
//    backgroundColor.m_z = 40;
//    {
//      for ( uint2 x=0; x<image.GetWidth(); ++x )
//      {
//        float4 barHeight = maxY * pow(GetCountAt(x)/maxCount, 0.3F);
//        for ( uint2 y=0; y<maxY; ++y )
//        {
//          if ( (maxY-y) > barHeight )
//          {
//            image.SetPixel(x,y+maxY, backgroundColor);
//          }
//          else
//          {
//            image.SetPixel(x,y+maxY, colorRamp[x]);
//          }
//        }
//      }
//    }
//  }
//  else
//  {
//    stringstream tt;
//    tt<<"<Histogram<T>::MakeHistogramImage> not yet implemented for image width different from histogram bins.";
//    LogWrn(tt.str(),"Histogram<T>","MakeHistogramImage");
//
//    throw ex::IllegalArgumentException(LogRec("Image width different from histogram bins.","Histogram","MakeHistogramImage"));
//  }
//} // MakeHistogramImage


/**
 * Append all histogram information to an ostream.
 *
 * @param T             The type of the data the histogram is made for.
 * @param os            The output stream to append the Histogram info to.
 * @return The modified output stream.
 */
template <class T>
std::ostream & Histogram<T>::AsOstream(std::ostream & os) const
{
  os << "Histogram{";
  uint2 k;
  os << " min="<<m_valMin;
  os << " min2="<<m_valMinNext;
  os << " max2="<<m_valMaxPrev;
  os << " max="<<m_valMax;
  os << " m_binWidth="<<m_fBinWidth;
  os << " m_numBins="<<GetNumOfBins();
  if ( GetNumOfBins() < 10 )
  {
    for ( k=0; k < GetNumOfBins(); ++k )
    {
      os <<" "<< GetCountAt(k) <<"x"<<GetDataValueAt(k)<<"@["<<k<<"],";
    }
    os << "\b }";
  }
  else
  {
    for ( k=0; k<3; ++k )
    {
      os <<" "<< GetCountAt(k) <<"x"<<GetDataValueAt(k)<<"@["<<k<<"],";
    }
    os << " ...";
    for ( k=GetNumOfBins()-4; k<GetNumOfBins(); ++k )
    {
      os <<" "<< GetCountAt(k) <<"x"<<GetDataValueAt(k)<<"@["<<k<<"],";
    }
    os << "\b }";
  }
  return(os);
} // AsOstream


////////////////////////////////////////////////////////////////////////
/// VERIFICATION



////////////////////////////////////////////////////////////////////////

/**
 * Overloaded << operator.
 * @param os ostream &
 * @param histogram template<class C>const Histogram<C> &
 * @return ostream &
 */
template <class T>
inline std::ostream & operator <<(std::ostream & os, const Histogram<T> & histogram)
{
  return(histogram.AsOstream(os));
} // operator <<

#undef FILE_REVISION

// $Log: Histogram.inl,v $
// Revision 1.4  2006/02/03 15:22:11  frank
// removed png library
//
// Revision 1.3  2005/04/14 12:58:19  geconomos
// resolved ambiguous use of pow()
//
// Revision 1.2  2004/03/04 21:00:55  geconomos
// Modifications for 7.1 compiler including: use of typename for dependant types and resolving ambiguities.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/06/05 19:34:43  mkontak
// Fixed compile issues with managed code, added namespace qualifier for std
//
// Revision 1.6  2003/02/13 17:21:14  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.5.2.1  2003/02/12 22:45:10  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.5  2003/01/02 16:46:06  ingmar
// removed throw from function interface (was in .inl only, not .h)
//
// Revision 1.4  2002/09/25 20:59:18  ingmar
// replaced pow(x,1) with x
//
// Revision 1.3  2002/03/12 20:34:49  michael
// added needed include to PNGimage (instead of forward declaration)
//
// Revision 1.2  2002/03/12 20:02:52  michael
// fixed wrong include
//
// Revision 1.1  2002/03/08 22:34:21  michael
// added Histogram to 20_image to avoid forward dependencies
//
// Revision 1.1  2002/02/20 01:39:28  michael
// moved histogram from 03_utils to 20_image since PNGimage is needed
//
// Revision 3.2  2001/11/09 00:19:35  ingmar
// made volume histogram appear at full 4096 density range in transfer function editor
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:29:58   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:20   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:36   ingmar
// Initial revision.
// Revision 1.5  2001/09/13 17:42:48  soeren
// added quotes to function name in logger macro
//
// Revision 1.4  2001/09/10 09:04:59  ingmar
// fixed use of logger defines
//
// Revision 1.3  2001/09/07 21:21:17  soeren
// added #undef FILE_VERSION in the end of each file
//
// Revision 1.2  2001/09/07 17:59:08  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:08:30  soeren
// renamed utils directory
//
// Revision 1.26  2001/08/13 20:04:53  tushar
// removed errors due to the new logger
//
// Revision 1.25  2001/08/07 17:02:11  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.24  2001/07/25 01:42:24  tushar
// no message
//
// Revision 1.23  2001/07/25 00:35:57  tushar
// fixed some compiler errors (logger)
//
// Revision 1.22  2001/07/24 22:57:25  tushar
// fixed a byg
//
// Revision 1.21  2001/07/23 19:18:20  lihong
// Defect ID: 599
// replaced abort() with throw ex
//
// Revision 1.20  2001/07/13 23:26:47  liwei
// Added BaseHistogram, A non-templatized class for histogram.
// Now the templatized classes Histogram<T> are derived from BaseHistogram.
//
// Revision 1.19  2001/04/12 18:51:53  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.18  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.17  2001/04/10 18:17:58  dmitry
// updated datatypes
//
// Revision 1.16  2001/04/09 22:24:28  dave
// added exceptions instead of Abort
//
// Revision 1.15  2001/04/09 16:25:34  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.14  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.13  2001/03/30 18:47:22  dmitry
// Updated datatypes.
//
// Revision 1.12  2001/03/27 19:10:51  dmitry
// Updated more types as def. in Typedef.h
//
// Revision 1.11  2001/02/13 20:02:32  sarang
// BUG_FIX : setting int to float removed.
//
// Revision 1.10  2000/11/22 06:43:27  dave
// Updated PNGImage class
//
// Revision 1.9  2000/11/13 18:07:43  ingmar
// much improved rewrite of the Histogram class
//
// Revision 1.8  2000/11/05 22:42:54  ingmar
// added GetValueAt
//
// Revision 1.7  2000/09/21 06:48:33  dave
// cosmetics
//
// Revision 1.6  2000/09/17 22:00:04  ingmar
// slight operator << improvement
//
// Revision 1.5  2000/09/17 21:32:09  ingmar
// improved operator <<
//
// Revision 1.4  2000/09/17 21:22:05  ingmar
// back to better generic Histogram of version 1.4
//
// Revision 1.3  2000/09/17 04:33:42  ingmar
// better operator <<
//
// Revision 1.2  2000/09/11 04:12:01  antonio
// got rid of a few more warnings
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
//
// 12    8/28/00 11:02a Antonio
// minor header additions
//
// 11    8/28/00 10:51a Antonio
// changed ulong to unsigned long
//
// 10    8/22/00 5:37p Jay
// Added GetMaxCount function which returns the maximum frequency of the
// data
//
// 9     8/21/00 3:19p Lihong
// Delimited Histogram << of Histogram with "," for proper parsing in
// excel
//
// 8     8/21/00 2:39p Antonio
// placed test function into Histogram.C
//
// 6     8/21/00 2:43a Sarang
// test function almost finished
//
// *****************  Version 5  *****************
// User: Jay          Date: 8/18/00    Time: 3:56p
// Updated in $/vx/src/Utils
// Histogram::Test now in Histogram.inl, Histogram.C removed from Project
//
// *****************  Version 4  *****************
// User: Antonio      Date: 8/11/00    Time: 10:26a
// Updated in $/vx/src/Utils
// updated header of file.
//
// *****************  Version 3  *****************
// User: Jay          Date: 8/11/00    Time: 1:38a
// Updated in $/v2k/src/Utils
//
// *****************  Version 5  *****************
// User: Jay          Date: 8/10/00    Time: 7:16p
// Updated in $/TemplateVol/src/Utils
// Added copy constructor and assignment operator
//
// *****************  Version 4  *****************
// User: Jay          Date: 8/08/00    Time: 3:03p
// Updated in $/TemplateVol/src/Utils
//
// *****************  Version 2  *****************
// User: Antonio      Date: 8/02/00    Time: 10:21p
// Updated in $/vx/src/Utils
// added header and complete history to end of file
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Histogram.inl
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/Histogram.inl,v 1.4 2006/02/03 15:22:11 frank Exp $
// $Id: Histogram.inl,v 1.4 2006/02/03 15:22:11 frank Exp $
