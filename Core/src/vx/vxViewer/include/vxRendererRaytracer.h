// $Id: vxRendererRaytracer.h,v 1.8 2007/03/02 21:19:53 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxRenderer.h"
#include "Image.h"

/** 
 * Draws the color image and depth buffer from the environment's vxRenderBuffer.  The raytracer is assumed
 * to have updated the render buffer.
 */
class VX_VIEWER_DLL vxRendererRaytracer : public vxRenderer
{
// member functions
public:

  /// constructor
  vxRendererRaytracer( vxEnvironment & environment );

  /// destructor
  virtual ~vxRendererRaytracer();

  /// initilize
  virtual void Initialize();
  
  /// uninitialize
  virtual void Uninitialize();
 
  /// perform rendering operations
  virtual void Render();

private:

  /// updates the texture from the associated vxRenderBuffer
  void UpdateTexture();

  /// draws the image stored in the texture
  void DrawTexture();

  /// updates the cached depth buffer from the current render buffer's depth image
  void UpdateCachedDepthBuffer();

  /// draws the cached depth buffer into the entire viewport
  void DrawCachedDepthBuffer();
  

// member variables
private:

  /// opengl texture id 
  GLuint m_textureId;

  /// normalized depth buffer
  Image< float4 > m_depthBuffer;

  /// aspect ratio of color image when first updated (used for fast resizes)
  std::pair< uint2, uint2 > m_imageAspectRef;
  
}; // class vxRendererRaytracer


// $Log: vxRendererRaytracer.h,v $
// Revision 1.8  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.7  2006/11/10 03:20:20  geconomos
// Issue 5089: aspect ratio incorrect for 3d; no longer calling flush in parent window classes when parent is resized
//
// Revision 1.6  2006/10/20 20:53:03  geconomos
// added depth buffer support
//
// Revision 1.5  2006/08/03 14:14:47  geconomos
// removed all rendering functionailty; now just texture maps the render buffer contained in the associated environment
//
// Revision 1.4  2006/07/18 16:21:23  geconomos
// modified CreateTexture and DrawTexture to take the index of the buffer to display
//
// Revision 1.3  2006/07/07 13:43:18  geconomos
// still working on final image generation
//
// Revision 1.2  2006/06/27 20:16:48  geconomos
// initial threading work
//
// Revision 1.1  2006/06/21 18:46:32  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererRaytracer.h,v 1.8 2007/03/02 21:19:53 geconomos Exp $
// $Id: vxRendererRaytracer.h,v 1.8 2007/03/02 21:19:53 geconomos Exp $
