// $Id: vxRendererSliceShadows3d.h,v 1.9 2007/03/02 21:19:53 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.net)


// pragma declarations
#pragma once


// include declarations
#include "vxRendererSliceShadows.h"


// forward declarations
class vxEnvironment;


/**
 * Renders the slice shadows in 3d for the following: orthogonal planes, oblique planes, curved planes,
 * and endoluminal vessels views.
 */
class VX_VIEWER_DLL vxRendererSliceShadows3d : public vxRendererSliceShadows
{
// member functions
public:

  /// constructor from specified environment
  vxRendererSliceShadows3d( vxEnvironment & environment );

  /// initializetion
  virtual void Initialize();
  
  /// uninitialize
  virtual void Uninitialize();

  /// renders the slice shadows
  virtual void Render();

private:

  /// render the projection of a curved plane 
  void RenderCurvedPlane( const vxEnvironment & otherEnvironment );

  /// Renders the current vessel section index as an arrow
  void RenderCurvedCamera( const vxEnvironment & otherEnvironment );

  /// render the projection of a plane with shading
  void RenderPlaneWithShading( const vxEnvironment & otherEnvironment, bool bOutlineOnly );

  /// render the projection of a plane with slice image
  void RenderPlaneWithSliceImage( const vxEnvironment & otherEnvironment );

  /// renders the endoluminal camera position and direction 
  void RenderVesselDetailEndoluminal( const vxEnvironment & otherEnvironment );

  /// gets the vertices and texture coordinates required to draw the active ortho slice as a texture
  void GetVerticesAndTexelsOrtho( const vxEnvironment & otherEnvironment, Point3Df vertices[], Point3Df textCoords[] );

  /// gets the vertices and texture coordinates required to draw the active oblique slice as a texture
  void GetVerticesAndTexelsOblique( const vxEnvironment & otherEnvironment, Point3Df vertices[], Point3Df textCoords[] );

  /// transform opengl for the detail volume
  void TransformDetail() const;

private:

  /// last active environment
  vxEnvironment * m_pLastActiveEnvironment;

   /// opengl texture id use for orhot and oblique planes with slice image
  GLuint m_textureId;

  /// direction flag used for endoluminal vessel shadows
  bool m_bDraggingdEndoCameraForward;

  /// previous vessel section index used for endoluminal vessel shadows
  float4 m_fPreviousVesselSectionIndex;

}; // class vxRendererSliceShadows3d


// $Log: vxRendererSliceShadows3d.h,v $
// Revision 1.9  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.8  2007/01/24 16:54:01  geconomos
// adjusted code for drawing slice shadows at volume boundaries
//
// Revision 1.7  2007/01/18 15:45:54  geconomos
// added RenderPlaneWithSliceImage()
//
// Revision 1.6  2006/11/13 20:59:06  geconomos
// Issue 5082: vessel postion not displaying for curved views
//
// Revision 1.5  2006/10/23 01:39:18  geconomos
// initial attempt at curved slice shadow
//
// Revision 1.4  2006/10/20 20:49:29  geconomos
// using SliceShadowMode from the rendering mode
//
// Revision 1.3  2006/10/05 16:59:44  geconomos
// redesgined and implemented
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererSliceShadows3d.h,v 1.9 2007/03/02 21:19:53 geconomos Exp $
// $Id: vxRendererSliceShadows3d.h,v 1.9 2007/03/02 21:19:53 geconomos Exp $
