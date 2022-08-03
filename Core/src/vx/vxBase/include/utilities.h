// $Id: utilities.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/utilities.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Wei Li (liwei@viatronix.com)

#ifndef UTILITIES_H
#define UTILITIES_H

#include "FileServerHeader.h"

// forward declarations
// VS-2013 (in math.h)
//const float4 log2(const float4 fValue);
const uint4 GetBits(uint4 uValue);


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


// global(?)
const float4 EPSILON = 0.0001f;



/**
 * Converts a string to a UNICODE (multibyte) string
 *
 * @param s   String to convert
 * @return    Converted, multibyte, string
 */
inline std::wstring ToWString( const std::string &s )
{
  int iWCLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, s.c_str(), -1, NULL, 0);
  if (iWCLen <= 0)    return std::wstring();
  wchar_t * pWCStr = new wchar_t[iWCLen];
  if (pWCStr == NULL) return std::wstring();
  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, s.c_str(), -1, pWCStr, iWCLen);
  std::wstring wsRtn(pWCStr);
  delete [] pWCStr;
  return wsRtn;
} // ToWString()


/**
 * Convert a variable to ascii (string). Attempt to overcome C++ deficiency 
 * of difficulty converting variables to ascii. (specifically int to Ascii).
 * @param variable
 * @return string
 */
template <class T>
std::string ToAscii(const T & value)
//*******************************************************************
{
  std::ostringstream os;
#pragma warning( disable : 4267 )
  os << value;
#pragma warning( default : 4267 )
  std::string res(os.str());

  return res;
} // ToAscii()


/** 
 * Clamp down
 * @param limit
 * @param value
 * @return result
 */
template<class T1, class T2>
inline T2 DnClamp(const T1 limit, T2 value) 
//*******************************************************************
{
  if (value <= limit)
  {
    return value;
  }
  return limit;
} // DnClamp()


/** 
 * Clamp up
 * @param limit
 * @param value
 * @return result
 */
template<class T1, class T2>
inline T2 UpClamp(const T1 limit, T2 value) 
//*******************************************************************
{
  if (value >= limit)
  {
    return value;
  }
  return limit;
} // UpClamp()


/**
 * Expand to power of 2. (120->128, 128->128, 129->256).
 * @param iNumber The number to expand
 * @return the nearest (larger or equal) power of two
 */
inline int4 ExpandToPowerOf2(int4 iNumber)
//*******************************************************************
{
  int4 iPowerOf2(2);

  while (iPowerOf2 < iNumber)
  {
    iPowerOf2 = iPowerOf2 << 1;
  }
  return iPowerOf2;
} // ExpandToPowerOf2()


/** 
 * Find shift offset
 * @param bit shift
 * @param offset
 * @param template min
 * @param template max
 * @param integer min
 * @param integer max
 */
template <class T>
void FindShiftOffset(int4& iBitsShift, 
                     int4& iOffset, 
                     const T sMinV, 
                     const T sMaxV, 
                     const int4 tMinV, 
                     const int4 tMaxV)
//*******************************************************************
{
  if (float4(sMinV) >= float4(sMaxV))
  {
    iBitsShift = 0;
    iOffset = 0;
    return;
  }
  float4 fScale(float(tMaxV - tMinV+1) / (float4(sMaxV) - float4(sMinV)+1));
  float fBitsShift(log2(fScale));

  iOffset = tMinV - int4(sMinV);
  fBitsShift = static_cast<int4>(fBitsShift*10) / 10.0f; 

  if (fBitsShift > 0)
    iBitsShift = floor(fBitsShift);
  else if (fBitsShift < 0)
    iBitsShift = floor(fBitsShift);
  else
    iBitsShift = 0;
} // FindShiftOffset()


/**
 * Round the input value to have a certain number of digits after the decimal
 * @param input value
 * @param desired number of digits after the decimal point (keep it reasonable :)
 */
inline float8 RoundDecimals(float8 inValue, uint4 uDigitsAfterDecimal)
//*******************************************************************
{
  float8 fOutValue(inValue);
  float8 fMultiplier = 1.0F;

  for( uint4 i( 0 ); i<uDigitsAfterDecimal; i++)
  {
    fMultiplier *= 10.0F;
  }

  return (float8)((int)(fOutValue * fMultiplier + 0.5F)) / fMultiplier;
} // RoundDecimals()


#undef FILE_REVISION 


#endif // UTILITIES_H


// $Log: utilities.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.14  2003/10/03 12:14:33  michael
// cosmetics
//
// Revision 3.13  2003/05/13 19:09:24  michael
// code review
//
// Revision 3.12  2003/05/12 16:31:48  michael
// code review
//
// Revision 3.11  2002/10/25 02:37:04  frank
// Added RoundDecimals()
//
// Revision 3.10  2002/09/30 20:47:13  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 3.9  2002/04/12 22:55:43  ingmar
// made ExpandToPowerOf2() coding standard compliant
//
// Revision 3.8  2001/12/20 14:59:48  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.7  2001/11/19 04:30:26  liwei
// A scaling problem is fixed.
//
// Revision 3.6  2001/11/01 00:49:33  liwei
// GetShiftOffset() revised.
//
// Revision 3.5  2001/10/31 20:40:38  liwei
// Revised log2 and GetShiftOffset.
//
// Revision 3.4  2001/10/30 15:22:04  liwei
// log2() revised.
//
// Revision 3.3  2001/10/26 18:14:54  liwei
// Added GetBits().
//
// Revision 3.2  2001/10/18 18:48:19  liwei
// Added constant EPSILON.
//
// Revision 3.1  2001/10/15 23:33:10  liwei
// Conversion functions moved to Conversion.h.
//
// Revision 3.0  2001/10/14 23:02:16  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
// 
//    Rev 2.2   Oct 10 2001 14:34:18   soeren
// sachin fixed convert
// 
//    Rev 2.1   Sep 19 2001 14:30:04   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:34   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:44   ingmar
// Initial revision.
// Revision 1.2  2001/09/02 23:10:09  ingmar
// code beautify
//
// Revision 1.1  2001/09/01 00:08:31  soeren
// renamed utils directory
//
// Revision 1.1  2001/07/25 19:02:24  liwei
// utilities.* moved from 3_math to 1_utils.
//
// Revision 1.4  2001/07/17 21:28:16  liwei
// A bug in data conversion fixed.
//
// Revision 1.3  2001/07/10 16:35:36  liwei
// Added function FindShiftOffset(...).
//
// Revision 1.2  2001/06/27 14:11:53  liwei
// A couple of functions by VolumeSet moved to Utilities.*
//
// Revision 1.1  2001/06/25 19:22:29  geconomos
// Moved from 9_render
//
// Revision 1.2  2001/06/13 14:40:21  liwei
// Add header
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/utilities.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: utilities.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
/*
  History:	$History: utilities.h $
 * 
 * *****************  Version 4  *****************
 * User: Liwei        Date: 6/07/01    Time: 9:37a
 * Updated in $/Vol/dataStruct
*/
