// $Id: vxSliceRenderer.C,v 1.18 2007/02/15 00:08:23 jmeade Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (george@viatronix.com)

#include "StdAfx.h"
#include "vxSliceRenderer.h"
#include "vxEnvironment.h"
#include "vxIntensityConverter.h"
#include "vxRendererSoftwareMPROblique.h"
#include "vxRendererSoftwareMPROrtho.h"
#include "vxSliceRendererData.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPRObliqueOld.h"
#include "vxUtils.h"
#include "Logger.h"
#include "Timer.h"

#define FILE_REVISION "$Revision: 1.18 $"

/**
 * Default constructor.
 */
vxSliceRenderer::vxSliceRenderer() : 
 m_pRenderer( NULL ),
 m_pData( NULL ),
 m_eOrientation( static_cast< vxSliceOrientationEnum >( -1 ) ),
 m_eRenderingMode( static_cast< vxSliceRenderingModeEnum >( -1 ) )
{
  // create rendering environment
  m_pEnvironment = vxEnvironment::Create();
  
  // set the default size of the image for oblique
  SetObliqueImageSize( 512, 512 );
  
  // default orientation
  SetOrientation( vxSliceRenderer::Axial );
  
  // default rendering mode
  SetRenderingMode( vxSliceRenderer::Default );

} // vxSliceRenderer()


/**
 * Destructor.
 */
vxSliceRenderer::~vxSliceRenderer()
{  
  if( m_pRenderer != NULL )
    delete m_pRenderer;
    
  if( m_pEnvironment != NULL )
  {
    vxEnvironment::Free( m_pEnvironment );
    delete m_pEnvironment;
  } 
} // ~vxSliceRenderer()


/**
 * Renders a slice into the image buffer
 * 
 * @param   iSliceNumber        Slice number to render
 * @param   bShowUncleansed     Indictaes if the rendering should be done from the uncleansed volume.
 * @param   bShowPseudoColor    Indicates if pseudo coloring should be used when rendering the slice
 */
void vxSliceRenderer::RenderSlice( const int4 iSliceNumber, const bool & bShowUncleansed, const bool & bShowPseudoColor )
{
  try
  {
    // set either the cleansed or uncleansed volume as active
    SetActiveVolume( bShowUncleansed );
    
    // set either the default or pseudo colormap as active
    SetActiveColormap( bShowPseudoColor );

    // get the voxel units for the active volume
    const Triple< float4 > & units =  m_pEnvironment->GetIntensityVolume()->GetHeader().GetVoxelUnits();   
    
    // compute the new ortho plane distance
    float fDistance = units[ m_pEnvironment->GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() ] * iSliceNumber;
    
    // update the distance in the environment
    m_pEnvironment->GetPlaneOrtho()->SetDistance( fDistance ); 
     
    // render the slice
    RenderSlice();

   } // try
   catch( ... )
   {
    LogErr( "An unhandled exception occurred.", "vxSliceRenderer", "RenderSlice( Ortho )" );
   } // catch( ... )  
} // RenderSlice( const int4 iSliceNumber, const bool & bShowUncleansed, const bool & bShowPseudoColor )


/**
 * Renders an oblique slice into the image buffer.
 *
 * @param   bottomLeft
 * @param   bottomRight
 * @param   topLeft
 * @param   topRight
 * @param   bShowUncleansed     Indictaes if the rendering should be done from the uncleansed volume.
 * @param   bShowPseudoColor    Indicates if pseudo coloring should be used when rendering the slice
 */
void vxSliceRenderer::RenderSlice(  const Point3Df & bottomLeft,        const Point3Df  & bottomRight, 
                                    const Point3Df & topLeft,           const Point3Df  & topRight, 
                                    const bool     & bShowUncleansed,   const bool      & bShowPseudoColor )
{
  float fDebug( 0.0F );

  try
  {
    // set either the cleansed or uncleansed volume as active
    SetActiveVolume( bShowUncleansed );
    
    // set either the default or pseudo colormap as active
    SetActiveColormap( bShowPseudoColor );
    
    // cast the current renderer to an oblique
    vxRendererSoftwareMPROblique * pObliqueRenderer = dynamic_cast< vxRendererSoftwareMPROblique * >( m_pRenderer ); 
    
    // get the voxel units for the active volume
    const Triple< float > & units( m_pEnvironment->GetIntensityVolume()->GetHeader().GetVoxelUnits() );

    // compute the camera direction for MIP and thin slab projection
    Vector3Df rightDirection( pObliqueRenderer->GetCornerPoints()[ 2 ] - pObliqueRenderer->GetCornerPoints()[ 1 ] );
    Vector3Df upDirection( pObliqueRenderer->GetCornerPoints()[ 0 ] - pObliqueRenderer->GetCornerPoints()[ 1 ] );
    Vector3Df crossProduct( rightDirection.Cross( upDirection ) );
    if ( crossProduct.GetLength() > 0.0F )
    {
      m_pEnvironment->GetCamera()->SetViewDirection( static_cast< Normal3Df >( crossProduct ) );
    }
    
    //
    // render the slice
    //
    m_pEnvironment->SetModified( vxEnvironment::ALL, true );
    try
    {
      m_pRenderer->PreRender();
    }
    catch ( ... )
    {
      // expect this to fail because OpenGL context is not set up and oblique requires one
    }

    // overwrite whatever the renderer has computed in PreRender with exactly what we want
    pObliqueRenderer->GetCornerPoints()[ 0 ] = topLeft.GetDividedAnIso( units );
    pObliqueRenderer->GetCornerPoints()[ 1 ] = bottomLeft.GetDividedAnIso( units );
    pObliqueRenderer->GetCornerPoints()[ 2 ] = bottomRight.GetDividedAnIso( units );
    pObliqueRenderer->GetCornerPoints()[ 3 ] = topRight.GetDividedAnIso( units );

    m_pRenderer->Render();
    m_pRenderer->PostRender();
    m_pEnvironment->SetModified( vxEnvironment::ALL, false );
    
   } // try
   catch( ... )
   {
    LogErr( "An unhandled exception occurred.", "vxSliceRenderer", "RenderSlice( Oblique )" );
   } // catch( ... )  
} // RenderSlice()


/**
 * Sets the cleansed or uncleansed volume as the active one.
 *
 * @param   bShowUncleansed   Indicates if the unleansed volume should be set active.
 */
void vxSliceRenderer::SetActiveVolume( bool bShowUncleansed )
{
  if( bShowUncleansed )
  {
    m_pEnvironment->SetIntensityVolume( m_pData->GetUncleansedVolume() );
  }
  else
  {
    m_pEnvironment->SetIntensityVolume( m_pData->GetCleansedVolume() );
  }
} // SetActiveVolume()


/**
 * Sets the default or psedu color colormap as the active one.
 */
void vxSliceRenderer::SetActiveColormap( bool bShowPseudoColor )
{
  if( bShowPseudoColor )
    m_pEnvironment->SetColormap( m_pData->GetBiopsyColormap() );
  else
    m_pEnvironment->SetColormap( m_pData->GetDefaultColormap() );
    
   m_pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR, bShowPseudoColor );
} // SetActiveColormap( bool bShowPseudoColor )


/**
 * Sets the window and level for default colormap.
 *
 * @param   fWindow   Window in HU.
 * @param   fLevel   Level in HU.
 */
void vxSliceRenderer::SetDefaultWindowLevel( float4 fWindowHU, float4 fLevelHU )
{
  vxVolumeHeader & header = m_pData->GetCleansedVolume()->GetHeader();
 
  vxWindowLevel windowLevel
  ( 
    vxIntensityConverter::FromShort( fWindowHU, header, false ),
    vxIntensityConverter::FromShort( fLevelHU,  header, true  )
  );
  
  m_pData->GetDefaultColormap()->SetWindowLevel( windowLevel );
  m_pData->GetDefaultColormap()->SetDirty();
  
  //m_pColormap->SetWindowLevel( windowLevel );
  //m_pColormap->SetDirty();
} // SetWindowLevel( float4 fWindowHU, float4 fLevelHU )


/**
 * Render functionality common to all orientations.
 */
void vxSliceRenderer::RenderSlice()
{
  m_pEnvironment->SetModified( vxEnvironment::ALL, true );
  m_pRenderer->PreRender();
  m_pRenderer->Render();
  m_pRenderer->PostRender();
  m_pEnvironment->SetModified( vxEnvironment::ALL, false );
} // RenderSlice()


/**
 * Sets the slice renderer data.
 *
 * @param   pData   New slice renderer data.
 */
void vxSliceRenderer::SetData( vxSliceRendererData * pData )
{
  m_pData = pData;

  m_pEnvironment->SetLabelVolume( pData->GetLabelVolume() );
  m_pEnvironment->SetComponents( pData->GetComponents() );

  m_pEnvironment->GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR, true );
  
  SetActiveVolume( false );

} // SetData( const vxSliceRendererData * pData )


/** 
 * Sets the slice orientation.
 *
 * @param   eOrientation    New slice orientation.
 */
void vxSliceRenderer::SetOrientation( const vxSliceOrientationEnum & eOrientation )
{  
  if( eOrientation != GetOrientation() )
  {
    // free the existing renderer
    if( m_pRenderer != NULL )
    {
      delete m_pRenderer;
      m_pRenderer = NULL;
    }
    
    // free the eixsting viewer type 
    if( m_pEnvironment->GetViewerType() != NULL )
    {
      delete m_pEnvironment->GetViewerType();
      m_pEnvironment->SetViewerType( NULL );
    } 

    if( eOrientation == vxSliceRenderer::Axial )
    {
      m_pEnvironment->SetViewerType( new vxViewerTypeMPROrtho( *m_pEnvironment ) );
      m_pEnvironment->GetPlaneOrtho()->SetNormal( Normal<float4>( 0, 0, -1 ) );
      m_pRenderer = new vxRendererSoftwareMPROrtho( *m_pEnvironment );
    } 

    else if( eOrientation == vxSliceRenderer::Coronal )
    {
      m_pEnvironment->SetViewerType( new vxViewerTypeMPROrtho( *m_pEnvironment ) );
      m_pEnvironment->GetPlaneOrtho()->SetNormal( Normal<float4>( 0, -1, 0 ) );
      m_pRenderer = new vxRendererSoftwareMPROrtho( *m_pEnvironment );
    }
    
    else if( eOrientation == vxSliceRenderer::Sagittal )
    {
      m_pEnvironment->SetViewerType( new vxViewerTypeMPROrtho( *m_pEnvironment ) );
      m_pEnvironment->GetPlaneOrtho()->SetNormal( Normal<float4>( -1, 0, 0 ) );
      m_pRenderer = new vxRendererSoftwareMPROrtho( *m_pEnvironment );
    }
    
    else if( eOrientation == vxSliceRenderer::Oblique )
    {
      m_pEnvironment->SetViewerType( new vxViewerTypeMPRObliqueOld( *m_pEnvironment ) );
      m_pRenderer = new vxRendererSoftwareMPROblique( *m_pEnvironment );
    }
    m_pEnvironment->SetModified( vxEnvironment::PLANE_MPR, true );
    m_eOrientation = eOrientation;
  } // if( eOrientation != GetOrientation() )
} // SetOrientation( const vxSliceOrientationEnum & eOrientation )


/**
 * Sets the rendering mode.
 *
 * @param   eRenderingMode    New rendering mode
 */
void vxSliceRenderer::SetRenderingMode( const vxSliceRenderingModeEnum & eRenderingMode )
{
  if( eRenderingMode != GetRenderingMode() )
  {  
    m_pEnvironment->GetRenderingMode()->SetRenderingAlgorithm( static_cast<vxRenderingMode::vxRenderingAlgorithmEnum>( eRenderingMode ) );
    m_eRenderingMode = eRenderingMode;
    m_pEnvironment->SetModified( vxEnvironment::RENDERING_MODE, true );
  }
} // SetRenderingMode( const vxRenderingModeEnum & eRenderingMode )


/**
 * Gets the slab thickness in millimeters.
 *
 * @return  Slab thickness in millimeters.
 */
float4 vxSliceRenderer::GetSlabThickness() const
{
  return m_pEnvironment->GetRenderingMode()->GetSlabThickness();
} // GetSlabThickness() const


/**
 * Sets the slab thickness in millimeters.
 *
 * @param fSlabThickness  Slab thickness in millimeters
 */
void vxSliceRenderer::SetSlabThickness( const float4 fSlabThickness )
{
  if( fSlabThickness != GetSlabThickness() )
  {
    m_pEnvironment->GetRenderingMode()->SetSlabThickness( fSlabThickness );
    m_pEnvironment->SetModified( vxEnvironment::RENDERING_MODE, true );
  }
} // SetSlabThickness( const float4 fSlabThickness )

/**
 * Sets the slice width and height.
 *
 * @param   iWidth    New slice width.
 * @param   iHeight   New slice height.
 */
void vxSliceRenderer::SetObliqueImageSize( const int4 iWidth, const int4 iHeight )
{
  vxWinRect winrect = m_pEnvironment->GetWinRect();
  winrect.SetWindowSize( std::make_pair( iWidth, iHeight ) );
  m_pEnvironment->SetWinRect( winrect );
} // SetImageSize()


/*
 * Calculates the intensity statistics for the specified ellisped
 *
 * @param   outStatistics
 * @param   firstCornerPosition
 * @param   verticalEdge
 * @param   horizontalEdge
 */
void vxSliceRenderer::GetIntensityStatistics( vxIntensityStatistic & outStatistics,  
                                              const Point3Df & firstCornerPosition,
                                              const Vector3Df & verticalEdge,  
                                              const Vector3Df & horizontalEdge )
{
  vxUtils::ComputeEllipseStatistics(  outStatistics, 
                                      firstCornerPosition,
                                      verticalEdge, 
                                      horizontalEdge,
                                      *(m_pEnvironment->GetIntensityVolume()) );

} // GetIntenistyStatistics()


/**
 * Gets the slice image width.
 * 
 * @return  Width of slice image in pixels
 */
int4 vxSliceRenderer::GetImageWidth() const
{
  return m_pRenderer->GetImage().GetDim().X();
} // GetImageWidth() const


/**
 * Gets the slice image height.
 * 
 * @return  Height of slice image in pixels
 */
int4 vxSliceRenderer::GetImageHeight() const
{
  return m_pRenderer->GetImage().GetDim().Y();
} // GetSliceImageWidth() const


/**
 * Gets the slice image pixel data.
 * 
 * @return  Pixel data for slice image.
 */
const uint1 * vxSliceRenderer::GetImageData() const
{
  return reinterpret_cast< const uint1 *>( m_pRenderer->GetImage().GetDataPtr() );
} // GetImageData() const


/**
 * Converts a raw voxel value into a modality-specific one based on the current volume (cleansed/not).
 *
 * @param fRawValue the raw input value (as you would get directly from a volume).
 *
 * @return The modality-specific value (eg HU).
 */
float4 vxSliceRenderer::ConvertRawToModality( const float4 fRawValue )
{
  vxBlockVolume< uint2 > * pVolume = m_pEnvironment->GetIntensityVolume();
  return vxIntensityConverter::RawToModality( fRawValue, pVolume->GetHeader() );
} // ConvertRawToModality()


/**
 * Converts a modality-specific value into a raw voxel one based on the current volume (cleansed/not).
 *
 * @param fModalityValue the modality-specific input value (eg HU).
 *
 * @return The raw value (as you would get directly from a volume).
 */
float4 vxSliceRenderer::ConvertModalityToRaw( const float4 fRawValue )
{
  vxBlockVolume< uint2 > * pVolume = m_pEnvironment->GetIntensityVolume();
  return vxIntensityConverter::ModalityToRaw( fRawValue, pVolume->GetHeader() );
} // ConvertModalityToRaw()


// $Log: vxSliceRenderer.C,v $
// Revision 1.18  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.17  2007/02/13 21:02:28  romy
// fixed issue with old oblique renderer and associated viewer type
//
// Revision 1.16  2006/09/26 19:35:01  dongqing
// comments
//
// Revision 1.15  2006/09/26 15:59:16  geconomos
// removed pimpl classes
//
// Revision 1.14  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.13.4.1  2006/05/19 14:13:41  geconomos
// removed dead code
//
// Revision 1.13  2005/07/14 20:16:39  vxconfig
// no longer need special compile for VC
//
// Revision 1.12  2005/07/13 14:44:42  geconomos
// Issue #4255: Improved the speed (and accuracy) of the ROI statistics
//
// Revision 1.11  2005/06/10 20:09:31  vxconfig
// fixed MIP mode for VC viewer
//
// Revision 1.10  2005/06/06 12:09:12  geconomos
// exposed image slice for oblique renderers
//
// Revision 1.9  2005/03/12 18:31:43  michael
// No more adjustments of the data being returned from the renderers, renderers
// have been adjusted to deliver the right values.
//
// Revision 1.8  2004/10/05 14:35:47  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.7  2004/09/16 15:53:31  geconomos
// Moved default colormap from renderer data to renderer
//
// Revision 1.6  2004/05/19 13:04:07  geconomos
// Moved manipulation of window level to vxSliceRendererData.
//
// Revision 1.5  2004/05/17 13:29:19  geconomos
// Changed signature of SetColormap()
//
// Revision 1.4  2004/04/23 15:35:27  geconomos
// Added methods for setting the rendering mode and related properties.
//
// Revision 1.3  2004/04/22 20:41:01  geconomos
// Defaulted initial volume.
//
// Revision 1.2  2004/04/21 18:33:39  geconomos
// Continued effort integrating 2D views within vc.
//
// Revision 1.1  2004/04/05 15:45:03  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxSliceRenderer.C,v 1.18 2007/02/15 00:08:23 jmeade Exp $
// $Id: vxSliceRenderer.C,v 1.18 2007/02/15 00:08:23 jmeade Exp $