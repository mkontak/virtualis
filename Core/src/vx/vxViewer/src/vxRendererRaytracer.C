// $Id: vxRendererRaytracer.C,v 1.28 2007/04/02 21:28:39 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererRaytracer.h"
#include "vxEnvironment.h"
#include "vxRendererUtils.h"
#include "vxElementInt4.h"
#include "vxVisualizationPreferences.h"
#include "vxRendererUtils.h"
#include "vxVisualizationGraph.h"
#include "SingleLock.h"
#include "vxViewerType3D.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.28 $"


/** 
 * Constructor
 *
 * @param environment the environment
 */
vxRendererRaytracer::vxRendererRaytracer( vxEnvironment & environment ) : vxRenderer( environment ) ,
  m_depthBuffer( 1.0F )
{
} // vxRendererRaytracer( vxEnvironment & environment )


/**
 * Destructor
 */
vxRendererRaytracer::~vxRendererRaytracer()
{
} // ~vxRendererRaytracer()


/**
 * Initialize the render environment
 */
void vxRendererRaytracer::Initialize()
{
  vxRenderer::Initialize();

  glGenTextures( 1, &m_textureId );
    
} // Initialize() 


/**
 * Uninitialize routine
 */
void vxRendererRaytracer::Uninitialize()
{
  if( m_textureId != 0 )
    glDeleteTextures( 1, &m_textureId );
  
} // Uninitialize()



/**
 * Render
 */
void vxRendererRaytracer::Render()
{
  vxRenderBuffer & buffer = const_cast< vxRenderBuffer & >( GetEnvironment().GetRenderBuffer() );

  // lock down the buffer
  SingleLock<CriticalSection> lock( &buffer.GetSyncObject(), true );

  // is this a 3d view? (we only use the depth buffer for 3d )
  const bool bIs3d = dynamic_cast< vxViewerType3D * >( GetEnvironment().GetViewerType() ) != NULL ? true : false;

  if( GetEnvironment().GetRenderBuffer().IsModified() )
  {
    UpdateTexture();
  
    // only update depth buffer for 3d (expensive)
    if( bIs3d )
      UpdateCachedDepthBuffer();

    // store the aspect ratio when texture is updated
    m_imageAspectRef = GetEnvironment().GetWinRect().GetViewportSize();
  }

  // only update depth buffer for 3D and if the viewport dimensions hasn't changed
  if( bIs3d && m_imageAspectRef == GetEnvironment().GetWinRect().GetViewportSize() )
    DrawCachedDepthBuffer();

  DrawTexture();
  
  // draw the warning indicator if the image of the highest quality
  if( buffer.GetState() == vxRenderBuffer::LOW )
    vxRendererUtils::DrawImageQualityWarningRectangle( GetEnvironment().GetWinRect(), true );

  else if( buffer.GetState() == vxRenderBuffer::INTERMEDIATE )
    vxRendererUtils::DrawImageQualityWarningRectangle( GetEnvironment().GetWinRect(), false );

  // rendering statistics
  if( vxVisualizationPreferences::GetShowStatistics() )
    vxVisualizationGraph::DrawStatistics( GetEnvironment() );
  
  // bounding box is for debugging purposes
  static bool bDrawBoundingBox = false;
  if( bDrawBoundingBox )
  {
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glEnable( GL_DEPTH_TEST );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    vxRendererUtils::GLDrawBox(Box<float4>(Point<float4>(0,0,0), Point<float4>(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim())), PixelRGBA<float4>(0,1,0,1));
    glPopAttrib();
  }
} // Render()


/**
 * Updates the texture from the associated vxRenderBuffer
 */
void vxRendererRaytracer::UpdateTexture()
{
  vxRenderBuffer & buffer = const_cast< vxRenderBuffer & >( GetEnvironment().GetRenderBuffer() );

  // update the texture for the renderer
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glBindTexture( GL_TEXTURE_2D, m_textureId );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  
  glTexImage2D( GL_TEXTURE_2D, 
                0, 
                GL_RGBA, 
                buffer.GetBufferSize().X(), 
                buffer.GetBufferSize().Y(), 
                0, 
                GL_RGBA, 
                GL_UNSIGNED_BYTE, 
                buffer.GetColorImage().GetDataPtr() );

  GetEnvironment().GetRenderBuffer().SetModified( false );
} // UpdateTexture()


/**
 * Draws the image stored in the texture
 */
void vxRendererRaytracer::DrawTexture()
{
  vxRenderBuffer & buffer = const_cast< vxRenderBuffer & >( GetEnvironment().GetRenderBuffer() );

  // preserve proper aspect ratio
  const float4 xmax = buffer.GetRenderSize().m_x / static_cast< float4 >( buffer.GetBufferSize().m_x );
  const float4 ymax = buffer.GetRenderSize().m_y / static_cast< float4 >( buffer.GetBufferSize().m_y );

  const std::pair< int2, int2 > & size =  GetEnvironment().GetWinRect().GetViewportSize();

  const float4 fImageAspectRatio = m_imageAspectRef.first / static_cast< float4 >( m_imageAspectRef.second ); 
  const float4 fViewportAspectRation = size.first / static_cast< float4 >( size.second );
 
  float4 fWidth( size.first ), fHeight( size.second );
  if( fImageAspectRatio < fViewportAspectRation )
  {
    fHeight = size.second;
    fWidth =  ( static_cast< int4 >( size.second ) * m_imageAspectRef.first ) / static_cast< float4 >( m_imageAspectRef.second );
  }
  else
  {
    fWidth = size.first;
    fHeight =  ( static_cast< int4 >( size.first ) * m_imageAspectRef.second ) / static_cast< float4 >( m_imageAspectRef.first );
  }
  
  const float4 x = ( static_cast< int4 >( size.first ) - fWidth ) / 2.0F;
  const float4 y = ( static_cast< int4 >( size.second ) - fHeight ) / 2.0F;

  // TODO: don't forget about blending for fusion
  glPushAttrib( GL_ALL_ATTRIB_BITS );

  glEnable( GL_TEXTURE_2D );
  glDisable( GL_BLEND );
  glDisable( GL_DEPTH_TEST );

  glBindTexture( GL_TEXTURE_2D, m_textureId );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
 
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  {
    glLoadIdentity();
    gluOrtho2D( 0.0f,  size.first, 0.0f, size.second );
    
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    {
      glLoadIdentity();
      glBegin( GL_QUADS ); 
      {
        glTexCoord2f( 0.0,  0.0 );  glVertex2f( x,            y );
        glTexCoord2f( 0.0,  ymax ); glVertex2f( x,            y + fHeight );
        glTexCoord2f( xmax, ymax ); glVertex2f( x + fWidth,   y + fHeight );
        glTexCoord2f( xmax, 0.0 );  glVertex2f( x + fWidth,   y );
      } 
      glEnd();
    }
    glPopMatrix();
  }
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();

  glDisable( GL_TEXTURE_2D );
  glPopAttrib();
} // DrawTexture()


/**
 * Updates the cached depth buffer from the render buffer's depth image. The depth buffer is interpolated to fill
 * the entire viewport. 
 */
void vxRendererRaytracer::UpdateCachedDepthBuffer()
{
  // grab the render buffer associated with this environment
  vxRenderBuffer & buffer = const_cast< vxRenderBuffer & >( GetEnvironment().GetRenderBuffer() );

  // get the distances to the near and far planes
  const float4 fDistanceFarPlane  = GetEnvironment().GetViewerType()->GetFarPlane();
  const float4 fDistanceNearPlane = GetEnvironment().GetViewerType()->GetNearPlane();

  Triple< uint4 > originalBufferSize =  buffer.GetRenderSize();

  // resize cached depth buffer to the viewport size
  m_depthBuffer.Resize( Triple< uint4 >( GetEnvironment().GetWinRect().GetViewportWidth(), GetEnvironment().GetWinRect().GetViewportHeight(), 0 ) );

  const bool bCameraIsPersepctive = GetEnvironment().GetCamera()->GetProjectionType() == vxCamera::PERSPECTIVE;
  const float4 fOneOverNearPlane = 1.0F / fDistanceNearPlane;
  const float4 fOneOverFarPlane  = 1.0F / fDistanceFarPlane;
  const float4 fOneOverNearMinusOneOverFar = fOneOverNearPlane - fOneOverFarPlane;
  
  // resample the render buffer's depth into m_depthBuffer
  for( uint4 y = 0; y < m_depthBuffer.GetDim().m_y; y++ ) 
  {
    const uint4 yy = static_cast< uint4 >( Interpolate( 0.0F, y, m_depthBuffer.GetDim().m_y, 0.0F, originalBufferSize.m_y - 1 )  );
    
    const float4 * pSourceScanline = buffer.GetDepthImageWC().GetDataPtr() +  ( yy *  buffer.GetBufferSize().m_x );

    float4 * pDestinationScanline = m_depthBuffer.GetDataPtr() +  ( y * m_depthBuffer.GetDim().m_x );

    for( uint4 x = 0; x < m_depthBuffer.GetDim().m_x; x++ ) 
    {        
      const uint4 xx = static_cast< uint4 >( Interpolate( 0.0F, x, m_depthBuffer.GetDim().m_x - 1, 0.0F, originalBufferSize.m_x - 1 ) );

      const float4 fDepth = *( pSourceScanline + xx );
   
      if( bCameraIsPersepctive )
        *pDestinationScanline++ = ( fOneOverNearPlane - 1 / fDepth ) / fOneOverNearMinusOneOverFar;
      else
        *pDestinationScanline++ = Bound( 0.0F, Interpolate( fDistanceNearPlane, fDepth, fDistanceFarPlane,  0.0F, 1.0F ), 1.0F );
    }
  }
} // CreateDepthBuffer()


/**
 * Draws the cached depth buffer into the entire viewport
 */
void vxRendererRaytracer::DrawCachedDepthBuffer()
{
  glPushAttrib( GL_ALL_ATTRIB_BITS );

  glEnable( GL_DEPTH_TEST );

  glDrawPixels( m_depthBuffer.GetDim().m_x, m_depthBuffer.GetDim().m_y, GL_DEPTH_COMPONENT, GL_FLOAT, m_depthBuffer.GetDataPtr() );

  glDisable( GL_DEPTH_TEST );
    
  glPopAttrib();
} // DrawCachedDepthBuffer()




// $Log: vxRendererRaytracer.C,v $
// Revision 1.28  2007/04/02 21:28:39  gdavidson
// Added IsModified to the vxRenderBuffer
//
// Revision 1.27  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.26  2007/01/25 22:43:41  gdavidson
// Added a condition to display an intermediate render level
//
// Revision 1.25  2007/01/19 15:28:50  geconomos
// reorganized visualization preferences
//
// Revision 1.24  2007/01/18 15:45:01  geconomos
// corrected texture drawing setup
//
// Revision 1.23  2007/01/17 16:42:31  geconomos
// moved DrawStatistics to vxVisualizationGraph
//
// Revision 1.22  2006/12/06 15:28:09  geconomos
// formatting
//
// Revision 1.21  2006/12/04 20:46:38  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.20  2006/11/19 23:24:09  geconomos
// only using depth buffer for 3D
//
// Revision 1.19  2006/11/17 21:13:51  geconomos
// only using depth buffer is viewer is 3D
//
// Revision 1.18  2006/11/10 03:20:20  geconomos
// Issue 5089: aspect ratio incorrect for 3d; no longer calling flush in parent window classes when parent is resized
//
// Revision 1.17  2006/10/23 01:38:29  geconomos
// using two depth buffers for world and volume coordinates
//
// Revision 1.16  2006/10/20 20:53:02  geconomos
// added depth buffer support
//
// Revision 1.15  2006/10/05 19:44:29  geconomos
// added sychronization for access to render buffer
//
// Revision 1.14  2006/10/03 20:05:51  geconomos
// formatting
//
// Revision 1.13  2006/09/28 18:29:50  geconomos
// Issue 4963: added indicator for non-optimal image quality
//
// Revision 1.12  2006/09/28 15:52:17  geconomos
// Issue 5027: sub viewer causing main view to disappear
//
// Revision 1.11  2006/09/20 15:04:48  frank
// added preference for displaying rendering manager statistics
//
// Revision 1.10  2006/09/14 20:14:14  geconomos
// added indicator if user is currently interacting
//
// Revision 1.9  2006/09/07 13:20:33  geconomos
// expanded buffer size for debug output
//
// Revision 1.8  2006/08/31 14:07:05  geconomos
// updated debugging output to include current dataset index
//
// Revision 1.7  2006/08/23 17:55:51  geconomos
// changed to display vxRenderBuffer from associated environment
//
// Revision 1.6  2006/08/03 14:14:47  geconomos
// removed all rendering functionailty; now just texture maps the render buffer contained in the associated environment
//
// Revision 1.5  2006/07/18 16:21:23  geconomos
// modified CreateTexture and DrawTexture to take the index of the buffer to display
//
// Revision 1.4  2006/07/12 19:52:32  geconomos
// + added segmentation intenisty range properties
// + added slice shadow related properties
//
// Revision 1.3  2006/07/07 13:43:18  geconomos
// still working on final image generation
//
// Revision 1.2  2006/06/27 20:16:48  geconomos
// initial threading work
//
// Revision 1.1  2006/06/22 14:19:42  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererRaytracer.C,v 1.28 2007/04/02 21:28:39 gdavidson Exp $
// $Id: vxRendererRaytracer.C,v 1.28 2007/04/02 21:28:39 gdavidson Exp $
