// $Id: vxRendererGraph.C,v 1.13 2006/12/07 19:58:47 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Romy Sreedharan (mailto:romy@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererGraph.h"
#include "vxEnvironment.h"

#include "vxGraph.h"
#include "vxViewerTypeGraph.h"

#define FILE_REVISION "$Revision: 1.13 $"


// namespaces
using namespace std;


/** 
* Constructor
*
* @param environment the environment
*/
vxRendererGraph::vxRendererGraph( vxEnvironment & environment ) : vxRenderer( environment ) 
{
	m_previousSize.first  = 0;
	m_previousSize.second = 0;
} // vxRendererGraph( vxEnvironment & environment )


/**
* Destructor
*/
vxRendererGraph::~vxRendererGraph()
{
} // ~vxRendererGraph()


/**
* Initialize the render environment
*/
void vxRendererGraph::Initialize()
{
	vxRenderer::Initialize();

	glGenTextures( 1, &m_textureId );
} // Initialize() 


/**
* Uninitialize routine
*/
void vxRendererGraph::Uninitialize()
{
	if( m_textureId != 0 )
		glDeleteTextures( 1, &m_textureId );

} // Uninitialize()


/** 
* Has something changed that makes rendering necessary?
* @return whether rendering is necessary
*/
bool vxRendererGraph::IsRedrawNecessary()
{
	//if ( m_previousSize == GetEnvironment().GetWinRect().GetViewportSize() )
	//	return false;

	return true;
} // IsRedrawNecessary()


/**
* PreRender routine
* @return success
*/
int4 vxRendererGraph::PreRender()
{
	return 1;
} // PreRender()



/**
* Render
*/
void vxRendererGraph::Render()
{
	try
	{
		std::pair<uint2,uint2> size = GetEnvironment().GetWinRect().GetViewportSize();

		// Update the texture only if the viewport size changes
		if( m_previousSize != size )
			UpdateTexture();

		DrawTexture();

		m_previousSize = size;
	}
	catch(...)
	{
		LogErr ( "crash occurred during  vxRendererGraph Render ", "vxRendererGraph", "Render" );
	}
} // Render()

/**
* Updates the texture from the render buffer's color image
*/
void vxRendererGraph::UpdateTexture()
{
	try
	{
		vxGraph * pGraph =  dynamic_cast<vxViewerTypeGraph*>( GetEnvironment().GetViewerType())->GetGraph(); 

		const std::pair<int2, int2> & size =  GetEnvironment().GetWinRect().GetViewportSize();

		int iWidth  = GetNextPowerOfTwo( size.first );
		int iHeight = GetNextPowerOfTwo( size.second );

    int imageSize = min ( iWidth, iHeight );

		// draw image
		pGraph->Initialize( imageSize, imageSize );
		pGraph->Draw();

    //Get the new Graph image
		Gdiplus::Bitmap * pBitmap = pGraph->GetGraphImage();
		if ( pBitmap == NULL )
			return;

		pBitmap->RotateFlip( Gdiplus::RotateNoneFlipY );

		// lock the down managed bitmap and grab a pointer to it's first scan line
		Gdiplus::Rect bounds ( 0, 0, imageSize, imageSize );   
		Gdiplus::BitmapData data;		
		pBitmap->LockBits( &bounds, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, &data );

		// create a texture for the renderer
		glPushAttrib( GL_ALL_ATTRIB_BITS );

		glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
		glPixelStorei( GL_UNPACK_ROW_LENGTH, data.Width );

		glBindTexture( GL_TEXTURE_2D, m_textureId );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, data.Width, data.Height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data.Scan0 );
		glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );

		glPopAttrib();

		pBitmap->UnlockBits( &data );

	}
	catch (...)
	{
		throw ex::VException( LogRec( "CreateTexture failed.", "vxRendererGraph", "CreateTexture" ));
	}
} // CreateTexture()


/**
* Draws the texture to the entire viewport
*/
void vxRendererGraph::DrawTexture()
{
	try
	{
		const std::pair<int2, int2> & size =  GetEnvironment().GetWinRect().GetViewportSize();
    vxGraph * pGraph =  dynamic_cast<vxViewerTypeGraph*>( GetEnvironment().GetViewerType())->GetGraph(); 

    //scale and keep aspect ratio of source image
    float4 scale = ( min ( size.first / static_cast<float4>( pGraph->GetWidth()) ,  size.second / static_cast<float4>( pGraph->GetHeight() )));
    float4 width   = pGraph->GetWidth()  * scale;
    float4 height  = pGraph->GetHeight() * scale;

    float4 x = ( size.first - width )   / 2.0F  + 0.5F; 
    float4 y = ( size.second - height ) / 2.0F + 0.5F; 

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
   				glTexCoord2f( 0.0,  0.0 );  glVertex2f( x,  y );
          glTexCoord2f( 0.0,  1.0 );  glVertex2f( x,  y + height );
          glTexCoord2f( 1.0,	1.0 );  glVertex2f( x + width, y + height);
          glTexCoord2f( 1.0,  0.0 );  glVertex2f( x + width, y );
				} 
				glEnd();
			}
			glPopMatrix();
		}
		glMatrixMode( GL_PROJECTION );
		glPopMatrix();

		glDisable( GL_TEXTURE_2D );
		glPopAttrib();

	}
	catch( ... )
	{
		throw ex::VException( LogRec( "DrawTexture failed", "vxRendererRaytracer", "DrawTexture" ));
	}
} // DrawTexture()




// $Log: vxRendererGraph.C,v $
// Revision 1.13  2006/12/07 19:58:47  romy
// code Fix to keep the aspect ratio added
//
// Revision 1.12  2006/11/21 20:14:35  romy
// fixed the issue of imageing not showing issue
//
// Revision 1.11  2006/11/17 21:13:28  geconomos
// restored row length after texture mapping graph image
//
// Revision 1.10  2006/11/16 21:17:58  geconomos
// Fixed the stride issue
//
// Revision 1.9  2006/11/16 18:43:18  romy
// using the Texture mapping instead of bitmap drawing
//
// Revision 1.8  2006/11/15 16:06:40  dongqing
// fixed Graph drawing issue on 9300 machines. Added glRasterPos2i
//
// Revision 1.7  2006/11/13 05:15:17  romy
// comments added
//
// Revision 1.6  2006/11/13 05:02:53  romy
// comments added
//
// Revision 1.5  2006/11/06 21:40:31  romy
// nasty pointers cleaned up
//
// Revision 1.4  2006/11/02 22:40:19  romy
// code optimization
//
// Revision 1.3  2006/11/02 20:27:45  romy
// initial version
//
// Revision 1.2  2006/11/02 20:24:59  romy
// initial version
//
// Revision 1.1  2006/11/02 03:53:37  romy
// added new viewer type
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererGraph.C,v 1.13 2006/12/07 19:58:47 romy Exp $
// $Id: vxRendererGraph.C,v 1.13 2006/12/07 19:58:47 romy Exp $


#undef FILE_REVISION
