// $Id: vxRendererCurvetracer.h,v 1.2 2007/03/02 21:19:53 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// pragma declarations
#pragma once


// include declarations
#include "vxRenderer.h"


/** 
 * Draws the color image from the environment's vxRenderBuffer.  The curvetracer is assumed
 * to have updated the render buffer.
 */
class VX_VIEWER_DLL vxRendererCurvetracer : public vxRenderer
{
// member functions
public:

  /// constructor
  vxRendererCurvetracer( vxEnvironment & environment );

  /// destructor
  virtual ~vxRendererCurvetracer();
  
  /// perform pre-rendering operations
  virtual void Initialize();
  
  /// uninitialize
  virtual void Uninitialize();

  /// texture maps and draws the color image from the vxRenderBuffer in the environment
  virtual void Render();

private:

  /// updates the texture from the associated vxRenderBuffer
  void UpdateTexture();

  /// draws the image stored in the texture
  void DrawTexture();

// member variables
private:

  /// texture id
  GLuint m_textureId;

  /// indicates if the texture has already been created
  bool m_bTextureCreated;

}; // class vxRendererCurvetracer


// $Log: vxRendererCurvetracer.h,v $
// Revision 1.2  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.1  2006/09/14 20:09:17  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererCurvetracer.h,v 1.2 2007/03/02 21:19:53 geconomos Exp $
// $Id: vxRendererCurvetracer.h,v 1.2 2007/03/02 21:19:53 geconomos Exp $
