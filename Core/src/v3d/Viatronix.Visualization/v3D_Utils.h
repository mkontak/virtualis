// $Id: v3D_Utils.h,v 1.8 2008/05/02 20:22:39 dongqing Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxUtils.h"
#include "v3D_Colormap.h"
#include "v3D_Camera.h"
#include "v3D_Volume.h"
#include "v3D_Preset.h"
#include "v3D_Components.h"
#include "v3D_Annotations.h"
#include "v3D_Environment.h"
#include "v3D_Enums.h"
#include "v3D_PlaneOblique.h"
#include "v3D_VisualizationPreset.h"
#include "v3D_Triplet.h"


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class Utils
{
// functions
public:
  
  /// Adjusts the colormap values to the data range
  static void AdjustColormapValuesToDataRange( Colormap ^ mpColormap, Volume ^ mpVolume );

  /// Adjust colormap from adaptive value to range on intensity volume back to standard CT range (bias = 1000)
  static void AdjustColormapValuesFromAdaptiveToCTStandard(Colormap ^ mpColormap, Volume ^ mpVolume);

  /// Finds the first associated annotation index for the specified component.
  static System::Collections::ArrayList ^ FindAssociatedAnnotations( int4 iComponent, Components ^ mpComponents, Annotations ^ mpAnnotations );

  /// adjust camera based on bounding boxes
  static void AdjustCameraProportionally( Camera ^ mpCamera, Volume ^ mpVolume, Preset ^ mpPreset );

  /// Save current state of the vizualization preset for later undo.
  static void SetRestorePointVizPreset( Environment ^ mpEnvironment );

  /// apply the preset to the environment
  static void ApplyVisualizationPreset( VisualizationPreset ^ mpPreset, Environment ^ mpEnvironment );

  /// sets the oblique view to a standard view (left, anterior, etc.)
  static void SetStandardCameraViewOblique( Viatronix::Visualization::CameraViews eView, Camera ^ mpCamera,
                                            PlaneOblique ^ mpPlaneOblique, Volume ^ mpVolume, bool bViewAll );

  /// Set the camera to view all of the data
  static void CameraViewAll( Camera ^ mpCamera, Triplet ^ mpBBoxMin, Triplet ^ mpBBoxMax );

  /// returns how much to crop from all sides of a box when adjusting the initial zoom on an oblique view
  static float4 GetObliqueZoomFactor();

  /// orient the camera so that it lets the vessel rotate about any of four specific axes (x, y, z, v).
  static void OrientCameraToVessel( Environment ^ mpEnvironment, int iKeyId );

}; // class Utils


// namespaces
CLOSE_VISUALIZATION_NS

  
// $Log: v3D_Utils.h,v $
// Revision 1.8  2008/05/02 20:22:39  dongqing
// When we apply colormap, we adjust it to adaptive to volume intensity range.
// When we save current colormap to file, we have not restroe the standard
// colormap range back from the adaptive value. That was fixed this time.
// The fix looks silly. In future, we need to fix the entire color map and volume
// intensity range problem from groun up.
//
// Revision 1.7  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.6  2006/09/14 20:24:54  geconomos
// added OrientCameraFromVessel
//
// Revision 1.5  2006/04/27 19:16:06  romy
// UnDoAction Reset method
//
// Revision 1.4  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.3  2005/07/29 18:19:48  frank
// Issue #4318: Added standard orientations button to oblique view
//
// Revision 1.3.2.2  2005/07/29 16:25:53  frank
// Issue #4318: Added standard orientations to oblique view
//
// Revision 1.3.2.1  2005/06/24 13:50:30  frank
// refactored the application of visualization presets
//
// Revision 1.3  2005/05/24 14:40:02  frank
// fixed camera link/unlink movement problem by adjusting proportionally only when the preset is selected, not when the camera is set in the environment
//
// Revision 1.2  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.3  2005/02/10 20:57:24  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
//
// Revision 1.1.1.1.6.2  2005/02/10 17:34:58  frank
// typo
//
// Revision 1.1.1.1.6.1  2005/02/10 17:32:06  frank
// added utility to get list of annotations associated with a component
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/12/23 21:33:23  michael
// added inital version of the utilities
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Utils.h,v 1.8 2008/05/02 20:22:39 dongqing Exp $
// $Id: v3D_Utils.h,v 1.8 2008/05/02 20:22:39 dongqing Exp $
