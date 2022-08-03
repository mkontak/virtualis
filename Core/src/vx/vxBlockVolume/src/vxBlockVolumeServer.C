// $Id: vxBlockVolumeServer.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com
//
// Complete history at bottom of file.


// include declarations
#include "stdafx.h"
#include "vxBlockVolumeServer.h"
#include "Vector3D.h"


// define declarations
#define FILE_REVISION "$Revision: 1.1.1.1 $"

template class VX_BLOCK_VOLUME_DLL vxBlockVolumeServer< int1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeServer< int2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeServer< int4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeServer<uint1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeServer<uint2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeServer<uint4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeServer<bool>;

using namespace std;



/////////////////////////////////////////////////////////////////////////////
// Constructor & assignment

/**
 * From volume constructor.
 * @param rVolume
 */
template <class VoxelType> 
vxBlockVolumeServer<VoxelType>::vxBlockVolumeServer(const vxBlockVolume<VoxelType> & rVolume)
{
  vxBlockVolume<VoxelType> * pVolume = new vxBlockVolume<VoxelType>(rVolume);
  m_volumeList.push_back(pVolume);
} // from volume constructor

/**
 * default constructor.
 */
template <class VoxelType> 
vxBlockVolumeServer<VoxelType>::vxBlockVolumeServer()
{
} // from volume constructor


/**
 * Destructor.
 */
template <class VoxelType> 
vxBlockVolumeServer<VoxelType>::~vxBlockVolumeServer()
{
  std::vector< vxBlockVolume<VoxelType> * >::iterator volListIter = m_volumeList.begin();
  for ( ;   volListIter != m_volumeList.end();   ++volListIter )
  {
    delete (*volListIter);
  }
} // Destructor


/**
 * Copy Contructor.
 * @param src
 */
template <class VoxelType> 
vxBlockVolumeServer<VoxelType>::vxBlockVolumeServer(const vxBlockVolumeServer<VoxelType> & src)
{

}


/**
 * From file constructor.
 * @param sFileName
 */
template <class VoxelType> 
vxBlockVolumeServer<VoxelType>::vxBlockVolumeServer(const std::string & sFileName)
{
  Read(sFileName);
} // From file constructor



/////////////////////////////////////////////////////////////////////////////
// I/O

/**
 * Read from disk.
 * @parm sFilename
 */
template <class VoxelType> 
void vxBlockVolumeServer<VoxelType>::Read(const std::string & sFileName)
{
  vxBlockVolume<VoxelType> * pVolume = new vxBlockVolume<VoxelType>(sFileName);
  m_volumeList.push_back(pVolume);
} // Read()


/**
 * Write to disk.
 * @param sFilename
 */
template <class VoxelType> 
void vxBlockVolumeServer<VoxelType>::Write(const std::string & sFileName)
{
  (*m_volumeList.begin())->Write();
} // Write()



/////////////////////////////////////////////////////////////////////////////
// Volume serving

/**
 * @param iDesiredVolumeDim
 * @param bForceDim
 * @return a pointer to a volume according to requested volume dimensions.
 */
template <class VoxelType> 
vxBlockVolume<VoxelType> * vxBlockVolumeServer<VoxelType>::GetVolumeAccordingToVolDim(const Triple<int4> & iDesiredVolumeDim, bool bForceDim)
{
  // first check if we have a volume of that size
  {
    std::vector< vxBlockVolume<VoxelType> * >::iterator volListIter = m_volumeList.begin();
    std::vector< vxBlockVolume<VoxelType> * >::iterator volListEnd  = m_volumeList.end();
    for ( vxBlockVolume<VoxelType> * pVol = *volListIter;   volListIter != volListEnd;   ++volListIter )
    {
      if ( (*volListIter)->GetHeader().GetVolDim() == iDesiredVolumeDim )
      {
        return(*volListIter);
      }
    }
  }

  // set the volume filter to use (filter down by default
  vxBlockVolumeFilter<VoxelType> * pVolumeFilter = m_pFilterVolDimDown;

  // not found, so find the smallest present volume that has more voxels than the one requested
  int4 iDesiredNumberOfVoxels = iDesiredVolumeDim.X() * iDesiredVolumeDim.Y() * iDesiredVolumeDim.Z();
  vxBlockVolume<VoxelType> * pSrcVolume = *m_volumeList.begin();
  {
    std::vector< vxBlockVolume<VoxelType> * >::iterator volListIter = m_volumeList.begin();
    std::vector< vxBlockVolume<VoxelType> * >::iterator volListEnd  = m_volumeList.end();
    for ( vxBlockVolume<VoxelType> * pVol = *volListIter;   volListIter != volListEnd;   ++volListIter )
    {
      if ( (*volListIter)->GetHeader().GetNumberOfVoxels() < pSrcVolume->GetHeader().GetNumberOfVoxels()
           && (*volListIter)->GetHeader().GetNumberOfVoxels() > iDesiredNumberOfVoxels )
      {
        pSrcVolume = *volListIter;
      }
    }
  }

  // in case we don't have a larger volume select the next smaller one
  if ( pSrcVolume->GetHeader().GetNumberOfVoxels() < iDesiredNumberOfVoxels )
  {
    std::vector< vxBlockVolume<VoxelType> * >::iterator volListIter = m_volumeList.begin();
    std::vector< vxBlockVolume<VoxelType> * >::iterator volListEnd  = m_volumeList.end();
    for ( vxBlockVolume<VoxelType> * pVol = *volListIter;   volListIter != volListEnd;   ++volListIter )
    {
      if ( (*volListIter)->GetHeader().GetNumberOfVoxels() > pSrcVolume->GetHeader().GetNumberOfVoxels() )
      {
        pSrcVolume = *volListIter;
      }
    }
    // also change the filter to the up filter
    pVolumeFilter = m_pFilterVolDimUp;
  }

  // make a new volume and aad it to the volume list
  vxBlockVolume<VoxelType> * pDstVol = new vxBlockVolume<VoxelType>(pSrcVolume->GetHeader());
  m_volumeList.push_back( pDstVol );
  Triple<float4> fSrcDim(pSrcVolume->GetHeader().GetVolDim().X(), pSrcVolume->GetHeader().GetVolDim().Y(), pSrcVolume->GetHeader().GetVolDim().Z());
  Triple<float4> fDstDim(iDesiredVolumeDim.X(), iDesiredVolumeDim.Y(), iDesiredVolumeDim.Z());
  Triple<float4> fSrcUnits = pSrcVolume->GetHeader().GetVoxelUnits();
  Vector3Df fUnits(fSrcUnits); 
  fUnits.ScaleAnIso(fSrcDim).DivideAnIso(fDstDim);
  Triple<float4> fDstUnits(fUnits);
  pDstVol->GetHeader().SetVolDim(iDesiredVolumeDim);
  pDstVol->GetHeader().SetVoxelUnits(fDstUnits);

  // set the volumes the filter should work on
  pVolumeFilter->SetSrcVolume(*pSrcVolume);
  pVolumeFilter->SetDstVolume(*pDstVol);

  // run the filter
  pVolumeFilter->ApplyFilter();

  return(pDstVol);
} // GetVolumeAccordingToVolDim()


/**
 * @param viDesiredVoxelUnits
 * @param bForceUnits
 * @return a pointer to a volume according to requested voxel units.
 */
template <class VoxelType> 
vxBlockVolume<VoxelType> * vxBlockVolumeServer<VoxelType>::GetVolumeAccordingToVoxelUnits( const Triple<float4> & viDesiredVoxelUnits, bool bForceUnits)
{
  return((*m_volumeList.begin()));
} // GetVolumeAccordingToVoxelUnits()


/**
 * @return a pointer to the volume with highest volume dimension available.
 */
template <class VoxelType> 
vxBlockVolume<VoxelType> * vxBlockVolumeServer<VoxelType>::GetVolumeMaxVolDimAvailable()
{
  // not found, so find the smallest present volume that has more voxels than the one requested
  vxBlockVolume<VoxelType> * pVolume = *m_volumeList.begin();
  {
    std::vector< vxBlockVolume<VoxelType> * >::iterator volListIter = m_volumeList.begin();
    std::vector< vxBlockVolume<VoxelType> * >::iterator volListEnd  = m_volumeList.end();
    for ( ; volListIter != volListEnd;   ++volListIter )
    {
      if ( (*volListIter)->GetHeader().GetNumberOfVoxels() > pVolume->GetHeader().GetNumberOfVoxels() )
      {
        pVolume = *volListIter;
      }
    }
  }
  return(pVolume);
} // GetVolumeMaxVolDimAvailable()



/////////////////////////////////////////////////////////////////////////////
// Memory management


/**
 * report freeable memory.
 * @return
 */
template <class VoxelType> 
uint4 vxBlockVolumeServer<VoxelType>::GetMemoryFreeableAll() const
{
  return(0);
} // GetMemoryFreeableAll()


/**
 * report freeable memory.
 * @return
 */
template <class VoxelType> 
uint4 vxBlockVolumeServer<VoxelType>::GetMemoryFreeableOnlyMaxVolDim() const
{
  return(0);
} // GetMemoryFreeableOnlyMaxVolDim()


/**
 * report freeable memory.
 * @return
 */
template <class VoxelType> 
uint4 vxBlockVolumeServer<VoxelType>::GetMemoryFreeableAllButMaxVolDim() const
{
  return(0);
} // GetMemoryFreeableAllButMaxVolDim()


/**
 * report freeable memory.
 * @param uMinNumBytesToFree
 */
template <class VoxelType> 
void vxBlockVolumeServer<VoxelType>::FreeMemory(uint4 uMinNumBytesToFree)
{
} // FreeMemory()



/////////////////////////////////////////////////////////////////////////////
// Get & Set Volume Filters

/**
 * Gets the volume filter used to interpolate from a small to a large volume.
 * @return
 */
template <class VoxelType> 
const vxBlockVolumeFilter<VoxelType> & vxBlockVolumeServer<VoxelType>::GetVolumeFilterScaleVolDimUp()
{
  return(*m_pFilterVolDimUp);
} // GetVolumeFilterScaleVolDimUp()

/**
 * Sets the volume filter used to interpolate from a small to a large volume.
 * @param volumeFilter
 */
template <class VoxelType> 
void vxBlockVolumeServer<VoxelType>::SetVolumeFilterScaleVolDimUp(vxBlockVolumeFilter<VoxelType> & volumeFilter)
{
  m_pFilterVolDimUp = &volumeFilter;
} // SetVolumeFilterScaleVolDimUp()

/**
 * Gets the volume filter used to sample a large volume into a small one.
 * @return
 */
template <class VoxelType> 
const vxBlockVolumeFilter<VoxelType> & vxBlockVolumeServer<VoxelType>::GetVolumeFilterScaleVolDimDown()
{
  return(*m_pFilterVolDimDown);
} // GetVolumeFilterScaleVolDimDown()

/**
 * Sets the volume filter used to sample a large volume into a small one.
 * @param volumeFilter
 */
template <class VoxelType> 
void vxBlockVolumeServer<VoxelType>::SetVolumeFilterScaleVolDimDown(vxBlockVolumeFilter<VoxelType> & volumeFilter)
{
  m_pFilterVolDimDown = &volumeFilter;
} // SetVolumeFilterScaleVolDimDown()

/**
 * @return
 */
template <class VoxelType> 
vxBlockVolumeServer<VoxelType> const & vxBlockVolumeServer<VoxelType>::operator= (const vxBlockVolumeServer<VoxelType> & rhs)
{
  return(*this);
} // operator= 


#undef FILE_REVISION

// $Log: vxBlockVolumeServer.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/05/16 11:54:11  michael
// code review
//
// Revision 1.8  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.7  2003/05/15 15:03:46  frank
// spelling
//
// Revision 1.6  2003/01/22 21:36:06  ingmar
// dimension and units are now Triples
//
// Revision 1.5  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.4  2002/05/14 21:36:42  ingmar
// added & to function parameter
//
// Revision 1.3  2002/05/14 17:54:45  ingmar
// first working Demo() and Test() of volume server
//
// Revision 1.2  2002/05/10 23:03:07  soeren
// removed some unnecessary functions and warnings
//
// Revision 1.1  2002/04/24 19:55:58  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeServer.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxBlockVolumeServer.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
