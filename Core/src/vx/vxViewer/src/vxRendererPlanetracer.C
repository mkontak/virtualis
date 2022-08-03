// $Id: vxRendererPlanetracer.C,v 1.14 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxRendererPlanetracer.h"
#include "vxEnvironment.h"
#include "vxVisualizationGraph.h"
#include "vxVisualizationPreferences.h"


// defines
#define FILE_REVISION "$Revision: 1.14 $"


/**
 * Constructor
 *
 * @param   environment     environment to associate with renderer
 */
vxRendererPlanetracer::vxRendererPlanetracer( vxEnvironment & environment ) :  vxRenderer( environment ),
m_bTextureCreated( false )
{
} // vxRendererPlanetracer()


/**
 * Destructor
 */
vxRendererPlanetracer::~vxRendererPlanetracer()
{
} // ~vxRendererPlanetracer()


/**
 * Initialization
 */
void vxRendererPlanetracer::Initialize()
{
  if( m_textureTile.GetDensityTextureId() == 0 )
  {
    GLuint id;
    glGenTextures( 1, &id );
    m_textureTile.SetDensityTextureId( id );
  }
} // Initialize()


/**
 * Cleanup
 */
void vxRendererPlanetracer::Uninitialize()
{
  if( m_textureTile.GetDensityTextureId() != 0 )
  {
    glDeleteTextures( 1, &(m_textureTile.GetDensityTextureId()) );
    m_textureTile.SetDensityTextureId( 0 );
  }
} // Uninitialize()


/**
 * Perform rendering 
 */
void vxRendererPlanetracer::Render()
{
  // TODO: why is this locking necessary while switch views?
  vxRenderBuffer & buffer = const_cast< vxRenderBuffer & >( GetEnvironment().GetRenderBuffer() );
  SingleLock<CriticalSection> lock( &buffer.GetSyncObject(), true );

  // does the texture needed to be recreated?
  if( GetEnvironment().GetRenderBuffer().IsModified() || !m_bTextureCreated )
  {
    UpdateTexture();
    m_bTextureCreated = true;

    GetEnvironment().GetRenderBuffer().SetModified( false );
  }

  DrawTexture();

  if( vxVisualizationPreferences::GetShowStatistics() )
    vxVisualizationGraph::DrawStatistics( GetEnvironment() );
} // Render()


/**
 * Updates the texture from the associated vxRenderBuffer
 */
void vxRendererPlanetracer::UpdateTexture()
{
  vxRenderBuffer & buffer = const_cast< vxRenderBuffer & >( GetEnvironment().GetRenderBuffer() );
 
  // textureSize is the texture image resolution. it must be power of 2
  Triple<uint4> textureSize( buffer.GetBufferSize()  );
  if( textureSize.X() != GetNextPowerOfTwo( textureSize.X() ))
  {
    throw ex::VException(LogRec("image to be used as texture is not power of 2 in DimX: " + textureSize, "vxRendererPlanetracer", "UpdateTexture"));
  }
  
  if( textureSize.Y() <  GetNextPowerOfTwo( textureSize.Y() ))
    textureSize.m_y = GetNextPowerOfTwo( textureSize.Y() ) / 2;

  // create a texture for the renderer
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glBindTexture( GL_TEXTURE_2D, m_textureTile.GetDensityTextureId() );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize.X(), textureSize.Y(), 
               0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.GetColorImage().GetDataPtr());

  // compute the texture coordinates for the texture tile
  const Vector3D<int4> size( static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVolDim());
  const Vector3D<float4> units( static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVoxelUnits());
  vxPlaneOrtho<float4> * pPlane( dynamic_cast<vxPlaneOrtho<float4> *>(GetEnvironment().GetPlaneOrtho()));
  uint4 uIndex(pPlane->GetNormal().GetMaxAbsComponentCoord());
  uint2 i1((uIndex + 0)%3);
  uint2 i2((uIndex + 1)%3);
  uint2 i3((uIndex + 2)%3);
  Vector<float4> lowerLeftPoint(0,0,0);
  Vector<float4> upperRightPoint( size[ 0 ] * units[ 0 ], size[ 1 ] * units[ 1 ], size[ 2 ] * units[ 2 ] );

  float4 fDistance(vxUtils::CalculateOffsetDistance(pPlane->GetDistance() / units[uIndex] + 0.5F,
                                                    size[uIndex], 
                                                    GetEnvironment().GetOffset(), 
                                                    GetEnvironment().GetMaximumOffset()) * units[uIndex]);
  
  lowerLeftPoint[ i1 ]  = fDistance;
  lowerLeftPoint[ i2 ]  = 0;
  lowerLeftPoint[ i3 ]  = 0;
  upperRightPoint[ i1 ] = fDistance;
  upperRightPoint[ i2 ] = units[i2] * max( 1,( size [i2 ]-1 ));
  upperRightPoint[ i3 ] = units[i3] * max( 1,( size[ i3 ]-1 ));

  m_textureTile.SetOrigin( lowerLeftPoint );
  m_textureTile.SetSize( upperRightPoint );

  Vector<float4> lowerLeftCoord( 0.0, 0.0, 0.0 );
  Vector<float4> upperRightCoord( 1.0, 1.0, 1.0 );
  upperRightCoord[i3] = (size[i3] > 1) ? (size[i3]-1)/(float4)(GetNextPowerOfTwo(size[i3])-1) : 1.0;
  upperRightCoord[i2] = (size[i2] > 1) ? (size[i2]-1)/(float4)(GetNextPowerOfTwo(size[i2])-1) : 1.0;

  m_textureTile.SetTexCoordLowerLeft(lowerLeftCoord);
  m_textureTile.SetTexCoordUpperRight(upperRightCoord);

  Vector<float4> direction(0,0,0);
  direction[i1] = 1.0F;
  m_textureTile.SetNormal(Normal3Df(direction));

} // UpdateTexture()


/**
 * Draws the image stored in the texture
 */
void vxRendererPlanetracer::DrawTexture()
{
  const uint1 uIndex = GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord();
  
  const GLuint id = m_textureTile.GetDensityTextureId();

  glBindTexture( GL_TEXTURE_2D, id );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

  glPushAttrib( GL_ALL_ATTRIB_BITS );

  glColor3f( 1, 1, 0 );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glEnable( GL_TEXTURE_2D );
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glDepthFunc( GL_LEQUAL);
  glBegin( GL_TRIANGLE_FAN );

  for (int i(0); i<4; i++)
  {
    glTexCoord2f(m_textureTile.GetVertexTexCoord(i)[ (uIndex+1)%3 ], 
                 m_textureTile.GetVertexTexCoord(i)[ (uIndex+2)%3 ]);
    glVertex3f(m_textureTile.GetVertex( i )[ 0 ], 
               m_textureTile.GetVertex( i )[ 1 ],
               m_textureTile.GetVertex( i )[ 2 ]);
  }
  glEnd();
  glDisable(GL_TEXTURE_2D);

  glPopAttrib();
} // DrawTexture()


// $Log: vxRendererPlanetracer.C,v $
// Revision 1.14  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.13  2007/04/02 21:28:39  gdavidson
// Added IsModified to the vxRenderBuffer
//
// Revision 1.12.2.1  2007/04/05 17:54:53  geconomos
// Issue# 5560: crash while switching "detail view" viewer type
//
// Revision 1.12  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.11  2007/01/19 15:28:50  geconomos
// reorganized visualization preferences
//
// Revision 1.10  2007/01/17 16:42:31  geconomos
// moved DrawStatistics to vxVisualizationGraph
//
// Revision 1.9  2007/01/16 17:02:13  gdavidson
// Implemented IsRedrawNecessary()
//
// Revision 1.8  2006/09/28 18:29:50  geconomos
// Issue 4963: added indicator for non-optimal image quality
//
// Revision 1.7  2006/09/28 15:52:17  geconomos
// Issue 5027: sub viewer causing main view to disappear
//
// Revision 1.6  2006/09/20 15:04:48  frank
// added preference for displaying rendering manager statistics
//
// Revision 1.5  2006/09/07 13:20:33  geconomos
// expanded buffer size for debug output
//
// Revision 1.4  2006/08/31 14:07:05  geconomos
// updated debugging output to include current dataset index
//
// Revision 1.3  2006/08/23 17:55:51  geconomos
// changed to display vxRenderBuffer from associated environment
//
// Revision 1.2  2006/08/03 14:14:47  geconomos
// removed all rendering functionailty; now just texture maps the render buffer contained in the associated environment
//
// Revision 1.1  2006/07/21 14:59:29  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererPlanetracer.C,v 1.14 2007/06/28 19:33:55 jmeade Exp $
// $Id: vxRendererPlanetracer.C,v 1.14 2007/06/28 19:33:55 jmeade Exp $
