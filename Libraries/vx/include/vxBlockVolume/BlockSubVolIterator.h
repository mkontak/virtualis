// $Id: BlockSubVolIterator.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/BlockSubVolIterator.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright© 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:	LinearVolSliceIterator.h
// Description:
// Created: July 19, 2001
// Author:
//	Wei Li (liwei@viatronix.com)
// Complete history on bottom of file

/// @see VxVoliterator

/**
 * Description
 * $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/BlockSubVolIterator.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a>
*/
class SubVolIterator : public VolIterator 
{
  /*
    Can implement w/ voxPtr & m_pos or voxPtr & m_inside, trade-offs for either option.
  */

private: // data
	/// bounding box
	Box<uint4> m_boundingBox;
public:

	SubVolIterator(const Triple<uint4> & dim,
		const T & background, 
    BlockVector<T> & blocks, 
		const uint4 & blockBitsX, 
		const uint4 & blockBitsXY,
		uint2 uBoundary) : 
	VolIterator(Point<uint2> (uBoundary, uBoundary, uBoundary), dim, background, blocks, blockBitsX, blockBitsXY),
		m_boundingBox(uBoundary, uBoundary, uBoundary, m_dim.m_x - uBoundary, m_dim.m_y - uBoundary, m_dim.m_z - uBoundary)
	{
  } 

	SubVolIterator(const Triple<uint4> & dim,
		const T & background, 
    BlockVector<T> & blocks, 
		const uint4 & blockBitsX, 
		const uint4 & blockBitsXY,
		const Box<uint4>& boundingBox): 
	VolIterator(Point<uint2>(boundingBox.GetMinX(), boundingBox.GetMinY(), boundingBox.GetMinZ()), 
											dim, background, blocks, blockBitsX, blockBitsXY), 
		m_boundingBox(boundingBox)
	{
	}

		
/**
 *
 */
  inline void IncLinearly()
  {
    NextZYX();
  }


/**
 *
 */
  inline void NextXZY()
  {
    // imitate triple for() loop behavior

    // make more efficient ?? prob. not used often.
    UpdatePosXZY();

    m_iterBlock = &m_rBlocks[GetBlockV(m_pos)];
    m_blockOffset = GetBlockIndexV(m_pos);
  }

/**
 *
 */
  inline void NextYXZ()
  {
    // imitate triple for() loop behavior

    // make more efficient ?? prob. not used often.
    UpdatePosYXZ();

    m_iterBlock = &m_rBlocks[GetBlockV(m_pos)];
    m_blockOffset = GetBlockIndexV(m_pos);
  }

/**
 *
 */
 inline void NextZYX()
  {
    // imitate triple for() loop behavior

    // make more efficient ?? prob. not used often.
    UpdatePosZYX();

    m_iterBlock = &m_rBlocks[GetBlockV(m_pos)];
    m_blockOffset = GetBlockIndexV(m_pos);
  }

protected:
/**
 *
 */
  inline void UpdatePosXZY()
  {

    if ( m_pos.m_y < (m_boundingBox.GetMaxY() - 1) )
    {
      ++m_pos.m_y; return;
    } else
    {
      m_pos.m_y = m_boundingBox.GetMinY();
    }

    if ( m_pos.m_z < (m_boundingBox.GetMaxZ() - 1) )
    {
      ++m_pos.m_z; return;
    } else
    {
			m_pos.m_z = m_boundingBox.GetMinZ();
		}

    if ( m_pos.m_x < (m_boundingBox.GetMaxX() - 1) )
    {
      ++m_pos.m_x; return;
    } else
    {
      // increment z anyway, should be equal to end now
			m_pos.m_x = m_pos.m_y = 0;
			m_pos.m_z = m_dim.m_z;
    }
  } // UpdatePosXZY


/**
 *
 */
  inline void UpdatePosYXZ()
  {

    if ( m_pos.m_z < (m_boundingBox.GetMaxZ() - 1) )
    {
      ++m_pos.m_z; return;
    } else
    {
			m_pos.m_z = m_boundingBox.GetMinZ();
		}

    if ( m_pos.m_x < (m_boundingBox.GetMaxX() - 1) )
    {
      ++m_pos.m_x; return;
    } else
    {
      m_pos.m_x = m_boundingBox.GetMinX();
    }

    if ( m_pos.m_y < (m_boundingBox.GetMaxY() - 1) )
    {
      ++m_pos.m_y; return;
    } else
    {
      // increment z anyway, should be equal to end now
			m_pos.m_x = m_pos.m_y = 0;
			m_pos.m_z = m_dim.m_z;
    }
  } // UpdatePosZYX

/**
 *
 */
  inline void UpdatePosZYX()
  {
    if ( m_pos.m_x < (m_boundingBox.GetMaxX() - 1) )
    {
      ++m_pos.m_x; return;
    } else
    {
      m_pos.m_x = m_boundingBox.GetMinX();
    }
		
    if ( m_pos.m_y < (m_boundingBox.GetMaxY() - 1) )
    {
      ++m_pos.m_y; return;
    } else
    {
      m_pos.m_y = m_boundingBox.GetMinY();
    }
		
    if ( m_pos.m_z < (m_boundingBox.GetMaxZ() - 1) )
    {
      ++m_pos.m_z; return;
    } else
    {
      // increment z anyway, should be equal to end now
			m_pos.m_x = m_pos.m_y = 0;
			m_pos.m_z = m_dim.m_z;
    }
  } // UpdatePosZYX

};


// $Log: BlockSubVolIterator.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.2  2003/05/16 11:48:10  michael
// code review
//
// Revision 3.1  2001/10/26 18:26:19  liwei
// Added IncLinearly().
//
// Revision 3.0  2001/10/14 23:02:21  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:52   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:24   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:46  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:18  soeren
// renamed volume  directory
//
// Revision 1.7  2001/08/29 15:22:59  soeren
// fixed std::vector size_t bug
//
// Revision 1.6  2001/08/28 23:58:03  ingmar
// std::vector<Block<T >, BlockAllocator<Block<T> > > -> BlockVector<T>
//
// Revision 1.5  2001/08/09 21:17:26  liwei
// Constructor of BoundingRect and Box changed.
//
// Revision 1.4  2001/08/08 17:32:57  liwei
// Bounding boxes are all of the type Box<uint4>.
// Bugs in LinearVolumeIterator::NextZYX() fixed.
//
// Revision 1.3  2001/08/06 23:24:26  liwei
// Passing boundingBox rather minCorner and maxCorner when creating SubVolume Iterator
// and slice iterators.
//
// Revision 1.2  2001/07/31 18:46:29  liwei
// Bugs in the revised version of SliceIterator fixed.
//
// Revision 1.1  2001/07/30 23:29:30  liwei
// Renamed VolIteratorSkipBoundary to SubVolIterator.
// Make slice iterators be subclasses of SubVolIterator.
//
// Revision 1.1  2001/07/19 22:49:47  liwei
// Added to CVS.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/BlockSubVolIterator.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: BlockSubVolIterator.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
