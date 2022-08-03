// $Id: LinearVolSliceIterator.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/LinearVolSliceIterator.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:	LinearVolSliceIterator.h
// Description:
// Created: 
// Author:
//	Wei Li (liwei@viatronix.com)
// Complete history on bottom of file

/// @see VxVoliterator

/**
 * Description
 * $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/LinearVolSliceIterator.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a>
*/
#include "Enums.h"

class XSliceIterator : public SubVolIterator 
{
public:

  XSliceIterator(uint2 uIdxSlice, const Triple<uint4> & dim,
    const T & background, const typename std::vector<T>::iterator baseAddr, uint2 uBoundary) :
		SubVolIterator(dim, background, baseAddr, 
			Box<uint4>(uIdxSlice, uBoundary, uBoundary, 
								 dim.m_x - uBoundary, dim.m_y - uBoundary, dim.m_z - uBoundary))
  {
  }
		

	XSliceIterator(uint2 uIdxSlice, const Triple<uint4> & dim,
		const T & background, const typename std::vector<T>::iterator baseAddr, 
		const BoundingRect<uint4>& boundingBox) :
	SubVolIterator(dim, background, baseAddr, 
				Box<uint4>(boundingBox, uIdxSlice, uIdxSlice + 1, nsORIENTATION::X_ORIENTATION))
	{
	}

	
/**
 *
 */
  void operator++()
  {
    NextXZY();
  }


/**
 *
 */
  inline void IncLinearly()
  {
    operator ++();
  }
};

class YSliceIterator : public SubVolIterator 
{
public:

  YSliceIterator(uint2 uIdxSlice, const Triple<uint4> & dim,
    const T & background, const typename std::vector<T>::iterator baseAddr, 
		uint2 uBoundary) :
		SubVolIterator(dim, background, baseAddr, 
			Box<uint4>(uBoundary, uIdxSlice, uBoundary, 
			           dim.m_x - uBoundary, dim.m_y - uBoundary, dim.m_z - uBoundary))
		{
		}
		
  YSliceIterator(uint2 uIdxSlice, const Triple<uint4> & dim,
    const T & background, const typename std::vector<T>::iterator baseAddr, const BoundingRect<uint4>& boundingBox) :
		SubVolIterator(dim, background, baseAddr, 
			Box<uint4>(boundingBox, uIdxSlice, uIdxSlice + 1, nsORIENTATION::Y_ORIENTATION))
  {
  }

/**
 *
 */
  void operator++()
  {
    NextYXZ();
  }

/**
 *
 */
  inline void IncLinearly()
  {
    operator ++();
  }

};

class ZSliceIterator : public SubVolIterator 
{
public:

  ZSliceIterator(uint2 uIdxSlice, const Triple<uint4> & dim,
    const T & background, const typename std::vector<T>::iterator baseAddr,  
		uint2 uBoundary) :
		SubVolIterator(dim, background, baseAddr, 
			Box<uint4>(	uBoundary, uBoundary, uIdxSlice, 
									dim.m_x - uBoundary, dim.m_y - uBoundary, dim.m_z - uBoundary))
		{
		}
		
  ZSliceIterator(uint2 uIdxSlice, const Triple<uint4> & dim,
    const T & background, const typename std::vector<T>::iterator baseAddr, const BoundingRect<uint4>& boundingBox) :
		SubVolIterator(dim, background, baseAddr, 
			Box<uint4>( boundingBox, uIdxSlice, uIdxSlice + 1, nsORIENTATION::Z_ORIENTATION))
  {
  }

/**
 *
 */
  void operator++()
  {
    NextZYX();
  }

/**
 *
 */
  inline void IncLinearly()
  {
    operator ++();
  }
};

// $Log: LinearVolSliceIterator.h,v $
// Revision 1.2  2004/03/04 21:00:55  geconomos
// Modifications for 7.1 compiler including: use of typename for dependant types and resolving ambiguities.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.2  2003/05/16 11:48:11  michael
// code review
//
// Revision 3.1  2001/10/26 18:26:20  liwei
// Added IncLinearly().
//
// Revision 3.0  2001/10/14 23:02:22  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:02   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:34   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:45  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:19  soeren
// renamed volume  directory
//
// Revision 1.8  2001/08/09 21:33:26  liwei
// Construct Box from the BoundingRect argument.
//
// Revision 1.7  2001/08/09 21:17:26  liwei
// Constructor of BoundingRect and Box changed.
//
// Revision 1.6  2001/08/08 17:32:57  liwei
// Bounding boxes are all of the type Box<uint4>.
// Bugs in LinearVolumeIterator::NextZYX() fixed.
//
// Revision 1.5  2001/08/06 23:24:26  liwei
// Passing boundingBox rather minCorner and maxCorner when creating SubVolume Iterator
// and slice iterators.
//
// Revision 1.4  2001/08/02 22:23:14  liwei
// Bugs in LinearSubVolumeIterator::NextZYX() fixed.
//
// Revision 1.3  2001/07/31 18:46:29  liwei
// Bugs in the revised version of SliceIterator fixed.
//
// Revision 1.2  2001/07/30 23:29:30  liwei
// Renamed VolIteratorSkipBoundary to SubVolIterator.
// Make slice iterators be subclasses of SubVolIterator.
//
// Revision 1.1  2001/06/20 00:29:11  liwei
// Bug fixing for slice iterators
//
// Revision 1.1  2001/06/18 23:40:12  liwei
// Slice iterator added
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/LinearVolSliceIterator.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $
// $Id: LinearVolSliceIterator.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $
