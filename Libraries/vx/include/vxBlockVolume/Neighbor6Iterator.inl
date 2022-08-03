// $Id: Neighbor6Iterator.inl,v 1.3 2007/04/09 20:26:27 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
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
 * Copy Constructor.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           Type of the voxels in the data volume.
 * @param dataIter      iterator for the volume in which a region is supposed to be traversed.
 */
template <class Iter, class Vox>
Neighbor6Iterator<Iter, Vox>::Neighbor6Iterator(const Neighbor6Iterator<Iter, Vox> & other)
{
  m_dataIter = other.m_dataIter;
  m_state.m_eChangeX = other.m_state.m_eChangeX;
  m_state.m_eChangeY = other.m_state.m_eChangeY;
  m_state.m_eChangeZ = other.m_state.m_eChangeZ;
  m_state.m_bEnd = other.m_state.m_bEnd;
} // Copy Constructor


/**
 * Constructor.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           Type of the voxels in the data volume.
 * @param dataIter      iterator for the volume in which a region is supposed to be traversed.
 */
template <class Iter, class Vox>
Neighbor6Iterator<Iter, Vox>::Neighbor6Iterator(const Iter & dataIter)
: m_dataIter(dataIter)
{
  // check if m_volIterator is inside
  if ( !dataIter.InsideVolume() ) 
  {
    LogWrn("<Neighbor6Iterator::constructor> : seed iterator not in volume","Neighbor6Iterator<Iter, Vox>", "Neighbor6Iterator");
  }
  
  // increment once to get the start neighbor voxel,
  m_dataIter.DecX();
  m_state.m_eChangeX = DECREMENTED;
  m_state.m_eChangeY = CENTERED;
  m_state.m_eChangeZ = CENTERED;
  m_state.m_bEnd = false;
} // Constructor


/**
 * assignment operator.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           Type of the voxels in the data volume.
 * @param rhs           source of copy
 */
template <class Iter, class Vox>
Neighbor6Iterator<Iter, Vox> & Neighbor6Iterator<Iter, Vox>::operator = (const Neighbor6Iterator<Iter, Vox> & rhs)
{
  m_dataIter = rhs.m_dataIter;
  m_state.m_eChangeX = rhs.m_state.m_eChangeX;
  m_state.m_eChangeY = rhs.m_state.m_eChangeY;
  m_state.m_eChangeZ = rhs.m_state.m_eChangeZ;
  m_state.m_bEnd = rhs.m_state.m_bEnd;
} // assignment operator


////////////////////////////////////////////////////////////////////////
/// VOLUME TRAVERSAL

/**
 * pops the next position of the queue, moved the data+flag iterators to there and schedules the neighbors for adition in to the queue
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 */
template <class Iter, class Vox>
void Neighbor6Iterator<Iter, Vox>::operator++()
{
  if ( m_state.m_eChangeX != CENTERED ) 
  {
    if ( m_state.m_eChangeX == DECREMENTED ) 
    {
      m_dataIter.IncX(2);
      m_state.m_eChangeX = INCREMENTED;
      return;
    } else { // m_state.m_eChangeX == INCREMENTED
      m_dataIter.DecX();
      m_dataIter.DecY();
      m_state.m_eChangeX = CENTERED;
      m_state.m_eChangeY = DECREMENTED;
      return;
    }
  }
  if ( m_state.m_eChangeY != CENTERED ) 
  {
    if ( m_state.m_eChangeY == DECREMENTED ) 
    {
      m_dataIter.IncY(2);
      m_state.m_eChangeY = INCREMENTED;
      return;
    } 
    else 
    { // m_state.m_eChangeY == INCREMENTED
      m_dataIter.DecY();
      m_dataIter.DecZ();
      m_state.m_eChangeY = CENTERED;
      m_state.m_eChangeZ = DECREMENTED;
      return;
    }
  }
  if ( m_state.m_eChangeZ != CENTERED ) 
  {
    if ( m_state.m_eChangeZ == DECREMENTED ) 
    {
      m_dataIter.IncZ(2);
      m_state.m_eChangeZ = INCREMENTED;
      return;
    } 
    else 
    { // m_state.m_eChangeZ == INCREMENTED
      m_dataIter.DecZ();
      m_state.m_eChangeZ = CENTERED;
      m_state.m_bEnd = true;
      return;
    }
  }
} // operator ++


/**
 * Indicates when the region growing is finished.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @return true when the traversal is finished.
 */
template <class Iter, class Vox>
bool Neighbor6Iterator<Iter, Vox>::End() const
{
  return(m_state.m_bEnd);
} // End


////////////////////////////////////////////////////////////////////////
/// GET & SET VOXELS

/**
 * Returns the voxel value at the current position (safe version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @return the voxel value at the position.
 */
template <class Iter, class Vox>
const Vox & Neighbor6Iterator<Iter, Vox>::GetVoxel() const
{
  return(m_dataIter.GetVoxel());
} // GetVoxel


/**
 * Returns the voxel value at the current position (un-safe/fast version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @return the voxel value at the position.
 */
template <class Iter, class Vox>
const Vox & Neighbor6Iterator<Iter, Vox>::GetVoxelRaw() const
{
  return(m_dataIter.GetVoxelRaw());
} // GetVoxelRaw


/**
 * Sets the voxel value at the current position (safe version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param value         the new voxel value.
 */
template <class Iter, class Vox>
void Neighbor6Iterator<Iter, Vox>::SetVoxel(const Vox & value)
{
  m_dataIter.SetVoxel(value);
} // SetVoxel


/**
 * Sets the voxel value at the current position (un-safe/fast version).
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param value         the new voxel value.
 */
template <class Iter, class Vox>
void Neighbor6Iterator<Iter, Vox>::SetVoxelRaw(const Vox & value)
{
  m_dataIter.SetVoxelRaw(value);
} // SetVoxelRaw


/**
 * Return a reference to the underlying VolIterator.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 */
template <class Iter, class Vox>
inline const Iter & Neighbor6Iterator<Iter, Vox>::GetDataVolIter() const
{
  return ( m_dataIter );
} // GetDataVolIter

////////////////////////////////////////////////////////////////////////
/// ABSOLUTE LOCATION FUNCTIONS

/**
 * Moves the iterator to the given absolute position.
 * @param delta the location relative to the current location to move to.
 * @return the voxel value at the new location.
 */
template <class Iter, class Vox>
void Neighbor6Iterator<Iter, Vox>::CenterAt(const Point<uint2> & pos)
{
  m_dataIter.MoveAbs(pos);
  
  // decrement once to get the start neighbor voxel,
  m_dataIter.DecX();
  m_state.m_eChangeX = DECREMENTED;
  m_state.m_eChangeY = CENTERED;
  m_state.m_eChangeZ = CENTERED;
  m_state.m_bEnd = false;
} // MoveAbs


////////////////////////////////////////////////////////////////////////
/// STATUS REPORTS

/**
 * Returns the current position of the iterator within the volume.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @return the x,y,z position in the volume.
 */
template <class Iter, class Vox>
const Point<uint2> & Neighbor6Iterator<Iter, Vox>::GetPos() const
{
  return(m_dataIter.GetPos());
} // GetPos


/**
 * indicated the iterator position relative to the volume extend.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @return true if the iterator is inside the volume.
 */
template <class Iter, class Vox>
bool Neighbor6Iterator<Iter, Vox>::InsideVolume() const
{
  return(m_dataIter.InsideVolume());
} // InsideVolume


/**
 * Overloaded << Operator.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param os            output stream
 * @param iter          iterator for the volume in which a region is supposed to be traversed.
 */
template <class Iter, class Vox>
std::ostream & operator<<(std::ostream & os, const Neighbor6Iterator<Iter, Vox> & iter)
{
  if ( iter.InsideVolume() ) {
    os << "Neighbor6Iterator{Pos:"<<iter.GetPos() << "(inside)"  << "Val:"<<iter.GetVoxel() << "}" << flush;
  } else {
    os << "Neighbor6Iterator{Pos:"<<iter.GetPos() << "(outside)" << "Val:"<<iter.GetVoxel() << "}" << flush;
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
#ifdef _gcc
/// this needs to work for any combinations of template arguments which breaks VC++, but are correct snd work in gcc
template <class Iter, class Vox> template < class Vol >
bool Neighbor6Iterator< Iter , Vox >::TestByType(std::ostream & os, Vol) {
  bool ok(true);
  
  Vox vox=(Vox)1;
  Vol vol(Triple<uint4>(3,3,3), 8*sizeof(vox));
  
  Iter iter(vol.GetVolIterator(Point<uint2>(1,1,1)));
  
  // construct Neighbor6Iterator
  Neighbor6Iterator< Iter, Vox > neigbor6Iter(iter);
  
  os << "Neighbor6Iterator<\n  "
    << typeid(iter).name()<<",\n  "
    << typeid(vox).name()<<",\n  "
    << typeid(vol).name()<<" >::TestByType()" << endl;
  
  // turn on a 3D cross of voxels
  vol.SetVoxelRaw(Point<uint2>(1,1,1),vox);
  
  // actually grow through region and update the label
  vol.PrintAllRaw();
  Vox newVoxel = (Vox)2;
  for ( ; !neigbor6Iter.End(); ++neigbor6Iter ) {
    neigbor6Iter.SetVoxelRaw(newVoxel);
    vol.PrintAllRaw();
  }
  ok &= (newVoxel == vol.GetVoxelRaw(Point<uint2>(0,1,1)));
  ok &= (newVoxel == vol.GetVoxelRaw(Point<uint2>(2,1,1)));
  ok &= (newVoxel == vol.GetVoxelRaw(Point<uint2>(1,0,1)));
  ok &= (newVoxel == vol.GetVoxelRaw(Point<uint2>(1,2,1)));
  ok &= (newVoxel == vol.GetVoxelRaw(Point<uint2>(1,1,0)));
  ok &= (newVoxel == vol.GetVoxelRaw(Point<uint2>(1,1,2)));
  if ( ok ) {
    os << "TEST SUCCEEDED :-)"<<endl;
  } else {
    os << "TEST FAILED :-(" << endl;
  }
  return(ok);
} // TestByType
#endif // _gcc

#undef FILE_REVISION

// $Log: Neighbor6Iterator.inl,v $
// Revision 1.3  2007/04/09 20:26:27  jmeade
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
//    Rev 2.0   Sep 16 2001 23:48:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:44   ingmar
// Initial revision.
// Revision 1.3  2001/09/13 17:42:49  soeren
// added quotes to function name in logger macro
//
// Revision 1.2  2001/09/10 09:04:59  ingmar
// fixed use of logger defines
//
// Revision 1.1  2001/09/10 01:16:45  ingmar
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
// Revision 1.11  2001/08/31 18:53:40  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.10  2001/08/07 17:03:40  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.9  2001/04/18 03:02:43  ingmar
// updated header info
//
// Revision 1.8  2001/04/09 18:03:50  dmitry
// Removed Revision History from Top of File
//
// Revision 1.7  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.6  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.5  2000/11/19 00:43:36  ingmar
// added copy constructor and assignment operator as well as CenterAt
//
// Revision 1.4  2000/11/15 03:03:37  ingmar
// Added MoveAbs()
//
// Revision 1.3  2000/11/04 22:07:36  ingmar
// disabled test for Neighbor6Iterator when VC++ is used
//
// Revision 1.2  2000/11/04 21:27:46  ingmar
// debugged templetized test for Neighbor6Iterator
//
// Revision 1.1  2000/11/04 20:54:41  ingmar
// first version of Neighbor6Iterator
//
// $Id: Neighbor6Iterator.inl,v 1.3 2007/04/09 20:26:27 jmeade Exp $

