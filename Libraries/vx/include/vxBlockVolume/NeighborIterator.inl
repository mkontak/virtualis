// $Id: NeighborIterator.inl,v 1.3 2007/04/09 17:03:28 jmeade Exp $
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
 * Constructor.
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             Type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 * @param dataIter        iterator for the volume in which a region is supposed to be traversed.
 */
template <class Iter, class Vox, class NeighborWalker>
NeighborIterator<Iter, Vox, NeighborWalker>::NeighborIterator(const Iter & dataIter)
  : m_neighborWalker(dataIter)
{
  // check if m_volIterator is inside
  if ( !m_neighborWalker.m_dataIter.InsideVolume() )
  {
    LogWrn("center point not inside volume but at "+m_neighborWalker.m_dataIter.GetPos(),"NeighborIterator", "Constructor");
  }

  // move to get the start neighbor voxel,
  m_neighborWalker.GotoNextNeighbor();
} // Constructor


/**
 * Increments through the 18 neighbors of the center voxel.     <BR>
 * NOTE that the First location is already a neighbor location. <BR>
 * NOTE that incrementing after the last position puts the iterator onto the center voxel.
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 */
template <class Iter, class Vox, class NeighborWalker>
void NeighborIterator<Iter, Vox, NeighborWalker>::operator++()
{
  m_neighborWalker.GotoNextNeighbor();
} // operator ++


/**
 * Indicates when the region growing is finished.
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 * @return                true when the traversal is finished.
 */
template <class Iter, class Vox, class NeighborWalker>
bool NeighborIterator<Iter, Vox, NeighborWalker>::End() const
{
  return( m_neighborWalker.End() );
} // End


////////////////////////////////////////////////////////////////////////
/// GET & SET VOXELS

/**
 * Returns the voxel value at the current position (safe version).
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 * @return                the voxel value at the position.
 */
template <class Iter, class Vox, class NeighborWalker>
const Vox & NeighborIterator<Iter, Vox, NeighborWalker>::GetVoxel() const
{
  return(m_neighborWalker.m_dataIter.GetVoxel());
} // GetVoxel


/**
 * Returns the voxel value at the current position (un-safe/fast version).
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 * @return                the voxel value at the position.
 */
template <class Iter, class Vox, class NeighborWalker>
const Vox & NeighborIterator<Iter, Vox, NeighborWalker>::GetVoxelRaw() const
{
  return(m_neighborWalker.m_dataIter.GetVoxelRaw());
} // GetVoxelRaw


/**
 * Sets the voxel value at the current position (safe version).
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 * @param value           the new voxel value.
 */
template <class Iter, class Vox, class NeighborWalker>
void NeighborIterator<Iter, Vox, NeighborWalker>::SetVoxel(const Vox & value)
{
  m_neighborWalker.m_dataIter.SetVoxel(value);
} // SetVoxel


/**
 * Sets the voxel value at the current position (un-safe/fast version).
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 * @param value           the new voxel value.
 */
template <class Iter, class Vox, class NeighborWalker>
void NeighborIterator<Iter, Vox, NeighborWalker>::SetVoxelRaw(const Vox & value)
{
  m_neighborWalker.m_dataIter.SetVoxelRaw(value);
} // SetVoxelRaw


/**
 * Gets the raw value of the median voxel
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 */
template <class Iter, class Vox, class NeighborWalker>
Vox NeighborIterator<Iter, Vox, NeighborWalker>::GetMedianVoxelRaw() const
{
  std::vector<Vox> vect(19);
  for ( uint4 i(0); i < 18; operator++(), ++i )
  {
    vect[i] = GetVoxelRaw();
  }
  vect[18] = GetVoxelRaw();

  nth_element(vect.begin(), vect.begin() + 10 , vect.end());
  return (vect[9]);
} // GetMedianVoxelRaw()


/**
 * Return a reference to the underlying VolIterator.
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 */
template <class Iter, class Vox, class NeighborWalker>
inline const Iter & NeighborIterator<Iter, Vox, NeighborWalker>::GetDataVolIter() const
{
  return( m_neighborWalker.m_dataIter );
} // GetDataVolIter



/**
 * Centers the iterator at the given absolute position.
 * BE AWARE that it automatically places the internal position to the
 *          first neighbor of the position in the function argument.
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 * @param pos             Position of the voxel of whom the neighbors are to be traversed.
 */
template <class Iter, class Vox, class NeighborWalker>
void NeighborIterator<Iter, Vox, NeighborWalker>::CenterAt(const Point<uint2> & pos)
{
  // move to one below center, as this will be the first neighbor
  m_neighborWalker.CenterAt(pos);
} // CenterAt

////////////////////////////////////////////////////////////////////////
/// STATUS REPORTS

/**
 * Returns the current position of the iterator within the volume.
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 * @return                the x,y,z position in the volume.
 */
template <class Iter, class Vox, class NeighborWalker>
const Point<uint2> & NeighborIterator<Iter, Vox, NeighborWalker>::GetPos() const
{
  return(m_neighborWalker.m_dataIter.GetPos());
} // GetPos


/**
 * indicated the iterator position relative to the volume extend.
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 * @return                true if the iterator is inside the volume.
 */
template <class Iter, class Vox, class NeighborWalker>
bool NeighborIterator<Iter, Vox, NeighborWalker>::InsideVolume() const
{
  return(m_neighborWalker.m_dataIter.InsideVolume());
} // InsideVolume


/**
 * Overloaded << Operator.
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 * @return                the stream after data written
 */
template <class Iter, class Vox, class NeighborWalker>
std::ostream & operator<<(std::ostream & os, const NeighborIterator<Iter, Vox, NeighborWalker> & iter)
{
  if ( iter.InsideVolume() )
  {
    os << "NeighborIterator{Pos:"<<iter.GetPos() << "(inside)"  << "Val:"<<iter.GetVoxel() << "}" << flush;
  } else
  {
    os << "NeighborIterator{Pos:"<<iter.GetPos() << "(outside)" << "Val:"<<iter.GetVoxel() << "}" << flush;
  }
  return( os );
} // operator<<


////////////////////////////////////////////////////////////////////////
/// VERIFICATION

/**
 * Test function to verify the class functionality.
 *
 * @param Iter            Type of the volume data iterator.
 * @param Vox             type of the voxels in the data volume.
 * @param NeighborWalker  Type of the volume data walker
 * @param Decider         Type of the decider for whether to include a voxel into the region.
 * @param os              output stream to print progress info to
 * @return                indicate whether the test was successful
 */
template <class Iter, class Vox, class NeighborWalker> 
bool NeighborIterator< Iter, Vox, NeighborWalker >::TestByTypeBlk(std::ostream & os)
{
  bool bOk(true);

  Vox vox=(Vox)1;
  typedef Volume<BlockVolume<Vox>,Vox> Vol;

  Vol vol(Triple<uint4>(3,3,3), 8*sizeof(vox));

  Iter iter(vol.GetVolIterator(Point<uint2>(1,1,1)));

  // construct NeighborIterator
  NeighborIterator< Iter, Vox, NeighborWalker > neigborIter(iter);

  os << "NeighborIterator<\n  "
    << typeid(iter).name()<<",\n  "
    << typeid(vox).name()<<",\n  "
    << typeid(NeighborWalker).name()<<",\n  "
    << typeid(vol).name()<<" >::TestByType()" << endl;

  // turn on a 3D cross of voxels
  vol.SetVoxelRaw(Point<uint2>(1,1,1),vox);

  // actually grow through region and update the label
  vol.PrintAllRaw(os);
  Vox newVoxel = (Vox)2;
  for ( ; !neigborIter.End(); ++neigborIter )
  {
    neigborIter.SetVoxelRaw(newVoxel);
  }
  vol.PrintAllRaw(os);
  bOk &= (newVoxel == vol.GetVoxelRaw(Point<uint2>(0,1,1)));
  bOk &= (newVoxel == vol.GetVoxelRaw(Point<uint2>(2,1,1)));
  bOk &= (newVoxel == vol.GetVoxelRaw(Point<uint2>(1,0,1)));
  bOk &= (newVoxel == vol.GetVoxelRaw(Point<uint2>(1,2,1)));
  bOk &= (newVoxel == vol.GetVoxelRaw(Point<uint2>(1,1,0)));
  bOk &= (newVoxel == vol.GetVoxelRaw(Point<uint2>(1,1,2)));
  if ( bOk )
  {
    os << ":-) TEST SUCCEEDED"<<endl;
  } else
  {
    os << ":-( TEST FAILED" << endl;
  }
  return(bOk);
} // TestByTypeBlk

#undef FILE_REVISION

// $Log: NeighborIterator.inl,v $
// Revision 1.3  2007/04/09 17:03:28  jmeade
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
// Revision 3.2  2003/01/22 21:29:17  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
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
//    Rev 2.0   Sep 16 2001 23:48:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:48   ingmar
// Initial revision.
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
// Revision 1.6  2001/08/14 05:23:04  ingmar
// removed basic volIter from NeighborIter, it is now only in NeighborWalker
//
// Revision 1.5  2001/08/07 17:03:40  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.4  2001/05/14 15:20:52  ingmar
// better testByType
//
// Revision 1.3  2001/04/28 01:22:56  ingmar
// fixed CenterAt() to call neighborWalker
//
// Revision 1.2  2001/04/18 03:02:43  ingmar
// updated header info
//
// Revision 1.1  2001/04/14 04:36:21  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/NeighborIterator.inl,v 1.3 2007/04/09 17:03:28 jmeade Exp $
// $Id: NeighborIterator.inl,v 1.3 2007/04/09 17:03:28 jmeade Exp $

