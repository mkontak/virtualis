// $Id: Typedef.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (ingmar@viatronix.com)

/*
  Declarations of platform independent basic types.
*/

#ifndef Typedef_h
#define Typedef_h


// includes
#include <fstream>


/************************************************************************
 * Define the Viatronix datatypes depending on the processor architecture
 ************************************************************************/
#if defined(_WIN32)

// Microsoft VC++ types, NOTE __int64 is not the same size as type long as of VC6.0
typedef char  int1; // 1 byte signed int ~ char
typedef short int2; // 2 byte signed int ~ short
typedef int int4; // 4 byte signed int ~ int
typedef __int64 int8; // 8 byte signed int ~ long

typedef unsigned char  uint1; // 1 byte unsigned int ~ char
typedef unsigned short uint2; // 2 byte unsigned int ~ short
typedef unsigned int uint4; // 4 byte unsigned int ~ int
typedef unsigned __int64 uint8; // 8 byte unsigned int ~ long

typedef float  float4; // 4 fyte floating point ~ float
typedef double float8; // 8 fyte floating point ~ double

#elif defined(_linux)

typedef char  int1; // 1 byte signed int ~ char
typedef short int2; // 2 byte signed int ~ short
typedef int   int4; // 4 byte signed int ~ int
typedef long  int8; // 8 byte signed int ~ long

typedef unsigned char  uint1; // 1 byte unsigned int ~ char
typedef unsigned short uint2; // 2 byte unsigned int ~ short
typedef unsigned int   uint4; // 4 byte unsigned int ~ int
typedef unsigned long  uint8; // 8 byte unsigned int ~ long

typedef float  float4; // 4 fyte floating point ~ float
typedef double float8; // 8 fyte floating point ~ double

#elif defined(_SunGCC)

typedef char  int1; // 1 byte signed int ~ char
typedef short int2; // 2 byte signed int ~ short
typedef int   int4; // 4 byte signed int ~ int
typedef long  int8; // 8 byte signed int ~ long

typedef unsigned char  uint1; // 1 byte unsigned int ~ char
typedef unsigned short uint2; // 2 byte unsigned int ~ short
typedef unsigned int   uint4; // 4 byte unsigned int ~ int
typedef unsigned long  uint8; // 8 byte unsigned int ~ long

typedef float  float4; // 4 fyte floating point ~ float
typedef double float8; // 8 fyte floating point ~ double

#endif // _WIN32


////////////////////////////////////////////////////////
// Mathematical definitions
////////////////////////////////////////////////////////

// forward declarations
template <class Type> class Triple;
template <class Type> class Point3D;
template <class Type> class Vector3D;
template <class Type> class Normal3D;
template <class Type> class Vector4d;


// typedefs
typedef Triple  < int4 > Triplei;
typedef Triple  <float4> Triplef;
typedef Point3D < int4 > Point3Di;
typedef Point3D <float4> Point3Df;
typedef Vector3D< int4 > Vector3Di;
typedef Vector3D<float4> Vector3Df;
typedef Normal3D<float4> Normal3Df;


/************************************************************************
 * Redefine the streams due to __int64 and long on Windows
 ************************************************************************/
#if defined(_WIN32)

#if _MSC_VER < 1300


/** 
 * Operator<< for outstream
 * @param ostream
 * @param uint8
 * @return outstream
 */
inline std::ostream & operator<<(std::ostream & os, const uint8 & ui) 
{
  os << static_cast<unsigned long>(ui);  
  return os;
} // operator<<()


/** 
 * Operator<< for outstream
 * @param ostream
 * @param int8
 * @return outstream
 */
inline std::ostream & operator<<(std::ostream & os, const int8 & ui) 
{
  os << static_cast<long>(ui);  
  return os;
} // operator<<()


#endif // _MSC_VER


/** 
 * Operator>> for instream
 * @param intream
 * @param int8
 * @return instream
 */
inline std::ifstream & operator>>(std::ifstream & inStream, uint2 & iVariable)
{
  unsigned short uTemp;
  inStream >> uTemp;
  iVariable = uTemp;
  return inStream;
} // operator>>()

#endif // _WIN32


// enums
enum { SYSTEM_RETURN_SUCCESS = 0 };


#endif // Typedef_h


// $Log: Typedef.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/05/13 15:15:55  michael
// code review
//
// Revision 3.4  2003/01/24 21:45:53  ingmar
// aded missing typedef for Triplef
//
// Revision 3.3  2003/01/24 18:17:55  frank
// Moved common typedefs to Typedef.h
//
// Revision 3.2  2002/03/08 13:55:45  michael
// comments on #endif
//
// Revision 3.1  2002/02/25 23:12:14  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 3.0  2001/10/14 23:02:12  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Sep 30 2001 18:27:50   kevin
// Added VIatronix datatype typedefs for SUN
// 
//    Rev 2.1   Sep 19 2001 14:28:40   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:45:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:32:44   ingmar
// Initial revision.
// Revision 1.1  2001/09/02 01:21:55  soeren
// moved typedef.h and global.h to 00_global
//
// Revision 1.1  2001/09/01 00:08:31  soeren
// renamed utils directory
//
// Revision 1.10  2001/08/03 00:42:36  soeren
// added system success
//
// Revision 1.9  2001/04/10 18:17:49  dmitry
// Removed standard unsigned typedefs
//
// Revision 1.8  2001/04/09 16:25:34  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.7  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.6  2001/04/05 17:59:25  dmitry
// added operator for int8
//
// Revision 1.5  2001/03/27 19:10:04  dmitry
// Changed typedefs to standard type
//
// Revision 1.4  2001/03/19 18:05:26  frank
// ifstream >> uint2     missing from implementation, added.
//
// Revision 1.3  2001/03/19 02:12:56  dave
// Updated typedefs to lowercase, changed header
//
// Revision 1.2  2001/03/16 01:01:20  ingmar
// added I1 I2 I4 I8 U1 U2 U4 U8 F4 F8
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// 
// 4     8/17/00 1:37p Antonio
// modified header.
// 
// *****************  Version 3  *****************
// User: Antonio      Date: 8/11/00    Time: 10:05a
// Updated in $/vx/src/Utils
// modified header to newer version
// 
// *****************  Version 2  *****************
// User: Antonio      Date: 8/02/00    Time: 10:32p
// Updated in $/vx/src/Utils
// added header and complete history to end of file  
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Typedef.h
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Typedef.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Typedef.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
