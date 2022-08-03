// $Id: RegGrow6Iterator.inl,v 1.3 2007/04/10 01:23:32 jmeade Exp $
//
// Copyright(c) 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.net

#define FILE_REVISION "$Revision: 1.3 $"

////////////////////////////////////////////////////////////////////////
/// CONSTRUCTOR & ASSIGNMENT

/**
 * Constructor.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param dataIter      iterator for the volume in which a region is supposed to be traversed.
 * @param decider       decides whether to include a voxel into the region
 */
template <class Iter, class Vox, class Decider>
RegGrow6Iterator<Iter, Vox, Decider>::RegGrow6Iterator(const Iter & dataIter, Decider decider)
: m_dataIter(dataIter), m_decider(decider),
m_flagVol(m_dataIter.GetDim(), 1, Triple<float4>(1.0,1.0,1.0),hdr::METER,false), // set m_background to false
m_flagIter(m_flagVol.GetVolIterator()), m_bGrowEnded(false),
m_peekFlagIter(m_flagVol.GetVolIterator()),
m_peekDataIter(dataIter)
{
  // check if m_volIterator is inside
  if ( !dataIter.InsideVolume() ) 
  {
    LogWrn("seed iterator not in volume","RegGrow6Iterator","RegGrow6Iterator");
  }
  if ( !m_decider.IsPartOfRegion( m_dataIter ) ) 
  {
    LogWrn("seed iterator not in region","RegGrow6Iterator","RegGrow6Iterator");
  }
  m_posQ.push(m_dataIter.GetPos());
  m_flagVol.SetVoxel(m_dataIter.GetPos(), true);
  
  // increment once to get the start voxel off the queue,
  //   but also add the neighbors on the queue
  ++(*this);
} // Constructor


////////////////////////////////////////////////////////////////////////
/// VOLUME TRAVERSAL

/**
 * Does the book keeping to find out it the current voxel could be included in the region.
 * If the mechanical analysis comes out positive, then the decider is asked to make the final decision.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 */
template <class Iter, class Vox, class Decider>
void RegGrow6Iterator<Iter, Vox, Decider>::PossiblyAddToQueue()
{
  if ( m_peekFlagIter.InsideVolume()    // still inside valid volume area
    && !m_peekFlagIter.GetVoxelRaw() // not yet flagged as processed/discovered
    && m_decider.IsPartOfRegion( m_peekDataIter.GetDataVolIter() ) ) 
  {
    m_posQ.push(m_peekDataIter.GetPos());
    m_peekFlagIter.SetVoxel(true);
  }
} // PossiblyAddToQueue


/**
 * pops the next position of the queue, moved the data+flag iterators to there and schedules the neighbors for adition in to the queue
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 */
template <class Iter, class Vox, class Decider>
void RegGrow6Iterator<Iter, Vox, Decider>::operator++()
{
  // set m_bGrowEnded if there are no more voxels to process
  if ( m_posQ.empty() ) 
  {
    m_bGrowEnded = true;
    return;
  }
  
  // get next voxel of region from queue
  Point<uint2> nextPos = m_posQ.front();
  m_dataIter.MoveAbs(nextPos);
  m_flagIter.MoveAbs(nextPos);
  m_posQ.pop();
  m_peekDataIter.CenterAt(nextPos);
  m_peekFlagIter.CenterAt(nextPos);
  
  // peek towards next voxels that are potentially part of the region and
  // test if these voxels indeed belong to the region
  
  for ( ; !m_peekFlagIter.End(); ++m_peekFlagIter,++m_peekDataIter ) 
  {
    PossiblyAddToQueue();
  }
} // operator ++


/**
 * Indicates when the region growing is finished.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @return true when the traversal is finished.
 */
template <class Iter, class Vox, class Decider>
bool RegGrow6Iterator<Iter, Vox, Decider>::End() const
{
  return(m_bGrowEnded);
} // End


////////////////////////////////////////////////////////////////////////
/// GET & SET VOXELS

/**
 * Returns the voxel value at the current position (safe version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @return the voxel value at the position.
 */
template <class Iter, class Vox, class Decider>
const Vox & RegGrow6Iterator<Iter, Vox, Decider>::GetVoxel() const
{
  return(m_dataIter.GetVoxel());
} // GetVoxel


/**
 * Returns the voxel value at the current position (un-safe/fast version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @return the voxel value at the position.
 */
template <class Iter, class Vox, class Decider>
const Vox & RegGrow6Iterator<Iter, Vox, Decider>::GetVoxelRaw() const
{
  return(m_dataIter.GetVoxelRaw());
} // GetVoxelRaw


/**
 * Sets the voxel value at the current position (safe version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param value         the new voxel value.
 */
template <class Iter, class Vox, class Decider>
void RegGrow6Iterator<Iter, Vox, Decider>::SetVoxel(const Vox & value)
{
  m_dataIter.SetVoxel(value);
} // SetVoxel


 /**
 * Sets the voxel value at the current position (un-safe/fast version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param value         the new voxel value.
 */
template <class Iter, class Vox, class Decider>
void RegGrow6Iterator<Iter, Vox, Decider>::SetVoxelRaw(const Vox & value)
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
 * @return the x,y,z position in the volume.
 */
template <class Iter, class Vox, class Decider>
const Point<uint2> & RegGrow6Iterator<Iter, Vox, Decider>::GetPos() const
{
  return(m_dataIter.GetPos());
} // GetPos


/**
 * indicated the iterator position relative to the volume extend.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @return true if the iterator is inside the volume.
 */
template <class Iter, class Vox, class Decider>
bool RegGrow6Iterator<Iter, Vox, Decider>::InsideVolume() const
{
  return(m_dataIter.InsideVolume());
} // InsideVolume


/**
 * Overloaded << Operator.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 */
template <class Iter, class Vox, class Decider >
std::ostream & operator<<(std::ostream & os, const RegGrow6Iterator<Iter, Vox, Decider> & iter)
{
  if ( iter.InsideVolume() ) 
  {
    os << "RegGrow6Iterator{Pos:"<<iter.GetPos() << "(inside)"  << "Val:"<<iter.GetVoxel() << "}" << flush;
  } 
  else 
  {
    os << "RegGrow6Iterator{Pos:"<<iter.GetPos() << "(outside)" << "Val:"<<iter.GetVoxel() << "}" << flush;
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
 * @param os            output stream to print progress info to
 * @return indicate whether the test was successful
 */
template <class Iter, class Vox, class Decider>
bool RegGrow6Iterator<Iter, Vox, Decider>::TestByType(std::ostream & os) 
{
  bool ok(true);
  
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
  
  // construct RegGrow6Iterator
  RegGrow6Iterator< BlockVolume<Vox>::VolIterator, Vox,
    DeciderEqual<Neighbor6Iterator<BlockVolume<Vox>::VolIterator,Vox>,Vox> >
    
    regGrow6Iter(iter, decider);
  
  os << "RegGrow6Iterator<\n  "
    << typeid(iter).name()<<",\n  "
    << typeid(label).name()<<",\n  "
    << typeid(decider).name()<<" >::TestByType()" << endl;
  // actually grow through region and update the label
  os << "Volume at start of region Grow:"<<endl;
  vol.PrintAllRaw();
  Vox newLabel = (Vox)0;
  for ( ; !regGrow6Iter.End(); ++regGrow6Iter ) {
    regGrow6Iter.SetVoxelRaw(newLabel);
    vol.PrintAllRaw();
  }
  ok &= (newLabel == vol.GetVoxelRaw(Point<uint2>(1,1,1)));
  ok &= (newLabel == vol.GetVoxelRaw(Point<uint2>(0,1,1)));
  ok &= (newLabel == vol.GetVoxelRaw(Point<uint2>(2,1,1)));
  ok &= (newLabel == vol.GetVoxelRaw(Point<uint2>(1,0,1)));
  ok &= (newLabel == vol.GetVoxelRaw(Point<uint2>(1,2,1)));
  ok &= (newLabel == vol.GetVoxelRaw(Point<uint2>(1,1,0)));
  ok &= (newLabel == vol.GetVoxelRaw(Point<uint2>(1,1,2)));
  if ( ok ) 
  {
    os << "TEST SUCCEEDED :-)"<<endl;
  } 
  else 
  {
    os << "TEST FAILED :-(" << endl;
  }
  return(ok);
} // TestByType

#undef FILE_REVISION

// $Log: RegGrow6Iterator.inl,v $
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
// Revision 3.0  2001/10/14 23:02:23  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:20   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:54   ingmar
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
// Revision 1.4  2001/09/08 22:25:21  ingmar
// code beautify
//
// Revision 1.3  2001/09/07 21:21:16  soeren
// added #undef FILE_VERSION in the end of each file
//
// Revision 1.2  2001/09/07 17:59:08  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.31  2001/08/31 18:53:41  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.30  2001/08/07 17:03:40  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.29  2001/04/28 01:21:08  ingmar
// updated header
//
// Revision 1.28  2001/04/10 17:38:27  dmitry
// updated datatypes
//
// Revision 1.27  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.26  2001/04/05 18:00:47  dmitry
// updated datatypes
//
// Revision 1.25  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.24  2000/11/19 00:41:34  ingmar
// the decider now takes a normal VolIterator
//
// Revision 1.23  2000/11/16 17:19:49  dave
// added native BlockVolume VolIterator
//
// Revision 1.22  2000/11/15 21:42:01  lihong
// added flaging in the start voxel
//
// Revision 1.21  2000/11/15 06:05:03  ingmar
// uups, deleted too much code by accident
//
// Revision 1.20  2000/11/15 06:00:21  ingmar
// better accumulation of ok
//
// Revision 1.19  2000/11/15 03:09:49  ingmar
// based the region growing on neighbor6Iterator
//
// Revision 1.18  2000/11/10 17:34:05  dave
// impl. API changes for VolIterator
//
// Revision 1.17  2000/11/04 20:55:41  ingmar
// code beautify
//
// Revision 1.16  2000/11/04 03:08:56  ingmar
// Streamlined VolIterator
//
// Revision 1.15  2000/11/03 17:02:26  dave
// converted back
//
// Revision 1.14  2000/11/02 19:48:09  dave
// Dimensions changed to Triple<uint> from uint
//
// Revision 1.13  2000/11/01 21:11:29  dave
// LZO default, GetDataType() fully removed, re-order Vol. declaration
//
// Revision 1.12  2000/11/01 17:18:33  ingmar
// added operator <<
//
// Revision 1.11  2000/11/01 16:30:12  lihong
// debugged surface crawling decider + itreator
//
// Revision 1.10  2000/10/30 18:08:02  ingmar
// added 26 connected region grow iterator to volume module
//
// Revision 1.9  2000/10/26 17:06:27  ingmar
// code cleanup and better documentaion
//
// Revision 1.8  2000/10/26 03:19:42  ingmar
// code cleanup and addition of test function skeleton
//
// Revision 1.7  2000/10/25 15:22:11  ingmar
// reformat
//
// Revision 1.6  2000/10/25 01:35:25  ingmar
// removed VxIterator and improved VxVolIterator and RegGrow6Iterator
//
// Revision 1.5  2000/10/23 20:18:08  ingmar
// removed VxDilator and added SpiralIterator
//
// Revision 1.4  2000/10/22 00:24:23  dave
// better iterators
//
// Revision 1.3  2000/10/21 06:06:03  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.2  2000/10/19 04:01:04  ingmar
// fixed 6-connected region grow and related VolIterator bugs
//
// Revision 1.1.1.1  2000/08/11 22:15:52  ingmar
// unit tests complete
// Revision 1.1.1.1  2000/08/11 22:15:52  ingmar
// unit tests complete
//
//
// 6     8/25/00 5:18p Jay
// towards a wroking version of a region grow iterator
//
// 5     8/25/00 11:31a Ingmar
// added idea of how to grow towards 6 neighbors
//
// 4     8/24/00 5:00p Ingmar
// region grow now with generic template decider
//
// 3     8/24/00 3:13p Ingmar
// now first procedual end condition working
//
// 2     8/24/00 12:53a Ingmar
// new 6 connected region grow iterator
//
// 1     8/23/00 2:28p Ingmar
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/RegGrow6Iterator.inl,v 1.3 2007/04/10 01:23:32 jmeade Exp $
// $Id: RegGrow6Iterator.inl,v 1.3 2007/04/10 01:23:32 jmeade Exp $

