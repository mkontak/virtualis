// $Id: Neighbor8Walker.inl,v 1.2 2007/04/09 20:43:51 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.net


/**
 * The constructor.
 * 
 * @param Iter          Type of the volume data iterator.
 * @param dataIter      The normal volume iterator that this iterator is based on.
 */
template <class Iter> 
Neighbor8Walker<Iter>::Neighbor8Walker(const Iter & dataIter)
  : m_dataIter(dataIter), m_neighborIndex(0) 
{
} // constructor


/**
 * Move the underlying volume iterator to the next neighbor position.
 * 
 * @param Iter   The normal volume iterator that this iterator is based on.
 */
template <class Iter> 
inline void Neighbor8Walker<Iter>::GotoNextNeighbor()
{
  // move along all 8 neighbors

  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  .  .  .    i--h--g    .  .  .     Y
  //             |     |                    Z
  //  .  .  .    j  z  f    .  .  .     | /
  //             |                      |/
  //  .  .  .    k--l--m    .  .  .     +--- X


  static const Vector<int2> m_toNextNeighbor[256] =
  {
    Vector<int2>(+1, 0, 0), // z, center -> f
    Vector<int2>( 0,+1, 0), // f -> g
    Vector<int2>(-1, 0, 0), // g -> h
    Vector<int2>(-1, 0, 0), // h -> i
    Vector<int2>( 0,-1, 0), // i -> j
    Vector<int2>( 0,-1, 0), // j -> k
    Vector<int2>(+1, 0, 0), // k -> l
    Vector<int2>(+1, 0, 0), // l -> m
    Vector<int2>(-1,+1, 0), // m -> z, center
      // the remainder is (0,0,0) by default constructor and
      // makes sure that calling ++ too often will stay at the
      // center and not cause segfaults
  };

  m_dataIter.MoveRel(m_toNextNeighbor[m_neighborIndex]);
  ++m_neighborIndex;    
} // GotoNextNeighbor


/**
 * Check whether the last neighbor has been reached.
 * 
 * @param Iter   The normal volume iterator that this iterator is based on.
 * @return true iff the last neighbor has been reached.
 */
template <class Iter> 
bool Neighbor8Walker<Iter>::End() const
{
  return( m_neighborIndex > m_uNumberOfNeighbors );
} // End


/**
 * Centers the neighbor walker at the given absolute position.
 * BE AWARE that it automatically places the internal position to the
 *          first neighbor of the position in the function argument.
 *
 * @param Iter          Type of the volume data iterator.
 * @param pos           Position of the voxel of whom the neighbors are to be traversed.
 */
template <class Iter>
void Neighbor8Walker<Iter>::CenterAt(const Point<uint2> & pos)
{
  // move to one below center, as this will be the first neighbor
  m_dataIter.MoveAbs(Point<uint2>(pos.m_x, pos.m_y, pos.m_z));
  m_neighborIndex = 0;
  GotoNextNeighbor();
} // CenterAt


/**
 * Get the the number of neighbours being traversed.
 * 
 * @param Iter   The normal volume iterator that this iterator is based on.
 * @return The value of the the number of neighbours being traversed
 */
template <class Iter> 
uint1 Neighbor8Walker<Iter>::GetNumberOfNeighbors() const
{
  return( m_uNumberOfNeighbors );
} // GetNumberOfNeighbors


/**
 * Get the maximum manhattan distance between center and any neighbor.
 * 
 * @param Iter   The normal volume iterator that this iterator is based on.
 * @return The value of the maximum manhattan distance between center and any neighbor
 */
template <class Iter> 
uint1 Neighbor8Walker<Iter>::GetMaxManhattanDistance() const
{
  return( m_uMaxManhattanDistance );
} // GetMaxManhattanDistance


// Revision 1.1  2001/04/14 04:36:22  jaddonisio
// initial version
//
// $Id: Neighbor8Walker.inl,v 1.2 2007/04/09 20:43:51 jmeade Exp $
