// $Id: DrawOverView.C,v 1.7.2.3 2010/03/10 00:42:00 dongqing Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (mailto:george@viatronix.com)

// includes
#include "stdafx.h"
#include "DrawOverView.h"

using namespace ColonViews::ChildViews;
using namespace ReaderLib;

/**
 * Constructor
 *
 * @param     pWnd          Parent window
 * @param     studyref      Dataset to render
 * @param     viewrect      view rectangle
 */
DrawOverView::DrawOverView( CWnd * pWnd, const Dataset& studyref, const Rect<int4>& viewRect ) : OverView( pWnd->GetSafeHwnd(), studyref.GetOrientation() )
{
  // turn off optimization
  SetRenderOnlyBackground( false );
  SetDisplayDistanceFromRectum(false);
  
  // allocate the dib
  m_dib.Create( viewRect.Width(), viewRect.Height() );

  // create the rendering context
  GLCreate();

  wglMakeCurrent( GetHDC(), GetRenderingContext() );

  // set the viewport
  SetViewPort( viewRect.Width(), viewRect.Height() );

  m_uViewWth = viewRect.Width();
  m_uViewHgt = viewRect.Height();

	// The DrawOverView must use the same trackball setting as that of current Overview
	// So, the followin track copy is not a need.
  // store the current trackbar
  //m_prevTrackball = *( studyref.m_pTrackball );
  
  //// create and initialize a new trackball based on the studies current orientation
  //studyref.m_pTrackball->Init( studyref.MatchOrientation( studyref.GetOrientation() ) );
  //studyref.m_pTrackball->ResetZoomFactor();

} // DrawOverView


/**
 * Destructor
 */
DrawOverView::~DrawOverView()
{
  // restore previous tackball
  //*( GetDatasetRef().m_pTrackball ) = m_prevTrackball;
} // ~DrawOverView()


/**
 * Creates the rendering context
 *
 * @return  true on success 
 */
bool DrawOverView::GLCreate()
{
  // attach to the dib's device conext
  CDC dc;
  dc.Attach( m_dib.GetHDC() );
 
  // get the info for the associated bitmap
  CBitmap * pBitmap = dc.GetCurrentBitmap();
  BITMAP bmInfo;
  pBitmap->GetObject( sizeof( BITMAP ), &bmInfo ) ;

  // fill in the pixel format descriptor
  PIXELFORMATDESCRIPTOR pfd ;
  memset(&pfd,0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
  pfd.nVersion = 1 ;
  pfd.dwFlags =  PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI | PFD_DRAW_TO_BITMAP;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = (BYTE)bmInfo.bmBitsPixel ;
  pfd.cDepthBits = 32 ;
  pfd.iLayerType = PFD_MAIN_PLANE ;

  // set the pixel format
  SetPixelFormat( m_dib.GetHDC(), ChoosePixelFormat( m_dib.GetHDC(), &pfd ), &pfd );

  // create and set the rendering context
  SetRenderingContext( wglCreateContext( m_dib.GetHDC() ) );

  dc.Detach();

  VERIFY( wglMakeCurrent( m_dib.GetHDC(), GetRenderingContext() ) );
  GetNormalFont().Create( m_dib.GetHDC(), -15, 0, "arial", FW_SEMIBOLD, false, false, true );
  GetLargeFont().Create( m_dib.GetHDC(), -20, 0, "arial", FW_SEMIBOLD, false, false, true );
  VERIFY( wglMakeCurrent( NULL, NULL ) );


  return true;
} // GLCreate()


/**
 * Retrieves dib pixel data
 *
 * @param iWidth               width of returned pixel data
 * @param iHeight              width of returned pixel data
 * @param ePixelFormat         not used
 * @param eDatatype            not used
 * @param bOnlyGLRenderScene   not used
 * @return                     Pointer to temporary update buffer data.
 */
const void  * DrawOverView::GetUpdateBuffer( int4 & iWidth, int4 & iHeight, int4 & iWidthAlignedDWORD, const GLenum ePixelFormat,
                                              const GLenum eDatatype, const bool bOnlyGLRenderScene )
{
  iWidth = m_dib.GetWidth();
  iHeight = m_dib.GetHeight();
  iWidthAlignedDWORD = ( (iWidth * 24 + 31) & (~31) ) / 8;

  InvalidateView();

  bool bGovern( GetGovernFrameRateWhileFlying() );
  SetGovernFrameRateWhileFlying( false );
  GLRender();
  SetGovernFrameRateWhileFlying( bGovern );
  
  return m_dib.GetPixelData();
} // GetUpdateBuffer()


// ====================================================================================================================


/**
 * Constructor
 *
 * @param     pWnd          Parent window
 * @param     studyref      Dataset to render
 * @param     viewrect      view rectangle
 */
DrawSegmentView::DrawSegmentView( CWnd * pWnd, const Dataset& studyref, const Rect<int4>& viewRect )
: Segment3DChildView(studyref.GetOrientation(), pWnd->GetSafeHwnd(), 1000 )
{
  // turn off optimization
  SetRenderOnlyBackground( false );
  
  // allocate the dib
  m_dib.Create( viewRect.Width(), viewRect.Height() );

  // create the rendering context
  GLCreate();

  wglMakeCurrent( GetHDC(), GetRenderingContext() );

  // set the viewport
  SetViewPort( viewRect.Width(), viewRect.Height() );

} // DrawSegmentView


/**
 * Destructor
 */
DrawSegmentView::~DrawSegmentView()
{
} // ~DrawSegmentView()


/**
 * Called on view window resize
 *
 * @param iWidth    Width of child view
 * @param iHeight   Height of child view
 */
void DrawSegmentView::GLResize(GLsizei iWidth, GLsizei iHeight)
{
  m_3DsegmentViewer.ResizeDisplay(iWidth, iHeight);
} // GLResize()


/**
 * Creates the rendering context
 *
 * @return  true on success 
 */
bool DrawSegmentView::GLCreate()
{
  // attach to the dib's device conext
  CDC dc;
  dc.Attach( m_dib.GetHDC() );
 
  // get the info for the associated bitmap
  CBitmap * pBitmap = dc.GetCurrentBitmap();
  BITMAP bmInfo;
  pBitmap->GetObject( sizeof( BITMAP ), &bmInfo ) ;

  // fill in the pixel format descriptor
  PIXELFORMATDESCRIPTOR pfd ;
  memset(&pfd,0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
  pfd.nVersion = 1 ;
  pfd.dwFlags =  PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI | PFD_DRAW_TO_BITMAP;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = (BYTE)bmInfo.bmBitsPixel ;
  pfd.cDepthBits = 32 ;
  pfd.iLayerType = PFD_MAIN_PLANE ;

  // set the pixel format
  SetPixelFormat( m_dib.GetHDC(), ChoosePixelFormat( m_dib.GetHDC(), &pfd ), &pfd );

  // create and set the rendering context
  SetRenderingContext( wglCreateContext( m_dib.GetHDC() ) );

  dc.Detach();

  VERIFY( wglMakeCurrent( m_dib.GetHDC(), GetRenderingContext() ) );
  GetNormalFont().Create( m_dib.GetHDC(), -15, 0, "arial", FW_SEMIBOLD, false, false, true );
  GetLargeFont().Create( m_dib.GetHDC(), -20, 0, "arial", FW_SEMIBOLD, false, false, true );
  VERIFY( wglMakeCurrent( NULL, NULL ) );


  return true;
} // GLCreate()


/**
 * Retrieves dib pixel data
 *
 * @param iWidth               width of returned pixel data
 * @param iHeight              width of returned pixel data
 * @param ePixelFormat         not used
 * @param eDatatype            not used
 * @param bOnlyGLRenderScene   not used
 * @return                     Pointer to temporary update buffer data.
 */
const void  * DrawSegmentView::GetUpdateBuffer( int4 & iWidth, int4 & iHeight, int4 & iWidthAlignedDWORD, const GLenum ePixelFormat,
                                              const GLenum eDatatype, const bool bOnlyGLRenderScene )
{
  iWidth = m_dib.GetWidth();
  iHeight = m_dib.GetHeight();
  iWidthAlignedDWORD = ( (iWidth * 24 + 31) & (~31) ) / 8;

  InvalidateView();

  GLRender();
  
  return m_dib.GetPixelData();
} // GetUpdateBuffer()



// $Log: DrawOverView.C,v $
// Revision 1.7.2.3  2010/03/10 00:42:00  dongqing
// localized version of segments 3d view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.2.2  2010/03/09 20:20:19  dongqing
// turn distance display off for all overview snapshots
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.2.1  2007/09/20 17:07:14  jmeade
// unicode char set for all fonts.
//
// Revision 1.7  2007/03/01 21:40:08  geconomos
// code review preparation
//
// Revision 1.6  2006/07/26 18:48:50  jmeade
// Issue 4867: wglMakeCurrent() calls where appropriate.
//
// Revision 1.5  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.4  2005/11/21 23:16:18  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.3.2.2  2006/02/22 20:11:46  geconomos
// Issue #4604:  Overview snapshot not working while flying
//
// Revision 1.3.2.1  2005/11/17 16:30:42  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.3  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.2.2.1  2005/09/21 13:28:47  geconomos
// modified DrawImage to implicitly use an image size of 512
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.4  2005/07/20 15:53:47  geconomos
// Issue# 4286: Snapshot crash with DrawOverImage.
//
// Revision 1.1.2.3  2005/07/18 18:35:00  geconomos
// Issue #4286: turned of the render only background optimization
//
// Revision 1.1.2.2  2005/06/23 18:50:50  geconomos
// Issue #4189: Return dib height instead of viewport height
//
// Revision 1.1.2.1  2005/06/21 15:12:53  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/DrawOverView.C,v 1.7.2.3 2010/03/10 00:42:00 dongqing Exp $
// $Id: DrawOverView.C,v 1.7.2.3 2010/03/10 00:42:00 dongqing Exp $
