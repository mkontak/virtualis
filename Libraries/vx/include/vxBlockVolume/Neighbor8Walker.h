// $Id: Neighbor8Walker.h,v 1.3 2007/04/09 20:43:50 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.net

#ifndef Neighbor8Walker_h
#define Neighbor8Walker_h

// includes

/**
 * class description
 **/
template <class Iter>
class Neighbor8Walker
{
public: // functions

  /// constructor
  Neighbor8Walker(const Iter & dataIter);

  /// place the data iterator at the next neighbor
  inline void  GotoNextNeighbor();

  /// indicate having reached (or passed) the last neighbor
  inline bool  End() const;

  /// Centers the neighbor walker at the location.
  inline void CenterAt(const Point<uint2> & pos);

  /// return the number of neighbours being traversed.
  inline uint1 GetNumberOfNeighbors() const;

  /// return the maximum manhattan distance between center and any neighbor.
  inline uint1 GetMaxManhattanDistance() const;

  /// Test function to verify and demonstrate the class functionality.
  /// call as NeighborIterator<int,int,int>::Test(cout).
  static bool Test(std::ostream & os = std::cout);

  /// the low level volume data iterator
  Iter m_dataIter;

protected: /// data
  /// the index into the table of incremental difference vectors
  uint1 m_uNeighborIndex;

  /// number of neighbors
  enum
  {
    NUMBEROFNEIGHBORS=8    // const uint1 m_uNumberOfNeighbors=18
  };

  /// maximum manhattan distance between center and any neighbor
  enum
  {
    MAXMANHATTANDISTANCE=2  // const uint1 m_uMaxManhattanDistance=2
  };
}; // Neighbor18Walker


#include "Neighbor8Walker.inl"


#endif Neighbor8Walker_h

//
// Revision 1.1  2001/04/14 04:36:22  jaddonisio
// initial version
//
// $Id: Neighbor8Walker.h,v 1.3 2007/04/09 20:43:50 jmeade Exp $
