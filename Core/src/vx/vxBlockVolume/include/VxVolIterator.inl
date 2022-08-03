// $Id: VxVolIterator.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created:  summer 00
// Author: David & Jayanta & Ingmar dave@viatronix.com
//
// Complete History at bottom of file.


////////////////////////////////////////////////////////////////////////
/// CONSTRUCTOR & ASSIGNMENT

/**
 * Constructor.
 */
template <class L, class T>
VxVolIterator<L,T>::VxVolIterator(L & container, const Point<uint2> & seed)
: m_vol(container), m_pos(seed), m_inside(m_vol.InsideVolume(seed))
{
} // constructor


/**
 * Assignment operator.
 */
/*
template <class L, class T>
VxVolIterator<L, T> const & VxVolIterator<L,T>::operator=(const VxVolIterator<L, T> & rhs)
{
  m_vol = rhs.m_vol;
  m_pos = rhs.m_pos;
  m_inside = rhs.m_inside;
  return(* this);
} // assignment
    */

    
////////////////////////////////////////////////////////////////////////
/// COMPLETE VOLUME TRAVERSAL

/**
 * Returns the next non-background voxel.
 */
template <class L, class T>
void VxVolIterator<L,T>::NextNonZero()
{
  do {
    NextZYX();
  } while( GetVoxel() == m_vol.GetBackground() );
}  // NextNonZero


/**
 * Prefix increment operator for iterator location.  Since no native, 
 * faster ++ exists this just calls NextZYX()
 */
template <class L, class T>
void VxVolIterator<L,T>::operator++()
{
  NextZYX();
} // operator ++


/**
 * Returns the next voxel according to a ZYX traversal.
 */
template <class L, class T>
void VxVolIterator<L,T>::NextZYX()
{
  // imitate triple for() loop behavior

  if ( m_pos.m_x < (m_vol.GetDim().m_x - 1) ) {
    ++m_pos.m_x; return;
  } else {
    m_pos.m_x = 0;
  }

  if ( m_pos.m_y < (m_vol.GetDim().m_y - 1) ) {
    ++m_pos.m_y; return;
  } else {
    m_pos.m_y = 0;
  }

  if ( m_pos.m_z < (m_vol.GetDim().m_z - 1) ) {
    m_pos.m_z += 1; return;
  } else {
    // note that calling GetVoxelRaw() in this state may cause bad things.
    if ( m_pos.m_z < (m_vol.GetDim().m_z) ) {
      m_pos.m_z += 1;  // increment z anyway, should be equal to end now
      m_inside = false;
    } else {
      Error::Warn("VxVolIterator<L,T>::operator++: Attempted VxVolIterator increment illegally");
      m_inside = false;
    }
  }
}  // NextZYX


/**
 *
 */
template <class L, class T>
bool VxVolIterator<L,T>::End()
{
  // true if operator++ has jumped to one past last location in volume
  return(!m_inside);
} // End


////////////////////////////////////////////////////////////////////////
/// GET & SET VOXELS

/**
 * Returns the value at the current location.
 * @return the value at the location.
 */
template <class L, class T>
const T & VxVolIterator<L,T>::GetVoxel() const
{
  if ( m_inside ) {
    return(m_vol.GetVoxelRaw(m_pos));
  } else {
    return(m_vol.GetBackground());
  }
} // GetVoxel


/**
 * Returns the value at the current location.
 * @return the value at the location.
 */
template <class L, class T>
const T & VxVolIterator<L,T>::GetVoxelRaw() const
{
  return(m_vol.GetVoxelRaw(m_pos));
} // GetVoxelRaw


/**
 * Sets the value at the current location.
 * @return the value at the location.
 */
template <class L, class T>
void VxVolIterator<L,T>::SetVoxel(const T & value)
{
  if ( m_inside ) {
    m_vol.SetVoxelRaw(m_pos, value);
  }
} // SetVoxel


/**
 * Sets the value at the current location.
 * @param value the new value for the current location.
 * @return the value at the location.
 */
template <class L, class T>
void VxVolIterator<L,T>::SetVoxelRaw(const T & value)
{
  m_vol.SetVoxelRaw(m_pos, value);
} // SetVoxelRaw


/**
 * deprecated operator * , remains in API to support STL algorithms - use GetVoxel instead.
 */
template <class L, class T>
const T & VxVolIterator<L,T>::operator*() const
{
  return(m_vol.GetVoxelRaw(m_pos));
} // * read access


////////////////////////////////////////////////////////////////////////
/// COMPARATORS

/**
 * Iterator inequality test by location.
 * @return true if the locations are not equal.
 */
template <class L, class T>
bool VxVolIterator<L,T>::operator!=(const VxVolIterator & rhs)
{
  return( m_pos != rhs.m_pos );
} // !=


/**
 * Iterator equality test by location.
 * @return true if the locations are equal.
 */
template <class L, class T>
bool VxVolIterator<L,T>::operator==(const VxVolIterator & rhs)
{
  return( m_pos == rhs.m_pos );
} // ==


////////////////////////////////////////////////////////////////////////
/// ABOSOLUTE 

/**
 * Moves the iterator to the given absolute position.
 * @param delta the location relative to the current location to move to.
 * @return the voxel value at the new location.
 */
template <class L, class T>
void VxVolIterator<L,T>::MoveAbs(const Point<uint2> & pos)
{
  m_pos = pos;
  m_inside = m_vol.InsideVolume(m_pos);
} // MoveAbs


/**
 * Peeks at the value at the location given relative to current location.
 * @param delta the position relative to current to look at.
 * @return the value at the location.
 */
template <class L, class T>
const T & VxVolIterator<L,T>::PeekRel(const Vector<int2> & delta) const
{
  Point<int2> pos = m_pos + delta;
  if ( m_vol.InsideVolume(pos) ) {
    return(m_vol.GetVoxelRaw(pos));
  } else {
    return(m_vol.GetBackground());
  }
} 


/**
 * Moves the iterator relative to its current position.
 * @param delta the location relative to the current location to move to.
 */
template <class L, class T>
void VxVolIterator<L,T>::MoveRel(const Vector<int2> & delta)
{
  m_pos.m_x += delta.m_x;
  m_pos.m_y += delta.m_y;
  m_pos.m_z += delta.m_z;
  m_inside = m_vol.InsideVolume(m_pos);
} // PeekRel


////////////////////////////////////////////////////////////////////////
/// INCREMENTAL PEEK AT SOME VOXEL

/**
 * Peeks at the next voxel in the x direction w/out moving the iterator.
 * @return the value at the location.
 */
template <class L, class T>
const T & VxVolIterator<L,T>::PeekNextX(const uint2 & delta) const
{
  uint2 posX = m_pos.m_x + delta;

  if ( posX >= m_vol.GetDim().m_x || // look from inside out
       !m_inside ) {                 // look from outside out
    return(m_vol.GetBackground());
  } else {
    return(m_vol.GetVoxelRaw(Point<uint2>(posX,m_pos.m_y,m_pos.m_z)));
  }
} // PeekIncX


/**
 * Peeks at the next voxel in the y direction w/out moving the iterator.
 * @return the value at the location.
 */
template <class L, class T>
const T & VxVolIterator<L,T>::PeekNextY(const uint2 & delta) const
{
  uint2 posY = m_pos.m_y + delta;

  if ( posY >= m_vol.GetDim().m_y || // look from inside out
       !m_inside ) {                 // look from outside out
    return(m_vol.GetBackground());
  } else {
    return(m_vol.GetVoxelRaw(Point<uint2>(m_pos.m_x,posY,m_pos.m_z)));
  }
} // PeekIncY


/**
 * Peeks at the next voxel in the z direction w/out moving the iterator.
 * @return the value at the location.
 */
template <class L, class T>
const T & VxVolIterator<L,T>::PeekNextZ(const uint2 & delta) const
{
  uint2 posZ = m_pos.m_z + delta;

  if ( posZ >= m_vol.GetDim().m_z || // look from inside out
       !m_inside ) {                 // look from outside out
    return(m_vol.GetBackground());
  } else {
    return(m_vol.GetVoxelRaw(Point<uint2>(m_pos.m_x,m_pos.m_y,posZ)));
  }
} // PeekIncZ


/**
 * Peeks at the prev voxel in the y direction w/out moving the iterator.
 * @return the value at the location.
 */
template <class L, class T>
const T & VxVolIterator<L,T>::PeekPrevX(const uint2 & delta) const
{
  uint2 posX = m_pos.m_x - delta;

  if ( posX >= m_vol.GetDim().m_x || // look from inside out
       !m_inside ) {                 // look from outside out
    return(m_vol.GetBackground());
  } else {
    return(m_vol.GetVoxelRaw(Point<uint2>(posX,m_pos.m_y,m_pos.m_z)));
  }
} // PeekDecX


/**
 * Peeks at the prev voxel in the y direction w/out moving the iterator.
 * @return the value at the location.
 */
template <class L, class T>
const T & VxVolIterator<L,T>::PeekPrevY(const uint2 & delta) const
{
  uint2 posY = m_pos.m_y - delta;

  if ( posY >= m_vol.GetDim().m_y || // look from inside out
       !m_inside ) {                 // look from outside out
    return(m_vol.GetBackground());
  } else {
    return(m_vol.GetVoxelRaw(Point<uint2>(m_pos.m_x,posY,m_pos.m_z)));
  }
} // PeekDecY


/**
 * Peeks at the prev voxel in the z direction w/out moving the iterator.
 * @return the value at the location.
 */
template <class L, class T>
const T & VxVolIterator<L,T>::PeekPrevZ(const uint2 & delta) const
{
  uint2 posZ = m_pos.m_z - delta;

  if ( posZ >= m_vol.GetDim().m_z || // look from inside out
       !m_inside ) {                 // look from outside out
    return(m_vol.GetBackground());
  } else {
    return(m_vol.GetVoxelRaw(Point<uint2>(m_pos.m_x,m_pos.m_y,posZ)));
  }
} // PeekDecZ


////////////////////////////////////////////////////////////////////////
/// INCREMENTAL WALK TO NEAR BY VOXELS


/**
 * Increments the iterator location in the x direction.
 */
template <class L, class T>
void VxVolIterator<L,T>::IncX(const uint2 & delta)
{
  m_pos.m_x += delta; // might step outside
  if ( m_pos.m_x >= m_vol.GetDim().m_x ) {
    m_inside = false;
  } else if ( !m_inside && m_vol.InsideVolume(m_pos) ) { // might step inside
    m_inside = true;
  }
} // IncX


/**
 * Increments the iterator location in the y direction.
 */
template <class L, class T>
void VxVolIterator<L,T>::IncY(const uint2 & delta)
{
  m_pos.m_y += delta; // might step outside
  if ( m_pos.m_y >= m_vol.GetDim().m_y ) {
    m_inside = false;
  } else if ( !m_inside && m_vol.InsideVolume(m_pos) ) { // might step inside
    m_inside = true;
  }
} // IncY


/**
 * Increments the iterator location in the z direction.
 */
template <class L, class T>
void VxVolIterator<L,T>::IncZ(const uint2 & delta)
{
  m_pos.m_z += delta; // might step outside
  if ( m_pos.m_z >= m_vol.GetDim().m_z ) {
    m_inside = false;
  } else if ( !m_inside && m_vol.InsideVolume(m_pos) ) { // might step inside
    m_inside = true;
  }
} // IncZ


/**
 * Decrements the iterator location in the x direction.
 */
template <class L, class T>
void VxVolIterator<L,T>::DecX(const uint2 & delta)
{
  m_pos.m_x -= delta; // might step outside
  if ( m_pos.m_x >= m_vol.GetDim().m_x ) {
    m_inside = false;
  } else if ( !m_inside && m_vol.InsideVolume(m_pos) ) { // might step inside
    m_inside = true;
  }
} // DecX


/**
 * Decrements the iterator location in the y direction.
 */
template <class L, class T>
void VxVolIterator<L,T>::DecY(const uint2 & delta)
{
  m_pos.m_y -= delta; // might step outside
  if ( m_pos.m_y >= m_vol.GetDim().m_y ) {
    m_inside = false;
  } else if ( !m_inside && m_vol.InsideVolume(m_pos) ) { // might step inside
    m_inside = true;
  }
} // DecY


/**
 * Decrements the iterator location in the z direction.
 */
template <class L, class T>
void VxVolIterator<L,T>::DecZ(const uint2 & delta)
{
  m_pos.m_z -= delta; // might step outside
  if ( m_pos.m_z >= m_vol.GetDim().m_z ) {
    m_inside = false;
  } else if ( !m_inside && m_vol.InsideVolume(m_pos) ) { // might step inside
    m_inside = true;
  }
} // DecZ


////////////////////////////////////////////////////////////////////////
/// STATUS REPORTS

/**
 * Returns true if the iterator is valid.
 * @return true if the VxVolIterator is inside the volume, false if the
 * the VxVolIterator was moved outside the volume and is no longer valid.
 */
template <class L, class T>
bool VxVolIterator<L,T>::InsideVolume() const
{
  return(m_inside);
} // InsideVolume


/**
 * Returns the current location of the iterator within the volume.
 * @return the x,y,z location in the volume.
 */
template <class L, class T>
const Point<uint2> & VxVolIterator<L,T>::GetPos() const
{
  return(m_pos);
} // GetPos


/**
 * Get Volume Dimension included in API for Convenience for other iterators.
 */
template <class L, class T>
const Triple<uint4> & VxVolIterator<L,T>::GetDim() const
{
  return(m_vol.GetDim());
} // GetDim


/**
 * Overloaded << Operator.
 *
 * @param Vox           type of the voxels in the data volume.
 */
template <class Layout, class Vox>
std::ostream & operator<<(std::ostream & os, const VxVolIterator<Layout, Vox> & iter)
{
  if ( iter.InsideVolume() ) {
    os << "VxVolIterator{Pos:"<<iter.GetPos() << "(inside)"  << "Vox:"<<iter.GetVoxel() << "}" << flush;
  } else {
    os << "VxVolIterator{Pos:"<<iter.GetPos() << "(outside)" << "Vox:"<<iter.GetVoxel() << "}" << flush;
  }
  return( os );
} // operator<<




// $Log: VxVolIterator.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.1  2003/05/16 11:48:13  michael
// code review
//
// Revision 3.0  2001/10/14 23:02:24  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:28   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:02   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:43  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:22  soeren
// renamed volume  directory
//
// Revision 1.24  2001/04/09 16:25:35  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.23  2001/04/08 21:38:23  dave
// updated doc headers
//
// Revision 1.22  2001/03/30 18:50:02  dmitry
// Updated datatypes.
//
// Revision 1.21  2000/11/16 17:19:49  dave
// added native BlockVolume VolIterator
//
// Revision 1.20  2000/11/14 01:59:11  sarang
// Added semi-colon..
// VC++'s poor compiler didn't find the syntax error!
//
// Revision 1.19  2000/11/10 17:34:06  dave
// impl. API changes for VolIterator
//
// Revision 1.18  2000/11/04 23:46:14  ingmar
// fixed casting problem
//
// Revision 1.17  2000/11/04 20:55:41  ingmar
// code beautify
//
// Revision 1.16  2000/11/04 03:08:56  ingmar
// Streamlined VolIterator
//
// Revision 1.15  2000/11/03 22:40:48  dongqing
// fixed last (?) ushort problem
//
// Revision 1.14  2000/11/03 20:34:02  ingmar
// moved volume dimension back to unit and made positions inside a volume ushort
//
// Revision 1.13  2000/11/03 18:20:15  ingmar
// moved volume dimension back to unit and made positions inside a volume ushort
//
// Revision 1.12  2000/11/03 17:02:27  dave
// converted back
//
// Revision 1.11  2000/11/02 19:48:10  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.10  2000/11/01 16:30:12  lihong
// debugged surface crawling decider + itreator
//
// Revision 1.9  2000/10/31 22:35:28  ingmar
// added tests and operator <<
//
// Revision 1.8  2000/10/24 03:06:00  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.7  2000/10/24 02:56:47  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.6  2000/10/23 20:18:08  ingmar
// removed VxDilator and added SpiralIterator
//
// Revision 1.5  2000/10/22 23:48:21  dave
// added RecBlockVolume
//
// Revision 1.4  2000/10/22 00:24:24  dave
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
//
// Revision 1.1.1.1  2000/08/11 22:15:52  ingmar
// unit tests complete
//
// 13    9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
//
// 12    8/25/00 5:18p Jay
// towards a wroking version of a region grow iterator
//
// 11    8/24/00 5:00p Ingmar
// region grow now with generic template decider
//
// 10    8/24/00 12:48a Ingmar
// started to change m_location from 3uint to 3int
//
// 9     8/23/00 3:09p Ingmar
// fixed wrong const functions
//
// 8     8/23/00 3:01p Ingmar
// fixed m_Background initialization
//
// 7     8/21/00 2:13a Sarang
// added header and footer
//
//*****************  Version 6  *****************
//User: Sarang          Date:  8/20/00  Time:  6:49p
//Checked in $/v2k/src/Volume
//Comment:
//  The whole pipeline now works under linux+gcc. Removed tons of bugs
//and syntax errors found by gcc.
//
//*****************  Version 5  *****************
//User: Ingmar          Date:  8/19/00  Time:  6:19p
//Checked in $/v2k/src/Volume
//Comment:
//  SetVoxelRaw
//
//*****************  Version 4  *****************
//User: Ingmar          Date:  8/19/00  Time:  3:16p
//Checked in $/v2k/src/Volume
//Comment:
//  GetVoxel/GetVoxelRaw + inline Error::DebugMsg
//
//*****************  Version 3  *****************
//User: Jay             Date:  8/09/00  Time: 11:54p
//Checked in $/TemplateVol/src/Volume
//Comment:
//  Template Volumes working.
//
//*****************  Version 2  *****************
//User: Jay             Date:  8/09/00  Time: 10:41p
//Checked in $/TemplateVol/src/Volume
//Comment:
//  added iterator template classes
//
//*****************  Version 1  *****************
//User: Jay             Date:  8/09/00  Time:  4:03p
//Created vxVolIterator.inl
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/VxVolIterator.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: VxVolIterator.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
