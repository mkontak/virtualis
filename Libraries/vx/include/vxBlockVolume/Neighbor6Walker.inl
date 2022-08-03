// $Id: Neighbor6Walker.inl,v 1.3 2007/04/09 20:39:52 jmeade Exp $
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
 * @param Iter       Type of the volume data iterator.
 * @param dataIter   The normal volume iterator that this iterator is based on.
 */
template <class Iter> 
Neighbor6Walker<Iter>::Neighbor6Walker(const Iter & dataIter)
  : m_dataIter(dataIter), m_uNeighborIndex(0) 
{
} // constructor


/**
 * Move the underlying volume iterator to the next neighbor position.
 * 
 * @param Iter   The normal volume iterator that this iterator is based on.
 */
template <class Iter> 
inline void Neighbor6Walker<Iter>::GotoNextNeighbor()
{
  // move along all 6 neighbors

  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  .  .  .    .  e  .    .  .  .     Y
  //                                        Z
  //  .  a  .    d  z  c    .  b  .     | /
  //                                    |/
  //  .  .  .    .  f  .    .  .  .     +--- X


  static const Vector<int2> m_toNextNeighbor[256] =
  {
    Vector<int2>( 0, 0,-1), // center -> a
    Vector<int2>( 0, 0,+2), // a -> b
    Vector<int2>(+1, 0,-1), // b -> c
    Vector<int2>(-2, 0, 0), // c
    Vector<int2>(+1,+1, 0), // d
    Vector<int2>( 0,-2, 0), // e
    Vector<int2>( 0,+1, 0), // f -> z (back to center position)
    // the remainder is (0,0,0) by default constructor and
    //   makes sure that calling ++ too often will stay at the
    //   center and not cause segfaults
  };

  m_dataIter.MoveRel(m_toNextNeighbor[m_uNeighborIndex]);
  ++m_uNeighborIndex;
} // GotoNextNeighbor


/**
 * Check whether the last neighbor has been reached.
 * 
 * @param Iter   The normal volume iterator that this iterator is based on.
 * @return true iff the last neighbor has been reached.
 */
template <class Iter> 
bool Neighbor6Walker<Iter>::End() const
{
  return( m_uNeighborIndex > NUMBEROFNEIGHBORS );
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
void Neighbor6Walker<Iter>::CenterAt(const Point<uint2> & pos)
{
  // move to one below center, as this will be the first neighbor
  m_dataIter.MoveAbs(Point<uint2>(pos.m_x, pos.m_y, pos.m_z));
  m_uNeighborIndex = 0;
  GotoNextNeighbor();
} // CenterAt


/**
 * Get the the number of neighbours being traversed.
 * 
 * @param Iter   The normal volume iterator that this iterator is based on.
 * @return The value of the the number of neighbours being traversed
 */
template <class Iter> 
uint1 Neighbor6Walker<Iter>::GetNumberOfNeighbors() const
{
  return( NUMBEROFNEIGHBORS );
} // GetNumberOfNeighbors


/**
 * Get the maximum manhattan distance between center and any neighbor.
 * 
 * @param Iter   The normal volume iterator that this iterator is based on.
 * @return The value of the maximum manhattan distance between center and any neighbor
 */
template <class Iter> 
uint1 Neighbor6Walker<Iter>::GetMaxManhattanDistance() const
{
  return( MAXMANHATTANDISTANCE );
} // GetMaxManhattanDistance


// $Log: Neighbor6Walker.inl,v $
// Revision 1.3  2007/04/09 20:39:52  jmeade
// code review.
//
// Revision 1.2  2007/03/06 18:58:23  dongqing
// code review
//
// Revision 1.1  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.0  2001/10/14 23:02:23  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:44   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:44  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:20  soeren
// renamed volume  directory
//
// Revision 1.5  2001/08/14 05:23:04  ingmar
// removed basic volIter from NeighborIter, it is now only in NeighborWalker
//
// Revision 1.4  2001/04/28 01:23:50  ingmar
// added CenterAt()
//
// Revision 1.3  2001/04/19 01:50:13  ingmar
// added test and comments
//
// Revision 1.2  2001/04/18 03:02:43  ingmar
// updated header info
//
// Revision 1.1  2001/04/14 04:36:22  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/Neighbor6Walker.inl,v 1.3 2007/04/09 20:39:52 jmeade Exp $
// $Id: Neighbor6Walker.inl,v 1.3 2007/04/09 20:39:52 jmeade Exp $
