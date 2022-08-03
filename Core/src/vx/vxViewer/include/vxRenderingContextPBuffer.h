// $Id: vxRenderingContextPBuffer.h,v 1.1 2007/10/24 20:33:48 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)


// pragma decalrations
#pragma once

// include declarations
#include "vxIRenderingContext.h"

// forward declarations
class vxRenderingContextGL;
class GLPaintDevice;

/**
 * Provides an opengl rendering context based on a p-buffer
 */
class vxRenderingContextPBuffer : public vxIRenderingContext
{
// member functions
public:
  
  /// constructor
  vxRenderingContextPBuffer();
  
  /// destructor
  virtual ~vxRenderingContextPBuffer();

  /// creates a the rendering context for the specified width and height
  virtual bool Create( const uint4 uWidth, const uint4 uHeight );

  /// makes the p-buffer context currrent
  virtual void MakeCurrent();

  /// swaps the screen contents with an off-screen buffer. Works only if the context is in double buffer mode.
  virtual void SwapBuffers() const { /* not implemented */ }

  /// gets the associated device context
  virtual const HDC GetHDC() const { return m_hDC; }

  /// indicates if the device context is valid
  virtual bool IsValid() const { return m_hRC != NULL; }

  /// destorys the p-buffer context 
  void Destroy();

  /// gets the p-buffer width
  const uint4 GetBufferWidth() const { return m_uWidth; }

  /// gets the p-buffer height
  const uint4 GetBufferHeight() const { return m_uHeight; }

private:

  /// initializes the required opengl extensions for using a p-buffer
  bool InitializeExtensions();

  /**
   * Hidden window containing hwnd-based rendering context needed to create p-buffer context
   */
  class HiddenWindow : public CWindowImpl< HiddenWindow >
  {
  // member functions
  public:

    // constructor
    HiddenWindow();

    /// gets the rendering context
    vxRenderingContextGL * GetContext() { return m_pContext; }
  
  protected:

    /// message map
    BEGIN_MSG_MAP(HiddenWindow)
      MESSAGE_HANDLER( WM_CREATE, OnCreate   )
      MESSAGE_HANDLER( WM_DESTROY,OnDestroy )
    END_MSG_MAP()

    /// WM_CREATE handler
    LRESULT OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

    /// WM_DESTROY handler
    LRESULT OnDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
  
  // member vairables
  private:

    /// paint device
    GLPaintDevice * m_pDevice;

    /// rendeirng context based on hWnd
    vxRenderingContextGL * m_pContext;
    
  }; // class HiddenWindow


// member variables
private:
  
  /// hwnd based rendering context window
  HiddenWindow m_hiddenWindow;

  /// handler to pbuffer
  HPBUFFERARB m_hPBuffer;
  
  /// associated device context
  HDC  m_hDC;
  
  /// associated rendering context
  HGLRC m_hRC;
  
  /// buffer width
  uint4 m_uWidth;
  
  /// buffer height
  uint4 m_uHeight;

}; // class vxRenderingContextPBuffer 


// $Log: vxRenderingContextPBuffer.h,v $
// Revision 1.1  2007/10/24 20:33:48  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderingContextPBuffer.h,v 1.1 2007/10/24 20:33:48 gdavidson Exp $
// $Id: vxRenderingContextPBuffer.h,v 1.1 2007/10/24 20:33:48 gdavidson Exp $
