// $Id: RegGrowHeapIterator.inl,v 1.4 2007/04/12 22:04:28 dongqing Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: jan-2001
// Author: Ingmar Bitter ingmar@viatronix.net

////////////////////////////////////////////////////////////////////////

#define FILE_REVISION "$Revision: 1.4 $"

/// CONSTRUCTOR & ASSIGNMENT

/**
 * Constructor.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 *
 * @param dataIter      iterator for the volume in which a region is supposed to be traversed.
 * @param decider       decides whether to include a voxel into the region
 * @param inserter      
 * @param backgroundCost 
 * @param seedCost      
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost>
RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>::
RegGrowHeapIterator(const Iter     & dataIter,
                    const Decider  & decider,
                          Inserter & inserter,
                    const Cost     & backgroundCost,
                    const Cost     & seedCost)
: m_dataIter(dataIter),
m_decider(decider),
m_inserter(inserter),
m_costVol(m_dataIter.GetDim(), 8*sizeof(Cost), Triple<float>(1.0,1.0,1.0),hdr::METER,CostVoxel(backgroundCost)),
m_costIter(m_costVol.GetVolIterator(dataIter.GetPos())),
m_peekDataIter(dataIter),
m_peekCostIter(m_costVol.GetVolIterator(dataIter.GetPos())),
m_bGrowEnded(false)
{
  // check if m_volIterator is inside
  if ( !dataIter.InsideVolume() ) 
  {
    LogWrn("<RegGrowHeapIterator::constructor> : seed iterator not in volume","RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>","RegGrowHeapIterator");
  }
  if ( !m_decider.IsPartOfRegion( m_dataIter ) ) 
  {
    LogWrn("<RegGrowHeapIterator::constructor> : seed iterator not in region","RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>","RegGrowHeapIterator");
  }
  m_costIter.SetVoxel(seedCost);
  Cost destCost = backgroundCost;
  if ( !m_inserter.NeedToUpdateHeap( seedCost, destCost, dataIter.GetPos(), 6) )
  { // updates cost
    LogWrn("<RegGrowHeapIterator::constructor> : no need to insert seed","RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>","RegGrowHeapIterator");
  }

  // put start voxel on heap
  m_costPosHeap.push(HeapElement(m_costIter.GetPos(), seedCost));

  // increment once to get the start voxel off the heap,
  //   but also add the neighbors on the heap
  ++(*this);
} // Constructor()


/**
 * add see to the region grow heap
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 *
 * @param seedCost      cost of the seed
 * @param seedPos       position of the seed
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost>
bool RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>::
AddSeedToHeap(const Point<uint2> & seedPos, const Cost & seedCost)
{
  Point<uint2> pos = m_dataIter.GetPos(); // back up current pos
  m_dataIter.MoveAbs(seedPos);
  if ( !m_dataIter.InsideVolume() ) 
  {
    LogWrn("<RegGrowHeapIterator::constructor> : seed iterator not in volume","RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>","AddSeedToHeap");
  }
  if ( !m_decider.IsPartOfRegion( m_dataIter ) ) 
  {
    LogWrn("<RegGrowHeapIterator::constructor> : seed iterator not in volume","RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>","AddSeedToHeap");
  }
  m_dataIter.MoveAbs(pos); // return to initial start position
  CostVoxel seedCostVoxel(seedCost, true);

  m_costVol.SetVoxelRaw(seedPos, seedCost);
  m_costPosHeap.push(HeapElement(seedPos, seedCost));
  return ( true );
} // AddSeedToHeap()


////////////////////////////////////////////////////////////////////////
/// VOLUME TRAVERSAL

/**
 * Does the book keeping to find out it the current voxel could be included in the region.
 * If the mechanical analysis comes out positive, then the decider is asked to make the final decision.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 *
 * @param uNeighborIndex   index of the neighbor
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost>
void RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>::
PossiblyAddToQueue(const uint4 uNeighborIndex)
{
  if ( m_peekCostIter.InsideVolume()    // still inside valid volume area
       && !m_peekCostIter.GetVoxelRaw().m_processed // not yet flagged as processed
       && m_decider.IsPartOfRegion( m_peekDataIter.GetDataVolIter() ) ) 
  {
    Cost  srcCost =     m_costIter.GetVoxelRaw().m_cost;
    Cost destCost = m_peekCostIter.GetVoxelRaw().m_cost;
    if ( m_inserter.NeedToUpdateHeap(srcCost, destCost, m_peekCostIter.GetPos(), uNeighborIndex) ) 
    {
      m_peekCostIter.SetVoxelRaw(destCost);

      m_costPosHeap.push(HeapElement(m_peekCostIter.GetPos(), destCost));
    }
  }
} // PossiblyAddToQueue()


/**
 * pops the next position of the queue, moved the data+flag iterators to there and schedules the neighbors for adition in to the queue
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost>
void RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>::operator++()
{
  // set m_bGrowEnded if there are no more voxels to process
  if ( m_costPosHeap.empty() )
  {
    m_bGrowEnded = true;
    return;
  }

  // purge the heap from entries that had already been processed before

  Point<uint2> nextPos = m_costPosHeap.top().m_pos;
  while (m_costVol.GetVoxelRaw(nextPos).m_processed) 
  {

    m_costPosHeap.pop();
    if ( m_costPosHeap.empty() )
    {
      m_bGrowEnded = true;
      return;
    }
    nextPos = m_costPosHeap.top().m_pos;
  }
  // get next voxel of region from heap
  m_costPosHeap.pop();
  m_dataIter.MoveAbs(nextPos);
  m_costIter.MoveAbs(nextPos);
  CostVoxel vc = m_costIter.GetVoxelRaw();
  vc.m_processed = true;
  m_costIter.SetVoxelRaw(vc);
  m_peekDataIter.CenterAt(nextPos);
  m_peekCostIter.CenterAt(nextPos);

  // peek towards next voxels that are potentially part of the region and
  // test if these voxels indeed belong to the region

  for ( uint4 k(0); !m_peekCostIter.End(); ++m_peekCostIter,++m_peekDataIter,++k ) 
  {
    PossiblyAddToQueue(k);
  }
} // operator ++()


/**
 * Indicates when the region growing is finished.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 * @return true when the traversal is finished.
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost>
bool RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>::End() const
{
  return(m_bGrowEnded);
} // End()


////////////////////////////////////////////////////////////////////////
/// GET & SET VOXELS

/**
 * Returns the voxel value at the current position (safe version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 * @return the voxel value at the position.
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost>
const Vox & RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>::GetVoxel() const
{
  return(m_dataIter.GetVoxel());
} // GetVoxel


/**
 * Returns the voxel value at the current position (un-safe/fast version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 * @return the voxel value at the position.
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost>
const Vox & RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>::GetVoxelRaw() const
{
  return(m_dataIter.GetVoxelRaw());
} // GetVoxelRaw


/**
 * Sets the voxel value at the current position (safe version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 * @param value         the new voxel value.
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost>
void RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>::SetVoxel(const Vox & value)
{
  m_dataIter.SetVoxel(value);
} // SetVoxel


/**
 * Sets the voxel value at the current position (un-safe/fast version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost>
void RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>::SetVoxelRaw(const Vox & value)
{
  m_dataIter.SetVoxelRaw(value);
} // SetVoxelRaw


////////////////////////////////////////////////////////////////////////
/// STATUS REPORTS

/**
 * Returns the current position of the iterator within the volume.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 * @return the x,y,z position in the volume.
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost>
const Point<uint2> & RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>::GetPos() const
{
  return(m_dataIter.GetPos());
} // GetPos


/**
 * indicated the iterator position relative to the volume extend.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 * @return true if the iterator is inside the volume.
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost>
bool RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>::InsideVolume() const
{
  return(m_dataIter.InsideVolume());
} // InsideVolume


/**
 * Overloaded << Operator.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 *
 * @param iter          heap iterator for growing
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost >
std::ostream & operator<<(std::ostream & os, const RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost> & iter)
{
  if ( iter.InsideVolume() ) 
  {
    os << "RegGrowHeapIterator{Pos:"<<iter.GetPos() << "(inside)"  << "Val:"<<iter.GetVoxel() << "}" << flush;
  } 
  else 
  {
    os << "RegGrowHeapIterator{Pos:"<<iter.GetPos() << "(outside)" << "Val:"<<iter.GetVoxel() << "}" << flush;
  }
  return( os );
} // operator<<


////////////////////////////////////////////////////////////////////////
/// VERIFICATION

/**
 * Test function to verify and demonstrate the class functionality.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param Inserter      Type of the inserter for assigning cost to grown voxel
 * @param Cost          Type of the assigned cost
 *
 * @param os            output stream to print progress info to
 * @return indicate whether the test was successful
 */
template <class Iter, class Vox, class Decider, class Inserter, class Cost>
bool RegGrowHeapIterator<Iter, Vox, Decider, Inserter, Cost>::TestByType(std::ostream & os) 
{
  bool ok(true);
/*
  Vox label=(Vox)1;
  Volume<BlockVolume<Vox>,Vox> vol(Triple<uint4>(3,3,3), 8*sizeof(label));

  // turn on a 3D cross of voxels
  vol.SetVoxelRaw(Point<uint2>(1,1,1),label);
  vol.SetVoxelRaw(Point<uint2>(0,1,1),label);
  vol.SetVoxelRaw(Point<uint2>(2,1,1),label);
  vol.SetVoxelRaw(Point<uint2>(1,0,1),label);
  vol.SetVoxelRaw(Point<uint2>(1,2,1),label);
  vol.SetVoxelRaw(Point<uint2>(1,1,0),label);
  vol.SetVoxelRaw(Point<uint2>(1,1,2),label);

  BlockVolume<Vox>::VolIterator
  iter(vol.GetVolIterator(Point<uint2>(1,1,1)));


  DeciderEqual<Neighbor6Iterator<BlockVolume<Vox>::VolIterator,Vox>,Vox>
  decider(label);

  // construct RegGrowHeapIterator
  RegGrowHeapIterator< BlockVolume<Vox>::VolIterator, Vox,
  DeciderEqual<Neighbor6Iterator<BlockVolume<Vox>::VolIterator,Vox>,Vox>, Cost>

  regGrowHeap6Iter(iter, decider, inserter);

  os << "RegGrowHeapIterator<\n  "
  << typeid(iter).name()<<",\n  "
  << typeid(label).name()<<",\n  "
  << typeid(decider).name()<<" >::TestByType()" << endl;
  // actually grow through region and update the label
  os << "Volume at start of region Grow:"<<endl;
  vol.PrintAllRaw();
  Vox newLabel = (Vox)0;
  for ( ; !regGrowHeap6Iter.End(); ++regGrowHeap6Iter ) {
    regGrowHeap6Iter.SetVoxelRaw(newLabel);
    vol.PrintAllRaw();
  }
  ok &= (newLabel == vol.GetVoxelRaw(Point<ushort>(1,1,1)));
  ok &= (newLabel == vol.GetVoxelRaw(Point<ushort>(0,1,1)));
  ok &= (newLabel == vol.GetVoxelRaw(Point<ushort>(2,1,1)));
  ok &= (newLabel == vol.GetVoxelRaw(Point<ushort>(1,0,1)));
  ok &= (newLabel == vol.GetVoxelRaw(Point<ushort>(1,2,1)));
  ok &= (newLabel == vol.GetVoxelRaw(Point<ushort>(1,1,0)));
  ok &= (newLabel == vol.GetVoxelRaw(Point<ushort>(1,1,2)));
  if ( ok ) {
    os << "TEST SUCCEEDED :-)"<<endl;
  } else {
    os << "TEST FAILED :-(" << endl;
  }
*/
  return(ok);
} // TestByType

#undef FILE_REVISION

// $Log: RegGrowHeapIterator.inl,v $
// Revision 1.4  2007/04/12 22:04:28  dongqing
// code review comments
//
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
// Revision 3.1  2001/10/26 02:21:07  tushar
// removed errors due to changes made to logger class, updating again
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
//    Rev 1.0   Sep 16 2001 17:35:58   ingmar
// Initial revision.
// Revision 1.3  2001/09/13 17:42:49  soeren
// added quotes to function name in logger macro
//
// Revision 1.2  2001/09/10 09:04:59  ingmar
// fixed use of logger defines
//
// Revision 1.1  2001/09/10 01:16:44  ingmar
// new dir structure
//
// Revision 1.3  2001/09/07 21:21:16  soeren
// added #undef FILE_VERSION in the end of each file
//
// Revision 1.2  2001/09/07 17:59:09  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.8  2001/08/31 18:53:41  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.7  2001/08/07 17:03:40  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.6  2001/06/04 01:12:31  ingmar
// towards coding standard compliance
//
// Revision 1.5  2001/04/10 17:38:07  dmitry
// updated datatypes
//
// Revision 1.4  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.3  2001/04/05 18:00:47  dmitry
// updated datatypes
//
// Revision 1.2  2001/04/03 08:04:28  ingmar
// in case the last voxel in the heap was already processed, it could segfault the code. now fixed
//
// Revision 1.1  2001/02/20 20:08:01  ingmar
// added skeleton of RegGrowHeapIterator
//
//
// $Id: RegGrowHeapIterator.inl,v 1.4 2007/04/12 22:04:28 dongqing Exp $

