// $Id: Patch.C,v 1.2 2006/01/31 14:04:55 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Kevin Kreeger (kevin@viatronix.net)


// pragmas
#pragma warning(disable:4786)


// includes
#include "stdafx.h"
#include "Patch.h"


/**
 * Constructor.
 */
Patch::Patch()
{
  m_uCSI = 0;
  m_iSegment = 0;
  m_fDistanceFromRectumMM = 0;
  m_viewPointWorld = Point<float4>(0.0F,0.0F,0.0F);
  m_vViewDirWorld = Vector<float4>(0.0F,0.0F,1.0F);
  m_uSurfaceCount = 0;
  m_centerWorld = Point<float4>(0.0F,0.0F,0.0F);
}


/**
 * Destructor.
 */
Patch::~Patch()
{
}


/**
 * Copy Constructor.
 */
Patch::Patch(const Patch & other)
{
  m_uCSI = other.m_uCSI;
  m_iSegment = other.m_iSegment;
  m_fDistanceFromRectumMM = other.m_fDistanceFromRectumMM;
  m_viewPointWorld = other.m_viewPointWorld;
  m_vViewDirWorld = other.m_vViewDirWorld;
  m_uSurfaceCount = other.m_uSurfaceCount;
  m_centerWorld = other.m_centerWorld;
}


/**
 * Assignment operator.
 */
Patch & Patch::operator =(const Patch & other)
{
  m_uCSI = other.m_uCSI;
  m_iSegment = other.m_iSegment;
  m_fDistanceFromRectumMM = other.m_fDistanceFromRectumMM;
  m_viewPointWorld = other.m_viewPointWorld;
  m_vViewDirWorld = other.m_vViewDirWorld;
  m_uSurfaceCount = other.m_uSurfaceCount;
  m_centerWorld = other.m_centerWorld;
  return * this;
}


/**
 * Compares the surface count of two Patch instances (e.g. for qsort function)
 * NOTE: Sorting using this function produces a Descending sort (largest first)
 *
 * @param const void * pVoidPatch1   Void pointer to Patch 1
 * @param const void * pVoidPatch2   Void pointer to Patch 2
 * @return int                       0 if surface counts are equal, -1 if first is greater, 1 otherwise.
 */
int Patch::SurfaceCountGreaterCompare(const void * pVoidPatch1, const void * pVoidPatch2)
{
  if (((Patch*)pVoidPatch1)->m_uSurfaceCount == ((Patch*)pVoidPatch2)->m_uSurfaceCount)
  {
    return 0;
  }
  else if (((Patch*)pVoidPatch1)->m_uSurfaceCount > ((Patch*)pVoidPatch2)->m_uSurfaceCount)
  {
    return -1;
  }
  else
  {
    return 1;
  }
}


// $Log: Patch.C,v $
// Revision 1.2  2006/01/31 14:04:55  frank
// code review
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.3.24.1  2005/06/28 12:06:18  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.3  2002/04/12 14:48:42  jmeade
// Changed sort function to sort in Descending order.
//
// Revision 3.2  2002/04/12 02:13:19  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.1  2002/03/08 17:15:18  michael
// further removed include dependencies
//
// Revision 3.0  2001/10/14 23:02:27  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:40   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.8  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.7  2001/03/29 16:18:22  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.6  2001/01/11 23:46:13  kevin
// Added navigation based missed patch viewing
//
// Revision 1.5  2001/01/08 02:42:50  kevin
// Cleaned up missed patch viewing by multiple methods
//
// Revision 1.4  2000/12/28 19:49:35  kevin
// Added new viewppoint and viewdir to see whcihc is better
//
// Revision 1.3  2000/11/25 03:04:43  kevin
// Fixed multi-segment patch jumping
//
// Revision 1.2  2000/11/16 17:38:58  kevin
// Added distance from Rectum to Patch structure
//
// Revision 1.1  2000/10/29 17:37:03  kevin
// Added Patch calss which holds a detected missed region of
// colon surface
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Patch.C,v 1.2 2006/01/31 14:04:55 frank Exp $
// $Id: Patch.C,v 1.2 2006/01/31 14:04:55 frank Exp $
