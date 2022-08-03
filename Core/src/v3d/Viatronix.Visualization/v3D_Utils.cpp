// $Id: v3D_Utils.cpp,v 1.9 2008/05/02 20:22:39 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_Utils.h"
#include "vxUtils.h"
#include "vxUndoActionPreset.h"
#include "vxVesselUtilities.h"

// namespaces
using namespace Viatronix::Visualization;
using namespace System;
using namespace System::Xml;

/// The following funcion should be got rid of if we could do the colormap and vxBlockVolume bias design better. Dongqing 20080502
/** 
 * Adjusts the colormap when being copied from a preset so that its ZeroOne
 * values matches the data range of the volume.
 */
void Utils::AdjustColormapValuesToDataRange(Colormap ^ mpColormap, Volume ^ mpVolume)
{
  try
  {
    vxShareableObject<vxBlockVolume<uint2> > * pVolume = reinterpret_cast<vxShareableObject<vxBlockVolume<uint2> > *>(mpVolume->GetVolumePtr().ToPointer());
    vxShareableObject<vxColormap> * pColormap = reinterpret_cast<vxShareableObject<vxColormap> *>(mpColormap->GetColormapPtr().ToPointer());
    ToNativeColormapPtr( mpColormap->GetElementPtr() );
    vxUtils::AdjustColormapValuesToDataRange(*pColormap, *pVolume);
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // AdjustColormapValuesToDataRange()


/** 
 * Adjust colormap from adaptive value to range on intensity volume back to standard CT range (bias = 1000)
 */
void Utils::AdjustColormapValuesFromAdaptiveToCTStandard(Colormap ^ mpColormap, Volume ^ mpVolume)
{
  try
  {
    vxShareableObject<vxBlockVolume<uint2> > * pVolume = reinterpret_cast<vxShareableObject<vxBlockVolume<uint2> > *>(mpVolume->GetVolumePtr().ToPointer());
    vxShareableObject<vxColormap> * pColormap = reinterpret_cast<vxShareableObject<vxColormap> *>(mpColormap->GetColormapPtr().ToPointer());
    ToNativeColormapPtr( mpColormap->GetElementPtr() );
    vxUtils::AdjustColormapValuesFromAdaptiveToCTStandard(*pColormap, *pVolume);
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // AdjustColormapValuesFromAdaptiveToCTStandard()



/** 
 * Adjusts the camera based on bounding boxes
 * values matches the data range of the volume.
 */
void Utils::AdjustCameraProportionally(Camera ^ mpCamera, Volume ^ mpVolume, Preset ^ mpPreset)
{
  try
  {
    Triple<float4> size(ToNativeVolumeInt2Ptr(mpVolume->GetElementPtr())->GetHeader().GetWorldDim());
    vxUtils::AdjustCameraProportionally(*ToNativeCameraPtr(mpCamera->GetElementPtr()), ToNativePresetPtr(mpPreset->GetElementPtr())->GetBoundingBox(), Box<float4>(0, 0, 0, size[0], size[1], size[2]));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // AdjustCameraToBoundingBox()


/**
 * Finds the first associated annotation index for the specified component.
 *
 * @param iComponent    component index
 * @param mpComponents  the components
 * @param mpAnnotations the annotations object
 *
 * @return a list of annotations attached to this component
 */
System::Collections::ArrayList ^ Utils::FindAssociatedAnnotations( int4 iComponent, Components ^ mpComponents,
                                                                   Annotations ^ mpAnnotations )
{

  // initially no association
  System::Collections::ArrayList ^ mpIndices = gcnew System::Collections::ArrayList();

  // go through all the annotations
  for ( int4 iIndex = 0; iIndex < mpAnnotations->Count; ++iIndex )
  {
    // are the comopnent and the annotation associated?
    if ( mpAnnotations->GetComponentIndex( iIndex, mpComponents->LabelVolume ) == mpComponents->GetLabelIndex( iComponent ) )
    { 
      mpIndices->Add( iIndex );
    } // if ( m_annotations.GetComponentIndex( iIndex, mpComponents->LabelVolume ) == m_components.GetLabelIndex( iComponent ) )
  } // for( int4 iIndex = 0; iIndex < m_annotations.Count; ++iIndex )

  return mpIndices;

} // FindAssociatedAnnotations( component )


/**
 * Save current state of the vizualization preset for later undo.
 *
 * @param mpEnvironment the environment
 */
void Utils::SetRestorePointVizPreset( Viatronix::Visualization::Environment ^ mpEnvironment )
{
  vxEnvironment * pEnvironment( ToNativeEnvironmentPtr( mpEnvironment->GetEnvironmentPtr() ) );
  
  vxEnvironment::GetUndoActions().PushAction( new vxUndoActionPreset( pEnvironment->GetColormap(),
    pEnvironment->GetBackgroundColor(), pEnvironment->GetRenderingMode() ) );
}//SetRestorePointVizPreset( Environment ^ mpEnvironment )


/**
 * apply the preset to the environment
 *
 * @param mpPreset      the visualization preset
 * @param mpEnvironment the environment
 */
void Utils::ApplyVisualizationPreset( Viatronix::Visualization::VisualizationPreset ^ mpPreset,
                                      Viatronix::Visualization::Environment ^ mpEnvironment )
{

  vxVisualizationPreset * pPreset( ToNativeVisualizationPresetPtr( mpPreset->GetElementPtr() ) );
  vxEnvironment * pEnvironment( ToNativeEnvironmentPtr( mpEnvironment->GetEnvironmentPtr() ) );
  
  vxUtils::ApplyVisualizationPreset( pPreset, pEnvironment );

} // ApplyPreset( visualizationPreset, environment )


/**
 * sets the view to a standard view (left, anterior, etc.)
 *
 * @param eView          the selected view
 * @param mpCamera       the camera
 * @param mpPlaneOblique the oblique plane
 * @param mpVolume       the intensity volume
 * @param bViewAll       also reset the zoom to show all
 */
void Utils::SetStandardCameraViewOblique( Viatronix::Visualization::CameraViews eView,
                                          Camera ^ mpCamera, PlaneOblique ^ mpPlaneOblique,
                                          Volume ^ mpVolume, bool bViewAll )
{

  // update the camera
  const float4 fZoomIn( vxUtils::GetObliqueZoomFactor() );
  Box<float4> roi(
    mpVolume->WorldDimensions->x * fZoomIn,
    mpVolume->WorldDimensions->y * fZoomIn,
    mpVolume->WorldDimensions->z * fZoomIn,
    mpVolume->WorldDimensions->x * ( 1 - fZoomIn ),
    mpVolume->WorldDimensions->y * ( 1 - fZoomIn ),
    mpVolume->WorldDimensions->z * ( 1 - fZoomIn ) );
  vxShareableObject< vxCamera > * pCamera( ToNativeCameraPtr( mpCamera->GetElementPtr() ) );
  vxUtils::SetStandardCameraView( * pCamera,
                                  static_cast< vxUtils::StandardCameraViewEnum >( eView ),
                                  roi, 
                                  bViewAll );
  pCamera->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

  // update the plane
  vxShareableObject< vxPlaneOblique<float4> > * pPlaneOblique( ToNativePlaneObliquePtr( mpPlaneOblique->GetElementPtr() ) );
  * pPlaneOblique = vxPlaneOblique<float4>( pCamera->GetViewDirection(), roi.GetMidPoint() );
  pPlaneOblique->Modified( vxModInfo( vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED ) );

} // SetStandardCameraViewOblique()


/**
 * Set the camera to view all of the data
 *
 * @param mpCamera   the camera
 * @param mpBBoxMin  the minimum corner of the bounding box
 * @param mpBBoxMax  the maximum corner of the bounding box
 */
void Utils::CameraViewAll( Camera ^ mpCamera, Triplet ^ mpBBoxMin, Triplet ^ mpBBoxMax )
{

  Box<float4> box( mpBBoxMin->x, mpBBoxMin->y, mpBBoxMin->z, mpBBoxMax->x, mpBBoxMax->y, mpBBoxMax->z );
  vxShareableObject< vxCamera > * pCamera( ToNativeCameraPtr( mpCamera->GetElementPtr() ) );
  vxUtils::CameraViewAll( * pCamera, box );
  pCamera->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

} // CameraViewAll()


/**
 * Returns how much to crop from all sides of a box when adjusting the initial zoom on an oblique view.
 *
 * @return how much to crop from all sides of a box
 */
float4 Utils::GetObliqueZoomFactor()
{
  return vxUtils::GetObliqueZoomFactor();
} // GetObliqueZoomFactor()


/** 
 * Orient the camera so that it lets the vessel rotate about any of four specific axes (x, y, z, v)
 *
 * @param   mpEnvironment   environment to adjust
 * @param   iKey            axis to orient
 */
void Utils::OrientCameraToVessel( Viatronix::Visualization::Environment ^ mpEnvironment, int iKeyId )
{
  vxEnvironment * pEnvironment = ToNativeEnvironmentPtr( mpEnvironment->GetEnvironmentPtr() );
  vxVesselUtilities::OrientCameraToVessel( *pEnvironment, iKeyId );
} // OrientCameraToVessel( Environment ^ mpEnvironment, int iKeyId )



// $Log: v3D_Utils.cpp,v $
// Revision 1.9  2008/05/02 20:22:39  dongqing
// When we apply colormap, we adjust it to adaptive to volume intensity range.
// When we save current colormap to file, we have not restroe the standard
// colormap range back from the adaptive value. That was fixed this time.
// The fix looks silly. In future, we need to fix the entire color map and volume
// intensity range problem from groun up.
//
// Revision 1.8  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.7  2006/09/14 20:24:54  geconomos
// added OrientCameraFromVessel
//
// Revision 1.6  2006/04/27 19:16:06  romy
// UnDoAction Reset method
//
// Revision 1.5  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.4  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.3  2005/07/29 18:19:48  frank
// Issue #4318: Added standard orientations button to oblique view
//
// Revision 1.3.2.2  2005/07/29 16:26:07  frank
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
// Revision 1.1.1.1.6.2  2005/02/10 20:57:24  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Utils.cpp,v 1.9 2008/05/02 20:22:39 dongqing Exp $
// $Id: v3D_Utils.cpp,v 1.9 2008/05/02 20:22:39 dongqing Exp $
