// $Id: vxRendererSoftwareMPRCurved.C,v 1.3 2006/10/24 12:30:53 geconomos Exp $
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
#include "vxRendererSoftwareMPRCurved.h"
#include "Matrix.h"
#include "vxEnvironment.h"
#include "vxPlaneCurved.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxRibbon.h"
#include "vxColormap.h"
#include "vxVisualizationPreferences.h"
#include "vxManipulatorVesselCurved.h"
#include "V3DFile.h"
#include "vxIDs.h"
#include "vxProgressBar.h"
#include "timer.h"
#include "vxIntensityConverter.h"
#include "ImageUtilities.h"
#include "vxMathUtils.h"
#include "vxVesselUtilities.h"
#include "vxVessel.h"


// Defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererSoftwareMPRCurved::vxRendererSoftwareMPRCurved( vxEnvironment & environment )
:vxRendererSoftwareMPR( environment ), m_fMPRRenderSamplingStep(0.5F)
{
} // vxRendererSoftwareMPRCurved()


/**
 * Destructor
 */
vxRendererSoftwareMPRCurved::~vxRendererSoftwareMPRCurved()
{
} // ~vxRendererSoftwareMPRCurved()


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererSoftwareMPRCurved::IsRedrawNecessary()
{

  const bool bBaseClassNeedsRedraw( vxRendererSoftwareMPR::IsRedrawNecessary() );
  const bool bRibbonChanged( GetEnvironment().IsModified( vxIDs::Ribbon ) );
  const bool bVesselChanged( GetEnvironment().IsModified( vxIDs::VesselArray ) );
  const bool bCurvedPlaneChanged( GetEnvironment().IsModified( vxEnvironment::PLANE_MPR ) );
  const bool bWindowChanged( m_lastWinRect != GetEnvironment().GetWinRect() );
  if ( bWindowChanged )
  {
    //LogDbg( "window is changed", "", "" );
  }
  else
  {
    //LogDbg( "window is not changed", "", "" );
  }

  return
    bBaseClassNeedsRedraw ||
    bRibbonChanged ||
    bVesselChanged ||
    bCurvedPlaneChanged ||
    bWindowChanged;

} // IsRedrawNecessary()


/**
 * Called before rendering to decide if the compute-intensive rendering needs to be executed.
 * It ends up calling SetRenderingNecessary() with its decision.
 *
 * @return true if successful
 */
int4 vxRendererSoftwareMPRCurved::PreRender()
{

  // call base class
  vxRendererSoftwareMPR::PreRender();

  // make sure the image size is what we expect
  const Triple<uint4> desiredImageSize( GetDesiredImageSize( GetEnvironment().GetWinRect().GetViewportWidth(),
                                                             GetEnvironment().GetWinRect().GetViewportHeight() ) );
  if ( GetImage().GetDim().m_x != desiredImageSize.m_x ||
       GetImage().GetDim().m_y != desiredImageSize.m_y )
  {
    GetImage().Resize( desiredImageSize );
  }

  // make sure the color table is available
  if ( GetEnvironment().GetColormap() == NULL )
    throw ex::AbortOperationException( LogRec( "colormap is null", "vxRendererSoftwareMPRCurved", "PreRender" ) );

  // begin deciding if we need to re-render
  SetRenderingNecessary( false );
  
  // has the volume changed?
  if ( GetEnvironment().IsModified( vxEnvironment::VOLUME_INTENSITY ) == true )
    SetRenderingNecessary( true );

  // has the transfer function changed?
  if ( GetEnvironment().IsModified( vxEnvironment::COLORMAP ) == true )
    SetRenderingNecessary( true );

  // has the camera changed?
  if ( GetEnvironment().IsModified( vxEnvironment::CAMERA ) == true )
    SetRenderingNecessary( true );

  // has the rendering mode changed?
  if ( GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE ) == true )
    SetRenderingNecessary( true );

  // has the plane mpr changed?
  if ( GetEnvironment().IsModified( vxEnvironment::PLANE_MPR ) == true )
    SetRenderingNecessary( true );
  
  // have the components changed?
  if ( GetEnvironment().IsModified( vxEnvironment::COMPONENT_VISUAL ) )
    SetRenderingNecessary( true );

  // has the vessel array changed?
  if ( GetEnvironment().IsModified( vxIDs::VesselArray ) )
    SetRenderingNecessary( true );

  // check if the window has changed
  if ( m_lastWinRect != GetEnvironment().GetWinRect() )
  {
    m_lastWinRect = GetEnvironment().GetWinRect();
    SetRenderingNecessary( true );
    vxManipulatorVesselCurved::ZoomEntireVolume( GetEnvironment(), Triple<uint4>( GetEnvironment().GetWinRect().GetViewportWidth(), GetEnvironment().GetWinRect().GetViewportHeight(), 0 ) );
  }

  // update the scanlines if anything has changed
  if ( GetRenderingNecessary() )
  {
    SetupScanlines();
    GetImage().Clear();
  }

  return 1;

} // PreRender()


/**
 * Setup the scanline ends.
 */
void vxRendererSoftwareMPRCurved::SetupScanlines()
{
  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurved", "SetupScanlines" );
    return;
  }
  
  // get the intensity volume
  vxBlockVolume<uint2> * pIntensityVolume = dynamic_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume());
  if (pIntensityVolume == NULL)
  {
    LogDbg( "Intensity volume pointer is NULL", "vxRendererSoftwareMPRCurved", "SetupScanlines" );
    return;
  }

  // get the ribbon
  vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL )
  {
    LogDbg( "Ribbon pointer is NULL", "vxRendererSoftwareMPRCurved", "SetupScanlines" );
    return;
  }

  // compute the ribbon
  pRibbon->ComputeScanlinesCurved( * pVessel, GetEnvironment(), Triple<uint4>( GetEnvironment().GetWinRect().GetViewportWidth(),
                                                                               GetEnvironment().GetWinRect().GetViewportHeight(), 0 ) );

  // The ribbon is lazily computed as needed.  The renderer should not modify any data, but in this case, it does.
  // For performance reasons the scanlines are computed as needed because they are a derivative of several
  // elements of the environment, not just a single one.  For instance, the user could rotate the curved
  // MPR plane using a manipulator in vxBase, but this affects the scanlines which need to be recomputed.
  // Since there is no manipulator for the scanlines, but just the individual elements, the scanline
  // data has to be updated somewhere.  All this would be alleviated if there were a centralized mechanism
  // to update data as needed before any rendering takes place.  This is postponed to a future version.
  //pRibbon->Modified( vxModInfoID( vxElement, vxVascularID::Ribbon, 0 ) );

  // any time you change the ribbon, you must re-render
  SetRenderingNecessary(true);

} // SetupScanlines()


/**
 * Render a section of the window
 *
 * @param window the part of the image to render
 *
 * @return true if computed
 */
int4 vxRendererSoftwareMPRCurved::RenderWindow( const BoundingRect<uint2> & window, const uint1 & uThreadID )
{
  // do not render if unncessary
  if ( GetRenderingNecessary() == false )
    return 1;


  // set up the rendering environment
  vxEnvironment & renderEnv( GetEnvironment() );
  int4 iRes(0);

  // select the proper renderer
  if ( renderEnv.GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::MIP )
  {
      iRes = RenderMIP( window, renderEnv, false );
  }
  else if ( renderEnv.GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::THIN_SLAB)
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

}


int4 vxRendererSoftwareMPRCurved::RenderMIP( const BoundingRect<uint2> & renderWindow, vxEnvironment &renderEnv, bool bSlab )
{
  Timer rednerTimer;

  // get the intensity volume
  vxBlockVolume<uint2> * pIntensityVolume = dynamic_cast<vxBlockVolume<uint2> *>( renderEnv.GetIntensityVolume() );
  if ( pIntensityVolume == NULL )
  {
    LogDbg( "Intensity volume pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderMIP" );
    return 0;
  }

  // get the ribbon
  vxRibbon * pRibbon = static_cast<vxRibbon *>( renderEnv.GetElement( vxIDs::Ribbon ) );
  if (pRibbon == NULL)
  {
    LogDbg( "Ribbon pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderMIP" );
    return 0;
  }

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderMIP" );
    return 0;
  }

  // make sure there is something to render
  if ( pVessel->size() < 2 )
  {
    GetImage().Clear(); 
    return 0;
  }


  // Get the scaling matrix
  const Vector3D<float4> units( pIntensityVolume->GetHeader().GetVoxelUnits() );
  Matrix<float4> mWorldToVolume;
  mWorldToVolume.Scale( Vector<float4>( 1.0F /units.X(), 1.0F/units.Y(), 1.0F/units.Z() ) );
  Matrix<float4> mVolumeToWorld;
  mVolumeToWorld.Scale( Vector<float4>( units.X(), units.Y(), units.Z() ) );


  // Get the render camera view direction  
  const Normal<float4> vViewDirection( pRibbon->GetRenderCamera().GetViewDirection() ); 
  const float4 fMIPMarginThicknessWorld( 1.0 ); // we put 1.0mm margine
  const float4 fMIPSlabThicknessWorld( 0.5F*renderEnv.GetRenderingMode()->GetSlabThickness() ); // radius

  const uint4 uSliceWidth( GetImage().GetDim().m_x );
  const uint4 uSliceHeight( GetImage().GetDim().m_y );

  // some items used frequently inside the loop
  uint1 * pImage( reinterpret_cast< uint1 * >( GetImage().GetDataPtr() ) );
  const float * pfRGBA( GetTablePointerRGBA() );

  vxBlockVolumeRayIterator<uint2> iterRayIntensity( * pIntensityVolume );
  vxBlockVolumeRayIterator<uint2> iterMIPIntensity( * pIntensityVolume );
  const float4 fMIPSamplingStep( m_fMPRRenderSamplingStep ); 

  // loop over scanlines
  for ( int4 y( renderWindow.GetMinY() ); y<renderWindow.GetMaxY() && y<pRibbon->GetScanlineData().size(); y++ )
  { 
    Point<float4> leftPointWorld(pRibbon->GetScanlineData()[y].m_leftPoint);
    Point<float4> rightPointWorld(pRibbon->GetScanlineData()[y].m_rightPoint);
    Point<float4> leftPointVol(mWorldToVolume * leftPointWorld);
    Point<float4> rightPointVol(mWorldToVolume * rightPointWorld);

    Point<float4> centerPointWorld( pRibbon->GetScanlineData()[y].m_centerlinePoint );
    Point<float4> centerPointVol( mWorldToVolume*centerPointWorld );

    float4 fRadiusWorld( bSlab ? 
                          fMIPSlabThicknessWorld : 
                          min( pRibbon->GetScanlineData()[y].m_fRadius, fMIPSlabThicknessWorld ) ) ;
                          //pRibbon->GetScanlineData()[y].m_fRadius + fMIPMarginThicknessWorld );

    // check for a valid scanline
    const Vector<float4> lineDirectionVector( rightPointVol - leftPointVol );
    if ( lineDirectionVector.GetLength() >= 1.0f )
    {
      // set up the ray segment
      // note this only works for full scanlines, it will break if the render window is tiled in the x-direction!
      const float4 fSampleDistance( lineDirectionVector.GetLength() / GetEnvironment().GetWinRect().GetViewportWidth() );
      RaySegment raySegmentIntensity( leftPointVol, rightPointVol );      
      Vector4Df sampleIncVecIntensity( raySegmentIntensity.GetDirection().GetNormalizedXYZ() * fSampleDistance );

      iterRayIntensity.SetSampleIncrementVector( sampleIncVecIntensity );
      iterRayIntensity.SetRaySegment( raySegmentIntensity );

      // set up the MIP parts
      Normal<float4> lineDirectionNormal( lineDirectionVector );
      Point<float4> MIPLeftPointWorld( centerPointWorld - 2.0F*fRadiusWorld*lineDirectionNormal );
      Point<float4> MIPRightPointWorld( centerPointWorld + 2.0F*fRadiusWorld*lineDirectionNormal );
      Point<float4> MIPLeftPointVol( mWorldToVolume*MIPLeftPointWorld );
      Point<float4> MIPRightPointVol( mWorldToVolume*MIPRightPointWorld );

      const float4 fMIPCenterX( leftPointVol.GetEuclideanDistance( centerPointVol ) / fSampleDistance );
      const float4 fMIPLeftX( leftPointVol.GetEuclideanDistance( MIPLeftPointVol ) / fSampleDistance );
      const float4 fMIPRightX( leftPointVol.GetEuclideanDistance( MIPRightPointVol ) / fSampleDistance );
      const float4 fRadiusX( fabs( fMIPRightX - fMIPLeftX ) / 4.0F );

      int4 iMIPLeftX(fMIPLeftX);
      int4 iMIPRightX(fMIPRightX);

      if ( bSlab )
      {
        iMIPLeftX = 0;
        iMIPRightX = uSliceWidth-1;
      }

      Vector<float4> vMIPDirectionWorld( vViewDirection*(fRadiusWorld) );
      Vector<float4> vMIPDirectionVol( mWorldToVolume*vMIPDirectionWorld );

      // loop over pixels in scanline
      uint1 * pImageLine = pImage + 4 * ( GetImage().GetDim().m_x * y );
      uint1 uRed  ( 0 );
      uint1 uGreen( 0 );
      uint1 uBlue ( 0 );
      for ( int4 x=0; x<uSliceWidth; ++x, ++iterRayIntensity )
      {
        // see if we are in a valid
        if ( y < uSliceHeight )
        {
          float4 fMIPVoxel( iterRayIntensity.GetSample() ); 

          if ( x > iMIPLeftX && x < iMIPRightX )
          { 
            const Point<float4> vCenterPntVol( iterRayIntensity.GetPosFloat() );

            float4 fMIPDepth(1.0F);
            float4 fX2Center(fabs(fMIPCenterX-x));
            if ( !bSlab && fX2Center > fRadiusX )
            {
              fMIPDepth = max( 0.001F, 1.0-(fX2Center-fRadiusX)/fRadiusX );  // to avoid 0.0F
            }

            Point<float4> vMIPStartPoint( vCenterPntVol-vMIPDirectionVol*fMIPDepth );
            Point<float4> vMIPEndPoint( vCenterPntVol+vMIPDirectionVol*fMIPDepth );            

            RaySegment MIPSegmentIntensity( vMIPStartPoint, vMIPEndPoint );          
            Vector4Df sampleMIPIncVecIntensity( MIPSegmentIntensity.GetDirection().GetNormalizedXYZ()*fMIPSamplingStep );

            iterMIPIntensity.SetSampleIncrementVector( sampleMIPIncVecIntensity );
            iterMIPIntensity.SetRaySegment( MIPSegmentIntensity );

            for ( ; !iterMIPIntensity.IsAtEnd(); ++iterMIPIntensity )
            {               
              float4 fVoxel( iterMIPIntensity.GetSample() );
              if ( fVoxel > fMIPVoxel )
                fMIPVoxel = fVoxel;
            } 
          }

          const uint4 uIntensity( fMIPVoxel+0.5F );

          // apply the transfer function
          const bool bShowColor( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR ) );
          if ( bShowColor )
          {
            // color
            const float4 * pColorTableEntry(pfRGBA + 4 * uIntensity);
            uRed   = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
            uGreen = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
            uBlue  = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
          }
          else
          {
            // grayscale
            uRed = uGreen = uBlue = (uint2)pfRGBA[4*uIntensity + 3] >> GetNumberOfShiftBitsFor8bitAlpha();
          }

          // write pixel
          *pImageLine++ = uRed;
          *pImageLine++ = uGreen;
          *pImageLine++ = uBlue;
          *pImageLine++ = 255; // alpha
        }
        else
        {
          pImageLine += 4;
        }

      } // loop over pixels on scaline

    } // if scanline has data
    else
    {
      // debug scanline that has no data
      if ( false )
      {
        uint1 * pImageLine = pImage + 4 * ( GetImage().GetDim().m_x * y );
        for (int4 x=0; x<uSliceWidth; ++x )
        {
          if ( y < uSliceHeight )
          {
            const bool bScanlineBefore( pRibbon->GetScanlineData()[y].m_fSectionIndex == -1 );
            const bool bScanlineAfter ( pRibbon->GetScanlineData()[y].m_fSectionIndex == -2 );
            *pImageLine++ = 255 * bScanlineBefore; // red
            *pImageLine++ = 255 * bScanlineAfter; // green
            *pImageLine++ = 0; // blue
            *pImageLine++ = 255;    // alpha
          }
        }
      }
    }
  } // loop over scanlines

  //LogDbg( rednerTimer.AsString() + " MIP Curve-MPR redner finished!" , "vxRendererSoftwareMPRCurved", "RenderWindow" );
  return 1;

} // RenderMIP()



int4 vxRendererSoftwareMPRCurved::RenderXRay( const BoundingRect<uint2> & renderWindow, vxEnvironment &renderEnv )
{

  // get the intensity volume
  vxBlockVolume<uint2> * pIntensityVolume = dynamic_cast<vxBlockVolume<uint2> *>( renderEnv.GetIntensityVolume() );
  if ( pIntensityVolume == NULL )
  {
    LogDbg( "Intensity volume pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderXRay" );
    return 0;
  }

  // get the ribbon
  vxRibbon * pRibbon = static_cast<vxRibbon *>( renderEnv.GetElement( vxIDs::Ribbon ) );
  if (pRibbon == NULL)
  {
    LogDbg( "Ribbon pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderXRay" );
    return 0;
  }

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderXRay" );
    return 0;
  }

  // make sure there is something to render
  if ( pVessel->size() < 2 )
  {
    GetImage().Clear(); 
    return 0;
  }

  // Get the scaling matrix
  const Vector3D<float4> units( pIntensityVolume->GetHeader().GetVoxelUnits() );
  Matrix<float4> mWorldToVolume;
  mWorldToVolume.Scale( Vector<float4>( 1.0F /units.X(), 1.0F/units.Y(), 1.0F/units.Z() ) );
  Matrix<float4> mVolumeToWorld;
  mVolumeToWorld.Scale( Vector<float4>( units.X(), units.Y(), units.Z() ) );


  // Get the render camera view direction  
  const Normal<float4> vViewDirection( pRibbon->GetRenderCamera().GetViewDirection() );
  const float4 fMarginThickness( 1.0F ); 

  const uint4 uSliceWidth( GetImage().GetDim().m_x );
  const uint4 uSliceHeight( GetImage().GetDim().m_y );

  // Get the maximum and minimum radius
  float4 fMaxRadiusWorld(0.0F);
  for ( int4 y( renderWindow.GetMinY() ); y<renderWindow.GetMaxY() && y<pRibbon->GetScanlineData().size(); ++y )
  {
    float fRadiusWorld( pRibbon->GetScanlineData()[y].m_fRadius );
    if ( fMaxRadiusWorld < fRadiusWorld )
      fMaxRadiusWorld = fRadiusWorld;
  }  

  // some items used frequently inside the loop
  uint1 * pImage( reinterpret_cast< uint1 * >( GetImage().GetDataPtr() ) );
  const float * pfRGBA( GetTablePointerRGBA() );
  const float4 fXRaySamplingStep( 1.0F ); 
  const float4 fXRaySlabThicknessWorld( 0.5F*renderEnv.GetRenderingMode()->GetSlabThickness() ); // radius

  vxBlockVolumeRayIterator<uint2> iterRayIntensity( * pIntensityVolume );
  vxBlockVolumeRayIterator<uint2> iterXRayIntensity( * pIntensityVolume );

  // loop over scanlines
  for ( int4 y( renderWindow.GetMinY() ); y<renderWindow.GetMaxY() && y<pRibbon->GetScanlineData().size(); y++ )
  { 
    Point<float4> leftPointWorld(pRibbon->GetScanlineData()[y].m_leftPoint);
    Point<float4> rightPointWorld(pRibbon->GetScanlineData()[y].m_rightPoint);
    Point<float4> leftPointVol(mWorldToVolume * leftPointWorld);
    Point<float4> rightPointVol(mWorldToVolume * rightPointWorld);

    Point<float4> centerPointWorld( pRibbon->GetScanlineData()[y].m_centerlinePoint );
    Point<float4> centerPointVol( mWorldToVolume*centerPointWorld );

    float4 fRadiusWorld( min( pRibbon->GetScanlineData()[y].m_fRadius, fXRaySlabThicknessWorld ) ) ;

    // check for a valid scanline
    const Vector<float4> lineDirectionVector( rightPointVol - leftPointVol );
    if ( lineDirectionVector.GetLength() >= 1.0f )
    {
      // set up the ray segment
      // note this only works for full scanlines, it will break if the render window is tiled in the x-direction!
      const float4 fSampleDistance( lineDirectionVector.GetLength() / GetEnvironment().GetWinRect().GetViewportWidth() );
      RaySegment raySegmentIntensity( leftPointVol, rightPointVol );      
      Vector4Df sampleIncVecIntensity( raySegmentIntensity.GetDirection().GetNormalizedXYZ() * fSampleDistance );

      iterRayIntensity.SetSampleIncrementVector( sampleIncVecIntensity );
      iterRayIntensity.SetRaySegment( raySegmentIntensity );

      // set up the xray parts
      Normal<float4> lineDirectionNormal( lineDirectionVector );
      Point<float4> XRayLeftPointWorld( centerPointWorld - 2.0F*fRadiusWorld*lineDirectionNormal );
      Point<float4> XRayRightPointWorld( centerPointWorld + 2.0F*fRadiusWorld*lineDirectionNormal );
      Point<float4> XRayLeftPointVol( mWorldToVolume*XRayLeftPointWorld );
      Point<float4> XRayRightPointVol( mWorldToVolume*XRayRightPointWorld );

      const float4 fXRayCenterX( leftPointVol.GetEuclideanDistance( centerPointVol ) / fSampleDistance );
      const float4 fXRayLeftX( leftPointVol.GetEuclideanDistance( XRayLeftPointVol ) / fSampleDistance );
      const float4 fXRayRightX( leftPointVol.GetEuclideanDistance( XRayRightPointVol ) / fSampleDistance );
      const float4 fRadiusX( fabs( fXRayRightX - fXRayLeftX ) / 4.0F );

      const int4 iXRayLeftX(fXRayLeftX);
      const int4 iXRayRightX(fXRayRightX);

      Vector<float4> vXRayDirectionWorld( vViewDirection*(fRadiusWorld+fMarginThickness) );
      Vector<float4> vXRayDirectionVol( mWorldToVolume*vXRayDirectionWorld );

      // loop over pixels in scanline
      uint1 * pImageLine( pImage + 4 * ( GetImage().GetDim().m_x * y ) );
      uint1 uRed  ( 0 );
      uint1 uGreen( 0 );
      uint1 uBlue ( 0 );
      for ( int4 x=0; x<uSliceWidth; ++x, ++iterRayIntensity )
      {
        // see if we are in a valid
        if ( y < uSliceHeight )
        {
          float4 fAvgVoxel( 0.0F ); 
          float4 fOpticLength( 0.0F );

          if ( x > iXRayLeftX && x < iXRayRightX )
          { 
            const Point<float4> vCenterPnt( iterRayIntensity.GetPosFloat() );
            
            float4 fXRayDepth(1.0F);
            float4 fX2Center(fabs(fXRayCenterX-x));
            if ( fX2Center > fRadiusX )
            {
              fXRayDepth = max( 0.001F, 1.0-(fX2Center-fRadiusX)/fRadiusX );  // to avoid 0.0F
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

            fOpticLength = 2.0F*fXRayDepth*fRadiusWorld + fMarginThickness;
          }
          else
          {
            fAvgVoxel = iterRayIntensity.GetSample();
            fAvgVoxel = pfRGBA[4*(int2)(fAvgVoxel) + 3];
            fOpticLength = fMarginThickness;
          }

          fAvgVoxel = ( (uint2)fAvgVoxel >> GetNumberOfShiftBitsFor8bitAlpha() );
          fAvgVoxel /= 255.0F;
          fAvgVoxel *= fAvgVoxel;
          fAvgVoxel *= fAvgVoxel;
          
          fOpticLength /= fMaxRadiusWorld; 

          float4 fAlpha = exp( -1.0F * fAvgVoxel * fOpticLength );
          
          const uint1 uIntensity( 255.0F * fAlpha );
          //uRed = uGreen = uBlue = (uint2)pfRGBA[4*uIntensity + 3] >> GetNumberOfShiftBitsFor8bitAlpha();
          
          // write pixel
          *pImageLine++ = uIntensity;
          *pImageLine++ = uIntensity;
          *pImageLine++ = uIntensity;
          *pImageLine++ = 255; // alpha
        }
        else
        {
          pImageLine += 4;
        }

      } // loop over pixels on scaline

    } // if scanline has data
    else
    {
      // debug scanline that has no data
      if ( false )
      {
        uint1 * pImageLine( pImage + 4 * ( GetImage().GetDim().m_x * y ) );
        for (int4 x=0; x<uSliceWidth; ++x )
        {
          if ( y < uSliceHeight )
          {
            const bool bScanlineBefore( pRibbon->GetScanlineData()[y].m_fSectionIndex == -1 );
            const bool bScanlineAfter ( pRibbon->GetScanlineData()[y].m_fSectionIndex == -2 );
            *pImageLine++ = 255 * bScanlineBefore; // red
            *pImageLine++ = 255 * bScanlineAfter; // green
            *pImageLine++ = 0; // blue
            *pImageLine++ = 255;    // alpha
          }
        }
      }
    }

  } // loop over scanlines

  return 1;

} // RenderXRay()




int4 vxRendererSoftwareMPRCurved::RenderSingleLayer( const BoundingRect<uint2> & renderWindow, vxEnvironment & renderEnv )
{

  try
  {

    // get the intensity volume
    vxBlockVolume<uint2> * pIntensityVolume = dynamic_cast<vxBlockVolume<uint2> *>( renderEnv.GetIntensityVolume() );
    if ( pIntensityVolume == NULL )
    {
        LogDbg( "Intensity volume pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderSingleLayer" );
      return 0;
    }

    // get the ribbon
    vxRibbon * pRibbon = static_cast<vxRibbon *>( renderEnv.GetElement( vxIDs::Ribbon ) );
    if (pRibbon == NULL)
    {
        LogDbg( "Ribbon pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderSingleLayer" );
      return 0;
    }

    // get the current vessel
    vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
    if (pVessel == NULL)
    {
        //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderSingleLayer" );
      return 0;
    }

    // make sure there is something to render
    if ( pVessel->size() < 2 )
    {
      GetImage().Clear(); 
      return 0;
    }

    // Get the scaling matrix
    const Vector3D<float4> units( pIntensityVolume->GetHeader().GetVoxelUnits() );
    Matrix<float4> mWorldToVolume;
    mWorldToVolume.Scale( Vector<float4>( 1.0F /units.X(), 1.0F/units.Y(), 1.0F/units.Z() ) );
    Matrix<float4> mVolumeToWorld;
    mVolumeToWorld.Scale( Vector<float4>( units.X(), units.Y(), units.Z() ) );
    float4 fWorldToVolumeScale( Max( 1.0F /units.X(), 1.0F/units.Y(), 1.0F/units.Z() ) );

    const uint4 uSliceWidth( GetImage().GetDim().m_x );
    const uint4 uSliceHeight( GetImage().GetDim().m_y );

    // some items used frequently inside the loop
    Point<uint2> vPixelPosition( 0, 0, 0 );
      uint1 * pImage( reinterpret_cast< uint1 * >( GetImage().GetDataPtr() ) );
    const float * pfRGBA( GetTablePointerRGBA() );
    const float4 fMIPSampleDistance( 0.5F );  // half voxel sampling
    vxBlockVolumeRayIterator<uint2> iterRayIntensity( * pIntensityVolume );
    vxBlockVolumeRayIterator<uint2> iterMIPIntensity( * pIntensityVolume );

    // loop over scanlines
    for ( int4 y( renderWindow.GetMinY() ); y<renderWindow.GetMaxY() && y<pRibbon->GetScanlineData().size(); y++ )
    { 
      Point<float4> leftPointWorld(pRibbon->GetScanlineData()[y].m_leftPoint);
      Point<float4> rightPointWorld(pRibbon->GetScanlineData()[y].m_rightPoint);
      Point<float4> leftPointVol(mWorldToVolume * leftPointWorld);
      Point<float4> rightPointVol(mWorldToVolume * rightPointWorld);

      // check for a valid scanline
      const Vector<float4> lineDirectionVector( rightPointVol - leftPointVol );
      if ( lineDirectionVector.GetLength() >= 1.0f )
      {
        // set up the ray segment
        // note this only works for full scanlines, it will break if the render window is tiled in the x-direction!
        const float4 fSampleDistance( lineDirectionVector.GetLength() / GetEnvironment().GetWinRect().GetViewportWidth() );
        RaySegment raySegmentIntensity( leftPointVol, rightPointVol );      
        Vector4Df sampleIncVecIntensity( raySegmentIntensity.GetDirection().GetNormalizedXYZ() * fSampleDistance );

        iterRayIntensity.SetSampleIncrementVector( sampleIncVecIntensity );
        iterRayIntensity.SetRaySegment( raySegmentIntensity );

        // loop over pixels in scanline
        uint1 * pImageLine( pImage + 4 * ( GetImage().GetDim().m_x * y ) );
        uint1 uRed  ( 0 );
        uint1 uGreen( 0 );
        uint1 uBlue ( 0 );
        for ( int4 x=0; x<uSliceWidth; ++x, ++iterRayIntensity )
        {
          // see if we are in a valid region
          if ( y < uSliceHeight )
          {
            const uint4 uIntensity( iterRayIntensity.GetSample()+0.5F );

            // apply the transfer function
            const bool bShowColor( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR ) );
            if ( bShowColor )
            {
              // color
              const float4 * pColorTableEntry(pfRGBA + 4 * uIntensity);
              uRed   = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
              uGreen = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
              uBlue  = (uint2)(*pColorTableEntry++) >> GetNumberOfShiftBitsFor8bitAlpha();
            }
            else
            {
              // grayscale
              uRed = uGreen = uBlue = (uint2)pfRGBA[4*uIntensity + 3] >> GetNumberOfShiftBitsFor8bitAlpha();
            }

            // write pixel
            *pImageLine++ = uRed;
            *pImageLine++ = uGreen;
            *pImageLine++ = uBlue;
            *pImageLine++ = 255; // alpha
          }
          else
          {
            pImageLine += 4;
          }

        } // loop over pixels on scaline

      } // if scanline has data
      else
      {
        // debug scanline that has no data
        if ( false )
        {
            uint1 * pImageLine( pImage + 4 * ( GetImage().GetDim().m_x * y ) );
            for ( int4 x(0); x<uSliceWidth; ++x )
          {
            if ( y < uSliceHeight )
            {
              const bool bScanlineBefore( pRibbon->GetScanlineData()[y].m_fSectionIndex == -1 );
              const bool bScanlineAfter ( pRibbon->GetScanlineData()[y].m_fSectionIndex == -2 );
              *pImageLine++ = 255 * bScanlineBefore; // red
              *pImageLine++ = 255 * bScanlineAfter; // green
              *pImageLine++ = 0; // blue
              *pImageLine++ = 255;    // alpha
            }
          }
        }
      }

    } // loop over scanlines

  } // try
  catch ( ... )
  {
    LogWrn( "Rendering failed", "vxRendererSoftwareMPRCurved", "RenderSingleLayer" );
  }

  return 1;

} // RenderSingleLayer()


/**
 * Draw the image stored in the texture tile
 */
void vxRendererSoftwareMPRCurved::DrawTexture()
{
  // Simply set the OpenGL texture environment and draw the slice
  GLuint id(GetTextureTile().GetDensityTextureId());
  glBindTexture(GL_TEXTURE_2D, id);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // save the OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0F, 1.0F, 0.0F, 1.0F);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  // GL draw textured tile:
  glColor3f(1, 1, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_TEXTURE_2D);
  {
    glBegin(GL_QUADS); 
    {

      float4 fTextureX( static_cast< float4 >( GetEnvironment().GetWinRect().GetViewportWidth() ) / static_cast< float4 >( GetImage().GetDim().m_x ) );
      float4 fTextureY( static_cast< float4 >( GetEnvironment().GetWinRect().GetViewportHeight() ) / static_cast< float4 >( GetImage().GetDim().m_y ) );

      glTexCoord2f( 0.0F, 0.0F );
      glVertex2f  ( 0.0F, 0.0F );

      glTexCoord2f( fTextureX, 0.0F );
      glVertex2f  ( 1.0F     , 0.0F );

      glTexCoord2f( fTextureX, fTextureY );
      glVertex2f  ( 1.0F     , 1.0F      );

      glTexCoord2f( 0.0F, fTextureY );
      glVertex2f  ( 0.0F, 1.0F      );
    } 
    glEnd();
    glFlush();
  }
  glDisable(GL_TEXTURE_2D);

  // restore the OpenGL state
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();

} // DrawTexture()


///**
// * Convert an image from PixelRGB<uint1> to PixelRGBA<uint1>
// *
// * @param outImage the output image
// * @param inImage  the input image
// */
//void ConvertImage( Image< PixelRGB< uint1 > > & outImage, const Image< PixelRGBA< uint1 > > & inImage )
//{
//  outImage.Resize( inImage.GetDim() );
//  for ( int4 y=0; y<outImage.GetDim().m_y; ++y )
//  {
//    for ( int4 x=0; x<outImage.GetDim().m_x; ++x )
//    {
//      const Point2D< uint2 > pos( x, y );
//      const PixelRGBA< uint1 > & srcPixel = inImage.GetPixelRaw( pos );
//      const PixelRGB< uint1 > destPixel( srcPixel.R(), srcPixel.G(), srcPixel.B() );
//      outImage.SetPixelRaw( pos, destPixel );
//    } // for x
//  } // for y
//} // ConvertImage()


/**
 * Returns the desired size of the rendered image
 *
 * @param uViewportWidth the width of the viewport
 * @param uViewportHeight the height of the viewport
 *
 * @return the desired image size
 */
Triple<uint4> vxRendererSoftwareMPRCurved::GetDesiredImageSize( const uint4 uViewportWidth, const uint4 uViewportHeight )
{

  uint4 uWidth = GetNextPowerOfTwo( uViewportWidth );
  uint4 uHeight = GetNextPowerOfTwo( uViewportHeight );

  return Triple<uint4>( uWidth, uHeight, 0 );

} // GetDesiredImageSize()


// undefines
#undef FILE_REVISION


// $Log: vxRendererSoftwareMPRCurved.C,v $
// Revision 1.3  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.2  2006/09/25 13:23:35  geconomos
// commented out unneeded  log debugging
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.13  2006/06/05 15:07:38  frank
// updated for visual studio 2005
//
// Revision 1.12  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.11.2.3  2005/08/01 20:05:42  frank
// changed function name according to coding standards
//
// Revision 1.11.2.2  2005/07/26 13:57:28  frank
// Issue #4307: Fixed missing overlay text
//
// Revision 1.11.2.1  2005/06/16 18:41:55  frank
// cleaning up code
//
// Revision 1.11  2005/06/08 15:02:09  vxconfig
// formatting
//
// Revision 1.10  2005/06/07 18:13:20  vxconfig
// removed the vessel array
//
// Revision 1.9  2005/05/20 19:09:39  frank
// fixed 3D cursor on 3x3 view, fixed active environment and oblique 'self-shadowing' problem
//
// Revision 1.8  2005/05/03 15:52:42  frank
// Adjusted curved MPR rendering for non-uniform aspect ratio windows
//
// Revision 1.7  2001/01/03 19:21:59  michael
// "Replaced" ActiveViewerType by ActiveEnvironment.
//
// Revision 1.6  2001/01/01 18:04:07  frank
// removed unused cine series
//
// Revision 1.5  2004/10/18 14:52:44  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.4  2004/10/05 14:43:40  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.3  2004/09/02 16:24:53  frank
// decreased the granularity of component array modified bits
//
// Revision 1.2  2004/08/31 00:03:49  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.23  2004/03/02 01:10:30  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.22.2.1  2004/02/25 15:00:17  frank
// formatting
//
// Revision 1.22  2004/01/14 18:36:46  wenli
// Reverse the XRay intensity
//
// Revision 1.21  2004/01/14 16:59:31  frank
// optimized redraws
//
// Revision 1.20  2004/01/13 14:53:13  frank
// Fixed problem with lack of update when toggling full screen mode of curved mpr view
//
// Revision 1.19  2004/01/07 18:26:21  frank
// removed old warnings
//
// Revision 1.18  2003/12/23 14:32:40  frank
// fixed 3D overview slice shadows
//
// Revision 1.17  2003/12/22 21:24:13  wenli
// remvoe margine for XRay rendering
//
// Revision 1.16  2003/12/18 17:05:32  frank
// Optimizing redraws
//
// Revision 1.15  2003/12/18 14:15:49  frank
// Renamed the element ID filename to match class name
//
// Revision 1.14  2003/12/15 16:09:10  frank
// Issue #3569: Initialized the oblique and curved views from annotations
//
// Revision 1.13  2003/11/19 21:29:21  wenli
// setup flat slab curve MPR
//
// Revision 1.12  2003/11/18 15:54:03  wenli
// 3D Biconvex Slab MIP
//
// Revision 1.11  2003/10/29 20:58:29  frank
// Removed double rendering in curved MPR
//
// Revision 1.10  2003/08/27 14:35:43  michael
// reworked the rendering mode by sorting flags into groups
//
// Revision 1.9  2003/07/24 17:53:53  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.8  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.7  2003/06/12 21:21:29  dongqing
// fix MR case of MPR curved image panel
//
// Revision 1.6  2003/06/09 12:29:52  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.5  2003/06/05 14:23:19  frank
// Removed unncessary check
//
// Revision 1.4  2003/05/23 15:43:55  frank
// turned off multithreading
//
// Revision 1.3  2003/05/21 17:51:56  frank
// code review
//
// Revision 1.2  2003/05/16 16:46:31  frank
// reflected interface change to Image class
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.42  2003/05/07 14:11:59  frank
// Checked some more things that might have changed
//
// Revision 1.41  2003/05/02 17:12:10  frank
// code review
//
// Revision 1.40  2003/04/14 19:45:00  frank
// Separated progress percent and progress message interfaces
//
// Revision 1.39  2003/03/21 19:26:48  frank
// Fixed handle selection and flipped vessel so that up is up
//
// Revision 1.38  2003/03/12 22:38:51  geconomos
// Commented out code to initialize the logger.
//
// Revision 1.37  2003/03/10 18:40:54  geconomos
// Added the proper modified event so the curved plane shadow will be active on other views
//
// Revision 1.36  2003/03/10 15:18:23  frank
// Cleaned out dead code, added comments
//
// Revision 1.35  2003/03/07 20:43:25  frank
// Added color transfer function
//
// Revision 1.34  2003/03/05 13:52:25  frank
// Added New Cine Series
//
// Revision 1.33  2003/03/01 13:20:26  frank
// GetWinRect() was removed from vxManipulator
//
// Revision 1.32  2003/02/28 19:26:15  frank
// Added debug for vessel handle selection
//
// Revision 1.31  2003/02/14 19:29:14  frank
// Fixed up initial curved zoom and made it possible to grab the endpoints of selection without using the ALT key
//
// Revision 1.30  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.29  2003/02/11 19:44:38  frank
// dead code removed
//
// Revision 1.28  2003/02/06 20:09:51  frank
// Fixed aspect ratio
//
// Revision 1.27.2.1  2003/02/12 23:10:18  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.27  2003/01/24 20:17:36  frank
// Overhauled the ribbon to contain more helpful data per scanline.
//
// Revision 1.26  2003/01/24 18:26:14  frank
// Implemented curved
//
// Revision 1.25  2003/01/23 23:46:13  frank
// fixed centerline coloring
//
// Revision 1.24  2003/01/22 22:18:47  ingmar
// the RayIterator is now only using a RaySegment and a Vector4Df increment to determine how to traverse the ray
//
// Revision 1.23  2003/01/20 19:34:40  wenli
// New Version with updated centerline selection and Hessian Filter
//
// Revision 1.22  2002/12/20 16:26:36  frank
// Change in PixelRGBA()
//
// Revision 1.21  2002/12/06 21:29:00  michael
// removed old fashioned Set/GetRGBATablePtr(); it was only copy of what is in
// the colormap in the environment.
//
// Revision 1.20  2002/12/06 20:21:16  frank
// Reflected name change elsewhere.
//
// Revision 1.19  2002/11/28 02:14:36  frank
// changed zoom to 0.75
//
// Revision 1.18  2002/11/27 23:22:50  frank
// less zoom in curved
//
// Revision 1.17  2002/11/27 21:54:40  frank
// changed throws into debug messages
//
// Revision 1.16  2002/11/27 21:41:29  wenli
// fix the size of the volumeDim-1
//
// Revision 1.15  2002/11/27 19:33:18  wenli
// mix opacity into both MPR
//
// Revision 1.14  2002/11/27 19:09:20  frank
// removed extra modified
//
// Revision 1.13  2002/11/27 16:56:34  frank
// Added some safe pixel access.
//
// Revision 1.12  2002/11/26 19:32:14  ingmar
// now using Vector4d<float4> to keep the current position as well as offset
//
// Revision 1.11  2002/11/26 18:32:00  wenli
// zooming curved-MPR using '+' and '-'
//
// Revision 1.9  2002/11/22 16:41:36  wenli
// antialiasing the label volume on MPR
//
// Revision 1.8  2002/11/18 16:55:41  michael
// made changes to account for changes to vx and v3D plane objects
//
// Revision 1.7  2002/11/15 02:08:23  frank
// Reflected name change in vxCamera.
//
// Revision 1.6  2002/11/15 00:06:22  frank
// Added the shared Ribbon element to the environment for the curved/luminal shadow
//
// Revision 1.5  2002/11/14 00:16:14  wenli
// Vascular Parameter Sets
//
// Revision 1.4  2002/11/11 20:26:48  frank
// Respected vascular preferences when drawing centerline.
//
// Revision 1.3  2002/11/11 20:10:16  frank
// Applied antialiasing to the centerline.
//
// Revision 1.2  2002/11/05 20:27:55  frank
// worked on Pre-Render()
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.14  2002/10/29 15:41:23  geconomos
// Added inclusion of header file.
//
// Revision 1.13  2002/10/25 01:51:49  frank
// Moved working vessel into the VesselArray
//
// Revision 1.12  2002/10/21 22:33:45  frank
// No need to create a new PlaneCurved.
//
// Revision 1.11  2002/10/07 21:38:27  wenli
// view label in MPR
//
// Revision 1.10  2002/09/27 21:47:10  wenli
// add curved MPR camera
//
// Revision 1.9  2002/09/27 14:21:59  geconomos
// Fixed colormap problems.
//
// Revision 1.8  2002/09/25 15:25:24  wenli
// v3D vessel component list
//
// Revision 1.7  2002/09/20 19:41:17  frank
// TRANSFER_FUNCTION -> COLORMAP
// pTF -> pColormap.
// Completed transition to vxColormap.
//
// Revision 1.6  2002/09/19 14:26:02  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.5  2002/09/16 20:22:14  dmitry
// New vxID (s) used.
//
// Revision 1.4  2002/08/23 17:33:59  frank
// Disabled removeElement
//
// Revision 1.3  2002/08/19 14:37:54  wenli
// initial version - Vascular
//
// Revision 1.2  2002/08/15 18:20:36  wenli
// initial version - Vascular
//
// Revision 1.1  2002/08/15 14:26:58  wenli
// initial version - Vascular
//
// Revision 1.8.2.13  2002/05/24 16:16:20  frank
// Cleaned up.
//
// Revision 1.8.2.12  2002/05/23 18:47:07  frank
// Cleaned out old code.
//
// Revision 1.8.2.11  2002/05/23 18:45:13  frank
// Moved to Ribbon data structure.
//
// Revision 1.8.2.10  2002/05/21 22:24:28  frank
// Improved a lot.
//
// Revision 1.8.2.9  2002/05/20 17:19:50  frank
// Changed over to using ray iterators.
//
// Revision 1.8.2.8  2002/05/17 23:19:30  frank
// Rendered using curved plane width.
//
// Revision 1.8.2.7  2002/05/15 17:41:46  frank
// Now it renders a curved plane.
//
// Revision 1.8.2.6  2002/05/15 16:22:17  frank
// Cleaned up image sizes.
//
// Revision 1.8.2.5  2002/05/08 22:36:12  frank
// Made the curved mainpulator work.
//
// Revision 1.8.2.4  2002/05/08 19:36:35  frank
// Updated rendering upon skeleton change.
//
// Revision 1.8.2.3  2002/05/08 18:25:27  frank
// Got it working again.
//
// Revision 1.8.2.2  2002/05/08 17:23:06  frank
// Added Curved MPR rendering.
//
// Revision 1.8.2.1  2002/05/07 21:57:17  frank
// Cleaning up format.
//
// Revision 1.8  2002/05/04 10:46:20  michael
// cosmetics, removed old commented out code
//
// Revision 1.7  2002/05/03 22:25:49  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.6  2002/05/01 22:36:17  michael
// replaced VOLUMESET and VOLUMELABELSET by VOLUME_{INTENSITY,LABEL}
//
// Revision 1.5  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.4  2002/04/25 01:39:48  michael
// no more VLI table, only ColorTableCpu!
//
// Revision 1.3  2002/04/24 17:36:14  jenny
// vxBlockVolume stuff
//
// Revision 1.2  2002/04/24 15:08:13  jenny
// vxBlockVolume stuff
//
// Revision 1.1  2002/04/23 21:39:14  jenny
// new software .C file
//
// Revision 1.3  2002/04/23 15:39:57  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.2  2002/04/11 14:38:30  michael
// added missing private copy constructor and assignment operator
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSoftwareMPRCurved.C,v 1.3 2006/10/24 12:30:53 geconomos Exp $
// $Id: vxRendererSoftwareMPRCurved.C,v 1.3 2006/10/24 12:30:53 geconomos Exp $
