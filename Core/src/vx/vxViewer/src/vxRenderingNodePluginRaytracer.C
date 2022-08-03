// $Id: vxRenderingNodePluginRaytracer.C,v 1.9 2007/09/17 18:50:20 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxRenderingNodePluginRaytracer.h"
#include "vxEnvironment.h"
#include "vxElementPixelRGBA.h"
#include "vxUtils.h"
#include "vxVisualizationPreferences.h"
#include "vxHistogramUpdaterLabel.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerType3D.h"
#include "vxDataset.h"

#define SLICE_SHADOWS_ENABLED 0
#define FILE_REVISION "$Revision: 1.9 $"

/**
 * Consturctor 
 * 
 * @param   environment   environment
 */
vxRenderingNodePluginRaytracer::vxRenderingNodePluginRaytracer( vxEnvironment & environment ) : 
  vxRenderingNodePlugin( environment ),
  m_pLastActiveEnvironment( NULL )
{
  Reset();
} // vxRenderingNodePluginRaytracer( )


/**
 * Destructor
 */
vxRenderingNodePluginRaytracer::~vxRenderingNodePluginRaytracer()
{
} // ~vxRenderingNodePluginRaytracer()


/**
 * Resets the plugin.
 */
void vxRenderingNodePluginRaytracer::Reset()
{
  // reset rendering paramateres from the visualization preferences object
  if( dynamic_cast< vxViewerType3D *>( GetEnvironment().GetViewerType() ) != NULL )
  {
    SetMinFramerate( vxVisualizationPreferences::GetFrameRateMin3D() );
    SetMaxFramerate( vxVisualizationPreferences::GetFrameRateMax3D() );
    SetMinRenderSize( Triple< uint4 >( vxVisualizationPreferences::GetRenderSizeMin3D(), vxVisualizationPreferences::GetRenderSizeMin3D(), 0 ));
    SetMaxRenderSize( Triple< uint4 >( vxVisualizationPreferences::GetRenderSizeMax3D(), vxVisualizationPreferences::GetRenderSizeMax3D(), 0 ));
    SetInitialRenderSize( Triple< uint4 >( vxVisualizationPreferences::GetRenderSizeInitial3D(), vxVisualizationPreferences::GetRenderSizeInitial3D(), 0 ));
    SetTileSize( Triple< uint4 >( vxVisualizationPreferences::GetTileSize3D(), vxVisualizationPreferences::GetTileSize3D(), 0 ) );
  }
  else if( dynamic_cast< vxViewerTypeMPROblique * >( GetEnvironment().GetViewerType() ) != NULL )
  {
    SetMinFramerate( vxVisualizationPreferences::GetFrameRateMinOblique() );
    SetMaxFramerate( vxVisualizationPreferences::GetFrameRateMaxOblique() );
    SetMinRenderSize( Triple< uint4 >( vxVisualizationPreferences::GetRenderSizeMin3D(), vxVisualizationPreferences::GetRenderSizeMinOblique(), 0 ));
    SetMaxRenderSize( Triple< uint4 >( vxVisualizationPreferences::GetRenderSizeMax3D(), vxVisualizationPreferences::GetRenderSizeMaxOblique(), 0 ));
    SetInitialRenderSize( Triple< uint4 >( vxVisualizationPreferences::GetRenderSizeInitialOblique(), vxVisualizationPreferences::GetRenderSizeInitialOblique(), 0 ));
    SetTileSize( Triple< uint4 >( vxVisualizationPreferences::GetTileSizeOblique(), vxVisualizationPreferences::GetTileSizeOblique(), 0 ) );
  }
} // Reset()


/**
 * Initializes the plugin from the specified environment
 */
void vxRenderingNodePluginRaytracer::InitializeFromEnvironment()
{
  Reset();

  if( dynamic_cast< vxViewerType3D *>( GetEnvironment().GetViewerType() ) != NULL )
  {
    m_raytracer.GetSettings().GetRayGenerator().SetType( vxRayGenerator::VOLUME );
    m_raytracer.GetRenderBuffer().SetDepthBufferFlags( vxRenderBuffer::DEPTH_IMAGE_VC | vxRenderBuffer::DEPTH_IMAGE_WC );
  }
  else if( dynamic_cast< vxViewerTypeMPROblique * >( GetEnvironment().GetViewerType() ) != NULL )
  {
    m_raytracer.GetSettings().GetRayGenerator().SetType( vxRayGenerator::OBLIQUE );
    m_raytracer.GetRenderBuffer().SetDepthBufferFlags( vxRenderBuffer::DEPTH_IMAGE_VC  );
  }

  m_raytracer.GetRenderBuffer().SetBufferSize( GetMaxRenderSize() );

  GetEnvironment().SetModified( vxEnvironment::ALL, true );

} // InitializeFromEnvironment( vxEnvironment & environment )


/**
 * Updates the plugin from the specified environment
 */
void vxRenderingNodePluginRaytracer::UpdateFromEnvironment()
{
  vxRaytracerSettings & settings = m_raytracer.GetSettings();

  ////////////////////////////////////////////////////////////
  // check environment elements common to all viewer types
 

  // intensity volume
  const bool bIntensityVolumeModified = GetEnvironment().IsModified( vxEnvironment::VOLUME_INTENSITY );
  if( bIntensityVolumeModified )
  {
    settings.SetIntensityVolume( *GetEnvironment().GetIntensityVolume() );
  }

  // camera
  const bool bCameraModified = GetEnvironment().IsModified( vxEnvironment::CAMERA );
  if( bCameraModified )
  {
    settings.SetCamera( *GetEnvironment().GetCamera() );
  }

  // colormap
  const bool bColormapModified = GetEnvironment().IsModified( vxEnvironment::COLORMAP );
  if( bColormapModified )
  {
    settings.SetTransferFunction( *GetEnvironment().GetIntensityVolume(), *GetEnvironment().GetColormap() );
  }

  // winrect
  const bool bWinRectModified = GetEnvironment().IsModified( vxEnvironment::WIN_RECT );
  //if( bWinRectModified )
  {
    settings.SetWinRect( GetEnvironment().GetWinRect() );
  }

  // background color
  const bool bBackgroundColorModified = GetEnvironment().IsModified( vxEnvironment::BACKGROUND_COLOR );
  if( bBackgroundColorModified )
  {
    settings.SetBackgroundColor( *GetEnvironment().GetBackgroundColor() );
  }

  // rendering mode
  // TODO: needs cleanup, use rendering mode object instead of redundant flags
  const bool bRenderingModeModified = GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE );
  if( bRenderingModeModified )
  {
    vxRenderingMode & mode = *GetEnvironment().GetRenderingMode();    
    settings.SetRenderingMode( mode );

    if( GetEnvironment().GetViewerType()->Is3D() )
      settings.GetRenderingMode().SetEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR, true );

    settings.SetSampleEveryPixel(      mode.IsEnabled( vxRenderingMode::SHOW_EVERY_PIXEL      ) );
    settings.SetPreIntegrationEnabled( mode.IsEnabled( vxRenderingMode::SHOW_PRE_INTEGRATION  ) );
    settings.SetShadingEnabled(        mode.IsEnabled( vxRenderingMode::SHOW_SHADING          ) );
    settings.SetJitteringEnabled(      mode.IsEnabled( vxRenderingMode::SHOW_JITTERING        ) );
    settings.SetShadowingEnabled(      mode.IsEnabled( vxRenderingMode::SHOW_SHADOWS   ) );

  }

  // components
  const bool bComponentsModified  = GetEnvironment().IsModified( vxEnvironment::COMPONENT_VISUAL );
  if( bComponentsModified )
  {
    settings.SetComponents( *GetEnvironment().GetComponents() );
  }

  // label volume and components
  const bool bLabelVolumeModified = GetEnvironment().IsModified( vxEnvironment::VOLUME_LABEL );
  if( bRenderingModeModified || bLabelVolumeModified )
  {
    settings.SetLabelVolume( *GetEnvironment().GetLabelVolume() );
  }

  // crop box
  const bool bCropBoxModified = GetEnvironment().IsModified( vxEnvironment::CROP_BOX );
  if( bCropBoxModified )
  {
    settings.SetCropBoxActive( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_CROP_BOX ) );
  }

  // segmentation
  settings.SetShowIntensityRange( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE ) );

  //Set the values if necessary
  if( settings.GetShowIntensityRange() &&  GetEnvironment().IsModified( vxEnvironment::INTERACTIVE_SEGMENTATION ) )
  {
    settings.SetIntensityRangeLow( vxIntensityConverter::ModalityToRaw( GetEnvironment().GetInteractiveSegmentation()->GetRegionIntensityLow(), 
              GetEnvironment().GetIntensityVolume()->GetHeader() ) );

    settings.SetIntensityRangeHigh( vxIntensityConverter::ModalityToRaw(GetEnvironment().GetInteractiveSegmentation()->GetRegionIntensityHigh() , 
              GetEnvironment().GetIntensityVolume()->GetHeader() ) );
  }
  
  
  ////////////////////////////////////////////////////////////
  // check environment elements based on viewer type
 
  
  if( GetEnvironment().GetViewerType()->Is3D() )
  {
    // TODO: renable box of interest
    //if( settings.GetBoxOfInterestActive() ) 
    //  settings. SetBoxOfInterest( *environment.GetIntensityVolume(), *environment.GetCropBox() );    

    // check if clipping slice shadows is enabled
    const bool bClipSliceShadows = GetEnvironment().GetRenderingMode()->GetSliceShadowMode() == vxRenderingMode::SHADOWS_ACTIVE_CLIPPED;
                      
    settings.SetSliceShadowsEnabled( bClipSliceShadows );
    settings.SetClipSliceShadow( bClipSliceShadows );
    settings.SetShowCutsShaded(  GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_CUTS_SHADED      ) );
    settings.SetShowCutsGrayscale( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_CUTS_GRAYSCALE   ) );

    // slice shadow clipping enabled?
    if( bClipSliceShadows )
    {
      const vxEnvironment * pActiveEnvironment = GetEnvironment().GetActiveEnvironment();
      if( dynamic_cast< vxViewerType3D * >( pActiveEnvironment->GetViewerType() ) != NULL && m_pLastActiveEnvironment != NULL )
        pActiveEnvironment = m_pLastActiveEnvironment;
      else
        m_pLastActiveEnvironment = pActiveEnvironment;

      if( dynamic_cast< vxViewerTypeMPROblique * >( pActiveEnvironment->GetViewerType() ) )
        settings.SetSliceShadowPlane( *pActiveEnvironment->GetPlaneOblique() );
      else if( dynamic_cast< vxViewerTypeMPROrtho * >( pActiveEnvironment->GetViewerType() ) )
        settings.SetSliceShadowPlane( *pActiveEnvironment->GetPlaneOrtho() );
    }  // if( bClipSliceShadows )     
  } // if( GetEnvironment().GetViewerType()->Is3D() )
  else
  {
    settings.SetSliceShadowsEnabled( false );

    // plane ( oblique or ortho )
    const bool bPlaneMPRModified = GetEnvironment().IsModified( vxEnvironment::PLANE_MPR );
    if( bPlaneMPRModified )
    {
      settings.SetPlaneOblique( *GetEnvironment().GetPlaneOblique() );
    }
  }


  bool bUpdateSpaceLeap = bIntensityVolumeModified || bLabelVolumeModified || bCropBoxModified;
  
  // bounding box
  const bool bUpdateBoundingBox = bIntensityVolumeModified || bLabelVolumeModified || bColormapModified | bComponentsModified | bCropBoxModified;
  if( bUpdateBoundingBox )
    settings.SetVisibilityBox( vxUtils::GetVisibleBoundingBox( GetEnvironment() ) );
  
  // intensity mask
  const bool bUpdateMaskForIntensities = bIntensityVolumeModified || bColormapModified ;
  if( bUpdateMaskForIntensities )
    bUpdateSpaceLeap |= settings.UpdateMaskForIntensities( *GetEnvironment().GetIntensityVolume(), *GetEnvironment().GetColormap() );
  
  bool const bNeedToUpdateMaskForLabels = bLabelVolumeModified  || bComponentsModified;
  if(  bNeedToUpdateMaskForLabels )
    bUpdateSpaceLeap |= settings.UpdateMaskForLabels( *GetEnvironment().GetComponents() );
  
  if( bUpdateSpaceLeap )
    settings.UpdateSpaceLeapVolume();

} // UpdateFromEnvironment()


/**
 * Updates the plugin from the specified dataset
 *
 * @param         dataset             vxDataset
 */
void vxRenderingNodePluginRaytracer::UpdateFromDataset( vxDataset & dataset )
{
  vxRaytracerSettings & settings = m_raytracer.GetSettings();
  settings.SetIntensityVolume( *dataset.GetIntensityVolume() );
  settings.SetLabelVolume( *dataset.GetLabelVolume() );

  settings.UpdateMaskForIntensities( *GetEnvironment().GetIntensityVolume(), *GetEnvironment().GetColormap() );
  settings.UpdateMaskForLabels( *GetEnvironment().GetComponents() );
  settings.UpdateSpaceLeapVolume();
} // UpdateFromDataset( dataset )


/**
 * Is rendering necessary?
 *
 * @return true if rendering is necessary
 */
bool vxRenderingNodePluginRaytracer::IsRenderingNecessary() const
{
  return GetEnvironment().GetViewerType()->Is3D()? IsRenderingNecessary3D() : IsRenderingNecessaryOblique();
} // IsRenderingNecessary() const


/**
 * Is 3D rendering necessary? 
 *
 * @return true if rendering is necessary
 */
bool vxRenderingNodePluginRaytracer::IsRenderingNecessary3D() const
{
  bool bRenderingNecessary = false;

    // read rendering mode options
  const bool bShowCropBox        = GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_CROP_BOX );
  const bool bShowIntensityRange = GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE );
  const bool bBOIAvaialable      = GetEnvironment().GetElement( vxIDs::BoxOfInterest ) != NULL;
  const bool bClipSliceShadows   = GetEnvironment().GetRenderingMode()->GetSliceShadowMode() == vxRenderingMode::SHADOWS_ACTIVE_CLIPPED;

  const bool bChangedVolumeIntensity   = GetEnvironment().IsModified( vxEnvironment::VOLUME_INTENSITY          );
  const bool bChangedVolumeLabel       = GetEnvironment().IsModified( vxEnvironment::VOLUME_LABEL              );
  const bool bChangedColormap          = GetEnvironment().IsModified( vxEnvironment::COLORMAP                  );
  const bool bChangedComponents        = GetEnvironment().IsModified( vxEnvironment::COMPONENT_VISUAL          );
  const bool bChangedCropbox           = GetEnvironment().IsModified( vxEnvironment::CROP_BOX                  ) && bShowCropBox;
  const bool bChangedSegmentation      = GetEnvironment().IsModified( vxEnvironment::INTERACTIVE_SEGMENTATION  ) && bShowIntensityRange;
  const bool bChangedCpu3dData         = GetEnvironment().IsModified( vxEnvironment::DATA_RENDERER_SOFTWARE    );
  const bool bChangedCamera            = GetEnvironment().IsModified( vxEnvironment::CAMERA                    );
  const bool bChangedMomentum          = GetEnvironment().IsModified( vxEnvironment::MOMENTUM                  );
  const bool bChangedBackgroundColor   = GetEnvironment().IsModified( vxEnvironment::BACKGROUND_COLOR          );
  const bool bChangedBoxOfInterest     = GetEnvironment().IsModified( vxIDs::BoxOfInterest                     ) && bBOIAvaialable;
  const bool bChangedMomentumTimer     = GetEnvironment().IsModified( vxEnvironment::EVENT_TIMER_MOMENTUM      );
  const bool bChangedRenderingMode     = GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE            );
  const bool bChangedWinRect           = GetEnvironment().IsModified( vxEnvironment::WIN_RECT                  );
  const bool bChangedSliceShadow       = bClipSliceShadows && vxEnvironment::GetActiveEnvironment()->IsModified( vxEnvironment::PLANE_MPR );
  const bool bChangedCursor3d          = bClipSliceShadows && &GetEnvironment() == vxEnvironment::GetActiveEnvironment() && GetEnvironment().IsModified( vxEnvironment::CURSOR3D );

  const bool bNewActiveEnvironment = &GetEnvironment() != vxEnvironment::GetActiveEnvironment() && vxEnvironment::GetActiveEnvironment() != m_pLastActiveEnvironment;
  const bool bChangedActiveEnvironment = GetEnvironment().IsModified( vxEnvironment::ACTIVE_ENVIRONMENT ) && bClipSliceShadows && bNewActiveEnvironment;
                                         

  bRenderingNecessary |= bChangedVolumeIntensity;
  bRenderingNecessary |= bChangedVolumeLabel;
  bRenderingNecessary |= bChangedColormap;
  bRenderingNecessary |= bChangedComponents;
  bRenderingNecessary |= bChangedCropbox;
  bRenderingNecessary |= bChangedSegmentation;
  bRenderingNecessary |= bChangedCpu3dData;
  bRenderingNecessary |= bChangedCamera;
  bRenderingNecessary |= bChangedMomentum;
  bRenderingNecessary |= bChangedBackgroundColor;
  bRenderingNecessary |= bChangedBoxOfInterest;
  bRenderingNecessary |= bChangedMomentumTimer;
  bRenderingNecessary |= bChangedRenderingMode;
  bRenderingNecessary |= bChangedWinRect;
  bRenderingNecessary |= bChangedActiveEnvironment;
  bRenderingNecessary |= bChangedSliceShadow;
  bRenderingNecessary |= bChangedCursor3d;

  return bRenderingNecessary;
} // IsRenderingNecessary3D() const


/**
 * Is oblique rendering necessary?
 *
 * @return true if rendering is necessary
 */
bool vxRenderingNodePluginRaytracer::IsRenderingNecessaryOblique() const
{
  bool bRenderingNecessary = false;

  bool bChangedVolumeIntensity(GetEnvironment().IsModified(vxEnvironment::VOLUME_INTENSITY));
  bool bChangedVolumeLabel(GetEnvironment().IsModified(vxEnvironment::VOLUME_LABEL));
  bool bChangedColormap(GetEnvironment().IsModified(vxEnvironment::COLORMAP));
  bool bChangedComponents(GetEnvironment().IsModified(vxEnvironment::COMPONENT_VISUAL) && GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_COMPONENT_IN_MPR));
  bool bChangedCropbox(GetEnvironment().IsModified(vxEnvironment::CROP_BOX) && GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_CROP_BOX));
  bool bChangedPlaneMPR(GetEnvironment().IsModified(vxEnvironment::PLANE_MPR));
  bool bChangedSegmentation(GetEnvironment().IsModified(vxEnvironment::INTERACTIVE_SEGMENTATION) && GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE));
  bool bChangedRenderingMode(GetEnvironment().IsModified(vxEnvironment::RENDERING_MODE));
  bool bChangedCamera(GetEnvironment().IsModified(vxEnvironment::CAMERA));
  bool bChangedWinRect(GetEnvironment().IsModified(vxEnvironment::WIN_RECT));
  bool bChangedBackgroundColor(GetEnvironment().IsModified(vxEnvironment::BACKGROUND_COLOR));
  bool bChangedDataRenderer(GetEnvironment().IsModified(vxEnvironment::DATA_RENDERER_SOFTWARE));
  bool bChangedZoomFactor(GetEnvironment().IsModified(vxEnvironment::ZOOM_FACTOR));

  bRenderingNecessary |= bChangedVolumeIntensity;
  bRenderingNecessary |= bChangedVolumeLabel;
  bRenderingNecessary |= bChangedColormap;
  bRenderingNecessary |= bChangedComponents;
  bRenderingNecessary |= bChangedCropbox;
  bRenderingNecessary |= bChangedPlaneMPR;
  bRenderingNecessary |= bChangedSegmentation;
  bRenderingNecessary |= bChangedRenderingMode;
  bRenderingNecessary |= bChangedCamera;
  bRenderingNecessary |= bChangedWinRect;
  bRenderingNecessary |= bChangedBackgroundColor;
  bRenderingNecessary |= bChangedDataRenderer;
  bRenderingNecessary |= bChangedZoomFactor;

  return bRenderingNecessary;
} // IsRenderingNecessaryOblique() const


// $Log: vxRenderingNodePluginRaytracer.C,v $
// Revision 1.9  2007/09/17 18:50:20  geconomos
// setting depth buffer flags accordingly in vxRenderBuffer
//
// Revision 1.8  2007/06/29 20:22:25  geconomos
// added reset
//
// Revision 1.7  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.6  2007/04/02 21:29:18  gdavidson
// Added UpdateFromDataset
//
// Revision 1.5.2.1  2007/03/19 21:23:00  romy
// added code to set the Low and High thresholds upon INTERACTIVE_SEGMENTATION modification - Issue# 5493
//
// Revision 1.5  2007/03/05 15:23:27  geconomos
// code review preparation
//
// Revision 1.4  2007/02/13 22:33:57  romy
// We need to convert all modality values to RAW before the segmentation. Issue #5382
//
// Revision 1.3  2007/02/08 18:16:18  gdavidson
// Added check for a NULL last active environment
//
// Revision 1.2  2007/02/07 20:51:46  gdavidson
// Use the stored last active environment if the current active environment is 3D
//
// Revision 1.1  2007/02/05 19:27:53  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.18  2007/01/25 22:42:12  gdavidson
// Refactored duplicate code
//
// Revision 1.17  2007/01/24 20:26:59  gdavidson
// Set the initial render size
//
// Revision 1.16  2007/01/23 21:43:54  gdavidson
// Set rendersize from vxVisualizationPreferences
//
// Revision 1.15  2007/01/22 15:34:44  geconomos
// Issue# 5321: Slice shadows mode reworked
//
// Revision 1.14  2007/01/16 16:49:41  gdavidson
// Changed the constructor to take a vxEnvironment
//
// Revision 1.13  2006/12/11 16:58:46  geconomos
// Issue #5158: compnent display is incorrect
//
// Revision 1.12  2006/12/11 15:50:15  geconomos
// using dynamic casts on viewer type when performing initialization
//
// Revision 1.11  2006/12/04 20:41:59  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.10  2006/11/10 02:08:47  geconomos
// always initializing thevxWinRect to ensure correct aspect ratio
//
// Revision 1.9  2006/10/24 12:31:59  geconomos
// re-enabled clipping slice shadows
//
// Revision 1.8  2006/10/20 20:51:25  geconomos
// corrected changes to slice shadow color
//
// Revision 1.7  2006/10/04 14:58:27  geconomos
// added conditional compilation for rendering the slice shadows
//
// Revision 1.6  2006/10/03 20:11:07  geconomos
// proper handling of vxRenderingMode::ComponentsInMPR
//
// Revision 1.5  2006/09/21 15:02:59  geconomos
// switch datatype for components from std::vector<vxComponent*>to std::vector<vxComponent>
//
// Revision 1.4  2006/08/28 14:03:14  geconomos
// removed slice shadows check from IsRenderingNecessary
//
// Revision 1.3  2006/08/23 17:54:59  geconomos
// removed setting of aspect ratio on vxRenderingNodeThreadManager
//
// Revision 1.2  2006/08/14 15:12:14  geconomos
// corrected initialization of 3d raytracer from environment
//
// Revision 1.1  2006/08/03 13:58:32  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderingNodePluginRaytracer.C,v 1.9 2007/09/17 18:50:20 geconomos Exp $
// $Id: vxRenderingNodePluginRaytracer.C,v 1.9 2007/09/17 18:50:20 geconomos Exp $
