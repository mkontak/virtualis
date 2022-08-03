// $Id: vxTexMap2dTile.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Joseph Addonisio (jaddonisio@viatronix.com)

/**
 * Class provides definition, data, and methods for a tile(2D) of volume data (denstiy and label).
 */

#ifndef vxTexMap2dTile_h
#define vxTexMap2dTile_h



// include declarations
#include "Triple.h"
#include "Point.h"
#include "Vector.h"
#include "Normal.h"
#include <gl\gl.h>


// forward declarations
class vxServerTexMap2d;
class vxRendererSoftware;
class vxRendererSoftwareMPROrtho;
template <class T> class vxPlaneOrtho;
template <class T> class Box;



// class definition
class VX_VIEWER_DLL vxTexMap2dTile
{
// functions
public:

  /// constructor
  vxTexMap2dTile();

  /// copy constructor
  vxTexMap2dTile(const vxTexMap2dTile & other);

  /// destructor
  virtual ~vxTexMap2dTile() {};
  
  /// assignment operator
  vxTexMap2dTile & operator=(const vxTexMap2dTile & other);

  /// get tile origin in real world coordinates
  inline const Vector<float4> & GetOrigin() const { return m_origin; }
  
  /// set tile origin
  inline void SetOrigin(const Vector<float4> & origin) { m_origin = origin; }

  /// get tile size in real world coordinates
  inline const Vector<float4> & GetSize() const { return m_size; }

  /// set the real world size of the tile
  inline void SetSize(const Vector<float4> & size) { m_size = size; }

  /// get normal of the tile plane
  const Normal<float4> & GetNormal() const;

  /// sets the normal of the tile plane.
  inline void SetNormal(const Normal<float4> & normal) { m_normal = normal; }

  /// get location of vertex (0-3) in real world coordinates
  Point<float4> GetVertex(const uint1 & uIndex) const;

  /// get vertex index based on view direction
  uint1 GetVertexIndex( const uint1 & uViewDir, const uint1 & uCornerIndex ) const;

  /// get tile size in index coordinates
  inline const Vector<uint2> & GetVolumeIndexOrigin() const { return m_volumeIndexOrigin; }
  
  /// set tile size in index coordinates
  inline void SetVolumeIndexOrigin(const Vector<uint2> & origin) { m_volumeIndexOrigin = origin; }

  /// get tile size in index coordinates 
  inline const Triple<int4> & GetVolumeIndexSize() const { return m_volumeIndexSize; }
  
  /// set tile size in index coordinates
  inline void SetVolumeIndexSize(const Triple<int4> & size)   { m_volumeIndexSize = size; }

  /// get the texture coordinate of lower left
  inline const Vector<float4> & GetTexCoordLowerLeft() const { return m_texCoordLowerLeft; }
  
  /// get the texture coordinate of upper right
  inline const Vector<float4> & GetTexCoordUpperRight() const { return m_texCoordUpperRight; }
  
  /// Get the texture coordinate of vertex (0-3)
  Point<float4> GetVertexTexCoord(const uint1 & uIndex) const;
  
  /// Get the size in texture coordinate space
  Vector<float4> GetTexCoordSize() const;

  /// get the density texture ID for this tile (handled by the TexMap2dServer)
  inline const GLuint & GetDensityTextureId() const { return m_iDensityTextureId; }
      
  /// set the density texture ID for this tile (handled by the TexMap2dServer)
  inline void SetDensityTextureId(const GLuint & id) { m_iDensityTextureId = id; }

  /// get the label texture ID for this tile (handled by the TexMap2dServer)
  inline const GLuint & GetLabelTextureId() const { return m_iLabelTextureId; }

  /// set the label texture ID for this tile (handled by the TexMap2dServer)
  inline void SetLabelTextureId(const GLuint & id) { m_iLabelTextureId = id; }

  /// get the distance to the eye point (handled by the TexMap2dServer)
  inline const float4 GetDistance() const { return m_fDistance; }

  /// set the distance to the eye point (handled by the TexMap2dServer)
  inline void SetDistance(float4 fDistance) { m_fDistance = fDistance; }

  /// clip with box, if true (i.e., intersect) member data is adjusted
  bool Clip(Box<float4> & cropBox);
  
  /// clip with slicing plane. 
  bool Clip(vxPlaneOrtho<float4> & cutPlane, const Triple<float4> fWorldToVolume);

  /// set the texture coordinate of lower left
  void SetTexCoordLowerLeft(const Vector<float4> & texCoord);
  
  /// set the texture coordinate of upper right (handled by the TexMap2dServer)
  void SetTexCoordUpperRight(const Vector<float4> & texCoord);
  
// data
private:

  /// actual tile origin
  Vector<float4> m_origin;
  /// normal vector of tile
  Normal<float4> m_normal;
  /// actual Tile size (index coordinates)
  Vector<float4> m_size;
  /// index coordinates of origin within volume
  Vector<uint2>  m_volumeIndexOrigin;
  /// actual Tile size (index coordinates)
  Triple<int4>   m_volumeIndexSize;
  /// texture coordinate lower left
  Vector<float4> m_texCoordLowerLeft;
  /// texture coordinate upper right
  Vector<float4> m_texCoordUpperRight;
  /// distance to eye position
  float4         m_fDistance;
  /// density texture ID
  GLuint         m_iDensityTextureId;
  /// label texture ID
  GLuint         m_iLabelTextureId;
  /// mapping of tile vertex indexes
  static const uint1 m_vuVrtxIdxMap[3][4];
};


#endif // vxTexMap2dTile_h


// $Log: vxTexMap2dTile.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2003/06/04 17:08:40  jmeade
// code review.
//
// Revision 1.11  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.10  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.9  2003/04/18 16:04:10  michael
// initial code review
//
// Revision 1.8  2003/01/22 21:47:00  ingmar
// dimension and units are now Triples
//
// Revision 1.7  2001/01/02 15:28:47  ingmar
// float8 -> float4 (no need to render with more than 23bits precision)
//
// Revision 1.6  2001/01/02 14:57:53  ingmar
// float4 -> float4 (no need to render with more than 23bits precision)
//
// Revision 1.5  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.4  2002/10/21 14:20:09  frank
// Necessary to export this class so we can have a software renderer outside of vxbase
//
// Revision 1.3  2002/10/17 22:16:46  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.2  2002/08/14 22:45:26  wenli
// initial version - Vascular
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.7  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 1.6  2002/06/25 21:49:47  uday
// Clip() parameter update.
//
// Revision 1.5  2002/05/16 01:30:22  michael
// removed unused code
//
// Revision 1.4  2002/05/07 18:23:43  frank
// Removed unneccessary includes.
//
// Revision 1.3  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.2  2002/04/23 21:36:48  jenny
// software renderer is not a template class any more
//
// Revision 1.1  2002/04/02 21:46:22  geconomos
// Moved from 75_rendererUtils and renamed to vxXXX.
//
// Revision 1.10  2002/03/27 17:38:13  jaddonisio
// Vertex index mapping inside GetVertex().
//
// Revision 1.9  2002/03/22 15:40:59  jaddonisio
// Comment change.
//
// Revision 1.8  2002/03/16 22:34:16  jaddonisio
// Copy constructor.
//
// Revision 1.7  2002/03/16 18:06:45  michael
// made protected stuff public such that class can be re-used. Will need further
// clean-up ... at some point.
//
// Revision 1.6  2002/03/04 23:47:30  jaddonisio
// Specifiy arguments per coding standard.
//
// Revision 1.5  2002/03/04 20:33:22  michael
// adjusted all PlaneOrtho and PlaneOblique instantiations with the new
// templetized types ...
//
// Revision 1.4  2002/02/27 20:13:32  jaddonisio
// Comments.
//
// Revision 1.3  2002/02/25 23:12:14  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 1.2  2002/02/19 17:19:16  jaddonisio
// i. New normal vector for a Tile.
// ii. New Clip method with PlaneOrtho.
// iii. New vertex index mapping for class.
//
// Revision 1.1  2002/02/06 14:52:01  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxTexMap2dTile.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxTexMap2dTile.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
