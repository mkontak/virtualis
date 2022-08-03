// $Id: RegGrow26Iterator.inl,v 1.3 2007/04/10 01:23:32 jmeade Exp $
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
RegGrow26Iterator<Iter, Vox, Decider>::RegGrow26Iterator( const Iter & dataIter, const Decider & decider )
: m_dataIter(dataIter), m_decider(decider),
m_flagVol(m_dataIter.GetDim(), 1, Triple<float4>(1.0,1.0,1.0), hdr::METER, false),
m_flagIter(m_flagVol.GetVolIterator()), m_bGrowEnded(false),
m_peekFlagIter(m_flagVol.GetVolIterator()),
m_peekDataIter(dataIter)
{
  // check if m_volIterator is inside
  if ( !dataIter.InsideVolume() ) 
  {
    LogWrn("<RegGrow26Iterator::constructor> : seed iterator not in volume","RegGrow26Iterator<Iter, Vox, Decider>","RegGrow26Iterator");
  }
  if ( !m_decider.IsPartOfRegion( m_dataIter ) ) 
  {
    LogWrn("<RegGrow26Iterator::constructor> : seed iterator not in region","RegGrow26Iterator<Iter, Vox, Decider>","RegGrow26Iterator");
  }

  m_posQ.push(m_dataIter.GetPos());
  m_flagVol.SetVoxel(m_dataIter.GetPos(), true);
  
  // increment once to get the start voxel off the queue,
  //   but also add the neighbors on the queue
  ++(*this);
} // Constructor


/**
 * Does the book keeping to find out it the current voxel could be included in the region.  If the mechanical analysis comes out positive, then the decider is asked to make the final decision.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 */
template <class Iter, class Vox, class Decider>
void RegGrow26Iterator<Iter, Vox, Decider>::PossiblyAddToQueue()
{
  if ( m_peekFlagIter.InsideVolume()   // still inside valid volume area
    && !m_peekFlagIter.GetVoxelRaw() // not yet flagged as processed/discovered
    && m_decider.IsPartOfRegion( m_peekDataIter ) ) 
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
void RegGrow26Iterator<Iter, Vox, Decider>::operator++()
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
  m_peekDataIter.MoveAbs(m_dataIter.GetPos());
  m_peekFlagIter.MoveAbs(m_flagIter.GetPos());
  
  // peek towards next voxels that are potentially part of the region and
  // test if these voxels indeed belong to the region
  
  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  i--h--g    j  q--p    s--r  y     Y
  //        |    |     |    |    /|         Z
  //  b--a  f    k     o    t  z  x     | /
  //  |     |    |     |    |     |     |/
  //  c--d--e    l--m--n    u--v--w     +--- X
  
  // bottom 3x3 face
  m_peekDataIter.DecZ();  m_peekFlagIter.DecZ();  PossiblyAddToQueue(); // a
  m_peekDataIter.DecX();  m_peekFlagIter.DecX();  PossiblyAddToQueue(); // b
  m_peekDataIter.DecY();  m_peekFlagIter.DecY();  PossiblyAddToQueue(); // c
  m_peekDataIter.IncX();  m_peekFlagIter.IncX();  PossiblyAddToQueue(); // d
  m_peekDataIter.IncX();  m_peekFlagIter.IncX();  PossiblyAddToQueue(); // e
  m_peekDataIter.IncY();  m_peekFlagIter.IncY();  PossiblyAddToQueue(); // f
  m_peekDataIter.IncY();  m_peekFlagIter.IncY();  PossiblyAddToQueue(); // g
  m_peekDataIter.DecX();  m_peekFlagIter.DecX();  PossiblyAddToQueue(); // h
  m_peekDataIter.DecX();  m_peekFlagIter.DecX();  PossiblyAddToQueue(); // i
  
  // middle 3x3 ring
  m_peekDataIter.IncZ();  m_peekFlagIter.IncZ();  PossiblyAddToQueue(); // j
  m_peekDataIter.DecY();  m_peekFlagIter.DecY();  PossiblyAddToQueue(); // k
  m_peekDataIter.DecY();  m_peekFlagIter.DecY();  PossiblyAddToQueue(); // l
  m_peekDataIter.IncX();  m_peekFlagIter.IncX();  PossiblyAddToQueue(); // m
  m_peekDataIter.IncX();  m_peekFlagIter.IncX();  PossiblyAddToQueue(); // n
  m_peekDataIter.IncY();  m_peekFlagIter.IncY();  PossiblyAddToQueue(); // o
  m_peekDataIter.IncY();  m_peekFlagIter.IncY();  PossiblyAddToQueue(); // p
  m_peekDataIter.DecX();  m_peekFlagIter.DecX();  PossiblyAddToQueue(); // q
  
  // top 3x3 face
  m_peekDataIter.IncZ();  m_peekFlagIter.IncZ();  PossiblyAddToQueue(); // r
  m_peekDataIter.DecX();  m_peekFlagIter.DecX();  PossiblyAddToQueue(); // s
  m_peekDataIter.DecY();  m_peekFlagIter.DecY();  PossiblyAddToQueue(); // t
  m_peekDataIter.DecY();  m_peekFlagIter.DecY();  PossiblyAddToQueue(); // u
  m_peekDataIter.IncX();  m_peekFlagIter.IncX();  PossiblyAddToQueue(); // v
  m_peekDataIter.IncX();  m_peekFlagIter.IncX();  PossiblyAddToQueue(); // w
  m_peekDataIter.IncY();  m_peekFlagIter.IncY();  PossiblyAddToQueue(); // x
  m_peekDataIter.IncY();  m_peekFlagIter.IncY();  PossiblyAddToQueue(); // y
  m_peekDataIter.DecX();  m_peekFlagIter.DecX();  // skip this one as it would be a repeat
  m_peekDataIter.DecY();  m_peekFlagIter.DecY();  PossiblyAddToQueue(); // z
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
bool RegGrow26Iterator<Iter, Vox, Decider>::End() const
{
  return(m_bGrowEnded);
} // End()


/**
 * Returns the voxel value at the current position (safe version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @return the voxel value at the position.
 */
template <class Iter, class Vox, class Decider>
const Vox & RegGrow26Iterator<Iter, Vox, Decider>::GetVoxel() const
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
const Vox & RegGrow26Iterator<Iter, Vox, Decider>::GetVoxelRaw() const
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
void RegGrow26Iterator<Iter, Vox, Decider>::SetVoxel(const Vox & value)
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
void RegGrow26Iterator<Iter, Vox, Decider>::SetVoxelRaw(const Vox & value)
{
  m_dataIter.SetVoxelRaw(value);
} // SetVoxelRaw()


/**
 * Returns the current position of the iterator within the volume.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param Decider       Type of the decider for whether to include a voxel into the region.
 * @return the x,y,z position in the volume.
 */
template <class Iter, class Vox, class Decider>
const Point<uint2> & RegGrow26Iterator<Iter, Vox, Decider>::GetPos() const
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
bool RegGrow26Iterator<Iter, Vox, Decider>::InsideVolume() const
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
std::ostream & operator<<(std::ostream & os, const RegGrow26Iterator<Iter, Vox, Decider> & iter)
{
  if ( iter.InsideVolume() ) 
  {
    os << "RegGrow26Iterator{Pos:"<<iter.GetPos() << "(inside)"  << "Val:"<<iter.GetVoxel() << "}" << flush;
  } 
  else 
  {
    os << "RegGrow26Iterator{Pos:"<<iter.GetPos() << "(outside)" << "Val:"<<iter.GetVoxel() << "}" << flush;
  }

  return( os );
} // operator<<()


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
bool RegGrow26Iterator<Iter, Vox, Decider>::TestByType(std::ostream & os) {
  bool ok(true);
  
  { // one voxel thick layer outside the region surface
    Vox label = (Vox)1;
    Volume<BlockVolume<Vox>, Vox> vol(Triple<uint4>(4,4,4), 8*sizeof(label));
    
    // turn on a 3D cross of voxels
    vol.SetVoxelRaw(Point<uint2>(0,0,0),label);
    
    BlockVolume<Vox>::VolIterator
      iter(vol.GetVolIterator(Point<uint2>(1,0,0)));
    
    DeciderEqual< BlockVolume<Vox>::VolIterator,Vox>
      inRegionDecider(label);
    
    DeciderOutsideSurface6<BlockVolume<Vox>::VolIterator, Vox,
      DeciderEqual<BlockVolume<Vox>::VolIterator,Vox> >
      deciderOutsideSurface6(inRegionDecider);
    
    // construct RegGrow26Iterator
    RegGrow26Iterator<
      BlockVolume<Vox>::VolIterator,
      Vox,
      DeciderOutsideSurface6<BlockVolume<Vox>::VolIterator, Vox,
      DeciderEqual< BlockVolume<Vox>::VolIterator, Vox> > >
      regGrowIter(iter, deciderOutsideSurface6);
    
    os << "one voxel thick layer outside the region surface region grow @ RegGrow26Iterator<\n  "
      << typeid(iter).name()<<",\n  "
      << typeid(label).name()<<",\n  "
      << typeid(deciderOutsideSurface6).name()<<" >::TestByType()" << endl;
    // actually grow through region and update the label
    vol.PrintAllRaw(os);
    os << iter.GetPos() << endl;
    Vox newLabel = (Vox)2; os << static_cast<int4>(newLabel) << "=newLabel iter="<<regGrowIter.GetPos();
    for ( ; !regGrowIter.End(); ++regGrowIter ) 
    {
      os << " iter="<<regGrowIter;
      regGrowIter.SetVoxelRaw(newLabel);
      vol.PrintAllRaw(os);
    }
    
    ok &= (   label == vol.GetVoxelRaw(Point<uint2>(0,0,0)));
    ok &= (newLabel == vol.GetVoxelRaw(Point<uint2>(0,0,1)));
    ok &= (newLabel == vol.GetVoxelRaw(Point<uint2>(0,1,0)));
    ok &= (newLabel == vol.GetVoxelRaw(Point<uint2>(1,0,0)));
    
    if ( ok )
    {
      os << "TEST SUCCEEDED :-)"<<endl;
    } 
    else 
    {
      os << "TEST FAILED :-(" << endl;
    }
  } // one voxel thick layer outside the region surface
  
  { //  simple region grow
    Vox label=(Vox)1;
    Volume<BlockVolume<Vox>,Vox> vol(Triple<uint4>(4,4,4), 8*sizeof(label));
    
    // turn on a 3D cross of voxels
    vol.SetVoxelRaw(Point<uint2>(0,0,0),label);
    vol.SetVoxelRaw(Point<uint2>(0,0,1),label);
    vol.SetVoxelRaw(Point<uint2>(0,0,2),label);
    vol.SetVoxelRaw(Point<uint2>(0,1,0),label);
    vol.SetVoxelRaw(Point<uint2>(0,1,1),label);
    vol.SetVoxelRaw(Point<uint2>(0,1,2),label);
    vol.SetVoxelRaw(Point<uint2>(0,2,0),label);
    vol.SetVoxelRaw(Point<uint2>(0,2,1),label);
    vol.SetVoxelRaw(Point<uint2>(0,2,2),label);
    
    vol.SetVoxelRaw(Point<uint2>(1,0,0),label);
    vol.SetVoxelRaw(Point<uint2>(1,0,1),label);
    vol.SetVoxelRaw(Point<uint2>(1,0,2),label);
    vol.SetVoxelRaw(Point<uint2>(1,1,0),label);
    vol.SetVoxelRaw(Point<uint2>(1,1,1),label);
    vol.SetVoxelRaw(Point<uint2>(1,1,2),label);
    vol.SetVoxelRaw(Point<uint2>(1,2,0),label);
    vol.SetVoxelRaw(Point<uint2>(1,2,1),label);
    vol.SetVoxelRaw(Point<uint2>(1,2,2),label);
    
    vol.SetVoxelRaw(Point<uint2>(2,0,0),label);
    vol.SetVoxelRaw(Point<uint2>(2,0,1),label);
    vol.SetVoxelRaw(Point<uint2>(2,0,2),label);
    vol.SetVoxelRaw(Point<uint2>(2,1,0),label);
    vol.SetVoxelRaw(Point<uint2>(2,1,1),label);
    vol.SetVoxelRaw(Point<uint2>(2,1,2),label);
    vol.SetVoxelRaw(Point<uint2>(2,2,0),label);
    vol.SetVoxelRaw(Point<uint2>(2,2,1),label);
    vol.SetVoxelRaw(Point<uint2>(2,2,2),label);
    
    BlockVolume<Vox>::VolIterator
      iter(vol.GetVolIterator(Point<uint2>(1,1,1)));
    DeciderEqual<BlockVolume<Vox>::VolIterator,Vox>
      decider(label);
    
    // construct RegGrow26Iterator
    RegGrow26Iterator<
      BlockVolume<Vox>::VolIterator,
      Vox,
      DeciderEqual<BlockVolume<Vox>::VolIterator,Vox> >
      regGrowIter(iter, decider);
    
    os << "Simple region grow @ RegGrow26Iterator<\n  "
      << typeid(iter).name()<<",\n  "
      << typeid(label).name()<<",\n  "
      << typeid(decider).name()<<" >::TestByType()" << endl;
    // actually grow through region and update the label
    vol.PrintAllRaw(os);
    Vox newLabel = (Vox)0;
    for ( ; !regGrowIter.End(); ++regGrowIter ) 
    {
      regGrowIter.SetVoxelRaw(newLabel);
      vol.PrintAllRaw(os);
    }
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,0,0)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,0,1)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,0,2)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,1,0)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,1,1)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,1,2)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,2,0)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,2,1)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,2,2)));
    
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,0,0)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,0,1)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,0,2)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(1,1,0)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(1,1,1)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(1,1,2)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(2,2,0)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(2,2,1)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(2,2,2)));
    
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,0,0)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,0,1)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(0,0,2)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(1,1,0)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(1,1,1)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(1,1,2)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(2,2,0)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(2,2,1)));
    ok == ok && (newLabel == vol.GetVoxelRaw(Point<uint2>(2,2,2)));
    
    if ( ok ) {
      os << "TEST SUCCEEDED :-)"<<endl;
    } else {
      os << "TEST FAILED :-(" << endl;
    }
  }
  
  return(ok);
} // TestByType()

#undef FILE_REVISION

// $Log: RegGrow26Iterator.inl,v $
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
//    Rev 2.0   Sep 16 2001 23:48:18   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:52   ingmar
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
// Revision 1.23  2001/08/31 18:53:40  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.22  2001/08/07 17:03:40  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.21  2001/04/10 17:39:10  dmitry
// updated datatypes
//
// Revision 1.20  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.19  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.18  2000/11/19 01:34:52  ingmar
// made DeciderOutsideSurface6  use the new neighbor iterator
//
// Revision 1.17  2000/11/17 00:28:41  dave
// remove syntax problems for gcc
//
// Revision 1.16  2000/11/16 17:19:49  dave
// added native BlockVolume VolIterator
//
// Revision 1.15  2000/11/15 21:45:36  lihong
// added flaging in the start voxel
//
// Revision 1.14  2000/11/10 17:34:05  dave
// impl. API changes for VolIterator
//
// Revision 1.13  2000/11/04 03:08:56  ingmar
// Streamlined VolIterator
//
// Revision 1.12  2000/11/03 18:20:15  ingmar
// moved volume dimension back to unit and made positions inside a volume ushort
//
// Revision 1.11  2000/11/03 17:02:26  dave
// converted back
//
// Revision 1.10  2000/11/02 19:48:09  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.9  2000/11/01 21:11:29  dave
// LZO default, GetDataType() fully removed, re-order Vol. declaration
//
// Revision 1.8  2000/11/01 17:18:18  ingmar
// added test for surface crawl decider for 26 connected voxels
//
// Revision 1.7  2000/11/01 16:30:12  lihong
// debugged surface crawling decider + itreator
//
// Revision 1.6  2000/11/01 07:22:32  antonio
// added test for 4x4x4 volume in regiongrow26 with 2x2x2 of 1's
//
// Revision 1.5  2000/11/01 06:41:28  antonio
// test of region grow 26 works
//
// Revision 1.4  2000/11/01 05:59:56  antonio
// improved operator <<
//
// Revision 1.3  2000/10/31 22:35:28  ingmar
// added tests and operator <<
//
// Revision 1.2  2000/10/30 21:34:32  ingmar
// added dilate region iterator test (still needs debugging)
//
// Revision 1.1  2000/10/30 18:08:02  ingmar
// added 26 connected region grow iterator to volume module
//
//
// $Id: RegGrow26Iterator.inl,v 1.3 2007/04/10 01:23:32 jmeade Exp $

