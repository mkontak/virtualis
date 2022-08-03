// $Id: vxRenderingContextDIB.h,v 1.2 2004/04/12 13:53:30 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (george@viatronix.com)

#ifndef vxRenderingContextDIB_h
#define vxRenderingContextDIB_h


// includes
#include "vxDib.h"
#include "vxIRenderingContext.h"


// class declaration
class VX_VIEWER_DLL vxRenderingContextDIB : public vxIRenderingContext
{
// member functions
public:
  
  /// default consturctor
  vxRenderingContextDIB();
  
  /// virtual desturctor
  virtual ~vxRenderingContextDIB();
  
  /// creates a rendering context with the spcified width and height 
 	virtual bool Create( const uint4 uWidth, const uint4 uHeight );
 	
 	/// destroys the context rendering object
 	void Destroy();
  
  /// sets the context as active
  virtual void MakeCurrent();
  
  /// gets the associated dib
  const vxDib & GetDib() const { return m_dib; }

  /// swaps the screen contents with an off-screen buffer. Works only if the context is in double buffer mode.
  virtual void SwapBuffers() const {}

  /// Win32 specific device context
  virtual const HDC GetHDC() const { return m_dib.GetHDC(); }

  /// returns TRUE if a the rendering context has been successfully created
  virtual bool IsValid() const { return true; }

// member variables
private:

  // opengl rendering context
  HGLRC	m_hrc;
  
  // dib rendered into
  vxDib m_dib;
  
}; // class vxRenderingContextDIB

#endif // #ifndef vxRenderingContextDIB_h

// $Log: vxRenderingContextDIB.h,v $
// Revision 1.2  2004/04/12 13:53:30  geconomos
// no message
//
// Revision 1.1  2004/04/02 14:06:29  frank
// working on unifying the various rendering contexts
//
// Revision 1.1  2004/03/30 17:23:03  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderingContextDIB.h,v 1.2 2004/04/12 13:53:30 geconomos Exp $
// $Id: vxRenderingContextDIB.h,v 1.2 2004/04/12 13:53:30 geconomos Exp $
