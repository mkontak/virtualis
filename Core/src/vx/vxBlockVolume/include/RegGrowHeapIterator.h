// $Id: RegGrowHeapIterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.net


#ifndef RegGrowHeapIterator_h
#define RegGrowHeapIterator_h

// includes
#include "Decider.h"
#include "Neighbor6Iterator.h"

/**
 * An iterator that linearizes 6-connected region growing.
 * $Id: RegGrowHeapIterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a> 
*/
template <class DataIter, class Vox, class Decider, class Inserter, class Cost>
class RegGrowHeapIterator 
{
public:

  /// CONSTRUCTOR & ASSIGNMENT

  /// Constructor called by clients
  RegGrowHeapIterator(const DataIter & dataIter,
                      const Decider  & decider,
                            Inserter & inserter,
                      const Cost     & backgroundCost,
                      const Cost     & seedCost);
  /// ????
  inline bool AddSeedToHeap(const Point<uint2> & seedPos, const Cost & seedCost);

  /// VOLUME TRAVERSAL

  /// Increments the iterator position in growing direction.
  inline void operator ++();
  /// is true when the traversal is finished.
  inline bool End() const;


  /// GET & SET VOXELS

  /// Returns the value at the current location: Safe version.
  inline const Vox & GetVoxel() const;
  /// Returns the value at the current location: Unsafe version.
  inline const Vox & GetVoxelRaw() const;
  /// Sets the value at the current location: Safe version.
  inline void SetVoxel(const Vox & value);
  /// Sets the value at the current location: Unsafe version.
  inline void SetVoxelRaw(const Vox & value);

  
  /// STATUS REPORTS

  /// Returns the current x,y,z position.
  inline const Point<uint2> & GetPos() const;
  /// Returns true if the iterator is inside the volume.
  inline bool InsideVolume() const;


  /// VERIFICATION

  /// Short function to demonstrate the class functionality.
  friend void RegGrowHeapIteratorDemo (std::ostream & os);
  /// Test function to verify the class functionality.
  friend bool RegGrowHeapIteratorTest (std::ostream & os);
  /// Test function to verify just one particular template instance
  static bool TestByType(std::ostream & os = std::cout);

protected:

  /// what to do after moving the iterators onto a potentially next region voxel
  inline void PossiblyAddToQueue(const uint4 uNeighborIndex);

protected: // data

  /**
   * ?????
   **/
  class CostVoxel : Sec_31_volIters 
  { // voxels of the block volume
  public:
    /// constructor
    CostVoxel(Cost cost, bool bProcessed=false)     : m_cost(cost), m_processed(processed) { }
    /// constructor
    CostVoxel(const int4  iCost = 0) : m_cost(iCost), m_processed(false) { }
    /// equality operator
    inline bool operator== (const CostVoxel & rhs) const { return(m_cost == rhs.m_cost && m_processed == rhs.m_processed);}
    /// equality operator
    inline bool operator!= (const CostVoxel & rhs) const { return(!(*this == rhs));}
    /// data
    Cost m_cost;
    /// whether or not voxel is processed
    bool m_bProcessed;
  }; // class CostVoxel

  /**
   * ?????
   **/
  class HeapElement  : Sec_31_volIters 
  {
  public:
    /// constructor
    HeapElement(const Point<uint2> & pos = Point<uint2>(0,0,0), const Cost & cost=0) : m_pos(pos), m_cost(cost)  {}
    /// comparison operator
    inline bool operator() (const HeapElement & a, const HeapElement & b) const { return(a.m_cost > b.m_cost);}
    /// equality operator
    inline bool operator== (const HeapElement & rhs) const { return(m_cost == rhs.m_cost && m_pos == rhs.m_pos);}
  public:
    /// ?????
    Point<uint2> m_pos;
    /// ??????
    Cost          m_cost;
  }; // class HeapElement

  /// iterator typedefs
  typedef Volume<BlockVolume<CostVoxel>, CostVoxel> CostVol;
  typedef typename BlockVolume<CostVoxel>::VolIterator       CostIter;
  typedef std::priority_queue<HeapElement,
	  std::vector<HeapElement>, HeapElement>        CostPosHeap;
  typedef Neighbor6Iterator<DataIter, Vox>          N6DataIter;
  typedef Neighbor6Iterator<CostIter, CostVoxel>    N6CostIter;

  /// data iterator
  DataIter    m_dataIter;                   
  /// decides whether to include a voxel in the region
  Decider     m_decider;                 
  /// decides whether to (re)insert a region voxel into the heap
  Inserter  & m_inserter;     
  /// volume of costs, positions of voxels in this volume will be inserted into the heap
  CostVol     m_costVol;      
  /// iterator for the volume of costs
  CostIter    m_costIter;     
  /// heap keeping track of discovered but not processed voxels, sorted by cost
  CostPosHeap m_costPosHeap;  
  /// temporary data iterator used in ++() and PossiblyAddToQueue()
  N6DataIter  m_peekDataIter; 
  /// temporary flag iterator used in ++() and PossiblyAddToQueue()
  N6CostIter  m_peekCostIter; 
  /// state of region grow (ended or not)
  bool  m_bGrowEnded;                 
  
}; // class RegGrowHeapIterator


/// Short function to demonstrate the class functionality.
void RegGrowHeapIteratorDemo (std::ostream & os = std::cout);

/// Test function to verify the class functionality.
bool RegGrowHeapIteratorTest (std::ostream & os = std::cout);



/// Overloaded << Operator.
template <class DataIter, class Vox, class Decider, class Inserter, class Cost >
inline std::ostream & operator<<(std::ostream & os, const RegGrowHeapIterator<DataIter, Vox, Decider, Inserter, Cost> & iter);

// typedef is more difficult due to decider, may need macro ??


#include "RegGrowHeapIterator.inl"

#endif



// $Log: RegGrowHeapIterator.h,v $
// Revision 1.3  2007/04/10 01:23:32  jmeade
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
// Revision 3.3.12.1  2004/03/22 21:46:19  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.3  2002/03/06 01:24:53  ingmar
// added != operator, so that it compiles with the new Block Class
//
// Revision 3.2  2001/12/26 16:15:54  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.1  2001/12/24 17:51:15  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:24  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:22   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:56   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:44  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.9  2001/06/04 01:12:21  ingmar
// towards coding standard compliance
//
// Revision 1.8  2001/04/12 18:10:50  dmitry
// modified base class for documentation
//
// Revision 1.7  2001/04/10 17:38:07  dmitry
// updated datatypes
//
// Revision 1.6  2001/04/08 22:04:17  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.5  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.4  2001/04/05 03:41:35  ingmar
// added documentation classes
//
// Revision 1.3  2001/04/03 05:46:59  soeren
// added documentation
//
// Revision 1.2  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.1  2001/02/20 20:08:01  ingmar
// added skeleton of RegGrowHeapIterator
//
//
// $Id: RegGrowHeapIterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $
