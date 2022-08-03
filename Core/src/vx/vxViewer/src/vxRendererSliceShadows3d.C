// $Id: vxRendererSliceShadows3d.C,v 1.19 2008/01/21 18:12:27 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.net)

// includes
#include "StdAfx.h"
#include "vxRendererSliceShadows3d.h"
#include "vxEnvironment.h"
#include "vxRibbon.h"
#include "vxEnvironmentList.h"
#include "vxViewerTypeMPRCurved.h"
#include "vxViewerTypeMPRCurvedLuminal.h"
#include "vxViewerTypeVesselDetailEndoluminal.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeGraph.h"
#include "vxRendererUtils.h"
#include "vxVesselUtilities.h"
#include "vxManipulatorVessel.h"
#include "vxElementFloat4.h"
#include "vxVessel.h"
#include "ValueRange.h"
#include "vxRayGenerator.h"


// defines
#define FILE_REVISION "$Revision: 1.19 $"


/**
 * Constructor from vxEnvironment
 *
 * @param environment  environment
 */
vxRendererSliceShadows3d::vxRendererSliceShadows3d(vxEnvironment & environment) : vxRendererSliceShadows( environment ),
 m_pLastActiveEnvironment( NULL ),
 m_bDraggingdEndoCameraForward( false ),
 m_fPreviousVesselSectionIndex( -1.0F )

{
} // vxRendererSliceShadows3d()


/**
 * Initialize routine
 */
void vxRendererSliceShadows3d::Initialize()
{
  glGenTextures( 1, &m_textureId );
} // Initialize() 


/**
 * Uninitialize routine
 */
void vxRendererSliceShadows3d::Uninitialize()
{
  if( m_textureId != 0 )
    glDeleteTextures( 1, &m_textureId );
  
} // Uninitialize()


/**
 * Render the slice shadows
 */
void vxRendererSliceShadows3d::Render()
{
  vxRenderingMode::vxSliceShadowModeEnum eSliceShadowMode = GetEnvironment().GetRenderingMode()->GetSliceShadowMode();
  
  // early return if no rendering necessary
  if ( eSliceShadowMode == vxRenderingMode::SHADOWS_NONE )
    return;

  if( eSliceShadowMode == vxRenderingMode::SHADOWS_ACTIVE  || 
      eSliceShadowMode == vxRenderingMode::SHADOWS_ACTIVE_IMAGE ||
      eSliceShadowMode == vxRenderingMode::SHADOWS_ACTIVE_CLIPPED  )
  {
    const vxEnvironment *  pActiveEnvironment = vxEnvironment::GetActiveEnvironment();

    const bool bIsActiveEnvironment3D = dynamic_cast< vxViewerType3D * >( pActiveEnvironment->GetViewerType() );
		const bool bIsActiveEnvironmentGraph = dynamic_cast< vxViewerTypeGraph * >( pActiveEnvironment->GetViewerType() );

    pActiveEnvironment = ( bIsActiveEnvironment3D || bIsActiveEnvironmentGraph )? m_pLastActiveEnvironment : pActiveEnvironment;

    if( pActiveEnvironment )
    {
      vxViewerType * pViewerType = pActiveEnvironment->GetViewerType();
      
      if( dynamic_cast< vxViewerTypeMPRCurved* >( pViewerType ) || dynamic_cast< vxViewerTypeMPRCurvedLuminal* >( pViewerType ) )
      {
        RenderCurvedPlane( *pActiveEnvironment );
        RenderCurvedCamera( *pActiveEnvironment );
      }

      else if( dynamic_cast< vxViewerTypeMPROblique* >( pViewerType ) || dynamic_cast< vxViewerTypeMPROrtho* >( pViewerType ) )
      {
        if( eSliceShadowMode == vxRenderingMode::SHADOWS_ACTIVE )
          RenderPlaneWithShading( *pActiveEnvironment, false );  
        
        else if( eSliceShadowMode == vxRenderingMode::SHADOWS_ACTIVE_CLIPPED )
          RenderPlaneWithShading( *pActiveEnvironment, true );  
        
        else        
          RenderPlaneWithSliceImage( *pActiveEnvironment );
      }

      else if( dynamic_cast< vxViewerTypeVesselDetailEndoluminal* >( pViewerType ) || dynamic_cast< vxViewerTypeMPRCurved* >( pViewerType ) )
        RenderVesselDetailEndoluminal( *pActiveEnvironment );

      m_pLastActiveEnvironment = const_cast< vxEnvironment *>( pActiveEnvironment );
    }
  }
  else //  render all slice shadows
  {

    vxEnvironmentList * pEnvironmentList( reinterpret_cast< vxEnvironmentList * >( GetEnvironment().GetElement( vxIDs::EnvironmentList )));
    for( vxEnvironmentList::EnvironmentList::iterator iter = pEnvironmentList->GetEnvironmentList().begin(); iter != pEnvironmentList->GetEnvironmentList().end(); ++iter )
    {
      vxEnvironment * pOtherEnvironment( * iter );
      vxViewerType * pViewerType = pOtherEnvironment->GetViewerType();
      
      if( dynamic_cast< vxViewerTypeMPRCurved* >( pViewerType ) || dynamic_cast< vxViewerTypeMPRCurvedLuminal* >( pViewerType ) )
      {
        RenderCurvedPlane( *pOtherEnvironment );
        RenderCurvedCamera( *pOtherEnvironment );
      }
      else if( dynamic_cast< vxViewerTypeMPROblique* >( pViewerType ) || dynamic_cast< vxViewerTypeMPROrtho* >( pViewerType ) )
        RenderPlaneWithShading( *pOtherEnvironment, false );

      else if( dynamic_cast< vxViewerTypeVesselDetailEndoluminal* >( pViewerType )  )
        RenderVesselDetailEndoluminal( *pOtherEnvironment );
    }
  }
} // Render()


/**
 * Renders the projection of a plane with shading
 *
 * @param   otherEnvironment   environment containing plane to render
 * @param   bOutlineOnly       indicates if only the outline should be drawn
 */
void vxRendererSliceShadows3d::RenderPlaneWithShading( const vxEnvironment & otherEnvironment, bool bOutlineOnly )
{
  // get required elements from the other environment
  const vxViewerType * pOtherViewerType = otherEnvironment.GetViewerType();
  const vxCamera  * pOtherCamera = otherEnvironment.GetCamera();
  const vxViewerType * pViewerType = GetEnvironment().GetViewerType();
  const vxCamera  * pCamera =  GetEnvironment().GetCamera();
  const vxPlaneOblique< float4 > * pOtherPlane = GetPlane( otherEnvironment );
  
  // compute the four corners of intersection of the camera frustum with the plane
  Quadrilateral quad( vxUtils::ComputeObliqueQuadrilateralAnalytical( pOtherCamera, pOtherPlane, pOtherViewerType ) );

  // compute the offset for the points so that they lie in between slices
  const Normal3Df & normal = pOtherPlane->GetNormal();
  const Triplef & worlddim = otherEnvironment.GetIntensityVolume()->GetHeader().GetWorldDim();
  const Triplef & units = otherEnvironment.GetIntensityVolume()->GetHeader().GetVoxelUnits();
  const float4 fOffset = max( max( units.m_x * normal.X(), units.m_y * normal.Y() ), units.m_z * normal.Z() ) * 1.2;
  const Vector3Df offset =  normal * fOffset;

  const int4 iNumOfVertices = 4;
  Point3Df vertices[ iNumOfVertices ] = { quad.GetPoint( Quadrilateral::TOP_LEFT ), quad.GetPoint( Quadrilateral::TOP_RIGHT ), quad.GetPoint( Quadrilateral::BOTTOM_LEFT ), quad.GetPoint( Quadrilateral::BOTTOM_RIGHT ) };
    
  // don't clamp oblique
  if( dynamic_cast< vxViewerTypeMPROrtho * >( otherEnvironment.GetViewerType() ) != NULL )
  {
    // offset and clamp the points to volume dimensions
    for( int4 i = 0; i < iNumOfVertices; ++i )
    {
      Point3Df * p = &vertices[ i ];
      
      ( *p ) += offset;
      p->m_x = Bound( units.m_x, p->m_x, worlddim.m_x - units.m_x );
      p->m_y = Bound( units.m_y, p->m_y, worlddim.m_y - units.m_y );
      p->m_z = Bound( units.m_z, p->m_z, worlddim.m_z - units.m_z );
    }
  }

  // transfer points for readability
  const Point3Df planeTopLeft  = vertices[ 0 ];
  const Point3Df planeTopRight  = vertices[ 1 ];
  const Point3Df planeBottomLeft  = vertices[ 2 ];
  const Point3Df planeBottomRight  = vertices[ 3 ];

  
  // compute the distances from the camera to each corner point 
  const float4 fDistanceTopLeft     = pCamera->GetEye().GetEuclideanDistance( planeTopLeft );
  const float4 fDistanceTopRight    = pCamera->GetEye().GetEuclideanDistance( planeTopRight );
  const float4 fDistanceBottomRight = pCamera->GetEye().GetEuclideanDistance( planeBottomRight );
  const float4 fDistanceBottomLeft  = pCamera->GetEye().GetEuclideanDistance( planeBottomLeft );


  // get the distances of the far and near planes
  const float4 fDistanceNearPlane = pViewerType->GetNearPlane();
  const float4 fDistanceFarPlane  = pViewerType->GetFarPlane();


  // compute the opactity weight for each corner point
  float4 fCornerPointOpacityWeight[ 4 ] = {0.0F, 0.0F, 0.0F, 0.0F };
  fCornerPointOpacityWeight[ 0 ] = Bound( 0.1F, Interpolate( fDistanceNearPlane, fDistanceTopLeft,     fDistanceFarPlane,  1.0F, 0.1F ), 1.0F );
  fCornerPointOpacityWeight[ 1 ] = Bound( 0.1F, Interpolate( fDistanceNearPlane, fDistanceTopRight,    fDistanceFarPlane,  1.0F, 0.1F ), 1.0F );
  fCornerPointOpacityWeight[ 2 ] = Bound( 0.1F, Interpolate( fDistanceNearPlane, fDistanceBottomRight, fDistanceFarPlane,  1.0F, 0.1F ), 1.0F );
  fCornerPointOpacityWeight[ 3 ] = Bound( 0.1F, Interpolate( fDistanceNearPlane, fDistanceBottomLeft,  fDistanceFarPlane,  1.0F, 0.1F ), 1.0F );

  // get plane color
  const PixelRGBA< uint1 > color = GetSliceShadowColor( otherEnvironment );

  // opengl setup
  glPushAttrib( GL_ALL_ATTRIB_BITS );

  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  glDepthFunc( GL_LESS ); 
  glEnable( GL_DEPTH_TEST ); 
  glEnable( GL_BLEND ); 
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glShadeModel( GL_SMOOTH ); 
  
  // render plane with additional transparency
  if( !bOutlineOnly )
  {
    const float4 fOpactityAjdustment = 0.60F;  
    glBegin( GL_QUADS  );
    {    
      glColor4ub( color.R(), color.G(), color.B(), color.A() * fCornerPointOpacityWeight[ 0 ] * fOpactityAjdustment );
      vxRendererUtils::GlVertex( planeTopLeft );
      
      glColor4ub( color.R(), color.G(), color.B(), color.A() * fCornerPointOpacityWeight[ 1 ] * fOpactityAjdustment );
      vxRendererUtils::GlVertex( planeTopRight );
      
      glColor4ub( color.R(), color.G(), color.B(), color.A() * fCornerPointOpacityWeight[ 2 ] * fOpactityAjdustment );
      vxRendererUtils::GlVertex( planeBottomRight );
      
      glColor4ub( color.R(), color.G(), color.B(), color.A() * fCornerPointOpacityWeight[ 3 ] * fOpactityAjdustment );
      vxRendererUtils::GlVertex( planeBottomLeft );  
    }
    glEnd();
  }

  // render plane outline 
  glBegin( GL_LINE_LOOP );
  {
    glColor4ub( color.R(), color.G(), color.B(), color.A() * fCornerPointOpacityWeight[ 0 ] );
    vxRendererUtils::GlVertex( planeTopLeft );
    
    glColor4ub( color.R(), color.G(), color.B(), color.A() * fCornerPointOpacityWeight[ 1 ] );
    vxRendererUtils::GlVertex( planeTopRight );
    
    glColor4ub( color.R(), color.G(), color.B(), color.A() * fCornerPointOpacityWeight[ 2 ] );
    vxRendererUtils::GlVertex( planeBottomRight );
    
    glColor4ub( color.R(), color.G(), color.B(), color.A() * fCornerPointOpacityWeight[ 3 ] );
    vxRendererUtils::GlVertex( planeBottomLeft );
  }  
  glEnd();
  
  // restore opengl state
  glPopAttrib();

} // RenderPlaneWithShading( const vxEnvironment & otherEnvironment )


/**
 * Render the projection of a plane with the associated slice image
 *
 * @param   otherEnvironment   environment containing plane to render
 */
void vxRendererSliceShadows3d::RenderPlaneWithSliceImage( const vxEnvironment & otherEnvironment )
{
  // get required elements from the other environment
  const vxViewerType * pOtherViewerType = otherEnvironment.GetViewerType();
  const vxCamera  * pOtherCamera = otherEnvironment.GetCamera();
  const vxPlaneOblique< float4 > * pOtherPlane = GetPlane( otherEnvironment );
  
  // create a texture for the renderer
  vxRenderBuffer & buffer = const_cast< vxEnvironment & >( otherEnvironment ).GetRenderBuffer();
  {

    SingleLock< CriticalSection > lock( &buffer.GetSyncObject(), true );
  
    // compute the four corners of intersection of the camera frustum with the plane
    Quadrilateral quad( vxUtils::ComputeObliqueQuadrilateralAnalytical( pOtherCamera, pOtherPlane, pOtherViewerType ) );
      
    const int4 iNumOfVertices = 4;
    Point3Df vertices[ iNumOfVertices ] = { quad.GetPoint( Quadrilateral::BOTTOM_LEFT ), quad.GetPoint( Quadrilateral::TOP_LEFT ), quad.GetPoint( Quadrilateral::TOP_RIGHT ), quad.GetPoint( Quadrilateral::BOTTOM_RIGHT ) };
    Point3Df texels[ iNumOfVertices ];
    
    if( dynamic_cast< vxViewerTypeMPROrtho *>( otherEnvironment.GetViewerType() ) )
    {
      GetVerticesAndTexelsOrtho( otherEnvironment, vertices, texels );
    }
    else
    {
      GetVerticesAndTexelsOblique( otherEnvironment, vertices, texels );
    }

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL ); 

    const bool bDrawOpaque = true;
    if( bDrawOpaque )
    { 
      glDisable( GL_BLEND );
      glBegin( GL_QUADS ); 
      {
        glColor4ub( 0, 0, 0, 255 );
      
        for( int4 i = 0; i < iNumOfVertices; ++i )
          vxRendererUtils::GlVertex( vertices[ i ] );
      }
      glEnd();
    }

    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glBindTexture( GL_TEXTURE_2D, m_textureId );
    
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, buffer.GetBufferSize().X(), buffer.GetBufferSize().Y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.GetColorImage().GetDataPtr() );

    glBegin( GL_QUADS ); 
    { 
      for( int4 i = 0; i < iNumOfVertices; ++i )
      {
        glTexCoord2f( texels[i].m_x,  texels[i].m_y ); 
        vxRendererUtils::GlVertex( vertices[ i ] );
      }
     } 
    glEnd();
    glDisable( GL_TEXTURE_2D );

    glPopAttrib();
  }

} // RenderPlaneWithSliceImage( const vxEnvironment & otherEnvironment )


/**
 * Gets the vertices and texture coordinates required to draw the active ortho slice as a texture
 *
 * @param   otherEnvironment    other environment containing the plane
 * @param   vertices            vertices in world coordinates
 * @param   texels              texture coordinates
 */
void vxRendererSliceShadows3d::GetVerticesAndTexelsOrtho( const vxEnvironment & otherEnvironment, Point3Df vertices[], Point3Df texels[] )
{
  const vxCamera  * pOtherCamera = otherEnvironment.GetCamera();
  const vxPlaneOblique< float4 > * pOtherPlane = GetPlane( otherEnvironment );

  const uint2 uAxisIndex = pOtherPlane->GetNormal().GetMaxAbsComponentCoord();

  const Triplei & voldim = otherEnvironment.GetIntensityVolume()->GetHeader().GetVolDim();
  const Triplef & worlddim = otherEnvironment.GetIntensityVolume()->GetHeader().GetWorldDim();

  const int4 i0 = ( uAxisIndex + 0 ) % 3;
  const int4 i2 = ( uAxisIndex + 1 ) % 3;
  const int4 i3 = ( uAxisIndex + 2 ) % 3;
  
  const float xmax = ( voldim[ i2 ] - 1 ) / static_cast< float4 >( GetNextPowerOfTwo( voldim[ i2 ] ) - 1 );
  const float ymax = ( voldim[ i3 ] - 1 ) / static_cast< float4 >( GetNextPowerOfTwo( voldim[ i3 ] ) - 1 );

  if( uAxisIndex == 0 ) // sagittal
  {
    texels[ 0 ] = Point3Df( xmax,  0.0F,  0.0F ); 
    texels[ 1 ] = Point3Df( xmax,  ymax,  0.0F ); 
    texels[ 2 ] = Point3Df( 0.0F,  ymax,  0.0F ); 
    texels[ 3 ] = Point3Df( 0.0F,  0.0F,  0.0F );       

  }
  else if( uAxisIndex == 1 ) // coronal
  {
    texels[ 0 ] = Point3Df( 0.0F,  ymax,  0.0F ); 
    texels[ 1 ] = Point3Df( xmax,  ymax,  0.0F ); 
    texels[ 2 ] = Point3Df( xmax,  0.0F,  0.0F ); 
    texels[ 3 ] = Point3Df( 0.0F,  0.0F,  0.0F ); 
  }
  else if( uAxisIndex == 2 ) // axial
  {
    texels[ 0 ] = Point3Df( xmax,  ymax,  0.0F ); 
    texels[ 1 ] = Point3Df( xmax,  0.0F,  0.0F ); 
    texels[ 2 ] = Point3Df( 0.0F,  0.0F,  0.0F ); 
    texels[ 3 ] = Point3Df( 0.0F,  ymax,  0.0F ); 
  }

  const uint2 uAxisX = pOtherCamera->GetViewRight().GetMaxAbsComponentCoord();
  const uint2 uAxisY = pOtherCamera->GetViewUp().GetMaxAbsComponentCoord();
  
  const float4 fWidth = otherEnvironment.GetZoomFactor()->GetSize();    
  const float4 fWidthMax = max( worlddim[ uAxisX ], worlddim[ uAxisY ] );
  if( fWidth != fWidthMax )
  {
    const float4 fZoom = fWidthMax / fWidth;
    const bool bZooming = fZoom > 1.0F? false : true;
    
    const float4 fQuadWidth  = fabs( ( vertices[ 2 ] - vertices[ 1 ] ).GetLength() );
    const float4 fQuadHeight = fabs( ( vertices[ 0 ] - vertices[ 1 ] ).GetLength() );   

    const float4 fAdjustmentX = ( fQuadWidth  * ( 1.0F - fZoom  ))  / 2.0F;
    const float4 fAdjustmentY = ( fQuadHeight * ( 1.0F - fZoom  ))  / 2.0F;
    
    Normal< float4 > normal1( vertices[ 2 ] - vertices[ 1 ] );
    Normal< float4 > normal2( vertices[ 3 ] - vertices[ 0 ] );
    Normal< float4 > normal3( vertices[ 0 ] - vertices[ 1 ] );
    Normal< float4 > normal4( vertices[ 3 ] - vertices[ 2 ] );

    vertices[ 0 ] += ( ( bZooming? -normal2 :  normal2 ) * fAdjustmentX );
    vertices[ 1 ] += ( ( bZooming? -normal1 :  normal1 ) * fAdjustmentX );
    vertices[ 2 ] += ( ( bZooming?  normal1 : -normal1 ) * fAdjustmentX );
    vertices[ 3 ] += ( ( bZooming?  normal2 : -normal2 ) * fAdjustmentX );

    vertices[ 0 ] += ( ( bZooming?  normal3 : -normal3 ) * fAdjustmentY );
    vertices[ 1 ] += ( ( bZooming? -normal3 :  normal3 ) * fAdjustmentY );
    vertices[ 2 ] += ( ( bZooming? -normal4 :  normal4 ) * fAdjustmentY );
    vertices[ 3 ] += ( ( bZooming?  normal4 : -normal4 ) * fAdjustmentY );
  }

  const Normal3Df & normal = pOtherPlane->GetNormal();
  const Triplef & units = otherEnvironment.GetIntensityVolume()->GetHeader().GetVoxelUnits();
  const float4 fOffset = max( max( units.m_x * normal.X(), units.m_y * normal.Y() ), units.m_z * normal.Z() ) * 1.2;
  const Vector3Df offset =  -(normal * fOffset);

  for( int4 i = 0; i < 4; ++i )
  {
    Point3Df * p = &vertices[ i ];

    (*p) += offset;
    p->m_x = Bound( 0.0F, p->m_x, worlddim.m_x );
    p->m_y = Bound( 0.0F, p->m_y, worlddim.m_y );
    p->m_z = Bound( 0.0F, p->m_z, worlddim.m_z );
  }
} // GetVerticesAndTexelsOrtho( const vxEnvironment & otherEnvironment, Point3Df vertices[], Point3Df texels[] )


/**
 * Gets the vertices and texture coordinates required to draw the active oblique slice as a texture
 *
 * @param   otherEnvironment    other environment containing the plane
 * @param   vertices            vertices in world coordinates
 * @param   texels              texture coordinates
 */
void vxRendererSliceShadows3d::GetVerticesAndTexelsOblique( const vxEnvironment & otherEnvironment, Point3Df vertices[], Point3Df texels[] )
{
  // create a texture for the renderer
  vxRenderBuffer & buffer = const_cast< vxEnvironment & >( otherEnvironment ).GetRenderBuffer();
  
  const uint2 uRenderWidth = buffer.GetRenderSize().m_x;
  const uint2 uRenderHeight = buffer.GetRenderSize().m_y;

  vxRayGenerator generator( const_cast< vxEnvironment &>( otherEnvironment ), vxRayGenerator::OBLIQUE );
  generator.Initialize(  const_cast< vxEnvironment &>( otherEnvironment ) );

  const uint2 x1 = 0;
  const uint2 x2 = uRenderWidth - 1;
  const uint2 y1 = 0;
  const uint2 y2 = uRenderHeight - 1;

  vertices [ 2 ] = Point3Df ( generator.GenerateRay( x1, y2 ).GetStartPoint() );
  vertices [ 3 ] = Point3Df ( generator.GenerateRay( x1, y1 ).GetStartPoint() );
  vertices [ 0 ] = Point3Df ( generator.GenerateRay( x2, y1 ).GetStartPoint() );
  vertices [ 1 ] = Point3Df ( generator.GenerateRay( x2, y2 ).GetStartPoint() );

  const float4 xmax = buffer.GetRenderSize().m_x / static_cast< float4 >( buffer.GetBufferSize().m_x );
  const float4 ymax = buffer.GetRenderSize().m_y / static_cast< float4 >( buffer.GetBufferSize().m_y );

  texels[ 0 ] = Point3Df( xmax,  0.0F,  0.0F );       
  texels[ 1 ] = Point3Df( xmax,  ymax,  0.0F ); 
  texels[ 2 ] = Point3Df( 0.0F,  ymax,  0.0F ); 
  texels[ 3 ] = Point3Df( 0.0F,  0.0F,  0.0F ); 
 
} // GetVerticesAndTexelsOblique( const vxEnvironment & otherEnvironment, Point3Df vertices[], Point3Df texels[] )


/**
 * Renders the vessel detail endoluminal camera position and direction 
 *
 * @param   otherEnvironment    other environment
 */
void vxRendererSliceShadows3d::RenderVesselDetailEndoluminal( const vxEnvironment & otherEnvironment )
{
  // save the OpenGL state
  glPushAttrib( GL_ALL_ATTRIB_BITS );

  glDisable( GL_LIGHTING );
  glDisable( GL_DEPTH_TEST );
  glDisable( GL_TEXTURE_2D );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  // possibly scale for detail volume
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  TransformDetail();

  // obtain the endoluminal camera object
  vxShareableObject< vxCamera > * pCamera = otherEnvironment.GetCamera();

  // show it
  const float4 fArrowLength( 10.0F );
  const PixelRGBA<float4> cameraColor( 0.0F, 0.0F, 1.0F, 1.0F );
  vxRendererUtils::RenderArrow( pCamera->GetEye(), pCamera->GetViewDirection(), fArrowLength, false, cameraColor );

  // restore the OpenGL state
  glPopMatrix();
  glPopAttrib();

} // RenderVesselDetailEndoluminal( const vxEnvironment & otherEnvironment );


/**
 * Renders the projection of a curved plane
 *
 * @param   otherEnvironment   other environment
 */
void vxRendererSliceShadows3d::RenderCurvedPlane( const vxEnvironment & otherEnvironment )
{
  vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( otherEnvironment.GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL )
    return;

  // early return if there is no data
  if( pRibbon->GetScanlineData().size() == 0 )
    return;

  // make a new copy with only the non-empty endpoints
  std::vector< vxRibbon::ScanlineData > ends;
  std::vector< vxRibbon::ScanlineData >::const_iterator it;
  for( it=pRibbon->GetScanlineData().begin(); it!= pRibbon->GetScanlineData().end(); ++it )
  {
    if ( it->m_fSectionIndex >= 0 )
      ends.push_back( * it );
  }

  // early return if no data to render
  if( ends.size() == 0 )
    return;

  // save the OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glDepthFunc( GL_LEQUAL ); 
  glEnable( GL_DEPTH_TEST ); 

  glEnable( GL_BLEND ); 
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  // set up antialiasing
  glEnable( GL_LINE_SMOOTH );
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

  glShadeModel( GL_SMOOTH ); 

  // possibly scale for detail volume
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  TransformDetail();


  const PixelRGBA< uint1 > color = GetSliceShadowColor( otherEnvironment );

  const float4 fOpactityAjdustment = 0.60F;
  const float4 fDistanceNearPlane = GetEnvironment().GetViewerType()->GetNearPlane();
  const float4 fDistanceFarPlane  = GetEnvironment().GetViewerType()->GetFarPlane();
  const vxCamera * pCamera = GetEnvironment().GetCamera();

  glBegin( GL_QUADS );
  std::vector< vxRibbon::ScanlineData >::const_iterator iter = ends.begin();
  const int4 iNumberOfQuads = ends.size() - 1;
  for( int4 i = 0; i < iNumberOfQuads; ++i )
  {
    const Point3Df leftPoint1   = (*iter).m_leftPoint;
    const Point3Df rightPoint1  = (*iter++).m_rightPoint;
    const Point3Df leftPoint2   = (*iter).m_leftPoint;
    const Point3Df rightPoint2  = (*iter).m_rightPoint;

    const float4 fDistanceLeft1   = pCamera->GetEye().GetEuclideanDistance( leftPoint1  );
    const float4 fDistanceRight1  = pCamera->GetEye().GetEuclideanDistance( rightPoint1 );
    const float4 fDistanceLeft2   = pCamera->GetEye().GetEuclideanDistance( leftPoint2  );
    const float4 fDistanceRight2  = pCamera->GetEye().GetEuclideanDistance( rightPoint2 );

    const float4 fOpacityWeightLeft1  = Bound( 0.1F, Interpolate( fDistanceNearPlane, fDistanceLeft1,     fDistanceFarPlane,  1.0F, 0.1F ), 1.0F );
    const float4 fOpacityWeightRight1 = Bound( 0.1F, Interpolate( fDistanceNearPlane, fDistanceRight1,    fDistanceFarPlane,  1.0F, 0.1F ), 1.0F );
    const float4 fOpacityWeightLeft2  = Bound( 0.1F, Interpolate( fDistanceNearPlane, fDistanceLeft2,     fDistanceFarPlane,  1.0F, 0.1F ), 1.0F );
    const float4 fOpacityWeightRight2 = Bound( 0.1F, Interpolate( fDistanceNearPlane, fDistanceRight2,    fDistanceFarPlane,  1.0F, 0.1F ), 1.0F );

    const Normal3Df normal1( rightPoint1 - leftPoint1 );
    const Normal3Df normal2( leftPoint2 - leftPoint1 );
    const Normal3Df planeNormal( normal1.Cross( normal2 ) );

    const float4 fMinModulation = 0.4F;
    const float4 fMaxModulation = 1.0F;
    const float4 fColorModulationFactor = Bound( fMinModulation, Interpolate( 0, abs( pCamera->GetViewDirection().Dot( planeNormal ) ),  1.0F,  fMinModulation, fMaxModulation ), fMaxModulation );
      
    const uint1 uRed    = color.R() * fColorModulationFactor;
    const uint1 uGreen  = color.G() * fColorModulationFactor;
    const uint1 uBlue   = color.B() * fColorModulationFactor;
    const uint1 uAlpha  = color.A() * fOpactityAjdustment;


    glColor4ub( uRed, uGreen, uBlue, uAlpha* fOpacityWeightLeft1 );    
    vxRendererUtils::GlVertex( leftPoint1 );
  
    glColor4ub( uRed, uGreen, uBlue, uAlpha * fOpacityWeightRight1 );    
    vxRendererUtils::GlVertex( rightPoint1 );

    glColor4ub( uRed, uGreen, uBlue, uAlpha * fOpacityWeightRight2 );    
    vxRendererUtils::GlVertex( rightPoint2 );

    glColor4ub( uRed, uGreen, uBlue, uAlpha* fOpacityWeightLeft2 );    
    vxRendererUtils::GlVertex( leftPoint2 );
  
  }
  glEnd();

 
  // restore the OpenGL state
  glPopMatrix();
  glPopAttrib();

} // RenderCurvedPlane( const vxEnvironment & environment )


/**
 * Renders the current vessel section index as an arrow
 *
 * @param   otherEnvironment      environment
 */
void vxRendererSliceShadows3d::RenderCurvedCamera( const vxEnvironment & otherEnvironment )
{
  // grab the vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( pVessel == NULL  )
    return;

  // get the current selected index
  const float4 fVesselSectionIndex = dynamic_cast< vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ) )->GetValue();
  if ( fVesselSectionIndex >= 0.0F && fVesselSectionIndex <= pVessel->size() - 1 )
  {
    //  figure out if we are draging frward or backward.
    m_bDraggingdEndoCameraForward = ( m_fPreviousVesselSectionIndex == fVesselSectionIndex ) ? m_bDraggingdEndoCameraForward : m_fPreviousVesselSectionIndex < fVesselSectionIndex;
      
    // get the cemars view direction and reverse it
    Normal< float4 >  cameraDirection = GetEnvironment().GetCamera()->GetViewDirection();
    cameraDirection = -cameraDirection;
    
    // compute the vessel radius at the specified vessel  sectionj
    const float fVesselRadius = pVessel->GetDiameterRange( vxVessel::MeasurePair( fVesselSectionIndex, fVesselSectionIndex ) ).GetMaximum() / 2.0F;

    Point<float4> refercentPnt( pVessel->GetResampledPosition( fVesselSectionIndex ) + fVesselRadius * cameraDirection );

    const Point3Df position( pVessel->GetResampledPosition( fVesselSectionIndex ) -Vector3Df( refercentPnt ) );
    Normal3Df viewDirection( pVessel->GetResampledTangent( fVesselSectionIndex ) );

    if ( ! m_bDraggingdEndoCameraForward ) 
      viewDirection = - viewDirection;
    Normal3Df upDirection( pVessel->GetResampledUpDirection( fVesselSectionIndex ) );

    m_fPreviousVesselSectionIndex = fVesselSectionIndex;

    const float4 fArrowLength( 5.0F );
    const float4 fArrowLengthOnScreen( 7.0F );
    Point<float4> arrowBgn( refercentPnt );
    Point<float4> arrowEnd( refercentPnt - fArrowLength*viewDirection );

    const vxViewerType * pViewerType = GetEnvironment().GetViewerType();
    if( pViewerType->GetRenderCoordinateSystem() == vxViewerType::eScreenSpace )
    {
      Point2D<int2> screenPos;

      pViewerType->ConvertWorldToScreen(arrowBgn, screenPos);
      arrowBgn.m_x = screenPos.m_x;
      arrowBgn.m_y = screenPos.m_y;
      arrowBgn.m_z = 0;

      pViewerType->ConvertWorldToScreen(arrowEnd, screenPos);
      arrowEnd.m_x = screenPos.m_x;
      arrowEnd.m_y = screenPos.m_y;
      arrowEnd.m_z = 0;
    }
    Vector3Df arrowDir( arrowBgn - arrowEnd );
    if( arrowDir.GetLength() < 1.0F )
      arrowDir = Vector3Df( 0.0F, 0.0F, 1.0F );
    arrowDir.Normalize();
    Point3Df arrowHead( arrowBgn - 0.5 * fArrowLengthOnScreen * arrowDir );

    vxRendererUtils::RenderArrow( arrowHead, Normal3Df( arrowDir ), fArrowLengthOnScreen, true, false, PixelRGBA<float4>( 0.0F, 0.0F, 1.0F, 1.0F ) );
  }
} // RenderCurvedCamera( const vxEnvironment & otherEnvironment )


/**
 * Transform OpenGL for the detail volume
 */
void vxRendererSliceShadows3d::TransformDetail() const
{
  Point<float4> centerPoint;
  if( GetEnvironment().GetIntensityVolume()->GetHeader().GetVariableVolumeHeader().GetHdrVar( "VolumeCenter", centerPoint ) )
  {
    // get the location of (0,0,0) in the original volume
    Vector<float4> worldDim( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim() );
    Point<float4> volMinPoint( centerPoint - worldDim / 2.0F );

    glTranslatef( -volMinPoint.m_x, -volMinPoint.m_y, -volMinPoint.m_z );
  }
} // TransformDetail()


// $Log: vxRendererSliceShadows3d.C,v $
// Revision 1.19  2008/01/21 18:12:27  geconomos
// Changed way that oblique slice shadow is drawn
//
// Revision 1.18  2007/11/02 16:44:04  geconomos
// Issue# 5827: Added synchronization around slice image access
//
// Revision 1.17  2007/08/14 18:53:27  romy
// If the viewertype is 3D or Graph, keep the slice shadows from the previous environment.
//
// Revision 1.16  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.14  2007/01/24 16:54:01  geconomos
// adjusted code for drawing slice shadows at volume boundaries
//
// Revision 1.13  2007/01/22 15:34:44  geconomos
// Issue# 5321: Slice shadows mode reworked
//
// Revision 1.12  2007/01/18 15:45:53  geconomos
// added RenderPlaneWithSliceImage()
//
// Revision 1.11  2006/12/12 16:50:23  romy
// Disables depth buffer during the arrow rendering to bring it to the front of Slice shadows.
//
// Revision 1.10  2006/11/13 20:58:59  geconomos
// Issue 5082: vessel postion not displaying for curved views
//
// Revision 1.9  2006/10/24 12:32:21  geconomos
// reworked curved slice shadow rendering
//
// Revision 1.8  2006/10/23 01:39:18  geconomos
// initial attempt at curved slice shadow
//
// Revision 1.7  2006/10/20 20:49:29  geconomos
// using SliceShadowMode from the rendering mode
//
// Revision 1.6  2006/10/05 19:45:40  geconomos
// enabled shading on plane shadow renderings
//
// Revision 1.5  2006/10/05 16:59:36  geconomos
// redesgined and implemented
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSliceShadows3d.C,v 1.19 2008/01/21 18:12:27 geconomos Exp $
// $Id: vxRendererSliceShadows3d.C,v 1.19 2008/01/21 18:12:27 geconomos Exp $
