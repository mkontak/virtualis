// $Id: vxComponentUtils.h,v 1.15.2.1 2009/08/31 13:16:29 kchalupa Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once

/**
  *  This class is for component utilities of viewer
  */

// includes
#include "Typedef.h"
#include "Box.h"
#include "vxShareableObject.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"


// forward declarations
template <class VoxelType> class vxBlockVolume;
template <class Type> class vxPlaneOblique;
class vxComponentArray;
class vxEnvironment;
class InteractiveSegmentation;
class vxComponent;
class vxComponentArray;
class vxRenderingMode;


namespace vxComponentUtils
{

  /// create a map of contiguous indices from the possibly non contiguous label volume
  VX_VIEWER_DLL std::map<uint2,uint2> CreateMapToCompactVolume( vxBlockVolume<uint2> & volumeLabel );

  /// write out as new volume
  VX_VIEWER_DLL  void WriteComponentsAsSeparateVolumes( vxBlockVolume<uint2> rIntensityVol, vxBlockVolume<uint2> rLabelVol, int4 iNumLabels );

  /// create a new component given a bool volume and the elements from the environment
  VX_VIEWER_DLL  uint2 CreateNewComponent( vxBlockVolume<bool> & desiredVoxels, const std::string & sName,
                                   vxShareableObject< vxComponentArray > * pComponentArray,
                                   vxShareableObject< vxBlockVolume<uint2> > * pLabelVolume,
                                   InteractiveSegmentation & interactiveSegmentation, bool bTestForLockedVoxels );

  /// create a new component given a bool volume and the environment
  VX_VIEWER_DLL  uint2 CreateNewComponent( vxBlockVolume<bool> & desiredVoxels, const std::string & sName,
                                           vxEnvironment & environmet, bool bTestForLockedVoxels );

  /// Create a new component given the environment with empty ROI that will be filled later.
  /// It will return the new component label index, but will not update the label volume 
  VX_VIEWER_DLL  uint2 CreateNewEmptyComponent( const std::string & sName, vxEnvironment & environment );

  /// create a unique name in the component list
  VX_VIEWER_DLL  std::string GetUniqueComponentName( vxComponentArray & componentArray, const std::string & sPrefix );

  /// Creates a mask volume to indicate locked voxels
  VX_VIEWER_DLL  void CalculateLockMask( vxBlockVolume<bool> & outMaskVolume, vxBlockVolume<uint2> & labelVolume, const vxComponentArray & components );

  /// Inverts all the voxels in the bool volume
  VX_VIEWER_DLL  void InvertVolume( vxBlockVolume<bool> & invertVolume );

  /// Calculates if both volumes have true for a given voxel. The result is placed in volumeA.
  VX_VIEWER_DLL  void GetIntersection( vxBlockVolume<bool> & volumeA, vxBlockVolume<bool> & volumeB, const bool & bInvertB );

  /// Cull voxels outside of the given bounding box
  VX_VIEWER_DLL  void CullOutside( vxBlockVolume<bool> & volume, const Box<uint4> & bounds );

  /// Dilate a boolean mask volume by one voxel in all directions based on the given connectivity
  VX_VIEWER_DLL  void DilateMask( vxBlockVolume<bool> & maskVolume, vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood );

  /// Calculates the volume of the component
  VX_VIEWER_DLL float8 CalculateComponentVolume( vxBlockVolume< uint2 > & labelVolume, const vxComponent & component );

	///Get the component Index by name
	VX_VIEWER_DLL int4 GetComponentIndex( vxComponentArray & componentArray, const std::string & sName );

	///Get the component Object from the Component Array which has a name match
	VX_VIEWER_DLL vxComponent * GetComponent( vxComponentArray & componentArray, const std::string & sName );

  /// returns a vector of constour points lists; one for each component that is intersected by the specified plane
  VX_VIEWER_DLL void GetComponentContourLists( vxBlockVolume< uint2 > & labelVolume, 
                                               const vxPlaneOblique< float4 > & plane, 
                                               const vxRenderingMode & mode,
                                               const vxComponentArray & components,
                                               std::vector< std::list< Vector3Di > > & points );

  /// removes the component
  VX_VIEWER_DLL bool RemoveComponent( vxBlockVolume<uint2> & labelVol, vxComponentArray & componentArray, const std::string & sName );

  /// Adds voxels to currently selected component.
  VX_VIEWER_DLL uint2 AddVoxelsToCurrentComponent( vxBlockVolume<bool> & desiredVoxels,
                                            vxShareableObject< vxComponentArray > * pComponentArray,
                                            vxShareableObject< vxBlockVolume<uint2> > * pLabelVolume,
                                            InteractiveSegmentation & interactiveSegmentation, bool bTestForLockedVoxels );

  /// Removes voxel from currently selected component.
  VX_VIEWER_DLL uint2 RemoveVoxelsFromCurrentComponent( vxBlockVolume<bool> & desiredVoxels,
                                            vxShareableObject< vxComponentArray > * pComponentArray,
                                            vxShareableObject< vxBlockVolume<uint2> > * pLabelVolume,
                                            InteractiveSegmentation & interactiveSegmentation, bool bTestForLockedVoxels );


} // namespace vxComponentUtils


// $Log: vxComponentUtils.h,v $
// Revision 1.15.2.1  2009/08/31 13:16:29  kchalupa
// Add methods for Adding and Removing Voxels
//
// Revision 1.15  2008/05/02 04:58:29  cqian
// check in
//
// Revision 1.14  2007/08/21 19:34:16  gdavidson
// Added RemoveComponent method to cleanup label volume and componentArray
//
// Revision 1.13  2007/07/20 18:50:42  dongqing
// add CreateNewEmptyComponent()
//
// Revision 1.12  2007/03/06 19:28:38  dongqing
// code review
//
// Revision 1.11  2006/12/13 17:50:25  dongqing
// implment the function GetComponentContourLists(...)
//
// Revision 1.10  2006/12/12 17:03:04  geconomos
// updated GetComponentContourLists
//
// Revision 1.9  2006/12/12 16:46:26  geconomos
// function prototype added for component contour lists
//
// Revision 1.8  2006/11/12 07:56:21  romy
// added GetComponent Object to get the component object by name
//
// Revision 1.7  2006/11/09 23:19:32  romy
// moved  GetComponentIndex method to componentUtils
//
// Revision 1.6  2006/09/21 19:24:56  gdavidson
// Added CalculateComponentVolume method
//
// Revision 1.5  2006/09/01 13:23:46  frank
// moved dilate volume function here and changed class to a namespace
//
// Revision 1.4  2006/04/14 18:24:32  frank
// Issue #4711: Fixed the unchecked growing into locked regions
//
// Revision 1.3  2006/03/29 20:03:19  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.2  2006/02/28 15:24:42  frank
// improved undo capabilities during interactive segmentation
//
// Revision 1.1  2006/02/27 15:22:58  frank
// split out component utilities
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxComponentUtils.h,v 1.15.2.1 2009/08/31 13:16:29 kchalupa Exp $
// $Id: vxComponentUtils.h,v 1.15.2.1 2009/08/31 13:16:29 kchalupa Exp $
