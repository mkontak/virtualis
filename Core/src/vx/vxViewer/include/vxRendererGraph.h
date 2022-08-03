// $Id: vxRendererGraph.h,v 1.7 2006/11/16 18:43:18 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Romy Sreedharan (mailto:romy@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxRenderer.h"

class vxEnvironment;


// class declaration
class VX_VIEWER_DLL vxRendererGraph : public vxRenderer
{

// member functions
public:

  /// constructor
  vxRendererGraph( vxEnvironment & environment );

  /// destructor
  virtual ~vxRendererGraph();

  /// perform pre-rendering operations
  virtual void Initialize();
  
  /// uninitialize
  virtual void Uninitialize();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// perform pre-rendering operations
  virtual int4 PreRender();

  /// perform rendering operations
  virtual void Render();

	/// Draws the image on the screen
	void DrawImage( const int4 x, const int4 y, const int4 iWidth, const int4 iHeight, const uint1 * pData );

private:

  /// updates the texture from the render buffer's color image
  void UpdateTexture();

  /// draws the texture to the entire viewport
  void DrawTexture();


  /// private copy constructor
  vxRendererGraph( const vxRendererGraph & other );

  /// private assignment operator
  const vxRendererGraph & operator=( const vxRendererGraph & rhs );

 private:

 /// opengl texture id for blitting engine buffer
  GLuint m_textureId;

	///Stores the previous size of the viewport
	std::pair<uint2,uint2> m_previousSize;
  
}; // class vxRendererGraph


// $Log: vxRendererGraph.h,v $
// Revision 1.7  2006/11/16 18:43:18  romy
// using the Texture mapping instead of bitmap drawing
//
// Revision 1.6  2006/11/15 16:05:38  dongqing
// removed duplicate data
//
// Revision 1.5  2006/11/13 05:15:17  romy
// comments added
//
// Revision 1.4  2006/11/13 05:02:53  romy
// comments added
//
// Revision 1.3  2006/11/02 20:24:59  romy
// initial version
//
// Revision 1.2  2006/11/02 14:55:21  romy
// exporting added

// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererGraph.h,v 1.7 2006/11/16 18:43:18 romy Exp $
// $Id: vxRendererGraph.h,v 1.7 2006/11/16 18:43:18 romy Exp $
