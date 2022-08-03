// $Id: vxBlockVolumeRegionGrowIteratorHeap.h,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.com

#ifndef vxBlockVolumeRegionGrowIteratorHeap_h
#define vxBlockVolumeRegionGrowIteratorHeap_h


// includes
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxDecider.h"
#include "vxInserter.h"

               
/**
 * An iterator for linearized region growing.
 */
template <class VoxelType>
class vxBlockVolumeRegionGrowIteratorHeap
{
// functions
public:

  /// constructor
  vxBlockVolumeRegionGrowIteratorHeap(Vector3D<int4> seedPos, 
                                      vxBlockVolume<VoxelType> & dataVolume, 
                                      const vxDecider<VoxelType> & decider,
                                      vxInserter & inserter,
                                      const int4 backgroundCost,
                                      const vxComplex & seedCost, 
                                      vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum e3DNeighborhood = vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06, 
                                      vxBlockVolume<bool> * pMaskVolume = NULL);



  /// destructor
  ~vxBlockVolumeRegionGrowIteratorHeap();

  /// increments the iterator position in growing direction
  inline void operator ++ ();

  /// traversal has reached end
  inline bool IsAtEnd() const;

  /// traversal has reached end
  inline bool IsNotAtEnd() const;

  /// add seed point to heap
  bool AddSeedToHeap(const Vector3D<int4> & seedPos, const vxComplex & seedCost);

  /// get internal volume iterator
  inline const vxBlockVolumeIterator<VoxelType> & GetInternalVolIter() const { return m_dataIter; }

  /// returns the value at the current location
  inline const VoxelType & GetVoxel() const;

  /// sets the value at the current location
  inline void SetVoxel(const VoxelType & value);

  /// returns true if the iterator is inside the volume
  inline bool IsInsideVolume() const;

  /// convert to string
  std::string GetAsString() const;

  /// overloaded operator<<
  friend inline std::ostream & operator<<(std::ostream & os, const vxBlockVolumeRegionGrowIteratorHeap<VoxelType> & iter)
  {
    iter.GetAsString();
  }

  /// get position
  inline const Vector3D<int4> GetPos() const;

private:

  /// what to do after moving the iterators onto a potentially next region voxel
  inline void PossiblyAddToQueue(int4 iNeighborIndex);

// data
private:

  // class definition
  class HeapElement
  {
  // functions
  public:
    
    // constructor
    HeapElement(const uint8 & iPosOffset = 0, const vxComplex & cost=vxComplex(0,0)) 
      : m_iPosOffset(iPosOffset), m_cost(cost) {}

    /// operator()
    inline bool operator() (const HeapElement & a, const HeapElement & b) const
    {
      return a.m_cost.real() > b.m_cost.real();
    }
    
    /// operator==
    inline bool operator== (const HeapElement & rhs) const
    {
      return m_cost == rhs.m_cost && m_iPosOffset == rhs.m_iPosOffset;
    }
  
  // data
  public:

    /// position offset
    uint8 m_iPosOffset;

    /// complex number
    vxComplex  m_cost;
  }; // class HeapElement

  /// typedef CostPosHeap to be a priority queue of heap elements
  typedef std::priority_queue<HeapElement, std::vector<HeapElement>, HeapElement>  CostPosHeap;

  /// decides wether to (re)insert a region voxel into the heap
  vxInserter & m_inserter;

  /// volume of costs, positions of voxels in this volume will be inserted into the heap
  vxBlockVolume<vxComplex> m_costVol;      

  /// the volume iterator for the data
  vxBlockVolumeIterator<VoxelType> m_dataIter;

  /// the volume of already processed flags
  vxBlockVolume<bool> * m_pMaskVol;

  /// the volume of already processed flags
  vxBlockVolume<bool> & m_rMaskVol;

  /// the volume iterator for the mask volume
  vxBlockVolumeIterator<bool> m_maskIter;

  /// the volume iterator for the mask volume
  vxBlockVolumeIterator<vxComplex> m_costIter;

  /// heap keeping track of discovered but not processed voxels, sorted by cost
  CostPosHeap m_costPosHeap;  

  /// status of region grow completion (ended or not)
  bool m_bGrowEnded;                 

  /// temporary flag neighbor iterator used in ++() and PossiblyAddToQueue()
  vxBlockVolumeIteratorNeighbors<bool> m_peekMaskIter;

  /// temporary data neighbor iterator used in ++() and PossiblyAddToQueue()
  vxBlockVolumeIteratorNeighbors<VoxelType> m_peekDataIter;

  /// temporary data neighbor iterator used in ++() and PossiblyAddToQueue()
  vxBlockVolumeIteratorNeighbors<vxComplex> m_peekCostIter;

  /// the real data
  vxBlockVolume<VoxelType> & m_rDataVolume;

  /// decider
  const vxDecider<VoxelType> & m_rDecider;
}; // vxBlockVolumeRegionGrowIteratorHeap


#include "vxBlockVolumeRegionGrowIteratorHeap.inl"


#endif // vxBlockVolumeRegionGrowIteratorHeap_h


// $Log: vxBlockVolumeRegionGrowIteratorHeap.h,v $
// Revision 1.1.1.1.34.1  2010/07/16 20:31:28  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/05/16 11:48:13  michael
// code review
//
// Revision 1.6  2003/05/03 16:18:20  michael
// completed code review
//
// Revision 1.5  2002/09/17 17:17:55  dongqing
// Tiny bug fixing and comments after testing on the Skeletonator Module
//
// Revision 1.4  2002/08/28 18:37:41  ingmar
// changed vector position to block offset  as the element to be queued up
//
// Revision 1.3  2002/08/28 16:48:46  ingmar
// replaced cxCost by vxComplex
//
// Revision 1.2  2002/08/28 15:07:22  ingmar
// first successfull test
//
// Revision 1.1  2002/08/27 13:59:13  ingmar
// initial version (copy of non-heap region grow iterator
//
// Revision 1.6  2002/05/22 20:50:23  ingmar
// added sectioning for doxygen documentation
//
// Revision 1.5  2002/05/03 21:18:58  soeren
// added GetInternalVolIter
//
// Revision 1.4  2002/04/26 12:53:13  ingmar
// new deciders and region grow iterators based only on a single template parameter, the VoxelType
//
// Revision 1.3  2002/04/22 20:44:36  soeren
// added new constructor to regiongrowIterator
//
// Revision 1.2  2002/04/22 20:27:32  soeren
// removed old logs
//
// Revision 1.1  2002/04/22 19:54:01  soeren
// intial version of Regiongrow iterator and decider

// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeRegionGrowIteratorHeap.h,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
// $Id: vxBlockVolumeRegionGrowIteratorHeap.h,v 1.1.1.1.34.1 2010/07/16 20:31:28 dongqing Exp $
