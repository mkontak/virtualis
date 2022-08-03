// $Id: vxRenderingContextPBuffer.C,v 1.1 2007/10/24 20:33:49 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "stdafx.h"
#include "vxRenderingContextPBuffer.h"
#include "vxRenderingContextGL.h"
#include "GLPaintDevice.h"

// defines
#define FILE_REVISION "$Revision: 1.1 $"


// function pointers for p-buffer related opengl extensions
static PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = NULL;
static PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB = NULL;
static PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB = NULL;
static PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB = NULL;
static PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB = NULL;
static PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB = NULL;
static PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB = NULL;
static PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB = NULL;
static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;


/**
 * Constructor
 */
vxRenderingContextPBuffer::vxRenderingContextPBuffer() : 
  m_hPBuffer( NULL ),
  m_hDC( NULL ),
  m_hRC( NULL ),
  m_uWidth( 0 ),
  m_uHeight( 0 )
{
  static bool bExtensionsInitialized = false;
  if( !bExtensionsInitialized )
  {
    InitializeExtensions();
    bExtensionsInitialized = true;
  }
} // vxRenderingContextPBuffer()


/**
 * Destructor
 */
vxRenderingContextPBuffer::~vxRenderingContextPBuffer()
{
  Destroy();

  m_hiddenWindow.DestroyWindow();
} // ~vxRenderingContextPBuffer()


/**
 * Initializes the required opengl extensions for using a pbuffer
 *
 * @return true on success
 */
bool vxRenderingContextPBuffer::InitializeExtensions()
{
  /// associated the pbuffer with the hidden window context
  m_hiddenWindow.GetContext()->MakeCurrent();
  
  /// get proc address for "wglGetExtensionsStringARB"
  wglGetExtensionsStringARB = reinterpret_cast< PFNWGLGETEXTENSIONSSTRINGARBPROC >( wglGetProcAddress( "wglGetExtensionsStringARB" ) );
  if( wglGetExtensionsStringARB == NULL )
  {
    LogErr( "Unable to obtain proc address for wglGetExtensionsStringARB", "vxRenderingContextPBuffer", "InitializeExtensions" );
    return false;
  }
	
  /// get available extensions
  char * ext = ( char* )wglGetExtensionsStringARB( m_hiddenWindow.GetContext()->GetHDC() );

  /// check if p-buffers are supported
  if( strstr( ext, "WGL_ARB_pbuffer" ) == NULL )
	{
    LogErr( "WGL_ARB_pbuffer not found in extension string", "vxRenderingContextPBuffer", "InitializeExtensions" );
		return false;
	}

  // get associated procedure addresses for all "WGL_ARB_pbuffer" related functions  
  wglCreatePbufferARB    = reinterpret_cast< PFNWGLCREATEPBUFFERARBPROC >    ( wglGetProcAddress( "wglCreatePbufferARB" ));
	wglGetPbufferDCARB     = reinterpret_cast< PFNWGLGETPBUFFERDCARBPROC >     ( wglGetProcAddress( "wglGetPbufferDCARB" ));
	wglReleasePbufferDCARB = reinterpret_cast< PFNWGLRELEASEPBUFFERDCARBPROC > ( wglGetProcAddress( "wglReleasePbufferDCARB" ));
	wglDestroyPbufferARB   = reinterpret_cast< PFNWGLDESTROYPBUFFERARBPROC >   ( wglGetProcAddress( "wglDestroyPbufferARB" ));
	wglQueryPbufferARB     = reinterpret_cast< PFNWGLQUERYPBUFFERARBPROC >     ( wglGetProcAddress( "wglQueryPbufferARB" ));
	if( !wglCreatePbufferARB || !wglGetPbufferDCARB || !wglReleasePbufferDCARB || !wglDestroyPbufferARB || !wglQueryPbufferARB )
	{
    LogErr( "Unable to obtain the address of one or more WGL_ARB_pbuffer related procedures", "vxRenderingContextPBuffer", "InitializeExtensions" );
    return false;
	}

  // check if the extended pixel format is available
	if( strstr( ext, "WGL_ARB_pixel_format" ) == NULL )
	{
    LogErr( "WGL_ARB_pixel_format not found in extension string", "vxRenderingContextPBuffer", "InitializeExtensions" );
		return false;
	}

  // get associated procedure addresses for all "WGL_ARB_pixel_format" related functions  
	wglGetPixelFormatAttribivARB = reinterpret_cast< PFNWGLGETPIXELFORMATATTRIBIVARBPROC > ( wglGetProcAddress("wglGetPixelFormatAttribivARB" ));
	wglGetPixelFormatAttribfvARB = reinterpret_cast< PFNWGLGETPIXELFORMATATTRIBFVARBPROC > ( wglGetProcAddress("wglGetPixelFormatAttribfvARB" ));
	wglChoosePixelFormatARB      = reinterpret_cast< PFNWGLCHOOSEPIXELFORMATARBPROC >      ( wglGetProcAddress("wglChoosePixelFormatARB" ));
	if( !wglGetExtensionsStringARB || !wglCreatePbufferARB || !wglGetPbufferDCARB )
	{
    LogErr( "Unable to obtain the address of one or more WGL_ARB_pixel_format related procedures", "vxRenderingContextPBuffer", "InitializeExtensions" );
    return false;
	}
  return true;
} // InitializeExtensions()


/**
 * Creates a pbuffer rendering context with the specified with and height
 *
 * @param uWidth    p-buffer width
 * @param uHeight   p-buffer height
 * @return true on success
 */
bool vxRenderingContextPBuffer::Create( const uint4 uWidth, const uint4 uHeight )
{
  // set the hwnd based context active
  m_hiddenWindow.GetContext()->MakeCurrent();

  // free existing context if necessary
  Destroy();
  
  // reset statew
  m_hPBuffer = NULL;
	m_uWidth   = uWidth;
	m_uHeight  = uHeight;

	// define the minimum pixel format requirements we will need for our p-buffer
	int pf_attr[] =
	{
		WGL_SUPPORT_OPENGL_ARB, TRUE,       // P-buffer will be used with OpenGL
		WGL_DRAW_TO_PBUFFER_ARB, TRUE,      // Enable render to p-buffer
		WGL_RED_BITS_ARB, 8,                // At least 8 bits for RED channel
		WGL_GREEN_BITS_ARB, 8,              // At least 8 bits for GREEN channel
		WGL_BLUE_BITS_ARB, 8,               // At least 8 bits for BLUE channel
		WGL_ALPHA_BITS_ARB, 8,              // At least 8 bits for ALPHA channel
		WGL_DEPTH_BITS_ARB, 16,             // At least 16 bits for depth buffer
		WGL_DOUBLE_BUFFER_ARB, FALSE,       // We don't require double buffering
		0                                   // Zero terminates the list
	};

  // pick a pixel format
	unsigned int count = 0;
	int pixelFormat;
	wglChoosePixelFormatARB( m_hiddenWindow.GetContext()->GetHDC(),(const int*) pf_attr, NULL, 1, &pixelFormat, &count);
	if( count == 0 )
	{
    LogErr( "Could not find an acceptable pixel format!", "vxRenderingContextPBuffer", "Create" );
		return false;
	}

	// Create a generic p-buffer
	m_hPBuffer = wglCreatePbufferARB( m_hiddenWindow.GetContext()->GetHDC(), pixelFormat, m_uWidth, m_uHeight, NULL );
	m_hDC      = wglGetPbufferDCARB( m_hPBuffer );
	m_hRC      = wglCreateContext( m_hDC );

	if( !m_hPBuffer )
	{
    LogErr( "Could not create the p-buffer!", "vxRenderingContextPBuffer", "Create" );
		return false;
	}
  return true;
} // Create( int iWidth, int iHeight )


/**
 * Destroys the rendering context
 */
void vxRenderingContextPBuffer::Destroy()
{
	if( m_hRC != NULL )
	{
		wglMakeCurrent( m_hDC, m_hRC );
		wglDeleteContext( m_hRC );
    wglReleasePbufferDCARB( m_hPBuffer, m_hDC );
		wglDestroyPbufferARB( m_hPBuffer );
		m_hRC = NULL;
	}
} // Destroy()


/**
 * Makes the rendering context current
 */
void vxRenderingContextPBuffer::MakeCurrent()
{
	if( !wglMakeCurrent( m_hDC, m_hRC ) )
    LogErr( "Could not make the p-buffer's context current!", "vxRenderingContextPBuffer", "MakeCurrent" );
} // MakeCurrent()


/**
 * Constructor for hidden window
 */
vxRenderingContextPBuffer::HiddenWindow::HiddenWindow()
{
  /// create underling window
  Create( NULL, 0, 0, WS_OVERLAPPEDWINDOW );
} // HiddenWindow()


/**
 * WM_CREATE handler. Creates the opengl rendering context
 *
 * @param   uMsg      window message
 * @param   wParam    associated wParam
 * @param   lParam    associated lParam
 * @param   bHandled  indicates if message was handled
 * @return  LRESULT   
 */
LRESULT vxRenderingContextPBuffer::HiddenWindow::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    // initialize the paint device
    m_pDevice = new GLPaintDevice;
    m_pDevice->SetHDC( GetDC() );
    m_pDevice->SetHWND( m_hWnd );

    // create and initialize the rendering context    
    m_pContext = new vxRenderingContextGL(GLFormat(), m_pDevice);
    m_pContext->Create();
    m_pContext->SetInitialized(true);   

  return 0;
} // OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )


/**
 * WM_DESTROY handler. Destriys the opengl rendering context
 *
 * @param   uMsg      window message
 * @param   wParam    associated wParam
 * @param   lParam    associated lParam
 * @param   bHandled  indicates if message was handled
 * @return  LRESULT   
 */
LRESULT vxRenderingContextPBuffer::HiddenWindow::OnDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
   // clean up the GL Context
  if( m_pContext != NULL )
  {
    m_pContext->DoneCurrent();
    m_pContext->Reset();
    delete m_pContext;
    m_pContext = NULL;
  }

  // free the associated device context
  if ( m_pDevice != NULL )
  {
    delete m_pDevice;
    m_pDevice = NULL;
  }
  return 0;
} // OnDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )


// $Log: vxRenderingContextPBuffer.C,v $
// Revision 1.1  2007/10/24 20:33:49  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderingContextPBuffer.C,v 1.1 2007/10/24 20:33:49 gdavidson Exp $
// $Id: vxRenderingContextPBuffer.C,v 1.1 2007/10/24 20:33:49 gdavidson Exp $
