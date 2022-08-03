// $Id: GLUpdateBuffer.h,v 1.2 2005/08/16 21:45:06 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.net


// pragmas
#pragma once


// includes
#include "typedef.h"
#include "Rect.h"


namespace OpenGLUtils
{

  /**
  * Child view update buffer
  */
  class GLUpdateBuffer
  {
  // member functions
  public:

    /// Constructor
    GLUpdateBuffer();

    /// Destructor
    ~GLUpdateBuffer();

    /// Allocates a GLUpdateBuffer
    bool Allocate(const GLenum eDatatype, const Point2D<uint4>& size, const uint4 uBitsPerPixel);

    /// Clears the buffer
    void Reset();

  // data members
  public:

    /// The update buffer bits
    void far * m_pBuffer;

    /// Size of the buffer
    Point2D<uint4> m_size;

    /// Buffer's bits per pixel size
    uint4 m_uBitsPerPixel;

  }; // class GLUpdateBuffer

} // namespace OpenGLUtils

// $Log: GLUpdateBuffer.h,v $
// Revision 1.2  2005/08/16 21:45:06  jmeade
// namespaces for OpenGLUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.2  2005/04/13 19:24:16  frank
// cleaned up
//
// Revision 1.1.2.1  2005/04/13 18:50:19  frank
// split out GL update buffer class
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GLUpdateBuffer.h,v 1.2 2005/08/16 21:45:06 jmeade Exp $
// $Id: GLUpdateBuffer.h,v 1.2 2005/08/16 21:45:06 jmeade Exp $
