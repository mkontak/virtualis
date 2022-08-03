// $Id: vxMathUtils.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxMathUtils.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxMathUtils.h"
#include "vxPlaneOblique.h"
#include "vxCamera.h"
#include "Box.h"



// defines
#define FILE_REVISION "$Revision: 1.2 $"



/**
 * Compute minimum distance of plane from origin but plane being within box
 * @param box
 * @param oblique plane
 * @return maximum distance
 */
float4 vxMathUtils::GetMinDistance(const Box<float4> & box, 
                                   const vxPlaneOblique<float4> & plane)
//*******************************************************************
{
  float4 fMinDist((std::numeric_limits<float>::max)()), fDist;

  for (int i(0); i<8; i++)
  {
    Point<float4> point(box.GetCorner(i));
    fDist = plane.GetDistance(box.GetCorner(i));
    if (fDist < fMinDist)
    {
      fMinDist = fDist;
    }
  }
  return fMinDist;
} // GetMinDistance


/**
 * Compute index of the minimum distance of plane from origin but plane being within box
 * @param const Box<float4> &
 * @param const vxPlaneOblique<float4> &
 * @return maximum distance
 */
uint2 vxMathUtils::GetMinDistanceCornerIndex(const Box<float4> & box, 
                                             const vxPlaneOblique<float4> & plane)
//*******************************************************************
{
  float4 fMinDist((std::numeric_limits<float>::max)()), fDist;
  uint2 uIndex(-1);

  for (int i(0); i<8; i++)
  {
    Point<float4> point(box.GetCorner(i));
    fDist = plane.GetDistance(box.GetCorner(i));
    if (fDist < fMinDist)
    {
      fMinDist = fDist;
      uIndex = i;
    }
  }
  return uIndex;
} // GetMinDistanceCornerIndex()


/**
 * Compute maximum distance of plane from origin but plane being within box
 * @param const Box<float4> &
 * @param const vxPlaneOblique<float4> &
 * @return minimum distance
 */
float4 vxMathUtils::GetMaxDistance(const Box<float4> & box, 
                                   const vxPlaneOblique<float4> & plane)
//*******************************************************************
{
  float4 fMaxDist(-1 * (std::numeric_limits<float>::max)()), fDist;

  for (int i(0); i<8; i++)
  {
    Point<float4> point(box.GetCorner(i));
    fDist = plane.GetDistance(box.GetCorner(i));
    if (fDist > fMaxDist)
      fMaxDist = fDist;
  }
  return fMaxDist;
} // GetMaxDistance()


/**
 * Compute maximum distance of plane from origin but plane being within box
 * @param const Box<float4> &
 * @param const vxPlaneOblique<float4> &
 * @return minimum distance
 */
uint2 vxMathUtils::GetMaxDistanceCornerIndex(const Box<float4> & box, 
                                             const vxPlaneOblique<float4> & plane)
//*******************************************************************
{
  float4 fMaxDist(-1 * (std::numeric_limits<float>::max)()), fDist;
  uint2 uIndex(-1);

  for (int i(0); i<8; i++)
  {
    Point<float4> point(box.GetCorner(i));
    fDist = plane.GetDistance(box.GetCorner(i));
    if (fDist > fMaxDist)
    {
      fMaxDist = fDist;
      uIndex = i;
    }
  }
  return uIndex;
} // GetMaxDistanceCornerIndex()


#undef FILE_REVISION


// Revision History:
// $Log: vxMathUtils.C,v $
// Revision 1.2  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/09/23 14:44:53  michael
// removed utility functions to computer Euler Angles for Shimadzu. Is now part
// of the V3D Angio plugin code.
//
// Revision 1.4  2003/08/29 16:41:44  michael
// adjusted code to reflect discussion with Takeshi Ozaki (Shimadzu)
//
// Revision 1.3  2003/08/07 19:20:00  michael
// flipped the view vector so that we have a right handed coordinate system
//
// Revision 1.2  2003/08/07 14:57:44  michael
// added general Euler angle function
//
// Revision 1.1  2003/08/07 14:22:35  michael
// moved vxMathUtils from 70_vxUtils to 12_mathGFX were it belongs to
//
// Revision 1.6  2003/05/16 14:56:27  michael
// code review
//
// Revision 1.5  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.4  2002/11/21 22:52:19  michael
// added corner helper methods
//
// Revision 1.3  2002/11/20 21:14:35  michael
// added a few more helper methods
//
// Revision 1.2  2002/11/20 18:18:27  michael
// added compilable version(s)
//
// Revision 1.1  2002/11/20 14:32:24  michael
// added inital version of math utilities to find intersections of objects, etc.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxMathUtils.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
// $Id: vxMathUtils.C,v 1.2 2006/01/18 20:39:26 geconomos Exp $
