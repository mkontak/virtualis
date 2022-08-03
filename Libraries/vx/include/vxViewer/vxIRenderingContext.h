// $Id: vxIRenderingContext.h,v 1.2 2007/10/24 20:11:49 gdavidson Exp $
//
// Copyright © 2004 , Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxIRenderingContext_h
#define vxIRenderingContext_h


// includes
#include "GlFormat.h"


/**
 * This is
 *
 */
class VX_VIEWER_DLL vxIRenderingContext
{
public:

  /// rendering context
  enum RenderingContextEnum
  {
    WINDOWED,
    SHARED,
    DIB,
  }; // enum RenderingContextEnum

// functions
public:

  /// creates a rendering context with the specified width and height
  virtual bool Create( const uint4 uWidth, const uint4 uHeight ) = 0;
  
  /// makes this context the current rendering context
  virtual void MakeCurrent() = 0;

  /// swaps the screen contents with an off-screen buffer. Works only if the context is in double buffer mode.
  virtual void SwapBuffers() const = 0;
  
  /// Win32 specific device context
  virtual const HDC GetHDC() const = 0;
  
  /// returns TRUE if a the rendering context has been successfully created
  virtual bool IsValid() const = 0;

  /// cleans up any resources
  virtual void Destroy() = 0;

}; // class vxIRenderingContext


#endif // vxIRenderingContext_h


// $Log: vxIRenderingContext.h,v $
// Revision 1.2  2007/10/24 20:11:49  gdavidson
// Added RenderingContextEnum
//
// Revision 1.1  2004/04/02 14:06:49  frank
// working on unifying the various rendering contexts
//
// $Id: vxIRenderingContext.h,v 1.2 2007/10/24 20:11:49 gdavidson Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxIRenderingContext.h,v 1.2 2007/10/24 20:11:49 gdavidson Exp $
