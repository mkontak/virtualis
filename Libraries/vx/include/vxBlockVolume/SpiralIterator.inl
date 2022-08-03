// $Id: SpiralIterator.inl,v 1.3 2007/04/10 01:30:16 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created:  summer 00
// Author: Jay B. jay@viatronix.net


/**
 * Constructor for a SpiralIterator.
 * @param iter   iterator value
 */
template <class I, class T>
SpiralIterator<I,T>::SpiralIterator(const I & iter)
: m_iter(iter), m_iDelta(0), m_eState(START)
{
  if ( !m_iter.InsideVolume() ) 
  {
    Error::Warn("<SpiralIterator::contructor> : seed outside volume");
  }
  m_seed = m_iter.GetPos();
} // constructor


/**
 * Overloaded ++ operator.
 */
template <class I, class T>
inline void SpiralIterator<I,T>::operator++()
{
  Increment();
} // operator++()


/**
 * @return true when the traversal is finished.
 */
template <class I, class T>
bool SpiralIterator<I,T>::End() const
{
  return( !m_iter.InsideVolume() );
} // End()


/**
 * Returns the value at the current location.
 * @return the value at the location.
 */
template <class I, class T>
const T & SpiralIterator<I,T>::GetVoxel() const
{
  return(m_iter.GetVoxel());
} // GetVoxel()


/**
 * Returns the value at the current location.
 * @return the value at the location.
 */
template <class I, class T>
const T & SpiralIterator<I,T>::GetVoxelRaw() const
{
  return(m_iter.GetVoxelRaw());
} // GetVoxelRaw()


/**
 * set the voxel value
 * @param value    value to set
 */
template <class I, class T>
void SpiralIterator<I,T>::SetVoxel(const T & value)
{
  m_iter.SetVoxel(value);
} // SetVoxel()


/**
 * set the voxel value
 * @param value    value to set
 */
template <class I, class T>
void SpiralIterator<I,T>::SetVoxelRaw(const T & value)
{
  m_iter.SetVoxelRaw(value);
} // SetVoxelRaw()


/**
 * Returns the current position of the SpiralIterator.
 * @return the current position.
 */
template <class I, class T>
inline const Point<uint2> & SpiralIterator<I,T>::GetPos() const
{
  return(m_iter.GetPos());
} // GetPos()


/**
 * Increments the location of the iterator in the 3D order.
 * @return the value at the current location.
 */
template <class I, class T> void
SpiralIterator<I,T>::Increment()
{
  /*
  // The main logic for the iterator
  switch ( m_state ) {
  case START:
    {
      ++m_delta;
      if (m_delta > Max(m_iter.GetDim().m_x, m_iter.GetDim().m_y, m_iter.GetDim().m_z)) {
        return;
      }
      m_iter.MoveTo(Point<ushort>(m_seed.m_x-m_delta,
                                       m_seed.m_y-m_delta,
                                       m_seed.m_z-m_delta));
      m_state = BASE;
      break;
    }
  case BASE:{
      if ( m_iter.GetPos().m_x <  m_seed.m_x+m_delta &&
           m_iter.GetPos().m_z <= m_seed.m_z+m_delta ) {
        m_iter.IncX();
      } else if ( m_iter.GetPos().m_x == m_seed.m_x+m_delta &&
                  m_iter.GetPos().m_z <  m_seed.m_z+m_delta ) {
        m_iter.MoveToX(m_seed.m_x-m_delta);
        m_iter.IncZ();
      } else if ( m_iter.GetPos().m_x == m_seed.m_x+m_delta &&
                  m_iter.GetPos().m_z == m_seed.m_z+m_delta ) {
        m_iter.MoveToX(m_seed.m_x-m_delta);
        m_iter.IncY();
        m_iter.MoveToZ(m_seed.m_z-m_delta);
        m_state = MID;
      }
      break;}
  case MID:{
      if ( m_iter.GetPos().m_x < m_seed.m_x+m_delta &&
           m_iter.GetPos().m_z == m_seed.m_z-m_delta ) {
        m_iter.IncX();
      } else if ( m_iter.GetPos().m_x == m_seed.m_x+m_delta &&
                  m_iter.GetPos().m_z <  m_seed.m_z+m_delta ) {
        m_iter.IncZ();
      } else if ( m_iter.GetPos().m_x <= m_seed.m_x+m_delta &&
                  m_iter.GetPos().m_z == m_seed.m_z+m_delta &&
                  m_iter.GetPos().m_x >  m_seed.m_x-m_delta ) {
        m_iter.DecX();
      } else if ( m_iter.GetPos().m_x == m_seed.m_x-m_delta &&
                  m_iter.GetPos().m_z <= m_seed.m_z+m_delta &&
                  m_iter.GetPos().m_z > m_seed.m_z-m_delta+1 ) {
        m_iter.DecZ();
      } else if ( m_iter.GetPos().m_x == m_seed.m_x-m_delta &&
                  m_iter.GetPos().m_z == m_seed.m_z-m_delta+1 &&
                  m_iter.GetPos().m_y < m_seed.m_y+m_delta-1 ) {
        m_iter.DecZ();
        m_iter.IncY();
      } else if ( m_iter.GetPos().m_x == m_seed.m_x-m_delta &&
                  m_iter.GetPos().m_y == m_seed.m_y+m_delta-1 &&
                  m_iter.GetPos().m_z == m_seed.m_z-m_delta+1 ) {
        m_iter.DecZ();
        m_iter.IncY();
        m_state = TOP;
      }
      break;}
  case TOP:{
      if ( m_iter.GetPos().m_x <  m_seed.m_x+m_delta &&
           m_iter.GetPos().m_z <= m_seed.m_z+m_delta ) {
        m_iter.IncX();
      } else if ( m_iter.GetPos().m_x == m_seed.m_x+m_delta &&
                  m_iter.GetPos().m_z <  m_seed.m_z+m_delta ) {
        m_iter.MoveToX(m_seed.m_x-m_delta);
        m_iter.IncZ();
      } else if ( m_iter.GetPos().m_x == m_seed.m_x+m_delta &&
                  m_iter.GetPos().m_z == m_seed.m_z+m_delta ) {
        ++m_delta;
        m_iter.MoveTo(Point<ushort>(m_seed.m_x-m_delta,
                                         m_seed.m_y-m_delta,
                                         m_seed.m_z-m_delta));
        m_state = BASE;
      }
      break;}
  }
  if ( m_iter.GetPos().m_x >= m_iter.GetDim().m_x ||
       m_iter.GetPos().m_y >= m_iter.GetDim().m_y ||
       m_iter.GetPos().m_z >= m_iter.GetDim().m_z ) {
    Increment();
  }
  */
} // Increment()


// $Log: SpiralIterator.inl,v $
// Revision 1.3  2007/04/10 01:30:16  jmeade
// code review.
//
// Revision 1.2  2007/03/06 18:58:23  dongqing
// code review
//
// Revision 1.1  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:02:24  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:26   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:00   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:43  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.9  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.8  2001/04/05 18:00:47  dmitry
// updated datatypes
//
// Revision 1.7  2000/11/04 03:08:56  ingmar
// Streamlined VolIterator
//
// Revision 1.6  2000/11/03 22:55:45  ingmar
// added recursion termination
//
// Revision 1.5  2000/11/03 19:10:49  ingmar
// fixed gcc warnings
//
// Revision 1.4  2000/11/03 17:02:26  dave
// converted back
//
// Revision 1.3  2000/10/25 05:28:05  antonio
// changed class L to class I in constructor.
//
// Revision 1.2  2000/10/25 01:35:25  ingmar
// removed VxIterator and improved VxVolIterator and RegGrow6Iterator
//
// Revision 1.1  2000/10/23 20:18:08  ingmar
// removed VxDilator and added SpiralIterator
//
// Revision 1.3  2000/10/21 06:06:03  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.2  2000/10/19 13:09:11  ingmar
// mergerd Franks fixes of the constructor
//
// Revision 1.1.1.1  2000/08/11 22:15:52  ingmar
// unit tests complete
//
// Revision 1.1.1.1  2000/08/11 22:15:52  ingmar
// unit tests complete
//
// 8     9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
//
// 7     9/01/00 5:56p Dave
// initial code walkthrough changes
//
// 6     8/21/00 2:09a Sarang
// added header and footer
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
//User: Jay             Date:  8/09/00  Time:  7:46p
//Created SpiralIterator.inl
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/SpiralIterator.inl,v 1.3 2007/04/10 01:30:16 jmeade Exp $
// $Id: SpiralIterator.inl,v 1.3 2007/04/10 01:30:16 jmeade Exp $





