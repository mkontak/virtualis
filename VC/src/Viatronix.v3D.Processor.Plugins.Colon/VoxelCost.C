// $Id: VoxelCost.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: 7/21/00
// Author: Michael Ming Wan & Mie Sato   {michael|mie}@viatronix.net
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "VoxelCost.h"

/**
 * Constructor for node.
 */
VoxelCost::VoxelCost()
{
  m_costOfVoxel = -1;
  m_piecewiseEuclideanDistance = FLT_MAX; // originally set to infinite
  m_costToRoot                 = FLT_MAX; // originally set to infinite
  m_location = Point<uint2>(0,0,0);
} // Constructor


/**
 * Constructor for node used in a BlockVolume.
 */
VoxelCost::VoxelCost(const uint4 & zeroForEmpty)
{
  VoxelCost();
} // Constructor


/**
 * Advanced constructor.
 */
VoxelCost::VoxelCost(const float4 & costOfVoxel, const float4 & piecewiseEuclideanDistance, const float4 & costToRoot, const Point<uint2> & location)
{
  m_costOfVoxel = costOfVoxel;
  m_piecewiseEuclideanDistance = piecewiseEuclideanDistance;
  m_costToRoot = costToRoot;
  m_location = location;
}

/**
 * Deconstructor for node structure.
 */
VoxelCost::~VoxelCost()
{
} // Deconstructor


// $Log: VoxelCost.C,v $
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.0  2001/10/14 23:01:51  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:28   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:06   ingmar
// Initial revision.
// Revision 1.3  2001/04/08 21:38:58  dave
// updated doc headers
//
// Revision 1.2  2001/04/06 18:20:36  dmitry
// updated datatypes
//
// Revision 1.1  2001/01/26 17:17:27  dave
// changed Node.h to VoxelCost.h
//
// Revision 1.8  2001/01/26 04:21:37  dave
// removed extra constructor
//
// Revision 1.7  2000/11/13 12:59:26  frank
// Cleaned up code
//
// Revision 1.6  2000/10/31 13:53:33  frank
// Merged in new skeleton and segment structure
//
// Revision 1.5.4.1  2000/10/28 17:38:59  ingmar
// new ierators for new segments
//
// Revision 1.5  2000/10/21 06:06:04  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.4.6.1  2000/10/18 01:48:23  binli
// moved GLobal.h up top to remove compilation warnings in debug mode
//
// Revision 1.4.2.3  2000/10/19 14:28:25  frank
// changed short to ushort
//
// Revision 1.4.2.2  2000/10/17 18:05:49  frank
// Debugged skeleton grow
//
// Revision 1.4.2.1  2000/10/05 19:00:58  frank
// Revamped entire algorithm
//
// Revision 1.4  2000/09/27 23:29:43  ingmar
// merged TEASAR back into main trunc
//
// Revision 1.3.6.2  2000/09/27 22:21:07  ingmar
// teasar and Michael's algorithm now both work and peacefully coexist
//
// Revision 1.3.6.1  2000/09/27 02:34:12  ingmar
// m_piecewiseEuclideanDistance
//
// Revision 1.3  2000/09/21 15:17:42  michael
// put more comments after code walk through of skeleton
//
// Revision 1.2  2000/09/12 17:58:17  ingmar
// moved initializations to constructor
//
// Revision 1.1.1.1  2000/08/11 22:15:55  ingmar
// unit tests complete
//
//
// 10    8/21/00 4:55p Antonio
// updated header
//
// *****************  Version 9  *****************
// User: Ingmar       Date: 8/15/00    Time: 6:41p
// Updated in $/v2k/src/Skeleton
// cleaned up debug prints and made Skeletonator use the ddb volume
//
// *****************  Version 8  *****************
// User: Michael      Date: 8/15/00    Time: 5:09p
// Updated in $/v2k/src/Skeleton
// delete neighbours from node
//
// *****************  Version 7  *****************
// User: Antonio      Date: 8/11/00    Time: 3:20p
// Updated in $/v2k/src/Skeleton
// added header and history to end of file.
//
// **********************
// Label: last_void_*_volume_version
// User: Jay             Date:  8/11/00  Time:  1:19a
// Labeled 'last_void_*_volume_version'
// Label comment:
//   last void * volume version
// about to replace with template volume
//
// *****************  Version 6  *****************
// User: Mie             Date:  8/02/00  Time: 12:36p
// Checked in $/v2k/src/Skeleton
// Comment:
//   streamlined heap, removed heapPosition index variable
//
// *****************  Version 5  *****************
// User: Mie             Date:  8/01/00  Time: 10:08a
// Checked in $/v2k/src/Skeleton
//
// *****************  Version 4  *****************
// User: Mie             Date:  7/31/00  Time:  5:23p
// Checked in $/v2k/src/Skeleton
// Comment:
//   added Viatronix comments
//
// *****************  Version 3  *****************
// User: Mie             Date:  7/31/00  Time:  3:21p
// Checked in $/v2k/src/Skeleton
// Comment:
//   added vx style comments
//
// *****************  Version 2  *****************
// User: Ingmar          Date:  7/20/00  Time:  4:36p
// Checked in $/v2k/src/Skeleton
// Comment:
//   improved comments
//
// *****************  Version 1  *****************
// User: Michael         Date:  7/18/00  Time:  7:16p
// Created Node.C
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/VoxelCost.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
// $Id: VoxelCost.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
