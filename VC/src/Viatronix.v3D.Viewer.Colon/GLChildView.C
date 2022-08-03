// $Id: GLChildView.C,v 1.18.2.5 2010/03/09 20:14:17 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: GLChildView.C  
// Owner: Jeffrey Meade  jmeade@viatronix.net


// includes
#include "stdafx.h"
#include "WinUtils.h"
#include "GLChildView.h"
#include "Viatron.h"
#include "MainAppFrame.h"
#include "glFont.h"
#include "Global.h"
#include "V1KMessages.h"
#include "GdiplusUtilities.h"


// defines
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define FILE_REVISION "$Revision: 1.18.2.5 $"


// namespaces
using namespace ColonViews::Layouts;
using namespace WinUtils;


// static declarations
GLChildView::GLVID GLChildView::uNextClassInstanceID = 1;
bool GLChildView::m_bReportRendering(false);

// message map
BEGIN_MESSAGE_MAP(GLChildView, CWnd)
  ON_WM_CREATE()
  ON_WM_DESTROY()
  ON_WM_ERASEBKGND()
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_MBUTTONDBLCLK()
  ON_WM_MBUTTONDOWN()
  ON_WM_MBUTTONUP()
  ON_WM_RBUTTONDBLCLK()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEWHEEL()
  ON_WM_SETCURSOR()
  ON_WM_SIZE()
  ON_WM_WINDOWPOSCHANGED()
  ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


/**
 * Constructor 
 *
 * @param eGLViewType   View window type
 * @param hParent       Parent window handle
 */
GLChildView::GLChildView( const ViewWndTypeEnum eGLViewType  ) :
  m_pdc( NULL ), 
  m_hrc( NULL ), 
  m_uControlID( 0 ), 
  m_uClassInstanceID( uNextClassInstanceID++ ), 
  m_eGLChildViewType( eGLViewType ),  
  m_pUpdateBuffer( NULL ), 
  m_uTexture( 0 ),
  m_bImageBuffered( false ), 
  m_bInverseImage(false),
  m_bRenderOnlyBackground( true ),
  m_pGLWnd( NULL )
{
} // GLChildView()


/**
 * Destructor 
 */
GLChildView::~GLChildView()
{ 
  if( m_pUpdateBuffer != NULL )
  {
    delete [] m_pUpdateBuffer;
  }
} // ~GLChildView()


/**
 * Called when the window is being created.
 *
 * @param lpCreateStruct  CREATESTRUCT structure that contains information about the CWnd object being created
 * @return                0 to continue creation, -1 to destroy the window  
 */
int GLChildView::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
  if( CWnd::OnCreate( lpCreateStruct ) == -1 )
    return -1;

  if( !GLCreate() )
  {
    LogErr( "Unable to create gl rendering context.", "GLChildView", "OnCreate" );
    return -1;
  }

  m_pGLWnd = dynamic_cast< GLWnd * >( GetParent() );

  return 0;
} // OnCreate()


/**
 * Called when the window is being destroyed
 */
void GLChildView::OnDestroy()
{
  GLDestroy();
  
  CWnd::OnDestroy();
} // OnDestroy();


/**
 * Called when the window needs painting.
 */
void GLChildView::OnPaint() 
{
  CPaintDC dc( this );
  GLRender();
  //m_bRenderOnlyBackground = false;
} // OnPaint()


/**
 * Called when background needs erasing
 *
 * @param pDC     Device context for window
 * @return BOOL   Non-zero to indicate NOT to erase, zero to let framework erase window
 */
BOOL GLChildView::OnEraseBkgnd( CDC * pdc ) 
{
  return TRUE;
} // OnEraseBkgnd


/**
 * Left button double-click
 *
 * @param uFlags   Indicates whether various virtual keys are down.
 * @param point    Coordinates of the cursor.
 */
void GLChildView::OnLButtonDblClk( UINT uFlags, CPoint point ) 
{
  GetParent()->SendMessage( WM_APP_SET_MAIN_VIEW_WINDOW, GetClassInstanceID(), 0 );
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL ), 0 );
} // OnLButtonDblClk()


/**
 * Right button down.
 *
 * @param uFlags   Indicates whether various virtual keys are down.
 * @param point    Coordinates of the cursor.
 */
void GLChildView::OnRButtonDown( UINT uFlags, CPoint point ) 
{  
  ShowPopupMenu( uFlags, point );
} // OnRButtonDown()


/**
 * Set window cursor for view
 *
 * @param pWnd       Pointer to window containing cursor
 * @param uHitTest   Code indicating cursor location
 * @param uMessage   Mouse message triggering set cursor event
 * @return           TRUE if cursor was set, FALSE otherwise (to set default for window)
 */
BOOL GLChildView::OnSetCursor( CWnd* pWnd, UINT uHitTest, UINT uMessage ) 
{
  SetCursor( AfxGetApp()->LoadCursor(IDC_CROSSMC) );
  return TRUE;
} // OnSetCursor()


/**
 * Resize event handler.
 *
 * @param uType   Type of resize event (see CWnd::OnSize)
 * @param iCX     Width of view.
 * @param iCY     Height of view.
 */
void GLChildView::OnSize( UINT uType, int4 iCX, int4 iCY )
{
  CWnd::OnSize( uType, iCX, iCY );

  CRect rcWindow;
  GetWindowRect( &rcWindow );
  GetParent()->ScreenToClient( &rcWindow );
  m_viewrect = Rect< int4 >( rcWindow.left, rcWindow.top, rcWindow.right, rcWindow.bottom );

  SetViewPort( iCX, iCY );

} // OnSize()


/**
 * Get the bounding rectangle for the view
 * 
 * @return the bounding rectangle for the view
 */
const Rect< int4 > GLChildView::GetViewRect() const
{
  if ( ::IsWindow( GetSafeHwnd() ) )
  {
    CRect rcWindow;
    GetWindowRect( &rcWindow );
    GetParent()->ScreenToClient( &rcWindow );
    return Rect< int4 >( rcWindow.left, rcWindow.top, rcWindow.right, rcWindow.bottom );
  }
  return Rect< int4 >( 0, 0, 1, 1 );
} // GetViewRect()


/**
 * Sets the viewport
 *
 * @param iWidth    Width of viewport
 * @param iHeight   Height of viewport
 */
void GLChildView::SetViewPort( int4 iWidth, int4 iHeight )
{
    // cache the viewport for quick access
  m_viewport = Rect< int4 >( 0, 0, iWidth, iHeight );  

  // Set the update buffer size
  m_iUpdateWidth  = m_viewport.Width();
  m_iUpdateWidth  = pow(2.0, ceil(LogB(2, m_iUpdateWidth)));
  m_iUpdateHeight = m_viewport.Height();
  m_iUpdateHeight = pow(2.0, ceil(LogB(2, m_iUpdateHeight)));
  
  // make the width and height same to avoid some problems.
  if( m_iUpdateWidth != m_iUpdateHeight )
  {
    m_iUpdateWidth = (m_iUpdateWidth > m_iUpdateHeight) ? m_iUpdateWidth : m_iUpdateHeight;
    m_iUpdateHeight = (m_iUpdateWidth > m_iUpdateHeight) ? m_iUpdateWidth : m_iUpdateHeight;
  }
  
  // Set update buffer for resize
  if( m_pUpdateBuffer !=  NULL )
  {
    delete [] m_pUpdateBuffer;
    m_pUpdateBuffer = NULL;
  }

} // SetViewPort()


/**
 * Window message processor
 *
 * @param uMmessage   Message
 * @param wParam      Message-dependent parameter
 * @param lParam      Message-dependent parameter
 * @return            Message-dependent
 */
LRESULT GLChildView::WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam ) 
{
  if( uMessage == WM_MOUSELEAVE )
  {																																																																																																																																																																																																																																																																																																														
    OnMouseLeave();
    return 0;
  }
  return CWnd::WindowProc(uMessage, wParam, lParam);
} // WindowProc()


/**
 * Sets the type of the child view
 *
 * @param ViewWndTypeEnum eType   Type of the child view
 */
void GLChildView::SetViewWndType(const ViewWndTypeEnum eType)
{
  switch (eType)
  {
  case eGVT_OVERVIEW:
  case eGVT_REPORTENTRY:
  case eGVT_CROSS_SECTION:
  case eGVT_NAVIGATION:
  case eGVT_CONTROLS:
  case eGVT_AXIAL:
  case eGVT_SAGITTAL:
  case eGVT_CORONAL:
  case eGVT_UDSEGMENTSMAUI:
    m_eGLChildViewType = eType;
    break;
    
  default:
    return;
  }
} // SetViewWndType()


/**
 * invalidates the specified views
 *
 * @param wParam   wparam of message to send.
 * @param lParam   lparam of message to send.
 */
void GLChildView::InvalidateViews( WPARAM wParam, LPARAM lParam )
{
  m_pGLWnd->OnAppGLChildViewTypeInvalidate( wParam, lParam );
} // InvalidateViews()


/**
 * Returns true if this window has the mouse capture.
 *
 * @return   True if this window has the mouse capture.
 */
bool GLChildView::IsCaptured()
{
  return ( GetCapture() == this );
} // IsCaptured()


/**
 * Returns true if this window is the current 'main' view
 *
 * @return  True if this is the main view, false otherwise
 */
bool GLChildView::IsMainViewWindow() const
{
  if ( GetParent() == NULL )
    return false;

  GLVID iMainID = GetParent()->SendMessage( WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID, GetClassInstanceID(), 0 );
  
  return (!iMainID || (iMainID == GetClassInstanceID()));
} // IsMainViewWindow()


/**
 * Creates the opengl rendering context for this child view window.
 *
 * @return true on success; false otherwise
 */
bool GLChildView::GLCreate()
{
  m_pdc = new CClientDC( this );
  
  HDC hdc = m_pdc->GetSafeHdc();
  
  static PIXELFORMATDESCRIPTOR pfd =
  {
    sizeof(PIXELFORMATDESCRIPTOR),
      1,
      PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
      PFD_TYPE_RGBA,
      24,
      0,0,0,0,0,0,
      0,0,
      0,0,0,0,0,
      32,
      1, 
      0,
      PFD_MAIN_PLANE,
      0,
      0,0,0
  };
  VERIFY( SetPixelFormat( hdc, ChoosePixelFormat( hdc, &pfd ), &pfd ) );
  
  m_hrc = wglCreateContext( hdc );

  VERIFY( wglMakeCurrent( hdc, m_hrc ) );

  GetNormalFont().Create( hdc, -15, 0, "arial", FW_SEMIBOLD, false, false, true );
  GetLargeFont().Create( hdc, -20, 0, "arial", FW_SEMIBOLD, false, false, true );

  return true;
} // GLCreate()


/**
 * Window destruction; releases the rendering context.
 */
void GLChildView::GLDestroy()
{
  wglMakeCurrent( GetHDC(), GetRenderingContext() );
  if( m_hrc != NULL )
  {
    if( m_uTexture )
    {
      glDeleteTextures( 1, & m_uTexture );
      m_uTexture = 0;
    }

    wglDeleteContext( m_hrc );
    m_hrc = NULL;
  }

  if( m_pdc != NULL )
  {
    delete m_pdc;
    m_pdc = NULL;
  }
} // GLDestroy()


/**
 * Default rendering context initialization
 */
void GLChildView::GLSetupRC()
{
  const GLfloat vfWhiteLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
  const GLfloat vfSourceLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  const GLfloat vfLightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  
  glEnable( GL_DEPTH_TEST );
  
  glEnable( GL_CULL_FACE );
  glFrontFace( GL_CCW );
  
  glEnable( GL_LIGHTING );
  
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, vfWhiteLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, vfSourceLight);
  glLightfv(GL_LIGHT0, GL_POSITION, vfLightPos);
  glEnable(GL_LIGHT0);
  
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
} // GLSetupRC()


/**
 * 3D Scene rendering
 *
 * @param pData   Application-defined data (unused).
 */
void GLChildView::GLRenderScene(void *)
{
} // GLRenderScene()


/**
 * 2D Scene rendering (e.g. text)
 */
void GLChildView::GLRasterScene()
{
  // draw "not loaded" in the middle of the screen
  CRect clientRect;
  GetClientRect( clientRect );
  GLFont & font = GetNormalFont();

  glColor3f(1,1,0);
  // TODO: add resource for this string
  const std::string sMessage( LPCSTR(LoadResourceString(IDS_NOT_LOADED)) );
  font.DrawText( clientRect.Width() / 2 - font.GetPixelWidth( sMessage ) / 2, clientRect.Height() / 2, sMessage.c_str(), GDT_ABOVERIGHT );
} // GLRasterScene()


/**
 * Called during resize, initializes the viewport for 3D rendering context
 *
 * @param iWidth    Width of view
 * @param iHeight   Height of view
 */
void GLChildView::GLResize(GLsizei iWidth, GLsizei iHeight)
{
  wglMakeCurrent( GetHDC(), GetRenderingContext() );

  glViewport(m_viewport.m_left, m_viewport.m_top, iWidth, iHeight);
  
  glMatrixMode(GL_PROJECTION);
  
  glLoadIdentity();
  
  iHeight = max( 1, iHeight );
  
  const GLfloat fAspect( (GLfloat) iWidth / (GLfloat)iHeight );
  gluPerspective( 45.0f, fAspect, 1.0, 886.0 );
  
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
} // GLResize(GLsizei iWidth, GLsizei iHeight)


/**
 * Called during resize, initializes the viewport for 2D rendering context
 *
 * @param width    width of viewport.
 * @param height   height of viewport.
 */
void GLChildView::GLRasterResize(GLsizei width, GLsizei height)
{
  glViewport(m_viewport.m_left, m_viewport.m_top, width, height);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  gluOrtho2D( 0, m_viewport.Width(), 0, m_viewport.Height() );
} // GLRasterResize(GLsizei width, GLsizei height)


/**
 * Renders the view (includes render and raster)
 */
void GLChildView::GLRender( bool bSwapBuffers )
{
  try
  {
    wglMakeCurrent( GetHDC(), GetRenderingContext() );
    
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    {
      GLSetupRC();
      GLResize( m_viewport.Width(), m_viewport.Height() );

      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      if( !m_bRenderOnlyBackground )
      {
        GLRenderScene();

        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        GLRasterResize( m_viewport.Width(), m_viewport.Height() );
        GLRasterScene();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
      }
    }
    glPopAttrib();

    if( bSwapBuffers )
      SwapBuffers( GetHDC() );
  } // try
  catch ( ... )
  {
    LogWrn( "unhandled exception", "GLChildView", "GLRender" );
  }
} // GLRender()



/**
 * Retrieves update image; DO NOT DELETE DATA!
 * Returns pointer to temporary data buffer that should NOT be deleted.
 *
 * @param iWidth               Width of returned update buffer.
 * @param iHeight              Height of returned update buffer.
 * @param iWidthAlignedDWORD   Width, DWORD aligned
 * @param ePixelFormat         Pixel data format.
 * @param eDatatype            Data type of buffer pixels.
 * @param bOnlyGLRenderScene   True to call GLRenderScene only, false to call both render and GLRasterScene.
 * @return                     Pointer to temporary update buffer data.
 */
const void  * GLChildView::GetUpdateBuffer(int4 & iWidth, int4 & iHeight, int4 & iWidthAlignedDWORD, const GLenum ePixelFormat,
                                              const GLenum eDatatype, const bool bOnlyGLRenderScene)
{
  GLRender( false );

  wglMakeCurrent( GetHDC(), GetRenderingContext() );

  CRect rc( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right-1, GetViewport().m_bottom );

  static const uint1 uBytesPerPixel( 3 );
  static const uint1 uBitsPerPixel( 24 );

  iWidth = rc.Width();
  iHeight = rc.Height();
  iWidthAlignedDWORD = ( (rc.Width() * uBitsPerPixel + 31) & (~31) ) / 8;
  
  // NOTE: doubled the buffer size to prevent crash on ATI cards
  if( (iWidthAlignedDWORD <= 0) ||
      (iHeight <= 0) ||
      (!m_tempUpdateBuffer.Allocate(eDatatype, Point2D<uint4>(iWidthAlignedDWORD*2,iHeight), 1)) )
    return NULL;
  
  glPushAttrib( GL_PIXEL_MODE_BIT );  
  {
    glReadBuffer( GL_BACK );
    glPixelStorei( GL_PACK_ALIGNMENT, 4 );
    glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
    glPixelStorei( GL_PACK_SKIP_ROWS, 0 );
    glPixelStorei( GL_PACK_SKIP_PIXELS, 0 );
    
    glReadPixels( 0, 0, iWidth, iHeight, ePixelFormat, eDatatype, LPVOID( m_tempUpdateBuffer.m_pBuffer ) );
  }
  glPopAttrib();
  
  wglMakeCurrent(NULL, NULL);
  
  return m_tempUpdateBuffer.m_pBuffer;
} // GetUpdateBuffer()


/**
 * Stores the current view in the internal update buffer (for use in RenderUpdateBuffer)
 *
 * @param bInverseImig   Whether or to to invert the image
 */
void GLChildView::GLSaveUpdateBuffer(const bool bInverseImg/* =false */)
{
  const int4 iReadWidth(  GetViewRect().Width());
  const int4 iReadHeight(  GetViewRect().Height());
  if( ( iReadWidth <= 0 ) || ( iReadHeight <= 0 ) )
    return;
  
  // initialize buffer if necessary
  if( m_pUpdateBuffer == NULL )
    m_pUpdateBuffer = new unsigned char[ m_iUpdateWidth * m_iUpdateHeight * 3 ];
  
  wglMakeCurrent(GetHDC(), GetRenderingContext());
  glFinish();
  glEnable( GL_TEXTURE_2D );
  
  if( m_uTexture == 0 )
    glGenTextures( 1, &m_uTexture );
  
  //glReadBuffer( GL_BACK );
  glPixelStorei( GL_PACK_ALIGNMENT, 1 );
  glPixelStorei( GL_PACK_ROW_LENGTH, m_iUpdateWidth );
  // NOTE: Machine will CRASH for invalid values (e.g. non-zero) for glReadPixels; 
  // must check as is done at start of this (SaveUpdateBuffer) function
  glReadPixels( m_viewport.m_left, m_viewport.m_top, iReadWidth, iReadHeight, GL_RGB, GL_UNSIGNED_BYTE, (void*) m_pUpdateBuffer );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glPixelStorei( GL_UNPACK_ROW_LENGTH, m_iUpdateWidth );
  
  // inverse image
  if (bInverseImg) 
    ImageProcess( eIP_RedBlueSwap, m_pUpdateBuffer, m_iUpdateWidth, m_iUpdateHeight );
 
  glBindTexture( GL_TEXTURE_2D, m_uTexture );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  glTexImage2D( GL_TEXTURE_2D, 0, 3, m_iUpdateWidth, m_iUpdateHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pUpdateBuffer );

  glBindTexture(GL_TEXTURE_2D, 0);
  
  m_bImageBuffered = true;
} // GLSaveUpdateBuffer(const bool bInverseImg/* =false */)


/**
 * Renders the latest view (stored in the update buffer)
 *
 * @param bAdjustViewport   whether to adjust the viewport according to the updated render buffer
 * @return                  success/failure
 */
bool GLChildView::GLRenderUpdateBuffer( const bool bAdjustViewport/* =false */ )
{
  if( ( m_pUpdateBuffer == NULL ) || ( m_iUpdateWidth <= 0 || m_iUpdateHeight <= 0 ) )
    return false;
  
  // texture map the last real-rendered image
  CRect rect( 0, 0, m_iUpdateWidth, m_iUpdateHeight );
  if( IsWindow( m_hWnd ))
    GetClientRect(&rect);
  const float fTX(float(rect.Width()) / float(m_iUpdateWidth));
  const float fTY(float(rect.Height()) / float(m_iUpdateHeight));
  
  if( m_uTexture == 0 )
    return false;
  
  // adjust view port if necessary (such as in SliceChildView, where the viewport has been changed)
  if( bAdjustViewport )
    glViewport( m_viewport.m_left, m_viewport.m_top, rect.Width(), rect.Height() );
  
  glEnable( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, m_uTexture );
  
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  {
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    {
      glLoadIdentity();
      glOrtho(0, 1, 0, 1, 0, -1);        
      glBegin(GL_QUADS);
      {
        glTexCoord2d(0 ,  0);   glVertex3f(0, 0, 0);
        glTexCoord2d(0 ,  fTY); glVertex3f(0, 1, 0);
        glTexCoord2d(fTX, fTY); glVertex3f(1, 1, 0);
        glTexCoord2d(fTX, 0);   glVertex3f(1, 0, 0);
      } glEnd();
      glDisable(GL_TEXTURE_2D);
      
    }
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
  }
  glPopMatrix();
  //glBindTexture(GL_TEXTURE_2D, 0);
  return true;
} // GLRenderUpdateBuffer( const bool bAdjustViewport/* =false */ )


/**
 * Idle event handler
 *
 * @return   whether or not an update event should be triggered, due to change of displayed data
 */
bool GLChildView::GLIdle()
{
  return false;
} // GLIdle()


/**
 * Renders the "Viatronix" logo
 *
 * @param bTranslucent    Render partially translucent
 */
void GLChildView::GLRenderLogo( bool bTranslucent )
{
  GLFont & font = GetNormalFont();

  if( bTranslucent )
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  }

  glColor4f( 1, 1, 1, 0.75F );

  // "Viatronix" at bottom, middle
  static const char * sLogo = "Viatronix";
  //font.DrawText( int4( ( GetViewport().Width() - font.GetPixelWidth(sLogo) ) * 0.5F ), GetViewport().m_top + m_iTextPadding + m_iBottomExtraTextPadding, sLogo, GDT_ABOVERIGHT );

  // draw the logo as a bitmap
  static Image< PixelRGBA<uint1> > * pLogoBitmap = NULL;
  if ( pLogoBitmap == NULL )
  {
    pLogoBitmap = GdiplusUtilities::LoadImageFromResource( MAKEINTRESOURCE( IDB_VIATRONIX_LOGO ), "PNG" );
    pLogoBitmap->FlipVertical();

  }
  if ( pLogoBitmap )
  {
    glPixelStorei( GL_UNPACK_ROW_LENGTH, pLogoBitmap->GetDim().m_x );
    glPixelTransferf( GL_ALPHA_SCALE, bTranslucent ? 0.5F : 1.0F );
    const int4 iRasterX = int4( ( GetViewport().Width() - pLogoBitmap->GetDim().m_x ) * 0.5F );
    const int4 iRasterY = GetViewport().m_top;
    glRasterPos2i( iRasterX, iRasterY );
    try
    {
      glEnable( GL_BLEND );
      glBlendFunc( GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA );
      glDrawPixels( pLogoBitmap->GetDim().m_x, pLogoBitmap->GetDim().m_y, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast< GLubyte * >( pLogoBitmap->GetDataPtr() ) );
      glDisable( GL_BLEND );
    }
    catch ( ... )
    {
    }
  }
  
  if( bTranslucent )
    glDisable(GL_BLEND);
} // GLRenderLogo()


/**
 * Performs special processing on an image.
 *
 * @param eType       identifies type of image processing to be performed.
 * @param pImage      pointer to an image.
 * @param uImgWidth   the width of the image.
 * @param uImgHeight  the height of the image.
 *
 */
void GLChildView::ImageProcess(const ImageProcessEnum eType, uint1 * pImage, const uint4 uImgWidth, const uint4 uImgHeight)
{
  switch (eType)
  {
  case eIP_RedBlueSwap: // Swap 1st and 3rd (usually Red and Blue) image colors bits
    {
      uint1 * pPtr = pImage;
      uint1 uTemp;
      const uint4 uImgSize(uImgWidth * uImgHeight);
      for (uint4 x=0; x < uImgSize; x++, pPtr += 3)
      {
        uTemp = *pPtr;
        *pPtr = *(pPtr+2);
        *(pPtr+2) = uTemp;
      }
    }
    break;
    
  default:
    break;
  }
} // ImageProcess()


/**
 * Invalidate window region of a control linked to this window
 *
 * @param control   control to invalidate
 */
void GLChildView::InvalidateAsscociatedControl( CWnd & control )
{
  CRect rc;
  control.GetWindowRect( &rc );
  control.GetParent()->ScreenToClient( &rc );
  control.GetParent()->InvalidateRect( &rc, FALSE );
} // InvalidateAsscociatedControl()


//////////////////// Popup Menus ///////////////////////////////////////////////////////


/**
 * Returns a prepared (items enabled/disabled/removed per context) popup menu for display
 *
 * @param uFlags      Mouse flags
 * @param point       Mouse point of event triggering popup menu
 * @param popupMenu   Out: Child view popup menu, should be created before invocation with default constructor
 * @return bool       Success of popup menu creation/validation
 */
bool GLChildView::PreparePopupMenu(UINT uFlags, CPoint point, CMenu & popupMenu)
{
  // Load the view window popup menu
  CMenu *pSubMenu = NULL;

  uint4 uMenuID( GetPopupMenuID( uFlags, point ) );
  uMenuID = dynamic_cast< GLWnd*>( GetParent() )->OnShowPopupMenu( *this, uMenuID);

  if (uMenuID == 0)
    return false;

  popupMenu.LoadMenu(uMenuID);
  if (
#ifdef _DEBUG
    (!::IsMenu(popupMenu.GetSafeHmenu())) ||
#endif
    ((pSubMenu = popupMenu.GetSubMenu(0)) == (CMenu *) NULL) )
  {
    return false;
  }

  GLChildViewRef view = *this;
  view.AdjustPopupMenu(*pSubMenu, uMenuID);

  // disable the "Main Window" option if not available
  if (view.IsMainViewWindow())
  {
    pSubMenu->EnableMenuItem(IDM_VIEWWINDOW_MAINWINDOW, MF_GRAYED | MF_BYCOMMAND);
  }

  // Remove separators that are: a) 1st item; b) last item; c) duplicate (more than one separator in succession)
  MENUITEMINFO itemInfo = { sizeof(MENUITEMINFO), MIIM_TYPE };
  UINT uPrevItemType(MFT_SEPARATOR); // type of the previous menu item
  for (uint4 uPos(0); uPos < pSubMenu->GetMenuItemCount(); uPos++)
  {
    pSubMenu->GetMenuItemInfo(uPos, &itemInfo, TRUE);
    if ( ((uPrevItemType & MFT_SEPARATOR) || (uPos+1 == pSubMenu->GetMenuItemCount())) && (itemInfo.fType & MFT_SEPARATOR) )
    {
      // Delete the current item if it and the previous are separators, or if the last is a separator
      pSubMenu->DeleteMenu(uPos, MF_BYPOSITION);
      // After deletion, all the other items will move, so step back to check the new current
      uPos--;
    }

    uPrevItemType = itemInfo.fType;
  }

  return true;
} // PreparePopupMenu()


/**
 * Displays a child view's popup menu
 *
 * @param uFlags  Mouse flags for event triggering popup menu display
 * @param point   Point
 * @return int4   Selected menu item command ID, or zero if non selected, or -1 if menu not displayed
 */
int4 GLChildView::ShowPopupMenu(UINT uFlags, CPoint point)
{
  if ( (GetAsyncKeyState(VK_CONTROL)&0x80000) || (GetAsyncKeyState(VK_SHIFT)&0x80000)
    || (GetAsyncKeyState(VK_MENU)&0x80000) )
  {
    return -1;
  }
  
  // Load the view window popup menu
  GLChildViewRef currView = *this;
  CMenu popupMenu;
  //const bool bSuccess(PreparePopupMenu(uFlags,
  //  point - CPoint(currView.GetViewRect().m_left,currView.GetViewRect().m_top), popupMenu));

  const bool bSuccess(PreparePopupMenu(uFlags,point, popupMenu));

  if (!bSuccess)
  {
    return -1;  // No menu displayed
  }

  int4 iMenuCmd(0);

  CMenu* pSubMenu = popupMenu.GetSubMenu(0);
  if (pSubMenu == (CMenu *) NULL)
  {
    return iMenuCmd;
  }

  // Menus are positioned relative to screen coordinates
  this->ClientToScreen(&point);
    
  // The framework is supposed to post these messages around the TrackPopupMenu()
  //  call to the application's main window, but I sure didn't see evidence of that!!!
  AfxGetMainWnd()->PostMessage(WM_ENTERMENULOOP, TRUE, 0);
  // Show popup menu, get user selection
  iMenuCmd = 
    TrackPopupMenu(pSubMenu->GetSafeHmenu(), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
    point.x, point.y, 0, GetSafeHwnd(), NULL);
  AfxGetMainWnd()->PostMessage(WM_EXITMENULOOP, TRUE, 0);

  // Force an immediate parent window client area update
  //  (to draw over the newly removed popup menu)
  RedrawWindow();

  if (iMenuCmd != 0)
  {
    // Give the current child view the first crack at the command,
    //  then send it to the parent view window if not handled by the child
    if (currView.OnCommand(iMenuCmd, 0) == 0)
    {
      SendMessage(WM_COMMAND, iMenuCmd, 0);
    }
  }

  // Notify the window of the current position of the mouse cursor
  CPoint currPt;
  GetCursorPos(&currPt);
  //this->ScreenToClient(&currPt);

  // Notify that right button has been released (to, for e.g., release mouse capture)
  CRect rc; currView.GetClientRect( rc );
  CPoint somePointInView = rc.CenterPoint();
  this->SendMessage(WM_RBUTTONUP,0,MAKELPARAM(somePointInView.x, somePointInView.y));

  // This effectively updates cursor position, current view, and sets cursor icon
  MSG msg;
  msg.hwnd = GetParent()->GetSafeHwnd();
  msg.message = WM_MOUSEMOVE;
  msg.wParam = 0;
  msg.lParam = MAKELPARAM(currPt.x, currPt.y);
  GetParent()->PreTranslateMessage( &msg );
  GetParent()->PostMessage(WM_SETCURSOR, WPARAM(this->GetSafeHwnd()), MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));

  if (iMenuCmd != 0)
    m_pGLWnd->GLRenderViews();

  return iMenuCmd;
} // ShowPopupMenu()


/**
 * Sends a message to this child view.
 *
 * @param uMessage   Message ID.
 * @param wParam     Message-dependent parameter.
 * @param lParam     Message-dependent parameter.
 * @return           Return of message handler (or override).
 */
LRESULT GLChildView::SendMessage( UINT uMessage, WPARAM wParam, LPARAM lParam )
{
  if (uMessage == WM_COMMAND)
  {
    return OnCommand(wParam, lParam);
  }
  else
  {
    return OnMessage(uMessage, lParam);
  }
  return 0;
} // SendMessage()


// $Log: GLChildView.C,v $
// Revision 1.18.2.5  2010/03/09 20:14:17  dongqing
// print study info on every snapshot image
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.18.2.4  2009/12/16 21:10:59  dongqing
// flip vertical method in Image class
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.18.2.3  2009/10/07 16:13:13  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.18.2.2  2007/10/03 20:01:37  jmeade
// typos.
//
// Revision 1.18.2.1  2007/09/20 17:07:14  jmeade
// unicode char set for all fonts.
//
// Revision 1.18  2007/03/06 01:47:46  jmeade
// comments.
//
// Revision 1.17  2007/02/15 00:08:21  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.16  2007/02/02 21:10:30  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.15  2007/01/03 16:51:40  jmeade
// future ud segments layout.
//
// Revision 1.14  2006/11/07 20:09:26  jmeade
// strings to string table.
//
// Revision 1.13  2006/11/02 19:24:32  jmeade
// prevent debug exception for menu id == 0.
//
// Revision 1.12  2006/10/26 20:37:05  frank
// replaced Viatronix text at bottom of images with actual logo
//
// Revision 1.11  2006/10/11 22:09:11  gdavidson
// Issue #4997: Double the image buffer size
//
// Revision 1.10  2006/02/07 23:34:37  jmeade
// code review: comments.
//
// Revision 1.9  2006/01/31 18:20:18  romy
// code review-rerranged include files
//
// Revision 1.8  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.7  2005/11/21 23:15:54  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.6  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.5  2005/11/04 18:06:09  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.4.2.6  2006/05/09 13:53:59  geconomos
// corrected GetViewrect to return a copy of a rect<int4> instead of passing a temporary reference back
//
// Revision 1.4.2.5  2006/01/19 19:53:32  jmeade
// Issue 4604: GetUpdateBuffer() needs to set current gl context.
//
// Revision 1.4.2.4  2005/12/23 16:49:15  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.4.2.3  2005/11/17 16:30:42  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.4.2.2  2005/11/15 13:22:26  frank
// additional error handling
//
// Revision 1.4.2.1  2005/11/04 18:21:53  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.5  2005/11/04 18:06:09  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.4  2005/10/25 15:47:37  jmeade
// ParentViews ==> Layouts
//
// Revision 1.3  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.2.2.1  2005/09/21 13:27:15  geconomos
// modified code to make non-window calls during rendering
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.18.2.5.4.9.2.7  2005/07/15 18:25:41  geconomos
// Added methods to invalidate parent window where external controls are placed.
//
// Revision 3.18.2.5.4.9.2.6  2005/07/15 17:21:43  geconomos
// Issue # 4225: Drawing is choppy while switching views
//
// Revision 3.18.2.5.4.9.2.5  2005/07/14 14:16:30  geconomos
// Refactored logo renering code
//
// Revision 3.18.2.5.4.9.2.4  2005/07/05 19:22:00  geconomos
// Issue #4246: Fixed handling of context menus; ensure that the main view wnd is set before displaying
//
// Revision 3.18.2.5.4.9.2.3  2005/06/30 16:37:01  geconomos
// Proper rerendering of views on WM_COMMAND handling
//
// Revision 3.18.2.5.4.9.2.2  2005/06/21 15:52:51  geconomos
// restored revision history
//
// Revision 3.18.2.6.4.10.2.1  2005/06/21 15:14:50  geconomos
// Added method to set the viewport size; changed GetHDC() to be virtual
//
// Revision 3.18.2.6.4.10  2005/06/10 18:20:38  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 3.18.2.6.4.9  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.18.2.6.4.7.2.2  2005/06/08 14:01:10  geconomos
// continued effort ( sorry for lame comment! )
//
// Revision 3.18.2.6.4.7.2.1  2005/05/25 15:50:22  geconomos
// initial backup
//
// Revision 3.18.2.5.4.7  2005/04/21 12:55:17  frank
// rollback to Tuesday's version - stable but slow
//
// Revision 3.18.2.5.4.6  2005/04/20 14:45:08  frank
// fixed font height and width computation and retrieval
//
// Revision 3.18.2.5.4.5  2005/04/19 13:40:55  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 3.18.2.5.4.4  2005/04/13 18:54:56  frank
// split out GL update buffer class
//
// Revision 3.18.2.5.4.3  2005/04/08 13:05:58  frank
// capitalization
//
// Revision 3.18.2.5.4.2  2005/04/05 19:25:42  frank
// Issue #4065: draws a blank image if no data loaded
//
// Revision 3.18.2.5.4.1  2005/03/21 19:53:59  frank
// Issue #4038: Sped up font drawing
//
// Revision 3.18.2.5  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.18.2.4  2003/07/18 17:54:12  jmeade
// Comments.
//
// Revision 3.18.2.3  2003/07/18 16:13:41  jmeade
// Child view timers, code standards, comments.
//
// Revision 3.18.2.2  2003/04/15 21:33:28  jmeade
// Issue 3001: Allow childview to alter display rectangle on sizing.
//
// Revision 3.18.2.1  2003/02/26 17:41:17  jmeade
// More concise method for triggering child view updates.
//
// Revision 3.18  2003/01/03 23:08:15  jmeade
// Reordered includes to appease gdi+.
//
// Revision 3.17  2002/11/19 20:17:39  jmeade
// Fixed memory leak: must delete 2D rendering context in child view.
//
// Revision 3.16  2002/11/04 20:37:37  kevin
// Mad annotations 3D instead of 2D
//
// Revision 3.15  2002/09/27 14:21:14  ingmar
// replaced Size2D with Point2D
//
// Revision 3.14  2002/09/06 16:10:26  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.13  2002/08/22 13:20:01  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.11  2002/07/29 17:06:34  jmeade
// Must ensure valid/non-zero values in calls to glReadPixels, or else....CRASH!!!!;
//  Coding standards.
//
// Revision 3.10  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.9  2002/06/04 18:36:59  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.8  2002/04/12 02:12:22  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.7  2002/03/13 22:01:15  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.6.2.6  2002/08/07 23:03:44  jmeade
// ImageProcess function cleanup.
//
// Revision 3.6.2.5  2002/07/01 19:22:55  jmeade
// Expose GLUpdateBuffer class, add allocation routine.
//
// Revision 3.6.2.4  2002/06/17 18:31:47  jmeade
// Code standards.
//
// Revision 3.6.2.3  2002/06/12 02:20:09  jmeade
// Issue 2423:  Use one GetUpdateBuffer method that returns private, temporary memory.
//
// Revision 3.6.2.2  2002/02/26 16:54:15  binli
// issue 1543: inverse image when taking snapshot.
//
// Revision 3.6.2.1  2002/02/05 19:57:50  jmeade
// Comments; Code standards.
//
// Revision 3.6  2001/12/22 00:08:32  jmeade
// Generalized update buffer retrieval function; Code standards.
//
// Revision 3.5  2001/11/20 22:03:09  binli
// solved problem of unstable slice image: reset viewport when rendering buffered slice image
// (SliceChildView: try to center the slice, so viewport is different from viewwnd.)
//
// Revision 3.4  2001/11/17 03:32:31  jmeade
// Windows created from forms; IsVisible() and controlID characteristics of
//  child views; threw out the old carpet
//
// Revision 3.3  2001/11/09 01:16:58  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.2.2.1  2001/11/04 04:31:20  jmeade
// Coding standards.
//
// Revision 3.2  2001/10/19 02:27:15  jmeade
// Get current mouse capture convenience functions; Coding standards
//
// Revision 3.1  2001/10/17 21:56:44  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:01:57  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 12 2001 23:07:46   jmeade
// Coding standards
// 
//    Rev 2.0   Sep 16 2001 23:41:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:10   ingmar
// Initial revision.
// Revision 1.56  2001/06/21 21:15:32  binli
// speed up overview/slice render in StandardView/Endo-OrthogonalView.
// (defect 263, partial)
//
// Revision 1.55  2001/05/12 00:00:59  jmeade
// Coding conventions
//
// Revision 1.54  2001/04/13 02:33:21  jmeade
// headers and footers
//
// Revision 1.53  2001/04/05 17:56:29  jmeade
// Coding standards!!!
//
// Revision 1.52  2001/03/08 15:52:10  geconomos
// Worked on grabbing Non-Distended regions when creating report.
//
// Revision 1.51  2001/02/28 23:30:30  liwei
// Overview rerendered when buttons on VerifyView clicked
//
// Revision 1.50  2001/02/28 17:53:55  liwei
// Overviews in VerifyView and Supine/ProneViews are set to buffer rendered image.
// That is, call RenderUpdateBuffer() rather than RenderObjects whenever possible
//
// Revision 1.49  2001/02/09 20:41:22  jeff
// Crash fix: memory for m_pDialogBar is not created by GLChildView
//
// Revision 1.48  2001/02/06 22:27:23  jeff
// moved the default dialog bar position closer to the view
//
// Revision 1.47  2001/02/05 23:32:35  liwei
// Non-live views in EndoAxialView are also grayed out
//
// Revision 1.46  2001/02/05 22:50:00  liwei
// Flag showing whether non-active views grayed out moved to ViatronViewWnd
//
// Revision 1.45  2001/02/05 22:19:31  liwei
// Non-active views grayed out during flying
//
// Revision 1.44  2001/02/02 16:28:02  liwei
// A try-catch pair added to wrap the rendering of TTP, hopefully solve the disapperaing problem
//
// Revision 1.43  2001/01/31 18:19:40  jeff
// CreateDlgBar(): doesn't need to take in a pointer; removed unnecessary friends
//
// Revision 1.42  2001/01/30 22:57:53  geconomos
// Added stacke based calls
//
// Revision 1.41  2001/01/29 22:14:11  liwei
// Texture object is exploited for update buffer
//
// Revision 1.40  2001/01/26 19:08:48  geconomos
// Changed GetupdateBuffer() to read from back buffer
//
// Revision 1.39  2001/01/24 20:42:03  liwei
// no message
//
// Revision 1.38  2001/01/15 19:46:08  jeff
// GetViewPort() method
//
// Revision 1.37  2001/01/15 16:37:55  jeff
// Bug: top and left were transposed, for offsetting points
//
// Revision 1.36  2001/01/13 00:33:19  jeff
// Using new Rect<> and Tuple<> classes; more CPoint and CRect translation convenience functions
//
// Revision 1.35  2001/01/11 21:57:53  geconomos
// Added GetUpdateBuffer()
//
// Revision 1.34  2001/01/02 21:02:38  jeff
// removed long-unused child view messages;
//  set view window type protected method, checks validity of type b/f setting
//
// Revision 1.33  2000/12/28 20:52:03  jeff
// SendMessage method, OnMessage handler, for general messages
//
// Revision 1.32  2000/12/28 19:53:54  jcohen
// Storage of database location and record number of current study
//
// Revision 1.31  2000/12/26 23:56:44  jeff
// ViewToChild/ChildToView convenience methods (akin to ScreenToClient())
//
// Revision 1.30  2000/12/26 21:06:37  jeff
// ViewToChild() and ChildToView() convenience methods
//
// Revision 1.29  2000/12/22 16:38:50  jeff
// MoveWindow() for dialog bar, strangely, doesn't preserve size set by parameters,
//  so use SetWindowPos() with SWP_NOSIZE
//
// Revision 1.28  2000/12/20 19:48:05  jeff
// reduced space between dialog bar and view window
//
// Revision 1.27  2000/12/12 23:51:44  jeff
// AdjustPopupMenu() now indicates which popup menu was specified
//
// Revision 1.26  2000/11/23 00:52:22  jeff
// Create one OpenGL font to use throughout application (instead of each
//  view creating their own every frame)
//
// Revision 1.25  2000/11/22 21:03:10  jeff
// GLChildView base class MUST return FALSE from OnCommand() to signify
//  a message (and i.e. all messages if not overridden) is not handled
//
// Revision 1.24  2000/11/20 00:56:21  jeff
// GetMouseKeyStringIDs() -- allows each view to set strings for mouse button use key
// AdjustPopupMenu() - takes menu reference instead of pointer
// Push/pop attrib instead of disabling
//
// Revision 1.23  2000/11/07 00:19:55  jeff
// Mouse hover/leave window event handlers
//
// Revision 1.22  2000/11/01 22:44:59  jeff
// Flags and point of the last mouse click passed into GetMenuPopupID()
// (for the child view to use to decide which popup menu to show)
//
// Revision 1.21  2000/10/27 01:16:50  kevin
// Added Texture Mag filter
//
// Revision 1.20  2000/10/25 17:55:54  jeff
// OnCommand overridable, for menu commands, etc.
// views can specify and adjust popup menus (ShowPopupMenu() must be called by parent view window)
//
// Revision 1.19  2000/10/13 04:55:35  jeff
// Increasing the border size between a view window and its dialog bar
//
// Revision 1.18  2000/09/30 18:30:21  jeff
// Fix the problem of the moving dialog bars
//
// Revision 1.17  2000/09/30 04:41:14  jeff
// Frank and Ingmar with your mathematical accuracy :)  Now I have to use
// ceil with your LogB function...accuracy shmacuracy!!!  :-)
//
// Revision 1.16  2000/09/30 02:51:47  jeff
// Get dialog bar window rect and set view rect functions; hide dialog bar while moving
//
// Revision 1.15  2000/09/28 15:24:46  ingmar
// LogN -> LogB
//
// Revision 1.14  2000/09/27 14:43:59  jeff
// Added LogN function to global utils library
//
// Revision 1.13  2000/09/22 23:16:52  jeff
// button menus
//
// Revision 1.12  2000/09/21 19:30:45  kevin
// Added height for updatebuffers
//
// Revision 1.11  2000/09/19 16:17:40  kevin
// Commented grey-screen debug (After 4 days with no success, I give up)
//
// Revision 1.10  2000/09/18 19:33:37  jeff
// New cross-hair cursor
//
// Revision 1.9  2000/09/18 15:06:19  kevin
// Added GLDebug to find grey-screen error
//
// Revision 1.8  2000/09/15 22:46:24  jeff
// no message
//
// Revision 1.7  2000/09/15 19:21:04  jeff
// Updating views only when necessary
//
// Revision 1.6  2000/09/15 16:09:28  jeff
// All rendering/drawing now done in OpenGL (no more GDI draw calls)
//
// Revision 1.5  2000/09/15 01:58:24  jeff
// Moving to text draw in OpenGL
//
// Revision 1.4  2000/09/14 16:14:51  jeff
// Borders around all view windows
//
// Revision 1.3  2000/09/13 16:31:17  jeff
// Removing unnecessary references to obsolete CChildView class
//
// Revision 1.2  2000/09/12 20:47:03  jeff
// Resizing and repositioning overview and (coming soon) visible-surface views
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 16    9/07/00 1:32p Jeff
// Embedded control dialog ("bars") in the GLChildView class, resized in
// OnSize
// 
// 15    9/06/00 12:44p Jeff
// Debug version clears view window
// 
// 14    9/01/00 7:59p Jeff
// Paramter to Set and Release Capture messages is now a child view
// instance ID (and no longer a variable address)
// 
// 13    8/28/00 7:27p Jeff
// Release capture message now passes GLChildViewPtr (as Set capture
// message does)
// 
// 12    8/28/00 1:17p Antonio
// updated header and footer
//
//*****************  Version 11  *****************
//User: Jeff            Date:  8/28/00  Time: 11:51a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Change of plan: references to popup menus shouldn't be here, the
//parent class should handle this
//
//*****************  Version 10  *****************
//User: Jeff            Date:  8/25/00  Time:  6:50p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Handling popup menus
//
//*****************  Version 9  *****************
//User: Jeff            Date:  8/25/00  Time:  5:28p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Removed ShowViewPopupMenu function
//
//*****************  Version 8  *****************
//User: Jeff            Date:  8/24/00  Time: 10:11a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  GLViewWnd's mouse-captured view is a member of GLViewWnd, and no
//longer a single global GLChildView member
//Changed WM_GL_..._INVALIDATE to WM_APP_GL_..._INVALIDATE
//
//*****************  Version 7  *****************
//User: Jeff            Date:  8/24/00  Time: 12:47a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Also return true for IsMainViewWindow() if the
//WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID message returns unanswered.
//
//*****************  Version 6  *****************
//User: Jeff            Date:  8/23/00  Time:  3:38p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Correct implementation of IsMainViewWindow()
//
//*****************  Version 5  *****************
//User: Jeff            Date:  8/22/00  Time: 11:46p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Invalidate using GL_CHILDVIEW_INVALIDATE message and overridden
//Invalidate() function calls in GLChildView
//
//**********************
//Label: Version 0.1
//User: Jeff            Date:  8/15/00  Time:  5:43p
//Labeled 'Version 0.1'
//Label comment:
//  First half-decent working version
//
//*****************  Version 4  *****************
//User: Jeff            Date:  8/14/00  Time:  1:27p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  
//
//*****************  Version 3  *****************
//User: Jeff            Date:  8/12/00  Time:  8:01p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  First errorless compile after changes to class architecture
//
//*****************  Version 2  *****************
//User: Jeff            Date:  8/12/00  Time:  5:59p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Renaming GLView ==> GLChildView to reflect new class architecture
//
//*****************  Version 1  *****************
//User: Jeff            Date:  8/11/00  Time:  3:44p
//Created GLView.C
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GLChildView.C,v 1.18.2.5 2010/03/09 20:14:17 dongqing Exp $
// $Id: GLChildView.C,v 1.18.2.5 2010/03/09 20:14:17 dongqing Exp $
