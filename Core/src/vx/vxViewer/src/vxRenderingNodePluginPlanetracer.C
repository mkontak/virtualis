// $Id: vxRenderingNodePluginPlanetracer.C,v 1.5 2007/06/29 20:22:25 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxRenderingNodePluginPlanetracer.h"
#include "vxRenderingNodePlugin.h"
#include "vxEnvironment.h"
#include "vxVisualizationPreferences.h"
#include "vxIntensityConverter.h"


/**
 * Constructor from environment
 *
 * @param   environment     environment
 */
vxRenderingNodePluginPlanetracer::vxRenderingNodePluginPlanetracer( vxEnvironment & environment ) : vxRenderingNodePlugin( environment )
{
  Reset();
} // vxRenderingNodePluginPlanetracer( vxEnvironment & environment )


/**
 * Destructor
 */
vxRenderingNodePluginPlanetracer::~vxRenderingNodePluginPlanetracer()
{
} // ~vxRenderingNodePluginPlanetracer()


/**
 * Resets the plugin.
 */
void vxRenderingNodePluginPlanetracer::Reset()
{
  SetTileSize( Triple< uint4 >( vxVisualizationPreferences::GetTileSize2D(), vxVisualizationPreferences::GetTileSize2D(), 0 ) );
} 


/**
 * Initializes the plugin from the environment
 */
void vxRenderingNodePluginPlanetracer::InitializeFromEnvironment()
{
  GetEnvironment().SetModified( vxEnvironment::ALL, true );
} // InitializeFromEnvironment()


/**
 * Is rendering necessary?
 *
 * @return true if rendering is necessary
 */
bool vxRenderingNodePluginPlanetracer::IsRenderingNecessary() const
{
  bool bRenderingNecessary = false;

  const bool bChangedVolumeIntensity  = GetEnvironment().IsModified( vxEnvironment::VOLUME_INTENSITY );
  const bool bChangedVolumeLabel      = GetEnvironment().IsModified( vxEnvironment::VOLUME_LABEL );
  const bool bChangedColormap         = GetEnvironment().IsModified( vxEnvironment::COLORMAP );
  const bool bChangedComponents       = GetEnvironment().IsModified( vxEnvironment::COMPONENT_VISUAL );
  const bool bChangedCropbox          = GetEnvironment().IsModified( vxEnvironment::CROP_BOX ) && GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_CROP_BOX );
  const bool bChangedPlaneMPR         = GetEnvironment().IsModified( vxEnvironment::PLANE_MPR );
  const bool bChangedSegmentation     = GetEnvironment().IsModified( vxEnvironment::INTERACTIVE_SEGMENTATION ) && GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE );
  const bool bChangedRenderingMode    = GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE);
  const bool bChangedCamera           = GetEnvironment().IsModified( vxEnvironment::CAMERA );
  const bool bChangedWinRect          = GetEnvironment().IsModified( vxEnvironment::WIN_RECT );
  const bool bChangedBackgroundColor  = GetEnvironment().IsModified( vxEnvironment::BACKGROUND_COLOR );
  const bool bChangedDataRenderer     = GetEnvironment().IsModified( vxEnvironment::DATA_RENDERER_SOFTWARE );
  const bool bChangedZoomFactor       = GetEnvironment().IsModified( vxEnvironment::ZOOM_FACTOR );

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
} // IsRenderingNecessary() const


/**
 * Updates the plugin from the environment
 */
void vxRenderingNodePluginPlanetracer::UpdateFromEnvironment()
{
  // intensity volume
  if( GetEnvironment().IsModified( vxEnvironment::VOLUME_INTENSITY ) )
  {
    m_planetracer.GetSettings().SetIntensityVolume( *GetEnvironment().GetIntensityVolume() );
  }

  // colormap
  if( GetEnvironment().IsModified( vxEnvironment::COLORMAP ) )
  {
    m_planetracer.GetSettings().SetTransferFunction( *GetEnvironment().GetIntensityVolume(), *GetEnvironment().GetColormap() );
  }

  //  components
  if( GetEnvironment().IsModified( vxEnvironment::COMPONENT_VISUAL ) )
  {
    const vxComponentArray & components = *GetEnvironment().GetComponents();
    m_planetracer.GetSettings().SetComponents( components );
    m_planetracer.GetSettings().SetNeedLabels( components.AreAnyLabelsVisible() || components.AreAnyIntensitiesInvisible() );
  }

  // rendering mode
  const bool bRenderingModeModified = GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE );
  if( bRenderingModeModified )
  {
    const vxRenderingMode & mode = *GetEnvironment().GetRenderingMode();
    
    m_planetracer.GetSettings().SetRenderingMode( mode );
    m_planetracer.GetSettings().SetShowColor( mode.IsEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR ));
    m_planetracer.GetSettings().SetShowLabels( mode.IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR ));
  }

  // label volume and components
  // TODO: double check this condition after the build
  const bool bLabelVolumeModified = GetEnvironment().IsModified( vxEnvironment::VOLUME_LABEL );
  if( bRenderingModeModified || bLabelVolumeModified )
  {
    m_planetracer.GetSettings().SetLabelVolume( *GetEnvironment().GetLabelVolume() );
  }

  // plane
  if( GetEnvironment().IsModified( vxEnvironment::PLANE_MPR ) )
  {
    m_planetracer.GetSettings().SetPlaneOrtho( *GetEnvironment().GetPlaneOrtho() );
  }

  // cropbox
  if( GetEnvironment().IsModified( vxEnvironment::CROP_BOX ) )
  {
    vxPlaneOrtho<float4> * pPlane = GetEnvironment().GetPlaneOrtho();
    m_planetracer.GetSettings().SetCropBox( GetEnvironment().GetCropBox()->GetBox() );
    m_planetracer.GetSettings().SetCropRect( GetEnvironment().GetCropBox()->GetCropRect( pPlane->GetNormal().GetMaxAbsComponentCoord() ) );
  }

  //Segmentation
  m_planetracer.GetSettings().SetShowThreshold( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE ));
  //Set the values if necessary
  if( m_planetracer.GetSettings().GetShowThreshold() && GetEnvironment().IsModified( vxEnvironment::INTERACTIVE_SEGMENTATION ) )
  {
    m_planetracer.GetSettings().SetThresholdLow( vxIntensityConverter::ModalityToRaw( GetEnvironment().GetInteractiveSegmentation()->GetRegionIntensityLow(), 
      GetEnvironment().GetIntensityVolume()->GetHeader() ) );

    m_planetracer.GetSettings().SetThresholdHigh( vxIntensityConverter::ModalityToRaw(GetEnvironment().GetInteractiveSegmentation()->GetRegionIntensityHigh() , 
      GetEnvironment().GetIntensityVolume()->GetHeader() ) );
  }


} // UpdateFromEnvironment()


// $Log: vxRenderingNodePluginPlanetracer.C,v $
// Revision 1.5  2007/06/29 20:22:25  geconomos
// added reset
//
// Revision 1.4  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.3.2.3  2007/03/20 13:15:34  romy
// added code to set the Low and High thresholds upon INTERACTIVE_SEGMENTATION modification - Issue# 5493
//
// Revision 1.3.2.2  2007/03/19 21:22:59  romy
// added code to set the Low and High thresholds upon INTERACTIVE_SEGMENTATION modification - Issue# 5493
//
// Revision 1.3.2.1  2007/03/16 15:11:28  romy
// Flag ShowThreshold  sets based on SHOW_INTENSITY_RANGE. Issue#5493
//
// Revision 1.3  2007/03/07 21:13:13  romy
// removed unnecessary include files.
//
// Revision 1.2  2007/03/05 15:23:27  geconomos
// code review preparation
//
// Revision 1.1  2007/02/05 19:27:53  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.10  2007/01/18 15:45:35  geconomos
// added ZOOM_FACTOR check to IsRenderingNecessary()
//
// Revision 1.9  2007/01/16 16:49:41  gdavidson
// Changed the constructor to take a vxEnvironment
//
// Revision 1.8  2006/12/12 19:54:36  geconomos
// correction with component setup
//
// Revision 1.7  2006/12/11 16:58:46  geconomos
// Issue #5158: compnent display is incorrect
//
// Revision 1.6  2006/10/03 20:11:07  geconomos
// proper handling of vxRenderingMode::ComponentsInMPR
//
// Revision 1.5  2006/09/28 15:52:17  geconomos
// Issue 5027: sub viewer causing main view to disappear
//
// Revision 1.4  2006/09/21 15:02:43  geconomos
// switch datatype for components from vxComponentArray to std::vector<vxComponent>
//
// Revision 1.3  2006/09/01 13:41:25  geconomos
// removed dead code
//
// Revision 1.2  2006/08/28 14:03:14  geconomos
// removed slice shadows check from IsRenderingNecessary
//
// Revision 1.1  2006/08/03 13:58:32  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderingNodePluginPlanetracer.C,v 1.5 2007/06/29 20:22:25 geconomos Exp $
// $Id: vxRenderingNodePluginPlanetracer.C,v 1.5 2007/06/29 20:22:25 geconomos Exp $
