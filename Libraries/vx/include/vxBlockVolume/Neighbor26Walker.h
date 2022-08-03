// $Id: Neighbor26Walker.h,v 1.3 2007/04/09 20:43:02 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.net


#ifndef Neighbor26Walker_h
#define Neighbor26Walker_h

/**
 * class description
 **/
template <class Iter>
class Neighbor26Walker 
{
public:

  /// constructor
  Neighbor26Walker(const Iter & dataIter);

  /// place the data iterator at teh next neighbor
  inline void  GotoNextNeighbor();

  /// indicate having reached (or passed) the last neighbor
  inline bool  End() const;

  /// Centers the neighbor walker at the location.
  inline void CenterAt(const Point<uint2> & pos);

  /// return the number of neighbours being traversed.
  inline uint1 GetNumberOfNeighbors() const;

  /// return the maximum mahhattan distance between center and any neighbor.
  inline uint1 GetMaxManhattanDistance() const;

  /// Test function to verify and demonstrate the class functionality.
  /// call as NeighborIterator<int,int,int>::Test(cout).
  static bool Test (std::ostream & os = std::cout);

  /// the low level volume data iterator
  Iter m_dataIter;

protected: /// data
  /// the index into the table of incremental difference vectors
  uint1 m_uNeighborIndex;

  /// number of neighbors
  enum
  {
    NUMBEROFNEIGHBORS=26 // const uint1 m_uNumberOfNeighbors=26
  };

  /// maximum mahhattan distance between center and any neighbor
  enum
  {
    MAXMANHATTANDISTANCE=3 // const uint1 m_uMaxManhattanDistance=3
  };
}; // Neighbor26Walker


#include "Neighbor26Walker.inl"


#endif Neighbor26Walker_h

// $Log: Neighbor26Walker.h,v $
// Revision 1.3  2007/04/09 20:43:02  jmeade
// code review.
//
// Revision 1.2  2007/03/06 18:58:22  dongqing
// code review
//
// Revision 1.1  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.2  2003/01/22 23:19:00  ingmar
// added missing includes
//
// Revision 3.1  2001/12/24 17:56:21  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:23  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:40   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:45  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:19  soeren
// renamed volume  directory
//
// Revision 1.5  2001/08/14 05:23:04  ingmar
// removed basic volIter from NeighborIter, it is now only in NeighborWalker
//
// Revision 1.4  2001/04/28 01:24:25  ingmar
// added CenterAt()
//
// Revision 1.3  2001/04/19 01:50:13  ingmar
// added test and comments
//
// Revision 1.2  2001/04/17 15:45:17  dmitry
// Inheritance class added for Documentation
//
// Revision 1.1  2001/04/14 04:36:22  ingmar
// initial version
//
// $Id: Neighbor26Walker.h,v 1.3 2007/04/09 20:43:02 jmeade Exp $
