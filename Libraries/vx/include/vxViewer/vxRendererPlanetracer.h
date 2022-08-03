// $Id: vxRendererPlanetracer.h,v 1.5 2007/03/07 21:08:45 romy Exp $
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
#include "vxTexMap2dTile.h"
#include "vxEnvironment.h"
//#include "vxRenderBuffer.h"


/** 
 * Draws the color image from the environment's vxRenderBuffer.  The planetracer is assumed
 * to have updated the render buffer.
 */
class VX_VIEWER_DLL vxRendererPlanetracer : public vxRenderer
{
// member functions
public:
  
  /// constructor
  vxRendererPlanetracer(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererPlanetracer();

  /// perform pre-rendering operations
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

// member variables
private:

  /// indicates if a texture has been created yet
  bool m_bTextureCreated;

  /// texture tile containing teximage and texture coordinates
  vxTexMap2dTile m_textureTile;
};


// $Log: vxRendererPlanetracer.h,v $
// Revision 1.5  2007/03/07 21:08:45  romy
// removed unnecessary include files.
//
// Revision 1.4  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.3  2006/12/04 20:33:52  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.2  2006/08/03 14:14:47  geconomos
// removed all rendering functionailty; now just texture maps the render buffer contained in the associated environment
//
// Revision 1.1  2006/07/21 14:59:15  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererPlanetracer.h,v 1.5 2007/03/07 21:08:45 romy Exp $
// $Id: vxRendererPlanetracer.h,v 1.5 2007/03/07 21:08:45 romy Exp $
