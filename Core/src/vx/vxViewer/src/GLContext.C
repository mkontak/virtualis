// $Id: GLContext.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george (geconmos@viatronix.com)


// includes
#include "stdafx.h"
#include "GLContext.h"


// static memeber initialization
GLContext* GLContext::m_pCurrentContext = 0;


/**
 * Constructor
 * @param GL format
 * @param GL paint device
 */
GLContext::GLContext( const GLFormat & format, const GLPaintDevice * pDevice ) :
m_format( format ),
m_pDevice( pDevice ),
m_bValid( false ),
m_bInitDone( false ),
m_bSharing( false )
{
  m_hRC = 0;
  m_hdc = 0;
  m_iPixelFormatID = 0;
} // GLContext()


/**
 * Destructor
 */
GLContext::~GLContext()
{
  Reset();
} // ~GLContext()


/**
 * Set format
 * @param GL format
 */
void GLContext::SetFormat( const GLFormat & format )
{
  Reset();
  m_format = format;
} // SetFormat()


/**
 * Create context
 * @param GL context
 */
bool GLContext::Create( const GLContext * pShareContext )
{
  Reset();
  m_bValid = ChooseContext( pShareContext );

  return m_bValid;
} // Create()


/**
 * Pixel format descriptor to GL format
 * @param pixel format descriptor
 * @return Gl format
 */
static GLFormat pfdToGLFormat( const PIXELFORMATDESCRIPTOR* pfd )
{
  GLFormat format;
  format.SetDoubleBuffer( pfd->dwFlags & PFD_DOUBLEBUFFER? true : false );
  format.SetDepthBuffer( pfd->cDepthBits? true : false );
  format.SetAlpha( pfd->cAlphaBits? true : false );
  format.SetAccumulationBuffer(pfd->cAccumBits? true : false );
  format.SetStencilBuffer( pfd->cStencilBits? true : false );
  format.SetStereoBuffer( pfd->dwFlags & PFD_STEREO? true : false );
  format.SetDirectRendering( ( pfd->dwFlags & PFD_GENERIC_ACCELERATED ) || !( pfd->dwFlags & PFD_GENERIC_FORMAT ) );

  return format;
} // pfdToGLFormat()


/**
 * Choose a context
 * @param shared GL context
 * @return success?
 */
bool GLContext::ChooseContext( const GLContext* pShareContext )
{
	m_hWnd = m_pDevice->GetHWND();
	
  HDC hdc = GetDC( m_hWnd );
  if( !hdc )
  { 
	  if( m_hWnd )
	    ReleaseDC( m_hWnd, hdc );
	  
    return false;
  }

	PIXELFORMATDESCRIPTOR pfd;
  m_iPixelFormatID = ChoosePixelFormat( &pfd, hdc );
  if( m_iPixelFormatID == 0 )
  {
	  if( m_hWnd )
	    ReleaseDC( m_hWnd, hdc );
  	
    return false;
  }
  
  PIXELFORMATDESCRIPTOR realPfd;
  DescribePixelFormat( hdc, m_iPixelFormatID, sizeof( PIXELFORMATDESCRIPTOR ), &realPfd );
  m_format = pfdToGLFormat( &realPfd );
  
  if( !SetPixelFormat( hdc, m_iPixelFormatID, &pfd ) ) 
  {
	  if( m_hWnd )
	    ReleaseDC( m_hWnd, hdc );
	  
    return false;
  }

  if( !( m_hRC = wglCreateContext( hdc ) ) ) 
  { 
	  if( m_hWnd )
	    ReleaseDC( m_hWnd, hdc );
	  
    return false;
  }

  if( pShareContext && pShareContext->IsValid() )
	  m_bSharing = wglShareLists(pShareContext->m_hRC, m_hRC ) != 0;

  if (m_hWnd)
    ReleaseDC(m_hWnd, hdc);

  return true;
} // ChooseContext()


/**
 * Choose a context
 * @param pixel format descriptor
 * @param device context handle
 * @return pixel format
 */
int GLContext::ChoosePixelFormat( void * pfd, HDC hdc )
{
  PIXELFORMATDESCRIPTOR * p( ( PIXELFORMATDESCRIPTOR* )pfd );
  memset(p, 0, sizeof(PIXELFORMATDESCRIPTOR));
  
  p->nSize = sizeof( PIXELFORMATDESCRIPTOR );
  p->nVersion = 1;
  p->dwFlags  = PFD_SUPPORT_OPENGL;
	p->dwFlags |= PFD_DRAW_TO_WINDOW;
  p->iPixelType = PFD_TYPE_RGBA;
  p->cColorBits = 24;
  
  if( m_format.GetDoubleBuffer() )
	  p->dwFlags |= PFD_DOUBLEBUFFER;
  
  if( m_format.GetStereoBuffer() )
	  p->dwFlags |= PFD_STEREO;
  
  if( m_format.GetDepthBuffer() )
	  p->cDepthBits = 32;
  
  if( m_format.GetAlpha() )
	  p->cAlphaBits = 8;
 
  if( m_format.GetAccumulationBuffer() )
	  p->cAccumBits = p->cColorBits + p->cAlphaBits;
  
  if( m_format.GetStencilBuffer() )	
    p->cStencilBits = 4;
  
  p->iLayerType = PFD_MAIN_PLANE;
  
  return ::ChoosePixelFormat( hdc, p ); 
}


/** 
 * Reset
 */
void GLContext::Reset()
{
  if( IsValid() == false )
	  return;
  
  DoneCurrent();
 
  if( m_hRC )
	  wglDeleteContext( m_hRC ); 
 
  if( m_hWnd &&m_hdc )
    ReleaseDC( m_hWnd, m_hdc );

  m_hRC  = 0;
  m_hdc  = 0;
  m_hWnd = 0;
  m_iPixelFormatID = 0;
  m_bSharing = false;
  m_bValid = false;
  m_bInitDone = false;
} // Reset()


/** 
 * Make current
 */
void GLContext::MakeCurrent()
{
  if( wglGetCurrentContext() == m_hRC) // already current
		return;

  if( IsValid() == false )
	  return;

  if( m_hWnd )
  {
    if( m_hdc )
    {
       ReleaseDC( m_hWnd, m_hdc );
       m_hdc = 0;
    }

    m_hdc = GetDC( m_hWnd );
  }
  else
  {
    if( m_hdc )
    {
       ReleaseDC( m_hWnd, m_hdc );
       m_hdc = 0;
    }

    m_hdc = m_pDevice->GetHDC();
  }

  wglMakeCurrent( m_hdc, m_hRC );
  
  m_pCurrentContext = this;
} // MakeCurrent()


/** 
 * Done current
 */
void GLContext::DoneCurrent()
{
  if( m_pCurrentContext != this )
	  return;
  m_pCurrentContext = 0;
  
  wglMakeCurrent( 0, 0 );
  
  if( m_hWnd && m_hdc ) 
  {
    ReleaseDC( m_hWnd, m_hdc );
    m_hdc = 0;
  }
} // DoneCurrent()


/** 
 * Swap buffers
 */
void GLContext::SwapBuffers() const
{
  if( m_hdc && m_format.GetDoubleBuffer() ) 
    ::SwapBuffers( m_hdc );
} // SwapBuffers()



// $Log: GLContext.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.6  2003/05/16 11:45:24  michael
// code review
//
// Revision 3.5  2003/05/14 13:26:59  geconomos
// Coding standards.
//
// Revision 3.4  2003/05/08 16:39:05  michael
// more code reviews (75 renderer Utils)
//
// Revision 3.3  2002/11/10 23:07:27  michael
// cosmetics
//
// Revision 3.2  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 3.1  2002/01/25 17:19:21  dmitry
// Released DC before creating a new one.
//
// Revision 3.0  2001/10/14 23:02:31  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:52   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:28   ingmar
// Initial revision.
// Revision 1.6  2001/09/07 13:38:19  geconomos
// Rename GLViewer=>Viewer renamed all OnGLXXX function to OnXXX
//
// Revision 1.5  2001/07/31 18:48:27  liwei
// MakeCurrent() changed to accomodate views not derived from Viewer.
//
// Revision 1.4  2001/07/18 17:42:43  liwei
// The function MakeCurrent() changed for testing.
//
// Revision 1.3  2001/07/06 22:17:54  liwei
// Now using wglGetCurrentContext to determine whether the current renderer own the OpenGL context.
//
// Revision 1.2  2001/07/06 21:26:28  liwei
// GLContext::MakeCurrent() changed temporarily to the activeX control compatible
// with other OpenGL windows.
//
// Revision 1.1  2001/06/29 16:53:08  geconomos
// Moved from to 53_rendererDataStruct
//
// Revision 1.3  2001/06/27 18:51:09  geconomos
// no message
//
// Revision 1.2  2001/06/22 18:22:07  geconomos
// Initial OpenGL implementation.
//
// Revision 1.1  2001/06/18 21:52:14  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/GLContext.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: GLContext.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
