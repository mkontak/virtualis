// $Id: vxRenderingContextDIB.C,v 1.3 2005/05/23 12:48:20 frank Exp $
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
#include "vxRenderingContextDIB.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Default consturctor.
 */
vxRenderingContextDIB::vxRenderingContextDIB() : 
  m_hrc( NULL )
{  
} // vxRenderingContextDIB()


/**
 * Virtual Destructor.
 */
vxRenderingContextDIB::~vxRenderingContextDIB()
{
  Destroy();
} // ~vxRenderingContextDIB()


/**
 * Creates a rendering context with the spcified width and height.
 *
 * @param   uWidth    Width in pixels.
 * @param   uHeight   Height in pixels.
 */
bool vxRenderingContextDIB::Create( const uint4 uWidth, const uint4 uHeight )
{

  try
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

  }
  catch ( ... )
  {
    LogErr( "unhandled exception in OnCreate", "vxRenderingContextDIB", "OnCreate" );
    return false;
  }
  return true;
  
} // Create( uWidth, uHeight )


/**
 * Destroys the context rendering object.
 */
void vxRenderingContextDIB::Destroy() 
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
void vxRenderingContextDIB::MakeCurrent()
{

  if( m_dib.GetHDC() && wglGetCurrentContext() != m_hrc )
    wglMakeCurrent( m_dib.GetHDC(), m_hrc );

} // MakeCurrent() 


// undefines
#undef FILE_REVISION


// $Log: vxRenderingContextDIB.C,v $
// Revision 1.3  2005/05/23 12:48:20  frank
// fixed file revision definition so that we get correct information in log files
//
// Revision 1.2  2004/04/12 18:08:02  geconomos
// Removed const modifier from MakeCurrent() definition.
//
// Revision 1.1  2004/04/02 14:06:29  frank
// working on unifying the various rendering contexts
//
// Revision 1.1  2004/03/30 17:24:02  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderingContextDIB.C,v 1.3 2005/05/23 12:48:20 frank Exp $
// $Id: vxRenderingContextDIB.C,v 1.3 2005/05/23 12:48:20 frank Exp $
