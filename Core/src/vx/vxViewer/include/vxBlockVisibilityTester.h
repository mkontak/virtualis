// $Id: vxBlockVisibilityTester.h,v 1.7 2007/02/15 00:08:23 jmeade Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxBlockVisibilityTester_h
#define vxBlockVisibilityTester_h


// includes
#include "vxBlockVolumeIterator.h"
#include "vxComponent.h"


// forward declarations
template <class VoxelType> class vxBlockVolume;
class vxComponentArray;
class vxEnvironment;
class vxRenderingMode;
class vxColormap;
class InteractiveSegmentation;


/**
 * Helper class which creates the masks and helps you test blocks
 * for visibility based on various criteria.
 */
class vxBlockVisibilityTester
{
// member functions
public:

  /// constructor
  vxBlockVisibilityTester( vxEnvironment & pEnvironment );

  /// constructor
  vxBlockVisibilityTester( vxBlockVolume<uint2> * pIntensityVolume, vxBlockVolume<uint2> * pLabelVolume,
                           vxRenderingMode * pRenderingMode, vxColormap * pColormap,
                           vxComponentArray * pComponentArray, InteractiveSegmentation * pInteractiveSegmentation );

 /// constructor
  vxBlockVisibilityTester( vxBlockVolume<uint2> * pIntensityVolume, vxBlockVolume<uint2> * pLabelVolume,
                           vxRenderingMode * pRenderingMode, vxColormap * pColormap,
                           vxComponentArray * pComponentArray,  int4 iSegmentationLow, int4 iSegmentationHigh );

  vxBlockVisibilityTester( vxBlockVolume<uint2> * pIntensityVolume, vxBlockVolume<uint2> * pLabelVolume,
                           vxRenderingMode * pRenderingMode, vxColormap * pColormap,
                           const std::vector< vxComponent > & components,  int4 iSegmentationLow, int4 iSegmentationHigh );


  /// destructor
  virtual ~vxBlockVisibilityTester();

  /// see if either intensity or label is visible
  bool IsBlockVisible( const Vector3D<int4> & volumePosition );

  /// see if either intensity or label is visible
  bool IsBlockVisibleRaw( int4 iHistVoxelIntensity, int4 iHistVoxelLabel );

  /// test only if intensity is visible
  bool IsBlockVisibleIntensity( const Vector3D<int4> & volumePosition );

  /// test only if segmentation range is visible
  bool IsBlockVisibleSegmentationRange( const Vector3D<int4> & volumePosition );

  /// test only if label is visible
  bool IsBlockVisibleLabel( const Vector3D<int4> & volumePosition );

// data members
private:

  /// whether the label is desired to be visible in this rendering mode
  bool m_bShowLabel;

  /// mask intensity
  int4 m_iMaskIntensity;

  /// mask labels
  int4 m_iMaskLabel;

  /// mask label color
  int4 m_iMaskLabelColor;

  /// masek segmentation range
  int4 m_iMaskSegmentationIntensityRange;

  /// intensity  volume iterator
  vxBlockVolumeIterator<uint2> * m_pVolIterIntensity;

  /// label volume iterator
  vxBlockVolumeIterator<uint2> * m_pVolIterLabel;

}; // class BlockVisibilityTester


#endif


// $Log: vxBlockVisibilityTester.h,v $
// Revision 1.7  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.6  2006/09/21 14:56:55  geconomos
// added new consturctor that takes std::vector<vxComponent> as argument
//
// Revision 1.5  2006/07/21 15:02:19  geconomos
// added yet another constructor
//
// Revision 1.4  2006/06/23 19:18:07  frank
// breaking out utility functions to take individual elements instead of just the environment
//
// Revision 1.3  2006/03/29 20:03:19  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.2.12.1  2006/05/19 14:12:44  geconomos
// +added virtual destructor
// + deletion of created iterators
//
// Revision 1.2  2004/10/21 16:11:14  romy
// Removed the inline option from the IsBlockVisibleRaw. Inline was declared but the function was defined in .cpp.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/08/04 14:39:24  frank
// Separated class out from vxUtils
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxBlockVisibilityTester.h,v 1.7 2007/02/15 00:08:23 jmeade Exp $
// $Id: vxBlockVisibilityTester.h,v 1.7 2007/02/15 00:08:23 jmeade Exp $
