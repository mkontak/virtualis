// $Id: VolumeRender.C,v 1.19.2.13 2010/12/22 13:02:17 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.net)


// includes
#include "stdafx.h"
#include <stdio.h>
#include <memory.h>
#include "VolumeRender.h"
#include "ReaderGlobal.h"
#include "SystemInfo.h"
#include "VolumeRender.inl"
#include "VCSettings.h"


// pragmas
#pragma warning (disable : 4244)


// defines
#define FILE_REVISION "$Revision: 1.19.2.13 $"


// namespaces
using namespace ReaderLib;


// static initializations
const float4 VolumeRender::m_fInterSampleDistance = 0.5F;
const float4 VolumeRender::m_fOpacityThreshold = 0.98F;
const float4 VolumeRender::m_fMinOpacity = 0.0F;
const float4 VolumeRender::m_fKA = 0.20F;
const float4 VolumeRender::m_fKD = 0.60F;
const float4 VolumeRender::m_fKS = 0.50F;
const float4 VolumeRender::m_fSpecularPower = 101.0F;
const float4 VolumeRender::m_fHalfShadowThickness = 0.5f;
const float4 VolumeRender::m_fDepthVisibilityThresh = 60.0F;
const uint4  VolumeRender::m_uColor_Threshold = 30;
const float4 VolumeRender::m_fDepth_Threshold = 3.0;
float4 VolumeRender::m_fFovDegrees( 90 );
int4 VolumeRender::m_iMinRenderSize( 400 );
int4 VolumeRender::m_iMaxRenderSize( 512 );
float4 VolumeRender::m_fMinFrameRate( 10 );
float4 VolumeRender::m_fMaxFrameRate( 16 );


/**
 * Constructor.
 */
VolumeRender::VolumeRender()
{
  m_puImageBuffer = NULL;
  m_pfDepthBuffer = NULL;
  m_puAccumulationBuffer = NULL;
  Reset();

} // VolumeRender()


/**
 * Destructor.
 */
VolumeRender::~VolumeRender()
{
  // end render threads (auto-delete their own memory)
  for( int4 i( 0 ); i < m_threads.size(); ++i )
    delete m_threads[ i ];
  m_threads.clear();

  if (m_puImageBuffer != NULL)
  {
    delete [] m_puImageBuffer;
    m_puImageBuffer = NULL;
  } // if m_imageBuffer allocated

  if (m_pfDepthBuffer != NULL)
  {
    delete [] m_pfDepthBuffer;
    m_pfDepthBuffer = NULL;
  } // if m_depthBuffer

  if (m_puAccumulationBuffer != NULL)
  {
    delete [] m_puAccumulationBuffer;
    m_puAccumulationBuffer = NULL;
  } // if m_pAccumulationBuffer

} // ~VolumeRender()


/**
 * Resets all variables that change whenever a new dataset is loaded.
 */
void VolumeRender::Reset()
{

  m_fFovDegrees = rdrGlobal.GetSettings()->GetFloatValue(VCSettings::ViewerXML(), VCSettings::FieldOfView3D());
  m_iMinRenderSize = rdrGlobal.GetSettings()->GetIntValue(VCSettings::ViewerXML(), VCSettings::MinRenderSize());
  m_iMaxRenderSize = rdrGlobal.GetSettings()->GetIntValue(VCSettings::ViewerXML(), VCSettings::MaxRenderSize());
  m_fMinFrameRate = rdrGlobal.GetSettings()->GetIntValue(VCSettings::ViewerXML(), VCSettings::MinFrameRate());
  m_fMaxFrameRate = rdrGlobal.GetSettings()->GetFloatValue(VCSettings::ViewerXML(), VCSettings::MaxFrameRate());

  Resize(Triple<uint2>(2,2,2));
  m_uRayCount = 0;
  m_uSampleCount = 0;
	m_uSpaceLeapCount = 0;
  m_fSumSpaceLeapDistance = 0;
  m_fMaxSpaceLeapDistance = 0.0F;
  m_fFramesPerSecond = 0.0F;
  m_uRenderSizeSum = 0;
  m_uRenderSizeCount = 1;
  m_fLastRenderTime = 0.0F;


  m_fMoveLightVertDirection = 0.0;
  m_fMoveLightHorizDirection = 0.0;
  
  m_eProjectionType = VolumeRender::PERSPECTIVE;
  m_eImageMode = VolumeRender::ADAPTIVE;

  m_magicWindowStartPt = Point<int4>(0,0,0);
  m_uMagicWindowSize = 0;

  m_uRenderSize = 1;
  m_uRenderBufferSize = 2;
  m_fRenderBufferRatio = float4(m_uRenderSize)/float4(m_uRenderBufferSize);
  m_uDisplaySize = 1;

  m_iAccumulationCount = -1; // -1 indicates needs clear
  
 
  m_pDiffVolume = NULL;
  m_pDiffVolumeIterator = NULL;
  m_pDiffVolumeBackground = 0;

  if (m_puImageBuffer != NULL)
  {
    delete [] m_puImageBuffer;
    m_puImageBuffer = new uint1[3*m_uRenderBufferSize*m_uRenderBufferSize];
  } // if m_imageBuffer was active

  if (m_pfDepthBuffer != NULL)
  {
    delete [] m_pfDepthBuffer;
    m_pfDepthBuffer = new float4[m_uRenderBufferSize*m_uRenderBufferSize];
  } // if m_depthBuffer was active

  if (m_puAccumulationBuffer != NULL)
  {
    delete [] m_puAccumulationBuffer;
    m_puAccumulationBuffer = new uint2[3*m_uRenderBufferSize*m_uRenderBufferSize];
  } // if m_pAccumulationBuffer was active

} // Reset()


void VolumeRender::PreLaunchRenderThreads( Dataset* dataset )
{
  // maximum number of threads
  const uint8 MAX_THREADS( rdrGlobal.GetSettings()->GetIntValue(VCSettings::ViewerXML(), VCSettings::MaxRenderThreads()) );
  
  // create the render threads
  //uint8 uNumberOfThreads( min( MAX_THREADS, SystemInfo::GetCpuInfo().GetTotalThreads() ) );
  uint8 uNumberOfThreads( MAX_THREADS );
  if( uNumberOfThreads > 1 )
  {
    for( uint4 i ( 0 ); i < uNumberOfThreads; ++i )
      m_threads.push_back( new RenderThread(*this, *dataset, i ) );
    m_surfaceCounts.resize( uNumberOfThreads , 0 );
  }
  else
  {
    m_surfaceCounts.resize( 1, 0 );
  }
} // PreLaunchRenderThreads()


/**
 * Read sets up some things after the Volume::Read.
 *
 * @param   sFilename   The root filename for reading.
 * @return  true if read succeeds.
 */
bool VolumeRender::ReadFile(const std::string & sFilename)
{
  try
  {
    Read(sFilename);
  }
  catch (ex::IOException)
  {
    return false;
  }

  return true;
} // ReadFile()


/**
 * Get a sample from the original volume data (before e-cleansing).
 *
 * @param point volume coordinate.
 * @return the sampled value.
 */
float4 VolumeRender::GetUncleansedResampledValue(const Point<float4> & point) const
{
  // truncate
  uint4 newx = uint4(point.m_x);
  uint4 newy = uint4(point.m_y);
  uint4 newz = uint4(point.m_z);
  
  Point<uint2> p(newx, newy, newz);
  
  // weights for tri-linear interpolation
  float4 fWeightX = point.m_x - newx;
  float4 fWeightY = point.m_y - newy;
  float4 fWeightZ = point.m_z - newz;
  const float4 fOneMinusWeightZ(1 - fWeightZ);
  const float4 fOneMinusWeightY(1 - fWeightY);
  const float4 fOneMinusWeightX(1 - fWeightX);
  
  // eight nearest neighbor voxels
  uint1 llf = (InsideVolume(p) ? GetUncleansedValue(p) : m_pDiffVolumeBackground);  p.m_y++;
  uint1 ulf = (InsideVolume(p) ? GetUncleansedValue(p) : m_pDiffVolumeBackground);  p.m_x++;
  uint1 urf = (InsideVolume(p) ? GetUncleansedValue(p) : m_pDiffVolumeBackground);  p.m_y--;
  uint1 lrf = (InsideVolume(p) ? GetUncleansedValue(p) : m_pDiffVolumeBackground);  p.m_z++;
  uint1 lrb = (InsideVolume(p) ? GetUncleansedValue(p) : m_pDiffVolumeBackground);  p.m_y++;
  uint1 urb = (InsideVolume(p) ? GetUncleansedValue(p) : m_pDiffVolumeBackground);  p.m_x--;
  uint1 ulb = (InsideVolume(p) ? GetUncleansedValue(p) : m_pDiffVolumeBackground);  p.m_y--;
  uint1 llb = (InsideVolume(p) ? GetUncleansedValue(p) : m_pDiffVolumeBackground);
  
  // tri-linear interpolation
  const float4 ll = llb * fWeightZ + llf * fOneMinusWeightZ;
  const float4 ul = ulb * fWeightZ + ulf * fOneMinusWeightZ;
  const float4 lr = lrb * fWeightZ + lrf * fOneMinusWeightZ;
  const float4 ur = urb * fWeightZ + urf * fOneMinusWeightZ;
  const float4 l  = ul  * fWeightY + ll  * fOneMinusWeightY;
  const float4 r  = ur  * fWeightY + lr  * fOneMinusWeightY;
  return           (r   * fWeightX + l   * fOneMinusWeightX);
} // GetUncleansedResampledValue()


/**
 * Setup the link for uncleansed rendering
 *
 * @param   pDiffVolume   The ecv-vol difference volume.
 */
void VolumeRender::LinkToDiffVolume( vxBlockVolume<uint1> * pDiffVolume )
{

  if ( pDiffVolume == NULL )
  {
    throw ex::IllegalArgumentException( LogRec( "Difference volume is null", "VolumeRender", "LinkToDiffVolume" ) );
  }

  // warn if called more than once
  if ( m_pDiffVolume != NULL )
  {
    LogWrn( "The difference volume was set more than once! This should not happen.", "VolumeRender", "LinkToDiffVolume" );
  }

  // set values
  m_pDiffVolume = pDiffVolume;
  m_pDiffVolumeIterator = new vxBlockVolumeIterator<uint1>( * m_pDiffVolume );
  m_pDiffVolumeBackground = m_pDiffVolume->GetHeader().GetBackground();

} // LinkToDiffVolume()


/**
 * Updates the number of rays to be sent AND the (power of 2) size of the buffer
 *
 * @param   uRenderSize   The size in rays.
 */
void VolumeRender::SetRenderSize(const uint4 uRenderSize)
{
  uint4 uOldRenderBufferSize = m_uRenderBufferSize;
  
  // ensure there are enough pixels for the threads to render properly
  switch ( GetNumberOfRenderThreads() )
  {
  case 2:
  case 4:
    m_uRenderSize = ( uRenderSize / 4 ) * 4 + 1;
  	break;
  case 8:
  case 16:
    m_uRenderSize = ( uRenderSize / 8 ) * 8 + 1;
  	break;
  case 32:
    m_uRenderSize = ( uRenderSize / 16 ) * 16 + 1;
  	break;
  default:
    m_uRenderSize = uRenderSize;
  	break;
  }
  m_uRenderBufferSize = 2; while(m_uRenderBufferSize<m_uRenderSize) m_uRenderBufferSize*=2;
  m_fRenderBufferRatio = float4(m_uRenderSize-1)/float4(m_uRenderBufferSize);
  
  if(m_uRenderBufferSize != uOldRenderBufferSize)
  {
    if (m_puImageBuffer != NULL)
    {
      delete [] m_puImageBuffer;
    } // if m_imageBuffer allocated
    m_puImageBuffer = new uint1[3*m_uRenderBufferSize*m_uRenderBufferSize];
    
    if (m_pfDepthBuffer != NULL)
    {
      delete [] m_pfDepthBuffer;
    } // if m_depthBuffer allocated
    m_pfDepthBuffer = new float4[m_uRenderBufferSize*m_uRenderBufferSize];
    
    if (m_puAccumulationBuffer != NULL)
    {
      delete [] m_puAccumulationBuffer;
    } // if m_pAccumulationBuffer allocated
    m_puAccumulationBuffer = new uint2[3*m_uRenderBufferSize*m_uRenderBufferSize];
    
    if (m_puImageBuffer == NULL || m_pfDepthBuffer == NULL || m_puAccumulationBuffer == NULL)
    {
      // Error::Abort("Out of memory");
      throw ex::OutOfMemoryException(LogRec("Out of memory", "VolumeRender", "SetRenderSize"));
    }
  } // endif buffer size changed also
  
  m_magicWindowStartPt = Point<int4>(m_uRenderSize/5, m_uRenderSize/5, 0);
  m_uMagicWindowSize = m_uRenderSize*3/5;
  m_iAccumulationCount = -1; // -1 indicates needs clear
} // SetRenderSize()


/**
 * Move the translucent magic window around the screen.
 *
 * @param   uDeltaX   amount the cursor moved acrss the screen (pixels).
 * @param   uDeltaY   amount the cursor moved acrss the screen (pixels).
 */
void VolumeRender::MoveWindow(const int4 iDeltaX, const int4 iDeltaY)
{
  m_magicWindowStartPt.m_x += iDeltaX*int(m_uRenderSize)/int(m_uDisplaySize);
  m_magicWindowStartPt.m_y += iDeltaY*int(m_uRenderSize)/int(m_uDisplaySize);
  m_magicWindowStartPt.Clamp(0, m_uRenderSize-m_uMagicWindowSize-1);
} // MoveWindow()


/**
 * Create an image.
 *
 * The function handles the non-raycasting portions of getting the image on the screen.
 * These include OpenGL, Accumulation and the multi-threading for faster speed. The
 * actual raycasting occurs in the RayCast function.
 *
 * @param dataset the current dataset
 * @param iMaxWindowSize
 * @bInverseImg indicator for inversing image
 */
void VolumeRender::Render(Dataset & dataset, int iMaxWindowSize, bool bInverseImg)
{
	if (!dataset.IsLoadingComplete())
		return;

  // Calculate the frames per second (time from last cycle)
  float4 fSecondsPerFrame = m_frameRateTimer.ElapsedTime();
  m_frameRateTimer.Reset();
  m_fFramesPerSecond = ((fSecondsPerFrame > 0.0F) ? 1.0F/fSecondsPerFrame : 0.0F);
  
  float4 fFramesPerSecondRenderingOnly( ((m_fLastRenderTime > 0.0F) ? 1.0F/m_fLastRenderTime : 0.0F) );
  
  // flush image if the view has changed at all
  if (dataset.m_submarine.IsFlying() ||
      m_uAccumulationViewIdentifier != dataset.m_submarine.GetViewIdentifier())
  {
    m_uAccumulationViewIdentifier = dataset.m_submarine.GetViewIdentifier();
    FlushAccumulationBuffer();

    if( m_eImageMode == VolumeRender::ADAPTIVE )
    {

      // rendering to slow and the image size is greater than the specified minimum
      uint4 uDesiredSize( GetRenderSize() );
      if(fFramesPerSecondRenderingOnly < VolumeRender::m_fMinFrameRate )
      {
        uDesiredSize = sqrt( fFramesPerSecondRenderingOnly / VolumeRender::m_fMinFrameRate ) * GetRenderSize();  
        uDesiredSize = max( VolumeRender::m_iMinRenderSize, uDesiredSize );
      }
      
      if(fFramesPerSecondRenderingOnly > VolumeRender::m_fMaxFrameRate && GetRenderSize() <= (VolumeRender::m_iMaxRenderSize)) 
      {
        uDesiredSize = sqrt( fFramesPerSecondRenderingOnly / VolumeRender::m_fMaxFrameRate ) * GetRenderSize();  
        uDesiredSize = min( VolumeRender::m_iMaxRenderSize, uDesiredSize );
      }
      if( abs( (long) (uDesiredSize - GetRenderSize()) ) > 4 )
        SetRenderSize( uDesiredSize );
    }    
  }

  // Either render the view again during accumulation or
  // display the accumulation buffer.
  const int4 iMaxAccumulations = 30;
  const Point<float4> flc_vViewPoint(dataset.m_submarine.GetPositionVolume());  
  if (flc_vViewPoint.m_x > 1 && flc_vViewPoint.m_x < GetDim().m_x - 2 &&  
      flc_vViewPoint.m_y > 1 && flc_vViewPoint.m_y < GetDim().m_y - 2 &&  
      flc_vViewPoint.m_z > 1 && flc_vViewPoint.m_z < GetDim().m_z - 2 &&  
      m_iAccumulationCount < iMaxAccumulations-1)
  {
    // Render a new view
    m_uRenderSizeCount++;
    m_uRenderSizeSum += GetRenderSize();
    
    // Per-frame setup
    m_jitter.SwapBuffers();
       
    // reset the surface counts
    for( int i( 0); i < m_surfaceCounts.size(); ++i )
      m_surfaceCounts[ i ] = 0;
    
    // are we using threads for rendering?
    uint4 uNumThreads( m_threads.size() );
    if( uNumThreads > 0 )
    {
      const uint4 uWidth( m_uRenderSize );
      const uint4 uWidthOverTwo( m_uRenderSize * 0.5F );
      const uint4 uWidthOverFour( m_uRenderSize * 0.25F );
      const uint4 uThreeWidthOverFour( m_uRenderSize * 0.75F );
      const uint4 uW8_1( m_uRenderSize * 0.125F );
      const uint4 uW8_3( m_uRenderSize * 0.125F * 3.0F );
      const uint4 uW8_5( m_uRenderSize * 0.125F * 5.0F );
      const uint4 uW8_7( m_uRenderSize * 0.125F * 7.0F );

      const uint4 uHeight( m_uRenderSize );
      const uint4 uHeightOverTwo( m_uRenderSize * 0.5F );
      const uint4 uHeightOverFour( m_uRenderSize * 0.25F );
      const uint4 uThreeHeightOverFour( m_uRenderSize * 0.75F );

      if( uNumThreads == 2 )
      {
        m_threads[ 0 ]->SetRenderingBounds( Rect< int4 >( 0,    0,               uWidth,  uHeightOverTwo ) );
        m_threads[ 1 ]->SetRenderingBounds( Rect< int4 >( 0,    uHeightOverTwo,  uWidth,  uHeight        ) );
      }      
      else if( uNumThreads == 4 )
      {
        m_threads[ 0 ]->SetRenderingBounds( Rect< int4 >( 0,              0,                uWidthOverTwo,    uHeightOverTwo ) );
        m_threads[ 1 ]->SetRenderingBounds( Rect< int4 >( uWidthOverTwo,  0,                uWidth,           uHeightOverTwo ) );
        m_threads[ 2 ]->SetRenderingBounds( Rect< int4 >( 0,              uHeightOverTwo,   uWidthOverTwo,    uHeight        ) );
        m_threads[ 3 ]->SetRenderingBounds( Rect< int4 >( uWidthOverTwo,  uHeightOverTwo,   uWidth,           uHeight        ) );
      }
      else if( uNumThreads == 8 )
      {
        m_threads[ 0 ]->SetRenderingBounds( Rect< int4 >( 0,                      0,                uWidthOverFour,        uHeightOverTwo ) );
        m_threads[ 1 ]->SetRenderingBounds( Rect< int4 >( uWidthOverFour,         0,                uWidthOverTwo,         uHeightOverTwo ) );
        m_threads[ 2 ]->SetRenderingBounds( Rect< int4 >( uWidthOverTwo,          0,                uThreeWidthOverFour,   uHeightOverTwo ) );
        m_threads[ 3 ]->SetRenderingBounds( Rect< int4 >( uThreeWidthOverFour,    0,                uWidth,                uHeightOverTwo ) );
        m_threads[ 4 ]->SetRenderingBounds( Rect< int4 >( 0,                      uHeightOverTwo,   uWidthOverFour,        uHeight        ) );
        m_threads[ 5 ]->SetRenderingBounds( Rect< int4 >( uWidthOverFour,         uHeightOverTwo,   uWidthOverTwo,         uHeight        ) );
        m_threads[ 6 ]->SetRenderingBounds( Rect< int4 >( uWidthOverTwo,          uHeightOverTwo,   uThreeWidthOverFour,   uHeight        ) );
        m_threads[ 7 ]->SetRenderingBounds( Rect< int4 >( uThreeWidthOverFour,    uHeightOverTwo,   uWidth,                uHeight        ) );
      }
      else if( uNumThreads == 16 )
      {
        m_threads[  0 ]->SetRenderingBounds( Rect< int4 >( 0,                      0,                uWidthOverFour,        uHeightOverFour ) );
        m_threads[  1 ]->SetRenderingBounds( Rect< int4 >( uWidthOverFour,         0,                uWidthOverTwo,         uHeightOverFour ) );
        m_threads[  2 ]->SetRenderingBounds( Rect< int4 >( uWidthOverTwo,          0,                uThreeWidthOverFour,   uHeightOverFour ) );
        m_threads[  3 ]->SetRenderingBounds( Rect< int4 >( uThreeWidthOverFour,    0,                uWidth,                uHeightOverFour ) );
        m_threads[  4 ]->SetRenderingBounds( Rect< int4 >( 0,                      uHeightOverFour,  uWidthOverFour,        uHeightOverTwo  ) );
        m_threads[  5 ]->SetRenderingBounds( Rect< int4 >( uWidthOverFour,         uHeightOverFour,  uWidthOverTwo,         uHeightOverTwo  ) );
        m_threads[  6 ]->SetRenderingBounds( Rect< int4 >( uWidthOverTwo,          uHeightOverFour,  uThreeWidthOverFour,   uHeightOverTwo  ) );
        m_threads[  7 ]->SetRenderingBounds( Rect< int4 >( uThreeWidthOverFour,    uHeightOverFour,  uWidth,                uHeightOverTwo  ) );
        m_threads[  8 ]->SetRenderingBounds( Rect< int4 >( 0,                      uHeightOverTwo,   uWidthOverFour,        uThreeHeightOverFour ) );
        m_threads[  9 ]->SetRenderingBounds( Rect< int4 >( uWidthOverFour,         uHeightOverTwo,   uWidthOverTwo,         uThreeHeightOverFour ) );
        m_threads[ 10 ]->SetRenderingBounds( Rect< int4 >( uWidthOverTwo,          uHeightOverTwo,   uThreeWidthOverFour,   uThreeHeightOverFour ) );
        m_threads[ 11 ]->SetRenderingBounds( Rect< int4 >( uThreeWidthOverFour,    uHeightOverTwo,   uWidth,                uThreeHeightOverFour ) );
        m_threads[ 12 ]->SetRenderingBounds( Rect< int4 >( 0,                      uThreeHeightOverFour,   uWidthOverFour,        uHeight        ) );
        m_threads[ 13 ]->SetRenderingBounds( Rect< int4 >( uWidthOverFour,         uThreeHeightOverFour,   uWidthOverTwo,         uHeight        ) );
        m_threads[ 14 ]->SetRenderingBounds( Rect< int4 >( uWidthOverTwo,          uThreeHeightOverFour,   uThreeWidthOverFour,   uHeight        ) );
        m_threads[ 15 ]->SetRenderingBounds( Rect< int4 >( uThreeWidthOverFour,    uThreeHeightOverFour,   uWidth,                uHeight        ) );
      }
      else if( uNumThreads == 32 )
      { 
        m_threads[  0 ]->SetRenderingBounds( Rect< int4 >( 0,                      0,                uW8_1,                 uHeightOverFour ) );
        m_threads[  1 ]->SetRenderingBounds( Rect< int4 >( uW8_1,                  0,                uWidthOverFour,        uHeightOverFour ) );
        m_threads[  2 ]->SetRenderingBounds( Rect< int4 >( uWidthOverFour,         0,                uW8_3,                 uHeightOverFour ) );
        m_threads[  3 ]->SetRenderingBounds( Rect< int4 >( uW8_3,                  0,                uWidthOverTwo,         uHeightOverFour ) );
        m_threads[  4 ]->SetRenderingBounds( Rect< int4 >( uWidthOverTwo,          0,                uW8_5,                 uHeightOverFour ) );
        m_threads[  5 ]->SetRenderingBounds( Rect< int4 >( uW8_5,                  0,                uThreeWidthOverFour,   uHeightOverFour ) );
        m_threads[  6 ]->SetRenderingBounds( Rect< int4 >( uThreeWidthOverFour,    0,                uW8_7,                 uHeightOverFour ) );
        m_threads[  7 ]->SetRenderingBounds( Rect< int4 >( uW8_7,                  0,                uWidth,                uHeightOverFour ) );
        m_threads[  8 ]->SetRenderingBounds( Rect< int4 >( 0,                      uHeightOverFour,  uW8_1,                 uHeightOverTwo ) );
        m_threads[  9 ]->SetRenderingBounds( Rect< int4 >( uW8_1,                  uHeightOverFour,  uWidthOverFour,        uHeightOverTwo ) );
        m_threads[ 10 ]->SetRenderingBounds( Rect< int4 >( uWidthOverFour,         uHeightOverFour,  uW8_3,                 uHeightOverTwo ) );
        m_threads[ 11 ]->SetRenderingBounds( Rect< int4 >( uW8_3,                  uHeightOverFour,  uWidthOverTwo,         uHeightOverTwo ) );
        m_threads[ 12 ]->SetRenderingBounds( Rect< int4 >( uWidthOverTwo,          uHeightOverFour,  uW8_5,                 uHeightOverTwo ) );
        m_threads[ 13 ]->SetRenderingBounds( Rect< int4 >( uW8_5,                  uHeightOverFour,  uThreeWidthOverFour,   uHeightOverTwo ) );
        m_threads[ 14 ]->SetRenderingBounds( Rect< int4 >( uThreeWidthOverFour,    uHeightOverFour,  uW8_7,                 uHeightOverTwo ) );
        m_threads[ 15 ]->SetRenderingBounds( Rect< int4 >( uW8_7,                  uHeightOverFour,  uWidth,                uHeightOverTwo ) );
        m_threads[ 16 ]->SetRenderingBounds( Rect< int4 >( 0,                      uHeightOverTwo,   uW8_1,                 uThreeHeightOverFour ) );
        m_threads[ 17 ]->SetRenderingBounds( Rect< int4 >( uW8_1,                  uHeightOverTwo,   uWidthOverFour,        uThreeHeightOverFour ) );
        m_threads[ 18 ]->SetRenderingBounds( Rect< int4 >( uWidthOverFour,         uHeightOverTwo,   uW8_3,                 uThreeHeightOverFour ) );
        m_threads[ 19 ]->SetRenderingBounds( Rect< int4 >( uW8_3,                  uHeightOverTwo,   uWidthOverTwo,         uThreeHeightOverFour ) );
        m_threads[ 20 ]->SetRenderingBounds( Rect< int4 >( uWidthOverTwo,          uHeightOverTwo,   uW8_5,                 uThreeHeightOverFour ) );
        m_threads[ 21 ]->SetRenderingBounds( Rect< int4 >( uW8_5,                  uHeightOverTwo,   uThreeWidthOverFour,   uThreeHeightOverFour ) );
        m_threads[ 22 ]->SetRenderingBounds( Rect< int4 >( uThreeWidthOverFour,    uHeightOverTwo,   uW8_7,                 uThreeHeightOverFour ) );
        m_threads[ 23 ]->SetRenderingBounds( Rect< int4 >( uW8_7,                  uHeightOverTwo,   uWidth,                uThreeHeightOverFour ) );
        m_threads[ 24 ]->SetRenderingBounds( Rect< int4 >( 0,                      uThreeHeightOverFour,  uW8_1,                 uHeight ) );
        m_threads[ 25 ]->SetRenderingBounds( Rect< int4 >( uW8_1,                  uThreeHeightOverFour,  uWidthOverFour,        uHeight ) );
        m_threads[ 26 ]->SetRenderingBounds( Rect< int4 >( uWidthOverFour,         uThreeHeightOverFour,  uW8_3,                 uHeight ) );
        m_threads[ 27 ]->SetRenderingBounds( Rect< int4 >( uW8_3,                  uThreeHeightOverFour,  uWidthOverTwo,         uHeight ) );
        m_threads[ 28 ]->SetRenderingBounds( Rect< int4 >( uWidthOverTwo,          uThreeHeightOverFour,  uW8_5,                 uHeight ) );
        m_threads[ 29 ]->SetRenderingBounds( Rect< int4 >( uW8_5,                  uThreeHeightOverFour,  uThreeWidthOverFour,   uHeight ) );
        m_threads[ 30 ]->SetRenderingBounds( Rect< int4 >( uThreeWidthOverFour,    uThreeHeightOverFour,  uW8_7,                 uHeight ) );
        m_threads[ 31 ]->SetRenderingBounds( Rect< int4 >( uW8_7,                  uThreeHeightOverFour,  uWidth,                uHeight ) );
      }

      // start the rendering threads
      for( int i( 0 ); i < m_threads.size(); ++i )
        m_threads[ i ]->StartRender();

      // wait for all threads to complete
      for( int i( 0 ); i < m_threads.size(); ++i )
        m_threads[ i ]->WaitForRender();
    }
    else
    {
      // not threading
      Raycast1(-1, dataset);
    }

    // add all the surface counts to the paint volume
    for( int i( 0 ); i < m_surfaceCounts.size(); ++i )
      dataset.m_fields.m_paintVol.m_uCurrentSurfaceCount += m_surfaceCounts[ i ];

    // if we have stopped flying, or the view is the same as before
    if (!dataset.m_submarine.IsFlying() &&
        m_uAccumulationViewIdentifier == dataset.m_submarine.GetViewIdentifier())
    {
      // Progressive refinement: accumulate image if we have stopped flying
      // see if we need to clear the accumulation buffer
      if (m_iAccumulationCount == -1)
      {
        m_iAccumulationCount = 0;
        // clear accumulation buffer
        memset((void*)m_puAccumulationBuffer, 0, 3*m_uRenderBufferSize*m_uRenderBufferSize*sizeof(short));
        m_uAccumulationViewIdentifier = dataset.m_submarine.GetViewIdentifier();
      } // need to clear buffer
      
      if (m_iAccumulationCount < iMaxAccumulations)
      {
        m_iAccumulationCount++;
        const float4 fOneOverAccumulationCount = 1.0F / m_iAccumulationCount;
        for (uint4 y=0; y<m_uRenderSize; y++)
        {
          for (uint4 x=0; x<m_uRenderSize; x++)
          {
            uint4 i = (y*m_uRenderBufferSize+x)*3;
            // accumulate current buffer into accumulation buffer
            m_puAccumulationBuffer[i] += m_puImageBuffer[i];
            // copy accumulated image back into image buffer
            m_puImageBuffer[i] = m_puAccumulationBuffer[i] * fOneOverAccumulationCount;
            // accumulate current buffer into accumulation buffer
            m_puAccumulationBuffer[i+1] += m_puImageBuffer[i+1];
            // copy accumulated image back into image buffer
            m_puImageBuffer[i+1] = m_puAccumulationBuffer[i+1] * fOneOverAccumulationCount;
            // accumulate current buffer into accumulation buffer
            m_puAccumulationBuffer[i+2] += m_puImageBuffer[i+2];
            // copy accumulated image back into image buffer
            m_puImageBuffer[i+2] = m_puAccumulationBuffer[i+2] * fOneOverAccumulationCount;
          } // for x
        } // for y
      } // too many accumulations
    } // if stopped flying
      
  }
  else
  {
    // copy the accumulation buffer into the image buffer
    const float4 fOneOverAccumulationCount = 1.0F / m_iAccumulationCount;
    for (uint4 y=0; y<m_uRenderSize; y++)
    {
      for (uint4 x=0; x<m_uRenderSize; x++)
      {
        uint4 i = (y*m_uRenderBufferSize+x)*3;
        // copy accumulated image back into image buffer
        m_puImageBuffer[i  ] = m_puAccumulationBuffer[i  ] * fOneOverAccumulationCount;
        m_puImageBuffer[i+1] = m_puAccumulationBuffer[i+1] * fOneOverAccumulationCount;
        m_puImageBuffer[i+2] = m_puAccumulationBuffer[i+2] * fOneOverAccumulationCount;
      } // for x
    } // for y
  } // if we need to render from the volume or from the accumulation buffer

  // if we need to invert the image (e.g., when taking a snapshot)
  if (bInverseImg)
  {
    for (uint4 yy=0; yy<m_uRenderSize; yy++)
    {
      for (uint4 xx=0; xx<m_uRenderSize; xx++)
      {
        uint4 i = (yy * m_uRenderBufferSize + xx) * 3;
        uint1 temp = m_puImageBuffer[i];
        m_puImageBuffer[i] = m_puImageBuffer[i+2];
        m_puImageBuffer[i+2] = temp;
      } // for xx
    } // for yy
  } // end-if (bInverseImage)
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  {  
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    {
      glDisable(GL_LIGHTING);
      glPixelStorei(GL_UNPACK_ALIGNMENT,1);
      glPixelStorei(GL_UNPACK_ROW_LENGTH, m_uRenderBufferSize);
      
      glLoadIdentity();
      glOrtho(0, 1, 0, 1, 0, -1);
      
      glBindTexture( GL_TEXTURE_2D, 0 );
      glTexImage2D (GL_TEXTURE_2D, 0, 3, m_uRenderBufferSize, m_uRenderBufferSize, 0, GL_RGB, GL_UNSIGNED_BYTE, m_puImageBuffer);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
      glEnable(GL_TEXTURE_2D);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_ALPHA_TEST);
      glBegin(GL_QUADS);
      {
        const float4 fStart = 1.0F / m_uRenderBufferSize;
        glTexCoord2d(fStart              , fStart              ); glVertex3f(0, 0, 0);
        glTexCoord2d(fStart              , m_fRenderBufferRatio); glVertex3f(0, 1, 0);
        glTexCoord2d(m_fRenderBufferRatio, m_fRenderBufferRatio); glVertex3f(1, 1, 0);
        glTexCoord2d(m_fRenderBufferRatio, fStart              ); glVertex3f(1, 0, 0);
      } glEnd();
      glDisable(GL_TEXTURE_2D);
      
    } glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
  } glPopMatrix();

} // Render()

/**
 * Create an image.
 *
 * The function handles the non-raycasting portions of getting the image on the screen.
 * These include OpenGL, Accumulation and the multi-threading for faster speed. The
 * actual raycasting occurs in the RayCast function.
 */
void VolumeRender::RenderIntoBitmap(Dataset & dataset)
{
	if (!dataset.IsLoadingComplete())
		return;
  
  if (SystemInfo::GetCpuInfo().GetTotalThreads() > 1)
  {
    for( int i( 0 ); i < m_threads.size(); ++i )
      m_threads[ i ]->StartRender();

    for( int i( 0 ); i < m_threads.size(); ++i )
      m_threads[ i ]->WaitForRender();
  }
  else
  {
    Raycast1(-1, dataset);
  } // endif parallel or not

} // RenderIntoBitmap()

#define CASTONERAY                                                                                                                                \
        float4 fLeapDistance = fFirstLeapDistance;                                                                                                \
                                                                                                                                                  \
        if(m_eProjectionType == VolumeRender::PERSPECTIVE )                                                                                                     \
        {                                                                                                                                         \
          vRayNormal = Normal<float4>( vRowVector + flc_vRightVector * fDu * flc_fTanFov);                                                        \
          vLight1Normal = Normal<float4>( Vector<float4>(vRayNormal) + vMoveLightVector);                                                         \
          vPoint = flc_vViewPoint;                                                                                                                \
        }                                                                                                                                         \
        else                                                                                                                                      \
        {                                                                                                                                         \
          SetupAlternativeProjection(u, v, fDu, fDv, flc_vViewPoint, flc_vViewVector,                                                             \
                                     flc_vUpVector, flc_vRightVector, flc_vDownVector, flc_fFovRadians,                                           \
                                     vRayNormal, vLight1Normal, vLight2Normal,                                                                    \
                                     vPoint, fLeapDistance);                                                                                      \
        }                                                                                                                                         \
                                                                                                                                                  \
        float4 fRedValue = 0.0F, fGreenValue = 0.0F, fBlueValue = 0.0F, fAlphaValue = 0.0F, fDepthValue = 0.0F;                                   \
                                                                                                                                                  \
        CastSingleRay(u, v,                                                                                                                       \
                      flc_bObliqueShadow, flc_bGridShadow, flc_bAxialShadow, flc_bSagittalShadow, flc_bCoronalShadow, flc_bSegmentHalfPlaneShadow,\
                      flc_bShadow, flc_bBiopsyMode, flc_bTestVol, flc_bRaycastPaintVol, flc_bSpaceLeaping, flc_bUncleansed,                       \
                      flc_vObliqueNormal, flc_fObliqueMinDistance, flc_fObliqueMaxDistance, flc_vOrthogonalPoint,                                 \
                      vRayNormal, vLight1Normal, vLight2Normal,                                                                                   \
                      vPoint,                                                                                                                     \
                      fLeapDistance,                                                                                                              \
                      flc_uXStep, flc_uYStep, flc_uZStep, flc_uXYStep, flc_uYZStep, flc_uXZStep, flc_uXYZStep,                                    \
                      flc_uXStep3, flc_uYStep3, flc_uZStep3, flc_uXYStep3, flc_uYZStep3, flc_uXZStep3, flc_uXYZStep3,                             \
                      flc_fXScl, flc_fYScl, flc_fZScl, flc_uVolMaxX, flc_uVolMaxY, flc_uVolMaxZ,                                                  \
                      flc_puECVvol,                                                                                                               \
                      flc_puSLDvol,                                                                                                               \
                      flc_puTestVol,                                                                                                              \
                      flc_puPaintVol, flc_uCurrentSeenVal,                                                                                        \
                      flc_fNotSeenRed, flc_fNotSeenGreen, flc_fNotSeenBlue,                                                                       \
                      flc_fSeenRed, flc_fSeenGreen, flc_fSeenBlue,                                                                                \
                      flc_pfOpacitySurfaceTransferFunction,                                                                                       \
                      flc_pfRedBiopsyTransferFunction, flc_pfGreenBiopsyTransferFunction,                                                         \
                      flc_pfBlueBiopsyTransferFunction, flc_pfOpacityBiopsyTransferFunction,                                                      \
                      fRedValue, fGreenValue, fBlueValue, fAlphaValue, fDepthValue,                                                               \
                      flc_fDfbScale, flc_fDfbSpaceLeapConversationFactor, flc_fDfbSpaceLeapThreshold,                                             \
                      uSurfaceCount,                                                                                                              \
                      vGradVector,                                                                                                                \
                      vHalfwayVector,                                                                                                             \
                      vIntervalVector);                                                                                                           \
        *(flc_puImageBuffer + uPixelOffset    ) = (unsigned char) fRedValue;                                                                      \
        *(flc_puImageBuffer + uPixelOffset + 1) = (unsigned char) fGreenValue;                                                                    \
        *(flc_puImageBuffer + uPixelOffset + 2) = (unsigned char) fBlueValue;                                                                     \
        *(flc_pfDepthBuffer + uDepthOffset)           = fDepthValue;                                                                              \

#define TESTONEINTERPOLATE(DepthBufferDelta,ImageBufferDelta)                                                                                     \
        float4 fPrevDepth = *(flc_pfDepthBuffer + uDepthOffset - DepthBufferDelta);                                                               \
        float4 fNextDepth = *(flc_pfDepthBuffer + uDepthOffset + DepthBufferDelta);                                                               \
                                                                                                                                                  \
        if ((abs(fPrevDepth-fNextDepth))<m_fDepth_Threshold)                                                                                      \
        {                                                                                                                                         \
          uint1 uPrevRed   =  *(flc_puImageBuffer + uPixelOffset - ImageBufferDelta);                                                             \
          uint1 uNextRed   =  *(flc_puImageBuffer + uPixelOffset + ImageBufferDelta);                                                             \
          uint1 uPrevGreen =  *(flc_puImageBuffer + uPixelOffset - ImageBufferDelta + 1);                                                         \
          uint1 uNextGreen =  *(flc_puImageBuffer + uPixelOffset + ImageBufferDelta + 1);                                                         \
          uint1 uPrevBlue  =  *(flc_puImageBuffer + uPixelOffset - ImageBufferDelta + 2);                                                         \
          uint1 uNextBlue  =  *(flc_puImageBuffer + uPixelOffset + ImageBufferDelta + 2);                                                         \
                                                                                                                                                  \
          if ( ( (abs(uPrevRed  -uNextRed  ))<m_uColor_Threshold ) &&                                                                             \
               ( (abs(uPrevGreen-uNextGreen))<m_uColor_Threshold ) &&                                                                             \
               ( (abs(uPrevBlue -uNextBlue ))<m_uColor_Threshold ) )                                                                              \
          {                                                                                                                                       \
            *(flc_puImageBuffer + uPixelOffset    ) = (unsigned char) (((int)uPrevRed   + uNextRed  ) / 2);                                       \
            *(flc_puImageBuffer + uPixelOffset + 1) = (unsigned char) (((int)uPrevGreen + uNextGreen) / 2);                                       \
            *(flc_puImageBuffer + uPixelOffset + 2) = (unsigned char) (((int)uPrevBlue  + uNextBlue ) / 2);                                       \
                                                                                                                                                  \
            *(flc_pfDepthBuffer + uDepthOffset) = (fPrevDepth + fNextDepth) / 2;                                                                  \
                                                                                                                                                  \
            continue;                                                                                                                             \
          }                                                                                                                                       \
        }                                                                                                                                         \



/**
 * Perspective Endoscopic Raycast (assumes the viewpoint is within the volume).
 *
 * This function does Adaptive-Sampling raycasting of the image plane. The work
 * per ray is performed in the CastSingleRay function.
 *
 * @param   id      Which thread this raycaster is associated with.
 * @param   dataset   dataset to render
 */
void VolumeRender::Raycast2(int iId, Dataset & dataset, const Rect<int4> & renderingBounds )
{
  // initialize performance measures
  //m_rayCount = 0;
  //m_sampleCount = 0;
  //m_spaceLeapCount = 0;
  //m_sumSpaceLeapDistance = 0;
  //m_maxSpaceLeapDistance = 0.0F;

  // Setup these local variables once per frame. "flc_" stands for Fast Local Constant
  // and this way the inner volume rendering loop remains highly optimized.
  const float4 flc_fFovRadians = m_fFovDegrees * M_PI / 180.0F;
  const float4 flc_fTwoOverSize = 2.0f / float4(m_uRenderSize);
  const float4 flc_fTanFov = float4( tan(flc_fFovRadians/2) );
  const uint4 flc_uVolMaxX = GetDim().m_x - 2;
  const uint4 flc_uVolMaxY = GetDim().m_y - 2;
  const uint4 flc_uVolMaxZ = GetDim().m_z - 2;

  const float4 flc_fNotSeenRed   = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vNotSeenScheme2.m_x : TransferFunctionVC::m_vNotSeenScheme1.m_x;
  const float4 flc_fNotSeenGreen = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vNotSeenScheme2.m_y : TransferFunctionVC::m_vNotSeenScheme1.m_y;
  const float4 flc_fNotSeenBlue  = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vNotSeenScheme2.m_z : TransferFunctionVC::m_vNotSeenScheme1.m_z;
  const float4 flc_fSeenRed   = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vSeenScheme2.m_x : TransferFunctionVC::m_vSeenScheme1.m_x;
  const float4 flc_fSeenGreen = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vSeenScheme2.m_y : TransferFunctionVC::m_vSeenScheme1.m_y;
  const float4 flc_fSeenBlue  = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vSeenScheme2.m_z : TransferFunctionVC::m_vSeenScheme1.m_z;
  const float4 * flc_pfOpacitySurfaceTransferFunction = dataset.m_surfaceTransferFunction.m_pfOpacity;
  const float4 * flc_pfRedBiopsyTransferFunction = dataset.m_biopsyTransferFunction.m_pfRed;
  const float4 * flc_pfGreenBiopsyTransferFunction = dataset.m_biopsyTransferFunction.m_pfGreen;
  const float4 * flc_pfBlueBiopsyTransferFunction = dataset.m_biopsyTransferFunction.m_pfBlue;
  const float4 * flc_pfOpacityBiopsyTransferFunction = dataset.m_biopsyTransferFunction.m_pfOpacity;
  
  const bool flc_bIsFlying = rdrGlobal.m_supine.m_submarine.IsFlying() || rdrGlobal.m_prone.m_submarine.IsFlying();

  const bool flc_bAxialShadow = rdrGlobal.m_bAxialShadow & !flc_bIsFlying;
  const bool flc_bSagittalShadow = rdrGlobal.m_bSagittalShadow & !flc_bIsFlying;
  const bool flc_bCoronalShadow = rdrGlobal.m_bCoronalShadow & !flc_bIsFlying;
  const bool flc_bSegmentHalfPlaneShadow = rdrGlobal.m_bSegmentHalfPlaneShadow & !flc_bIsFlying;
  const bool flc_bObliqueShadow = rdrGlobal.m_bObliqueShadow & !flc_bIsFlying;
  const bool flc_bGridShadow = rdrGlobal.m_bGridShadow & !flc_bIsFlying;
  const bool flc_bShadow = flc_bAxialShadow || flc_bSagittalShadow || flc_bCoronalShadow || flc_bSegmentHalfPlaneShadow || flc_bObliqueShadow || flc_bGridShadow;
  const bool flc_bBiopsyMode = rdrGlobal.m_bBiopsyMode;
  const bool flc_bTestVol = rdrGlobal.m_bShowHomogenity;
  const bool flc_bRaycastPaintVol = rdrGlobal.m_bRaycastPaintVol;
	bool flc_bSpaceLeaping = true; //rdrGlobal.m_bSpaceLeaping; This is not necessary since we'll always use space leaping when ULD volume available.
  const bool flc_bUncleansed = (dataset.m_fields.IsDiffVolLoaded() & !rdrGlobal.m_bSpaceLeaping );  //Note, the m_bSpaceLeaping is used as a status flag for <S> key.
	//if( flc_bUncleansed && rdrGlobal.m_bUseULDvolume ) 
	//{                                                  
	//	flc_bSpaceLeaping = true; //:: Use both SLD and ULD all the time, So, the space leaping always needs.
	//}

  const float4 flc_fDfbSpaceLeapConversationFactor = Max(GetUnits().m_x,GetUnits().m_y,GetUnits().m_z) + m_fInterSampleDistance;
  const float4 flc_fDfbSpaceLeapThreshold = flc_fDfbSpaceLeapConversationFactor;

  const uint4 flc_uXStep = 1;
  const uint4 flc_uYStep = GetDim().m_x;
  const uint4 flc_uZStep = GetDim().m_x*GetDim().m_y;
  const uint4 flc_uXYStep = flc_uXStep+flc_uYStep;
  const uint4 flc_uXZStep = flc_uXStep+flc_uZStep;
  const uint4 flc_uYZStep = flc_uYStep+flc_uZStep;
  const uint4 flc_uXYZStep = flc_uXYStep+flc_uZStep;

  const uint4 flc_uXStep3 = 3;
  const uint4 flc_uYStep3 = GetDim().m_x*3;
  const uint4 flc_uZStep3 = GetDim().m_x*GetDim().m_y*3;
  const uint4 flc_uXYStep3 = flc_uXStep3+flc_uYStep3;
  const uint4 flc_uXZStep3 = flc_uXStep3+flc_uZStep3;
  const uint4 flc_uYZStep3 = flc_uYStep3+flc_uZStep3;
  const uint4 flc_uXYZStep3 = flc_uXYStep3+flc_uZStep3;

  const float4 flc_fXScl = 1.0f / GetUnits().m_x;
  const float4 flc_fYScl = 1.0f / GetUnits().m_y;
  const float4 flc_fZScl = 1.0f / GetUnits().m_z;

  const uint1 * const flc_puECVvol = (unsigned char *) GetDataPtr();
  uint1 * flc_puSLDvol = (unsigned char *) dataset.m_fields.m_sldInterior.GetDataPtr();
	if( flc_bUncleansed && rdrGlobal.m_bUseULDvolume )
  {
    flc_puSLDvol = (unsigned char *) dataset.m_fields.m_sldExterior.GetDataPtr();  //:: Use this for un-cleansed SLD, i.e. the ULD volume
  } // if
  const uint1 * const flc_puTestVol = (unsigned char *) dataset.m_fields.m_testVolumeUC.GetDataPtr();

  const uint1 flc_uCurrentSeenVal = dataset.m_fields.m_paintVol.m_uCurrentSeenVal;

  uint1 * const flc_puPaintVol = (unsigned char *) dataset.m_fields.m_paintVol.GetDataPtr();

  const uint4 flc_uXStepPixel = 3;
  const uint4 flc_uYStepPixel = m_uRenderBufferSize*3;
  
  const Normal<float4> flc_vObliqueNormal = dataset.m_vObliqueOrientation;
  const float4 fMprDistance = flc_vObliqueNormal.Dot(Vector<float4>(dataset.m_vObliqueLocation));
  const float4 flc_fObliqueMinDistance = fMprDistance - m_fHalfShadowThickness;
  const float4 flc_fObliqueMaxDistance = fMprDistance + m_fHalfShadowThickness;
  const Point<float4> flc_vOrthogonalPoint = Point3Df(dataset.m_vOrthogonalPtWorld.GetDividedAnIso(GetUnits()));

  const float4 flc_fDfbScale = dataset.m_fields.GetSldScale();
  const float4 flc_fColonWallThreshold = dataset.m_fColonWallThreshold;

  // retrieve the camera specification (ASSUME submarine quarantee's viewpoint is within volume)
  const Point<float4> flc_vViewPoint(dataset.m_submarine.GetPositionWorld());
  const Vector<float4> flc_vViewVector(dataset.m_submarine.GetOrientation());
  const Normal<float4> flc_vUpNormal(dataset.m_submarine.GetUpDirection());
  const Vector<float4> flc_vUpVector(flc_vUpNormal);
  const Vector<float4> flc_vDownVector(-flc_vUpNormal);
  const Vector<float4> flc_vRightVector(Normal<float4>(Normal<float4>(flc_vViewVector).Cross(flc_vUpNormal)));

  // Load the diff volume before rendering if needed
  if(flc_bUncleansed && !dataset.m_fields.IsDiffVolLoaded()) dataset.m_fields.LazyLoadDiffVolume();

  uint1  * const flc_puImageBuffer = m_puImageBuffer;
  float4 * const flc_pfDepthBuffer = m_pfDepthBuffer;

  // perform per-frame setup
  uint4 uSurfaceCount = 0;
  
  // If parallel compute which section to work on
  const uint4   flc_uStartV( renderingBounds.m_top );
  const uint4   flc_uEndV( renderingBounds.m_bottom );
  const uint4   flc_uStartU( renderingBounds.m_left );
  const uint4   flc_uEndU( renderingBounds.m_right );
  const int4    flc_iDeltaV( 2 );
  const int4    flc_iInterpolateV( -1 );

  // pre-compute the initial leap distance, since that is the same for every direction
  uint4 uVoxelX, uVoxelY, uVoxelZ;;
  FastRoundFloatToIntXYZ(uVoxelX,uVoxelY,uVoxelZ,(flc_vViewPoint.m_x*flc_fXScl),(flc_vViewPoint.m_y*flc_fYScl),(flc_vViewPoint.m_z*flc_fZScl));
  float4 fFirstLeapDistance = flc_puSLDvol[uVoxelX*flc_uXStep + uVoxelY*flc_uYStep + uVoxelZ*flc_uZStep]*flc_fDfbScale - flc_fDfbSpaceLeapConversationFactor;
  if (fFirstLeapDistance <= flc_fDfbSpaceLeapThreshold) fFirstLeapDistance = 0.0F;
  
  // step through all the pixels in three step as follows:
  Vector<float4> vHalfwayVector;
  Vector<float4> vIntervalVector;
  Vector<float4> vGradVector;
  Point<float4> vPoint;
  const Vector<float4> vMoveLightVector(flc_vUpVector    * m_fMoveLightVertDirection +
                                        flc_vRightVector * m_fMoveLightHorizDirection);
  Normal<float4> vLight1Normal;
  Normal<float4> vLight2Normal(flc_vDownVector + flc_vViewVector * m_fMoveLightVertDirection
                                              + flc_vRightVector * m_fMoveLightHorizDirection);
  float4 fDu,fDv;
  Normal<float4> vRayNormal;

  for (uint4 v=flc_uStartV; v <= flc_uEndV; v+=flc_iDeltaV)
  {
    fDv = float4(v) * flc_fTwoOverSize - 1.0F;
    Vector<float4> vRowVector(flc_vViewVector + flc_vUpVector * fDv * flc_fTanFov);
    for (uint4 u=flc_uStartU; u<=flc_uEndU; u+=2)
    {
      fDu = float4(u) * flc_fTwoOverSize - 1.0F;
      uint4 uPixelOffset = (v * flc_uYStepPixel + u * flc_uXStepPixel);
      uint4 uDepthOffset = u + v * m_uRenderBufferSize;
      
      CASTONERAY;//m_rayCount++;

      /************** (2) compute vacant pixels in non vacant rows **************/
      if(u!=0)
      {
        fDu = float4(u-1) * flc_fTwoOverSize - 1.0F;
        uint4 uPixelOffset = (v * flc_uYStepPixel + (u-1) * flc_uXStepPixel);
        uint4 uDepthOffset = (u-1) + v * m_uRenderBufferSize;

        TESTONEINTERPOLATE(1,flc_uXStepPixel);
        CASTONERAY;//m_rayCount++;
      } // endif not first pixel
    } // endfor all pixels in row

    /****************** (3) compute all pixels in vacant rows *****************/
    if(v!=flc_uStartV)
    {
      fDv = float4(v+flc_iInterpolateV) * flc_fTwoOverSize - 1.0F;
      Vector<float4> vRowVector(flc_vViewVector + flc_vUpVector * fDv * flc_fTanFov);
      for (uint4 u=flc_uStartU; u<=flc_uEndU; u+=1)
      {
        fDu = float4(u) * flc_fTwoOverSize - 1.0F;
        uint4 uPixelOffset = ((v+flc_iInterpolateV) * flc_uYStepPixel + u * flc_uXStepPixel);
        uint4 uDepthOffset = u + (v+flc_iInterpolateV) * m_uRenderBufferSize;
        
        TESTONEINTERPOLATE(m_uRenderBufferSize,flc_uYStepPixel);
        CASTONERAY;//m_rayCount++;
      } // // endfor all pixels in row
    } // endif not first row
  } // endfor all rows for this PE
  
  if(iId > 0)
  {
    m_surfaceCounts[ iId ] = uSurfaceCount;
  }
  else
  {
    m_surfaceCounts[ 0 ] = uSurfaceCount;
  }


} // Raycast()



uint4 uCurrent0, uCurrent1;
/**
 * Perspective Endoscopic Raycast (assumes the viewpoint is within the volume).
 *
 * This function does Adaptive-Sampling raycasting of the image plane. The work
 * per ray is performed in the CastSingleRay function.
 *
 * @param   id      Which thread this raycaster is associated with.
 * @param   dataset   dataset to render
 */
void VolumeRender::Raycast1(int iId, Dataset & dataset)
{ 
  // initialize performance measures
  //m_rayCount = 0;
  //m_sampleCount = 0;
  //m_spaceLeapCount = 0;
  //m_sumSpaceLeapDistance = 0;
  //m_maxSpaceLeapDistance = 0.0F;

  // Setup these local variables once per frame. "flc_" stands for Fast Local Constant
  // and this way the inner volume rendering loop remains highly optimized.
  const float4 flc_fFovRadians = m_fFovDegrees * M_PI / 180.0F;
  const float4 flc_fTwoOverSize = 2.0f / float4(m_uRenderSize);
  const float4 flc_fTanFov = float4( tan(flc_fFovRadians/2) );
  const uint4 flc_uVolMaxX = GetDim().m_x - 2;
  const uint4 flc_uVolMaxY = GetDim().m_y - 2;
  const uint4 flc_uVolMaxZ = GetDim().m_z - 2;

  const float4 flc_fNotSeenRed   = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vNotSeenScheme2.m_x : TransferFunctionVC::m_vNotSeenScheme1.m_x;
  const float4 flc_fNotSeenGreen = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vNotSeenScheme2.m_y : TransferFunctionVC::m_vNotSeenScheme1.m_y;
  const float4 flc_fNotSeenBlue  = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vNotSeenScheme2.m_z : TransferFunctionVC::m_vNotSeenScheme1.m_z;
  const float4 flc_fSeenRed   = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vSeenScheme2.m_x : TransferFunctionVC::m_vSeenScheme1.m_x;
  const float4 flc_fSeenGreen = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vSeenScheme2.m_y : TransferFunctionVC::m_vSeenScheme1.m_y;
  const float4 flc_fSeenBlue  = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vSeenScheme2.m_z : TransferFunctionVC::m_vSeenScheme1.m_z;
  const float4 * flc_pfOpacitySurfaceTransferFunction = dataset.m_surfaceTransferFunction.m_pfOpacity;
  const float4 * flc_pfRedBiopsyTransferFunction = dataset.m_biopsyTransferFunction.m_pfRed;
  const float4 * flc_pfGreenBiopsyTransferFunction = dataset.m_biopsyTransferFunction.m_pfGreen;
  const float4 * flc_pfBlueBiopsyTransferFunction = dataset.m_biopsyTransferFunction.m_pfBlue;
  const float4 * flc_pfOpacityBiopsyTransferFunction = dataset.m_biopsyTransferFunction.m_pfOpacity;
  
  const bool flc_bIsFlying = rdrGlobal.m_supine.m_submarine.IsFlying() || rdrGlobal.m_prone.m_submarine.IsFlying();

  const bool flc_bAxialShadow = rdrGlobal.m_bAxialShadow & !flc_bIsFlying;
  const bool flc_bSagittalShadow = rdrGlobal.m_bSagittalShadow & !flc_bIsFlying;
  const bool flc_bCoronalShadow = rdrGlobal.m_bCoronalShadow & !flc_bIsFlying;
  const bool flc_bSegmentHalfPlaneShadow = rdrGlobal.m_bSegmentHalfPlaneShadow & !flc_bIsFlying;
  const bool flc_bObliqueShadow = rdrGlobal.m_bObliqueShadow & !flc_bIsFlying;
  const bool flc_bGridShadow = rdrGlobal.m_bGridShadow & !flc_bIsFlying;
  const bool flc_bShadow = flc_bAxialShadow || flc_bSagittalShadow || flc_bCoronalShadow || flc_bSegmentHalfPlaneShadow || flc_bObliqueShadow || flc_bGridShadow;
  const bool flc_bBiopsyMode = rdrGlobal.m_bBiopsyMode;
  const bool flc_bTestVol = rdrGlobal.m_bShowHomogenity;
  const bool flc_bRaycastPaintVol = rdrGlobal.m_bRaycastPaintVol;
  const bool flc_bSpaceLeaping = rdrGlobal.m_bSpaceLeaping;
  const bool flc_bUncleansed = (dataset.m_fields.IsDiffVolLoaded() & !flc_bSpaceLeaping);

  const float4 flc_fDfbSpaceLeapConversationFactor = Max(GetUnits().m_x,GetUnits().m_y,GetUnits().m_z) + m_fInterSampleDistance;
  const float4 flc_fDfbSpaceLeapThreshold = flc_fDfbSpaceLeapConversationFactor;

  const uint4 flc_uXStep = 1;
  const uint4 flc_uYStep = GetDim().m_x;
  const uint4 flc_uZStep = GetDim().m_x*GetDim().m_y;
  const uint4 flc_uXYStep = flc_uXStep+flc_uYStep;
  const uint4 flc_uXZStep = flc_uXStep+flc_uZStep;
  const uint4 flc_uYZStep = flc_uYStep+flc_uZStep;
  const uint4 flc_uXYZStep = flc_uXYStep+flc_uZStep;

  const uint4 flc_uXStep3 = 3;
  const uint4 flc_uYStep3 = GetDim().m_x*3;
  const uint4 flc_uZStep3 = GetDim().m_x*GetDim().m_y*3;
  const uint4 flc_uXYStep3 = flc_uXStep3+flc_uYStep3;
  const uint4 flc_uXZStep3 = flc_uXStep3+flc_uZStep3;
  const uint4 flc_uYZStep3 = flc_uYStep3+flc_uZStep3;
  const uint4 flc_uXYZStep3 = flc_uXYStep3+flc_uZStep3;

  const float4 flc_fXScl = 1.0f / GetUnits().m_x;
  const float4 flc_fYScl = 1.0f / GetUnits().m_y;
  const float4 flc_fZScl = 1.0f / GetUnits().m_z;

  const uint1 * const flc_puECVvol = (unsigned char *) GetDataPtr();
  const uint1 * const flc_puSLDvol = (unsigned char *) dataset.m_fields.m_sldInterior.GetDataPtr();
  const uint1 * const flc_puTestVol = (unsigned char *) dataset.m_fields.m_testVolumeUC.GetDataPtr();

  const uint1 flc_uCurrentSeenVal = dataset.m_fields.m_paintVol.m_uCurrentSeenVal;

  uint1 * const flc_puPaintVol = (unsigned char *) dataset.m_fields.m_paintVol.GetDataPtr();

  const uint4 flc_uXStepPixel = 3;
  const uint4 flc_uYStepPixel = m_uRenderBufferSize*3;
  
  const Normal<float4> flc_vObliqueNormal = dataset.m_vObliqueOrientation;
  const float4 fMprDistance = flc_vObliqueNormal.Dot(Vector<float4>(dataset.m_vObliqueLocation));
  const float4 flc_fObliqueMinDistance = fMprDistance - m_fHalfShadowThickness;
  const float4 flc_fObliqueMaxDistance = fMprDistance + m_fHalfShadowThickness;
  const Point<float4> flc_vOrthogonalPoint = Point3Df(dataset.m_vOrthogonalPtWorld.GetDividedAnIso(GetUnits()));

  const float4 flc_fDfbScale = dataset.m_fields.GetSldScale();
  const float4 flc_fColonWallThreshold = dataset.m_fColonWallThreshold;

  // retrieve the camera specification (ASSUME submarine quarantee's viewpoint is within volume)
  const Point<float4> flc_vViewPoint(dataset.m_submarine.GetPositionWorld());
  const Vector<float4> flc_vViewVector(dataset.m_submarine.GetOrientation());
  const Normal<float4> flc_vUpNormal(dataset.m_submarine.GetUpDirection());
  const Vector<float4> flc_vUpVector(flc_vUpNormal);
  const Vector<float4> flc_vDownVector(-flc_vUpNormal);
  const Vector<float4> flc_vRightVector(Normal<float4>(Normal<float4>(flc_vViewVector).Cross(flc_vUpNormal)));

  // Load the diff volume before rendering if needed
  if(flc_bUncleansed && !dataset.m_fields.IsDiffVolLoaded()) dataset.m_fields.LazyLoadDiffVolume();

  uint1  * const flc_puImageBuffer = m_puImageBuffer;
  float4 * const flc_pfDepthBuffer = m_pfDepthBuffer;

  // perform per-frame setup
  uint4 uSurfaceCount = 0;
  uCurrent0 = 0;
  uCurrent1 = m_uRenderSize-1;
  
  // If parallel compute which section to work on
  //                                          iId == -1                 iId == 0     iId == 1
  const uint4   flc_uStartV       = (iId == -1) ? 0          : ( (iId==0) ? 0          : m_uRenderSize-1 );
  const int4    flc_iDeltaV       = (iId == -1) ? 2          : ( (iId==0) ? 2          : -2              );
  const int4    flc_iInterpolateV = (iId == -1) ? -1         : ( (iId==0) ? -1         : 1               );
  uint4 * const flc_puCurrent     = (iId == -1) ? &uCurrent0 : ( (iId==0) ? &uCurrent0 : &uCurrent1      );

  // pre-compute the initial leap distance, since that is the same for every direction
  uint4 uVoxelX, uVoxelY, uVoxelZ;;
  FastRoundFloatToIntXYZ(uVoxelX,uVoxelY,uVoxelZ,(flc_vViewPoint.m_x*flc_fXScl),(flc_vViewPoint.m_y*flc_fYScl),(flc_vViewPoint.m_z*flc_fZScl));
  float4 fFirstLeapDistance = flc_puSLDvol[uVoxelX*flc_uXStep + uVoxelY*flc_uYStep + uVoxelZ*flc_uZStep]*flc_fDfbScale - flc_fDfbSpaceLeapConversationFactor;
  if (fFirstLeapDistance <= flc_fDfbSpaceLeapThreshold) fFirstLeapDistance = 0.0F;
  
  // step through all the pixels in three step as follows:
  Vector<float4> vHalfwayVector;
  Vector<float4> vIntervalVector;
  Vector<float4> vGradVector;
  Point<float4> vPoint;
  const Vector<float4> vMoveLightVector(flc_vUpVector    * m_fMoveLightVertDirection +
                                        flc_vRightVector * m_fMoveLightHorizDirection);
  Normal<float4> vLight1Normal;
  Normal<float4> vLight2Normal(flc_vDownVector + flc_vViewVector * m_fMoveLightVertDirection
                                              + flc_vRightVector * m_fMoveLightHorizDirection);
  float4 fDu,fDv;
  Normal<float4> vRayNormal;

  for (uint4 v=flc_uStartV; uCurrent0 <= uCurrent1; v+=flc_iDeltaV, *flc_puCurrent = v)
  {
    fDv = float4(v) * flc_fTwoOverSize - 1.0F;
    Vector<float4> vRowVector(flc_vViewVector + flc_vUpVector * fDv * flc_fTanFov);
    for (uint4 u=0; u<m_uRenderSize; u+=2)
    {
      fDu = float4(u) * flc_fTwoOverSize - 1.0F;
      uint4 uPixelOffset = (v * flc_uYStepPixel + u * flc_uXStepPixel);
      uint4 uDepthOffset = u + v * m_uRenderBufferSize;
      
      CASTONERAY;//m_rayCount++;

      /************** (2) compute vacant pixels in non vacant rows **************/
      if(u!=0)
      {
        fDu = float4(u-1) * flc_fTwoOverSize - 1.0F;
        uint4 uPixelOffset = (v * flc_uYStepPixel + (u-1) * flc_uXStepPixel);
        uint4 uDepthOffset = (u-1) + v * m_uRenderBufferSize;

        TESTONEINTERPOLATE(1,flc_uXStepPixel);
        CASTONERAY;//m_rayCount++;
      } // endif not first pixel
    } // endfor all pixels in row

    /****************** (3) compute all pixels in vacant rows *****************/
    if(v!=flc_uStartV)
    {
      fDv = float4(v+flc_iInterpolateV) * flc_fTwoOverSize - 1.0F;
      Vector<float4> vRowVector(flc_vViewVector + flc_vUpVector * fDv * flc_fTanFov);
      for (uint4 u=0; u<m_uRenderSize; u+=1)
      {
        fDu = float4(u) * flc_fTwoOverSize - 1.0F;
        uint4 uPixelOffset = ((v+flc_iInterpolateV) * flc_uYStepPixel + u * flc_uXStepPixel);
        uint4 uDepthOffset = u + (v+flc_iInterpolateV) * m_uRenderBufferSize;
        
        TESTONEINTERPOLATE(m_uRenderBufferSize,flc_uYStepPixel);
        CASTONERAY;//m_rayCount++;
      } // // endfor all pixels in row
    } // endif not first row
  } // endfor all rows for this PE
  
  if(iId > 0)
  {
    m_surfaceCounts[ iId ] = uSurfaceCount;
  }
  else
  {
    m_surfaceCounts[ 0 ] = uSurfaceCount;
  }

} // Raycast()



/**
 * Render an arbitrarily oriented 2D slice.
 *
 *
 *
 * @param   vBottomLeft     Corner of the plane.
 * @param   vBottomRight    Corner of the plane.
 * @param   vTopLeft        Corner of the plane.
 * @param   vTopRight       Corner of the plane.
 * @param   puImageBuffer   Pointer to the output image buffer.
 * @param   uImageSize      Size (square) of the image.
 */
void VolumeRender::RayCastObliqueSlice( Dataset & dataset, const Point<float4> & vBottomLeft, const Point<float4> & vBottomRight,
                                        const Point<float4> & vTopLeft, const Point<float4> & vTopRight,
                                        const Vector<float4> & vViewVector, const Normal<float4> & vUpNormal,
                                        uint1 * puImageBuffer, const uint4 uImageWidth, const uint4 uImageHeight,
                                        const uint4 uDimBorderPixelsX,
                                        const uint4 uSubImageStartX, const uint4 uSubImageStartY,
                                        const uint4 uSubImageWidth, const uint4 uSubImageHeight, 
                                        const bool bRadialProjection, const bool bSampleUncleansed, const bool bSampleDepth,
                                        const bool bSampleActualDepth, Normal3D<float4> & depthSampleNormal, Point3D<float4> & depthSampleInOut )
{ 
  const uint4 flc_uVolMaxX = GetDim().m_x - 2;
  const uint4 flc_uVolMaxY = GetDim().m_y - 2;
  const uint4 flc_uVolMaxZ = GetDim().m_z - 2;

  const float4 flc_fNotSeenRed   = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vNotSeenScheme2.m_x : TransferFunctionVC::m_vNotSeenScheme1.m_x;
  const float4 flc_fNotSeenGreen = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vNotSeenScheme2.m_y : TransferFunctionVC::m_vNotSeenScheme1.m_y;
  const float4 flc_fNotSeenBlue  = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vNotSeenScheme2.m_z : TransferFunctionVC::m_vNotSeenScheme1.m_z;
  const float4 flc_fSeenRed   = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vSeenScheme2.m_x : TransferFunctionVC::m_vSeenScheme1.m_x;
  const float4 flc_fSeenGreen = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vSeenScheme2.m_y : TransferFunctionVC::m_vSeenScheme1.m_y;
  const float4 flc_fSeenBlue  = rdrGlobal.m_iColorScheme ? TransferFunctionVC::m_vSeenScheme2.m_z : TransferFunctionVC::m_vSeenScheme1.m_z;
  const float4 * flc_pfOpacitySurfaceTransferFunction = dataset.m_surfaceTransferFunction.m_pfOpacity;
  const float4 * flc_pfRedBiopsyTransferFunction = dataset.m_biopsyTransferFunction.m_pfRed;
  const float4 * flc_pfGreenBiopsyTransferFunction = dataset.m_biopsyTransferFunction.m_pfGreen;
  const float4 * flc_pfBlueBiopsyTransferFunction = dataset.m_biopsyTransferFunction.m_pfBlue;
  const float4 * flc_pfOpacityBiopsyTransferFunction = dataset.m_biopsyTransferFunction.m_pfOpacity;
  
  const bool flc_bIsFlying = rdrGlobal.m_supine.m_submarine.IsFlying() || rdrGlobal.m_prone.m_submarine.IsFlying();

  const bool flc_bAxialShadow = rdrGlobal.m_bAxialShadow & !flc_bIsFlying;
  const bool flc_bSagittalShadow = rdrGlobal.m_bSagittalShadow & !flc_bIsFlying;
  const bool flc_bCoronalShadow = rdrGlobal.m_bCoronalShadow & !flc_bIsFlying;
  const bool flc_bSegmentHalfPlaneShadow = false;//rdrGlobal.m_bSegmentHalfPlaneShadow & !flc_bIsFlying;
  const bool flc_bObliqueShadow = false;//rdrGlobal.m_bObliqueShadow & !flc_bIsFlying;
  const bool flc_bGridShadow = rdrGlobal.m_bGridShadow & !flc_bIsFlying;
  const bool flc_bShadow = flc_bAxialShadow || flc_bSagittalShadow || flc_bCoronalShadow || flc_bSegmentHalfPlaneShadow || flc_bObliqueShadow || flc_bGridShadow;
  const bool flc_bBiopsyMode = rdrGlobal.m_bBiopsyMode;
  const bool flc_bTestVol = false;
  const bool flc_bRaycastPaintVol = rdrGlobal.m_bRaycastPaintVol;
	bool flc_bSpaceLeaping = true; //rdrGlobal.m_bSpaceLeaping;  
  const bool flc_bUncleansed = (dataset.m_fields.IsDiffVolLoaded() & !rdrGlobal.m_bSpaceLeaping );  
	//if( flc_bUncleansed && rdrGlobal.m_bUseULDvolume )
	//{
	//	flc_bSpaceLeaping = true; //:: Use both SLD and ULD all the time, So, the space leaping always needs.
	//}

  const float4 flc_fDfbSpaceLeapConversationFactor = Max(GetUnits().m_x,GetUnits().m_y,GetUnits().m_z) + m_fInterSampleDistance;
  const float4 flc_fDfbSpaceLeapThreshold = flc_fDfbSpaceLeapConversationFactor;

  const uint4 flc_uXStep = 1;
  const uint4 flc_uYStep = GetDim().m_x;
  const uint4 flc_uZStep = GetDim().m_x*GetDim().m_y;
  const uint4 flc_uXYStep = flc_uXStep+flc_uYStep;
  const uint4 flc_uXZStep = flc_uXStep+flc_uZStep;
  const uint4 flc_uYZStep = flc_uYStep+flc_uZStep;
  const uint4 flc_uXYZStep = flc_uXYStep+flc_uZStep;

  const uint4 flc_uXStep3 = 3;
  const uint4 flc_uYStep3 = GetDim().m_x*3;
  const uint4 flc_uZStep3 = GetDim().m_x*GetDim().m_y*3;
  const uint4 flc_uXYStep3 = flc_uXStep3+flc_uYStep3;
  const uint4 flc_uXZStep3 = flc_uXStep3+flc_uZStep3;
  const uint4 flc_uYZStep3 = flc_uYStep3+flc_uZStep3;
  const uint4 flc_uXYZStep3 = flc_uXYStep3+flc_uZStep3;

  const float4 flc_fXScl = 1.0f / GetUnits().m_x;
  const float4 flc_fYScl = 1.0f / GetUnits().m_y;
  const float4 flc_fZScl = 1.0f / GetUnits().m_z;

  const uint1 * const flc_puECVvol = (unsigned char *) GetDataPtr();
  uint1 * flc_puSLDvol = (unsigned char *) dataset.m_fields.m_sldInterior.GetDataPtr();
  if( flc_bUncleansed && rdrGlobal.m_bUseULDvolume )
  {
    flc_puSLDvol = (unsigned char *) dataset.m_fields.m_sldExterior.GetDataPtr();  //:: Use this for un-cleansed SLD, i.e. the ULD volume
  } // if
  const uint1 * const flc_puTestVol = (unsigned char *) dataset.m_fields.m_testVolumeUC.GetDataPtr();

  const uint1 flc_uCurrentSeenVal = dataset.m_fields.m_paintVol.m_uCurrentSeenVal;
  
  uint1 * const flc_puPaintVol = (unsigned char *) dataset.m_fields.m_paintVol.GetDataPtr();

  const uint4 flc_uXStepPixel = 3;
  const uint4 flc_uYStepPixel = uImageWidth*3;
  const float4 fRadialEdgeOverlap = ( bRadialProjection ? 0.15F : 0.0F );
  const uint4 uDimBorderPixelsY = uSubImageHeight / 2 * fRadialEdgeOverlap;
  
  const Normal<float4> flc_vObliqueNormal = dataset.m_vObliqueOrientation;
  const float4 fMprDistance = flc_vObliqueNormal.Dot(Vector<float4>(dataset.m_vObliqueLocation));
  const float4 flc_fObliqueMinDistance = fMprDistance - m_fHalfShadowThickness;
  const float4 flc_fObliqueMaxDistance = fMprDistance + m_fHalfShadowThickness;
  const Point<float4> flc_vOrthogonalPoint = Point3Df(dataset.m_vOrthogonalPtWorld.GetDividedAnIso(GetUnits()));

  const float4 flc_fDfbScale = dataset.m_fields.GetSldScale();
  const float4 flc_fColonWallThreshold = dataset.m_fColonWallThreshold;

  // Load the diff volume before rendering if needed
  if(flc_bUncleansed && !dataset.m_fields.IsDiffVolLoaded()) dataset.m_fields.LazyLoadDiffVolume();

  // retrieve the camera specification (ASSUME quarantee's viewpoint is within volume)
  Vector<float4> vPoint;
  const Vector<float4> flc_vViewVector(vViewVector);
  const Normal<float4> flc_vUpNormal(vUpNormal);
  const Vector<float4> flc_vUpVector(flc_vUpNormal);
  const Vector<float4> flc_vDownVector(-flc_vUpNormal);
  const Vector<float4> flc_vRightVector(Normal<float4>(Normal<float4>(flc_vViewVector).Cross(flc_vUpNormal)));
  Normal<float4> vPlaneNormal(flc_vViewVector);
  Normal<float4> vLight1Normal(Vector<float4>(vPlaneNormal) + flc_vViewVector * m_fMoveLightVertDirection + flc_vRightVector * m_fMoveLightHorizDirection);
  Normal<float4> vLight2Normal(vLight1Normal);

  Vector<float4> vHalfwayVector;
  Vector<float4> vIntervalVector;
  Vector<float4> vGradVector;

  const Vector<float4> vCenterLeft (Vector<float4>(vBottomLeft )*0.5 + Vector<float4>(vTopLeft )*0.5);
  const Vector<float4> vCenterRight(Vector<float4>(vBottomRight)*0.5 + Vector<float4>(vTopRight)*0.5);

  Normal<float4> vRayNormal;
  uint4 uSurfaceCount = 0;
  uint1 * puPixel = puImageBuffer + uSubImageStartY * flc_uYStepPixel;
  const uint4 uFullIntensityWidth = uSubImageWidth - uDimBorderPixelsX;
  const uint4 uFullIntensityHeight = uSubImageHeight - uDimBorderPixelsY;
  uint4 uStartV = bSampleDepth ? static_cast< uint4>( depthSampleInOut.m_y ) : 0;
  for (uint4 v=uStartV; v<uSubImageHeight; v++)
  {
    float4 dv = float4(v)/(uSubImageHeight-1);
    const Vector<float4> vLeft (Vector<float4>(vBottomLeft )*(1.0F-dv) + Vector<float4>(vTopLeft )*dv);
    const Vector<float4> vRight(Vector<float4>(vBottomRight)*(1.0F-dv) + Vector<float4>(vTopRight)*dv);
    double fAngle = (dv-0.5) * (2 * M_PI) * (1 + fRadialEdgeOverlap);

    Matrix<float4> m;
    m.Rotate(Normal<float4>(vRight-vLeft), fAngle);
    uint4 uStartU = bSampleDepth ? static_cast< uint4>( depthSampleInOut.m_x ) : 0;
    for (uint4 u=uStartU; u<uSubImageWidth; u++)
    {
      float4 du = float4(u)/(uSubImageWidth-1);
      uint4 uPixelOffset = (v * flc_uYStepPixel + (u+uSubImageStartX) * flc_uXStepPixel);
      if ( bRadialProjection )
      {
        vRayNormal = m * vPlaneNormal;
        vPoint = vCenterLeft * (1.0F - du) + vCenterRight * du;
        vLight1Normal = vLight2Normal = vRayNormal;
      }
      else
      {
        vRayNormal = vPlaneNormal;
        vPoint = vLeft * (1.0F - du) + vRight * du;
      }
      if ( bSampleDepth && !bSampleActualDepth )
      { // This is the ray start point rather than the actual depth.
        depthSampleInOut = static_cast< Point3Df >( vPoint );
        depthSampleNormal = vRayNormal;
        return;
      }
      
      float4 fRedValue = 0.0F, fGreenValue = 0.0F, fBlueValue = 0.0F, fAlphaValue = 0.0F, fDepthValue = 0.0F;
      
      uint4 uVoxelX, uVoxelY, uVoxelZ;
      FastRoundFloatToIntXYZ(uVoxelX,uVoxelY,uVoxelZ,(vPoint.m_x*flc_fXScl),(vPoint.m_y*flc_fYScl),(vPoint.m_z*flc_fZScl));
      float4 fXDiff = vPoint.m_x*flc_fXScl - uVoxelX;
      float4 fYDiff = vPoint.m_y*flc_fYScl - uVoxelY;
      float4 fZDiff = vPoint.m_z*flc_fZScl - uVoxelZ;
      if (uVoxelX > 1 && uVoxelX < flc_uVolMaxX - 1 &&  
          uVoxelY > 1 && uVoxelY < flc_uVolMaxY - 1 &&  
          uVoxelZ > 1 && uVoxelZ < flc_uVolMaxZ - 1)
      {
        float4 fValue;
        const uint1 * const puVol = flc_puECVvol + uVoxelX * flc_uXStep + uVoxelY * flc_uYStep + uVoxelZ * flc_uZStep;
        fValue = Trilin(fXDiff,fYDiff,fZDiff,
                        *(puVol), 
                        *(puVol + flc_uXStep), 
                        *(puVol + flc_uYStep), 
                        *(puVol + flc_uXYStep),
                        *(puVol + flc_uZStep), 
                        *(puVol + flc_uXZStep),
                        *(puVol + flc_uYZStep),
                        *(puVol + flc_uXYZStep));

				/// the Trilin interpolation may let the value greater than the range.
				/// So, the necessary clampping is still needed. Dongqing
				if( fValue > 254.0F )
					fValue = 254.0F;

        //if (fValue > dataset.m_surfaceTransferFunction.m_fColonWallThreshold )
        if (fValue > 60 ) /// This (60 = 255*450/2000) is equivalent to the -550 HU. It should be OK for all studies
        {
          fRedValue = fGreenValue = fBlueValue = (fValue < 0.0F) ? 0.0F : fValue;
        }
        else
        {
          float4 fLeapDistance = flc_puSLDvol[uVoxelX*flc_uXStep + uVoxelY*flc_uYStep + uVoxelZ*flc_uZStep]*flc_fDfbScale - flc_fDfbSpaceLeapConversationFactor;
          if (fLeapDistance <= flc_fDfbSpaceLeapThreshold) fLeapDistance = 0;

          CastSingleRayForUDview(u, v, 
                        flc_bObliqueShadow, flc_bGridShadow, flc_bAxialShadow, flc_bSagittalShadow, flc_bCoronalShadow, flc_bSegmentHalfPlaneShadow,
                        flc_bShadow, flc_bBiopsyMode, flc_bTestVol, flc_bRaycastPaintVol, flc_bSpaceLeaping, flc_bUncleansed,
                        flc_vObliqueNormal, flc_fObliqueMinDistance, flc_fObliqueMaxDistance, flc_vOrthogonalPoint,
                        vRayNormal, vLight1Normal, vLight2Normal,
                        Point<float4>(vPoint),
                        fLeapDistance,
                        flc_uXStep, flc_uYStep, flc_uZStep, flc_uXYStep, flc_uYZStep, flc_uXZStep, flc_uXYZStep,
                        flc_uXStep3, flc_uYStep3, flc_uZStep3, flc_uXYStep3, flc_uYZStep3, flc_uXZStep3, flc_uXYZStep3,
                        flc_fXScl, flc_fYScl, flc_fZScl, flc_uVolMaxX, flc_uVolMaxY, flc_uVolMaxZ,
                        flc_puECVvol,
                        flc_puSLDvol,
                        flc_puTestVol,
                        flc_puPaintVol, flc_uCurrentSeenVal,
                        flc_fNotSeenRed, flc_fNotSeenGreen, flc_fNotSeenBlue,
                        flc_fSeenRed, flc_fSeenGreen, flc_fSeenBlue,
                        flc_pfOpacitySurfaceTransferFunction,
                        flc_pfRedBiopsyTransferFunction, flc_pfGreenBiopsyTransferFunction,
                        flc_pfBlueBiopsyTransferFunction, flc_pfOpacityBiopsyTransferFunction,
                        fRedValue, fGreenValue, fBlueValue, fAlphaValue, fDepthValue,
                        flc_fDfbScale, flc_fDfbSpaceLeapConversationFactor, flc_fDfbSpaceLeapThreshold,
                        uSurfaceCount,
                        vGradVector,
                        vHalfwayVector,
                        vIntervalVector);  
        }

      }

      if( bSampleActualDepth )
      { // This is the actual depth.
        depthSampleInOut = static_cast< Point3Df >( vPoint + vRayNormal * fDepthValue );
        depthSampleNormal = vRayNormal;
        return;
      } // if

      if ( u < uDimBorderPixelsX || u > uFullIntensityWidth ||
           v < uDimBorderPixelsY || v > uFullIntensityHeight )
      {
        // store result
        const float4 fDimAmount = 0.8F;
        *(puPixel + uPixelOffset    ) = (unsigned char) fRedValue * fDimAmount;
        *(puPixel + uPixelOffset + 1) = (unsigned char) fGreenValue * fDimAmount;
        *(puPixel + uPixelOffset + 2) = (unsigned char) fBlueValue * fDimAmount;
        //*(pfDepthPixel + uDepthOffset)           = fDepthValue;
      }
      else
      {
        // store result
        *(puPixel + uPixelOffset    ) = (unsigned char) fRedValue;
        *(puPixel + uPixelOffset + 1) = (unsigned char) fGreenValue;
        *(puPixel + uPixelOffset + 2) = (unsigned char) fBlueValue;
        //*(pfDepthPixel + uDepthOffset)           = fDepthValue;
      }
      
    }  // endfor all u
  } // endfor all v

} // RaycastObliqueSlice()


/**
 * Render an arbitrarily oriented 2D slice.
 *
 * @param   vBottomLeft     Corner of the plane.
 * @param   vBottomRight    Corner of the plane.
 * @param   vTopLeft        Corner of the plane.
 * @param   vTopRight       Corner of the plane.
 * @param   puImageBuffer   Pointer to the output image buffer.
 * @param   uImageSize      Size (square) of the image.
 */
void VolumeRender::CreateObliqueSlice( const Point<float4> & vBottomLeft, const Point<float4> & vBottomRight,
                                       const Point<float4> & vTopLeft, const Point<float4> & vTopRight,
                                       uint1 * puImageBuffer, const uint4 uImageWidth, const uint4 uImageHeight,
                                       const uint4 uDimBorderPixels, const bool bSampleUncleansed )
{ 
  const uint4 flc_uVolMaxX = GetDim().m_x - 2;
  const uint4 flc_uVolMaxY = GetDim().m_y - 2;
  const uint4 flc_uVolMaxZ = GetDim().m_z - 2;
  
  const uint4 flc_uXStep = 1;
  const uint4 flc_uYStep = GetDim().m_x;
  const uint4 flc_uZStep = GetDim().m_x*GetDim().m_y;
  const uint4 flc_uXYStep = flc_uXStep+flc_uYStep;
  const uint4 flc_uXZStep = flc_uXStep+flc_uZStep;
  const uint4 flc_uYZStep = flc_uYStep+flc_uZStep;
  const uint4 flc_uXYZStep = flc_uXYStep+flc_uZStep;
  
  const float4 flc_fXScl = 1.0f / GetUnits().m_x;
  const float4 flc_fYScl = 1.0f / GetUnits().m_y;
  const float4 flc_fZScl = 1.0f / GetUnits().m_z;
  
  const uint1 * const flc_puECVvol = (unsigned char *) GetDataPtr();

  Point<float4> vPixelPosition;
  
  // Convert to Volume Coordinates
  Vector<float4> vBottomLeftVolCoord  ( GetWorldToVolumeMatrix() * vBottomLeft );
  Vector<float4> vBottomRightVolCoord ( GetWorldToVolumeMatrix() * vBottomRight);
  Vector<float4> vTopLeftVolCoord     ( GetWorldToVolumeMatrix() * vTopLeft    );
  Vector<float4> vTopRightVolCoord    ( GetWorldToVolumeMatrix() * vTopRight   );
  
  uint1 * puPixel = puImageBuffer;
  const uint4 uFullIntensityWidth = uImageWidth - uDimBorderPixels;
  for (uint4 v=0; v<uImageHeight; v++)
  {
    float4 dv = float4(v)/(uImageHeight-1);
    const Vector<float4> vLeft (vBottomLeftVolCoord *(1.0F-dv) + vTopLeftVolCoord *dv);
    const Vector<float4> vRight(vBottomRightVolCoord*(1.0F-dv) + vTopRightVolCoord*dv);
    for (uint4 u=0; u<uDimBorderPixels; u++)
    {
      float4 du = float4(u)/(uImageWidth-1);
      vPixelPosition = Point<float4>( vLeft * (1.0F - du) + vRight * du );
      
      uint4 uVoxelX, uVoxelY, uVoxelZ;
      FastRoundFloatToIntXYZ(uVoxelX,uVoxelY,uVoxelZ,vPixelPosition.m_x,vPixelPosition.m_y,vPixelPosition.m_z);
      float4 fXDiff = vPixelPosition.m_x - uVoxelX;
      float4 fYDiff = vPixelPosition.m_y - uVoxelY;
      float4 fZDiff = vPixelPosition.m_z - uVoxelZ;
      if (uVoxelX > 1 && uVoxelX < flc_uVolMaxX - 1 &&  
          uVoxelY > 1 && uVoxelY < flc_uVolMaxY - 1 &&  
          uVoxelZ > 1 && uVoxelZ < flc_uVolMaxZ - 1)
      {
        const uint4 uIndexOffset = uVoxelX * flc_uXStep + uVoxelY * flc_uYStep + uVoxelZ * flc_uZStep;
        const uint1 * const puVol = flc_puECVvol + uIndexOffset;
        float4 fValue = Trilin(fXDiff,fYDiff,fZDiff,
                               *(puVol), 
                               *(puVol + flc_uXStep), 
                               *(puVol + flc_uYStep), 
                               *(puVol + flc_uXYStep),
                               *(puVol + flc_uZStep), 
                               *(puVol + flc_uXZStep),
                               *(puVol + flc_uYZStep),
                               *(puVol + flc_uXYZStep));
        *(puPixel++) = (fValue < 0.0F) ? 0 : uint1(fValue)/2;
      }
      else
      {
        // render black outside
        *(puPixel++) = 0;
      }
    }  // endfor all u
    for (uint4 u=0; u<uFullIntensityWidth; u++)
    {
      float4 du = float4(u)/(uImageWidth-1);
      vPixelPosition = Point3D<float4>( vLeft * (1.0F - du) + vRight * du );  
      
      float4 fValue = (bSampleUncleansed ? GetUncleansedResampledValue(vPixelPosition) : GetResampledValue(vPixelPosition));
      // render black outside (GetResampledValue returns -1 outside)
      *(puPixel++) = (fValue < 0.0F) ? 0.0F : fValue;
    }  // endfor all u
    for (uint4 u=0; u<uImageWidth; u++)
    {
      float4 du = float4(u)/(uImageWidth-1);
      vPixelPosition = Point< float4> (vLeft * (1.0F - du) + vRight * du);
      
      float4 fValue = (bSampleUncleansed ? GetUncleansedResampledValue(vPixelPosition) : GetResampledValue(vPixelPosition));
      // render black outside (GetResampledValue returns -1 outside)
      *(puPixel++) = (fValue < 0.0F) ? 0.0F : fValue/2;
    }  // endfor all u
  } // endfor all v
} // CreateObliqueSlice()


/**
 * Calculate the accumulated opacity between two points in the volume
 *
 * @param startPointVolume                 the start point in volume coordinates
 * @param endPointVolume                   the end point in volume coordinates
 * @param pfOpacitySurfaceTransferFunction the transfer function used to convert voxels into opacity
 * @param bCleansed                        whether to used the cleansed volume
 * @param opaquePointOut                   the point at which maximum opacity was reached
 *
 * @return the accumulated opacity between the two points (1 if above normal opacity threshold)
 */
float4 VolumeRender::CalculateAccumulatedOpacity( const Point3D<float4> & startPointVolume,
                                                  const Point3D<float4> & endPointVolume,
                                                  const float4 * pfOpacitySurfaceTransferFunction,
                                                  const bool & bCleansed,
                                                  Point<float4> & opaquePointOut )
{

  // loop over all sample points and accumulate opacity
  float4 fAccumulatedOpacity( 0 );
  const float4 fLength( Vector3Df( endPointVolume - startPointVolume ).GetLength() );
  for ( float4 fDistance(0);
        fDistance <= fLength && fAccumulatedOpacity < m_fOpacityThreshold;
        fDistance += m_fInterSampleDistance )
  {

    // get the sample position
    const Normal3Df rayDirection( endPointVolume - startPointVolume );
    const Point3Df vfSamplePosition( startPointVolume + rayDirection * fDistance );
    opaquePointOut = vfSamplePosition;

    // check that it's inside the volume
    const Triple<int4> dim( GetDim() );
    if ( vfSamplePosition.m_x < 2 || vfSamplePosition.m_x > GetDim().m_x - 3 ||
         vfSamplePosition.m_y < 2 || vfSamplePosition.m_y > GetDim().m_y - 3 ||
         vfSamplePosition.m_z < 2 || vfSamplePosition.m_z > GetDim().m_z - 3 )
    {
      // outside the volume -> stop the traversal
      break;
    }

    // lookup sample and opacity
    const Point3D<uint2> vuSamplePosition( vfSamplePosition.m_x + 0.5F, vfSamplePosition.m_y + 0.5F, vfSamplePosition.m_z + 0.5F );
    const float4 fValue( bCleansed ? GetVoxel( vuSamplePosition ) : GetUncleansedValue( vuSamplePosition ) );
    const float4 fOpacity( pfOpacitySurfaceTransferFunction[ static_cast< int4 >( fValue + 0.5F ) ] );

    // accumulate opacity
    fAccumulatedOpacity += ( 1.0F - fAccumulatedOpacity ) * fOpacity;

  } // loop over all sample points and accumulate opacity

  if ( fAccumulatedOpacity >= m_fOpacityThreshold ) fAccumulatedOpacity = 1;
  return fAccumulatedOpacity;

} // CalculateAccumulatedOpacity()


// $Log: VolumeRender.C,v $
// Revision 1.19.2.13  2010/12/22 13:02:17  dongqing
// add more uncleansed check for space leaping loading ULD
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.12  2010/12/21 22:54:49  dongqing
// clean up ULD volume tranisition and space leaping flag
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.11  2010/12/21 20:55:08  dongqing
// correct typo
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.10  2010/12/21 15:40:48  dongqing
// comments
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.9  2010/12/20 18:53:27  dongqing
// add VCSetting switch for using ULD volume
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.8  2010/06/14 20:23:05  dongqing
// Minor update for support more than 16 threads in rendering
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.7  2010/06/11 20:32:18  dongqing
// implement 32 threads rendering
//
// Revision 1.19.2.6  2010/06/08 21:12:03  dongqing
// change code to allow spinning out 16 threads
//
// Revision 1.19.2.5  2010/02/01 19:32:58  dongqing
// change the RayCastObliqueSlice for handling the selection for both ray depth situations
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.4  2010/01/29 22:55:14  dongqing
// switch to return the actual ray depath
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.3  2009/11/10 19:03:48  dongqing
// Improving fly speed for non-cleansed images by using ULD volume
//
// Revision 1.19.2.2  2007/05/07 18:41:37  dongqing
// remove debug log info
//
// Revision 1.19.2.1  2007/05/03 15:24:19  dongqing
// add function CastSingleRayForUDview() to fix lumen coverage problem
//
// Revision 1.19  2006/11/16 20:40:22  dongqing
// fix the black spots in the UDSV. It is related the fValue and air lumen threshold.
//
// Revision 1.18  2006/10/09 17:29:47  frank
// Issue #5048: fixed stray pixel writing during 3D/UD synchronization clicks
//
// Revision 1.17  2006/10/04 20:55:51  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.16  2006/09/28 15:59:11  frank
// formatting
//
// Revision 1.15  2006/09/26 20:04:55  frank
// Fixed up 3D segment view
//
// Revision 1.14  2006/09/19 16:12:11  frank
// fixing up the experimental layouts
//
// Revision 1.13  2006/02/02 20:01:46  geconomos
// updated for changes to SystemInfo
//
// Revision 1.12  2006/01/27 20:31:38  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.11  2005/12/06 20:51:30  frank
// manually copied fix from branch to trunk to accelerate testing of this critical bug fix (50% surface coverage).
//
// Revision 1.10  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.9  2005/11/04 18:09:58  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.8.2.3  2005/12/06 21:53:11  jmeade
// Issue 4551: Don't show slice indicators when flying in either dataset.
//
// Revision 1.8.2.2  2005/12/06 18:02:15  frank
// Issue #4552: Fixed lumen coverage pecentage
//
// Revision 1.8.2.1  2005/11/04 18:23:08  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.9  2005/11/04 18:09:58  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.8  2005/11/04 01:14:07  jmeade
// updated
//
// Revision 1.7  2005/10/28 18:02:08  geconomos
// code cleanup
//
// Revision 1.6  2005/10/28 13:32:04  geconomos
// added ability to render into a fixed or adpative image buffer
//
// Revision 1.5  2005/10/25 13:48:11  geconomos
// reworked threading to support more than 4 threads
//
// Revision 1.4  2005/10/13 18:11:31  vxconfig
// moved settings file relative to executable in config folder
// removed numbers from arrows prior to revealing the CAD findings list
//
// Revision 1.3  2005/09/29 13:33:13  vxconfig
// made the difference volume a new vx block volume
//
// Revision 1.2  2005/09/27 13:52:32  vxconfig
// added 3D field of view
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.3  2005/09/09 14:27:16  geconomos
// Issue# 4349: Image resolution not increasing at higher framerates.
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.30.2.3.2.4.2.1  2005/06/27 17:28:47  geconomos
// fixed the bounds check for the volume rendering camera (frank)
//
// Revision 3.30.2.3.2.4  2005/06/14 19:13:26  geconomos
// Un-did changes to high resolution 3d rendering.
//
// Revision 3.30.2.3.2.3  2005/06/10 18:19:41  geconomos
// Added mechansim to render a high quality image immediately after flight navigation has stopped.
//
// Revision 3.30.2.3.2.2  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.30.2.3.2.1  2005/04/22 01:36:14  frank
// Issue #4108: Re-working 2D/3D projection
//
// Revision 3.30.2.3  2005/02/15 18:42:39  jmeade
// merge from vc1.4 branch.
//
// Revision 3.30.2.2.2.1  2005/02/04 23:57:09  jmeade
// Allowing for variances in 2D and 3D volume coordinate units.
//
// Revision 3.30.2.2  2003/04/04 20:13:26  kevin
// Issue 2835: The threads were crashing on creatiion. 2 things were
// done to fix this:
// (1) Handle rendierng if the threads are non-existant by switching
// to sequential rendering (this should have been in there nayway)
// (2) Create the threads early when there is plenty of memory blocks
//
// Revision 3.30.2.1  2003/02/21 19:14:56  jmeade
// One-line FlushAccumulationBuffer() method made inline.
//
// Revision 3.30  2003/01/29 18:53:38  jmeade
// Removed code duplication in RenderThread and derivatives.
//
// Revision 3.29  2003/01/22 22:29:34  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.28  2003/01/07 15:08:30  kevin
// Render threads switched to worker-threads rather than windows-threads
// AND
// Added dynamic load blancing to the parallel rednering providing better performance
// with highe PE utilization. Also cleaned up the in-plane adaptive sampling code
// a bit while doing this
//
// Revision 3.27  2001/01/09 17:36:21  kevin
// Adjust Debug output for endoscopic frame rate to also include
// adaptive render size for the current frame and the average since the
// viewer started.
//
// Revision 3.26  2002/12/24 03:47:47  jmeade
// Bug: Measure list not saved in session b/c there were two copies of it!
//
// Revision 3.25  2002/12/23 22:28:08  kevin
// Adjust the rendersize based on the frame rate
//
// Revision 3.24  2002/11/24 17:44:28  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
//
// Revision 3.23  2002/11/11 14:26:57  kevin
// Dim the border for segmetnal view
//
// Revision 3.22  2002/10/21 18:51:53  kevin
// Kevin's refactored S/P Registration with new heirarchical algorithm
//
// Revision 3.21  2002/09/13 00:54:17  jmeade
// Move paint volume characteristics to the VolumePaint class.
//
// Revision 3.20  2002/09/02 20:38:14  kevin
// Render a specific sized segment and mark which part the user should focus on
//
// Revision 3.19  2002/08/28 19:29:09  kevin
// Lots of updates to segmental viewing
//
// Revision 3.18  2002/08/22 13:20:08  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.17  2002/08/20 19:23:41  kevin
// 2D and 3D segment views.
//
// Revision 3.16  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.15  2002/04/12 02:20:41  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.14  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.13.2.2  2002/02/26 16:54:32  binli
// issue 1543: inverse image when taking snapshot.
//
// Revision 3.13.2.1  2002/02/06 23:52:51  kevin
// Added color schemes to handle paintnig information for color blind people (NOTE: we should
// expand this to all our color information in the next release. e.g. centerline color, bookmark color etc)
//
// Revision 3.13  2002/01/10 18:04:09  kevin
// Made diff volumes lazy-load since they will not be used in the common case
//
// Revision 3.12  2001/11/14 01:34:16  kevin
// Cleaned up alot of Supine/Proneregistration stuff, fixed some
// of the multi-rendering and added feature match pair rendering
//
// Revision 3.11  2001/11/12 23:00:17  kevin
// Cleanup
//
// Revision 3.10  2001/11/09 04:55:01  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.9  2001/11/07 16:57:37  binli
// Added a new version to get uncleansed sample method which utilizes the diff volume
// (position in Volume Coordinate/check if position is inside volume)
//
// Revision 3.7  2001/11/06 15:25:02  kevin
// Added a get uncleansed sample method which utilizes the diff volume
//
// Revision 3.6  2001/11/02 16:13:55  binli
// 2D render from block vol (.diff)
//
// Revision 3.5  2001/01/13 09:47:40  kevin
// Fixed the bug in uncleased rendering where both supine and prone datasets
// pointed the curent study for the block diff volume
//
// Revision 3.4  2001/11/01 19:52:59  kevin
// Added 3D Uncleansed DIff-Block_volume rendering
//
// Revision 3.3  2001/10/31 00:44:04  soeren
// removed memeory leaks and improved resetting of Studybrowser regarding
// the volumes
//
// Revision 3.2  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.1  2001/10/25 13:19:46  kevin
// Added hooks for avi/mpeg saving
//
// Revision 3.0.2.1  2001/10/31 01:16:52  soeren
// removed memeory leaks and improved resetting of Studybrowser regarding the volumes
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 08 2001 21:37:50   jmeade
// Specify study to be rendered (removed legacy assumption of rdrGlobal.m_pStudy)
// Resolution for 1078: Registration support
// 
//    Rev 2.0   Sep 16 2001 23:40:30   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:22   ingmar
// Initial revision.
// Revision 1.111  2001/09/07 23:14:22  jmeade
// Bug fix for Track Record ID 589:  Out-of-Memory when creating bkgd render
// threads:  Ensure threads are created before rendering, render in primary thread otherwise
//
// Revision 1.110  2001/08/20 20:08:08  tushar
// removed some errors in vc2000src due to logger class
//
// Revision 1.109  2001/07/25 22:52:17  binli
// ID 89: new exception handling.
//
// Revision 1.108  2001/05/29 17:25:40  jmeade
// Fixed changes from (and continued) code review, particularly with translucent window
//
// Revision 1.107  2001/05/25 20:28:59  kevin
// Various coding conventions related to ReaderGlobal and VolumeRender
//
// Revision 1.105  2001/05/07 22:35:53  soeren
// rewrote the whole SystemInfo class
//
// Revision 1.104  2001/05/07 00:51:27  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.102  2001/04/30 02:20:03  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.101  2001/04/29 23:17:33  kevin
// Put changes from FILE_1.99 back in  (seems CVS somehow
// missed that revision, may have happened whne the RAID
// was down the the repository was moved back and forth)
//
// Revision 1.100  2001/04/29 22:42:41  kevin
// Variable naming for coding standards in VolumeRender Class
//
// Revision 1.98  2001/04/12 19:20:49  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.97  2001/04/12 15:58:20  dave
// added namespace vx for exceptions
//
// Revision 1.96  2001/04/11 20:50:26  kevin
// Coding Standard updates of variable names, types and
// comments in the VolumeRender class.
//
// Revision 1.95  2001/04/11 14:07:27  frank
// Renamed class members of Fields.
//
// Revision 1.94  2001/04/07 02:58:29  jmeade
// comments to clarify use of frame sleep timer on render
//
// Revision 1.93  2001/04/05 23:31:02  jmeade
// removed header revision history
//
// Revision 1.92  2001/04/05 23:24:40  jmeade
// use persistent thread for rendering
//
// Revision 1.91  2001/03/21 14:59:49  frank
// Updated transfer function class for coding guideline compliance.
//
// Revision 1.90  2001/03/16 17:11:09  frank
// Updated Fields class variables for new coding standards.
//
// Revision 1.89  2001/03/16 16:24:33  frank
// Updated variable names in Study class according to new coding standards.
//
// Revision 1.88  2001/03/16 14:38:03  frank
// Updated ReaderGlobal variables for new coding standards.
//
// Revision 1.87  2001/02/12 17:14:52  jeff
// throw viatronix exceptions for V1K errors instead of aborting
//
// Revision 1.86  2001/02/05 16:52:31  frank
// Better space leaping with these parameters.
//
// Revision 1.85  2001/02/02 14:21:53  frank
// Scheduled rendering threads with higher priority and processor affinity.
//
// Revision 1.84  2001/02/02 13:00:28  frank
// Adjusted threshold some.
//
// Revision 1.83  2001/02/02 12:43:01  frank
// Added space leaping threshold as member.  Tried space leaping with a higher threshold and no stepping back.
//
// Revision 1.82  2001/01/31 13:45:36  frank
// Improved space leaping noise.
//
// Revision 1.81  2001/01/31 13:11:24  frank
// Rendered black on x-section view outside volume.
//
// Revision 1.80  2001/01/30 04:41:31  kevin
// FIxed the live update of painted information
//
// Revision 1.79  2001/01/24 19:18:20  liwei
// Software volume rendering for overview
//
// Revision 1.78  2001/01/24 15:55:22  jeff
// Aren't all bools supposed to start m_b...?
//
// Revision 1.77  2001/01/24 14:01:29  frank
// Improved image accumulation and rendering.
//
// Revision 1.76  2001/01/23 18:26:08  frank
// Consolidated point data into Point<> data.
//
// Revision 1.75  2001/01/23 18:17:34  frank
// Saved a unique view identifier for view caching purposes.
//
// Revision 1.74  2001/01/23 17:41:51  frank
// Minor speed optimization.
//
// Revision 1.73  2001/01/19 21:07:09  kevin
// Added OnStudyLoadedAndDone Verification function to perform
// tasks once both of these conditions are true. The function currently
// contins code to calculate the percent coverage values and to
// calculate segmnet offsets from the rectum once the skeletons have
// been smoothed and the segmnet order defined.
//
// Revision 1.72  2001/01/18 16:33:08  dave
// changed SystemInfo to inline GetCPUCount()
//
// Revision 1.71  2001/01/05 19:27:39  frank
// Moved marking functions into Study class.
//
// Revision 1.70  2001/01/05 16:09:51  frank
// Improved speed of MPR.
//
// Revision 1.69  2000/12/27 17:51:17  kevin
// CHanges to remove Mprviewer
//
// Revision 1.68  2000/12/20 14:34:38  kevin
// Initial chnges to make mpr viewer and slice viewer one 2d slice viewer class
//
// Revision 1.67  2000/12/19 14:56:35  kevin
// Added Grid Shadow
//
// Revision 1.66  2000/12/19 14:33:44  frank
// Removed warning
//
// Revision 1.65  2000/12/18 23:38:40  kevin
// Removed 2D unwrap-painting and clenaed up it's memory and flags.
// (NOTE; This seemed to be causing a weird memory crash where
// there was memory, but the call to new was creating an out-of-mem
// exception and there was plenty of memory left. Since this code never
// caused problems before, I am concerned that this is really some
// other problem rearing it's ugly head)
//
// Revision 1.64  2000/12/11 21:39:35  dave
// slight changes in Study.C,Fields.C,VolumeRender.C in try catch
//
// Revision 1.63  2000/12/11 17:35:37  frank
// Defaults image display to true
//
// Revision 1.62  2000/12/11 14:44:58  frank
// Added toggle for main image display
//
// Revision 1.61  2000/12/05 19:53:32  frank
// Cleaned up memory managment
//
// Revision 1.60  2000/11/24 14:03:31  frank
// Plugged resource leak
//
// Revision 1.59  2000/11/21 15:04:42  frank
// Plugged resource leak
//
// Revision 1.58  2000/11/20 19:07:06  kevin
// Made INS block Again (finally not being used for painting)
//
// Revision 1.57  2000/11/20 15:55:30  frank
// Enabled toggling accumulation buffer
//
// Revision 1.56  2000/11/20 03:37:01  kevin
// Switched to "paint volume" schema.
//
// Revision 1.55  2000/11/19 23:47:05  kevin
// Changed RGBvol to PaintVol to reflect new meaning
//
// Revision 1.54  2000/11/19 22:48:17  kevin
// Made PaintVol a flag
//
// Revision 1.53  2000/11/18 18:38:07  kevin
// Removed #if for sld counting and painting (didn't work)
//
// Revision 1.52  2000/11/17 21:15:55  kevin
// FIxed some errors from changing the rendersize to be non-power-of-two
//
// Revision 1.51  2000/11/17 04:23:02  kevin
// Added resizing of raycasting window to non-power-of-two
//
// Revision 1.50  2000/11/16 22:15:19  kevin
// Begin to handle non-power-of-2 rendering
//
// Revision 1.49  2000/11/16 19:48:01  jeff
// SystemInfo class now stores number of processors
//
// Revision 1.48  2000/11/16 20:46:53  kevin
// FIxed parallel depth and surface counting
//
// Revision 1.47  2000/11/16 18:13:57  kevin
// Changed the color of the painted surfaces
//
// Revision 1.46  2000/11/16 15:19:55  jeff
// Number of processors data member in ReaderGlobal, used to determine
//  whether to use threads for rendering
//
// Revision 1.45  2000/11/14 19:25:44  frank
// Improved the speed of the accumulation buffer
//
// Revision 1.44  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.43  2000/11/14 14:28:07  frank
// Added accumulation buffer
//
// Revision 1.42  2000/11/14 04:38:06  kevin
// Changed marking and painting to utilize bits within the ins vol
//
// Revision 1.41  2000/11/14 02:23:12  kevin
// Created inline CastSingleRay and numerous inlines for the pieces within
//
// Revision 1.40  2000/11/13 12:13:17  frank
// Updated for C++ coding standards
//
// Revision 1.39  2000/11/10 20:31:36  kevin
// Created VolumeRenderOrtho to better encapsulate and
// seperate the different things. Fixed the #defines for
// inline assembly code to spped up rounding and trunc.
// Added these to VolumeRenderOrtho and cleaned up
// that raycasting loop some
//
// Revision 1.38  2000/11/10 13:56:22  frank
// Inline assembler misbehavin'
//
// Revision 1.37  2000/11/10 13:15:06  frank
// Unsuccessful attempts to speed up and simplify ray casting.
//
// Revision 1.36  2000/11/09 20:11:08  frank
// Moved Study-specific information to study class from ReaderGlobal
//
// Revision 1.35  2000/11/09 19:31:04  liwei
// A bug of hybrid view fixed
//
// Revision 1.34  2000/11/09 17:00:41  jeff
// Clarifying parallel/non-parallel rendering
//
// Revision 1.33  2000/11/09 02:55:04  kevin
// Many changes to different methods of visible surface determination
//
// Revision 1.32  2000/11/08 17:38:56  frank
// Added m_study to ReaderGlobal
//
// Revision 1.31  2000/11/08 04:39:59  frank
// Moved ecv into Study class
//
// Revision 1.30  2000/11/06 21:26:14  liwei
// Minor changes for 2D painting
//
// Revision 1.29  2000/11/06 20:13:30  liwei
// CastRay now uses space leaping
//
// Revision 1.28  2000/11/06 19:11:09  liwei
// 2D painting enabled in adaptive sampling
//
// Revision 1.27  2000/11/06 16:18:31  liwei
// 2D painting is displayed with hardware
//
// Revision 1.26  2000/11/03 20:32:13  dave
// converted back to unsigned dim.
//
// Revision 1.25  2000/11/03 19:57:03  liwei
// Using hardware blending for 2D marking
//
// Revision 1.24  2000/11/02 20:09:12  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.23  2000/11/01 17:57:45  liwei
// Hybrid view added
//
// Revision 1.22  2000/10/31 14:37:40  frank
// Merged in new segment structure
//
// Revision 1.21  2000/10/30 13:36:40  kevin
// Added MOving the Magic Window around
//
// Revision 1.20  2000/10/29 17:41:23  kevin
// Added two surface count variables (So we don't have to
// put mutex locks around the critical increment code
// so that it remains quick. This is possible since we are forcing
// onlly ever 2 PEs)
// AND
// Fixed the counting to count the entire neighborhood
//
// Revision 1.19  2000/10/27 20:36:58  michael
// add adaptive sampling for rendering
//
// Revision 1.18  2000/10/27 01:12:11  kevin
// Lots of changes to Marking
//
// Revision 1.17  2000/10/25 14:05:43  ingmar
// put GetDataPtr back
//
// Revision 1.16.2.1.2.1  2000/10/28 20:39:14  ingmar
// merged v1k-newSegments branch and new iterators
//
// Revision 1.16.2.1  2000/10/26 13:28:33  frank
// Added new segment structure
//
// Revision 1.16  2000/10/20 17:15:08  kevin
// Added multiple different projection methods
//
// Revision 1.15  2000/10/13 22:48:45  kevin
// Fixed Orthogonal SHadow colors
//
// Revision 1.14  2000/10/12 21:05:44  kevin
// Added Axial, Sagittal and Coronal Flags. Also cleaned up a little
//
// Revision 1.13  2000/10/12 02:46:52  kevin
// Cleaned up so no longer used marking code
//
// Revision 1.12  2000/10/09 23:35:01  kevin
// Changes to Volume Render Color
//
// Revision 1.11  2000/10/04 14:26:11  kevin
// Removed rdrGlobal.m_volume redirection
//
// Revision 1.10  2000/09/30 13:10:57  kevin
// Removed performance measurement
//
// Revision 1.9  2000/09/22 17:48:03  kevin
// Added perfromance measuremnet
//
// Revision 1.8  2000/09/21 19:41:11  kevin
// Changes to unwrapped display
//
// Revision 1.7  2000/09/19 12:52:44  frank
// Added memory allocation check
//
// Revision 1.6  2000/09/16 01:33:32  kevin
// Cleanup
//
// Revision 1.5  2000/09/15 20:14:21  kevin
// Cleaned up a few pieces of dead code
//
// Revision 1.4  2000/09/12 20:32:24  kevin
// Removed .ins field
//
// Revision 1.3  2000/09/11 20:04:25  kevin
// Updated hooks for volume measurement
//
// Revision 1.2  2000/09/11 19:03:11  frank
// Restricted shadow display to static scenes only
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
// 
// 54    9/09/00 9:01a Kevin
// Adjusted spaceleapconservation factor
// 
// 53    9/07/00 4:02p Frank
// Added new colon unwrapping features:
// -green composited overlay
// -continuous examination confidence (compared to binary)
// -percent examined
// 
// 52    9/06/00 3:12p Frank
// Prepared to update colon unwrapping visualization
// 
// 51    9/06/00 11:12a Kevin
// Fixed marking of unraveled image
// 
// 50    9/05/00 1:34a Ingmar
// now works with new naming
// 
// 49    9/02/00 4:20p Kevin
// 
// 48    9/01/00 10:23a Kevin
// DFS for unwrap debug
// 
// 47    8/31/00 4:19p Frank
// Added spherical projection visualization mode
// 
// 46    8/28/00 12:12p Antonio
// updated header and footer
//
//*****************  Version 45  *****************
//User: Kevin           Date:  8/23/00  Time: 12:25p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added SYSMODAL to messageboxes
//
//*****************  Version 44  *****************
//User: Kevin           Date:  8/22/00  Time: 11:14p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed slice positioning to utilize "orthogonalPoint"
//
//*****************  Version 43  *****************
//User: Kevin           Date:  8/22/00  Time:  7:17p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Don't go into "fast" mode if orthognal shadow
//
//*****************  Version 42  *****************
//User: Kevin           Date:  8/18/00  Time: 11:51a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added MPR Measure
//
//*****************  Version 41  *****************
//User: Frank           Date:  8/18/00  Time: 11:01a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added colored orthogonal slice viewing in navigation view
//
//*****************  Version 40  *****************
//User: Frank           Date:  8/17/00  Time: 10:44a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added orthogonal slice shadows
//Fixed cross-section slice shadow location
//
//*****************  Version 39  *****************
//User: Kevin           Date:  8/14/00  Time:  8:33p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added RGB marked volume writing for Revli viewing
//
//*****************  Version 38  *****************
//User: Kevin           Date:  8/13/00  Time:  4:30p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Visible Rendering Marking
//
//*****************  Version 37  *****************
//User: Frank           Date:  8/11/00  Time:  2:01a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Updated to reflect new Volume classes
//
//*****************  Version 36  *****************
//User: Frank           Date:  8/10/00  Time:  4:20p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed temporal jittering
//
//*****************  Version 35  *****************
//User: Kevin           Date:  8/09/00  Time: 11:43a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added parallel s/w volume rendering
//
//*****************  Version 34  *****************
//User: Jeff            Date:  8/08/00  Time: 10:31p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed calls to glViewport(), which need be called only when
//resizing the enclosing view window
//
//*****************  Version 33  *****************
//User: Kevin           Date:  8/08/00  Time:  9:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed RearView rendering hooks
//
//*****************  Version 32  *****************
//User: Frank           Date:  8/08/00  Time:  3:13p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added fast mode for a slight performance increase
//
//*****************  Version 31  *****************
//User: Frank           Date:  8/08/00  Time:  1:30p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed MPR slice display
//
//*****************  Version 30  *****************
//User: Kevin           Date:  8/06/00  Time:  9:58a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  moved m_colonWall Thershold ot readerGloabl (from fields) and moved
//the reading of the inf file
//
//*****************  Version 29  *****************
//User: Kevin           Date:  8/01/00  Time: 10:23a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Code Cleanup after changeing to new file format
//
//*****************  Version 28  *****************
//User: Kevin           Date:  8/01/00  Time:  9:13a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed to use the new pipeline file formats
//
//*****************  Version 27  *****************
//User: Kevin           Date:  7/31/00  Time:  3:20p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added leaps/ray and samples/ray debug
//
//*****************  Version 26  *****************
//User: Kevin           Date:  7/31/00  Time:  3:04p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 25  *****************
//User: Kevin           Date:  7/26/00  Time:  8:51p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added the use of .sld (space lead distance" file. But it did not
//work, so commented it out (left it in though until v2000 gets it computed
//correctly)
//
//*****************  Version 24  *****************
//User: Frank           Date:  7/26/00  Time: 11:09a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Small changes to make it work equally well
//with OldVolume and new Volume
//
//*****************  Version 23  *****************
//User: Frank           Date:  7/26/00  Time: 11:01a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Made the OldVolume interface just like Volume
//
//*****************  Version 22  *****************
//User: Frank           Date:  7/26/00  Time: 10:45a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Moved MPR functionality into VolumeRender
//
//*****************  Version 21  *****************
//User: Frank           Date:  7/26/00  Time:  9:40a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wored OldVolume variables in
//anticipation of new Volume class
//
//*****************  Version 20  *****************
//User: Frank           Date:  7/25/00  Time:  9:13a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added SkeletonRender class.
//Updated with STL string class.
//
//*****************  Version 19  *****************
//User: Kevin           Date:  7/24/00  Time:  8:14p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added translucent rendering as a "magic window" with context in
//surface rendering
//
//*****************  Version 18  *****************
//User: Kevin           Date:  7/23/00  Time:  4:47p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Better strucure for performing adaptive sampling. This one barely
//slows downat all. SO far only renders small image and does a bilnear blowup, no
//checking for where to cast more rays. 
//
//*****************  Version 17  *****************
//User: Kevin           Date:  7/23/00  Time:  3:35p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  First stab at Adaptive Sampling. This version is very slow. I will
//try a second method next.
//
//*****************  Version 16  *****************
//User: Kevin           Date:  7/21/00  Time:  3:03p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed moving of "top" light
//
//*****************  Version 15  *****************
//User: Kevin           Date:  7/20/00  Time: 10:17p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Moving Lights and ifxed some distance measuring stuff
//
//*****************  Version 14  *****************
//User: Kevin           Date:  7/19/00  Time:  8:17p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  oops "2"
//
//*****************  Version 13  *****************
//User: Kevin           Date:  7/19/00  Time:  8:07p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Renamed ray casting vectors
//
//*****************  Version 12  *****************
//User: Kevin           Date:  7/19/00  Time:  3:57p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added global Field-Of-View for rear and forward view
//
//*****************  Version 11  *****************
//User: Kevin           Date:  7/19/00  Time:  2:32p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  FIxed the KA so that fine textures show up again (like on the 256^2
//SGI version)
//
//*****************  Version 10  *****************
//User: Frank           Date:  7/18/00  Time:  3:56p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed "using namespace *" from header files to prevent ambiguities
//
//*****************  Version 9  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 9
//Comment:
//  
//
//*****************  Version 8  *****************
//User: Jeff            Date:  7/18/00  Time:  1:40p
//Checked in $/v1k/src/ReaderLib
//Comment:
//  class Global --> class ReaderGlobal
//
//*****************  Version 7  *****************
//User: Frank           Date:  7/17/00  Time:  4:19a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Improved the performance of ray casting by 16%
//
//*****************  Version 6  *****************
//User: Kevin           Date:  7/14/00  Time:  4:36p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Made the rear view point backwards and render at half the resolution
//
//*****************  Version 5  *****************
//User: Jeff            Date:  7/14/00  Time:  2:42p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Added preliminaries for forward OR rearview volume rendering
//
//*****************  Version 4  *****************
//User: Kevin           Date:  7/14/00  Time:  2:20p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Changed RGBA of floats to RGB of char and moved
//so it doesn't always grab large memory untill needed
//
//*****************  Version 3  *****************
//User: Kevin           Date:  7/13/00  Time:  3:46p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  
//
//*****************  Version 2  *****************
//User: Kevin           Date:  7/13/00  Time:  1:20p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Lots of perofrmance nehncents, debug toggles and code cleanup
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created VolumeRender.C
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/VolumeRender.C,v 1.19.2.13 2010/12/22 13:02:17 dongqing Exp $
// $Id: VolumeRender.C,v 1.19.2.13 2010/12/22 13:02:17 dongqing Exp $
