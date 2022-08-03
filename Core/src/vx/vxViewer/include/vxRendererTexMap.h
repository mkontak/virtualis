// $Id: vxRendererTexMap.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/**
 * Base class for Texture Mapping Renderers
 */

#ifndef vxRendererTexMap_h
#define vxRendererTexMap_h


// includes
#include "vxRenderer.h"
#include "Thread.h"
#include "Rect.h"


// forward declarations
template <class T> class vxPlaneOrtho;


// class definition
class  VX_VIEWER_DLL vxRendererTexMap : public vxRenderer
{
// enums
public:

  enum RENDER_MODE
  {
    DENSITY_TAG_POST_CLASSIFY = 0,
    DENSITY_PRE_INTEGRATED    = 1,
  }; // RENDER_MODE

// functions
public:

  /// constructor
  vxRendererTexMap(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererTexMap();

  /// initialize the renderer
  virtual void Initialize();

  /// uninitialize the renderer
  virtual void Uninitialize();

  /// preRender sets up textures for TF and components
  virtual int4 PreRender();

  /// Render the volume/all tiles.
  virtual void Render() = 0;

  /// perform pre-rendering operations
  virtual int4 PostRender();

  /// returns the current rendering mode
  RENDER_MODE GetRenderMode() const { return m_eRenderMode; }

  /// set the current rendering mode
  void SetRenderMode(RENDER_MODE eRenderMode) { m_eRenderMode = eRenderMode; }

  /// returns the distance between slices
  virtual float8 GetSliceDistance() const { return m_fPlaneDistance; };

  /// set the distance between slices
  void SetSliceDistance(const float8 & fDistance) { m_fPlaneDistance = fDistance; }

  /// enable tag rendering
  void EnableTags(const bool bSet) { m_bTagsEnabled = bSet; }

  /// get TF texture ID
  GLuint GetTFTextureID() const { return m_iTFTextureID; }

  /// set TF texture ID
  void SetTFTextureID(GLuint id) { m_iTFTextureID = id; };

  /// get TF texture ID
  GLuint GetTFPreIntegratedTextureID() const { return m_iTFPreIntegratedTextureID; }

  /// set TF texture ID
  void SetTFPreIntegratedTextureID(GLuint id) { m_iTFPreIntegratedTextureID = id; }

  /// get component texture ID
  GLuint GetComponentTextureID() const { return m_iComponentTextureID; }

  /// set component texture ID
  void SetComponentTextureID(GLuint id) { m_iComponentTextureID = id; }

  /// get intensity range texture ID
  GLuint GetIntensityRangeTextureID() const { return m_iIntensityRangeTextureID; }

  /// set intensity range texture ID
  void SetIntensityRangeTextureID(GLuint id) { m_iIntensityRangeTextureID = id; };

  /// get dummy texture ID
  GLuint GetDummyTextureID() const { return m_iDummyTextureID; }

  /// set Dummy Texture ID
  void SetDummyTextureID(GLuint id) { m_iDummyTextureID = id; };

  /// get dummy texture ID
  GLuint GetDummyTextureCombID() const { return m_iDummyTextureCombID; }

  /// set Dummy Texture ID
  void SetDummyTextureCombID(GLuint id) { m_iDummyTextureCombID = id; };

  /// draw a textured polygon
  void GLDrawTexPolygon(const uint1 & uNumberOfPoints, Point3D<float8> * pPoints, Point3D<float8> * pTexCoords, Point3D<float8> * pTexCoordsNext);

  /// draw a textured polygon with tag option
  void GLDrawTexPolygonTags(uint1 & uNumberOfPoints, Point3D<float8> * pPoints, Point3D<float8> * pTexCoords, bool bTags = false);

  /// get texture combiner enabled flag
  bool GetTexCombEnabledFlag() const { return m_bTexCombEnabled; }

protected:

  /// setup GL texture combiners
  void SetupGLTextureCombiners();

  /// disable GL texture combiners
  void DisableGLTextureCombiners();

  /// update the transfer function texture
  void UpdateTransferFunctionTexture();

  /// draw information that texture generation is on its way ...
  void RenderBusyText();

  /// sets flag whether rendering is necesary
  void SetRenderingNecessary(bool bFlag) { m_bRenderingNecessary = bFlag; };

  /// returns state for rendering
  bool GetRenderingNecessary() { return m_bRenderingNecessary; };

private:

  /// copy constructor
  vxRendererTexMap(const vxRendererTexMap & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererTexMap & operator=(const vxRendererTexMap & other); // should only be public if really implemented!

  /// setup dummy texture
  void SetupDummyTexture();

  /// get TF texture data pointer
  GLubyte * GetTFTextureDataPtr() const { return m_piTFTextureData; }

  /// get TF pre-integrated texture data pointer
  GLubyte * GetTFPreIntegratedTextureDataPtr() const { return m_piTFPreIntegratedTextureData; }

  /// get Component texture data pointer
  GLubyte * GetComponentTextureDataPtr() const { return m_piComponentTextureData; }

  /// update the component texture(label color)
  void UpdateComponentTexture();

  /// update the intensity range texture(fixed color and opacity)
  void UpdateIntensityRangeTexture();

  /// pre-integrate the transfer function
  void PreIntegrateTF(GLubyte * pData);

  /// write out preintegrated tables
  void WritePreintegratedTables(GLubyte * pData);

  /// generate OpenGL list for 3D cursor
  GLuint GenCursor3dGLlist(vxPlaneOrtho<float8> & rCursor3dPlane, uint2 uIndex);

// data
private:

  /// rendering mode
  RENDER_MODE m_eRenderMode;

  /// transfer function texture ID
  GLuint m_iTFTextureID;

  /// pre-integrated transfer function texture 2D
  GLuint m_iTFPreIntegratedTextureID;

  /// transfer function texture data
  GLubyte * m_piTFTextureData;

  /// pre-integrated transfer function texture data
  GLubyte * m_piTFPreIntegratedTextureData;

  /// component texture ID
  GLuint m_iComponentTextureID;

  /// component texture data
  GLubyte * m_piComponentTextureData;  

  /// intensity range texture ID
  GLuint m_iIntensityRangeTextureID;

  /// intensity range texture data
  GLubyte * m_piIntensityRangeTextureData;

  /// dummy texture ID
  GLuint m_iDummyTextureID;

  /// dummy texture data
  GLubyte * m_piDummyTextureData;

  /// dummy combiner texture ID
  GLuint m_iDummyTextureCombID;

  /// dummy combiner texture data
  GLubyte * m_piDummyTextureCombData;

  /// texture combiners enabled
  bool m_bTexCombEnabled;

  /// are tags enabled
  bool m_bTagsEnabled;

  /// plane distance
  float8 m_fPlaneDistance;

  /// OpenGL id of the rendering object, 3D cursor plane
  GLuint m_cursorPlaneId[2];

  /// flag if rendering is necessary
  bool m_bRenderingNecessary;
}; // vxRendererTexMap


#endif // vxRendererTexMap_h


// Revision History:
// $Log: vxRendererTexMap.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.24  2003/05/19 14:46:49  frank
// Removed unused return parameter
//
// Revision 1.23  2003/05/16 15:34:08  dongqing
// code review
//
// Revision 1.22  2003/05/16 13:58:18  michael
// code review
//
// Revision 1.21  2003/05/16 11:45:27  michael
// code review
//
// Revision 1.20  2003/05/15 20:10:41  frank
// Moving away from old point class
//
// Revision 1.19  2003/01/21 22:17:31  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.18  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.17  2002/11/10 23:05:59  michael
// removed redundant forward declarations
//
// Revision 1.16  2002/11/05 22:51:22  ingmar
// 3DCursor -> Cursor3d
//
// Revision 1.15  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.14  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.13  2002/05/16 04:27:30  michael
// TexMapServer returns NULL if tile or tile vector is not (yet) available. Adjusted
// texture map renderer to display information when texture generation is in progress.
//
// Revision 1.12  2002/05/13 20:16:33  michael
// Display busyText while texture generation
//
// Revision 1.11  2002/05/07 18:37:42  frank
// Removed unneccessary includes.
//
// Revision 1.10  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.9  2002/04/14 00:20:23  michael
// moved GetSliceDistanceFirst to TexMap3D renderer and made GetSliceDistance()
// a virtual function such that it can be overloaded. E.g., 2DTexMap has view
// dependent slice distance ...
//
// Revision 1.8  2002/04/06 00:17:38  michael
// fixed sample intensity range
//
// Revision 1.7  2002/04/03 21:15:35  michael
// cosmetics
//
// Revision 1.6  2002/03/29 22:31:02  jaddonisio
// Removed needless friend declarations.
//
// Revision 1.5  2002/03/22 15:57:57  jaddonisio
// New functions for Intensity Range texture, and a dummy texture combiner.
//
// Revision 1.4  2002/03/20 16:45:33  michael
// Added PostRender to clean up used gl stuff
//
// Revision 1.3  2002/03/18 23:24:41  jaddonisio
// New setup and disable functions for GL texture combiners.
//
// Revision 1.2  2002/03/14 22:47:25  jaddonisio
// i. cleaned-up dead code
// ii. Made RenderWithCutPlane() function.
//
// Revision 1.1  2002/03/12 23:23:22  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererTexMap.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxRendererTexMap.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
