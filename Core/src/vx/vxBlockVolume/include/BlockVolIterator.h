// $Id: BlockVolIterator.h,v 1.3 2004/10/25 23:45:13 michael Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: Sept. 00
// Author: David dave@cs.sunysb.edu
//
// Complete history at bottom of file.

/**
* The BlockVolIterator allows convenient access to BlockVolume voxels.
*/
class VolIterator 
{
protected: // data
  const Triple<uint4> m_dim;
  const T & m_backgroundVoxel;
  BlockVector<T> & m_rBlocks;
  const uint2 m_offsetX;
  const uint2 m_offsetXY;
  const uint4 m_blockBitsX;
  const uint4 m_blockBitsXY;

  Point<uint2> m_pos;
  uint4 m_blockOffset;

public:
  typename BlockVector<T>::iterator m_iterBlock;  
  VolIterator(const Point<uint2> & point, const Triple<uint4> & dim,
              const T & background, BlockVector<T> & blocks, 
              const uint4 & blockBitsX, 
              const uint4 & blockBitsXY) 
  :
  m_dim(dim), 
  m_backgroundVoxel(background),
  m_rBlocks(blocks), m_offsetX(8), m_offsetXY(64),
  m_blockBitsX(blockBitsX), m_blockBitsXY(blockBitsXY), m_pos(point)
  {
    m_blockOffset = GetBlockIndexV(m_pos);
    m_iterBlock = &m_rBlocks[GetBlockV(m_pos)];
  }


  /**
   * Moves the position to the first non-background voxel, very efficient
   * for the BlockVolume layout.
   */
  inline void FirstNonZero()
  {
    MoveAbs(Point<uint2>(0,0,0));
    if ( m_backgroundVoxel == m_iterBlock->At(m_blockOffset) )
    {
      NextNonZero();
    }
  }


/**
 * Moves the position to the next non-background voxel, very efficient 
 * for the BlockVolume layout.
 */
  inline void NextNonZero()
  {
    do 
    {
      ++m_blockOffset;

      if ( m_blockOffset >= 512 )
      {  // stepped into the next block
        do 
        {
          MoveNextBlock(); // could improve effic. slightly
          if ( m_pos.m_z >= m_dim.m_z )
            return;
        } while ( !m_iterBlock->IsDirty() );
      }
    } while ( m_backgroundVoxel == m_iterBlock->At(m_blockOffset) );

    // set m_pos from m_blockOffset
    m_pos.m_x &= 0xFFF8;   m_pos.m_x |= ( m_blockOffset     & 0x7);
    m_pos.m_y &= 0xFFF8;   m_pos.m_y |= ((m_blockOffset>>3) & 0x7);
    m_pos.m_z &= 0xFFF8;   m_pos.m_z |= ((m_blockOffset>>6) & 0x7);
  }


  /**
   * Moves the position to the first non-background voxel, very efficient
   * for the BlockVolume layout.
   */
  inline void FirstNonZeroBlock()
  {
    MoveAbs(Point<uint2>(0,0,0));
    if ( m_backgroundVoxel == m_iterBlock->At(m_blockOffset) )
    {
      NextNonZeroBlock();
    }
  }


/**
 * Moves the position to the next non-background block, very efficient 
 * for the BlockVolume layout.
 */
  inline void NextNonZeroBlock()
  {
    do 
    {
      MoveNextBlock(); // could improve effic. slightly
      if ( m_pos.m_z >= m_dim.m_z )
        return;
    } while ( !m_iterBlock->IsDirty() );
  }


/**
 *
 */
  inline void operator++()
  {
    // just increment blockOffset
    ++m_blockOffset;
    if ( m_blockOffset < 512 )
    { // update m_pos within the block
      // set m_pos from m_blockOffset
      m_pos.m_x &= 0xFFF8;   m_pos.m_x |= ( m_blockOffset     & 0x7);
      m_pos.m_y &= 0xFFF8;   m_pos.m_y |= ((m_blockOffset>>3) & 0x7);
      m_pos.m_z &= 0xFFF8;   m_pos.m_z |= ((m_blockOffset>>6) & 0x7);
      if ( m_pos.m_x >= m_dim.m_x || m_pos.m_y >= m_dim.m_y || m_pos.m_z >= m_dim.m_z )
        operator++();
    }
    else
    {  // move to a new block
      MoveNextBlock();
    }
  }

/**
 *
 */
  inline void IncLinearly()
  {
    NextZYX();
  }


protected:
  inline void MoveNextBlock() 
  {
    m_blockOffset = 0;

    m_pos.m_x &= 0xFFF8;
    m_pos.m_y &= 0xFFF8;
    m_pos.m_z &= 0xFFF8;
    m_pos.m_x += 8;
    if ( m_pos.m_x < m_dim.m_x )
    {
      m_iterBlock = &m_rBlocks[GetBlockV(m_pos)];
      return;
    }
    else
    {
      m_pos.m_x = 0;
    }

    m_pos.m_y += 8;
    if ( m_pos.m_y < m_dim.m_y )
    {
      m_iterBlock = &m_rBlocks[GetBlockV(m_pos)];
      return;
    }
    else
    {
      m_pos.m_y = 0;
    }

    m_pos.m_z += 8;
    if ( m_pos.m_z < m_dim.m_z )
    {
      m_iterBlock = &m_rBlocks[GetBlockV(m_pos)];
    }
    else
    {
      // shouldn't be nec. just makes code safer for user mistakes
      m_iterBlock = &m_rBlocks[m_rBlocks.Size()-1];
    }
  }

public:

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

  inline void NextYXZ()
  {
    // imitate triple for() loop behavior

    // make more efficient ?? prob. not used often.
    UpdatePosYXZ();

    m_iterBlock = &m_rBlocks[GetBlockV(m_pos)];
    m_blockOffset = GetBlockIndexV(m_pos);
  }

  inline void NextZYX()
  {
    // imitate triple for() loop behavior

    // make more efficient ?? prob. not used often.
    UpdatePosZYX();

    m_iterBlock = &m_rBlocks[GetBlockV(m_pos)];
    m_blockOffset = GetBlockIndexV(m_pos);
  }

protected:

  inline void UpdatePosXZY()
  {

    if ( m_pos.m_y < (m_dim.m_y - 1) )
    {
      ++m_pos.m_y; return;
    }
    else
    {
      m_pos.m_y = 0;
    }

    if ( m_pos.m_z < (m_dim.m_z - 1) )
    {
      ++m_pos.m_z; return;
    }
    else
    {
      m_pos.m_z = 0;
    }

    if ( m_pos.m_x < (m_dim.m_x - 1) )
    {
      ++m_pos.m_x; return;
    }
    else
    {
      // note that calling GetVoxelRaw() in this state may cause bad things.
      if ( m_pos.m_x < (m_dim.m_x) )
      {
        ++m_pos.m_x;  // increment z anyway, should be equal to end now
        //m_inside = false;
      }
      else
      {
        //wrn << "<BlockVolume<T>::operator++> Attempted VolIterator increment illegally" << nl;
        //m_inside = false;
      }
    }
  } // UpdatePosXZY

  inline void UpdatePosYXZ()
  {

    if ( m_pos.m_z < (m_dim.m_z - 1) )
    {
      ++m_pos.m_z; return;
    }
    else
    {
      m_pos.m_z = 0;
    }

    if ( m_pos.m_x < (m_dim.m_x - 1) )
    {
      ++m_pos.m_x; return;
    }
    else
    {
      m_pos.m_x = 0;
    }

    if ( m_pos.m_y < (m_dim.m_y - 1) )
    {
      ++m_pos.m_y; return;
    }
    else
    {
      // note that calling GetVoxelRaw() in this state may cause bad things.
      if ( m_pos.m_y < (m_dim.m_y) )
      {
        ++m_pos.m_y;  // increment z anyway, should be equal to end now
        //m_inside = false;
      }
      else
      {
        //wrn << "<BlockVolume<T>::operator++> Attempted VolIterator increment illegally" << nl;
        //m_inside = false;
      }
    }
  } // UpdatePosZYX


  inline void UpdatePosZYX()
  {
    if ( m_pos.m_x < (m_dim.m_x - 1) )
    {
      ++m_pos.m_x; return;
    }
    else
    {
      m_pos.m_x = 0;
    }

    if ( m_pos.m_y < (m_dim.m_y - 1) )
    {
      ++m_pos.m_y; return;
    }
    else
    {
      m_pos.m_y = 0;
    }

    if ( m_pos.m_z < (m_dim.m_z - 1) )
    {
      ++m_pos.m_z; return;
    }
    else
    {
      // note that calling GetVoxelRaw() in this state may cause bad things.
      if ( m_pos.m_z < (m_dim.m_z) )
      {
        ++m_pos.m_z;  // increment z anyway, should be equal to end now
        //m_inside = false;
      }
      else
      {
        //wrn << "<BlockVolume<T>::operator++> Attempted VolIterator increment illegally" << nl;
        //m_inside = false;
      }
    }
  } // UpdatePosZYX


public:

/**
 *
 */
  inline bool End()
  {
    if ( InsideVolume() )
      return(false);
    else
      return(true);
  }

  void SetVoxel(const T & value)
  {
    if ( InsideVolume() )
    {
      SetVoxelRaw(value);
    }
  }


/// Sets the value at the current location.
  inline void SetVoxelRaw(const T & value)
  {
    m_iterBlock->Set(value,m_blockOffset);
  }


/// Returns the value at the current location: Safe version.
  const T & GetVoxel() const
  {
    if ( InsideVolume() )
      return(GetVoxelRaw());
    else
      return(m_backgroundVoxel);
  }


/// Returns the value at the current location: Unsafe version.
  inline const T &  GetVoxelRaw() const
  {
    return( m_iterBlock->At(m_blockOffset) );
  }


/// deprecated operator * , remains in API to support STL algorithms - use GetVoxel instead.
  inline const T & operator*() const
  {
    return(GetVoxelRaw());
  }


/// COMPARATORS
/// iterator inequality test by location & size.
  inline bool operator!=(const VolIterator & rhs)
  {
    return(!operator==(rhs));
  }


/// Iterator equality test by location & size.
  inline bool operator==(const VolIterator & rhs)
  {
    if ( rhs.GetDim() == m_dim && rhs.GetPos() == m_pos )
      return(true);
    else
      return(false);
  }


/// ABSOLUTE LOCATION FUNCTIONS

/// Moves the iterator to the given absolute position.
  inline void MoveAbs(const Point<uint2> & pos)
  {
    m_pos = pos;
    m_blockOffset = GetBlockIndexV(pos);
    uint4 block = GetBlockV(pos);
    if ( block < m_rBlocks.Size() )
      m_iterBlock = &m_rBlocks[block];
    else
      m_iterBlock = &m_rBlocks[m_rBlocks.Size()-1]; //background block
  }


/// Peeks at the value at the location given relative to current location.
  inline const T & PeekRel(const Vector<int2> & delta) const
  {
    Point<uint2> pos(m_pos.m_x+delta.m_x, m_pos.m_y+delta.m_y, m_pos.m_z+delta.m_z);
    if ( pos.m_x<m_dim.m_x && pos.m_y<m_dim.m_y && pos.m_z<m_dim.m_z )
    {
      return(m_rBlocks[GetBlockV(pos)].At(GetBlockIndexV(pos)));
    }
    else
    {
      return(m_backgroundVoxel);
    }
  }


/// Moves the iterator relative to its current position.
  inline void MoveRel(const Vector<int2> & delta)
  {
    m_pos.m_x += delta.m_x; m_pos.m_y += delta.m_y; m_pos.m_z += delta.m_z;

    m_blockOffset = GetBlockIndexV(m_pos);
    uint4 block = GetBlockV(m_pos);
    if ( block < m_rBlocks.Size() )
      m_iterBlock = &m_rBlocks[block];
    else
      m_iterBlock = &m_rBlocks[m_rBlocks.Size()-1]; //background block
  }


/// INCREMENTAL PEEK AT SOME VOXEL
/// Peeks at the next voxel in the x direction w/out moving the iterator.
  inline const T & PeekNextX() const
  {
    if ( (m_pos.m_x+1 & 7) != 0 )
      return( m_iterBlock->At(m_blockOffset+1) );
    else
      return(GetVoxelV(m_pos.m_x+1, m_pos.m_y, m_pos.m_z));
  }

  inline const T & PeekNextX(const uint2 & delta) const
  {
    return(GetVoxelV(m_pos.m_x + delta, m_pos.m_y, m_pos.m_z)); 
  }


/// Peeks at the next voxel in the y direction w/out moving the iterator.
  inline const T & PeekNextY() const
  {
    if ( (m_pos.m_y+1 & 7) != 0 )
    {
      return( m_iterBlock->At(m_blockOffset+m_offsetX) );
    }
    else
    {
      return(GetVoxelV(m_pos.m_x, m_pos.m_y+1, m_pos.m_z));
    }
  }

  inline const T & PeekNextY(const uint2 & delta) const
  {
    return(GetVoxelV(m_pos.m_x, m_pos.m_y + delta, m_pos.m_z));
  }


/// Peeks at the next voxel in the z direction w/out moving the iterator.

  inline const T & PeekNextZ() const
  {
    if ( (m_pos.m_z+1 & 7) != 0 )  return(m_iterBlock->At(m_blockOffset+m_offsetXY) );
    else return(GetVoxelV(Point<uint2>(m_pos.m_x, m_pos.m_y, m_pos.m_z+1)));
  }

  inline const T  PeekNextZ(const uint2 & delta) const
  {
    return(GetVoxelV(m_pos.m_x, m_pos.m_y, m_pos.m_z + delta));
  }


/// Peeks at the prev voxel in the x direction w/out moving the iterator.
  inline const T & PeekPrevX() const
  {
    if ( (m_pos.m_x-1 & 7) != 7 ) return(m_iterBlock->At(m_blockOffset-1));
    else return(GetVoxelV(Point<uint2>(m_pos.m_x-1, m_pos.m_y, m_pos.m_z)));
  }
  inline const T & PeekPrevX(const uint2 & delta) const
  {
    return(GetVoxelV(m_pos.m_x - delta, m_pos.m_y, m_pos.m_z));
  } 


/// Peeks at the prev voxel in the y direction w/out moving the iterator.
  inline const T & PeekPrevY() const
  {
    if ( (m_pos.m_y-1 & 7) != 7 ) return(m_iterBlock->At(m_blockOffset-m_offsetX));
    else return(GetVoxelV(Point<uint2>(m_pos.m_x, m_pos.m_y-1, m_pos.m_z)));
  }
  inline const T & PeekPrevY(const uint2 & delta) const
  {
    return(GetVoxelV(m_pos.m_x, m_pos.m_y - delta, m_pos.m_z));
  }  


/// Peeks at the prev voxel in the z direction w/out moving the iterator.
  inline const T & PeekPrevZ() const
  {
    if ( (m_pos.m_z-1 & 7) != 7 ) return(m_iterBlock->At(m_blockOffset-m_offsetXY));
    else return(GetVoxelV(Point<uint2>(m_pos.m_x, m_pos.m_y, m_pos.m_z-1))); 
  }
  inline const T & PeekPrevZ(const uint2 & delta) const
  {
    return(GetVoxelV(m_pos.m_x, m_pos.m_y, m_pos.m_z - delta));
  }


/// INCREMENTAL WALK TO NEAR BY VOXELS
/// Increments the iterator location in the x direction.
  inline void IncX()
  {
    ++m_pos.m_x;

    if ( (m_pos.m_x & 7) != 0 ) ++m_blockOffset;
    else
    {
      MoveAbs(m_pos);
    }
  }

  inline void IncX(const uint2 & delta)
  {
    m_pos.m_x += delta;
    MoveAbs(m_pos);
  }


/// Increments the iterator location in the y direction.
  inline void IncY()
  {
    ++m_pos.m_y;

    if ( (m_pos.m_y & 7) != 0 ) m_blockOffset += m_offsetX;
    else
    {
      MoveAbs(m_pos);
    }
  }

  void IncY(const uint2 & delta)
  {
    m_pos.m_y += delta;
    MoveAbs(m_pos);
  }


/// Increments the iterator location in the z direction.

  inline void IncZ()
  {
    ++m_pos.m_z;

    if ( (m_pos.m_z & 7) != 0 ) m_blockOffset += m_offsetXY;
    else
    {
      MoveAbs(m_pos);
    }
  }


  inline void IncZ(const uint2 & delta)
  {
    m_pos.m_z += delta;
    MoveAbs(m_pos);
  }


/// Decrements the iterator location in the x direction.
  inline void DecX()
  {
    --m_pos.m_x;

    // could wrap around here to Uint2_MAX
    // would not do MoveAbs() as it should, m_iterBlock is invalid
    if ( (m_pos.m_x & 7) != 7 ) --m_blockOffset;
    else
    {
      MoveAbs(m_pos);
    }
  }


  inline void DecX(const uint2 & delta)
  {
    m_pos.m_x -= delta;
    MoveAbs(m_pos);
  }

/// Decrements the iterator location in the y direction.
  inline void DecY()
  {
    --m_pos.m_y;

    // could wrap around here to Uint2_MAX
    if ( (m_pos.m_y & 7) != 7 ) m_blockOffset -= m_offsetX;
    else
    {
      MoveAbs(m_pos);
    }
  }

  inline void DecY(const uint2 & delta)
  {
    m_pos.m_y -= delta;
    MoveAbs(m_pos);
  }

/// Decrements the iterator location in the z direction.
  inline void DecZ()
  {
    --m_pos.m_z;

    // could wrap around here to USHORT_MAX
    if ( (m_pos.m_z & 7) != 7 ) m_blockOffset -= m_offsetXY;
    else
    {
      MoveAbs(m_pos);
    }
  }

  inline void DecZ(const uint2 & delta)
  {
    m_pos.m_z -= delta;
    MoveAbs(m_pos);
  }

/// STATUS REPORTS
/// Returns true if the iterator is inside the volume.

  inline bool InsideVolume() const
  {
    return(
          m_pos.m_x<m_dim.m_x &&
          m_pos.m_y<m_dim.m_y && 
          m_pos.m_z<m_dim.m_z );
  }


/// Returns the current x,y,z position.

  inline const Point<uint2> & GetPos() const
  {
    return(m_pos);
  }


/// Get Volume Dimension included in API for Convenience for other iterators.

  inline const Triple<uint4> & GetDim() const
  {
    return(m_dim);
  }

protected:

  inline uint2 GetBlockIndexV(const Point<uint2> & pos) const
  {
    return( (pos.m_x & 7) | ((pos.m_y & 7) << 3) |
            ((pos.m_z & 7) << 6 ));
  }



  inline uint4 GetBlockV(const Point<uint2> & pos) const
  {
    return((pos.m_x >> 3) | ((pos.m_y >> 3) << (m_blockBitsX)) |
           (uint4((pos.m_z >> 3)) << (m_blockBitsXY)));
  }

  inline const T & GetVoxelV(const Point<uint2> & pos) const
  {
    return(m_rBlocks[GetBlockV(pos)].At(GetBlockIndexV(pos)));
  }

  inline const T & GetVoxelV(const uint2 x, const uint2 y, const uint2 z) const
  {
    Point<uint2> pos(x, y, z);
    return(m_rBlocks[GetBlockV(pos)].At(GetBlockIndexV(pos)));
  }

};

// $Log: BlockVolIterator.h,v $
// Revision 1.3  2004/10/25 23:45:13  michael
// took out redundant code
//
// Revision 1.2  2004/03/04 21:00:55  geconomos
// Modifications for 7.1 compiler including: use of typename for dependant types and resolving ambiguities.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.7  2003/05/16 11:48:10  michael
// code review
//
// Revision 3.6  2002/04/02 21:28:24  ingmar
// added First+NextNonZeroBlock()
//
// Revision 3.5  2001/12/27 18:08:36  ingmar
// removed obsolete documentation inheritance
//
// Revision 3.4  2001/12/17 16:33:32  jaddonisio
// Set dim to actual size(not a multiple of 8).
//
// Revision 3.3  2001/11/30 02:35:02  soeren
// Ups fix number two
//
// Revision 3.2  2001/11/30 02:03:20  soeren
// Fixed bug with BlockVolume Dimensions which are NOT multiple of 8
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
//    Rev 2.0   Sep 16 2001 23:47:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:26   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:46  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:18  soeren
// renamed volume  directory
//
// Revision 1.18  2001/08/28 23:58:03  ingmar
// std::vector<Block<T >, BlockAllocator<Block<T> > > -> BlockVector<T>
//
// Revision 1.17  2001/07/19 22:50:43  liwei
// Added VolIteratorSkipBoundary for linear and block volume.
// Bugs of the PeekX() functions of the block volume fixed.
//
// Revision 1.16  2001/06/20 00:29:11  liwei
// Bug fixing for slice iterators
//
// Revision 1.15  2001/06/18 23:40:12  liwei
// Slice iterator added
//
// Revision 1.14  2001/05/29 20:26:28  dave
// commented out calls to wrn, speed is back
//
// Revision 1.13  2001/04/14 04:21:24  ingmar
// added inline directive
//
// Revision 1.12  2001/04/09 16:25:35  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.11  2001/04/08 22:04:17  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.10  2001/04/08 21:38:21  dave
// updated doc headers
//
// $Id: BlockVolIterator.h,v 1.3 2004/10/25 23:45:13 michael Exp $