// $Id: utilities.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/utilities.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Wei Li (liwei@viatronix.com)


// includes
#include "stdafx.h"
#include "utilities.h"



// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


/** 
 * Compute log base 2
 * @param value
 * @return log
 */
//const float4 log2(const float4 fValue)
////*******************************************************************
//{
//	if (fValue <= 0.0)
//    throw ex::IllegalArgumentException(LogRec("values <= 0", "Utilities.C", "log2()"));
//
//  return static_cast< float4 >( log(fValue)/log(2.0f) );
//} // log2()


/** 
 * Get bits needed to represent value
 * @param value
 * @return number of bits
 */
const uint4 GetBits(uint4 iValue)
//*******************************************************************
{
  uint4 uBits(0);

  while (iValue > 0)
  {
    iValue = iValue >> 1;
    uBits++;
  }

  return uBits;
} // GetBits()


#undef FILE_REVISION


// $Log: utilities.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/05/12 16:31:48  michael
// code review
//
// Revision 3.4  2002/03/08 17:15:17  michael
// further removed include dependencies
//
// Revision 3.3  2001/10/31 20:40:38  liwei
// Revised log2 and GetShiftOffset.
//
// Revision 3.2  2001/10/30 15:22:04  liwei
// log2() revised.
//
// Revision 3.1  2001/10/26 18:14:54  liwei
// Added GetBits().
//
// Revision 3.0  2001/10/14 23:02:16  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Sep 24 2001 17:14:38   liwei
// A bug in log2() fixed.
// 
//    Rev 2.1   Sep 19 2001 14:30:02   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:34   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:44   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:31  soeren
// renamed utils directory
//
// Revision 1.4  2001/07/31 18:48:51  liwei
// log2() changed.
//
// Revision 1.3  2001/07/27 23:53:51  liwei
// Bugs in log2() fixed.
//
// Revision 1.2  2001/07/26 15:00:17  liwei
// Compiler warning about log2 fixed.
//
// Revision 1.1  2001/07/25 19:02:24  liwei
// utilities.* moved from 3_math to 1_utils.
//
// Revision 1.2  2001/06/27 14:11:53  liwei
// A couple of functions by VolumeSet moved to Utilities.*
//
// Revision 1.1  2001/06/25 19:22:29  geconomos
// Moved from 9_render
//
// Revision 1.1  2001/06/25 17:23:49  liwei
// *.cpp files renamed to *.C files
//
// Revision 1.2  2001/06/13 14:40:21  liwei
// Add header
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/utilities.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: utilities.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $

/*
History:	$History: utilities.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Liwei        Date: 5/24/01    Time: 7:24p
 * Updated in $/AAA/dataStruct
 * Window Level added to VolumeRenderView
 * 
 * *****************  Version 1  *****************
 * User: Liwei        Date: 5/02/01    Time: 10:28a
 * Created in $/aaa/dataStruct
*/