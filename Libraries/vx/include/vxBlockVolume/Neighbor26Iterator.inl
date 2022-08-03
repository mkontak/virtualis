// $Id: Neighbor26Iterator.inl,v 1.2 2007/04/09 20:38:16 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.net

#define FILE_REVISION "$Revision: 1.2 $"

////////////////////////////////////////////////////////////////////////
/// CONSTRUCTOR & ASSIGNMENT

/**
 * Constructor.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           Type of the voxels in the data volume.
 * @param dataIter      iterator for the volume in which a region is supposed to be traversed.
 */
template <class Iter, class Vox>
Neighbor26Iterator<Iter, Vox>::Neighbor26Iterator(const Iter & dataIter)
: m_dataIter(dataIter)
{
  // check if m_volIterator is inside
  if ( !dataIter.InsideVolume() ) 
  {
    LogWrn("<Neighbor26Iterator::constructor> : seed iterator not in volume","Neighbor26Iterator<Iter, Vox>","Neighbor26Iterator");
  }
  
  // move to get the start neighbor voxel,
  m_dataIter.DecZ();
  m_uNeighborIndex = 0;
} // Constructor


/**
 * Increments through the 26 neighbors of the center voxel.     <BR>
 * NOTE that the FIrst location is already a neighbor location. <BR>
 * NOTE that incrementing after the last position puts the iterator onto the center voxel.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 */
template <class Iter, class Vox>
void Neighbor26Iterator<Iter, Vox>::operator++()
{
  // move along all 26 neighbors
  
  //  bottom     middle     top
  //  z = -1     z = 0      z = +1
  //
  //  i--h--g    j  q--p    s--r  y     Y
  //        |    |     |    |    /|         Z
  //  b--a  f    k     o    t  z  x     | /
  //  |     |    |     |    |     |     |/
  //  c--d--e    l--m--n    u--v--w     +--- X
  
  
  static const Vector<int2> m_toNextNeighbor[256] =
  {
    Vector<int2>(-1, 0, 0), // a -> b
      Vector<int2>( 0,-1, 0), // b -> c
      Vector<int2>(+1, 0, 0), // c
      Vector<int2>(+1, 0, 0), // d
      Vector<int2>( 0,+1, 0), // e
      Vector<int2>( 0,+1, 0), // f
      Vector<int2>(-1, 0, 0), // g
      Vector<int2>(-1, 0, 0), // h
      Vector<int2>( 0, 0,+1), // i
      Vector<int2>( 0,-1, 0), // j
      Vector<int2>( 0,-1, 0), // k
      Vector<int2>(+1, 0, 0), // l
      Vector<int2>(+1, 0, 0), // m
      Vector<int2>( 0,+1, 0), // n
      Vector<int2>( 0,+1, 0), // o
      Vector<int2>(-1, 0, 0), // p
      Vector<int2>( 0, 0,+1), // q
      Vector<int2>(-1, 0, 0), // r
      Vector<int2>( 0,-1, 0), // s
      Vector<int2>( 0,-1, 0), // t
      Vector<int2>(+1, 0, 0), // u
      Vector<int2>(+1, 0, 0), // v
      Vector<int2>( 0,+1, 0), // w
      Vector<int2>( 0,+1, 0), // x -> y
      Vector<int2>(-1,-1, 0), // y -> z
      Vector<int2>( 0, 0,-1)  // back to center position
      // the remainder is (0,0,0) by default constructor and
      //   makes sure that calling ++ too often will stay at the
      //   center and not cause segfaults
  };
  
  m_dataIter.MoveRel(m_toNextNeighbor[m_uNeighborIndex]);
  ++m_uNeighborIndex;
} // operator ++


/**
 * Indicates when the region growing is finished.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @return true when the traversal is finished.
 */
template <class Iter, class Vox>
bool Neighbor26Iterator<Iter, Vox>::End() const
{
  return ( m_uNeighborIndex > 25 );
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
const Vox & Neighbor26Iterator<Iter, Vox>::GetVoxel() const
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
const Vox & Neighbor26Iterator<Iter, Vox>::GetVoxelRaw() const
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
void Neighbor26Iterator<Iter, Vox>::SetVoxel(const Vox & value)
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
void Neighbor26Iterator<Iter, Vox>::SetVoxelRaw(const Vox & value)
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
inline const Iter & Neighbor26Iterator<Iter, Vox>::GetDataVolIter() const
{
  return ( m_dataIter );
} // GetDataVolIter



/**
 * Centers the iterator at the given absolute position.
 * BE AWARE that it automatically places the internal position to the
 *          first neighbor of the position in the function argument.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param pos           Position of the voxel of whom the neighbors are to be traversed.
 */
template <class Iter, class Vox>
void Neighbor26Iterator<Iter, Vox>::CenterAt(const Point<uint2> & pos)
{
  // move to one below center, as this will be the first neighbor
  m_dataIter.MoveAbs(Point<uint2>(pos.m_x, pos.m_y, pos.m_z-1));
  m_uNeighborIndex = 0;
} // CenterAt

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
const Point<uint2> & Neighbor26Iterator<Iter, Vox>::GetPos() const
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
bool Neighbor26Iterator<Iter, Vox>::InsideVolume() const
{
  return(m_dataIter.InsideVolume());
} // InsideVolume


/**
 * Overloaded << Operator.
 *
 * @param Iter          Type of the volume data iterator.
 * @param Vox           type of the voxels in the data volume.
 * @param iter          iterator for the volume in which a region is supposed to be traversed.
 * @param os            stream to output data to
 * @return              the output stream
 */
template <class Iter, class Vox>
std::ostream & operator<<(std::ostream & os, const Neighbor26Iterator<Iter, Vox> & iter)
{
  if ( iter.InsideVolume() ) {
    os << "Neighbor26Iterator{Pos:"<<iter.GetPos() << "(inside)"  << "Val:"<<iter.GetVoxel() << "}" << flush;
  } else {
    os << "Neighbor26Iterator{Pos:"<<iter.GetPos() << "(outside)" << "Val:"<<iter.GetVoxel() << "}" << flush;
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
bool Neighbor26Iterator< Iter , Vox >::TestByType(std::ostream & os, Vol) {
  bool ok(true);
  
  Vox vox=(Vox)1;
  Vol vol(Triple<uint4>(3,3,3), 8*sizeof(vox));
  
  Iter iter(vol.GetVolIterator(Point<uint2>(1,1,1)));
  
  // construct Neighbor26Iterator
  Neighbor26Iterator< Iter, Vox > neigbor6Iter(iter);
  
  os << "Neighbor26Iterator<\n  "
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

// $Log: Neighbor26Iterator.inl,v $
// Revision 1.2  2007/04/09 20:38:16  jmeade
// code review.
//
// Revision 1.1  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
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
// Revision 1.1  2001/09/01 00:12:19  soeren
// renamed volume  directory
//
// Revision 1.13  2001/08/31 18:53:40  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.12  2001/08/07 17:03:40  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.11  2001/04/18 03:02:43  ingmar
// updated header info
//
// Revision 1.10  2001/04/14 04:24:58  ingmar
// spelling
//
// Revision 1.9  2001/04/09 18:03:51  dmitry
// Removed Revision History from Top of File
//
// Revision 1.8  2001/04/08 21:38:21  dave
// updated doc headers
//
// Revision 1.7  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.6  2000/11/19 01:33:35  ingmar
// added GetDataVolIter
//
// Revision 1.5  2000/11/10 17:34:05  dave
// impl. API changes for VolIterator
//
// Revision 1.4  2000/11/06 17:35:05  ingmar
// added comments
//
// Revision 1.3  2000/11/06 17:27:35  dave
// CenterAt() function added
//
// Revision 1.2  2000/11/06 15:12:47  frank
// Added new CenterAt function
//
// Revision 1.1  2000/11/04 23:46:55  ingmar
// first version of Neighbor26Iterator
//
//
// $Id: Neighbor26Iterator.inl,v 1.2 2007/04/09 20:38:16 jmeade Exp $

