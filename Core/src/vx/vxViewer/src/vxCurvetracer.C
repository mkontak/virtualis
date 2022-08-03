// $Id: vxCurvetracer.C,v 1.7 2007/08/23 19:10:41 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

// includes
#include "StdAfx.h"
#include "vxCurvetracer.h"
#include "Timer.h"
#include "vxBlockVolume.h"
#include "vxRibbon.h"
#include "vxVesselUtilities.h"
#include "vxVessel.h"
#include "Matrix.h"
#include "vxRenderingMode.h"
#include "vxRenderBuffer.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeRayIterator.h"

// defines
#define FILE_REVISION "$Revision: 1.7 $"

/**
 * Constructor
 */
vxCurvetracer::vxCurvetracer()
{
  m_pRenderBuffer = new vxRenderBuffer();
	m_iMaxVoxelValue = 4094;
} // vxCurvetracer


/**
 * Destructor
 */
vxCurvetracer::~vxCurvetracer()
{
  delete m_pRenderBuffer;
} // ~vxCurvetracer()


/**
 * Performs initialization prior to rendering
 */
void vxCurvetracer::PreRender()
{
  SetImageSize();

  m_pRenderBuffer->Clear( PixelRGBA<uint1>( 0,0,0, 0) );

  InitializeRenderArguments();
} // PreRender()


/**
 * Initializes the curvetracer arguments structure
 */
void vxCurvetracer::InitializeRenderArguments()
{
  m_arguments.m_pIntensityVolume = const_cast< vxBlockVolume< uint2 > * >( &m_settings.GetIntensityVolume() );
  m_arguments.m_pRenderingMode = const_cast< vxRenderingMode * >( &m_settings.GetRenderingMode() );
  m_arguments.m_pWinRect = const_cast< vxWinRect * >( &m_settings.GetWinRect() );
  m_arguments.m_pfRGBA = const_cast< float4 * >( m_settings.GetColorTable() );
  m_arguments.m_iShiftBitsFor8bitAlpha = m_settings.GetNumberOfShiftBitsFor8bitAlpha();
  m_arguments.m_pVessel = const_cast< vxVessel * >( &m_settings.GetVessel() );
  m_arguments.m_pRibbon = const_cast< vxRibbon * >( &m_settings.GetRibbon() );
  m_arguments.m_fMPRRenderSamplingStep = 0.5F;

  std::string sModality("");
  sModality = m_settings.GetIntensityVolume().GetHeader().GetDicomFieldValue("0008", "0060", sModality);
  if( sModality != "CT" )
	{
	  int4 iHeaderVersion(0);
	  m_settings.GetIntensityVolume().GetHeader().GetVariableVolumeHeader().GetHdrVar("version", iHeaderVersion);

	  if( iHeaderVersion >= 5 )
	  {
      m_settings.GetIntensityVolume().GetHeader().GetVariableVolumeHeader().GetHdrVar("maxValueRange", m_iMaxVoxelValue);
		  if( m_iMaxVoxelValue > 4096 )
				m_iMaxVoxelValue = 65534;
		}
		else
		{
		  m_iMaxVoxelValue = 65534;
		}
	}  // non CT modality	

} // InitializeRenderArguments()


/**
 * Determines the image size
 */
void vxCurvetracer::SetImageSize()
{
  const uint4 uWidth = GetNextPowerOfTwo( m_settings.GetWinRect().GetViewportWidth() );
  const uint4 uHeight = GetNextPowerOfTwo(  m_settings.GetWinRect().GetViewportHeight() );

  // make sure the image size is what we expect
  const Triple<uint4> desiredImageSize( uWidth, uHeight, 0 );
  const Triple<uint4> & actualImageSize = GetRenderBuffer().GetBufferSize();
  if ( desiredImageSize[0] != actualImageSize[0] || desiredImageSize[1] != actualImageSize[1] )
    GetRenderBuffer().SetBufferSize( desiredImageSize );

} // SetImageSize()


/**
 * Renders into the specified window
 *
 * @param   window        render window
 * @param   uThreadId     id of calling thread
 */
void vxCurvetracer::RenderWindow( const BoundingRect< uint2 > & window, const uint1 & uThreadId )
{
  // select the proper renderer
  if( m_settings.GetRenderingMode().GetRenderingAlgorithm() == vxRenderingMode::MIP )
  {
    RenderMIP( window, m_arguments, false );
  }
  else if ( m_settings.GetRenderingMode().GetRenderingAlgorithm() == vxRenderingMode::THIN_SLAB)
  {
    RenderMIP( window, m_arguments, true );
  }
  else if ( m_settings.GetRenderingMode().GetRenderingAlgorithm() == vxRenderingMode::XRAY )
  {
    RenderXRay( window, m_arguments );
  }
  else
  {
    RenderSingleLayer( window, m_arguments);
  }
} // RenderWindow( const BoundingRect< uint2 > & window, const uint1 & uThreadId )


/**
 * Render a MIP projection
 *
 * @param   renderWindow  window
 * @oparam  args          rendering args
 * @param   bSlab         thin slab indicator
 * @return 0 for success
 */
int4 vxCurvetracer::RenderMIP( const BoundingRect<uint2> & renderWindow, vxCurvetracerRenderArgs & args, bool bSlab )
{
  Timer rednerTimer;

  // get the intensity volume
  vxBlockVolume<uint2> * pIntensityVolume = args.m_pIntensityVolume;
  if ( pIntensityVolume == NULL )
  {
    LogDbg( "Intensity volume pointer is NULL", "vxCurvetracer", "RenderMIP" );
    return 0;
  }

  // get the ribbon
  vxRibbon * pRibbon = args.m_pRibbon;
  if (pRibbon == NULL)
  {
    LogDbg( "Ribbon pointer is NULL", "vxCurvetracer", "RenderMIP" );
    return 0;
  }

  // get the current vessel
  vxVessel * pVessel = args.m_pVessel;
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxCurvetracer", "RenderMIP" );
    return 0;
  }

  // make sure there is something to render
  if ( pVessel->size() < 2 )
  {
    m_pRenderBuffer->GetColorImage().Clear(); 
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
  const float4 fMIPSlabThicknessWorld( 0.5F*args.m_pRenderingMode->GetSlabThickness() ); // radius

  const uint4 uSliceWidth( m_pRenderBuffer->GetColorImage().GetDim().m_x );
  const uint4 uSliceHeight( m_pRenderBuffer->GetColorImage().GetDim().m_y );

  // some items used frequently inside the loop
  uint1 * pImage( reinterpret_cast< uint1 * >( m_pRenderBuffer->GetColorImage().GetDataPtr() ) );
  const float * pfRGBA( args.m_pfRGBA );

  vxBlockVolumeRayIterator<uint2> iterRayIntensity( * pIntensityVolume );
  vxBlockVolumeRayIterator<uint2> iterMIPIntensity( * pIntensityVolume );
  const float4 fMIPSamplingStep( args.m_fMPRRenderSamplingStep ); 

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
      const float4 fSampleDistance( lineDirectionVector.GetLength() / args.m_pWinRect->GetViewportWidth() );
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
      uint1 * pImageLine = pImage + 4 * ( m_pRenderBuffer->GetColorImage().GetDim().m_x * y );
      uint1 uRed  ( 0 );
      uint1 uGreen( 0 );
      uint1 uBlue ( 0 );
      for ( int4 x=0; x<uSliceWidth; ++x, ++iterRayIntensity )
      {
        // see if we are in a valid
        if ( y < uSliceHeight )
        {
          float4 fMIPVoxel( min(m_iMaxVoxelValue, (iterRayIntensity.GetSampleInt()>0 ? iterRayIntensity.GetSampleInt() : 0)));

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
              float4 fVoxel( min(m_iMaxVoxelValue, (iterMIPIntensity.GetSampleInt()>0 ? iterRayIntensity.GetSampleInt() : 0)));
              if ( fVoxel > fMIPVoxel )
                fMIPVoxel = fVoxel;
            } 
          }

          const uint4 uIntensity( fMIPVoxel+0.5F );

          // apply the transfer function
          const bool bShowColor( args.m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR ) );
          if ( bShowColor )
          {
            // color
            const float4 * pColorTableEntry(pfRGBA + 4 * uIntensity);
            uRed   = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
            uGreen = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
            uBlue  = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
          }
          else
          {
            // grayscale
            uRed = uGreen = uBlue = (uint2)pfRGBA[4*uIntensity + 3] >> args.m_iShiftBitsFor8bitAlpha;
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
        uint1 * pImageLine = pImage + 4 * ( m_pRenderBuffer->GetColorImage().GetDim().m_x * y );
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

  //LogDbg( rednerTimer.AsString() + " MIP Curve-MPR redner finished!" , "vxCurvetracer", "RenderWindow" );
  return 1;

} // RenderMIP()


/**
 * Render a X-RAY projection
 *
 * @param   renderWindow  window
 * @oparam  args          rendering args
 * @return 0 for success
 */
int4 vxCurvetracer::RenderXRay( const BoundingRect<uint2> & renderWindow, vxCurvetracerRenderArgs & args )
{
  // get the intensity volume
  vxBlockVolume<uint2> * pIntensityVolume = args.m_pIntensityVolume;
  if ( pIntensityVolume == NULL )
  {
    LogDbg( "Intensity volume pointer is NULL", "vxCurvetracer", "RenderMIP" );
    return 0;
  }

  // get the ribbon
  vxRibbon * pRibbon = args.m_pRibbon;
  if (pRibbon == NULL)
  {
    LogDbg( "Ribbon pointer is NULL", "vxCurvetracer", "RenderMIP" );
    return 0;
  }

  // get the current vessel
  vxVessel * pVessel = args.m_pVessel;
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxCurvetracer", "RenderMIP" );
    return 0;
  }

  // make sure there is something to render
  if ( pVessel->size() < 2 )
  {
    m_pRenderBuffer->GetColorImage().Clear(); 
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

  const uint4 uSliceWidth( m_pRenderBuffer->GetColorImage().GetDim().m_x );
  const uint4 uSliceHeight( m_pRenderBuffer->GetColorImage().GetDim().m_y );

  // Get the maximum and minimum radius
  float4 fMaxRadiusWorld(0.0F);
  for ( int4 y( renderWindow.GetMinY() ); y<renderWindow.GetMaxY() && y<pRibbon->GetScanlineData().size(); ++y )
  {
    float fRadiusWorld( pRibbon->GetScanlineData()[y].m_fRadius );
    if ( fMaxRadiusWorld < fRadiusWorld )
      fMaxRadiusWorld = fRadiusWorld;
  }  

  // some items used frequently inside the loop
  uint1 * pImage( reinterpret_cast< uint1 * >( m_pRenderBuffer->GetColorImage().GetDataPtr() ) );
  const float * pfRGBA( args.m_pfRGBA );
  const float4 fXRaySamplingStep( 1.0F ); 
  const float4 fXRaySlabThicknessWorld( 0.5F*args.m_pRenderingMode->GetSlabThickness() ); // radius

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
      const float4 fSampleDistance( lineDirectionVector.GetLength() / args.m_pWinRect->GetViewportWidth() );
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
      uint1 * pImageLine( pImage + 4 * ( m_pRenderBuffer->GetColorImage().GetDim().m_x * y ) );
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
              float4 fVoxel( min(m_iMaxVoxelValue, (iterRayIntensity.GetSampleInt()>0 ? iterRayIntensity.GetSampleInt() : 0)) );
              fAvgVoxel += pfRGBA[4*(int2)(fVoxel) + 3];
            } 
            fAvgVoxel /= iCount;

            fOpticLength = 2.0F*fXRayDepth*fRadiusWorld + fMarginThickness;
          }
          else
          {
            fAvgVoxel = min(m_iMaxVoxelValue, (iterRayIntensity.GetSampleInt()>0 ? iterRayIntensity.GetSampleInt() : 0));
            fAvgVoxel = pfRGBA[4*(int2)(fAvgVoxel) + 3];
            fOpticLength = fMarginThickness;
          }

          fAvgVoxel = ( (uint2)fAvgVoxel >> args.m_iShiftBitsFor8bitAlpha );
          fAvgVoxel /= 255.0F;
          fAvgVoxel *= fAvgVoxel;
          fAvgVoxel *= fAvgVoxel;
          
          fOpticLength /= fMaxRadiusWorld; 

          float4 fAlpha = exp( -1.0F * fAvgVoxel * fOpticLength );
          
          const uint1 uIntensity( 255.0F * fAlpha );
          //uRed = uGreen = uBlue = (uint2)pfRGBA[4*uIntensity + 3] >> args.m_iShiftBitsFor8bitAlpha;
          
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
        uint1 * pImageLine( pImage + 4 * ( m_pRenderBuffer->GetColorImage().GetDim().m_x * y ) );
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


/**
 * Render a single layer projection
 *
 * @param   renderWindow  window
 * @oparam  args          rendering args
 * @return 0 for success
 */
int4 vxCurvetracer::RenderSingleLayer( const BoundingRect<uint2> & renderWindow, vxCurvetracerRenderArgs & args )
{
  try
  {
    // get the intensity volume
    vxBlockVolume<uint2> * pIntensityVolume = args.m_pIntensityVolume;
    if ( pIntensityVolume == NULL )
    {
      LogDbg( "Intensity volume pointer is NULL", "vxCurvetracer", "RenderMIP" );
      return 0;
    }

    // get the ribbon
    vxRibbon * pRibbon = args.m_pRibbon;
    if (pRibbon == NULL)
    {
      LogDbg( "Ribbon pointer is NULL", "vxCurvetracer", "RenderMIP" );
      return 0;
    }

    // get the current vessel
    vxVessel * pVessel = args.m_pVessel;
    if (pVessel == NULL)
    {
      //LogDbg( "Vessel pointer is NULL", "vxCurvetracer", "RenderMIP" );
      return 0;
    }
   
    // make sure there is something to render
    if ( pVessel->size() < 2 )
    {
      m_pRenderBuffer->GetColorImage().Clear(); 
      return 0;
    }

    // Get the scaling matrix
    const Vector3D<float4> units( pIntensityVolume->GetHeader().GetVoxelUnits() );
    Matrix<float4> mWorldToVolume;
    mWorldToVolume.Scale( Vector<float4>( 1.0F /units.X(), 1.0F/units.Y(), 1.0F/units.Z() ) );
    Matrix<float4> mVolumeToWorld;
    mVolumeToWorld.Scale( Vector<float4>( units.X(), units.Y(), units.Z() ) );
    float4 fWorldToVolumeScale( Max( 1.0F /units.X(), 1.0F/units.Y(), 1.0F/units.Z() ) );

    const uint4 uSliceWidth( m_pRenderBuffer->GetColorImage().GetDim().m_x );
    const uint4 uSliceHeight( m_pRenderBuffer->GetColorImage().GetDim().m_y );

    // some items used frequently inside the loop
    Point<uint2> vPixelPosition( 0, 0, 0 );
      uint1 * pImage( reinterpret_cast< uint1 * >( m_pRenderBuffer->GetColorImage().GetDataPtr() ) );
    const float * pfRGBA( args.m_pfRGBA );
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
        const float4 fSampleDistance( lineDirectionVector.GetLength() / args.m_pWinRect->GetViewportWidth() );
        RaySegment raySegmentIntensity( leftPointVol, rightPointVol );      
        Vector4Df sampleIncVecIntensity( raySegmentIntensity.GetDirection().GetNormalizedXYZ() * fSampleDistance );

        iterRayIntensity.SetSampleIncrementVector( sampleIncVecIntensity );
        iterRayIntensity.SetRaySegment( raySegmentIntensity );

        // loop over pixels in scanline
        uint1 * pImageLine( pImage + 4 * ( m_pRenderBuffer->GetColorImage().GetDim().m_x * y ) );
        uint1 uRed  ( 0 );
        uint1 uGreen( 0 );
        uint1 uBlue ( 0 );
        for ( int4 x=0; x<uSliceWidth; ++x, ++iterRayIntensity )
        {
          // see if we are in a valid region
          if ( y < uSliceHeight )
          {
            const uint4 uIntensity( min(m_iMaxVoxelValue, (iterRayIntensity.GetSampleInt()>0 ? iterRayIntensity.GetSampleInt() : 0)));

            // apply the transfer function
            const bool bShowColor( args.m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR ) );
            if ( bShowColor )
            {
              // color
              const float4 * pColorTableEntry(pfRGBA + 4 * uIntensity);
              uRed   = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
              uGreen = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
              uBlue  = (uint2)(*pColorTableEntry++) >> args.m_iShiftBitsFor8bitAlpha;
            }
            else
            {
              // grayscale
              uRed = uGreen = uBlue = (uint2)pfRGBA[4*uIntensity + 3] >> args.m_iShiftBitsFor8bitAlpha;
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
            uint1 * pImageLine( pImage + 4 * ( m_pRenderBuffer->GetColorImage().GetDim().m_x * y ) );
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
    LogWrn( "Rendering failed", "vxCurvetracer", "RenderSingleLayer" );
  }

  return 1;

} // RenderSingleLayer()

// $Log: vxCurvetracer.C,v $
// Revision 1.7  2007/08/23 19:10:41  romy
// reverted the size change back. //TODO: DO it after the alpha
//
// Revision 1.5  2007/03/02 17:40:47  geconomos
// code review preparation
//
// Revision 1.4  2007/02/22 18:27:15  geconomos
// code cleanup
//
// Revision 1.3  2006/09/25 13:24:53  geconomos
// commented out unneeded  log debugging
//
// Revision 1.2  2006/09/21 14:57:31  geconomos
// removed unused vxComponentArray
//
// Revision 1.1  2006/09/14 20:06:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxCurvetracer.C,v 1.7 2007/08/23 19:10:41 romy Exp $
// $Id: vxCurvetracer.C,v 1.7 2007/08/23 19:10:41 romy Exp $
