// $Id: vxRaytracer.C,v 1.26.2.2 2008/09/11 20:09:01 dongqing Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille  (mailto:frank@viatronix.com)

#include "StdAfx.h"
#include "vxRaytracer.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxUtils.h"
#include "vxEnvironment.h"
#include "vxMathUtils.h"
#include "vxRayGenerator.h"


#define FILE_REVISION "$Revision: 1.26.2.2 $"
#define ALIGN16 __declspec(align(16)) 
#define SLICE_SHADOWS_ENABLED 0


// global const variables
const bool   Global_bShowAdaptiveSamplingPattern  = false;
const bool   Global_bShowSpaceLeapPattern         = false;
const bool   Global_bShowThreadingPattern         = false;
const bool   Global_bDebug                        = (Global_bShowAdaptiveSamplingPattern || Global_bShowSpaceLeapPattern);
const int4   Global_iMaxDifferenceColor  = 8;
const float4 Global_fInterSampleDistance = 0.5;
const float4 Global_fSliceShadowOpacity = 0.5F;
const int4   Global_iSldBlockWidth = 16;
float4 m_fQualitySampleDistanceScaleResolutionDependent = 1.0F;

const PixelRGBA< uint1 > Global_debugColors3D[8] = { 
  PixelRGBA< uint1 >( 255,    0,     0,   255 ), 
  PixelRGBA< uint1 >(   0,  255,     0,   255 ), 
  PixelRGBA< uint1 >(   0,    0,   255,   255 ), 
  PixelRGBA< uint1 >( 255,  255,     0,   255 ),  
  PixelRGBA< uint1 >(   0,  255,   255,   255 ),  
  PixelRGBA< uint1 >( 255,  165,     0,   255 ),  
  PixelRGBA< uint1 >( 160,   32,   240,   255 ), 
  PixelRGBA< uint1 >( 255,  255,   255,   255 )  
};    

// Enumerations
enum SamplingInfoEnum
{
  /// Space leap info enums
  eRayStart,             //0    // red    
  eMultipleEmptyBlocks,  //1    // green       
  eSingleEmptyBlock,     //2    // blue (dark) 
  eSingleEmptySmallBlock,//3    // orange      
  eInvisible,            //4    // pink        
  eInvisibleBacktrace,   //5    // yellow      
  eZeroOpacity,          //6    // light blue  
  eZeroOpacityBacktrace, //7     
  eColoredSample,       
  eGradientLocation,    
  eDummy1,    
  eDummy2,    
  eRayEnd,    

  /// adaptive sampling info enums
  eFirstRayInTile,          //0
  eAdditionalRay,           //1
  eInterpolatedColor,       //2
  eHeterogeniousRay,        //3
  eAdditionalRayPassTwo,    //4
  eInterpolatedColorPassTwo,//5
  eHeterogeniousRayPassTwo, //6

  eNowDrawImage
}; // enum SamplingInfoEnum




/**
 * Constructor
 */
vxRaytracer::vxRaytracer() : m_pRenderBuffer( new vxRenderBuffer() )
 {
  if( Global_bDebug )
  {
    m_iSamplingPatternIndex = 0;
    m_evSamplingInfo.resize(2000);
    m_fvSamplingPos.resize(m_evSamplingInfo.size());

    for (int4 k(0); k<m_evSamplingInfo.size(); ++k)
    {
      m_evSamplingInfo[k] = 0;
    }
  }

	m_iMaxVoxelValue = 4094;
} // vxRaytracer


/**
 * Destructor
 */
vxRaytracer::~vxRaytracer()
{
  if( m_pRenderBuffer != NULL )
    delete m_pRenderBuffer;

  m_pRenderBuffer = NULL;
} // ~vxRaytracer()


/**
 * Pre-initialization routine.
 */
void vxRaytracer::PreRender()
{
  m_pRenderBuffer->Clear( m_settings.GetBackgroundColor() );
  m_pRenderBuffer->GetDepthImageWC().SetClearColor( (std::numeric_limits<float>::max)() );
  m_pRenderBuffer->GetDepthImageWC().Clear();
  m_pRenderBuffer->GetDepthImageVC().SetClearColor( (std::numeric_limits<float>::max)() );
  m_pRenderBuffer->GetDepthImageVC().Clear();
  
  InitializeRenderArguments();
} // PreRender()


/**
 * Renders and image into the buffer
 */
void vxRaytracer::Render()
{
  PreRender();
  
  BoundingRect< uint2 > window( m_settings.GetRenderSize().m_x, m_settings.GetRenderSize().m_y );
  
  RenderWindow( window, 0 );
} // Render()


/**
 * Aborts rendering
 */
void vxRaytracer::Abort()
{
  m_bCancelRendering = true;
} // Abort()


/**
 * Fills in all render arguments that are common amoung all threads.
 */
void vxRaytracer::InitializeRenderArguments()
{
  try
  {
    // Get local const of environment elements
    const Vector3Df fVoxelUnits( m_settings.GetIntensityVolume().GetHeader().GetVoxelUnits() );
    const Vector3Df fVoxelUnitsReciprocal(1.0/fVoxelUnits.X(),1.0/fVoxelUnits.Y(),1.0/fVoxelUnits.Z());
    const Vector3Df fViewReferenceWC(m_settings.GetCamera().GetViewReference().X(),m_settings.GetCamera().GetViewReference().Y(),m_settings.GetCamera().GetViewReference().Z());
    const Vector3Df fViewDirectionWC(m_settings.GetCamera().GetViewDirection().X(),m_settings.GetCamera().GetViewDirection().Y(),m_settings.GetCamera().GetViewDirection().Z());
    const Vector3Df fViewUpWC(m_settings.GetCamera().GetViewUp().X(),m_settings.GetCamera().GetViewUp().Y(),m_settings.GetCamera().GetViewUp().Z());
    const Vector3Df fViewDownWC(-fViewUpWC);
    const Vector3Df fViewEyeWC(m_settings.GetCamera().GetEye().X(),m_settings.GetCamera().GetEye().Y(),m_settings.GetCamera().GetEye().Z());
    const Vector3Df fViewRightWC(fViewDirectionWC.GetNormalized().Cross(fViewUpWC));
    const Vector3Df fBoxOfInterestMin(Vector3D<float4>(m_settings.GetBoxOfInterest().GetMinX(), m_settings.GetBoxOfInterest().GetMinY(), m_settings.GetBoxOfInterest().GetMinZ()));
    const Vector3Df fBoxOfInterestMax(Vector3D<float4>(ceil(m_settings.GetBoxOfInterest().GetMaxX()), ceil(m_settings.GetBoxOfInterest().GetMaxY()), ceil(m_settings.GetBoxOfInterest().GetMaxZ())));

    std::string sModality("");
    sModality = m_settings.GetIntensityVolume().GetHeader().GetDicomFieldValue("0008", "0060", sModality);
    if( sModality != "CT" )
	  {
	    int4 iHeaderVersion(0);
	    m_settings.GetIntensityVolume().GetHeader().GetVariableVolumeHeader().GetHdrVar("version", iHeaderVersion);

	    if( iHeaderVersion >= 5 )
	    {
        m_settings.GetIntensityVolume().GetHeader().GetVariableVolumeHeader().GetHdrVar("maxValueRange", m_iMaxVoxelValue);
		    if( m_iMaxVoxelValue > 4095 )
				  m_iMaxVoxelValue = 65534;
		  }
		  else
		  {
		    m_iMaxVoxelValue = 65534;
		  }
	  }  // non CT modality	

    // initialize the ray generator
    m_arguments.pRayGenerator = &m_settings.GetRayGenerator();
    m_arguments.pRayGenerator->Initialize( &m_settings );
    
    m_arguments.fVoxelUnits           = fVoxelUnits;
    m_arguments.fVoxelUnitsReciprocal = fVoxelUnitsReciprocal;
    m_arguments.fInterSampleDistance  = Global_fInterSampleDistance * m_fQualitySampleDistanceScaleResolutionDependent;

    // Set all the parameterd which are necessary for rendering
    m_arguments.fCropBoxVC       = m_settings.GetVisibilityBox(); 
    m_arguments.fBoxOfInterest   = m_settings.GetBoxOfInterest(); 
    m_arguments.fViewDirectionVC = Vector3D<float4>(fViewDirectionWC.X() * fVoxelUnitsReciprocal.X(), fViewDirectionWC.Y() * fVoxelUnitsReciprocal.Y(), fViewDirectionWC.Z() * fVoxelUnitsReciprocal.Z());
    Vector3D<float4> fViewUpVC   = Vector3D<float4>(fViewUpWC.X() * fVoxelUnitsReciprocal.X(), fViewUpWC.Y() * fVoxelUnitsReciprocal.Y(), fViewUpWC.Z() * fVoxelUnitsReciprocal.Z());
    m_arguments.eyePositionWorld = m_settings.GetCamera().GetEye();
    m_arguments.viewDirectionWC  = m_settings.GetCamera().GetViewDirection();

    /// initial light direction
    Vector3D<float4> fLight1DirVC =  fViewUpVC.GetNormalized();
    m_arguments.fLight2DirVC      = -m_arguments.fViewDirectionVC.GetNormalized();

    /// moveable light adjustment
    float4 fAngleX(M_PI_2 * (0.5 -  m_settings.GetRenderingMode().GetRelativeMousePosition().first));
    float4 fAngleY(M_PI_2 * (0.0 - m_settings.GetRenderingMode().GetRelativeMousePosition().second));
    const Normal<float4> fAxisX(fViewUpWC);
    const Normal<float4> fAxisY(fViewUpWC.Cross(fViewDirectionWC));
    Matrix<float4> fRotateMatrix;
    fRotateMatrix.Rotate(fAxisY, -fAngleY);
    fRotateMatrix.Rotate(fAxisX,  fAngleX);
    m_arguments.fLight2DirVC = fRotateMatrix * m_arguments.fLight2DirVC;

    // set vectors dependent on light direction
    m_arguments.fHalfwayLight1VC = (-m_arguments.fViewDirectionVC.GetNormalized() + fLight1DirVC).GetNormalized();
    m_arguments.fHalfwayLight1VC = fRotateMatrix * m_arguments.fHalfwayLight1VC;

    /// anisotropic correction gradient scaling factors
    m_arguments.fGradientScaleY = m_settings.GetIntensityVolume().GetHeader().GetVoxelUnits().X() / m_settings.GetIntensityVolume().GetHeader().GetVoxelUnits().Y();
    m_arguments.fGradientScaleZ = m_settings.GetIntensityVolume().GetHeader().GetVoxelUnits().X() / m_settings.GetIntensityVolume().GetHeader().GetVoxelUnits().Z();

    /// image info
    m_arguments.pImageBuffer = reinterpret_cast< uint1 * >( m_pRenderBuffer->GetColorImage().GetDataPtr() );
    m_arguments.pDepthBufferVC = static_cast< float4* >( m_pRenderBuffer->GetDepthImageVC().GetDataPtr() );
    m_arguments.iImageWidth  = m_pRenderBuffer->GetColorImage().GetDim().X();
    m_arguments.iImageHeight = m_pRenderBuffer->GetColorImage().GetDim().Y() - 2;

    if( m_pRenderBuffer->GetDepthBufferFlags() & vxRenderBuffer::DEPTH_IMAGE_WC )
      m_arguments.pDepthBufferWC = static_cast< float4* >( m_pRenderBuffer->GetDepthImageWC().GetDataPtr() );
    else
      m_arguments.pDepthBufferWC = NULL;

    /// rendering mode related
    m_arguments.bGrayscale = ! m_settings.GetRenderingMode().IsEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR );
    m_arguments.bIsMipMode  = m_settings.GetRenderingMode().GetRenderingAlgorithm() ==  vxRenderingMode::MIP;
    m_arguments.bIsXRayMode = m_settings.GetRenderingMode().GetRenderingAlgorithm() ==  vxRenderingMode::XRAY;
    m_arguments.bIsVRMode   = m_arguments.bIsMipMode == false && m_arguments.bIsXRayMode == false;
    m_arguments.bShowLabels = m_settings.GetRenderingMode().IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR );
    m_arguments.bClipSliceShadow = m_settings.GetClipSliceShadow();

    // segmentation intensity range
    m_arguments.bIsShowIntensityRange = m_settings.GetShowIntensityRange();
    m_arguments.iShowIntensityRangeLower = m_settings.GetIntensityRangeLow();
    m_arguments.iShowIntensityRangeUpper = m_settings.GetIntensityRangeHigh();


    // components
    m_arguments.pvComponents = &m_settings.GetComponents(); 

    m_arguments.bIsLabelsOnTopMode = false; 

    // background color
    m_arguments.uBackgroundColor = m_settings.GetBackgroundColor();
    m_arguments.fBackgroundColor = PixelRGBAf(m_arguments.uBackgroundColor.R(), 
                                                          m_arguments.uBackgroundColor.G(), 
                                                          m_arguments.uBackgroundColor.B(), 
                                                          m_arguments.uBackgroundColor.A());

    // color tables
    m_arguments.iShiftTo8Bit = m_settings.GetNumberOfShiftBitsFor8bitAlpha();
    m_arguments.pfColormap2d8bitPreIntegrated = const_cast< float4 *>( m_settings.GetColormap2d8bitPreIntegrated() );
 
    // choose a foreground color (black or white only) that is most dissimilar to the background color.
    // this is only used in x-ray rendering.
    if ((m_arguments.fBackgroundColor.R() + 
         m_arguments.fBackgroundColor.G() +
         m_arguments.fBackgroundColor.B() ) / 3 > 128)
    {
      m_arguments.fForegroundColor = PixelRGBAf(0, 0, 0, m_arguments.fBackgroundColor.A());
    }
    else
    {
      m_arguments.fForegroundColor = PixelRGBAf(255, 255, 255, m_arguments.fBackgroundColor.A());
    }
    
    m_bCancelRendering = false;
  }
  catch (...)
  {
    throw ex::VException(LogRec("SetCommonRenderArguments failed.", "vxRaytracer", "SetCommonRenderArguments"));
  }
} // SetCommonRenderArguments()


/**
 * Renders the actual image, but constraint to the 
 * area defined by the window parameter.
 * @param window defines the windows to render
 * @param uThreadID thread ID
 * @return 1 in case of success
 */
void vxRaytracer::RenderWindow( const BoundingRect<uint2> & window, const uint1 & uThreadID )
{
  try
  {     
    // make a list of arguments that is local to the current thread  and partially updated for each ray
    ALIGN16 vxRaytracerRenderArgs args;
    args = m_arguments;
    args.uThreadID = uThreadID;
    args.debugColor = Global_debugColors3D[ uThreadID ];
    if( args.iImageWidth == 0 && args.iImageHeight == 0 )
      return;

    vxBlockVolumeRayIterator<uint2> rayIterBlk( m_settings.GetIntensityVolume() );
    vxBlockVolumeRayIterator<uint2> labelRayIter( m_settings.GetLabelVolume() );
    vxBlockVolumeRayIterator<uint2> shadowRayIter( m_settings.GetIntensityVolume() );
    vxBlockVolumeIterator<uint2>    iterLabelShadow( m_settings.GetLabelVolume());
    vxBlockVolumeIterator<int1>     SpaceLeapInfoVolIter(m_settings.GetSpaceLeapVolume()  );
    args.pRayIterLabel      = &labelRayIter;
    args.pRayIterIntensity  = &rayIterBlk;
    args.pVolIterSLD        = &SpaceLeapInfoVolIter;   
    args.pRayIterShadow     = &shadowRayIter;
    args.pIterLabelShadow   = &iterLabelShadow;

    const uint4 byte_per_pixel = 4;
    const uint4 flc_xStepPixel = byte_per_pixel;
    const uint4 flc_yStepPixel = m_pRenderBuffer->GetColorImage().GetDim().X() * byte_per_pixel;
    
    const uint4 flc_xStepDepth = 1 ;
    const uint4 flc_yStepDepth = flc_xStepDepth * m_pRenderBuffer->GetDepthImageWC().GetDim().m_x;

    const int4 iBlockSize = 8;

    //
    // Cast rays scanline by scanline.
    // Cast ray normally for every eighth pixel, but accelerate by casting from nearest
    // (nearest using depth information) of left and right fully cast rays.
    // Sequence along a ray is:
    //   1 - - - - - - - -
    //   1 - - - - - - 2 -
    //   1 3 4 5 6 7 8 2 -
    //

    float4 fFeelerRayDepth = 0;
    float4 fPreviousFeelerRayDepth = 0;

    // create buckets equal to the block size to track the minimum depth between two feeler rays
    const int4 iNumberOfScanlineBuckets 
      = static_cast< int >( ( static_cast< float4 >( window.GetMaxX() - window.GetMinX() + 1 ) / iBlockSize ) + 0.5F ) + 1;
    
    std::vector< float4 > fPreviousDepths( iNumberOfScanlineBuckets, (std::numeric_limits<float4>::max)() );

    // tracks if the previous feeler ray hit anything
    bool bPreviousFeelerRayHitSomething = false;
    
    // foreach scanline
    const int4 iMaxScanLines = window.GetMaxY() - window.GetMinY();
    for( int y=0; y <= iMaxScanLines; y++ )
    {
      // is this the top most scan line?
      const bool bTopScanline = ( y == 0 );

      // foreach column incrementing by block size
      for( int x=0; x<=window.GetMaxX() - window.GetMinX(); x+=iBlockSize )
      {
        // check if rendering has been canceled       
        if( m_bCancelRendering )
          return;

        // tracks the minimum depth for this scanline
        float4 fMinBucketDepth = (std::numeric_limits<float4>::max)();
 
        // send feeler ray
        args.u = x + window.GetMinX();
        args.v = y + window.GetMinY();
        args.iPixelOffset = args.u * flc_xStepPixel + args.v * flc_yStepPixel;
        args.iDepthOffset = args.u * flc_xStepDepth + args.v * flc_yStepDepth;
        args.bUseNeighborDepth = false;
        args.fDepthToSurfaceVC = (std::numeric_limits<float4>::max)();
        args.fDepthToSurfaceWC = (std::numeric_limits<float4>::max)();
        CastOneRay( args );


        // check if the sampled pixel color is the same as the background color
        bool bFeelerRayHitSomething = args.fDepthToSurfaceVC != (std::numeric_limits<float4>::max)();

        /*if( bFeelerRayHitSomething )
        {
          *( args.pImageBuffer + args.iPixelOffset + 0 ) = 255;
          *( args.pImageBuffer + args.iPixelOffset + 1 ) = 0;
          *( args.pImageBuffer + args.iPixelOffset + 2 ) = 0;
        }
        else
        {
          *( args.pImageBuffer + args.iPixelOffset + 0 ) = 0;
          *( args.pImageBuffer + args.iPixelOffset + 1 ) = 0;
          *( args.pImageBuffer + args.iPixelOffset + 2 ) = 255;
        }*/

        // store the depth of the feeler ray and initialize the minimum bucket depth
        fFeelerRayDepth = fMinBucketDepth = args.fDepthToSurfaceVC;
        
        // grab the value of the previous scanline bucket and check if we hit something
        const int4 iBucketIndex = x / iBlockSize;
        const float4 fPreviousBucketDepth = fPreviousDepths[ iBucketIndex ];
        const bool bPreviousScanlineHitSomething = fPreviousBucketDepth != (std::numeric_limits<float4>::max)();
        // if we hit anything to the left, right or previous scanline fill in the pixels
        // (don't do this the first time where x==0)
        const bool bHitAnything = bFeelerRayHitSomething || bPreviousFeelerRayHitSomething || bPreviousScanlineHitSomething;
        for( int xx=x-iBlockSize+1; x>0 && xx<x && ( bHitAnything || bTopScanline ); xx++ )
        {
          // cast ray using the miniumum depth of the left , right or previous scanline bucket
          args.u = xx + window.GetMinX();
          args.v = y + window.GetMinY();
          args.iPixelOffset = args.u * flc_xStepPixel + args.v * flc_yStepPixel;
          args.iDepthOffset = args.u * flc_xStepDepth + args.v * flc_yStepDepth;
          if( bTopScanline )
          {
            args.bUseNeighborDepth = false;          
          }
          else
          {
            args.fNeighborDepth = Min( Min( fPreviousFeelerRayDepth, fFeelerRayDepth ), fPreviousDepths[iBucketIndex] );
            args.bUseNeighborDepth = true;          
          }
          CastOneRay( args );

  
          // store minimum depth
          fMinBucketDepth = Min( fMinBucketDepth, args.fDepthToSurfaceVC );

          const float4 fSuprisingDepthChange = 10;
          const bool bDepthSurprise = 
            ( args.fDepthToSurfaceVC < args.fNeighborDepth && args.fNeighborDepth - args.fDepthToSurfaceVC > fSuprisingDepthChange );

   
          // if there was a depth suprise (depth discontinuity)
          // Go back through the scanlines in the reverse order to make see if there were any discontinuities
          // in the depth image. As we went up the image last time, we were able to anticipate increasing depth
          // but not decreasing depth (getting much closer).
          if ( bDepthSurprise )
          {
            // re-cast the ray from the very beginning
            args.bUseNeighborDepth = false;
            CastOneRay( args );

            // loop over pixels in -y direction casting each ray fully and
            // stopping when we find that we have matched a previous depth
            bool bExpectedDepth = false;
            for( int i=1; bExpectedDepth == false && y - i > 0; i++ )
            {
              // check if rendering has been canceled 
               if( m_bCancelRendering )
                 return;
        
              // cast full ray
              args.v = y - i + window.GetMinY();
              args.iPixelOffset = args.u * flc_xStepPixel + args.v * flc_yStepPixel;
              args.iDepthOffset = args.u * flc_xStepDepth + args.v * flc_yStepDepth;
              args.bUseNeighborDepth = false;
              CastOneRay( args );

              // check if we are withing a resonable range from the previous depth
              const float4 fOldDepth = args.pDepthBufferVC[ args.iDepthOffset ];
              const float4 fReasonableDepthChange = 4;
              bExpectedDepth = ( args.fDepthToSurfaceVC > fOldDepth || fOldDepth - args.fDepthToSurfaceVC < fReasonableDepthChange );
            }
          } // depth surprise
          
          // check if rendering has been canceled 
          if( m_bCancelRendering )
            return;
        
        } // foreach column in block
        
        // prepare for next group
        fPreviousDepths[ iBucketIndex ] = fMinBucketDepth;
        fPreviousFeelerRayDepth = fFeelerRayDepth;
        bPreviousFeelerRayHitSomething = bFeelerRayHitSomething;
      
      } // foreach column incrementing by block size
    
    } // for each scanline
  } // try
  catch (ex::VException & error)
  {
    LogRec(error.GetLogRecord().GetTextMessage(), "vxRaytracer", "RenderWindow" );
    throw;
  }
  catch (... )
  {
    throw ex::DataCorruptionException(LogRec("unhandled exception", "vxRaytracer", "RenderWindow" ) );
  }
} // RenderWindow()


/**
 * Casts a single ray from the image plane. Test if ray will hit the volume, 
 * if yes it will sample a ray otherwise the corresponding pixel in the final 
 * image is set to black.
 * @param args     the ray data structure
 */
void vxRaytracer::CastOneRay(vxRaytracerRenderArgs & args)
{

  // Generate the maximum possible ray segment by clipping with the view volume
  // This can be done by using OpenGL and getting the ray intersection with the
  // front and back clipping planes, or by a direct mathematical computation
  RaySegment raySegmentWC = args.pRayGenerator->GenerateRay( args.u, args.v );

  // convert the world-coordinate ray segment into volume coordinates
  Vector4d<float4> fRayBgnVC( raySegmentWC.GetStartPoint().GetScaledUnIso( Vector4d<float4>( args.fVoxelUnitsReciprocal, 1 ) ) );
  Vector4d<float4> fRayEndVC( raySegmentWC.GetEndPoint  ().GetScaledUnIso( Vector4d<float4>( args.fVoxelUnitsReciprocal, 1 ) ) );

  // correct the volume-coordinate length
  fRayBgnVC.T() = 0.0F;  
  fRayEndVC.T() = (fRayEndVC - fRayBgnVC).GetLengthXYZ();

  Vector4d<float4> fRayDirVC((fRayEndVC - fRayBgnVC).GetNormalizedXYZ());

  args.fRayIntersectBoundingBoxBgnVC = fRayBgnVC; 
  if (Global_bShowSpaceLeapPattern)
  {
    DrawSpaceLeapPatternLine(args, eRayStart);
  }

  // further refine the ray segment by intersecting with the bounding box of the visible volume
  bool bIntersection(vxUtils::HitBoundingBox4d(args.fCropBoxVC, fRayBgnVC, fRayDirVC, args.fRayIntersectBoundingBoxBgnVC));

  if (bIntersection == true)
  {
    // compute ray exit point
    vxUtils::HitBoundingBox4d(args.fCropBoxVC, fRayEndVC, -fRayDirVC, args.fRayIntersectBoundingBoxEndVC);

    // set the exit value of T to the matching OpenGL depth buffer value
    args.fRayIntersectBoundingBoxEndVC.T() = fRayEndVC.T() - args.fRayIntersectBoundingBoxEndVC.T();

    // move from outside bounding box into valid data area (precision issue)
    args.fRayIntersectBoundingBoxBgnVC += fRayDirVC * 0.01F;

    // set iterators to ray start location
    RaySegment raySegment(args.fRayIntersectBoundingBoxBgnVC, args.fRayIntersectBoundingBoxEndVC); 
    if (raySegment.GetEndPoint().T() <= raySegment.GetStartPoint().T())
    {
      //LogWrn("fixing inverted T on ray setup", "vxRaytracer", "CastOneRay");
      raySegment.GetEndPoint().T() = raySegment.GetStartPoint().T()+0.1F;
    }
    Vector4Df raySampleIncVec(raySegment.GetDirection().GetNormalizedXYZ() * args.fInterSampleDistance);
    args.pRayIterIntensity->SetRaySegment(raySegment);
    args.pRayIterLabel    ->SetRaySegment(raySegment);
    args.pRayIterShadow   ->SetRaySegment(raySegment);
    args.pRayIterIntensity->SetSampleIncrementVector(raySampleIncVec);
    args.pRayIterLabel    ->SetSampleIncrementVector(raySampleIncVec);
    args.pRayIterShadow   ->SetSampleIncrementVector(raySampleIncVec);

    args.fLight2DirVC.X() = fRayDirVC.X();
    args.fLight2DirVC.Y() = fRayDirVC.Y();
    args.fLight2DirVC.Z() = fRayDirVC.Z();

    args.bContinueRay = true;

    args.iSample    = 0;
    args.iPrevIndex = 0;

    if (m_settings.GetJitteringEnabled() == true)
    {
      float4 fJitterOffset(m_settings.GetJitter().GetValue(args.u, args.v ) );
      args.pRayIterIntensity->SetPos(args.pRayIterIntensity->GetPosFloat() + raySampleIncVec * fJitterOffset );
      args.pRayIterLabel    ->SetPos(args.pRayIterLabel    ->GetPosFloat() + raySampleIncVec * fJitterOffset );
    }

    // compute ray-plane intersection
    args.bSliceShadowsEnabled = m_settings.GetSliceShadowsEnabled();
    if ( args.bSliceShadowsEnabled && args.bClipSliceShadow )
    {
      // compute the intersection in world coordinates
      float4 fDistance( 0 );
      Point3Df rayBoxBgnWC( Point3Df( args.fRayIntersectBoundingBoxBgnVC ).GetScaledAnIso( Triplef( args.fVoxelUnits ) ) );
      Point3Df rayBoxEndWC( Point3Df( args.fRayIntersectBoundingBoxEndVC ).GetScaledAnIso( Triplef( args.fVoxelUnits ) ) );
      if ( rayBoxBgnWC != rayBoxEndWC )
      {
        Vector3Df rayBoxVectorWC( rayBoxEndWC - rayBoxBgnWC );
        bool bValidIntersection = 
            m_settings.GetSliceShadowPlane().GetDistanceToIntersection( rayBoxBgnWC, Normal3Df( rayBoxVectorWC ), fDistance );
        if( bValidIntersection )
        {
          args.fShadowPlaneDepth = Interpolate( 0.0F, fDistance, float4(rayBoxVectorWC.GetLength() ),
                                              args.fRayIntersectBoundingBoxBgnVC.T(), args.fRayIntersectBoundingBoxEndVC.T() );
        }
        else
        {
          args.bSliceShadowsEnabled = false;
        }
      }
      
      // check if the plane intersection is inside the bounding box
      // (add 0.1 to ensure that float comparison always keeps the slice shadow visible, even on the first/last slice)
      const bool bInFrontOrBack( ( args.fShadowPlaneDepth + 0.1F ) < args.fRayIntersectBoundingBoxBgnVC.T() );
      const bool bInvisible( args.fRayIntersectBoundingBoxEndVC.T() < ( args.fShadowPlaneDepth-0.1F ) );
      if ( bInFrontOrBack || bInvisible )
      {
        args.fShadowPlaneDepth = (std::numeric_limits<float>::max)();
      }
      else if ( ( args.fShadowPlaneDepth - 0.2F ) < args.fRayIntersectBoundingBoxBgnVC.T() )
      {
        args.fShadowPlaneDepth = args.fRayIntersectBoundingBoxBgnVC.T() + 0.2F;
      }
      else if ( ( args.fShadowPlaneDepth + 0.2F ) > args.fRayIntersectBoundingBoxEndVC.T() )
      {
        args.fShadowPlaneDepth = args.fRayIntersectBoundingBoxEndVC.T() - 0.2F;
      }

      // handle clipping away of the volume due to the slice shadow
      if ( args.bClipSliceShadow )
      {
        if ( args.fShadowPlaneDepth != (std::numeric_limits<float>::max)() )
        {
          // start sampling directly at clip plane
          const float4 fDistanceToLeap( fabs( args.fShadowPlaneDepth - args.pRayIterIntensity->GetPosFloat().T() ) );
          const float4 fDistancePerLeap( args.pRayIterIntensity->GetSldDeltaVector().T() );
          const float4 fLeaps( fDistanceToLeap / fDistancePerLeap );
          args.pRayIterIntensity->SpaceLeap( fLeaps );
          args.pRayIterLabel    ->SpaceLeap( fLeaps );
        }

        else if ( bInvisible )
        {
          // this part should be clipped away (invisible)
          args.bContinueRay = false;
        }

        else if ( bInFrontOrBack )
        {
          // if the eyepoint and the ray-box entrance are on the same side of the clip plane
          // then make the ray invisible

          // compute the eye point (far away if orthographic projection)
          Vector3Df eyePosWC( Vector3Df( m_settings.GetCamera().GetEye() ) );
					if ( m_settings.GetCamera().GetProjectionType() == vxCamera::ORTHOGRAPHIC )
          {
            const float4 fExtremelyFar( 1e5 );
            eyePosWC -= Vector3Df( m_settings.GetCamera().GetViewDirection() ) * fExtremelyFar;
          }

          // oblique slice shadow
          Point3Df rayBoxBgnWC( Point3Df( args.fRayIntersectBoundingBoxBgnVC ).GetScaledAnIso( Triplef( args.fVoxelUnits ) ) );
          const bool bEyeGreaterPlane( m_settings.GetSliceShadowPlane().GetDistance( Point3Df( eyePosWC ) ) >= 0 );
          const bool bRayGreaterPlane( m_settings.GetSliceShadowPlane().GetDistance( rayBoxBgnWC ) >= 0 );
          if ( bEyeGreaterPlane == bRayGreaterPlane )
          {
            args.bContinueRay = false;
          }
        }
      }
    }
    
    SampleSingleRay(args);

    // store result
    if (!args.bIsLabelsOnTopMode || args.fPreMultAccum.A() > 0.1)
    {

      uint1 * pImageDataPtrCurrent(args.pImageBuffer + args.iPixelOffset);
      if( Global_bShowThreadingPattern )
      {  
        *(pImageDataPtrCurrent++) = args.debugColor.R();
        *(pImageDataPtrCurrent++) = args.debugColor.G();
        *(pImageDataPtrCurrent++) = args.debugColor.B();
        *(pImageDataPtrCurrent  ) = args.debugColor.A();
      }
      else
      {
        *(pImageDataPtrCurrent++) = args.iDisplayColor.R();
        *(pImageDataPtrCurrent++) = args.iDisplayColor.G();
        *(pImageDataPtrCurrent++) = args.iDisplayColor.B();
        *(pImageDataPtrCurrent  ) = args.iDisplayColor.A();
      }

      // code purely for debugging purposes
      ///////////////////////////////////////
      //*(pImageDataPtrCurrent++) = 255 * (1-exp(-args.fDepthToSurfaceVC/500));
      //*(pImageDataPtrCurrent++) = 255 * (1-exp(-args.fDepthToSurfaceVC/2000));
      //*(pImageDataPtrCurrent++) = 255 * (1-exp(-args.fDepthToSurfaceVC/8000));

      //*(pImageDataPtrCurrent++) = 128 * (1-raySegment.GetDirection().GetNormalizedXYZ().X());  
      //*(pImageDataPtrCurrent++) = 128 * (1-raySegment.GetDirection().GetNormalizedXYZ().Y());
      //*(pImageDataPtrCurrent++) = 128 * (1-raySegment.GetDirection().GetNormalizedXYZ().Z());

      //*(pImageDataPtrCurrent++) = 128 * (1-2*raySampleIncVec.X());  
      //*(pImageDataPtrCurrent++) = 128 * (1-2*raySampleIncVec.Y());
      //*(pImageDataPtrCurrent++) = 128 * (1-2*raySampleIncVec.Z());

      if( args.pDepthBufferWC != NULL )
        *(args.pDepthBufferWC + args.iDepthOffset) = args.fDepthToSurfaceWC;
      
      *(args.pDepthBufferVC + args.iDepthOffset) = args.fDepthToSurfaceVC;
    }
  }
  else
  {
    uint1 * pImageDataPtrCurrent(args.pImageBuffer + args.iPixelOffset);
    if( Global_bShowThreadingPattern )
    {  
      *(pImageDataPtrCurrent++) = args.debugColor.R();
      *(pImageDataPtrCurrent++) = args.debugColor.G();
      *(pImageDataPtrCurrent++) = args.debugColor.B();
      *(pImageDataPtrCurrent  ) = args.debugColor.A();
    }
    else
    {
      *(pImageDataPtrCurrent++) = args.uBackgroundColor.R();
      *(pImageDataPtrCurrent++) = args.uBackgroundColor.G();
      *(pImageDataPtrCurrent++) = args.uBackgroundColor.B();
      *(pImageDataPtrCurrent  ) = args.uBackgroundColor.A();
    }
  
    if( args.pDepthBufferWC != NULL )
      *(args.pDepthBufferWC + args.iDepthOffset) = (std::numeric_limits<float>::max)();
    
    *(args.pDepthBufferVC + args.iDepthOffset) = (std::numeric_limits<float>::max)();
  }
  if (Global_bShowSpaceLeapPattern)
  {
    DrawSpaceLeapPatternLine(args, eNowDrawImage);
  }
} // CastOneRay()


/**
 * Space Leap
 * @param args the ray data structure
 */
void vxRaytracer::SpaceLeap(vxRaytracerRenderArgs & args)
{
  //// traverse empty space
  RaySpaceLeapLargeBlocks(args);
  RaySkipDensityInvisible(args);
  RaySkipAlphaInvisible(args);
} // SpaceLeap()


/**
 * Space leaps through all 16^3 blocks that have at least one layer of empty 16^3 
 * blocks around them. After that it jumps top the end of the border 16^3 blocks.
 * @param args ray data structure
 */
inline void vxRaytracer::RaySpaceLeapLargeBlocks(vxRaytracerRenderArgs & args)
{
  // space leap through empty blocks
  int4 iSpaceLeapInfo;
  args.pVolIterSLD->SetPos(args.pRayIterLabel->GetPosInt() / Global_iSldBlockWidth);
  while (0 < (iSpaceLeapInfo = args.pVolIterSLD->GetVoxel()) && args.pRayIterLabel->GetPosFloat().T() < args.fRayIntersectBoundingBoxEndVC.T() )
  {
    args.bFirstSample = false;
    float4 fSpaceLeapDist;
    if (1 < iSpaceLeapInfo)
    {
      // leap through complete iSldBlockWidth^3 blocks
      fSpaceLeapDist = (iSpaceLeapInfo-1) * Global_iSldBlockWidth;
      if (Global_bShowSpaceLeapPattern) 
        DrawSpaceLeapPatternLine(args, eMultipleEmptyBlocks);
    }
    else
    {
      // 1 == iSpaceLeapDist
      // leap to the end of the current block
      fSpaceLeapDist = args.pRayIterLabel->GetDistanceToEndOfBlock() + 0.1;
      if (Global_bShowSpaceLeapPattern) 
        DrawSpaceLeapPatternLine(args, eSingleEmptyBlock);
    }
    args.pRayIterLabel->SpaceLeap(fSpaceLeapDist); 

    args.pVolIterSLD->SetPos(args.pRayIterLabel->GetPosInt() / Global_iSldBlockWidth);
  }  
} // RaySpaceLeapLargeBlocks()


/**
 * Move in 2 voxel increments through the region that has invisible labels associated.
 * Go backwards in 0.5 voxel increments if a visible voxel is encountered.
 * @param args ray data structure
 */
inline void vxRaytracer::RaySkipDensityInvisible(vxRaytracerRenderArgs & args)
{
  if (args.bContinueRay == false || args.pRayIterLabel->GetPosFloat().T() > args.fRayIntersectBoundingBoxEndVC.T())
  {
    args.bContinueRay = false;
    return;
  }
  // TODO: revisit component dereferencing
  int4 iLabel(args.pRayIterLabel->GetVoxel());
  if (args.pRayIterLabel->GetPosFloat().T() < args.fRayIntersectBoundingBoxEndVC.T() && !IsIntensityVisible( iLabel ) )
  {
    args.bFirstSample = false;
    // current position is outside the visible region, -> move forward till visible or end of block
    for (int4 iCount(Global_iSldBlockWidth); iCount > 0; --iCount)
    {
      args.pRayIterLabel->SpaceLeapTwo();
      if (args.pRayIterLabel->GetPosFloat().T() > args.fRayIntersectBoundingBoxEndVC.T())
      {
        // the slice shadow would be missing here if we do not
        // update the position of the intensity ray iterator
        args.pRayIterIntensity->SetPos(args.pRayIterLabel->GetPosFloat() );
        args.bContinueRay = false;
        return;
      }
      iLabel = args.pRayIterLabel->GetVoxel();
      
      if (Global_bShowSpaceLeapPattern)
      {
        DrawSpaceLeapPatternLine(args, eInvisible);
      }
      if (args.pRayIterLabel->GetPosFloat().T() < args.fRayIntersectBoundingBoxEndVC.T() && IsIntensityVisible( iLabel ) )
      {
        // we went a little to far, lets go back to the last invisible position
        int4 iNumIterations(0);
        do
        {
          ++iNumIterations;
          args.pRayIterLabel->SpaceLeapMinusHalf();
          if (Global_bShowSpaceLeapPattern == true) 
          {
            DrawSpaceLeapPatternLine(args, eInvisibleBacktrace);
          }
          if (args.pRayIterLabel->GetPosFloat().T() < args.fRayIntersectBoundingBoxBgnVC.T())
          {
            // solves the dotted "lines" around sculpted areas which happen for a 
            // new ray that enters, space leaps too far into a visible component, 
            // and then backs up several times.  Thus we need to go back into the
            // volume several steps.
            if (iNumIterations == 2)
            {
              // In this case it stepped 2*(-0.5) times until it is outside the volume again
              // so we step forward half that distance or 0.5 voxel units
              args.pRayIterLabel->SpaceLeap(0.5);
            }
            else if (iNumIterations == 4 )
            {
              // In this case it stepped 4*(-0.5) times until it is outside the volume again
              // so we step forward half that distance or 1.0 voxel units
              args.pRayIterLabel->SpaceLeap(1.0);
            }
            else
            {
              // does this ever happen
              args.bContinueRay = false;
            }
            return;
          }
        } while ( IsIntensityVisible( args.pRayIterLabel->GetVoxel() ) );
        iCount = 0;
      } // end visibility test
    } // end off-label skipping
  } // end section advance through off-labels
} // RaySkipDensityInvisible()


/**
 * Move in 1 voxel increments through the region that is invisible according 
 * to the transfer function. Go backwards in 0.5 voxel increments if a visible 
 * sample is encountered
 * @param args ray data structure
 */
inline void vxRaytracer::RaySkipAlphaInvisible(vxRaytracerRenderArgs & args)
{
  // check right away to make sure we are not already outside the volume or previously interruped the ray
  if (args.pRayIterLabel->GetPosFloat().T() < args.fRayIntersectBoundingBoxBgnVC.T() ||
      args.pRayIterLabel->GetPosFloat().T() > args.fRayIntersectBoundingBoxEndVC.T())
  {
    args.bContinueRay = false;
  }
  if (args.bContinueRay == false)
  {
    return;
  }

  // transfer the labelRayIter position to the intensity ray iter position
  args.pRayIterIntensity->SetPosFromRayIter(*args.pRayIterLabel);

  // move to next somewhat opaque voxel
	int2 iIntensitySample( min(m_iMaxVoxelValue, args.pRayIterIntensity->GetSampleInt()) );

	///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if( !m_arguments.bGrayscale )
	{
	  Vector4Df vf(args.fRayIntersectBoundingBoxEndVC - args.fRayIntersectBoundingBoxBgnVC);
		vf.Normalize();
		vf = vf * 0.05F;
    if((args.pRayIterIntensity->GetPosFloat() - vf).T() < args.fRayIntersectBoundingBoxBgnVC.T() ||
       (args.pRayIterIntensity->GetPosFloat() + vf).T() > args.fRayIntersectBoundingBoxEndVC.T()) 
    {
      iIntensitySample = 0;
    }
	} // if
	
  // clamp for out-of-range values
  if (iIntensitySample < 0) iIntensitySample = 0;

  // use 2D pre-integration table: NEEDS TO BE REFINED TO TAKE WHATEVER IS APPROPRIATE!!!! 1D/2D
  const int4 iIndexSample((uint2(iIntensitySample)) >> args.iShiftTo8Bit);
  const int4 iIndex(5 * (256 * args.iPrevIndex + iIndexSample) + 3);

  if (args.pfColormap2d8bitPreIntegrated[iIndex] == 0)
  {
    args.bFirstSample = false;
    args.iPrevIndex = iIndexSample;

    if (Global_bShowSpaceLeapPattern)
    {
      DrawSpaceLeapPatternLine(args, eZeroOpacity, true);
    }

    for (int4 iCount(Global_iSldBlockWidth); iCount>0; --iCount)
    {
      args.pRayIterIntensity->SpaceLeap(1); //Two();
      if (Global_bShowSpaceLeapPattern == true)
      {
        DrawSpaceLeapPatternLine(args, eZeroOpacity, true);
      }
  
      // we jumped forward - make sure we did not jump outside
      if( !args.pRayIterIntensity->IsInsideVolume() || args.pRayIterIntensity->GetPosFloat().T() > args.fRayIntersectBoundingBoxEndVC.T() )
      { // The inside volume check is necessary. The space leap may go outside the volume and cause crash in the following code. Dongqing 20080909
        // if we jumped forward, go back and start from there

        args.pRayIterIntensity->SpaceLeap(-1); //Int(-2);
        //LogFyi("The space leap caused the ray iterator go outside of the volume", "vxRaytracer.C", "RaySkipAlphaInvisible");
        return;

      } // if
       
	    ///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			int2 iIntensitySample = min(m_iMaxVoxelValue, args.pRayIterIntensity->GetSampleInt());
	    if( !m_arguments.bGrayscale )
			{
			  Vector4Df vf(args.fRayIntersectBoundingBoxEndVC - args.fRayIntersectBoundingBoxBgnVC);
	      vf.Normalize();
				vf = vf * 0.05F;
        if((args.pRayIterIntensity->GetPosFloat() - vf).T() < args.fRayIntersectBoundingBoxBgnVC.T() ||
           (args.pRayIterIntensity->GetPosFloat() + vf).T() > args.fRayIntersectBoundingBoxEndVC.T() )
        {
          iIntensitySample = 0;
        }
			} // if

			if (0 != args.pfColormap2d8bitPreIntegrated[3 + 5 * (256 * args.iPrevIndex + ( iIntensitySample >> args.iShiftTo8Bit))])
      {
        // sample visible
        do
        {
          args.pRayIterIntensity->SpaceLeapMinusHalf();
          if (Global_bShowSpaceLeapPattern) 
          {
            DrawSpaceLeapPatternLine(args, eZeroOpacityBacktrace, true);
          }

					iIntensitySample = min(m_iMaxVoxelValue, args.pRayIterIntensity->GetSampleInt());
					///++++++++++++++++++++++++++++++++++++++++++++++++++++++
	        if( !m_arguments.bGrayscale )
			    {
			      Vector4Df vf(args.fRayIntersectBoundingBoxEndVC - args.fRayIntersectBoundingBoxBgnVC);
	          vf.Normalize();
				    vf = vf * 0.05F;
            if((args.pRayIterIntensity->GetPosFloat() - vf).T() < args.fRayIntersectBoundingBoxBgnVC.T() ||
               (args.pRayIterIntensity->GetPosFloat() + vf).T() > args.fRayIntersectBoundingBoxEndVC.T() )
            {
              iIntensitySample = 0;
            }
			    } // if

        } while (args.pRayIterIntensity->GetPosFloat().T() > args.fRayIntersectBoundingBoxBgnVC.T() && 0 != args.pfColormap2d8bitPreIntegrated[3 + 5 * (256 * args.iPrevIndex + (iIntensitySample>>args.iShiftTo8Bit))]);

        // WHY IS THIS ONE DONE???
        //args.pRayIterIntensity->SpaceLeap(0.5 - 0.5*args.fInterSampleDistance);
        //if (Global_bShowSpaceLeapPattern) 
        //{
        //  DrawSpaceLeapPatternLine(args, eZeroOpacityBacktrace, true);
        //}
        if (args.pRayIterIntensity->GetPosFloat().T() > args.fRayIntersectBoundingBoxEndVC.T())
        {
          args.bContinueRay = false;
          return;
        }
        iCount = 0;
      }
      else if (iCount == 1)
      { 
        // sample not visible
        args.pRayIterLabel->SetPosFromRayIter(*args.pRayIterIntensity);
        int4 iLabel(args.pRayIterLabel->GetVoxel());

        if (args.pRayIterLabel->GetPosFloat().T() < args.fRayIntersectBoundingBoxEndVC.T() && IsIntensityVisible( iLabel ))
        {
          iCount = 16;
        }
      }
    }
  }
} // RaySkipAlphaInvisible()


/**
 * Sample the intensity and label at the current ray position
 * @param args     the ray data structure
 */
inline void vxRaytracer::RayGetSampleAndLabel(vxRaytracerRenderArgs & args)
{
  ///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  int2 iIntensitySample = min( m_iMaxVoxelValue, args.pRayIterIntensity->GetSampleInt() );
	if( !m_arguments.bGrayscale )
	{
	  Vector4Df vf(args.fRayIntersectBoundingBoxEndVC - args.fRayIntersectBoundingBoxBgnVC);
		vf.Normalize();
		vf = vf * 0.05F;
    if ((args.pRayIterIntensity->GetPosFloat() - vf).T() < args.fRayIntersectBoundingBoxBgnVC.T() ||
        (args.pRayIterIntensity->GetPosFloat() + vf).T() > args.fRayIntersectBoundingBoxEndVC.T())
    {
      iIntensitySample = 0;
    }
	} // 
	
  // Get sample intensity
  args.iSample = max( 0, iIntensitySample );

  // check for out-of-bounds
  if (args.bIsVRMode == true || args.bIsXRayMode == true)
  {
    if ( args.bFirstSample )
    {
      // similar to sample-based classification
      const float4 * pfRGBA(m_settings.GetColormap1d() + 4 * args.iSample);
      const float4 fFactor(1.0F / pow(2.0F, args.iShiftTo8Bit));
      args.fPreMultSample.R() = (*pfRGBA++) * fFactor;
      args.fPreMultSample.G() = (*pfRGBA++) * fFactor;
      args.fPreMultSample.B() = (*pfRGBA++) * fFactor;
      // don't shift but divide because it throws away important bits!!!
      args.fPreMultSample.A() = ((*pfRGBA) * fFactor) / 256.0F;
      //////////////////////////////////////////////////////////////////////////////////////
      // The storage of values in the colormap needs to be completely re-implemented to
      // either use floating-point values (0-1) or better, integer values that can be
      // more efficiently computed on standard processors than floating-point computations.
      // We use "___/256.0F" above instead of float(int(___/255.0F)) for speed purposes.
      // The side effect is that you don't reach full opacity quite as quickly as you
      // should. But, the effect should be impossible to notice.
      //////////////////////////////////////////////////////////////////////////////////////
      args.fPreMultSample.ScaleRGB(args.fPreMultSample.A());

      // accentuate the opacity of the first sample
      args.fPreMultSample.A() = pow( args.fPreMultSample.A(), 0.1F );

      // TODO: test if this should be done above during sampling for efficiency
      if ( m_settings.GetShowCutsGrayscale() || args.bGrayscale )
      {
        args.fPreMultSample.R() = args.fPreMultSample.G() = args.fPreMultSample.B() =
          ( args.fPreMultSample.R() + args.fPreMultSample.G() + args.fPreMultSample.B() ) * 0.3333F;
      }
    }
    else // not first sample
    {

      if (m_settings.GetPreIntegrationEnabled() == true)
      {
        const int4 iIndex((uint2(args.iSample)) >> args.iShiftTo8Bit);
        float4 * pTFunc2D(&args.pfColormap2d8bitPreIntegrated[5*(256*args.iPrevIndex + iIndex)]);
        args.iPrevIndex = iIndex;
        args.fPreMultSample.R() = *pTFunc2D++;
        args.fPreMultSample.G() = *pTFunc2D++;
        args.fPreMultSample.B() = *pTFunc2D++;
        if (args.bIsVRMode == true)
          args.fPreMultSample.A() = *pTFunc2D++;
        else
          args.fPreMultSample.A() = *pTFunc2D++ * 0.1F;
        args.fGradientWeight = *pTFunc2D;
      }
      else // sample based classification
      {
        // TODO: pre-correct the samples for efficiency
        const float4 * pfRGBA(m_settings.GetColormap1dAlphaCorrected() + 4 * args.iSample);
        const float4 fFactor(1.0F / pow(2.0F, args.iShiftTo8Bit));
        args.fPreMultSample.R() = (*pfRGBA++) * fFactor;
        args.fPreMultSample.G() = (*pfRGBA++) * fFactor;
        args.fPreMultSample.B() = (*pfRGBA++) * fFactor;
        // don't shift but divide because it throws away important bits!!!
        args.fPreMultSample.A() = ((*pfRGBA) * fFactor) / 256.0F;
        //////////////////////////////////////////////////////////////////////////////////////
        // The storage of values in the colormap needs to be completely re-implemented to
        // either use floating-point values (0-1) or better, integer values that can be
        // more efficiently computed on standard processors than floating-point computations.
        // We use "___/256.0F" above instead of float(int(___/255.0F)) for speed purposes.
        // The side effect is that you don't reach full opacity quite as quickly as you
        // should. But, the effect should be impossible to notice.
        //////////////////////////////////////////////////////////////////////////////////////
        args.fPreMultSample.ScaleRGB(args.fPreMultSample.A());
      }

      // TODO: test if this should be done above during sampling for efficiency
      if ( args.bGrayscale )
      {
        args.fPreMultSample.R() = args.fPreMultSample.G() = args.fPreMultSample.B() =
          ( args.fPreMultSample.R() + args.fPreMultSample.G() + args.fPreMultSample.B() ) * 0.3333F;
      }

    }

  }
  else //if (args.bIsMipMode == true)
  {
    // MIP mode
    // TODO: pre-correct the samples for efficiency
    const float4 * pfRGBA( m_settings.GetColormap1d() + 4 * args.iSample);
    const float4 fFactor(1.0F / pow(2.0F, args.iShiftTo8Bit));
    args.fPreMultSample.R() = (*pfRGBA++) * fFactor;
    args.fPreMultSample.G() = (*pfRGBA++) * fFactor;
    args.fPreMultSample.B() = (*pfRGBA++) * fFactor;
    // don't shift but divide because it throws away important bits!!!
    float4 fTempAlpha = ((*pfRGBA) * fFactor) / 255.0F;
    args.fPreMultSample.A() = ( fTempAlpha <= 1.0F ) ? fTempAlpha : 1.0F;
    //////////////////////////////////////////////////////////////////////////////////////
    // The storage of values in the colormap needs to be completely re-implemented to
    // either use floating-point values (0-1) or better, integer values that can be
    // more efficiently computed on standard processors than floating-point computations.
    // We are using a floating-point comparison here instead of a truncation for a
    // hopeful speed increase.
    //////////////////////////////////////////////////////////////////////////////////////
    args.fPreMultSample.ScaleRGB( args.fPreMultSample.A() );

    // TODO: test if this should be done above during sampling for efficiency
    if ( args.bGrayscale )
    {
      args.fPreMultSample.R() = args.fPreMultSample.G() = args.fPreMultSample.B() =
        ( args.fPreMultSample.R() + args.fPreMultSample.G() + args.fPreMultSample.B() ) * 0.3333F;
    }
  }

  // get corresponding label
  args.pRayIterLabel->SetPosFromRayIter(*args.pRayIterIntensity);
  args.iLabel = args.pRayIterLabel->GetSampleNearest();
  if (Global_bShowSpaceLeapPattern)
  {
    DrawSpaceLeapPatternLine(args, eColoredSample, false);
  }

} // RayGetSampleAndLabel()


/**
 * blend the label color over the sample
 *
 * @param args     the ray data structure
 */
inline void vxRaytracer::RayBlendLabelColor(vxRaytracerRenderArgs & args)
{
  // blend label color onto sampleColor
  if(args.bShowLabels && args.iLabel > 0 && (*args.pvComponents)[args.iLabel].IsLabelVisible())
  {
    PixelRGBA<float4> color = static_cast<PixelRGBAf>((*args.pvComponents)[args.iLabel].GetColor());

    const float4 fBlendFraction(color.A() / 255.0F );

    args.fPreMultSample.BlendRGB(color, fBlendFraction);
  }
} // RayBlendLabelColor()


/**
 * blend the intensity range over the sample if blending is enabled
 *
 * @param args     the ray data structure
 */
inline void vxRaytracer::RayBlendIntensityRange(vxRaytracerRenderArgs & args)
{
  if (args.bIsShowIntensityRange)
  {
    if ((args.iShowIntensityRangeLower <= args.iSample) && (args.iSample <= args.iShowIntensityRangeUpper))
    {
      PixelRGBAf color(0, 255, 0, 0);
      const float4 fBlendFraction(0.5F );
      color.ScaleRGB(args.fPreMultSample.A());
      args.fPreMultSample.BlendRGB(color, fBlendFraction);
    }
  }
}


/**
 * Blend the slice shadows over the sample if shadow blending is enabled
 * @param args     the ray data structure
 */
inline void vxRaytracer::RayBlendSliceShadows( vxRaytracerRenderArgs & args )
{

  if ( args.bSliceShadowsEnabled &&
       args.pRayIterIntensity->GetPosFloat().T() > args.fShadowPlaneDepth )
  {
    if ( args.bClipSliceShadow == false )
    {
      PixelRGBAf color( m_settings.GetSliceShadowColor() );
      color.A() = Global_fSliceShadowOpacity;
      args.fPreMultSample.Composite( color );
      args.fDepthToSurfaceVC =   args.fShadowPlaneDepth;
    }

    // lets not add more slice shadow color after this
    args.fShadowPlaneDepth = (std::numeric_limits<float>::max)();
  }

} // RayBlendSliceShadows()


/**
 * Blend the slice shadows at the end of sampling the ray
 * @param args     the ray data structure
 */
inline void vxRaytracer::RayBlendSliceShadowsAtEndOfRay( vxRaytracerRenderArgs & args )
{
  if ( args.bSliceShadowsEnabled &&
       ( args.pRayIterIntensity->GetPosFloat().T() > args.fShadowPlaneDepth ||
         args.pRayIterLabel    ->GetPosFloat().T() > args.fShadowPlaneDepth ) &&
         args.bClipSliceShadow == false  )
  {

    args.fPreMultSample.R() = m_settings.GetSliceShadowColor().R();
    args.fPreMultSample.G() = m_settings.GetSliceShadowColor().G();
    args.fPreMultSample.B() = m_settings.GetSliceShadowColor().B();
    args.fPreMultSample.A() = Global_fSliceShadowOpacity;
    args.fPreMultAccum.Composite( args.fPreMultSample );
    args.fDepthToSurfaceVC =   args.fShadowPlaneDepth;

  } // if should blend

} // RayBlendSliceShadowsAtEndOfRay()


/**
 * Color the ray based on the gradient direction (debug use only)
 * @param args          the ray data structure
 * @param vGradVector3D the gradient vector
 */
inline void vxRaytracer::RayColorFromGradientDir(vxRaytracerRenderArgs & args, const Vector3D<float4> vGradVector3D)
{
  //// replace colors with gradient direction for debugging
  if (args.u%64 < 32)
  {
    args.fPreMultSample.R() = args.fPreMultSample.A()*255*fabs(vGradVector3D.Z());
    args.fPreMultSample.G() = args.fPreMultSample.A()*255*fabs(vGradVector3D.X());
    args.fPreMultSample.B() = args.fPreMultSample.A()*255*fabs(vGradVector3D.Y());
  }
} // RayColorFromGradientDir(vxRaytracerRenderArgs & args, const Vector3D<float4> vGradVector3D)


/**
 * Gets the next MIP sample along the ray
 * @param args     the ray data structure
 */
inline void vxRaytracer::RayNextMipSample(vxRaytracerRenderArgs & args)
{
  // only do something (replace) if the current sample has higher opacity than any of the previous.
  if (args.fPreMultAccum.A() < args.fPreMultSample.A())
  {
    args.fPreMultAccum = args.fPreMultSample;
  }
} // RayNextMipSample(vxRaytracerRenderArgs & args)


/**
 * Adjust (lower) the sample color intensity doe to shading.
 * Considers one diffuse and one specular light source.
 * Both operate on the data gradient as well as the inverted data gradient to
 * allow surface appearance for low to high density transitions as well as high to low
 * @param args     the ray data structure
 */
inline void vxRaytracer::RayShadeSample(vxRaytracerRenderArgs & args)
{
  if ( m_settings.GetShadingEnabled() && ! args.bFirstSample )
  {
    // get gradient from precomputed optimal position between previous and curent sample
    if (m_settings.GetPreIntegrationEnabled() == true)
      args.pRayIterIntensity->SpaceLeap(-args.fGradientWeight * args.fInterSampleDistance);

    Vector3D<float4> vGradVector3D((args.fPreMultSample.A() > 0.005) ? -args.pRayIterIntensity->GetSampleGradientSobel() : -args.pRayIterIntensity->GetSampleGradientCentralDifference());
    if (Global_bShowSpaceLeapPattern) DrawSpaceLeapPatternLine(args, eGradientLocation, true);

    if (m_settings.GetPreIntegrationEnabled() == true)
      args.pRayIterIntensity->SpaceLeap(args.fGradientWeight * args.fInterSampleDistance);

    // anisotropic gradient direction correction
    vGradVector3D.Y() *= args.fGradientScaleY;
    vGradVector3D.Z() *= args.fGradientScaleZ;

    // illumination computation
    if (vGradVector3D.GetLengthSquared() > 10)
    {
      vGradVector3D.Normalize();
      float4 fIlluminationIntensity(m_settings.GetIlluminator().ShadePhongDiffuse (vGradVector3D, args.fLight2DirVC));
      float4 fSpecular(args.fPreMultSample.A() * 255 * m_settings.GetIlluminator().ShadePhongSpecular(vGradVector3D, args.fHalfwayLight1VC));

      // replace above with below for x-ray shading effect
      //fIlluminationIntensity = 1.0F - fIlluminationIntensity * fIlluminationIntensity; // invert the direct shading
      //float4 fSpecular( 0.5F * args.fPreMultSample.A() * 255 * m_settings.GetIlluminator().ShadePhongSpecular( vGradVector3D, args.fHalfwayLight1VC ) );

      if (m_settings.GetShadowingEnabled() == true)
      {

        //
        // sample from the current sample position toward the light source and get an opacity in the range (0,1)
        //
        Vector3Df samplePosition( args.pRayIterIntensity->GetPosFloat().m_x, args.pRayIterIntensity->GetPosFloat().m_y, args.pRayIterIntensity->GetPosFloat().m_z );
        //const float4 fJitterAmount = 0.1F;
        //Normal3Df jitteredLightDirection( args.fHalfwayLight1VC +
        //  Vector3Df( Drand48( -fJitterAmount, fJitterAmount ),
        //             Drand48( -fJitterAmount, fJitterAmount ),
        //             Drand48( -fJitterAmount, fJitterAmount ) ) );
        Normal3Df jitteredLightDirection( args.fHalfwayLight1VC );
        Point3Df startPosition( samplePosition + jitteredLightDirection * 1.0F ); // start a little beyond current sample to avoid self-shadowing
        Point3Df endPosition( samplePosition + jitteredLightDirection * 1000.0F ); // well outside the volume - revisit

        // compute ray exit point
        Vector4Df endPos4d;
        vxUtils::HitBoundingBox4d(args.fCropBoxVC, Vector4Df(endPosition, 0 ), -Vector4Df(jitteredLightDirection.X(), jitteredLightDirection.Y(), jitteredLightDirection.Z(), 1 ), endPos4d);
        RaySegment raySegment(startPosition, Point3Df(endPos4d.m_x, endPos4d.m_y, endPos4d.m_z ) ); 
        if (raySegment.GetEndPoint().T() <= raySegment.GetStartPoint().T())
        {
          raySegment.GetEndPoint().T() = raySegment.GetStartPoint().T()+0.1F;
        }

        // setup the shadow ray
        Vector4Df raySampleIncVec(raySegment.GetDirection().GetNormalizedXYZ() * args.fInterSampleDistance);
        args.pRayIterShadow->SetRaySegment(raySegment );
        args.pRayIterShadow->SetSampleIncrementVector(raySampleIncVec );

        // sample the entire shadow ray
        float4 fOpacity = 0.0F;
        while (fOpacity < 0.95F && args.pRayIterShadow->GetPosFloat().T() < raySegment.GetEndPoint().T() )
        {
          ///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          bool bIsBoundary(false);
	        if( !m_arguments.bGrayscale )
					{
					  Vector4Df vf(args.fRayIntersectBoundingBoxEndVC - args.fRayIntersectBoundingBoxBgnVC);
            vf.Normalize();
						vf = vf * 0.05F;
            if ((args.pRayIterIntensity->GetPosFloat() - vf).T() < args.fRayIntersectBoundingBoxBgnVC.T() ||
                (args.pRayIterIntensity->GetPosFloat() + vf).T() > args.fRayIntersectBoundingBoxEndVC.T())
            {
              bIsBoundary = true;
            }
					}  // if

          // sample and classify
          int4 iSample = min(m_iMaxVoxelValue, (bIsBoundary ? 0 : args.pRayIterIntensity->GetSampleInt()));
          const float4 * pfRGBA(m_settings.GetColormap1dAlphaCorrected() + 4 * iSample + 3); // get to Alpha
          const float4 fFactor(1.0F / pow(2.0F, args.iShiftTo8Bit));
          // don't shift but divide because it throws away important bits!!!
          float4 fAlpha = ((*pfRGBA) * fFactor) / 256.0F;

          // check that it's not invisible
          if (fAlpha > 0 )
          {
            Vector3Di nearestPos(args.pRayIterShadow->GetPosFloat().m_x + 0.5F,
                                  args.pRayIterShadow->GetPosFloat().m_y + 0.5F,
                                  args.pRayIterShadow->GetPosFloat().m_z + 0.5F );
            args.pIterLabelShadow->SetPos(nearestPos );
            int4 iLabel(args.pIterLabelShadow->GetVoxel() );
            if( !IsIntensityVisible( iLabel )) fAlpha = 0;
          }
          // accumlate ray opacity
          fOpacity += (1.0F - fOpacity ) * fAlpha;
          args.pRayIterShadow->SpaceLeapOne();
        }

        // clamp
        if (fOpacity < 0 ) fOpacity = 0;
        if (fOpacity > 1 ) fOpacity = 1;

        // increase the ambient light so the shadows are not too dark
        const float4 fAmbientIntensity(0.65F );
        fOpacity *= (1.0F - fAmbientIntensity );

        // darken the sample
        fIlluminationIntensity *= (1.0F - fOpacity );
        fSpecular *= (1.0F - fOpacity );
      }

      // sample color intensity adjustment
      args.fPreMultSample.ScaleRGB(fIlluminationIntensity);
      args.fPreMultSample.AddRGB(fSpecular);
    }
  }

  if (m_settings.GetBoxOfInterestActive() == true)
  {
    Vector4d<float4> fPos(args.pRayIterIntensity->GetPosFloat());

    if(args.fBoxOfInterest.IsInside(Triple<float4>(fPos.X(), fPos.Y(), fPos.Z())))
    {
      PixelRGBA<float4> blend(0, 0, 255, 255);
      args.fPreMultSample.BlendRGB(blend, .40F);
      //args.fPreMultSample.ScaleRGB(0.75F);
    }
  }
} // RayShadeSample()


/**
 * Composite the current sample over the accumulated ray
 * @param args     the ray data structure
 */
inline void vxRaytracer::RayCompositeSample(vxRaytracerRenderArgs & args)
{
  args.fPreMultAccum.Composite(args.fPreMultSample);
} // RayCompositeSample()


/**
 * Take the accumulated ray colors and convert them into displayable uint1 pixels
 * @param args     the ray data structure
 */
inline void vxRaytracer::RaySetFinalColors(vxRaytracerRenderArgs & args)
{
  // if x-ray convert to RGBA
  if (args.bIsXRayMode)
  {
    args.fPreMultAccum.A() = 1 - exp(- 3.0F * args.fPreMultAccum.A() );
    const float4 fAlpha( args.fPreMultAccum.A() );
    args.fPreMultAccum.R() = fAlpha * args.fForegroundColor.R() + (1 - fAlpha ) * args.fBackgroundColor.R();
    args.fPreMultAccum.G() = fAlpha * args.fForegroundColor.G() + (1 - fAlpha ) * args.fBackgroundColor.G();
    args.fPreMultAccum.B() = fAlpha * args.fForegroundColor.B() + (1 - fAlpha ) * args.fBackgroundColor.B();
    args.fPreMultAccum.A() = fAlpha * args.fForegroundColor.R();
  }
  else
  {
    // blend ray over background color
    args.fPreMultAccum.Composite( args.fBackgroundColor );
  }

  // convert alpha from 0-1 to 0-255
  args.fPreMultAccum.A() *= 255;

  // ensure [0,255] range for color
  if (args.fPreMultAccum.R() > 255) args.fPreMultAccum.R() = 255;
  if (args.fPreMultAccum.G() > 255) args.fPreMultAccum.G() = 255;
  if (args.fPreMultAccum.B() > 255) args.fPreMultAccum.B() = 255;
  if (args.fPreMultAccum.A() > 255) args.fPreMultAccum.A() = 255;

  //// transfer final float4 colors to int1 display colors
  args.iDisplayColor.R() = RoundFloatToInt_FISTP(args.fPreMultAccum.R());
  args.iDisplayColor.G() = RoundFloatToInt_FISTP(args.fPreMultAccum.G());
  args.iDisplayColor.B() = RoundFloatToInt_FISTP(args.fPreMultAccum.B());
  args.iDisplayColor.A() = RoundFloatToInt_FISTP(args.fPreMultAccum.A());
} // RaySetFinalColors()


/**
 * Composite sampling points along a single ray.
 * @param args the ray data structure
 */
void vxRaytracer::SampleSingleRay(vxRaytracerRenderArgs & args)
{
  // initialize the colors that are about to be accumulated along the ray
  args.fPreMultAccum.R() = args.fPreMultAccum.G() = args.fPreMultAccum.B() = args.fPreMultAccum.A() = 0;
  args.fDepthToSurfaceVC = (std::numeric_limits<float>::max)();
  args.fDepthToSurfaceWC = (std::numeric_limits<float>::max)();
  args.iPrevIndex = 0;
  args.bFirstSample = ! m_settings.GetShowCutsShaded()                   && ! m_arguments.bIsMipMode &&
                      ! m_arguments.bIsXRayMode && m_settings.GetPreIntegrationEnabled();

  // potentially jump to neighbor depth in order to speed up the space leaping portion of ray casting
  if ( args.bUseNeighborDepth )
  {
    const float4 fStepBack = 2.0F;
    const float4 fDistanceToLeap( args.fNeighborDepth - fStepBack - args.pRayIterIntensity->GetPosFloat().T() );
    if ( fDistanceToLeap > 1 )
    {
      const float4 fDistancePerLeap( args.pRayIterIntensity->GetSldDeltaVector().T() );
      const float4 fLeaps( fDistanceToLeap / fDistancePerLeap );
      const Vector4d<float4> originalPosition = args.pRayIterIntensity->GetPosFloat();
      args.pRayIterIntensity->SpaceLeap( fLeaps );
      args.pRayIterLabel->SetPosFromRayIter( * args.pRayIterIntensity );

      // forget acceleration if we leaped directly into the data or out of the volume
      RayGetSampleAndLabel(args);
      if ( args.fPreMultSample.A() > 0 )
      {
        // send a message back that the neighbor depth was bad since we encountered a suprise patch of opaque data
        args.bUseNeighborDepth = false;
        args.pRayIterIntensity->SetPos( originalPosition );
        args.pRayIterLabel->SetPosFromRayIter( * args.pRayIterIntensity );
      }
    }
  }

  //// Composite the sampling points along a ray.
  const bool bUseSpaceLeaping = m_settings.GetUseSpaceLeaping();
  while (args.bContinueRay == true)
  {
    if ( bUseSpaceLeaping )
      SpaceLeap(args);

    //// sample at least 4 times, expecting to get a density > 0
    for (int4 iCount(4); args.bContinueRay == true && iCount>0; --iCount)
    {
      if (args.pRayIterIntensity->GetPosFloat().T() <= args.fRayIntersectBoundingBoxEndVC.T()) 
      {
        RayGetSampleAndLabel(args);

        // composite only if alpha is > 0
        if ( args.fPreMultSample.A() > 0 && IsIntensityVisible(args.iLabel)
             && ((!args.bIsLabelsOnTopMode) || (args.bIsLabelsOnTopMode && args.iLabel!=0 )) )
        {
          // set "z"-buffer depth
          if (args.fPreMultAccum.A() == 0)
          {
            const Vector4Df v = args.pRayIterIntensity->GetPosFloat().GetScaledUnIso( Vector4Df( args.fVoxelUnits, 1.0F ) );
            const Vector3Df sampleVectorWorld( Point3Df( v.X(), v.Y(), v.Z() ) - args.eyePositionWorld );
            const float4 fDistance = sampleVectorWorld.Dot( args.viewDirectionWC );
            args.fDepthToSurfaceWC = fDistance;
            args.fDepthToSurfaceVC = args.pRayIterIntensity->GetPosFloat().T();
          }

          RayBlendLabelColor(args);
          RayBlendIntensityRange(args);
          
          //#if SLICE_SHADOWS_ENABLED
            RayBlendSliceShadows(args);
          //#endif

          if (args.bIsVRMode )
          {
            RayShadeSample(args);
            RayCompositeSample(args);
          }
          else if (args.bIsMipMode )
          {
            RayNextMipSample(args);
          }
          else if (args.bIsXRayMode )
          {
            RayCompositeSample(args);
          }

          if (args.fPreMultAccum.A() > 0.95 && ! args.bIsMipMode )
          {
            args.bContinueRay = false;
          }
          if (Global_bShowSpaceLeapPattern )
          {
            DrawSpaceLeapPatternLine(args, eColoredSample);
          }

          args.bFirstSample = false;
          ++iCount;  // now we are in the important section of the data, lets keep sampling
        } // composite only if alpha is > 0
        else
        { // alpha == 0
          if (args.pRayIterIntensity->GetPosFloat().T() > args.fRayIntersectBoundingBoxEndVC.T()) 
          {
            args.bContinueRay = false;
          }
          if (Global_bShowSpaceLeapPattern && args.fPreMultSample.A() == 0) 
          {
            DrawSpaceLeapPatternLine(args, eZeroOpacity);
          }
        } // start empty space traversal again, if iCount has reached value 4
      }
      else
      {
        // we actually went outside and the iterator is outside volume 
        args.bContinueRay = false;
        break;
      }

      ++(*args.pRayIterIntensity);
    } // end sampling
  } // while not early ray termination

  //#if SLICE_SHADOWS_ENABLED
    RayBlendSliceShadowsAtEndOfRay(args);
  //#endif
  
  RaySetFinalColors(args);
  
  if (Global_bShowSpaceLeapPattern) DrawSpaceLeapPatternLine(args, eRayEnd);

} // SampleSingleRay()


/**
 * Decide if the pixels are close enough in color to interpolate rather than raycast
 * @param pPixelA             pixel A
 * @param pPixelB1            pixel B1
 * @param pPixelB2            pixel B2
 * @param pPixelB3            pixel B3
 * @param bPixelsAreDifferent if the corner pixels are similar
 * 
 * @return true if ok to interpolate
 */
inline bool vxRaytracer::IsOkToInterpolate(const uint1 * const pPixelA,const uint1 * const pPixelB1, const uint1 * const pPixelB2, const uint1 * const pPixelB3, bool & bPixelsAreDifferent )
{
  const int4 iMaxDiff(Global_iMaxDifferenceColor );

  // in case any of the three corner pixels is different, it is not ok to interpolate
  if ((abs(* (pPixelA   ) - * (pPixelB1   ) ) >= iMaxDiff ) || // R
       (abs(* (pPixelA+1 ) - * (pPixelB1+1 ) ) >= iMaxDiff ) || // G
       (abs(* (pPixelA+2 ) - * (pPixelB1+2 ) ) >= iMaxDiff ) || // B
       (abs(* (pPixelA+3 ) - * (pPixelB1+3 ) ) >= iMaxDiff ) || // A

       (abs(* (pPixelA   ) - * (pPixelB2   ) ) >= iMaxDiff ) || // R
       (abs(* (pPixelA+1 ) - * (pPixelB2+1 ) ) >= iMaxDiff ) || // G
       (abs(* (pPixelA+2 ) - * (pPixelB2+2 ) ) >= iMaxDiff ) || // B
       (abs(* (pPixelA+3 ) - * (pPixelB2+3 ) ) >= iMaxDiff ) || // A

       (abs(* (pPixelA   ) - * (pPixelB3   ) ) >= iMaxDiff ) || // R
       (abs(* (pPixelA+1 ) - * (pPixelB3+1 ) ) >= iMaxDiff ) || // G
       (abs(* (pPixelA+2 ) - * (pPixelB3+2 ) ) >= iMaxDiff ) || // B
       (abs(* (pPixelA+3 ) - * (pPixelB3+3 ) ) >= iMaxDiff ) )  // A
  {
    bPixelsAreDifferent = true;
    return false;
  }

  // if we get here we know that the corner pixels are similar
  bPixelsAreDifferent = false;

  // we may still want NOT to interpolate if there are heterogeneous pixels nearby
  #ifdef _WIN64
    int8 iPixelPosDiff = _abs64((int8)pPixelB1 - (int8)pPixelA);
  #else
    int4 iPixelPosDiff = abs(pPixelB1 - pPixelA);
  #endif
  if ( (* (pPixelB1 + 3 ) & iPixelPosDiff ) ||
       (* (pPixelB2 + 3 ) & iPixelPosDiff ) ||
       (* (pPixelB3 + 3 ) & iPixelPosDiff ) )
  {
    return false;
  }

  // ok all bad cases did not happen, lets interpolate
  return true;
} // IsOkToInterpolate()


/**
 * Test if Pixels are similar enough to just interpolate intead of sending a ray.
 * 
 * @param flc_pPixelA first pixel
 * @param flc_pPixelB second pixel
 * @param depthA      depth value of first pixel
 * @param depthB      depth pixel of second pixel
 * 
 * @return true if pixels are similar
 */
bool vxRaytracer::ArePixelsSimilar(const uint1 * const pPixelA, const uint1 * const pPixelB1,const uint1 * const pPixelB2,const uint1 * const pPixelB3)
{
  const int4 iMaxDiff(Global_iMaxDifferenceColor );

  return ((abs(* (pPixelA   ) - * (pPixelB1   ) ) < iMaxDiff ) && // R
           (abs(* (pPixelA+1 ) - * (pPixelB1+1 ) ) < iMaxDiff ) && // G
           (abs(* (pPixelA+2 ) - * (pPixelB1+2 ) ) < iMaxDiff ) && // B
           (abs(* (pPixelA+3 ) - * (pPixelB1+3 ) ) < iMaxDiff ) && // A

           (abs(* (pPixelA   ) - * (pPixelB2   ) ) < iMaxDiff ) && // R
           (abs(* (pPixelA+1 ) - * (pPixelB2+1 ) ) < iMaxDiff ) && // G
           (abs(* (pPixelA+2 ) - * (pPixelB2+2 ) ) < iMaxDiff ) && // B
           (abs(* (pPixelA+3 ) - * (pPixelB2+3 ) ) < iMaxDiff ) && // A

           (abs(* (pPixelA   ) - * (pPixelB3   ) ) < iMaxDiff ) && // R
           (abs(* (pPixelA+1 ) - * (pPixelB3+1 ) ) < iMaxDiff ) && // G
           (abs(* (pPixelA+2 ) - * (pPixelB3+2 ) ) < iMaxDiff ) && // B
           (abs(* (pPixelA+3 ) - * (pPixelB3+3 ) ) < iMaxDiff ) ); // A

} // ArePixelsSimilar()


/**
 * Test if Pixels are similar enough to just interpolate intead of sending a ray
 * @param pPixelA pixel in question
 * @return true if pixel is transparent
 */
bool vxRaytracer::IsPixelTransparent(const unsigned char * pPixelA)
{
  return (*(pPixelA+3)==0); // previously it tested RGB to each be zero
} // IsPixelTransparent()


/**
 * Interpolates between two pixels. Assumes interpolation weight 0.5
 * @param pPixelA first input pixel
 * @param pPixelB second input pixel
 * @param pPixelC result pixel
 */
void vxRaytracer::PixelInterpolate(const uint1 * const pPixelA, const uint1 * const pPixelB,  uint1 * pPixelC)
{
  *(pPixelC )  = ((int4)*(pPixelA  ) + (int4)*(pPixelB  )) / 2; // R
  *(pPixelC+1) = ((int4)*(pPixelA+1) + (int4)*(pPixelB+1)) / 2; // G
  *(pPixelC+2) = ((int4)*(pPixelA+2) + (int4)*(pPixelB+2)) / 2; // B
  *(pPixelC+3) = ((int4)*(pPixelA+3) + (int4)*(pPixelB+3)) / 2; // A
} // PixelInterpolate()




///////////////////////////////////////////////////////////////////////
// debug image creation


/**
 * Draws the adaptive sampling pattern.
 * 
 * @param args          render settings
 * @param u             x position
 * @param v             y position
 * @param eSamplingInfo indicates ray - start/interpolated/additional
 */
void vxRaytracer::DrawAdaptiveSamplingPatternPixel(vxRaytracerRenderArgs & args, int4 u, int4 v, uint1 eSamplingInfo)
{
  if (u==0 || v==0 || u==args.iImageWidth || v==args.iImageHeight) return;
  const uint4 byte_per_pixel(4);
  const uint4 uStepPixelX(byte_per_pixel);
  const uint4 uStepPixelY(m_pRenderBuffer->GetColorImage().GetDim().X() * uStepPixelX);
  uint1 * pPixel(args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + v * uStepPixelY);
  int4 iIndex(eSamplingInfo - eFirstRayInTile);
  PixelRGBA<uint1> pixel(PixelRGBA<uint1>::GetColorPalettePixel(iIndex));
  *(pPixel++) = pixel.R(); *(pPixel++) = pixel.G(); *(pPixel++) = pixel.B(); *(pPixel++) = pixel.A();

  if (iIndex <= eHeterogeniousRay - eFirstRayInTile)
  {
    pPixel = args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + (v+args.iImageHeight) * uStepPixelY;
    *(pPixel++) = pixel.R(); *(pPixel++) = pixel.G(); *(pPixel++) = pixel.B(); *(pPixel++) = pixel.A();
  }
  if (u<4 && v<4)
  {
    *(args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + v * uStepPixelY + 0) = 255;
    *(args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + v * uStepPixelY + 1) = 255;
    *(args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + v * uStepPixelY + 2) = 255;
    *(args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + v * uStepPixelY + 3) = 255;
  }
  // make non-colored pixels darker
  pPixel = args.pImageBuffer + (u) * uStepPixelX + (v) * uStepPixelY;
  if (*pPixel == 0 && *(pPixel+1) == 0 && *(pPixel+2) == 0 && *(pPixel+3) == 0)
  {
    pPixel = args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + (v+args.iImageHeight) * uStepPixelY;
    *(pPixel) = *pPixel++ / 2; // R
    *(pPixel) = *pPixel++ / 2; // G
    *(pPixel) = *pPixel++ / 2; // B
    *(pPixel) = *pPixel++ / 2; // A
    pPixel = args.pImageBuffer + (u+args.iImageWidth) * uStepPixelX + (v) * uStepPixelY;
    *(pPixel) = *pPixel++ / 2; // R
    *(pPixel) = *pPixel++ / 2; // G
    *(pPixel) = *pPixel++ / 2; // B
    *(pPixel) = *pPixel++ / 2; // A
  }
} // DrawAdaptiveSamplingPatternPixel()


/**
 * Draws space leap pattern line.
 * 
 * @param args                       render settings
 * @param eSamplingInfo              indicates ray - start/interpolated/additional
 * @param bNeedToUpdateLabelPosition unknown
 */
void vxRaytracer::DrawSpaceLeapPatternLine(vxRaytracerRenderArgs & args, uint1 eSamplingInfo, bool bNeedToUpdateLabelPos)
{
  // early exit if we are not in the right scanline 
  if (args.v != args.iImageHeight/2)
  {
    return;
  }

  if (bNeedToUpdateLabelPos == true)
  {
    args.pRayIterLabel->SetPosFromRayIter(*args.pRayIterIntensity);
  }

  const int4 iSldBlockWidth = Global_iSldBlockWidth;
  // depending on the type of sampling info, either fill in the sampling info arrays or display the image
  m_evSamplingInfo[m_iSamplingPatternIndex]  = eSamplingInfo;
  switch (eSamplingInfo)
  {
  case eRayStart:
    {
      m_fvSamplingPos[m_iSamplingPatternIndex++] = args.fRayIntersectBoundingBoxBgnVC;
      break;
    }
  case eMultipleEmptyBlocks:
  case eSingleEmptyBlock:
  case eSingleEmptySmallBlock:
  case eInvisible:
  case eInvisibleBacktrace:
  case eZeroOpacity:
  case eZeroOpacityBacktrace:
  case eColoredSample:
  case eGradientLocation:
  case eRayEnd:
    {
      m_fvSamplingPos[m_iSamplingPatternIndex++] = args.pRayIterLabel->GetPosFloat();
      break;
    }
  case eNowDrawImage:
    {
      // update extreme points of the ray
      float4 fThisFrameDistBBox(9999999.9f);
      float4 fThisFrameDistRayEnd(0.2f);
      float4 fLastFrameDistBBox(0.1f);
      float4 fLastFrameDistRayEnd(0.2f);

      if (m_iSamplingPatternIndex>1)
      {
        Vector4d<float4> fRayStartPos(m_fvSamplingPos[0]);
        Vector4d<float4> fBBoxHitPos(m_fvSamplingPos[1]);
        Vector4d<float4> fRayEndPos(m_fvSamplingPos[m_iSamplingPatternIndex-1]);
        float4 fDistBBox((fBBoxHitPos.T() - fRayStartPos.T()));
        float4 fDistRayEnd((fRayEndPos.T()  - fRayStartPos.T()));
        if (fDistBBox   > 0.1f && fDistBBox   < fThisFrameDistBBox) fThisFrameDistBBox = fDistBBox;
        if (fDistRayEnd > 0.1f && fDistRayEnd > fThisFrameDistRayEnd) fThisFrameDistRayEnd = fDistRayEnd;
      }

      const uint4 byte_per_pixel(4);
      const uint4 uStepPixelX(byte_per_pixel);
      const uint4 uStepPixelY(m_pRenderBuffer->GetColorImage().GetDim().X() * uStepPixelX);
      const float4 fScale(((args.iImageHeight-1) / (fLastFrameDistRayEnd-fLastFrameDistBBox)));
      const float4 fOffset(0 + fScale * fLastFrameDistBBox); 
      Vector4d<float4> fPosInc(args.fViewDirectionVC.GetNormalized() / fScale, 1/fScale);
      Vector4d<float4> fPos(m_fvSamplingPos[0] + fPosInc * fOffset);
      {
        //// sampling pattern
        uint1 * pImageDataPtr(args.pImageBuffer + (args.iImageHeight+1)*uStepPixelY + (args.iPixelOffset % uStepPixelY) + args.iImageWidth * 4);

        // clear bg to black
        for (int4 yPos(0); yPos < args.iImageHeight-1; ++yPos)
        {
          uint1 * pPixel(pImageDataPtr +  yPos * uStepPixelY);
          *pPixel++ = 0;
          *pPixel++ = 0;
          *pPixel   = 0;
        }

        // fill in fg with colors for the sampling events
        for (int4 iIndex(1); iIndex < m_iSamplingPatternIndex; ++iIndex)
        {
          PixelRGBA<uint1> pixel(PixelRGBA<uint1>::GetColorPalettePixel(m_evSamplingInfo[iIndex] - 1));
          float4 fPosY((-fOffset + fScale * (m_fvSamplingPos[iIndex] - m_fvSamplingPos[0]).GetLength()));
          //fPosY /= (args.iImageHeight-1.0f);
          //fPosY = pow(fPosY, 3);
          //fPosY *= (args.iImageHeight-1.0f);
          int4 yPos(Bound(0.0f, fPosY, args.iImageHeight-1.0f));
          uint1 * pPixel(pImageDataPtr +  yPos * uStepPixelY);
          *pPixel++ = pixel.R();
          *pPixel++ = pixel.G();
          *pPixel++ = pixel.B();
          *pPixel   = pixel.A();
        }
      }
      {
        //// space leap info pattern
        // set gray color according to SLD info
        uint1 * pImageDataPtr(args.pImageBuffer + (args.iImageHeight+1)*uStepPixelY + (args.iPixelOffset % uStepPixelY));
        vxBlockVolumeIterator< int1> sldIter(m_settings.GetSpaceLeapVolume());
        vxBlockVolumeIterator<uint2> labelIter(m_settings.GetLabelVolume());
        vxBlockVolumeIterator<uint2> intensityIter(m_settings.GetIntensityVolume());

        { // draw color map
          for (int4 yPos(args.iImageHeight-11); yPos < args.iImageHeight-1; ++yPos)
          {
            uint1 * pPixel(pImageDataPtr + yPos * uStepPixelY);
            int4 xPos(args.iPixelOffset % uStepPixelY);
            PixelRGBA<uint1> pixel = PixelRGBA<uint1>::GetColorPalettePixel(xPos * 5 / args.iImageWidth);
            *pPixel++ = pixel.R();
            *pPixel++ = pixel.G();
            *pPixel++ = pixel.B();
            *pPixel   = pixel.A();
          }
        }
        // draw space leap info
        for (int4 yPos(0); yPos < args.iImageHeight-11; ++yPos, fPos = fPos + fPosInc)
        {
          sldIter.SetPos(fPos.X()/iSldBlockWidth, fPos.Y()/iSldBlockWidth, fPos.Z()/iSldBlockWidth);
          int4 color(Min(255, int4(sldIter.GetVoxel()) * 55));
          uint1 * pPixel(pImageDataPtr + yPos * uStepPixelY);

          PixelRGBA<uint1> pixel;
          labelIter.SetPos(fPos.X()+0.5, fPos.Y()+0.5, fPos.Z()+0.5);
          if (fPos.X() < 0 || fPos.X() > m_settings.GetLabelVolume().GetHeader().GetVolDim().X()-1 ||
               fPos.Y() < 0 || fPos.Y() > m_settings.GetLabelVolume().GetHeader().GetVolDim().Y()-1 ||
               fPos.Z() < 0 || fPos.Z() > m_settings.GetLabelVolume().GetHeader().GetVolDim().Z()-1)
          { // outside valid blocks area
            pixel = PixelRGBA<uint1>::GetColorPalettePixel(11);
          }
          else
          { // normal inside, but not visible area
            pixel = PixelRGBA<uint1>::GetColorPalettePixel(sldIter.GetVoxel());
            // visibile aera
            if( IsIntensityVisible(labelIter.GetVoxel()) )
            {
              intensityIter.SetPos(labelIter);
              if (args.pfColormap2d8bitPreIntegrated[3+5*(256*(intensityIter.GetVoxel()>>args.iShiftTo8Bit))] > 0)
              //if (args.pfColormap2d8bitPreIntegrated[3+5*(257*(intensityIter.GetVoxel()>>args.iShiftTo8Bit))] > 0)
              // The 257 appears to be a typo. No so sure. Dongqing 20080909
              {
                pixel = PixelRGBA<uint1>::GetColorPalettePixel(12);
              }
            }
          }
          *pPixel++ = pixel.R();
          *pPixel++ = pixel.G();
          *pPixel++ = pixel.B();
          *pPixel   = pixel.A();
        }
      }
      if (args.u == args.iImageWidth-1)
      {
        if (fThisFrameDistBBox > 9999999) fThisFrameDistBBox = 0.2f;
        fLastFrameDistBBox   = fThisFrameDistBBox  ; 
        fLastFrameDistRayEnd = fThisFrameDistRayEnd; 
        fThisFrameDistBBox   = 9999999.9f;
        fThisFrameDistRayEnd = 0.2f;
      }
      // reintialize for the next ray.
      for (int4 k(0); k<m_evSamplingInfo.size(); ++k)
      {
        m_evSamplingInfo[k] = 0;
      }

      // show line in volume rendered image
      *(args.pImageBuffer + args.u * uStepPixelX + args.v * uStepPixelY   ) = 200;

      m_iSamplingPatternIndex = 0;
      break;
    } // end case eNowDrawImage
  } // end switch eSamplingInfo
} // DrawSpaceLeapPatternLine()


// $Log: vxRaytracer.C,v $
// Revision 1.26.2.2  2008/09/11 20:09:01  dongqing
// chang fixing approach for the oblique interaction crash. The IsInsideVolume
// check is necessary for ray iterator all the time.
//
// Revision 1.26.2.1  2008/09/11 14:01:26  dongqing
// add array index negative check to fix the oblique crash in the
// RaySkipAlphaInvisible. This is not a directly fix. The ray iterator bounds check
// somehow failed. That was not fixed.
//
// Revision 1.26  2008/04/24 21:55:35  jmeade
// Merge from CON_3-0 on 080424
//
// Revision 1.25  2007/09/21 20:39:10  geconomos
// dummy check in RayGetSmaple (needs to be fixed)
//
// Revision 1.24  2007/09/17 18:49:48  geconomos
// only updating depth buffer (in world coordinates)  if one is specified in the vxRenderBuffer
//
// Revision 1.23  2007/04/12 14:13:41  geconomos
// switched threading to tile-based approach
//
// Revision 1.22.2.1  2008/01/21 21:13:05  geconomos
// corrected the setting of Alpha when a ray doesn't intersect with bounding box
//
// Revision 1.22  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.21  2007/02/05 19:28:40  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.20  2007/01/19 16:59:36  gdavidson
// Issue #5296: Freed memory used by render buffer
//
// Revision 1.19  2006/12/11 16:58:46  geconomos
// Issue #5158: compnent display is incorrect
//
// Revision 1.18  2006/12/08 18:51:10  romy
// checks the cancelRenderingFlag during Renderer to avoid delays in 2D manipulation
//
// Revision 1.17  2006/11/13 15:32:22  geconomos
// issue# 5082: temp fix for high intensity color lookup.
//
// Revision 1.16  2006/10/24 12:31:20  geconomos
// re-enabled clipping slice shadows
//
// Revision 1.15  2006/10/23 01:38:29  geconomos
// using two depth buffers for world and volume coordinates
//
// Revision 1.14  2006/10/20 20:58:04  geconomos
// disabling neighbor depth for a bit
//
// Revision 1.13  2006/10/20 20:52:32  geconomos
// changed depth buffer to store values in world coordinates
//
// Revision 1.12  2006/10/04 14:58:27  geconomos
// added conditional compilation for rendering the slice shadows
//
// Revision 1.11  2006/09/21 14:59:06  geconomos
// switch datatype for components from td::vector<vxComponent*>to std::vector<vxComponent>
//
// Revision 1.10  2006/09/20 15:05:56  frank
// removed dead code
//
// Revision 1.9  2006/08/03 14:17:25  geconomos
// updated for switch to vxRenderingNodePlugin usage
//
// Revision 1.8  2006/07/21 15:03:13  geconomos
// cosmetics
//
// Revision 1.7  2006/07/18 16:22:32  geconomos
// + removed unused "SampleDistanceScale" instance field
// + using DepthToSurface instead of pixel color when determing if a ray hit something
//
// Revision 1.6  2006/07/12 19:51:49  geconomos
// + renabled display of segmentation intensity range
// + reimplemented slice shadows
//
// Revision 1.5  2006/07/07 13:44:52  geconomos
// + update for changes to vxRenderingEngineRenderer
// + working on rendering loop improve image quality (missing chuncks from image)
//
// Revision 1.4  2006/06/27 20:15:57  geconomos
// + now inheriting from vxRenderingEngineRenderer
// + oblique rendering proof of concept
//
// Revision 1.2  2006/06/21 18:47:01  geconomos
// added vxRayGenerator instance pointer
//
// Revision 1.1  2006/06/20 20:06:09  geconomos
// refactored vxSoftwareRendererCpu3D
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRaytracer.C,v 1.26.2.2 2008/09/11 20:09:01 dongqing Exp $
// $Id: vxRaytracer.C,v 1.26.2.2 2008/09/11 20:09:01 dongqing Exp $
