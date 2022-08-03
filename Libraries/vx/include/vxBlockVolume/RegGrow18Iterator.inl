// $Id: RegGrow18Iterator.inl,v 1.3 2007/04/10 01:23:32 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: 8-23-2000
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
RegGrow18Iterator<Iter, Vox, Decider>::RegGrow18Iterator( const Iter & dataIter, Decider decider )
: m_dataIter(dataIter), m_decider(decider),
m_flagVol(m_dataIter.GetDim(), 1, Triple<float4>(1.0,1.0,1.0),hdr::METER,false), // set m_background to false
m_flagIter(m_flagVol.GetVolIterator()), m_bGrowEnded(false),
m_peekFlagIter(m_flagVol.GetVolIterator()),
m_peekDataIter(dataIter)
{
  // check if m_volIterator is inside
  if ( !dataIter.InsideVolume() ) 
  {
    LogWrn("<RegGrow18Iterator::constructor> : seed iterator not in volume","RegGrow18Iterator<Iter, Vox, Decider>","RegGrow18Iterator");    
  }
  if ( !m_decider.IsPartOfRegion( m_dataIter ) ) 
  {
    LogWrn("<RegGrow18Iterator::constructor> : seed iterator not in region","RegGrow18Iterator<Iter, Vox, Decider>","RegGrow18Iterator");
  }

  m_posQ.push(m_dataIter.GetPos());
  m_flagVol.SetVoxel(m_dataIter.GetPos(), true);
  
  // increment once to get the start voxel off the queue,
  //   but also add the neighbors on the queue
  ++(*this);
} // Constructor()


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
void RegGrow18Iterator<Iter, Vox, Decider>::PossiblyAddToQueue()
{
  if( m_peekFlagIter.InsideVolume()    // still inside valid volume area
      && !m_peekFlagIter.GetVoxelRaw() // not yet flagged as processed/discovered
      && m_decider.IsPartOfRegion( m_peekDataIter.GetDataVolIter() ) ) 
  {
    m_posQ.push(m_peekDataIter.GetPos());
    m_peekFlagIter.SetVoxel(true);
  }
} // PossiblyAddToQueue()


/**
 * pops the next position of the queue, moved the data+flag iterators to there and schedules the neighbors for adition in to the queue
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 */
template <class Iter, class Vox, class Decider>
void RegGrow18Iterator<Iter, Vox, Decider>::operator++()
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
} // operator ++()


/**
 * Indicates when the region growing is finished.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @return true when the traversal is finished.
 */
template <class Iter, class Vox, class Decider>
bool RegGrow18Iterator<Iter, Vox, Decider>::End() const
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
 * @return the voxel value at the position.
 */
template <class Iter, class Vox, class Decider>
const Vox & RegGrow18Iterator<Iter, Vox, Decider>::GetVoxel() const
{
  return(m_dataIter.GetVoxel());
} // GetVoxel()


/**
 * Returns the voxel value at the current position (un-safe/fast version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @return the voxel value at the position.
 */
template <class Iter, class Vox, class Decider>
const Vox & RegGrow18Iterator<Iter, Vox, Decider>::GetVoxelRaw() const
{
  return(m_dataIter.GetVoxelRaw());
} // GetVoxelRaw()


  /**
 * Sets the voxel value at the current position (safe version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param value         the new voxel value.
 */
template <class Iter, class Vox, class Decider>
void RegGrow18Iterator<Iter, Vox, Decider>::SetVoxel(const Vox & value)
{
  m_dataIter.SetVoxel(value);
} // SetVoxel()


/**
 * Sets the voxel value at the current position (un-safe/fast version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @param value         the new voxel value.
 */
template <class Iter, class Vox, class Decider>
void RegGrow18Iterator<Iter, Vox, Decider>::SetVoxelRaw(const Vox & value)
{
  m_dataIter.SetVoxelRaw(value);
} // SetVoxelRaw()


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
const Point<uint2> & RegGrow18Iterator<Iter, Vox, Decider>::GetPos() const
{
  return(m_dataIter.GetPos());
} // GetPos()


/**
 * indicated the iterator position relative to the volume extend.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @return true if the iterator is inside the volume.
 */
template <class Iter, class Vox, class Decider>
bool RegGrow18Iterator<Iter, Vox, Decider>::InsideVolume() const
{
  return(m_dataIter.InsideVolume());
} // InsideVolume()


/**
 * Overloaded << Operator.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 */
template <class Iter, class Vox, class Decider >
std::ostream & operator<<(std::ostream & os, const RegGrow18Iterator<Iter, Vox, Decider> & iter)
{
  if ( iter.InsideVolume() ) 
  {
    os << "RegGrow18Iterator{Pos:"<<iter.GetPos() << "(inside)"  << "Val:"<<iter.GetVoxel() << "}" << flush;
  } 
  else 
  {
    os << "RegGrow18Iterator{Pos:"<<iter.GetPos() << "(outside)" << "Val:"<<iter.GetVoxel() << "}" << flush;
  }
  return( os );
} // operator<<()


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
bool RegGrow18Iterator<Iter, Vox, Decider>::TestByType(std::ostream & os) {
  bool ok(true);
  
  Vox label=(Vox)1;
  typedef BlockVolume<Vox> VolLayout;
  typedef Volume<VolLayout,Vox> Vol;
  Vol vol(Triple<uint4>(3,3,3), 8*sizeof(label));
  
  // turn on a 3D cross of voxels
  vol.SetVoxelRaw(Point<uint2>(1,1,1),label);
  // make an neighbor 18 iterator centered at voxel (1,1,1)
  typedef Neighbor18Iterator<VolLayout::VolIterator, Vox> N18Iter;
  N18Iter n18iter(vol.GetVolIterator(Point<uint2>(1,1,1)));
  for ( ; !n18iter.End();  ++n18iter ) 
  {
    n18iter.SetVoxel(label);
  }
  
  typedef VolLayout::VolIterator VolIter;
  VolIter iter(vol.GetVolIterator(Point<uint2>(1,1,1)));
  
  
  typedef DeciderThreshGreaterEqual<VolIter,Vox> MyDecider;
  MyDecider decider(label);
  
  // construct RegGrow18Iterator
  typedef RegGrow18Iterator< VolIter, Vox, MyDecider> RegGrow18Iter;
  RegGrow18Iter regGrow18Iter(iter, decider);
  
  os << "RegGrow18Iterator<\n  "
    << typeid(iter).name()<<",\n  "
    << typeid(label).name()<<",\n  "
    << typeid(decider).name()<<" >::TestByType()" << endl;
  // actually grow through region and update the label
  os << "Volume at start of region Grow:"<<endl;
  vol.PrintAllRaw();

#ifdef _WIN32
#pragma warning (disable : 4800)
#endif

  Vox newLabel = (Vox)label+1;
  for ( ; !regGrow18Iter.End(); ++regGrow18Iter ) 
  {
    regGrow18Iter.SetVoxelRaw(newLabel);
  }
  vol.PrintAllRaw();
  
  // verify the results by comparing to precomputed results
  ok &= (newLabel == vol.GetVoxelRaw(Point<uint2>(1,1,1)));
  n18iter.CenterAt(Point<uint2>(1,1,1));
  for ( ; !n18iter.End();  ++n18iter ) 
  {
    ok &= (newLabel == n18iter.GetVoxel());
  }

  if ( ok ) {
    os << "TEST SUCCEEDED :-)"<<endl;
  } 
  else 
  {
    os << "TEST FAILED :-(" << endl;
  }
  return(ok);
} // TestByType()

#undef FILE_REVISION

// $Log: RegGrow18Iterator.inl,v $
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
//    Rev 2.0   Sep 16 2001 23:48:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:50   ingmar
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
// Revision 1.2  2001/09/07 17:59:08  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:12:20  soeren
// renamed volume  directory
//
// Revision 1.8  2001/08/31 18:53:40  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.7  2001/08/07 17:03:40  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.6  2001/04/10 17:39:24  dmitry
// updated datatypes
//
// Revision 1.5  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.4  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.3  2000/11/21 23:43:47  ingmar
// disabled preformance warning
//
// Revision 1.2  2000/11/21 19:38:58  sarang
// gcc_bug_fix : header file was not included!!
//
// Revision 1.1  2000/11/21 03:15:20  ingmar
// first version of RegGrow18Iterator
//
//
// $Id: RegGrow18Iterator.inl,v 1.3 2007/04/10 01:23:32 jmeade Exp $

