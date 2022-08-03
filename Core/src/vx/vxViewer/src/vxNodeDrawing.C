// $Id: vxNodeDrawing.C,v 1.4 2007/10/29 20:34:27 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxNodeDrawing.h"
#include "vxViewer.h"
#include "vxEnvironment.h"
#include "vxVisualizationGraph.h"
#include "vxViewerControl.h"
#include "vxIRenderingContext.h"
#include "vxRenderingContextPBuffer.h"


#define FILE_REVISION "$Revision: 1.4 $"


/**
 * constructor
 */
vxNodeDrawing::vxNodeDrawing() 
{
  m_dib.Create( 2048,2048 );
}


/**
 * destructor
 */
vxNodeDrawing::~vxNodeDrawing() 
{
}


/**
 * Draws
 */
void vxNodeDrawing::Run()
{
  for( int4 i = 0; i < GetVisualizationGraph()->GetViewerControls().size(); ++i )
  {
    vxViewerControl & viewer = *GetVisualizationGraph()->GetViewerControl( i );
    vxEnvironment & environment = viewer.GetEnvironment();

    if( !environment.IsModified( vxEnvironment::ALL ) && !environment.GetRenderBuffer().IsModified() )
      continue;

    Redraw( viewer );
  }
} // Run()


/**
 * Redraws
 *
 * @param       bForce          true forces a redraw
 */
void vxNodeDrawing::Redraw( vxViewerControl & viewer )
{
  vxEnvironment & environment = viewer.GetEnvironment();
  environment.GetContext()->MakeCurrent();

  try
  {
    // determine if we need to rerender
    bool bRenderingNecessary( false );
    for( vxViewerControl::Viewers::reverse_iterator iter = viewer.GetViewerStructs().rbegin(); iter != viewer.GetViewerStructs().rend(); ++iter )
    {
      if ((*iter).second->m_pViewer->IsEnabled() == true)
        bRenderingNecessary |= iter->second->m_pViewer->IsRedrawNecessary();
    }

    if( bRenderingNecessary == false )
      return;

    // since we need to re-render, do so
    environment.GetContext()->MakeCurrent();
    
    // save the active viewer type, rendering will mess it up
    const vxEnvironment * pActiveEnvironment = environment.GetActiveEnvironment();
    const vxViewerType * pMainViewerType = viewer.GetViewer( vxViewerControl::MainViewer )->GetViewerType();

    // render all viewers in reverse z order
    //////////////////////////////////////////
    for( vxViewerControl::Viewers::reverse_iterator iter = viewer.GetViewerStructs().rbegin(); iter != viewer.GetViewerStructs().rend(); ++iter )
    {
      if ( (*iter).second->m_pViewer->IsEnabled() )
      {
        // setup the view
        (*iter).second->m_pViewer->SetupView();

        ClearGlBuffer((*iter).second->m_pViewer);

        if ((*iter).second->m_pViewer->PreRender() < 0) 
          continue;

        (*iter).second->m_pViewer->Render();

        if ((*iter).second->m_pViewer->PostRender() <= 0) 
          continue;
      }
    }

    if( viewer.HasBorder() )
      DrawBorder( viewer );

    glFlush();

    environment.SetModified( vxEnvironment::ALL, false );

    // restore the previous active viewer type
    environment.SetActiveEnvironment( pActiveEnvironment );
    environment.SetViewerType( const_cast< vxViewerType * >( pMainViewerType ) );

    if( GetVisualizationGraph()->GetRenderingContextMode() == vxIRenderingContext::WINDOWED )
    {
      environment.GetContext()->SwapBuffers();
    }
    else if( GetVisualizationGraph()->GetRenderingContextMode() == vxIRenderingContext::SHARED )
    {
      HWND hWnd = dynamic_cast< vxWindowHandle * >( environment.GetElement( vxIDs::WindowHandle ) )->GetHandle();
      HDC hdc = GetDC( hWnd );
    
      vxRenderingContextPBuffer & context = * dynamic_cast< vxRenderingContextPBuffer * >( environment.GetContext() );

      context.MakeCurrent();

      glReadPixels( 0, 0, 2048, 2048, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_dib.GetPixelData() );

      const std::pair< uint2, uint2 > & size = environment.GetWinRect().GetWindowSize();
      
      
      PAINTSTRUCT ps;
      BeginPaint( hWnd, &ps );
      BitBlt( hdc, 0, 0, size.first, size.second, m_dib.GetHDC(), 0, m_dib.GetHeight() - size.second, SRCCOPY );
      EndPaint( hWnd, &ps );

      ReleaseDC( hWnd, hdc );
    }
    else if( GetVisualizationGraph()->GetRenderingContextMode() == vxIRenderingContext::DIB )
    {
      HWND hWnd = dynamic_cast< vxWindowHandle * >( environment.GetElement( vxIDs::WindowHandle ) )->GetHandle();
      HDC hdc = GetDC( hWnd );
      
    
      vxRenderingContextDIB & context = * dynamic_cast< vxRenderingContextDIB * >( environment.GetContext() );

      context.MakeCurrent();

      //glReadPixels( 0, 0, 2048, 2048, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_dib.GetPixelData() );

      const std::pair< uint2, uint2 > & size = environment.GetWinRect().GetWindowSize();
      
      
      PAINTSTRUCT ps;
      BeginPaint( hWnd, &ps );
      BitBlt( hdc, 0, 0, size.first, size.second, context.GetDib().GetHDC(), 0, context.GetDib().GetHeight() - size.second, SRCCOPY );
      EndPaint( hWnd, &ps );

      ReleaseDC( hWnd, hdc );
    }
  }
  catch (ex::VException &)
  {
    LogErr("Redraw failed.", "vxNodeDrawing", "Run");
  }
} // Run()


/**
 * Clears the drawing area of the framebuffer.
 * @param pViewer viewer of buffer to clear
 */
void vxNodeDrawing::ClearGlBuffer( vxViewer * pViewer )
{
  vxWinRect rect(pViewer->GetWinRect());
  bool bNotFullScreen(rect.GetWindowSize().first > rect.GetViewportSize().first || rect.GetWindowSize().second > rect.GetViewportSize().second);

  if (bNotFullScreen == true)
  {
    glEnable(GL_SCISSOR_TEST);
    glScissor(rect.GetReferencePoint().first,
              rect.GetReferencePoint().second,
              rect.GetViewportSize().first,
              rect.GetViewportSize().second);
  }
 
  glClearColor(pViewer->GetEnvironment().GetBackgroundColor()->R()/255.0,
    pViewer->GetEnvironment().GetBackgroundColor()->G()/255.0,
    pViewer->GetEnvironment().GetBackgroundColor()->B()/255.0,
    pViewer->GetEnvironment().GetBackgroundColor()->A()/255.0);
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (bNotFullScreen == true)
  {
    glDisable(GL_SCISSOR_TEST);
  }

} // ClearGlBuffer()


/**
 * Draws a border around the window.
 *
 * @param       viewer          vxViewerControl
 */
void vxNodeDrawing::DrawBorder( vxViewerControl & viewer )
{
  // save OpenGl settings.
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  glDisable(GL_BLEND);

  RECT clientRect;
  GetClientRect( viewer.GetWindowHandle().GetHandle(), &clientRect);
  uint2 uWidth(clientRect.right - clientRect.left);
  uint2 uHeight(clientRect.bottom - clientRect.top);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, uWidth, 0, uHeight);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glViewport( 0, 0, uWidth, uHeight);

  glColor3ub( viewer.GetBorderColor().R(), viewer.GetBorderColor().G(), viewer.GetBorderColor().B() );

  glLineWidth( viewer.GetBorderWidth() );
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  glBegin(GL_LINE_LOOP);
  glVertex2i(0, 0);
  glVertex2i(uWidth, 0);
  glVertex2i(uWidth, uHeight);
  glVertex2i(0, uHeight);
  glEnd();

  // restore OpenGL settings
  glPopAttrib();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  //glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
} // DrawBorder()



// $Log: vxNodeDrawing.C,v $
// Revision 1.4  2007/10/29 20:34:27  gdavidson
// Added a Redraw method
//
// Revision 1.3  2007/10/26 14:04:58  gdavidson
// Do not draw unless the environment has been modified
//
// Revision 1.2  2007/10/24 20:13:42  gdavidson
// Moved rendering (drawing) into node
//
// Revision 1.1  2007/05/03 21:42:49  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxNodeDrawing.C,v 1.4 2007/10/29 20:34:27 gdavidson Exp $
// $Id: vxNodeDrawing.C,v 1.4 2007/10/29 20:34:27 gdavidson Exp $
