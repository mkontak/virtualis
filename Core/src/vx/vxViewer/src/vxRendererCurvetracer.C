// $Id: vxRendererCurvetracer.C,v 1.8 2007/04/02 21:28:39 gdavidson Exp $
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
#include "vxRendererCurvetracer.h"
#include "vxEnvironment.h"
#include "vxVisualizationGraph.h"
#include "vxVesselUtilities.h"
#include "vxVisualizationPreferences.h"

// defines
#define FILE_REVISION "$Revision: 1.8 $"


/**
 * Constructor
 */
vxRendererCurvetracer::vxRendererCurvetracer( vxEnvironment & environment ) : vxRenderer( environment ),
  m_bTextureCreated( false )
{
} // vxRendererCurvetracer()


/**
 * Destructor
 */
vxRendererCurvetracer::~vxRendererCurvetracer()
{
} // ~vxRendererCurvetracer()


/**
 * Initialize the render environment
 */
void vxRendererCurvetracer::Initialize()
{
  // create gl texture
  glGenTextures( 1, &m_textureId );
    
} // Initialize() 


/**
 * Uninitialize routine
 */
void vxRendererCurvetracer::Uninitialize()
{
  // create gl texture
  if( m_textureId != 0 )
    glDeleteTextures( 1, &m_textureId );
  
} // Uninitialize()


/**
 * Texture maps and draws the color image from the vxRenderBuffer in the environment
 */
void vxRendererCurvetracer::Render()
{
  // make sure we have a vessel
  if( vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() ) == NULL )
    return;

  // do we need to recreate the texture?
  if( GetEnvironment().GetRenderBuffer().IsModified() || !m_bTextureCreated )
  {
    UpdateTexture();
    m_bTextureCreated = true;
  }

  DrawTexture();
  
  if( vxVisualizationPreferences::GetShowStatistics() )
    vxVisualizationGraph::DrawStatistics( GetEnvironment() );

} // Render()


/**
 * Updates the texture from the associated vxRenderBuffer
 */
void vxRendererCurvetracer::UpdateTexture()
{
  vxRenderBuffer & buffer = const_cast< vxRenderBuffer & >( GetEnvironment().GetRenderBuffer() );

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
void vxRendererCurvetracer::DrawTexture()
{
  vxRenderBuffer & buffer = const_cast< vxRenderBuffer & >( GetEnvironment().GetRenderBuffer() );
  
  glBindTexture( GL_TEXTURE_2D, m_textureId );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

  glPushAttrib( GL_ALL_ATTRIB_BITS );
  glMatrixMode( GL_PROJECTION );

  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0F, 1.0F, 0.0F, 1.0F );
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  
  glColor3f( 1, 1, 0 );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glEnable( GL_TEXTURE_2D );
  {
    glBegin( GL_QUADS ); 
    {

      const float4 fTextureX( static_cast< float4 >( GetEnvironment().GetWinRect().GetViewportWidth() ) / static_cast< float4 >( buffer.GetColorImage().GetDim().m_x ) );
      const float4 fTextureY( static_cast< float4 >( GetEnvironment().GetWinRect().GetViewportHeight() ) / static_cast< float4 >( buffer.GetColorImage().GetDim().m_y ) );

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
  glDisable( GL_TEXTURE_2D );

  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();
  glPopAttrib();

} // DrawTexture()


// $Log: vxRendererCurvetracer.C,v $
// Revision 1.8  2007/04/02 21:28:39  gdavidson
// Added IsModified to the vxRenderBuffer
//
// Revision 1.7  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.6  2007/01/19 15:28:50  geconomos
// reorganized visualization preferences
//
// Revision 1.5  2007/01/17 16:42:31  geconomos
// moved DrawStatistics to vxVisualizationGraph
//
// Revision 1.4  2006/09/28 18:29:50  geconomos
// Issue 4963: added indicator for non-optimal image quality
//
// Revision 1.3  2006/09/21 15:02:10  geconomos
// added check to see if vessel exists before rendering
//
// Revision 1.2  2006/09/20 15:04:48  frank
// added preference for displaying rendering manager statistics
//
// Revision 1.1  2006/09/14 20:09:17  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererCurvetracer.C,v 1.8 2007/04/02 21:28:39 gdavidson Exp $
// $Id: vxRendererCurvetracer.C,v 1.8 2007/04/02 21:28:39 gdavidson Exp $
