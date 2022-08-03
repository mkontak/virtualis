// $Id: vxRenderingContext.C,v 1.1 2004/03/30 17:24:02 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (george@viatronix.com)

// includes
#include "stdafx.h"
#include "vxRenderingContext.h"

/**
 * Default consturctor.
 */
vxRenderingContext::vxRenderingContext() : 
  m_hrc( NULL )
{  
} // vxRenderingContext()


/**
 * Virtual Destructor.
 */
vxRenderingContext::~vxRenderingContext()
{
  Destroy();
} // ~vxRenderingContext()


/**
 * Creates a rendering context with the spcified width and height.
 *
 * @param   uWidth    Width in pixels.
 * @param   uHeight   Height in pixels.
 */
void vxRenderingContext::Create( uint4 uWidth, uint4 uHeight )
{
  // create the dib to render to
  m_dib.Create( uWidth, uHeight );
	
  // fill in the pixel format descriptor
  PIXELFORMATDESCRIPTOR pfd ;
  memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ));
  pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );  
  pfd.nVersion = 1 ;
  pfd.dwFlags =  PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI | PFD_DRAW_TO_BITMAP;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  pfd.cDepthBits = 32;
  pfd.iLayerType = PFD_MAIN_PLANE ;

	// set the pixel format
  int4 iPixelFormat = ChoosePixelFormat( m_dib.GetHDC(), &pfd );
	SetPixelFormat( m_dib.GetHDC(), iPixelFormat, &pfd );
	
  // create a rendering context
	m_hrc = wglCreateContext( m_dib.GetHDC() );
} // Create( uWidth, uHeight )


/**
 * Destroys the context rendering object.
 */
void vxRenderingContext::Destroy() 
{
  if( m_hrc ) 
  {
	  // delete rendering context
	  if( m_hrc == wglGetCurrentContext() )
		  wglMakeCurrent( NULL, NULL ) ; 

	  wglDeleteContext( m_hrc ) ;
	  m_hrc = NULL;
  } // if( m_hrc ) 		
} // Destroy()


/**
 * Sets the context rendering object as active.
 */
void vxRenderingContext::Activate() 
{
  if( m_dib.GetHDC() && wglGetCurrentContext() != m_hrc )
    wglMakeCurrent( m_dib.GetHDC(), m_hrc );
} // Activate() 

// $Log: vxRenderingContext.C,v $
// Revision 1.1  2004/03/30 17:24:02  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderingContext.C,v 1.1 2004/03/30 17:24:02 geconomos Exp $
// $Id: vxRenderingContext.C,v 1.1 2004/03/30 17:24:02 geconomos Exp $
