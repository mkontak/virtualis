// $Id: vxRendererSoftwareMPRCurvedLuminal.C,v 1.3 2008/04/08 13:28:01 cqian Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxRendererSoftwareMPRCurvedLuminal.h"
#include "Matrix.h"
#include "vxEnvironment.h"
#include "vxPlaneCurved.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxRibbon.h"
#include "vxIDs.h"
#include "vxVesselPreferences.h"
#include "vxManipulatorVesselCurved.h"
#include "vxVesselUtilities.h"
#include "vxVessel.h"


// Defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererSoftwareMPRCurvedLuminal::vxRendererSoftwareMPRCurvedLuminal( vxEnvironment & environment ) :
vxRendererSoftwareMPRCurved( environment )
{
  SetMultiThreadRendering( false );
} // vxRendererSoftwareMPRCurvedLuminal()


/**
 * Destructor
 */
vxRendererSoftwareMPRCurvedLuminal::~vxRendererSoftwareMPRCurvedLuminal()
{
} // ~vxRendererSoftwareMPRCurvedLuminal()


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererSoftwareMPRCurvedLuminal::IsRedrawNecessary()
{

  const bool bBaseClassNeedsRedraw( vxRendererSoftwareMPR::IsRedrawNecessary() );
  const bool bRibbonChanged( GetEnvironment().IsModified( vxIDs::Ribbon ) );
  const bool bVesselChanged( GetEnvironment().IsModified( vxIDs::VesselArray ) );
  const bool bPreferencesChanged( GetEnvironment().IsModified( vxIDs::VesselPreferences ));
  const bool bCurvedPlaneChanged( GetEnvironment().IsModified( vxEnvironment::PLANE_MPR ) );

  return
    bBaseClassNeedsRedraw ||
    bPreferencesChanged ||
    bRibbonChanged ||
    bVesselChanged ||
    bCurvedPlaneChanged;

} // IsRedrawNecessary()


/**
 *  Pre-Render routine
 */
int4 vxRendererSoftwareMPRCurvedLuminal::PreRender()
{
  // call base class
  vxRendererSoftwareMPRCurved::PreRender();

  // make sure the image size is what we expect
  const Triple<uint4> desiredImageSize( GetDesiredImageSize( GetEnvironment().GetWinRect().GetViewportWidth(),
                                                             GetEnvironment().GetWinRect().GetViewportHeight() ) );
  if ( GetImage().GetDim().m_x != desiredImageSize.m_x || GetImage().GetDim().m_y != desiredImageSize.m_y )
  {
    GetImage().Resize( desiredImageSize );
  }


  if ( GetEnvironment().GetColormap() == NULL )
  {
    LogDbg( "GetEnvironment().GetColormapPtr() is NULL", "vxRendererSoftwareMPRCurvedLuminal", "PreRender" );
    return 0;
  }

  SetRenderingNecessary( false );

  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if ( pVessel == NULL || pVessel->size() == 0)
  {
    SetRenderingNecessary( true );
  }

  // has the volume changed?
  if ( GetEnvironment().IsModified(vxEnvironment::VOLUME_INTENSITY) == true )
  {
    SetRenderingNecessary(true);
  }

  if ( GetEnvironment().GetColormap() == NULL )
  {
    throw ex::NullPointerException( LogRec( "GetEnvironment().GetColormapPtr() is NULL", 
                                          "vxRendererSoftwareMPRCurvedLuminal",
                                            "PreRender" ) );
  }
  // has the transfer function changed?
  if ( GetEnvironment().IsModified( vxEnvironment::COLORMAP ) == true ) SetRenderingNecessary( true );

  // have any of the labels changed?
  if ( GetEnvironment().IsModified( vxEnvironment::COMPONENT_VISUAL ) == true ) SetRenderingNecessary( true );

  // has the camera changed?
  if ( GetEnvironment().IsModified( vxEnvironment::CAMERA ) == true ) SetRenderingNecessary( true );

  // has the plane changed?
  if ( GetEnvironment().IsModified( vxEnvironment::PLANE_MPR ) == true ) SetRenderingNecessary( true );

  // has the vessel changed?
  if ( GetEnvironment().IsModified( vxIDs::VesselArray ) ) SetRenderingNecessary( true );

  // has the rendering mode changed?
  if ( GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE ) ) SetRenderingNecessary( true );

  // update the scanlines
  if ( GetRenderingNecessary() ) SetupScanlines();

  return 1;

} // PreRender()


/**
 * Setup the scanline ends.
 */
void vxRendererSoftwareMPRCurvedLuminal::SetupScanlines()
{
  // get the vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if ( pVessel == NULL )
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurvedLuminal", "SetupScanlines" );
    return;
  }

  // get the curved plane
  vxPlaneCurved * pCurvedPlane( dynamic_cast< vxPlaneCurved * >( GetEnvironment().GetPlaneCurved() ) );
  if ( pCurvedPlane == NULL )
  {
    throw ex::NullPointerException( LogRec( "Curved Plane pointer is NULL", "vxRendererSoftwareMPRCurvedLuminal", "SetupScanlines" ) );
  }

  // get the ribbon
  vxShareableObject< vxRibbon > * pRibbon(
    dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) ) );
  if ( pRibbon == NULL )
  {
    throw ex::NullPointerException( LogRec( "Ribbon pointer is NULL", "vxRendererSoftwareMPRCurvedLuminal", "SetupScanlines" ) );
  }

  // compute the ribbon
  pRibbon->ComputeScanlinesLuminal( * pVessel, GetEnvironment(), Triple<uint4>( GetEnvironment().GetWinRect().GetViewportWidth(),
                                                                                GetEnvironment().GetWinRect().GetViewportHeight(), 0 ) );

} // SetupScanlines()


/**
 * render a subset of the image
 *
 * @param window    the subset of the image to render
 * @param uThreadID index of current thread
 */
int4 vxRendererSoftwareMPRCurvedLuminal::RenderWindow( const BoundingRect<uint2> & window, const uint1 & uThreadID )
{

  // do not render if unncessary
  if ( GetRenderingNecessary() == false ) return 1;

  // set up the rendering environment
  vxEnvironment & renderEnv( GetEnvironment() );
  int4 iRes( 0 );

  // select the proper renderer
  if ( renderEnv.GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::MIP )
  {
    iRes = RenderMIP( window, renderEnv, false );
  }
  else if ( renderEnv.GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::THIN_SLAB )
  {
    iRes = RenderMIP( window, renderEnv, true );
  }
  else if ( renderEnv.GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::XRAY )
  {
    iRes = RenderXRay( window, renderEnv );
  }
  else
  {
    iRes = RenderSingleLayer( window, renderEnv );
  }

  return iRes;

} // RenderWindow()


/**
 * Render a MIP image
 *
 * @param renderWindow the subset of the image to render
 * @param renderEnv    the rendering environment
 *
 * @return 1 if success
 */
int4 vxRendererSoftwareMPRCurvedLuminal::RenderMIP( const BoundingRect<uint2> & renderWindow, vxEnvironment & renderEnv, bool bSlab )
{

  if ( GetRenderingNecessary() == false ) return 0;

  // get the intensity volume - this is the whole volume (NOT detail volume)
  vxBlockVolume<uint2> * pIntensityVolume( dynamic_cast< vxBlockVolume<uint2> * >( renderEnv.GetIntensityVolume() ) );
  if ( pIntensityVolume == NULL )
  {
    LogDbg( "Intensity volume pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  // get the ribbon
  vxRibbon * pRibbon( static_cast< vxRibbon * >( renderEnv.GetElement( vxIDs::Ribbon ) ) );
  if ( pRibbon == NULL )
  {
    LogDbg( "Ribbon pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if ( pVessel == NULL )
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  // make sure there is something to render
  if ( pVessel->size() < 2 )
  {
    GetImage().Clear(); 
    return 0;
  }

  // see if the user wishes to display the graphs
  vxVesselPreferences * pPreferences = dynamic_cast< vxVesselPreferences * >(GetEnvironment().GetElement( vxIDs::VesselPreferences ));
  if( pPreferences == NULL )
  {
    LogDbg( "Preferences pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  vxBlockVolume<uint2> * pLabelVolume( dynamic_cast<vxBlockVolume<uint2> *>( renderEnv.GetLabelVolume() ) );
  if ( pLabelVolume == NULL )
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  const Vector3D<float4> intensityUnits( pIntensityVolume->GetHeader().GetVoxelUnits() );
  Matrix<float4> mWorldToVolume;
  mWorldToVolume.Scale(Vector<float4>(1.0F / intensityUnits.X(), 1.0F / intensityUnits.Y(), 1.0F / intensityUnits.Z()));

  const bool bCenterlineVisible( pPreferences->GetDisplayCenterline() );
  const bool bLabelsVisible( renderEnv.GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR ) );

  // get the reference point of Label Volume
  const Point<float4> referPntLabelVolume(  vxVesselUtilities::GetVolReferencePoint( pLabelVolume->GetHeader() ) );
  const std::vector< vxComponent * > & vpComponentInfo( renderEnv.GetComponents()->ArrayByLabel() );


  // some items used frequently inside the loop
  const Normal<float4> vViewDirectionWorld( pRibbon->GetRenderCamera().GetViewDirection() ); 
  
  const uint4 uSliceWidth( GetEnvironment().GetWinRect().GetViewportWidth() );
  const uint4 uSliceHeight( GetEnvironment().GetWinRect().GetViewportHeight() );
  const float4 fMIPSlabThicknessWorld( 0.5F * renderEnv.GetRenderingMode()->GetSlabThickness() ); // radius
  const float4 fMIPSamplingStep( 0.5F );  // half voxel sampling
  const float4 fMIPCenterX( 0.5F * uSliceWidth );

  //Point<uint2> vPixelPosition(0,0,0);
  uint1 * pImage( ( uint1 * )( GetImage().GetDataPtr() ) );
  //pImage += ( 4 * renderWindow.GetMinY() * GetImage().GetDim().m_x );

  vxBlockVolumeRayIterator<uint2> iterRayDensity( *pIntensityVolume );
  vxBlockVolumeRayIterator<uint2> iterMIPIntensity(  *pIntensityVolume );

  // loop over scanlines
  float4 * pfRGBA( renderEnv.GetColormap()->GetTablePointerRGBA( renderEnv.GetIntensityVolume()->GetHeader()));
  const uint1 blackColor( ( uint2 ) pfRGBA[ 3 ] >> GetNumberOfShiftBitsFor8bitAlpha() );

  for ( int4 y( renderWindow.GetMinY() ); y < renderWindow.GetMaxY() && y<pRibbon->GetScanlineData().size(); ++y )
  {
    // world coordinates
    Point<float4> leftPointWorld( pRibbon->GetScanlineData()[ y ].m_leftPoint );
    Point<float4> rightPointWorld( pRibbon->GetScanlineData()[ y ].m_rightPoint );
    float4 fRadiusWorld( bSlab ? 
                         fMIPSlabThicknessWorld : 
                         min( pRibbon->GetScanlineData()[ y ].m_fRadius, fMIPSlabThicknessWorld ) ) ;
    //pRibbon->GetScanlineData()[y].m_fRadius + fMIPMarginThicknessWorld );

    // volume coordinates
    Point<float4> leftPointVol( mWorldToVolume * leftPointWorld );
    Point<float4> rightPointVol( mWorldToVolume * rightPointWorld );

    // check for a valid scanline
    uint1 * pImageLine( pImage + 4 * ( GetImage().GetDim().m_x * y ) );
    const Vector<float4> lineDirectionVector( rightPointVol - leftPointVol );
    if ( lineDirectionVector.GetLength() < 1.0F )
    {
      for ( int4 x(0); x < uSliceWidth; ++x )
      {
        *pImageLine++ = blackColor;
        *pImageLine++ = blackColor;
        *pImageLine++ = blackColor;
        *pImageLine++ = 255;
      }
    } // if invalid scanline
    else
    {

      // we have a valid scanline, set up the ray
      iterRayDensity.SetRaySegment( RaySegment( Vector4Df( leftPointVol, 0 ), Vector4Df( rightPointVol, 1 ) ) );
      iterRayDensity.SetSampleIncrementVector( Vector4Df( lineDirectionVector, 1 ) / float4( uSliceWidth - 1 ) );
    
      // set up the MIP parts   
      const float4 fSampleDistanceWorld( ( rightPointWorld - leftPointWorld ).GetLength() / float4( uSliceWidth - 1 ) );
      float4 fMIPRadiusX( fRadiusWorld / fSampleDistanceWorld );

      int4 iMIPLeftX( uSliceWidth / 2 - fMIPRadiusX );
      int4 iMIPRightX( uSliceWidth / 2 + fMIPRadiusX );

      if ( bSlab )
      {
        iMIPLeftX = 0;
        iMIPRightX = uSliceWidth-1;
      }

      Vector<float4> vMIPDirectionWorld( vViewDirectionWorld*(fRadiusWorld) );
      Vector<float4> vMIPDirectionVol( mWorldToVolume*vMIPDirectionWorld );

      // loop over pixels in scanline
      for ( int4 x(0); x < uSliceWidth; ++x, ++iterRayDensity )
      {
        float4 fMIPVoxel( iterRayDensity.GetSample() );

        if ( x > iMIPLeftX && x < iMIPRightX )
        {
          const Point<float4> vMIPCenterVol( iterRayDensity.GetPosFloat() );

          float4 fMIPDepth( 1.0F );
          float4 fX2Center( fabs( fMIPCenterX - x ) );
          if ( ! bSlab && fX2Center > fMIPRadiusX )
          {
             fMIPDepth = max( 0.001F, 1.0F - ( fX2Center - fMIPRadiusX ) / fMIPRadiusX );  // to avoid 0.0F
          }

          Point<float4> vMIPStartPoint( vMIPCenterVol - vMIPDirectionVol * fMIPDepth );
          Point<float4> vMIPEndPoint( vMIPCenterVol + vMIPDirectionVol * fMIPDepth );
          if ( vMIPStartPoint == vMIPEndPoint )
          {
           //LogDbg( "vMIPStartPoint == vMIPEndPoint " + vMIPCenterVol.ToString() + " " + vMIPDirectionVol.ToString() +  " " + ToAscii( fMIPDepth ), "", "" );
          }

          RaySegment MIPSegmentIntensity( vMIPStartPoint, vMIPEndPoint );          
          Vector4Df sampleMIPIncVecIntensity( MIPSegmentIntensity.GetDirection().GetNormalizedXYZ() * fMIPSamplingStep );

          iterMIPIntensity.SetSampleIncrementVector( sampleMIPIncVecIntensity );
          iterMIPIntensity.SetRaySegment( MIPSegmentIntensity );

          for ( ; ! iterMIPIntensity.IsAtEnd(); ++iterMIPIntensity )
          {               
            float4 fVoxel( iterMIPIntensity.GetSample() );
             if ( fVoxel > fMIPVoxel ) fMIPVoxel = fVoxel;
          } 
        }

        const uint4 uIntensity( fMIPVoxel + 0.5F );
        const uint1 uColor( ( uint2 )( pfRGBA[ 4 * uIntensity + 3]) >> GetNumberOfShiftBitsFor8bitAlpha());

        *pImageLine++ = uColor; // red
        *pImageLine++ = uColor; // green
        *pImageLine++ = uColor; // blue
        *pImageLine++ = 255;    // alpha

      } // loop over pixels of scanline

    } // if valid scanline

  } // loop over scanlines

  return 1;
} // RenderMIP()


/**
 * Render an x-ray image
 *
 * @param renderWindow the subset of the image to render
 * @param renderEnv    the rendering environment
 *
 * @return 1 if success
 */
int4 vxRendererSoftwareMPRCurvedLuminal::RenderXRay( const BoundingRect<uint2> & renderWindow, vxEnvironment &renderEnv )
{
  if (GetRenderingNecessary() == false) 
    return 0;

  // get the intensity volume - this is the whole volume (NOT detail volume)
  vxBlockVolume<uint2> * pIntensityVolume = dynamic_cast<vxBlockVolume<uint2> *>(renderEnv.GetIntensityVolume());
  if (pIntensityVolume == NULL)
  {
    LogDbg( "Intensity volume pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  // get the ribbon
  vxRibbon * pRibbon = static_cast<vxRibbon *>( renderEnv.GetElement( vxIDs::Ribbon ) );
  if (pRibbon == NULL)
  {
    LogDbg( "Ribbon pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  // make sure there is something to render
  if ( pVessel->size() < 2 )
  {
    GetImage().Clear(); 
    return 0;
  }

  // see if the user wishes to display the graphs
  vxVesselPreferences * pPreferences = dynamic_cast< vxVesselPreferences * >(GetEnvironment().GetElement( vxIDs::VesselPreferences ));
  if( pPreferences == NULL )
  {
    LogDbg( "Preferences pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  vxBlockVolume<uint2> * pLabelVolume = dynamic_cast<vxBlockVolume<uint2> *>( renderEnv.GetLabelVolume() );
  if (pLabelVolume == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  const Vector3D<float4> intensityUnits( pIntensityVolume->GetHeader().GetVoxelUnits() );
  Matrix<float4> mWorldToVolume;
  mWorldToVolume.Scale(Vector<float4>(1.0F / intensityUnits.X(), 1.0F / intensityUnits.Y(), 1.0F / intensityUnits.Z()));

  const bool bCenterlineVisible( pPreferences->GetDisplayCenterline() );
  const bool bLabelsVisible( renderEnv.GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR ) );

  // get the reference point of Label Volume
  const Point<float4> referPntLabelVolume(  vxVesselUtilities::GetVolReferencePoint( pLabelVolume->GetHeader() ) );
  const std::vector< vxComponent * > & vpComponentInfo = renderEnv.GetComponents()->ArrayByLabel();


  // some items used frequently inside the loop
  const Normal<float4> vViewDirectionWorld( pRibbon->GetRenderCamera().GetViewDirection() ); 
  
  const uint4 uSliceWidth( GetEnvironment().GetWinRect().GetViewportWidth() );
  const uint4 uSliceHeight( GetEnvironment().GetWinRect().GetViewportHeight() );
  const float4 fMarginWorld(1.0F);
  const float4 fXRaySamplingStep( 1.0F );  // half voxel sampling
  const float4 fXRayCenterX( 0.5F*uSliceWidth );
  float4 fMaxRadiusWorld(0.0F);
  for ( int4 y( renderWindow.GetMinY() ); y<renderWindow.GetMaxY() && y<pRibbon->GetScanlineData().size(); ++y )
  {
    if ( pRibbon->GetScanlineData()[y].m_fSectionIndex < 0 )
      continue;

    float4 fRadiusWorld( pRibbon->GetScanlineData()[y].m_fRadius );
    if ( fMaxRadiusWorld < fRadiusWorld )
      fMaxRadiusWorld = fRadiusWorld;
  }
  fMaxRadiusWorld += fMarginWorld;

  uint1 * pImage = (uint1*)(GetImage().GetDataPtr());
  //pImage += (4*renderWindow.GetMinY()*uSliceWidth);

  vxBlockVolumeRayIterator<uint2> iterRayDensity( *pIntensityVolume );
  vxBlockVolumeRayIterator<uint2> iterXRayIntensity(  *pIntensityVolume );

  // loop over scanlines
  float4 * pfRGBA( renderEnv.GetColormap()->GetTablePointerRGBA( renderEnv.GetIntensityVolume()->GetHeader()));
  const uint1 blackColor((uint2)pfRGBA[3] >> GetNumberOfShiftBitsFor8bitAlpha());
  const float4 fXRaySlabThicknessWorld( 0.5F*renderEnv.GetRenderingMode()->GetSlabThickness() ); // radius

  for ( int4 y(renderWindow.GetMinY()); y<renderWindow.GetMaxY() && y<pRibbon->GetScanlineData().size(); ++y )
  {
    // world coordinates
    Point<float4> leftPointWorld(pRibbon->GetScanlineData()[y].m_leftPoint);
    Point<float4> rightPointWorld(pRibbon->GetScanlineData()[y].m_rightPoint);
    float4 fRadiusWorld( pRibbon->GetScanlineData()[y].m_fRadius+fMarginWorld );

    // volume coordinates
    Point<float4> leftPointVol(mWorldToVolume * leftPointWorld);
    Point<float4> rightPointVol(mWorldToVolume * rightPointWorld);

    uint1 * pImageLine( pImage + 4 * ( GetImage().GetDim().m_x * y ) );
    const Vector<float4> lineDirectionVector( rightPointVol - leftPointVol );
    if ( lineDirectionVector.GetLength() < 1.0f )
    {
      for ( int4 x(0); x < uSliceWidth; ++x )
      {
        *pImageLine++ = blackColor;
        *pImageLine++ = blackColor;
        *pImageLine++ = blackColor;
        *pImageLine++ = 255; // RGBA
      }
      continue; // continue can be a difficult construct to follow - try to avoid it unless if clarifies the program flow
    }

    iterRayDensity.SetRaySegment( RaySegment( Vector4Df(leftPointVol,0), Vector4Df(rightPointVol,1)) );
    iterRayDensity.SetSampleIncrementVector( Vector4Df(lineDirectionVector, 1) / float4(uSliceWidth-1) );

    // set up the XRay parts   
    const float4 fSampleDistanceWorld( (rightPointWorld-leftPointWorld).GetLength() / float4(uSliceWidth-1) );
    float4 fXRayRadiusX( fRadiusWorld/fSampleDistanceWorld );

    const int4 iXRayLeftX(uSliceWidth/2-fXRayRadiusX);
    const int4 iXRayRightX(uSliceWidth/2+fXRayRadiusX);

    Vector<float4> vXRayDirectionWorld( vViewDirectionWorld*(fRadiusWorld) );
    Vector<float4> vXRayDirectionVol( mWorldToVolume*vXRayDirectionWorld );

    // loop over pixels in scanline
    for ( int4 x(0); x < uSliceWidth; ++x, ++iterRayDensity )
    {
      float4 fAvgVoxel( 0.0F ); 
      float4 fOpticLength( 0.0F );

      if ( x > iXRayLeftX && x < iXRayRightX )
      { 
        const Point<float4> vCenterPnt( iterRayDensity.GetPosFloat() );

        float4 fXRayDepth(1.0F);
        float4 fX2Center(fabs(fXRayCenterX-x));
        if ( fX2Center > fXRayRadiusX )
        {
          fXRayDepth = max( 0.001F, 1.0-(fX2Center-fXRayRadiusX)/fXRayRadiusX );  // to avoid 0.0F
        }

        Point<float4> vXRayStartPoint( vCenterPnt-vXRayDirectionVol*fXRayDepth );
        Point<float4> vXRayEndPoint( vCenterPnt+vXRayDirectionVol*fXRayDepth );            

        RaySegment XRaySegmentIntensity( vXRayStartPoint, vXRayEndPoint );          
        Vector4Df sampleXRayIncVecIntensity( XRaySegmentIntensity.GetDirection().GetNormalizedXYZ()*fXRaySamplingStep );

        iterXRayIntensity.SetSampleIncrementVector( sampleXRayIncVecIntensity );
        iterXRayIntensity.SetRaySegment( XRaySegmentIntensity );

        int4 iCount(0);
        for ( ; !iterXRayIntensity.IsAtEnd(); ++iterXRayIntensity, ++iCount )
        {               
          float4 fVoxel( iterXRayIntensity.GetSample() );   
          fAvgVoxel += pfRGBA[4*(int2)(fVoxel) + 3];
        } 
        fAvgVoxel /= iCount;

        fOpticLength = 2.0F*fXRayDepth*fRadiusWorld + fMarginWorld;
      }
      else
      {
        fAvgVoxel = iterRayDensity.GetSample();
        fAvgVoxel = pfRGBA[4*(int2)(fAvgVoxel) + 3];
        fOpticLength = fMarginWorld;
      }

      fAvgVoxel = ( (uint2)fAvgVoxel >> GetNumberOfShiftBitsFor8bitAlpha() );
      fAvgVoxel /= 255.0F;
      fAvgVoxel *= fAvgVoxel;
      fAvgVoxel *= fAvgVoxel;
      fOpticLength /= fMaxRadiusWorld; 
      float4 fAlpha = exp( -1.0F * fAvgVoxel * fOpticLength );
      
      const uint1 uIntensity( 255.0F * fAlpha );

      // write pixel
      *pImageLine++ = uIntensity;
      *pImageLine++ = uIntensity;
      *pImageLine++ = uIntensity;
      *pImageLine++ = 255; // alpha
    }

   } // loop over scanlines

  return 1;
} // RenderXRay


/**
 * Render a thin slice
 *
 * @param renderWindow the subset of the image to render
 * @param renderEnv    the rendering environment
 *
 * @return 1 if success
 */
int4 vxRendererSoftwareMPRCurvedLuminal::RenderSingleLayer( const BoundingRect<uint2> & renderWindow, vxEnvironment & renderEnv )
{

  if ( GetRenderingNecessary() == false ) return 0;

  // get the intensity volume - this is the whole volume (NOT detail volume)
  vxBlockVolume<uint2> * pIntensityVolume( dynamic_cast< vxBlockVolume<uint2> * >( renderEnv.GetIntensityVolume() ) );
  if ( pIntensityVolume == NULL )
  {
    LogDbg( "Intensity volume pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }
  
  // get the ribbon
  vxRibbon * pRibbon( static_cast<vxRibbon *>( renderEnv.GetElement( vxIDs::Ribbon ) ) );
  if ( pRibbon == NULL )
  {
    LogDbg( "Ribbon pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if ( pVessel == NULL )
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  // make sure there is something to render
  if ( pVessel->size() < 2 )
  {
    GetImage().Clear(); 
    return 0;
  }

  // see if the user wishes to display the graphs
  vxVesselPreferences * pPreferences = dynamic_cast< vxVesselPreferences * >(GetEnvironment().GetElement( vxIDs::VesselPreferences ));
  if( pPreferences == NULL )
  {
    LogDbg( "Preferences pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  vxBlockVolume<uint2> * pLabelVolume( dynamic_cast<vxBlockVolume<uint2> *>( renderEnv.GetLabelVolume() ) );
  if ( pLabelVolume == NULL )
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  const Vector3D<float4> intensityUnits( pIntensityVolume->GetHeader().GetVoxelUnits() );
  Matrix<float4> mWorldToVolume;
  mWorldToVolume.Scale( Vector<float4>( 1.0F / intensityUnits.X(), 1.0F / intensityUnits.Y(), 1.0F / intensityUnits.Z() ) );

  const bool bCenterlineVisible( pPreferences->GetDisplayCenterline() );
  const bool bLabelsVisible( renderEnv.GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR ) );

  const uint4 uSliceWidth( GetEnvironment().GetWinRect().GetViewportWidth() );
  const uint4 uSliceHeight( GetEnvironment().GetWinRect().GetViewportHeight() );

  // get the reference point of Label Volume
  const Point<float4> referPntLabelVolume(  vxVesselUtilities::GetVolReferencePoint( pLabelVolume->GetHeader() ) );

  // some items used frequently inside the loop
  Point<uint2> vPixelPosition( 0, 0, 0 );
  uint1 * pImage( reinterpret_cast< uint1 * >( GetImage().GetDataPtr() ) );
  //pImage += (4*renderWindow.GetMinY()*GetImage().GetDim().m_x);

  const std::vector< vxComponent * > & vpComponentInfo( renderEnv.GetComponents()->ArrayByLabel() );

  vxBlockVolumeRayIterator<uint2> iterRayDensity( * pIntensityVolume );
  vxBlockVolumeRayIterator<uint2> iterRayLabel( * pLabelVolume );

  // loop over scanlines
  float4 * pfRGBA( renderEnv.GetColormap()->GetTablePointerRGBA( renderEnv.GetIntensityVolume()->GetHeader()));
  const uint1 blackColor( ( uint2 ) pfRGBA[3] >> GetNumberOfShiftBitsFor8bitAlpha() );

  for ( vPixelPosition.m_y = renderWindow.GetMinY();
        vPixelPosition.m_y < renderWindow.GetMaxY() && vPixelPosition.m_y<pRibbon->GetScanlineData().size();
        ++ vPixelPosition.m_y )
  {

    // world coordinates
    Point<float4> leftPointWorld( pRibbon->GetScanlineData()[ vPixelPosition.m_y ].m_leftPoint );
    Point<float4> rightPointWorld( pRibbon->GetScanlineData()[ vPixelPosition.m_y ].m_rightPoint );

    // volume coordinates
    Point<float4> leftPointVol( mWorldToVolume * leftPointWorld );
    Point<float4> rightPointVol( mWorldToVolume * rightPointWorld );

    uint1 * pImageLine( pImage + 4 * ( GetImage().GetDim().m_x * vPixelPosition.m_y ) );
    const Vector< float4 > majorDirectionVector( rightPointVol - leftPointVol );
    if ( majorDirectionVector.GetLength() < 1.0F )
    {
      for ( vPixelPosition.m_x = 0;
            vPixelPosition.m_x < uSliceWidth;
            ++vPixelPosition.m_x )
      {
        * pImageLine++ = blackColor; // red
        * pImageLine++ = blackColor; // green
        * pImageLine++ = blackColor; // blue
        * pImageLine++ = 255;        // alpha
      }
    } // if invalid scanline (out of range so should be black)
    else
      {

      iterRayDensity.SetRaySegment( RaySegment( Vector4Df( leftPointVol, 0 ), Vector4Df( rightPointVol, 1 ) ) );
      iterRayDensity.SetSampleIncrementVector( Vector4Df( majorDirectionVector, 1 ) / float4( uSliceWidth - 1 ) );

      // loop over pixels in scanline
      for ( vPixelPosition.m_x = 0;
            vPixelPosition.m_x < uSliceWidth;
            ++ vPixelPosition.m_x, ++ iterRayDensity )
    {

        const uint4 uIntensity( iterRayDensity.GetSample() );
      const bool bShowColor( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR ) );
      if ( bShowColor )
      {
        const float4 * pColorTableEntry( pfRGBA + 4 * uIntensity );
          *pImageLine++ = ( uint2 )( * pColorTableEntry++ ) >> GetNumberOfShiftBitsFor8bitAlpha();
          *pImageLine++ = ( uint2 )( * pColorTableEntry++ ) >> GetNumberOfShiftBitsFor8bitAlpha();
          *pImageLine++ = ( uint2 )( * pColorTableEntry++ ) >> GetNumberOfShiftBitsFor8bitAlpha();
      }
      else
      {
          const uint1 uColor( ( uint2 ) ( pfRGBA[ 4 * uIntensity + 3 ] ) >> GetNumberOfShiftBitsFor8bitAlpha() );
          *pImageLine++ = uColor; // red
          *pImageLine++ = uColor; // green
          *pImageLine++ = uColor; // blue
      }

        *pImageLine++ = 255;    // alpha
    } // loop over pixels of scanline

    } // if valid scanline

  } // loop over scanlines

  return 1;
} // RenderWindow()


// undefines
#undef FILE_REVISION


// $Log: vxRendererSoftwareMPRCurvedLuminal.C,v $
// Revision 1.3  2008/04/08 13:28:01  cqian
// check in
//
// Revision 1.2  2006/09/25 13:23:35  geconomos
// commented out unneeded  log debugging
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.9  2006/06/05 15:07:38  frank
// updated for visual studio 2005
//
// Revision 1.8  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.7.2.1  2005/07/26 13:57:28  frank
// Issue #4307: Fixed missing overlay text
//
// Revision 1.7  2005/06/08 15:02:09  vxconfig
// formatting
//
// Revision 1.6  2005/06/07 18:13:20  vxconfig
// removed the vessel array
//
// Revision 1.5  2005/05/25 17:09:01  frank
// removed artificial constraint on render size
//
// Revision 1.4  2005/05/03 15:52:42  frank
// Adjusted curved MPR rendering for non-uniform aspect ratio windows
//
// Revision 1.3  2004/10/05 14:43:40  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.2  2004/09/02 16:24:53  frank
// decreased the granularity of component array modified bits
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1  2004/05/26 12:23:15  frank
// initial version
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.17  2004/03/02 01:10:30  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.16.2.1  2004/02/25 15:00:25  frank
// formatting
//
// Revision 1.16  2004/01/15 17:43:45  wenli
// Add GetVolReferencePoint()
//
// Revision 1.15  2004/01/14 18:36:46  wenli
// Reverse the XRay intensity
//
// Revision 1.14  2003/12/22 21:23:39  wenli
// clear codes
//
// Revision 1.13  2003/12/22 20:29:12  frank
// removed yellow images from luminal view when no vessel is selected
//
// Revision 1.12  2003/12/18 17:05:32  frank
// Optimizing redraws
//
// Revision 1.11  2003/11/19 21:29:21  wenli
// setup flat slab curve MPR
//
// Revision 1.10  2003/11/19 06:15:20  frank
// Added colormap rendering
//
// Revision 1.9  2003/11/18 15:54:03  wenli
// 3D Biconvex Slab MIP
//
// Revision 1.8  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.7  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.6  2003/06/16 13:57:54  michael
// replaced 12 bit hard coded color conversion to use data dependent shifting
//
// Revision 1.5  2003/06/12 21:21:29  dongqing
// fix MR case of MPR curved image panel
//
// Revision 1.4  2003/06/09 12:29:52  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.3  2003/05/27 13:19:53  frank
// code review
//
// Revision 1.2  2003/05/16 16:46:31  frank
// reflected interface change to Image class
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.25  2003/04/02 19:49:43  wenli
// fInterval is added & the scaling of Luminal MPR is fixed
//
// Revision 1.24  2003/02/18 18:49:55  frank
// Moved selection and centerline rendering to vxRendererVesselCurved
//
// Revision 1.23  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.22  2003/02/12 15:08:21  wenli
// fix the zoom-in/out of the LuminalMPR
//
// Revision 1.21.2.1  2003/02/12 23:10:19  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.21  2003/01/24 20:16:58  frank
// Overhauled the ribbon to contain more helpful data per scanline.
//
// Revision 1.20  2003/01/22 22:19:04  ingmar
// the RayIterator is now only using a RaySegment and a Vector4Df increment to determine how to traverse the ray
//
// Revision 1.19  2003/01/20 19:34:40  wenli
// New Version with updated centerline selection and Hessian Filter
//
// Revision 1.18  2002/12/20 16:27:01  frank
// Change in PixelRGBA()
//
// Revision 1.17  2002/12/06 21:29:00  michael
// removed old fashioned Set/GetRGBATablePtr(); it was only copy of what is in
// the colormap in the environment.
//
// Revision 1.16  2002/12/06 20:21:16  frank
// Reflected name change elsewhere.
//
// Revision 1.15  2002/11/28 02:14:26  frank
// removed debug
//
// Revision 1.14  2002/11/27 21:41:29  wenli
// fix the size of the volumeDim-1
//
// Revision 1.13  2002/11/27 19:33:18  wenli
// mix opacity into both MPR
//
// Revision 1.12  2002/11/27 19:07:08  frank
// removed extra modified
//
// Revision 1.11  2002/11/26 19:24:52  frank
// vector 4d is here!
//
// Revision 1.10  2002/11/26 15:12:02  frank
// Blended label alpha.
//
// Revision 1.9  2002/11/22 16:41:10  wenli
// antialiasing the label volume on MPR
//
// Revision 1.8  2002/11/21 00:51:51  frank
// Let you go to vessel views without a vessel without a crash
//
// Revision 1.7  2002/11/20 23:29:13  frank
// Moved interpolation to RenderUtil
//
// Revision 1.6  2002/11/19 20:18:02  frank
// Removed dead code.
//
// Revision 1.5  2002/11/18 16:55:41  michael
// made changes to account for changes to vx and v3D plane objects
//
// Revision 1.4  2002/11/15 00:06:02  frank
// Changed ribbon from volume/world coordinates to always world coordinates.
//
// Revision 1.3  2002/11/05 20:23:21  frank
// checked rendering mode
//
// Revision 1.2  2002/11/05 17:15:26  frank
// moved ray iterator up out of inner loop
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.20  2002/10/30 23:33:54  wenli
// update parameters
//
// Revision 1.19  2002/10/30 00:03:32  frank
// Changed interface.
//
// Revision 1.18  2002/10/29 16:51:21  frank
// revamped measurement
//
// Revision 1.17  2002/10/29 15:41:23  geconomos
// Added inclusion of header file.
//
// Revision 1.16  2002/10/25 18:16:53  frank
// Made vessel selection update the lumen view
//
// Revision 1.15  2002/10/25 01:51:49  frank
// Moved working vessel into the VesselArray
//
// Revision 1.14  2002/10/07 21:38:27  wenli
// view label in MPR
//
// Revision 1.13  2002/09/27 21:47:10  wenli
// add curved MPR camera
//
// Revision 1.12  2002/09/27 14:21:59  geconomos
// Fixed colormap problems.
//
// Revision 1.11  2002/09/25 15:25:24  wenli
// v3D vessel component list
//
// Revision 1.10  2002/09/20 22:23:25  frank
// TRANSFER_FUNCTION -> COLORMAP
// pTF -> pColormap.
// Completed transition to vxColormap.
//
// Revision 1.9  2002/09/19 14:26:03  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.8  2002/09/16 20:22:14  dmitry
// New vxID (s) used.
//
// Revision 1.7  2002/08/23 17:33:59  frank
// Disabled removeElement
//
// Revision 1.6  2002/08/22 17:18:17  frank
// working on live updates
//
// Revision 1.5  2002/08/22 02:59:42  frank
// Added start/end scanlines rendering.
//
// Revision 1.4  2002/08/19 23:15:38  wenli
// initial version - Vascular
//
// Revision 1.3  2002/08/19 14:37:47  wenli
// initial version - Vascular
//
// Revision 1.2  2002/08/15 18:20:36  wenli
// initial version - Vascular
//
// Revision 1.1  2002/08/15 14:26:58  wenli
// initial version - Vascular
//
// Revision 1.1.2.5  2002/05/24 16:16:20  frank
// Cleaned up.
//
// Revision 1.1.2.4  2002/05/23 18:45:13  frank
// Moved to Ribbon data structure.
//
// Revision 1.1.2.3  2002/05/15 16:22:17  frank
// Cleaned up image sizes.
//
// Revision 1.1.2.2  2002/05/13 20:37:25  frank
// Initial version.
//
// Revision 1.1.2.1  2002/05/07 21:57:37  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSoftwareMPRCurvedLuminal.C,v 1.3 2008/04/08 13:28:01 cqian Exp $
// $Id: vxRendererSoftwareMPRCurvedLuminal.C,v 1.3 2008/04/08 13:28:01 cqian Exp $
