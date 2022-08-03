// $Id: vxMathUtils.inl,v 1.3 2006/03/29 20:03:19 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxMathUtils.inl,v 1.3 2006/03/29 20:03:19 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "Box.h"
#include "vxPlaneOblique.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"

//
///**
// * Compute the smallest enclosing power of two (7->8, 8->8, 9->16)
// * @param power of two or -1 if not existing
// */
//inline int4 GetNextPowerOfTwo(uint4 iValue)
////*******************************************************************
//{
//  uint4 iPowerOfTwo(1);
//
//  for (int i(0); i<32; ++i)
//  {
//    if (iPowerOfTwo >= iValue)
//    {
//      return iPowerOfTwo;
//    }
//    iPowerOfTwo = iPowerOfTwo << 1;
//  }
//
//  return -1;
//} // GetNextPowerOfTwo()


/**
 * Convert float to int4 using FISTP and the default float control register setting (IEEE round)
 * @param flaoting point number
 * @return integer part of given float
 */
inline int4 RoundFloatToInt_FISTP(const float4 & fNum)
//*******************************************************************
{
#ifdef _WIN64
  return static_cast< int4 >( fNum );
#else
  float4 fNumLocal(fNum);
  int4   iNumLocal;
  __asm FLD   DWORD PTR [fNumLocal]
  __asm FISTP DWORD PTR [iNumLocal] 
  return iNumLocal;
#endif
} // RoundFloatToInt_FISTP


#undef FILE_REVISION


// Revision History:
// $Log: vxMathUtils.inl,v $
// Revision 1.3  2006/03/29 20:03:19  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.2  2006/02/09 14:06:12  geconomos
// 64-bit conditionals
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/08/07 14:22:35  michael
// moved vxMathUtils from 70_vxUtils to 12_mathGFX were it belongs to
//
// Revision 1.1  2003/05/16 15:07:31  michael
// added inl file
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxMathUtils.inl,v 1.3 2006/03/29 20:03:19 geconomos Exp $
// $Id: vxMathUtils.inl,v 1.3 2006/03/29 20:03:19 geconomos Exp $
