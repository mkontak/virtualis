// $Id: vxBlockVolumeIteratorNeighborsConsts.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Rekha (ingmar|rekha)@viatronix.com

#ifndef vxBlockVolumeIteratorNeighborsConsts_h
#define vxBlockVolumeIteratorNeighborsConsts_h



// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


// class definition
class vxBlockVolumeIteratorNeighborsConsts
{
// enums
public:

  enum NeigborhoodsEnum
  {
    e2Dneighbors04xy, // traverse 4 neighbors that are face adjacent in 2D of xy plane (axial plane)
    e2Dneighbors04xz, // traverse 4 neighbors that are face adjacent in 2D of xz plane (??? plane)
    e2Dneighbors04yz, // traverse 4 neighbors that are face adjacent in 2D of yz plane (??? plane)
    e2Dneighbors08xy, // traverse 8 neighbors that are face adjacent in 2D of xy plane (axial plane)
    e2Dneighbors08xz, // traverse 8 neighbors that are face adjacent in 2D of xz plane (??? plane)
    e2Dneighbors08yz, // traverse 8 neighbors that are face adjacent in 2D of yz plane (??? plane)
    e3Dneighbors07,   // traverse 7 neighbors that are part of the same 2x2x2 cell
    e3Dneighbors06,   // traverse 6 neighbors that are face adjacent in 3D
    e3Dneighbors18,   // traverse 18 neighbors that are face and edge adjacent in 3D
    e3Dneighbors26,   // traverse 26 neighbors that are face and edge and corner adjacent in 3D
    e3Dneighbors63    // traverse 64 neighbors that are near in 3D
  }; // NeigborhoodsEnum
}; // vxBlockVolumeIteratorNeighborsConsts


#undef FILE_REVISION


#endif // vxBlockVolumeIteratorNeighborsConsts_h


// $Log: vxBlockVolumeIteratorNeighborsConsts.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/05/03 16:18:19  michael
// completed code review
//
// Revision 1.8  2002/05/22 20:50:23  ingmar
// added sectioning for doxygen documentation
//
// Revision 1.7  2002/05/14 21:52:09  ingmar
// added e3Dneighbors07 for traversal of 2x2x2 neighborhood
//
// Revision 1.6  2002/04/25 16:35:34  ingmar
// added e2Dneighbors08 xy, xz and zy neighbor traversal
//
// Revision 1.5  2002/04/25 16:13:19  ingmar
// added e2Dneighbors04xz and zy neighbor traversal
//
// Revision 1.4  2002/04/25 16:02:28  ingmar
// added e2Dneighbors04xy neighbor traversal
//
// Revision 1.3  2002/04/12 14:55:25  rekha
// extended to 6 & 18 & 26 & 63 neighborhoods
//
// Revision 1.2  2002/04/09 21:20:41  ingmar
// first working test
//
// Revision 1.1  2002/04/09 18:46:49  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeIteratorNeighborsConsts.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeIteratorNeighborsConsts.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
