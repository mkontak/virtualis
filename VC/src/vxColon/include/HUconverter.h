// $Id: HUconverter.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
// Copyright(c) 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author:      Ingmar Bitter
//
// Complete History at bottom of file.

/**
 * Class to convert to and from Hounsfield units 8 bit and 16 bit.
 * Hounsfield is spelled so in honor of 1979 Nobel laureate Godfrey Hounsfield.
 */

#ifndef HUconverter_h
#define HUconverter_h

// Includes
#include "Global.h"
#include "ZeroOne.h"


// This class is deprecated.  The HU class should be used in the future.
class HUconverter 
{

// Member Functions
public:

  /// Convert to HU.
  inline static int2 Uchar2HU(const uint1 uIn);

  /// Convert to HU.
  inline static int2 Short2HU(const int2 iIn);
  
  /// Convert to uint1.
  inline static uint1 HU2uchar(const int2 iInHU);

  /// Convert to uint1.
  inline static uint1 Short2uchar(const int2 iIn);
  
  /// Convert to int2.
  inline static int2 HU2short(const int2 iInHU);

  /// Convert to int2.
  inline static int2 Uchar2short(const uint1 uIn);

protected:
private:

// Data Members
protected:
private:
public:

}; // class HUconverter


// This class is overloaded so the same call will produce the desired conversion when called from within templates.
class HU 
{

// Member Functions
public:

  /// Convert to HU.
  inline static int2 ConvertToHU  (const uint1 uIn);

  /// Convert to HU.
  inline static int2 ConvertToHU  (const int2 iIn);
  
  /// Convert to HU.
  inline static int2 ConvertToHU  (const ZeroOne inZeroOne);
  
  /// Convert to uint1.
  inline static void ConvertFromHU(const int2 iInHU, int1 & result);
  
  /// Convert to int2.
  inline static void ConvertFromHU(const int2 iInHU, int2 & result);
  
  /// Convert to ZeroOne.
  inline static void ConvertFromHU(const int2 iInHU, ZeroOne & result);
  
  /// Convert to ZeroOne; additional helper returning result
  inline static ZeroOne ConvertFromHU(const int2 iInHU);
  
  /// Convert to int2.
  inline static void Convert      (const uint1 uIn, int2 & result);  
  
  /// Convert to uint1.
  inline static void Convert      (const int2 iIn, int1 & result);

  /// Convert to ZeroOne.
  inline static void Convert      (const uint1 uIn, ZeroOne & result);

  /// Convert to ZeroOne.
  inline static void Convert      (const int2 iIn, ZeroOne & result);

  /// Convert from ZeroOne to int2
  inline static void Convert      (const ZeroOne & zo, int2 & result);
 
  /// Convert from ZeroOne to int4
  inline static void Convert      (const ZeroOne & zo, int4 & result);

protected:
private:
  
// Data Members
protected:
private:
public:
  
}; // class HU
  

#include "HUconverter.inl"


#endif // HUconverter_h


// $Log: HUconverter.h,v $
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.7  2002/10/07 18:58:25  michael
// removed redundant comment
//
// Revision 3.6  2002/10/01 13:48:10  michael
// Added another helper function for conversion that returns the result rather
// than taking it as input reference.
//
// Revision 3.5  2002/04/21 22:57:40  michael
// Changed HU converter to return int2 instead of requiring it to be a second parameter
//
// Revision 3.4  2002/03/27 19:00:38  ingmar
// added conversions from ZeroOne to int2 and int4
//
// Revision 3.3  2002/03/11 17:39:19  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.2  2002/03/08 17:15:18  michael
// further removed include dependencies
//
// Revision 3.1.2.1  2002/02/21 20:18:59  frank
// Coding standards compliance.
//
// Revision 3.1  2001/12/21 17:49:16  ingmar
// added conversion to and from ZeroOne
//
// Revision 3.0  2001/10/14 23:02:27  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 24 2001 14:42:26   frank
// Spelling
// 
//    Rev 2.0   Sep 16 2001 23:49:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:40   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:25  soeren
// renamed datastruct directory
//
// Revision 1.11  2001/08/19 22:29:08  ingmar
// added class HU
//
// Revision 1.10  2001/05/18 20:37:01  ingmar
// comments
//
// Revision 1.9  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.8  2001/04/12 14:38:13  dmitry
// Sec_6_dataStruct base class added
//
// Revision 1.7  2001/03/29 17:22:46  jmeade
// (const type & var) instead of (type var) where appropriate;
// unsigned/signed corrections
//
// Revision 1.6  2001/03/29 16:18:22  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.5  2001/01/03 19:31:35  frank
// Spelling.
//
// Revision 1.4  2000/10/31 14:15:07  frank
// Merged in new segment structure
//
// Revision 1.2.6.1  2000/10/28 19:31:06  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.3  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.2.6.1  2000/10/28 19:31:06  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.2  2000/10/14 18:53:49  kevin
// Made these inline
//
// Revision 1.1  2000/09/28 14:50:57  jeff
// Hundsfeld unit converter
//
// $Id: HUconverter.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
