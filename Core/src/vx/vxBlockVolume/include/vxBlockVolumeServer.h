// $Id: vxBlockVolumeServer.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
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

/**
 * lock volume server. Can make and return volumes of any resolution.
 */

#ifndef vxBlockVolumeServer_h
#define vxBlockVolumeServer_h



// include declarations
#include "vxBlockVolume.h"
#include "vxBlockVolumeFilter.h"

/**
 * Block volume server. Can make and return volumes of any resolution.
 */
template < class VoxelType >
class vxBlockVolumeServer 
{
  // public member functions
public:

  /////////////////////////////////////////////////////////////////////////////
  // Constructor & assignment

  /// From volume Contructor.
  vxBlockVolumeServer(const vxBlockVolume<VoxelType> & rVolume);
  
  /// From file Constructor.
  vxBlockVolumeServer(const std::string & sFileName);

  /// Destructor.
  ~vxBlockVolumeServer();

  

  /////////////////////////////////////////////////////////////////////////////
  // I/O

  /// Read Volume(s) from disk.
  void Read(const std::string & sFileName);

  /// Write Volume(s) to disk.
  void Write(const std::string & sFileName = "sameAsInHeader");



  /////////////////////////////////////////////////////////////////////////////
  // Volume serving

  /// return a volume according to requested volume dimensions
  vxBlockVolume<VoxelType> * GetVolumeAccordingToVolDim( const Triple<int4> & viDesiredVolumeDim, bool bForceDim = false);

  /// return a volume according to requested voxel units
  vxBlockVolume<VoxelType> * GetVolumeAccordingToVoxelUnits( const Triple<float4> & viDesiredVoxelUnits, bool bForceUnits = false);
  
  /// return the volume with highest volume dimension available
  vxBlockVolume<VoxelType> * GetVolumeMaxVolDimAvailable();
  

  
  /////////////////////////////////////////////////////////////////////////////
  // Memory management

  /// report freeable memory
  uint4 GetMemoryFreeableAll() const;

  /// report freeable memory
  uint4 GetMemoryFreeableOnlyMaxVolDim() const;

  /// report freeable memory
  uint4 GetMemoryFreeableAllButMaxVolDim() const;

  /// free memory
  void FreeMemory(uint4 uMinNumBytesToFree);



  /////////////////////////////////////////////////////////////////////////////
  // Get & Set Volume Filters

  /// Gets the volume filter used to interpolate from a small to a large volume
  const vxBlockVolumeFilter<VoxelType> & GetVolumeFilterScaleVolDimUp();

  /// Sets the volume filter used to interpolate from a small to a large volume
  void SetVolumeFilterScaleVolDimUp(vxBlockVolumeFilter<VoxelType> & volumeFilter);

  /// Gets the volume filter used to sample a large volume into a small one
  const vxBlockVolumeFilter<VoxelType> & GetVolumeFilterScaleVolDimDown();

  /// Sets the volume filter used to sample a large volume into a small one
  void SetVolumeFilterScaleVolDimDown(vxBlockVolumeFilter<VoxelType> & volumeFilter);

// private (locked) member functions
private:

  /// Default Contructor.
  vxBlockVolumeServer();

  /// Copy Contructor.
  vxBlockVolumeServer(const vxBlockVolumeServer<VoxelType> & src);

  /// assignment operator
  vxBlockVolumeServer<VoxelType> const & operator= (const vxBlockVolumeServer<VoxelType> & rhs);


// private data members
private:

  /// the list of all volumes managed
  std::vector< vxBlockVolume<VoxelType> * > m_volumeList;

  /// the volume filter to interpolate from a small to a large volume
  vxBlockVolumeFilter<VoxelType> * m_pFilterVolDimUp;

  /// the volume filter to sample a large volume into a small one
  vxBlockVolumeFilter<VoxelType> * m_pFilterVolDimDown;

}; // class vxBlockVolumeServer


#endif // vxBlockVolumeServer_h


// $Log: vxBlockVolumeServer.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/05/16 11:54:11  michael
// code review
//
// Revision 1.5  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.4  2003/01/22 21:36:06  ingmar
// dimension and units are now Triples
//
// Revision 1.3  2002/05/14 21:36:42  ingmar
// added & to function parameter
//
// Revision 1.2  2002/05/14 17:54:45  ingmar
// first working Demo() and Test() of volume server
//
// Revision 1.1  2002/04/24 19:55:58  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeServer.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeServer.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $