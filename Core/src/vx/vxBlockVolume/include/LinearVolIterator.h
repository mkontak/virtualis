// $Id: LinearVolIterator.h,v 1.4 2006/01/18 20:36:48 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/LinearVolIterator.h,v 1.4 2006/01/18 20:36:48 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description:
// Created: 
// Author:
//
// Complete history on bottom of file

/// @see VxVoliterator
#ifndef __LINEAR_VOL_ITERATOR_H
#define __LINEAR_VOL_ITERATOR_H

/**
 * Description
 * $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/LinearVolIterator.h,v 1.4 2006/01/18 20:36:48 geconomos Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a>
*/
class VolIterator
{
  /*
    Can implement w/ voxPtr & m_pos or voxPtr & m_inside, trade-offs for either option.
  */

protected: // data
  const Triple<uint4> m_dim;
  const T & m_background;
  const uint4 m_yStep;
  const uint4 m_zStep;
  const typename std::vector<T>::iterator m_pBaseAddr;
  
  typename std::vector<T>::iterator m_pVox;
  Point<uint2> m_pos;

public:

  VolIterator(const Point<uint2> & point, const Triple<uint4> & dim,
    const T & background, const typename std::vector<T>::iterator baseAddr) :
  m_dim(dim), m_background(background), m_yStep(m_dim.m_x), 
  m_zStep(m_dim.m_x*m_dim.m_y), m_pBaseAddr(baseAddr), m_pos(point)
  {
    m_pVox = m_pBaseAddr + GetOffset(m_pos);
  }

  /**
  * Moves the position to the first non-background voxel.
  */
  inline void FirstNonZero()
  {
    MoveAbs(Point<uint2>(0,0,0));
    while((*m_pVox == m_background) && InsideVolume())
    {
      NextZYX();
    }
  }
  
  
/**
 * Moves the position to the next non-background voxel, very efficient 
 * for the BlockVolume layout.
 */
  void NextNonZero()
  {
    do {
      NextZYX();
    } while(InsideVolume() && (*m_pVox == m_background));
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


/**
 *
 */
  void NextZYX()
  {
    // imitate triple for() loop behavior
    ++m_pVox;
    if ( m_pos.m_x < (m_dim.m_x - 1) ) {
      ++m_pos.m_x; return;
    } else {
      m_pos.m_x = 0;
    }
    
    if ( m_pos.m_y < (m_dim.m_y - 1) ) {
      ++m_pos.m_y; return;
    } else {
      m_pos.m_y = 0;
    }
    
    if ( m_pos.m_z < (m_dim.m_z - 1) ) {
      ++m_pos.m_z; return;
    } else {
      --m_pVox;  // place pointer back where it was, try to keep it valid even though outside
      // note that calling GetVoxelRaw() in this state may cause bad things.
      if ( m_pos.m_z < (m_dim.m_z) ) {
        ++m_pos.m_z;  // increment z anyway, should be equal to end now
      } 
      else 
      {
        //wrn << "VxVolIterator<L,T>::operator++: Attempted VxVolIterator increment illegally" <<std::endl;
      }
    }

    /*
    // this works too (just slower)
    if(InsideVolume(m_pos)) {
      ++m_pVox;
      m_pos = PosFromOffset();
    }
    */
  }

/**
 *
 */
  void NextXZY()
  {
    if ( m_pos.m_y < (m_dim.m_y - 1) ) {
      ++m_pos.m_y; 
	    m_pVox += m_yStep;
			return;
    } else {
      m_pos.m_y = 0;
    }
    
    if ( m_pos.m_z < (m_dim.m_z - 1) ) {
      ++m_pos.m_z; 
	    m_pVox = m_pBaseAddr + GetOffset(m_pos);
			return;
    } else {
			m_pos.m_z = 0;
		}
    if ( m_pos.m_x < (m_dim.m_x - 1) ) {
      ++m_pos.m_x; 
	    m_pVox = m_pBaseAddr + GetOffset(m_pos);
			return;
    } else {
			// increment z anyway, should be equal to end now
        m_pos.m_x = 0;
				m_pos.m_y = 0;
				m_pos.m_z = m_dim.m_z; 
		    m_pVox = m_pBaseAddr + GetOffset(m_pos);
				-- m_pVox; // place pointer back where it was, try to keep it valid even though outside
    }
  }

/**
 *
 */
  void NextYXZ()
  {
    if ( m_pos.m_z < (m_dim.m_z - 1) ) {
      ++m_pos.m_z; 
	    m_pVox += m_zStep;
			return;
    } else {
			m_pos.m_z = 0;
		}
    if ( m_pos.m_x < (m_dim.m_x - 1) ) {
      ++m_pos.m_x; 
	    m_pVox = m_pBaseAddr + GetOffset(m_pos);
			return;
    } else {
      m_pos.m_x = 0;
    }
    
    if ( m_pos.m_y < (m_dim.m_y - 1) ) {
      ++m_pos.m_y; 
	    m_pVox = m_pBaseAddr + GetOffset(m_pos);
			return;
    } else {
      m_pos.m_x = 0;
			m_pos.m_y = 0;
			m_pos.m_z = m_dim.m_z; 
	    m_pVox = m_pBaseAddr + GetOffset(m_pos);
			-- m_pVox; // place pointer back where it was, try to keep it valid even though outside
    }
  }


protected:
Point<uint2> PosFromOffset()
{
  Point<uint2> res;
  uint offset = (m_pVox - m_pBaseAddr);

  res.m_z = offset / m_zStep;
  uint rem = offset % m_zStep;
  res.m_y =  rem / m_yStep;
  rem = rem % m_yStep;
  res.m_x = rem;
  
  return (res);
}

public:


/**
 *
 */
  bool End()
  {
    if ( InsideVolume(m_pos) )
      return (false);
    else
      return (true);
  }

  void SetVoxel(const T & value)
  {
    if ( InsideVolume(m_pos) ) {
      SetVoxelRaw(value);
    }
  }


/// Sets the value at the current location.
  void SetVoxelRaw(const T & value)
  {
    *m_pVox = value;
  }


/// Returns the value at the current location: Safe version.
  const T  GetVoxel() const
  {
    if ( InsideVolume(m_pos) )
      return (GetVoxelRaw());
    else
      return (m_background);
  }


/// Returns the value at the current location: Unsafe version.
  const T  GetVoxelRaw() const
  {
    return *m_pVox;
  }


/// deprecated operator * , remains in API to support STL algorithms - use GetVoxel instead.
  const T  operator*() const
  {
    return (GetVoxelRaw());
  }


/// COMPARATORS
/// iterator inequality test by location & size.
  bool operator!=(const VolIterator & rhs)
  {
    return (!operator==(rhs));
  }


/// Iterator equality test by location & size.
  bool operator==(const VolIterator & rhs)
  {
    if ( rhs.GetDim() == m_dim && rhs.GetPos() == m_pos )
      return (true);
    else
      return (false);
  }


/// ABSOLUTE LOCATION FUNCTIONS

/// Moves the iterator to the given absolute position.
  void MoveAbs(const Point<uint2> & pos)
  {
    m_pos  = pos;
    m_pVox = m_pBaseAddr + GetOffset(pos);
  }

  protected:
  const uint4 GetOffset(const Point<uint2> & pos) const
  {
     return (pos.m_x + pos.m_y * m_yStep + m_pos.m_z * m_zStep);
  }
  public:

/// Peeks at the value at the location given relative to current location.
  const T & PeekRel(const Vector<int2> & delta) const
  {
    Point<uint2> pos(m_pos.m_x+delta.m_x, m_pos.m_y+delta.m_y, m_pos.m_z+delta.m_z);
    if(InsideVolume(pos))
      return *(m_pBaseAddr + GetOffset(pos));
    else
      return m_background;
  }


/// Moves the iterator relative to its current position.
  void MoveRel(const Vector<int2> & delta)
  {
    m_pos.m_x += delta.m_x; m_pos.m_y += delta.m_y; m_pos.m_z += delta.m_z;
    m_pVox = m_pBaseAddr + GetOffset(m_pos);
  }


/// INCREMENTAL PEEK AT SOME VOXEL
/// Peeks at the next voxel in the x direction w/out moving the iterator.
  const T  PeekNextX(const uint2 & delta = 1) const
  {
    // can't just compare pointer, may return wrong val
    if(InsideVolume(Point<uint2>(m_pos.m_x+delta, m_pos.m_y, m_pos.m_z)))
      return *(m_pVox + delta);
    else
      return m_background;
  }


/// Peeks at the next voxel in the y direction w/out moving the iterator.
  const T  PeekNextY(const uint2 & delta = 1) const
  {
    if(InsideVolume(Point<uint2>(m_pos.m_x, m_pos.m_y+delta, m_pos.m_z)))
      return *(m_pVox + (delta*m_yStep));
    else
      return m_background;
  }


/// Peeks at the next voxel in the z direction w/out moving the iterator.
  const T  PeekNextZ(const uint2 & delta = 1) const
  {
    if(InsideVolume(Point<uint2>(m_pos.m_x, m_pos.m_y, m_pos.m_z+delta)))
      return *(m_pVox + (delta*m_zStep));
    else
      return m_background;
  }


/// Peeks at the prev voxel in the x direction w/out moving the iterator.
  const T & PeekPrevX(const uint2 & delta = 1) const
  {
    if(InsideVolume(Point<uint2>(m_pos.m_x-delta, m_pos.m_y, m_pos.m_z)))
      return *(m_pVox - (delta));
    else
      return m_background;
  } 


/// Peeks at the prev voxel in the y direction w/out moving the iterator.
  const T & PeekPrevY(const uint2 & delta = 1) const
  {
    if(InsideVolume(Point<uint2>(m_pos.m_x, m_pos.m_y-delta, m_pos.m_z)))
      return *(m_pVox - (delta*m_yStep));
    else
      return m_background;
  }  


/// Peeks at the prev voxel in the z direction w/out moving the iterator.
  const T & PeekPrevZ(const uint2 & delta = 1) const
  {
     if(InsideVolume(Point<uint2>(m_pos.m_x, m_pos.m_y, m_pos.m_z-delta)))
      return *(m_pVox - (delta*m_zStep));
    else
      return m_background; 
  }


/// INCREMENTAL WALK TO NEAR BY VOXELS
/// Increments the iterator location in the x direction.
  
  void IncX(const uint2 & delta = 1)
  {
    m_pos.m_x += delta;
    m_pVox += delta;
  }


  void IncY(const uint2 & delta = 1)
  {
    m_pos.m_y += delta;
    m_pVox += delta * m_yStep;
  }


/// Increments the iterator location in the z direction.

  void IncZ(const uint2 & delta = 1)
  {
    m_pos.m_z += delta;
    m_pVox += delta * m_zStep;
  }


  void DecX(const uint2 & delta = 1)
  {
    m_pos.m_x -= delta;
    m_pVox -= delta;
  }

/// Decrements the iterator location in the y direction.
  void DecY(const uint2 & delta = 1)
  {
    m_pos.m_y -= delta;
    m_pVox -= delta * m_yStep;
  }

/// Decrements the iterator location in the z direction.
  
  void DecZ(const uint2 & delta = 1)
  {
    m_pos.m_z -= delta;
    m_pVox -= delta * m_zStep;
  }

/// STATUS REPORTS
/// Returns true if the iterator is inside the volume.
protected:
  bool InsideVolume(const Point<uint2> & pos) const
  {
    return( pos.m_x<m_dim.m_x && pos.m_y<m_dim.m_y && pos.m_z<m_dim.m_z );
  }
public:
  bool InsideVolume() const
  {
    return(InsideVolume(m_pos));
  }

/// Returns the current x,y,z position.

  const Point<uint2> & GetPos() const
  {
    return (m_pos);
  }


/// Get Volume Dimension included in API for Convenience for other iterators.

  const Triple<uint4> & GetDim() const
  {
    return (m_dim);
  }

};

#endif