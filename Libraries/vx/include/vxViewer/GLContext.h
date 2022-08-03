// $Id: GLContext.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos( geconomos@viatronix.com )

/* 
  The GLContext class encapsulates an OpenGL rendering context
*/

#ifndef GLContext_h
#define GLContext_h


// includes
#include "GlFormat.h"


// class definition
class VX_VIEWER_DLL GLContext 
{
// functions
public:
  
  /// constructor
  GLContext( const GLFormat & format, const GLPaintDevice * pDevice );
  
  /// destructor
  virtual ~GLContext();
  
  /// creates the GL context
  virtual bool Create( const GLContext * pShareContext = 0 );
  
  /// resets the context and makes it invalid. 
  virtual void Reset();
  
  /// makes this context the current OpenGL rendering context. 
  virtual void MakeCurrent();
  
  /// swaps the screen contents with an off-screen buffer. Works only if the context is in double buffer mode. 
  virtual void SwapBuffers() const;

  /// sets a format for this context. The context is reset. 
  virtual void SetFormat( const GLFormat & format );
    
  /// returns TRUE if a GL rendering context has been successfully created. 
  bool IsValid() const { return m_bValid; }
  
  /// returns TRUE if display list sharing with another context was requested in the create() call, 
  /// and the GL system was able to fulfill this request. 
  bool IsSharing() const { return m_bSharing; }
  
  /// returns TRUE if this context has been initialized
  bool IsInitialized() const { return m_bInitDone; }
  
  /// tells the context whether it has been initialized
  void SetInitialized( bool bInitialized ) { m_bInitDone = bInitialized; }  
  
  /// returns the OpenGL format for this context. 
  GLFormat GetFormat() const { return m_format; }

  /// returns the painting device for this context
  const GLPaintDevice * GetDevice() { return m_pDevice; }
  
  /// returns the current context, i.e. the context to which any OpenGL commands will currently be directed to. 
  static const GLContext*	GetCurrentContext() { return m_pCurrentContext; }

  /// Win32 specific OpenGL context
  const HGLRC	GetContext() const { return m_hRC; }

  /// Win32 specific OpenGL context
  const HDC GetWinDevice() const { return m_hdc; };

  /// makes no GL context the current context. 
  virtual void DoneCurrent();

protected:
  
  /// creates a system-dependent OpenGL handle that matches the specified format as closely as possible. 
  virtual bool ChooseContext( const GLContext* shareContext = 0 );
  
  /// creates a Win32 OpenGL handle that matches the specified format as closely as possible. 
  virtual int	ChoosePixelFormat( void* pfd, HDC hdc );

private:

  /// default constructor
  GLContext() {}

  /// copy constructor
  GLContext( const GLContext & other ) {}

  /// assignment operator
  GLContext& operator=( const GLContext & other ) { return *this; }

// member variables
private:

  /// Win32 specific OpenGL context
  HGLRC	m_hRC;
  
  /// Win32 specific device context associated with context
  HDC m_hdc;
  
  /// selected Win32 specific OpenGL pixel format identifier
  int4 m_iPixelFormatID;

  /// Win32 specific 
  HWND m_hWnd; 

  /// current format for this context
  GLFormat m_format;
  
  /// flag indicating if conext is valid
  bool m_bValid;
  
  /// flag indicating if conext is being shared
  bool m_bSharing;
  
  /// flag indicating that the context is fully initialized
  bool m_bInitDone;
  
  /// painting device associated with this context
  const GLPaintDevice * m_pDevice;
  
  /// current context
  static GLContext * m_pCurrentContext;
  
  /// friend classes
  friend class vxViewer;
  friend class GLFontGeorge;
}; // GLContext()


#endif // GLContext_h


// $Log: GLContext.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.9  2003/05/14 13:26:59  geconomos
// Coding standards.
//
// Revision 3.8  2003/05/08 16:39:05  michael
// more code reviews (75 renderer Utils)
//
// Revision 3.7  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 3.6  2002/03/11 19:01:37  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.5  2002/03/07 22:31:08  dmitry
// Modified to fit new architecture.
//
// Revision 3.4  2002/03/04 16:43:26  michael
// fomatting and coding guidelines
//
// Revision 3.3.2.1  2002/02/05 16:31:35  manju
// Issue 1690: Added method to get the window device context.
//
// Revision 3.3  2002/01/08 21:42:36  dmitry
// Method made public.
//
// Revision 3.2  2001/12/28 21:57:52  ingmar
// added Sec_EnvElements to structure doxygen documentation
//
// Revision 3.1  2001/12/18 17:16:46  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:31  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:30   ingmar
// Initial revision.
// Revision 1.4  2001/09/07 14:04:15  geconomos
// added get DeviceCOntext
//
// Revision 1.3  2001/09/07 13:38:19  geconomos
// Rename GLViewer=>Viewer renamed all OnGLXXX function to OnXXX
//
// Revision 1.2  2001/09/01 05:12:50  ingmar
// new directory structure
//
// Revision 1.1  2001/06/29 16:53:08  geconomos
// Moved from to 53_rendererDataStruct
//
// Revision 1.2  2001/06/22 18:22:07  geconomos
// Initial OpenGL implementation.
//
// Revision 1.1  2001/06/18 21:52:14  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/GLContext.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: GLContext.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
