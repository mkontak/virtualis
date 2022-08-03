// $Id: Global.h,v 1.8 2006/06/19 19:28:43 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter (ingmar@vaitronix.com)


// pragmas
#pragma once
// The VC++ debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#pragma warning (disable : 4786)
#pragma warning (disable : 4251)
#pragma warning (disable : 565)


// includes
// Global.h should contain includes only when absolutely necessary
// New includes must be given serious consideration
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <strstream>
#include <limits>
#include "Typedef.h"
#include "Exception.h"


// standard library versions of Max, Min, etc are preferred over these
template <class T> inline       T  Abs (const T& a) { return(a >= T(0)) ? a : -a;}
template <class T> inline const T& Max (const T& a, const T& b) { return(a>b) ? a : b;}
template <class T> inline const T& Min (const T& a, const T& b) { return(a<b) ? a : b;}
template <class T> inline const T& Max (const T& a, const T& b, const T& c) { return(a>b) ? Max(a,c) : Max(b,c);}
template <class T> inline const T& Min (const T& a, const T& b, const T& c) { return(a<b) ? Min(a,c) : Min(b,c);}
template <class T> inline const T& Mid (const T& a, const T& b, const T& c) { return(a<b) ? ((b<c)?b:Max(a,c)) : ((b>c)?b:Min(a,c));}
template <class T> inline const T& Bound(const T&min, const T& val, const T& max) { return(min<val) ? ((val<max)?val:max) : min;} // assumes min<max  //desired: min<=val<=max
template <class T> inline       T  Sign (const T& a) { return (a < T(0)) ? T(-1) : (a > T(0)) ? T(1) : T(0);}

// should used STL version
template <class T> inline void Swap (T& a, T& b) { T tmp=a; a=b; b=tmp; }

/**
 * Function object for deleting "newed" pointers in containers (See EffectiveSTL Item 7).
 * Should really use Boost::shared_ptr.
 */
struct ReleaseObject
{
  template<typename T>
  void operator()(const T* ptr) const
  {
    if (ptr != 0)
    {
      delete ptr;
    }
  } // operator()
}; // struct DeleteObject


/**
 * Convert the string str to type T and assign.
 * @param string
 * @param type
 */
template <class T>
void ToType(const std::string & str, T & type)
{
  // should check typeid and push ends for strings
  std::istringstream is(str.c_str());
  is >> type;
} // ToType()


/**
 * Interpolate so that the return value (fY2) fulfills the proportion (fX1:fX2:fX3),(fY1:fY2:fY3).
 * @param fX1   the low value of the example proportion.
 * @param fX2   the goal value of the example proportion.
 * @param fX3   the high value of the example proportion.
 * @param fY1   the low value of the desired proportion.
 * @param fY3   the high value of the desired proportion.
 * @return      the scaled value in the desired proportion.
 */
inline float8 Interpolate(const float8 & fX1, const float8 & fX2, const float8 & fX3,
                          const float8 & fY1, const float8 & fY3)
{
  return fY1 + (fX2-fX1) / (fX3-fX1) * (fY3-fY1);
} // Interpolate()


/**
 * Interpolate so that the return value (fY2) fulfills the proportion (fX1:fX2:fX3),(fY1:fY2:fY3).
 * @param fX1   the low value of the example proportion.
 * @param fX2   the goal value of the example proportion.
 * @param fX3   the high value of the example proportion.
 * @param fY1   the low value of the desired proportion.
 * @param fY3   the high value of the desired proportion.
 * @return      the scaled value in the desired proportion.
 */
inline float4 Interpolate(const float4 & fX1, const float4 & fX2, const float4 & fX3,
                          const float4 & fY1, const float4 & fY3)
{
  return fY1 + (fX2-fX1) / (fX3-fX1) * (fY3-fY1);
} // Interpolate()


/**
 * Generate a random value between min and max
 * @param minimum random value allowed
 * @param maximum random value allowed
 * @return random number generated
 */
inline float8 Drand48(const float8 & fMin, const float8 & fMax)
{
  // Note: uses rand(), a very poor random number generator unlike drand but WIN has no drand
  static const float8 fOneOverMax(1.0 / float8(RAND_MAX));

  return (fMax-fMin) * float8(rand()) * fOneOverMax + fMin;
} // Drand48()


inline int4 GetNextPowerOfTwo(uint4 iValue)
{
  uint4 iPowerOfTwo(1);

  for (int i(0); i<32; ++i)
  {
    if (iPowerOfTwo >= iValue)
    {
      return iPowerOfTwo;
    }
    iPowerOfTwo = iPowerOfTwo << 1;
  }

  return -1;
} // GetNextPowerOfTwo()


/**
 * Operator<< for std::vector stream 
 * @param os     output stream
 * @param vctr   output vector
 * @return       output stream after vector output
 */
template <class T> inline std::ostream & operator<<(std::ostream & os, 
                                                    const std::vector<T> & vctr)
{
  os << "std::vector["<< vctr.size() <<"]{ ";
  if (vctr.size() > 0)
  {
    for (uint4 i(0); i < vctr.size()-1; i++)
    {
      os << vctr[i]<<", ";
    }
    os << vctr[vctr.size()-1];
  } // list not empty?
  os <<" }";

  return os;
} // operator<<()


/**
 * Operator>> for std::vector stream 
 * @param input stream
 * @param input vector
 * @return input stream after vector input
 */
template <class T> inline std::istream & operator>>(std::istream & is, 
                                                    std::vector<T> & vctr)
{
  size_t iVectSize = 0;
  is.ignore(13, '['); // will read "std::vector[" or " std::list["
  is >> iVectSize;
  vctr.resize(iVectSize);
  
  // strip off "]{ "
  char oneCharacter;
  is.read(&oneCharacter,  1);

  if (oneCharacter != ']')
  {
#ifndef FILE_REVISION
  #define FILE_REVISION "$Revision: 1.8 $"
  throw ex::InvalidDataException(LogRec("vector did not have ']' token after size", "Global.h", "operator>> for std::vector"));
  #undef FILE_REVISION
#else
    throw ex::InvalidDataException(LogRec("vector did not have ']' token after size", "Global.h", "operator>> for std::vector"));
#endif

  }

  is.ignore(2, ' '); // advance to start of vector data

  std::strstream ss;
  do 
  {
    is >> oneCharacter;
    if (isalnum(oneCharacter))
      ss << oneCharacter;
    else
      ss << ' ';
  } while(oneCharacter != '}');

  // read vector data
  for (size_t i=0; i < iVectSize; ++i)
  {
    ss >> vctr[i];
    continue;
    // skip ", " or " }" at end 
    char firstCharacter;
    char secondCharacter;
    is.read(&firstCharacter,  1);
    is.read(&secondCharacter, 1);

    if (!(firstCharacter == ',' && secondCharacter == ' ') && !(firstCharacter == ' ' && secondCharacter == '}'))
    {
#ifndef FILE_REVISION
  #define FILE_REVISION "$Revision: 1.8 $"
   throw ex::InvalidDataException(LogRec("vector did not have ', ' or ' }' token after vector data point", "Global.h", "operator>> for std::vector"));
  #undef FILE_REVISION
#else
   throw ex::InvalidDataException(LogRec("vector did not have ', ' or ' }' token after vector data point", "Global.h", "operator>> for std::vector"));
#endif
    }
  } // for all elements
  if (iVectSize == 0)
  {
    is.ignore(2);
  }

  return is;
} // operator>>()


/**
 * Operator<< for std::list stream 
 * @param os      output stream
 * @param lList   output list
 * @return        output stream after list output
 */
template <class T> inline std::ostream & operator<<(std::ostream & os, const std::list<T> & lList)
{
  os << "std::list["<< lList.size() <<"]{ ";
  if (lList.size() > 0)
  {
    for (std::list<T>::const_iterator listIt(lList.begin()); listIt != lList.end(); listIt++)
    {
      os << *listIt <<", ";
    }

    // remove the last ", " separator
    os.seekp(-2, std::ios::cur);
  } // list not empty?
  os <<" }";

  return os;
} // operator<<()


/**
 * Operator>> for std::list
 * @param input stream
 * @param input list
 * @return input stream after list input
 */
template <class T> inline std::istream & operator>>(std::istream & is, std::list<T> & lList)
{
  size_t iListSize = 0;
  is.ignore(13, '['); // will read "std::vector[" or " std::list["
  is >> iListSize;
    
  // strip off "]{ "
  char oneCharacter;  is.read(&oneCharacter,  1);
  if (oneCharacter != ']')
  {
#ifndef FILE_REVISION
  #define FILE_REVISION "$Revision: 1.8 $"
    throw ex::InvalidDataException(LogRec("list did not have ']' token after size", "Global.h", "operator>> for std::list"));
  #undef FILE_REVISION
#else
    throw ex::InvalidDataException(LogRec("list did not have ']' token after size", "Global.h", "operator>> for std::list"));
#endif
  }
  is.ignore(2); // advance to start of vector data
  
  // read list data
  for (size_t i=0; i < iListSize; ++i)
  {
    T listElement;
    is >> listElement;
    lList.push_back(listElement); 
    // skip ", " or " }" at end 
    char firstCharacter;
    char secondCharacter;
    is.read(&firstCharacter,  1);
    is.read(&secondCharacter, 1);

    if (!(firstCharacter == ',' && secondCharacter == ' ') && !(firstCharacter == ' ' && secondCharacter == '}'))
    {
#ifndef FILE_REVISION
  #define FILE_REVISION "$Revision: 1.8 $"
      throw ex::InvalidDataException(LogRec("list did not have ', ' or ' }' token after vector data point", "Global.h", "operator>> for std::list"));
  #undef FILE_REVISION
#else
      throw ex::InvalidDataException(LogRec("list did not have ', ' or ' }' token after vector data point", "Global.h", "operator>> for std::list"));
#endif
    }
  } // for all elements
  if (iListSize == 0)
  {
    is.ignore(2);
  }

  return is;
} // operator>>()


/**
 * Operator<< for std::pair
 * @param output stream
 * @param output pair
 * @return output stream after pair output
 */
template <class T, class U> inline std::ostream & operator<<(std::ostream & os, const std::pair<T,U> & couple)
{
  os << "std::pair(" << couple.first << "," << couple.second << ")";

  return os;
} // operator<<()


/**
 * Operator>> for std::pair
 * @param is       input stream
 * @param couple   input pair
 * @return         input stream after pair input
 */
template <class T, class U> inline std::istream & operator>>(std::istream & is, std::pair<T,U> & couple)
{
  is.ignore(strlen("std::pair("), '(');
  is >> couple.first;
  is.ignore(1, ',');
  is >> couple.second;
  is.ignore(1, ')');

  return is;
} // operator>>()


// constants
#ifndef M_E
#define M_E        2.7182818284590452354   /* e */
#endif
#ifndef M_LOG2E
#define M_LOG2E    1.4426950408889634074   /* log_2 e */
#endif
#ifndef M_LOG10E
#define M_LOG10E   0.43429448190325182765  /* log_10 e */
#endif
#ifndef M_LN2
#define M_LN2      0.69314718055994530942  /* log_e 2 */
#endif
#ifndef M_LN10
#define M_LN10     2.30258509299404568402  /* log_e 10 */
#endif
#ifndef M_PI
#define M_PI       3.14159265358979323846  /* pi */
#endif
#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923  /* pi/2 */
#endif
#ifndef M_PI_4
#define M_PI_4     0.78539816339744830962  /* pi/4 */
#endif
#ifndef M_1_PI
#define M_1_PI     0.31830988618379067154  /* 1/pi */
#endif
#ifndef M_2_PI
#define M_2_PI     0.63661977236758134308  /* 2/pi */
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390  /* 2/sqrt(pi) */
#endif
#ifndef M_SQRT2
#define M_SQRT2    1.41421356237309504880  /* sqrt(2) */
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2  0.70710678118654752440  /* 1/sqrt(2) */
#endif


// $Log: Global.h,v $
// Revision 1.8  2006/06/19 19:28:43  jmeade
// errors reading data with commas (likely from european float (with comma) data)
//
// Revision 1.7  2006/03/29 19:39:09  geconomos
// added  NextPowerofTwo()
//
// Revision 1.6  2006/01/31 14:01:30  frank
// code review
//
// Revision 1.5  2006/01/18 20:35:35  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.4  2005/08/05 13:03:31  geconomos
// added workaround for "FILE_REVISION" not found compliation error on operator<< for std::lists
//
// Revision 1.3  2005/07/01 12:29:26  vxconfig
// Added new Logger.
//
// Revision 1.2  2005/04/14 12:57:58  geconomos
// temporary fix for FILE_REVISION compilation error
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.11  2003/05/13 15:15:55  michael
// code review
//
// Revision 3.10  2003/01/27 17:12:50  jmeade
// Fixed incorrect return type on pair input stream operator.
//
// Revision 3.9  2002/12/24 03:39:49  jmeade
// vector, list, pair stream i/o operators.
//
// Revision 3.8  2002/09/30 20:47:24  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 3.7  2002/04/26 15:57:34  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.5  2002/04/01 21:23:19  jmeade
// Add IsBitSet() inline function; corrected vxLogRecord lines.
//
// Revision 3.4  2002/03/20 14:50:08  geconomos
// Renamed DeleteObject() to ReleaseObject()
//
// Revision 3.3  2002/03/14 16:05:57  geconomos
// Added DeleteObject function object.
//
// Revision 3.2  2002/02/20 01:46:44  michael
// removed include of vxStringTable
//
// Revision 3.1.2.3  2002/04/08 18:20:27  jmeade
// Returned "old" style LogRec instances (defining FILE_REVISION in a globally-included file will likely cause headaches).
//
// Revision 3.1.2.2  2002/04/02 17:20:11  jmeade
// IsBitSet() - let compiler sort out the two types.
//
// Revision 3.1.2.1  2002/04/01 21:18:02  jmeade
// Add IsBitSet() inline function; corrected vxLogRecord lines.
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:12  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
// 
//    Rev 2.2   Oct 01 2001 16:10:12   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.1   Sep 19 2001 14:28:40   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:45:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:32:44   ingmar
// Initial revision.
// Revision 1.3  2001/09/03 01:26:34  ingmar
// StringTable -> vxStringTable
//
// Revision 1.2  2001/09/02 02:57:48  soeren
// moved StringTable and VxStrings to 01_logger and disabled StringTable because of dependencies
// to PnGImage
//
// Revision 1.1  2001/09/02 01:21:55  soeren
// moved typedef.h and global.h to 00_global
//
// Revision 1.1  2001/09/01 00:08:29  soeren
// renamed utils directory
//
// Revision 1.68  2001/08/31 18:53:38  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.67  2001/08/22 23:23:56  soeren
// made some changes to make it to compile with the intel compiler
//
// Revision 1.66  2001/08/01 22:45:03  ingmar
// moved istream >> std::list<T> to Global.h
//
// Revision 1.65  2001/07/03 14:56:49  soeren
// removed stdafx.c and .h
//
// Revision 1.64  2001/06/01 17:28:46  ana
// PrintProgress function replaced by ProgressPrinter class
//
// Revision 1.63  2001/05/31 01:41:38  lihong
// changed iLastPercent as a static int for fast execution
//
// Revision 1.62  2001/05/30 22:41:49  dave
// new vector read function
//
// Revision 1.61  2001/05/27 22:03:13  dmitry
// fixed the <strstream> include
//
// Revision 1.60  2001/05/20 23:49:02  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.59  2001/05/19 00:33:03  jmeade
// Keeping all exceptions localized in Exception.h
//
// Revision 1.58  2001/05/18 14:57:11  dave
// streams use extern
//
// Revision 1.57  2001/05/18 05:41:53  ingmar
// towards globally unique streams
//
// Revision 1.56  2001/05/17 23:08:34  dave
// new attempt to declare global stream vars
//
// Revision 1.55  2001/05/02 19:37:57  ingmar
// Global.h
//
// Revision 1.54  2001/05/02 18:30:17  dave
// stringTable back to extern instead of static
//
// Revision 1.53  2001/05/02 18:06:18  dave
// global vars static, OutputStream classes renamed, destructors called correctly
//
// Revision 1.52  2001/04/30 02:20:04  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.51.2.1  2001/05/01 23:09:28  jmeade
// added a missing std:: (April 26 - jmeade)
//
// Revision 1.51  2001/04/16 14:12:49  geconomos
// Inital version of string table.
//
// Revision 1.50  2001/04/13 00:15:21  jmeade
// added FileExtensions.h (viatronix file extensions)
//
// Revision 1.49  2001/04/10 21:31:36  dave
// updated stream comments so that everyone has to recompile ALL
//
// Revision 1.48  2001/04/10 13:13:14  frank
// Clarified Bound function arguments.
// Added random number functions.
//
// Revision 1.47  2001/04/09 22:24:11  dave
// added include of Exception.h
//
// Revision 1.46  2001/04/08 22:12:12  dave
// add std:: where needed
//
// Revision 1.45  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.44  2001/04/05 03:41:19  ingmar
// added documentation classes
//
// Revision 1.43  2001/04/04 20:24:01  jmeade
// coding conventions
//
// Revision 1.42  2001/04/03 05:47:24  soeren
// added to faked classes
//
// Revision 1.41  2001/03/26 17:20:02  frank
// Moved serialization functions to separate namespace.
//
// Revision 1.40  2001/03/25 18:29:10  dave
// corrected bug in ToAscii
//
// Revision 1.39  2001/03/25 16:35:33  dave
// added classes for send to socket, console
//
// Revision 1.38  2001/03/23 14:59:58  dave
// added Typedef.h
//
// Revision 1.37  2001/03/21 17:43:11  dave
// added OutputStream class & instances in Global
//
// Revision 1.36  2001/02/12 17:11:38  jeff
// viatronix exceptions
//
// Revision 1.35  2001/02/08 22:21:30  lihong
// added vctr.size() for output in AsOstream
//
// Revision 1.34  2001/02/01 23:55:25  lihong
// fixed a bug for operator >>
//
// Revision 1.33  2001/01/26 04:22:00  dave
// tryin' understand skel stage
//
// Revision 1.32  2001/01/25 16:48:42  dave
// added some File.C stuff for Linux
//
// Revision 1.31  2001/01/23 19:49:03  ingmar
// improved  istream >> vector
//
// Revision 1.30  2001/01/23 13:20:52  frank
// Moved functionality to File.C
//
// Revision 1.29  2001/01/22 22:27:45  dave
// attemted to compile in UNIX
//
// Revision 1.28  2001/01/20 03:43:17  ingmar
// added const to operator << (vector) function
//
// Revision 1.27  2001/01/18 21:30:08  dave
// added PersonName class
//
// Revision 1.26  2001/01/16 13:36:26  frank
// Added os-specific temp path generation.
//
// Revision 1.25  2001/01/15 23:26:26  dave
// added v2kversion to workspace
//
// Revision 1.24  2000/12/19 20:38:33  dave
// added some stuff
//
// Revision 1.23  2000/12/19 19:27:28  frank
// fix for C++
//
// Revision 1.22  2000/12/19 17:01:24  sarang
// fix for gcc/linux
//
// Revision 1.21  2000/12/14 14:51:00  frank
// Simplified the interface to serialization/deserialization
// Added additinal error checking
//
// Revision 1.20  2000/12/12 15:20:39  frank
// Fixed string serialization
//
// Revision 1.19  2000/12/11 19:55:07  dave
// new libpng, exception, file versions etc
//
// Revision 1.18  2000/12/09 19:06:30  frank
// fixed pointer math
//
// Revision 1.17  2000/12/09 18:17:43  frank
// Added two functions to aid deserialization
//
// Revision 1.16  2000/12/09 17:48:38  frank
// Added two functions to aid serialization
//
// Revision 1.15  2000/12/08 17:14:39  frank
// Add two new functions
//
// Revision 1.14  2000/11/07 04:09:38  dave
// added header vars for v1k
//
// Revision 1.13  2000/11/03 12:59:15  frank
// off-by-one error
//
// Revision 1.12  2000/11/02 15:38:20  jeff
// Fixed references to cout in the GetPath... functions
//
// Revision 1.11  2000/11/02 12:19:55  frank
// Added two new functions for manipulating file paths
//
// Revision 1.10  2000/11/02 04:17:47  dave
// misc updates
//
// Revision 1.9  2000/10/21 06:06:03  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.8.2.2  2000/10/21 01:56:03  ingmar
// cosmetics
//
// Revision 1.8.2.1  2000/10/18 20:48:50  binli
// added Majority computation -Ingmar
//
// Revision 1.8  2000/10/11 15:46:42  dave
// no message
//
// Revision 1.7  2000/10/03 20:07:08  ingmar
// changes due to CS-lab compile
//
// Revision 1.6  2000/09/27 18:50:25  frank
// Revised LogN function and renamed
//
// Revision 1.5  2000/09/27 18:42:52  frank
// New ANSI compliant for loop
//
// Revision 1.4  2000/09/27 14:36:00  jeff
// Added LogN function (generalized version of log or log10)
//
// Revision 1.3  2000/09/17 02:49:24  dave
// removed SpecificHeader*, as now replaced by reneric map
//
// Revision 1.2  2000/09/17 02:17:28  dave
// removed SpecificHeader*, as now replaced by reneric map
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// 13    8/28/00 2:11p Lihong
// add pragma warning disable: 4786, 4251
//
// 12    8/21/00 12:33a Sarang
// modified header
//
// *****************  Version 11  *****************
// User: Jay          Date: 8/16/00    Time: 6:54p
// Updated in $/v2k/src/Utils
// Tri works
//
// *****************  Version 10  *****************
// User: Jeff         Date: 8/12/00    Time: 7:58p
// Updated in $/ViatronMDI/src/Utils
// Generalizing the Swap() function for the case that T does not have a
// default constructor
//
// *****************  Version 9  *****************
// User: Antonio      Date: 8/11/00    Time: 10:23a
// Updated in $/vx/src/Utils
// updated header in file
//
// *****************  Version 8  *****************
// User: Michael      Date: 8/04/00    Time: 11:47a
// Updated in $/v2k/src/Utils
// made history variable static
//
// *****************  Version 7  *****************
// User: Antonio      Date: 8/02/00    Time: 10:18p
// Updated in $/vx/src/Utils
// added header and complete history to end of file
//
// *****************  Version 6  *****************
// User: Ingmar          Date:  8/01/00  Time: 10:07a
// Checked in $/v2k/src/Utils
//
// *****************  Version 5  *****************
// User: Ingmar          Date:  8/01/00  Time:  9:38a
// Checked in $/v2k/src/Utils
// Comment:
//   minimized progress print overhead
//
// *****************  Version 4  *****************
// User: Michael         Date:  7/31/00  Time:  9:19p
// Checked in $/v2k/src/Utils
// Comment:
//   small > to >= fix
//
// *****************  Version 3  *****************
// User: Michael         Date:  7/31/00  Time:  3:07p
// Checked in $/v2k/src/Utils
// Comment:
//   aded explanation
//
// *****************  Version 2  *****************
// User: Ingmar          Date:  7/30/00  Time:  4:17p
// Checked in $/v2k/src/Utils
// Comment:
//   added progess template
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Global.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Global.h,v 1.8 2006/06/19 19:28:43 jmeade Exp $
// $Id: Global.h,v 1.8 2006/06/19 19:28:43 jmeade Exp $
