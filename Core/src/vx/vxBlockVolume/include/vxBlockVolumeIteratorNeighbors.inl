// $Id: vxBlockVolumeIteratorNeighbors.inl,v 1.2.12.1 2010/07/16 20:31:28 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Rekha (ingmar|rekha)@viatronix.com


/** 
 * Constructor
 * @param volume
 * @param center point
 * @param neighbour enum
 */
template <class VoxelType>
vxBlockVolumeIteratorNeighbors<VoxelType>::vxBlockVolumeIteratorNeighbors(vxBlockVolume<VoxelType> & vol,
                                                                          const Vector3D<int4> & centerPoint,
                                                                          NeigborhoodsEnum eNeighborhood)
: m_volIter(vol, centerPoint)
{
  m_eNeighborhood = eNeighborhood;

  switch (eNeighborhood)
  {
  case vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors04xy:
    {
      m_iNumberOfNeighbors = 4;
      m_NextNeighborFunctionPtr = &vxBlockVolumeIteratorNeighbors<VoxelType>::Next2Dneighbor04xy;
      break;
    }
  case vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors04xz:
    {
      m_iNumberOfNeighbors = 4;
      m_NextNeighborFunctionPtr = &vxBlockVolumeIteratorNeighbors<VoxelType>::Next2Dneighbor04xz;
      break;
    }
  case vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors04yz:
    {
      m_iNumberOfNeighbors = 4;
      m_NextNeighborFunctionPtr = &vxBlockVolumeIteratorNeighbors<VoxelType>::Next2Dneighbor04yz;
      break;
    }

  case vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors08xy:
    {
      m_iNumberOfNeighbors = 8;
      m_NextNeighborFunctionPtr = &vxBlockVolumeIteratorNeighbors<VoxelType>::Next2Dneighbor08xy;
      break;
    }
  case vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors08xz:
    {
      m_iNumberOfNeighbors = 8;
      m_NextNeighborFunctionPtr = &vxBlockVolumeIteratorNeighbors<VoxelType>::Next2Dneighbor08xz;
      break;
    }
  case vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors08yz:
    {
      m_iNumberOfNeighbors = 8;  
      m_NextNeighborFunctionPtr = &vxBlockVolumeIteratorNeighbors<VoxelType>::Next2Dneighbor08yz;
      break;
    }
  case vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors07:
    {
      m_iNumberOfNeighbors = 7;
      m_NextNeighborFunctionPtr = &vxBlockVolumeIteratorNeighbors<VoxelType>::Next3Dneighbor07;
      break;
    }
  case vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06:
    {
      m_iNumberOfNeighbors = 6;
      m_NextNeighborFunctionPtr = &vxBlockVolumeIteratorNeighbors<VoxelType>::Next3Dneighbor06;
      break;
    }
  case vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors18:
    {
      m_iNumberOfNeighbors = 18;
      m_NextNeighborFunctionPtr = &vxBlockVolumeIteratorNeighbors<VoxelType>::Next3Dneighbor18;
      break;
    }
  case vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors26:
    {
      m_iNumberOfNeighbors = 26;
      m_NextNeighborFunctionPtr = &vxBlockVolumeIteratorNeighbors<VoxelType>::Next3Dneighbor26;
      break;
    }
  case vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors63:
    {
      m_iNumberOfNeighbors = 63;
      m_NextNeighborFunctionPtr = &vxBlockVolumeIteratorNeighbors<VoxelType>::Next3Dneighbor63;
      break;
    }
  }
  CenterAt(centerPoint);
} // constructor


/**
 * Prefix increment operator for iterator location.  Since no native, 
 * faster ++ exists this just calls NextZYX()
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::operator++()
{
  (this->*m_NextNeighborFunctionPtr)();
} // operator++()


/**
 * Status of the volume traversal
 * @return true if the iterator has been advanced beyond the volume boundaries
 */
template <class VoxelType>
bool vxBlockVolumeIteratorNeighbors<VoxelType>::IsAtEnd() const
{
  return m_iNeighborIndex > m_iNumberOfNeighbors;
} // IsAtEnd()


/**
 * Returns the value at the current location; Safe version
 * @return const reference to the voxel
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeIteratorNeighbors<VoxelType>::GetVoxel() const
{
  return m_volIter.GetVoxel();
} // GetVoxel()


/**
 * Sets the value at the current location
 * @param value  The new voxel value
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::SetVoxel(const VoxelType & value)
{
  m_volIter.SetVoxel(value);
} // SetVoxel()


/**
 * Get positions
 * @return the current x,y,z position, computed from the current voxel pointer offset
 * @param value  The position.
 */
template <class VoxelType>
const Vector3D<int4> vxBlockVolumeIteratorNeighbors<VoxelType>::GetPos() const
{
  return m_volIter.GetPos();
} // GetPos()


/**
 * Sets the current x,y,z position
 * This is computes and sets the corresponding voxel pointer offset
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::CenterAt(const Vector3D<int4> & pos)
{
  m_volIter.SetPos(pos);
  m_iNeighborIndex = 0;
  operator++();
} // CenterAt()


/**
 * Sets the iterator to the given absolute position
 * @param packed block offset
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::CenterFromPackedBlockOffsets(uint8 iPackedBlockOffsets)
{
  m_volIter.SetPosFromPackedBlockOffsets(iPackedBlockOffsets);
  m_iNeighborIndex = 0;
  operator++(); 
} // CenterFromPackedBlockOffsets()


/**
 * Sets the current x,y,z position.
 * This is computes and sets the corresponding voxel pointer offset.
 * @param position in X
 * @param position in Y
 * @param position in Z
 */
template <class VoxelType>
void vxBlockVolumeIteratorNeighbors<VoxelType>::CenterAt(const int4 iX, const int4 iY, const int4 iZ)
{
  m_volIter.SetPos(iX,iY,iZ);
  m_iNeighborIndex = 0;
  operator++();
} // CenterAt()


/**
 * This is computed from the current voxel pointer, hence
 * overruns in x and y are NOThandled corretly, but it is FAST.
 * @return true if the iterator is inside the volume.
 */
template <class VoxelType>
bool vxBlockVolumeIteratorNeighbors<VoxelType>::IsInsideVolume() const
{
  return m_volIter.IsInsideVolume();
} // IsInsideVolume()


/**
 * Returns the number of voxels in the neighborhood
 * @param number of neighbours
 */
template <class VoxelType>
int4 vxBlockVolumeIteratorNeighbors<VoxelType>::GetNumberOfNeighbors() const
{
  return m_iNumberOfNeighbors;
} // GetNumberOfNeighbors()


/**
 * Get volume
 * @return volume
 */
template <class VoxelType>
const vxBlockVolume<VoxelType> & vxBlockVolumeIteratorNeighbors<VoxelType>::GetVolume() const 
{
  return m_volIter.GetVolume();
} // GetVolume()


/**
 * Get internal volume iterator
 * @return volume iterator
 */
template <class VoxelType>
const vxBlockVolumeIterator<VoxelType> & vxBlockVolumeIteratorNeighbors<VoxelType>::GetInternalVolIter() const
{
  return m_volIter;
}


// $Log: vxBlockVolumeIteratorNeighbors.inl,v $
// Revision 1.2.12.1  2010/07/16 20:31:28  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.2  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2003/07/08 19:20:57  michael
// added Get() for chosen neighbourhood iteration scheme
//
// Revision 1.15  2003/05/03 16:18:19  michael
// completed code review
//
// Revision 1.14  2003/05/02 14:36:56  dongqing
// code walk through
//
// Revision 1.13  2002/09/25 21:20:25  ingmar
// implemented operator<< inside the class declaration
//
// Revision 1.12  2002/05/14 21:52:09  ingmar
// added e3Dneighbors07 for traversal of 2x2x2 neighborhood
//
// Revision 1.11  2002/05/13 20:11:08  ingmar
// added GetNumberOfNeighbors()
//
// Revision 1.10  2002/05/03 21:10:50  soeren
// added GetInternalVolIter
//
// Revision 1.9  2002/04/30 13:51:06  ingmar
// added Get/SetPos From PackedBlockOffsets
//
// Revision 1.8  2002/04/25 17:11:41  ingmar
// code beautify
//
// Revision 1.7  2002/04/25 16:35:34  ingmar
// added e2Dneighbors08 xy, xz and zy neighbor traversal
//
// Revision 1.6  2002/04/25 16:13:19  ingmar
// added e2Dneighbors04xz and zy neighbor traversal
//
// Revision 1.5  2002/04/25 16:02:28  ingmar
// added e2Dneighbors04xy neighbor traversal
//
// Revision 1.4  2002/04/18 19:53:45  ingmar
// removed obsolete GetVoxelRaw() from vxBlockVolumeIterator class
//
// Revision 1.3  2002/04/12 14:59:25  rekha
// extended to 6 & 18 & 26 & 63 neighborhoods
//
// Revision 1.2  2002/04/09 21:20:41  ingmar
// first working test
//
// Revision 1.1  2002/04/09 18:46:49  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeIteratorNeighbors.inl,v 1.2.12.1 2010/07/16 20:31:28 dongqing Exp $
// $Id: vxBlockVolumeIteratorNeighbors.inl,v 1.2.12.1 2010/07/16 20:31:28 dongqing Exp $
