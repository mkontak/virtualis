// $Id: vxRendererTexMap2DMPRNew.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)

/**
 * Renderer that does not require a texture server but
 * generates the textures on the fly. In addition, uses
 * simple slice drawing without requiring texture shaders
 * and/or register combiners
 */

#ifndef vxRendererTexMap2DMPRNew_h
#define vxRendererTexMap2DMPRNew_h


// includes
#include "vxRendererTexMap.h"
#include "vxTexMap2dTile.h"


// class definition
class VX_VIEWER_DLL vxRendererTexMap2DMPRNew : public vxRendererTexMap
{
// functions
public:

  /// constructor
	vxRendererTexMap2DMPRNew(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererTexMap2DMPRNew() {}

  /// initialize
  void Initialize();

  /// uninitialize
  void Uninitialize();

  /// preRender sets up textures for TF and components
  virtual int4 PreRender();
  
  /// Render the volume/all tiles.
  virtual void Render();

protected:

  /// setup texture shaders and register combiners
  void SetupTileTexture(const GLuint & Id0, const GLuint & Id1, const GLuint & Id2, const GLuint & Id3);

  /// update the transfer function texture.
  void UpdateTransferFunctionTexture();

  /// Draw textured tile using openGL calls.
  void GLDrawTexturedTile(const vxTexMap2dTile * pTile, const uint1 & uIndex);

  /// Draw untextured tile using openGL calls.
  void GLDrawUntexturedTile(const vxTexMap2dTile * pTile, const uint1 & uIndex);

  /// Setup texture for intensity range.
  void SetupTileTextureIntensityRange(const vxTexMap2dTile * pTile);

private:

  /// copy constructor
  vxRendererTexMap2DMPRNew(vxRendererTexMap2DMPRNew & other);  // should only be public if really implemented!

  /// assignment operator
  vxRendererTexMap2DMPRNew & operator=(vxRendererTexMap2DMPRNew & other);  // should only be public if really implemented!

  /// render a single tile
  void RenderTile(const vxTexMap2dTile * pTile, const uint1 & uIndex, GLuint id0, GLuint id1, bool bTexOn = 1);

  /// create the texture for the slice
  void CopyIntensityTextureToBuffer();
  
  /// create the texture for the slice
  void CopyLabelTextureToBuffer();

  /// generate the tile parameters for rendering
  void GenerateTileInfo();
  
  /// generate intensity and label volume texture for the current slice
  void GenerateIntensityTexture();
  
  /// generate intensity and label volume texture for the current slice
  void GenerateLabelTexture();

  /// get Intensity texture ID
  GLuint GetIntensityTextureID() { return m_uIntensityTextureID; };

  /// get Label texture ID
  GLuint GetLabelTextureID() { return m_uLabelTextureID; };

  /// set Intensity texture ID
  void SetIntensityTextureID(GLuint Id) { m_uIntensityTextureID = Id; };

  /// set Label texture ID
  void SetLabelTextureID(GLuint Id) { m_uLabelTextureID = Id; };

  /// get texture ID
  GLuint GetTFtextureIDalphaOne() { return m_TFtextureIDalphaOne; };

  /// get texture ID
  void SetTFtextureIDalphaOne(GLuint Id) { m_TFtextureIDalphaOne = Id; };

// data
private:

  /// intenstity texture ID
  GLuint m_uIntensityTextureID;

  /// label texture ID
  GLuint m_uLabelTextureID;

  /// colormap texture with alpha being set to 1 (255) --> no blend
  GLuint m_TFtextureIDalphaOne;

  /// texture data point
  GLubyte * m_piTFTextureDataAlphaOne;

  /// tile of texture
  vxTexMap2dTile m_Tile;
 
  // texture buffers in main memory for intensity
  GLubyte * m_pTextureIntensityBuffer;
  
  // texture buffers in main memory for labels
  GLubyte * m_pTextureLabelBuffer;
}; // vxRendererTexMap2DMPRNew


#endif // vxRendererTexMap2DMPRNew_h


// Revision History:
// $Log: vxRendererTexMap2DMPRNew.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2004/01/05 19:03:03  michael
// added "mailto:"
//
// Revision 1.11  2003/05/19 14:46:49  frank
// Removed unused return parameter
//
// Revision 1.10  2003/05/16 15:34:08  dongqing
// code review
//
// Revision 1.9  2003/05/16 13:58:18  michael
// code review
//
// Revision 1.8  2003/05/16 13:22:15  frank
// formatting
//
// Revision 1.7  2002/11/10 23:05:38  michael
// This renderer now directly inherits from vxRendererTexMap and does not
// require any texture server
//
// Revision 1.6  2002/11/06 23:29:04  michael
// added author and comment
//
// Revision 1.5  2002/10/17 22:16:48  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.4  2002/07/30 15:07:51  uday
// Renamed functions.
//
// Revision 1.3  2002/07/29 17:03:16  uday
// Seperated Intensity and label texture generations.
//
// Revision 1.2  2002/07/26 18:30:46  michael
// Fixed new texture map based renderers
//
// Revision 1.1  2002/07/26 17:09:21  uday
// Initial Version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererTexMap2DMPRNew.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxRendererTexMap2DMPRNew.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
