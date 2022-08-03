// $Id: RenderUtil.inl,v 1.2 2006/02/07 21:32:44 gdavidson Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/RenderUtil.inl,v 1.2 2006/02/07 21:32:44 gdavidson Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: Utility rendering classes
// Author: Kevin Kreeger (kevin@viatronix.net)


/**
 * Efficiently interpolate two uint1 values using fixed point math.
 *
 * @param uLeft the left-hand value
 * @param uRight the right-hand value
 * @param uWeightRight the weight of the right-hand value (0 corresponds to all left, 255 corresponds to all right)
 * @return 
 */
uint2 RenderUtil::InterpolateFixedPoint( const uint1 & uLeft, const uint1 & uRight, const uint1 & uWeightRight )
{
  const uint2 uWeightLeft = 255 - uWeightRight;
  const uint2 uTmp1 = 128 + uWeightLeft * uLeft;
  const uint2 uTmp2 = 128 + uWeightRight * uRight;
  const uint2 uRes1 = (uTmp1 + (uTmp1 >> 8)) >> 8;
  const uint2 uRes2 = (uTmp2 + (uTmp2 >> 8)) >> 8;
  return uRes1 + uRes2;
} // InterpolateFixedPoint()


// $Log: RenderUtil.inl,v $
// Revision 1.2  2006/02/07 21:32:44  gdavidson
// Code Review
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.1  2002/11/20 22:29:09  frank
// Added fixed-point interpolation
//
// Revision 3.0  2001/10/14 23:02:31  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:50:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:40   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 23:32:18  ingmar
// new directory structure
//
// Revision 1.1  2001/05/15 17:50:53  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/RenderUtil.inl,v 1.2 2006/02/07 21:32:44 gdavidson Exp $
// $Id: RenderUtil.inl,v 1.2 2006/02/07 21:32:44 gdavidson Exp $
