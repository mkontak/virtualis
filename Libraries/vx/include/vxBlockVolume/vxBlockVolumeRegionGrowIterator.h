// $Id: vxBlockVolumeRegionGrowIterator.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

#ifndef vxBlockVolumeRegionGrowIterator_h
#define vxBlockVolumeRegionGrowIterator_h


// includes
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxDecider.h"


/**
 * An iterator for linearized region growing
 */
template <class VoxelType>
class vxBlockVolumeRegionGrowIterator
{
// functions
public:

  /// constructor
  vxBlockVolumeRegionGrowIterator(Vector3D<int4> seedPos, 
                                  vxBlockVolume<VoxelType> & dataVolume, 
                                  const vxDecider<VoxelType> & decider, 
                                  vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum e3DNeighborhood = vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06, 
                                  vxBlockVolume<bool> * pMaskVolume = NULL);



  /// destructor
  ~vxBlockVolumeRegionGrowIterator();

  /// increments the iterator position in growing direction
  inline void operator++();

  /// traversal has reached end
  inline bool IsAtEnd() const;

  /// traversal has reached end
  inline bool IsNotAtEnd() const;

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

  /// overloaded << operator
  friend inline std::ostream & operator<<(std::ostream & os, const vxBlockVolumeRegionGrowIterator<VoxelType> & iter)
  {
    iter.GetAsString();
  }

  /// returns position
  inline const Vector3D<int4> GetPos() const;

private:

  /// what to do after moving the iterators onto a potentially next region voxel
  inline void PossiblyAddToQueue();

// data
private:

  /// the volume iterator for the data
  vxBlockVolumeIterator<VoxelType> m_dataIter;

  /// the volume of already processed flags
  vxBlockVolume<bool> * m_pMaskVol;

  /// the volume of already processed flags
  vxBlockVolume<bool> & m_rMaskVol;

  /// the volume iterator for the mask volume
  vxBlockVolumeIterator<bool> m_maskIter;

  /// the queue of discovered but not processed voxels
  std::queue<Vector3D<int4> > m_posQueue;       

  /// status of region grow completion (ended or not)
  bool m_bGrowEnded;                 

  /// temporary flag neighbor iterator used in ++() and PossiblyAddToQueue()
  vxBlockVolumeIteratorNeighbors<bool> m_peekMaskIter;

  /// temporary data neighbor iterator used in ++() and PossiblyAddToQueue()
  vxBlockVolumeIteratorNeighbors<VoxelType> m_peekDataIter;

  /// real data
  vxBlockVolume<VoxelType> & m_rDataVolume;

  /// decider
  const vxDecider<VoxelType> & m_rDecider;
}; // vxBlockVolumeRegionGrowIterator


#include "vxBlockVolumeRegionGrowIterator.inl"


#endif // vxBlockVolumeRegionGrowIterator_h


// $Log: vxBlockVolumeRegionGrowIterator.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/05/03 16:18:20  michael
// completed code review
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

// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeRegionGrowIterator.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeRegionGrowIterator.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
