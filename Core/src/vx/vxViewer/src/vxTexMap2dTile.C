// $Id: vxTexMap2dTile.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Joseph Addonisio (jaddonisio@viatronix.com)

/**
 *  Class implementation for Texture Map 2D Tile(vxTexMap2dTile).
 */


// includes
#include "stdafx.h"
#include "vxTexMap2dTile.h"
#include "vxPlaneOrtho.h"
#include "Box.h"
#include <math.h>


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


// static member initialization
const uint1 vxTexMap2dTile::m_vuVrtxIdxMap[3][4] = {{0,4,6,2}, {0,1,5,4}, {0,1,3,2}};


/**
 * Constructor
 */ 
vxTexMap2dTile::vxTexMap2dTile() :
//*******************************************************************
  m_origin(Vector<float4>(0.0, 0.0, 0.0)),  m_normal(Normal<float4>(0.0, 0.0, 1.0)),
  m_size(Vector<float4>(0.0, 0.0, 0.0)),  m_volumeIndexOrigin(Vector<uint2 >(0.0, 0.0, 0.0)),
  m_volumeIndexSize(Triple< int4 >(0.0, 0.0, 0.0)),  m_texCoordLowerLeft(Vector<float4>(0.0, 0.0, 0.0)),
  m_texCoordUpperRight(Vector<float4>(1.0, 1.0, 0.0)),  m_iDensityTextureId(0),
  m_iLabelTextureId(0), m_fDistance(-1.0F)
{
} // constructor


/** 
 * Copy constructor
 *
 * @param reference to a vxTexMap2dTile object.
 */
vxTexMap2dTile::vxTexMap2dTile(const vxTexMap2dTile & other)
//*******************************************************************
{
  if (&other != this)
  {
    SetOrigin(other.GetOrigin());
    SetNormal(other.GetNormal());
    SetSize(other.GetSize());
    SetVolumeIndexOrigin(other.GetVolumeIndexOrigin());
    SetVolumeIndexSize(other.GetVolumeIndexSize());
    SetTexCoordLowerLeft(other.GetTexCoordLowerLeft());
    SetTexCoordUpperRight(other.GetTexCoordUpperRight());
    SetDistance(other.GetDistance());
    SetDensityTextureId(other.GetDensityTextureId());
    SetLabelTextureId(other.GetLabelTextureId());
  }
} // copy constructor


/** 
 * operator=
 * ---------
 * Assignment operator, needs to copy all data into this class.
 *
 * @param reference to a vxTexMap2dTile object.
 * @return Reference to this object.
 */
vxTexMap2dTile & vxTexMap2dTile::operator=(const vxTexMap2dTile & tile)
//*******************************************************************
{
  if (&tile != this)
  {
    SetOrigin(tile.GetOrigin());
    SetNormal(tile.GetNormal());
    SetSize(tile.GetSize());
    SetVolumeIndexOrigin(tile.GetVolumeIndexOrigin());
    SetVolumeIndexSize(tile.GetVolumeIndexSize());
    SetTexCoordLowerLeft(tile.GetTexCoordLowerLeft());
    SetTexCoordUpperRight(tile.GetTexCoordUpperRight());
    SetDistance(tile.GetDistance());
    SetDensityTextureId(tile.GetDensityTextureId());
    SetLabelTextureId(tile.GetLabelTextureId());
  }

  return *this;
} // operator=()


/**
 * Public method to get the actual coordinate of the vertex.
 * index uses the following indexing of the eight vertices:
 *      6-----7
 *     /|    /|
 *    2-----3 |
 *    | |   | |
 *    | 4---|-5
 *    |/    |/
 *    0-----1
 *
 * @param uCornerIndex   value of index.
 * @return               Point of vertex.
 */
Point<float4> vxTexMap2dTile::GetVertex(const uint1 & uCornerIndex) const
//*******************************************************************
{
  uint1 uVertexIndex = GetVertexIndex(m_normal.GetMaxAbsComponentCoord(), uCornerIndex);
  return Point<float4>(GetOrigin().m_x + GetSize().m_x * ((uVertexIndex & 1)        ? 1.0 : 0.0),
                       GetOrigin().m_y + GetSize().m_y * (((uVertexIndex >> 1) & 1) ? 1.0 : 0.0),
                       GetOrigin().m_z + GetSize().m_z * ((uVertexIndex & 4)        ? 1.0 : 0.0));
} // GetVertex()


/**
 * Public method to get normal vector of the plane
 *
 * @param uViewDir       View direction, 0(x-axis), 1(y-axis), or 2(z-axis)
 * @param uCornerIndex   Corner index of tile; 0, 1, 2, 3
 * @return               Tile vertex index
 */
uint1 vxTexMap2dTile::GetVertexIndex(const uint1 & uViewDir, 
                                     const uint1 & uCornerIndex) const
                                     //*******************************************************************
{
  return m_vuVrtxIdxMap[uViewDir][uCornerIndex];
}


/**
 * Public method to get normal vector of the plane
 *
 * @return normal
 */
const Normal<float4> & vxTexMap2dTile::GetNormal() const
//*******************************************************************
{
  return (const Normal<float4>&)(m_normal);
}


/**
 * Public method to get the actual texture coordinate of the vertex.
 * iIndex uses the following indexing of the eight vertices:
 *      6-----7
 *     /|    /|
 *    2-----3 |
 *    | |   | |
 *    | 4---|-5
 *    |/    |/
 *    0-----1
 *
 * @param uCornerIndex   value of index.
 * @return               Point of vertex in texture coordinates.
 */
Point<float4> vxTexMap2dTile::GetVertexTexCoord(const uint1 & uCornerIndex) const
//*******************************************************************
{
  uint1 uVertexIndex(GetVertexIndex(m_normal.GetMaxAbsComponentCoord(), uCornerIndex));
  return Point<float4>(GetTexCoordLowerLeft().m_x + ((uVertexIndex & 1)        ? GetTexCoordSize().m_x : 0.0),
                       GetTexCoordLowerLeft().m_y + (((uVertexIndex >> 1) & 1) ? GetTexCoordSize().m_y : 0.0),
                       GetTexCoordLowerLeft().m_z + ((uVertexIndex & 4)        ? GetTexCoordSize().m_z : 0.0));
} // GetVertexTexCoord()


/**
 * Public method to set the lower left texture coordinate of the vertex
 *
 * @param texCoord   vector to texture coordinate
 */
void vxTexMap2dTile::SetTexCoordLowerLeft(const Vector<float4> & texCoord)
//*******************************************************************
{
  m_texCoordLowerLeft = Vector<float4>(texCoord.m_x, texCoord.m_y, texCoord.m_z);
}


/**
 * SetTexCoordUpperRight
 * ---------------------
 * Public method to set the upper right texture coordinate of the vertex.
 *
 * @param texCoord   vector to texture coordinate.
 */
void vxTexMap2dTile::SetTexCoordUpperRight(const Vector<float4> & texCoord)
//*******************************************************************
{
  m_texCoordUpperRight = Vector<float4>(texCoord);
}


/**
 * Public method to get the actual size in texture coordinate space
 *
 * @return Vector to coordinate size
 */
Vector<float4> vxTexMap2dTile::GetTexCoordSize() const
//*******************************************************************
{
  return Vector<float4>(GetTexCoordUpperRight().m_x - GetTexCoordLowerLeft().m_x,
                        GetTexCoordUpperRight().m_y - GetTexCoordLowerLeft().m_y,
                        GetTexCoordUpperRight().m_z - GetTexCoordLowerLeft().m_z);
}


/**
 * Clip
 *
 * @param box - cropping box
 * @return Success flag
 */ 
bool vxTexMap2dTile::Clip(Box<float4> & box)
//*******************************************************************
{
  // for x-dimension
  ////////////////////
  float4 fMinX(GetOrigin().m_x);
  if (box.GetMinX() > fMinX)
  {
    fMinX = box.GetMinX();
  }
  float4 fMaxX(GetVertex(1).m_x);
  if (box.GetMaxX() < fMaxX)
  {
    fMaxX = box.GetMaxX();
  }
  if (fMinX > GetVertex(1).m_x || fMaxX < GetOrigin().m_x)
  {
    return false;
  }
  
  // for y-dimension
  ////////////////////
  float4 fMinY(GetOrigin().m_y);
  if (box.GetMinY() > fMinY)
  {
    fMinY = box.GetMinY();
  }
  float4 fMaxY(GetVertex(2).m_y);
  if (box.GetMaxY() < fMaxY)
  {
    fMaxY = box.GetMaxY();
  }
  if (fMinY > GetVertex(2).m_y || fMaxY < GetOrigin().m_y)
  {
    return false;
  }
  
  // for z-dimension
  ////////////////////
  float4 fMinZ(GetOrigin().m_z);
  if (box.GetMinZ() > fMinZ)
  {
    fMinZ = box.GetMinZ();
  }
  float4 fMaxZ(GetVertex(4).m_z);
  if (box.GetMaxZ() < fMaxZ)
  {
    fMaxZ = box.GetMaxZ();
  }
  if (fMinZ > GetVertex(4).m_z || fMaxZ < GetOrigin().m_z)
  {
    return false;
  }

  // compute weights
  ////////////////////
  float4 fWeightXmin((fMinX - GetOrigin().m_x) / (GetSize().m_x));
  float4 fWeightXmax((fMaxX - GetOrigin().m_x) / (GetSize().m_x));
  float4 fWeightYmin((fMinY - GetOrigin().m_y) / (GetSize().m_y));
  float4 fWeightYmax((fMaxY - GetOrigin().m_y) / (GetSize().m_y));
  float4 fWeightZmin((fMinZ - GetOrigin().m_z) / (GetSize().m_z));
  float4 fWeightZmax((fMaxZ - GetOrigin().m_z) / (GetSize().m_z));

  float4 fTexCoordXmin(GetTexCoordLowerLeft().m_x + fWeightXmin * GetTexCoordSize().m_x);
  float4 fTexCoordXmax(GetTexCoordLowerLeft().m_x + fWeightXmax * GetTexCoordSize().m_x);
  float4 fTexCoordYmin(GetTexCoordLowerLeft().m_y + fWeightYmin * GetTexCoordSize().m_y);
  float4 fTexCoordYmax(GetTexCoordLowerLeft().m_y + fWeightYmax * GetTexCoordSize().m_y);
  float4 fTexCoordZmin(GetTexCoordLowerLeft().m_z + fWeightZmin * GetTexCoordSize().m_z);
  float4 fTexCoordZmax(GetTexCoordLowerLeft().m_z + fWeightZmax * GetTexCoordSize().m_z);

  SetOrigin(Vector<float4>(fMinX, fMinY, fMinZ));
  SetSize(Vector<float4>(fMaxX-fMinX, fMaxY-fMinY, fMaxZ-fMinZ));
  SetTexCoordLowerLeft(Vector<float4>(fTexCoordXmin, fTexCoordYmin, fTexCoordZmin));
  SetTexCoordUpperRight(Vector<float4>(fTexCoordXmax, fTexCoordYmax, fTexCoordZmax));

  return true;
} // Clip()


/**
 * Clip
 *
 * @param cutPlane         Cutting orthonormal plane.
 * @param fWorldToVolume   conversion between world and volume coordinates.
 * @return Success flag.
 */ 
bool vxTexMap2dTile::Clip(vxPlaneOrtho<float4> & cutPlane, 
                          const Triple<float4> fWorldToVolume)
//*******************************************************************
{
  // Flags: Visibility & Clipping
  bool bClip   (false);
  bool bVisible(false);

  // Cutting plane setup:
  Normal<float4> cutPlaneNormal(cutPlane.GetNormal());
  uint2  iCutPlaneDir      = cutPlaneNormal.GetMaxAbsComponentCoord();
  float4 fCutPlaneNormDist = cutPlane.GetAbsDistance();

  // Tile Origin & Size:
  Vector<float4> tileSize(GetSize());
  Vector<float4> tileOrigin(GetOrigin());
  uint1 uTileAxisIndex = m_normal.GetMaxAbsComponentCoord(); // 0(x), 1(y), or 2(z).
  Triple<float4> fWorldToVol(fWorldToVolume);

  // Tile Corners(0,1,2,3, starting lower-left and progressing counterclockwise):
  Point<float4> P0 = GetVertex(0);  // Min. corner.
  Point<float4> P2 = GetVertex(2);  // Max. corner.

  // Distance to tile corner:
  float4 dP0 = cutPlane.GetDistance(P0);  // Signed distance to P0.
  float4 dP2 = cutPlane.GetDistance(P2);  // Signed distance to P2.

  // Set flags:
  bVisible = (dP0 > 0.0 || dP2 > 0.0);  // visible if at least one corner is in front of plane.
  bClip    = (dP0 * dP2) < 0.0;         // clip if one corner on each side of plane.

  // Perform clipping(if necessary):
  if (bClip == true)
  {
    if (cutPlaneNormal[iCutPlaneDir] > 0)
    {
      Vector<float4> newOrigin     (tileOrigin);
      Vector<float4> newSize       (tileSize);
      Vector<uint2 > newIndexOrigin(GetVolumeIndexOrigin());
      Triplei        newIndexSize  (GetVolumeIndexSize());
      newOrigin     [iCutPlaneDir] = fCutPlaneNormDist;
      newSize       [iCutPlaneDir] = tileSize[iCutPlaneDir] - (fCutPlaneNormDist - tileOrigin[iCutPlaneDir]);
      newIndexOrigin[iCutPlaneDir] = ceil(fCutPlaneNormDist*fWorldToVol[iCutPlaneDir]);
      newIndexSize  [iCutPlaneDir] = ceil(newSize[iCutPlaneDir]*fWorldToVol[iCutPlaneDir]);

      Vector<float4> texCoordLL(GetTexCoordLowerLeft());
      Vector<float4> texCoordSize(GetTexCoordSize());
      Vector<float4> newTexCoordLL(texCoordLL);
      newTexCoordLL[iCutPlaneDir] = texCoordLL[iCutPlaneDir] + ((fCutPlaneNormDist-tileOrigin[iCutPlaneDir])/tileSize[iCutPlaneDir]) * texCoordSize[iCutPlaneDir];

      // Update Tile with new dimensions:
      SetOrigin(newOrigin);
      SetSize(newSize);
      SetVolumeIndexOrigin(newIndexOrigin);
      SetVolumeIndexSize(newIndexSize);
      SetTexCoordLowerLeft(newTexCoordLL);
    } // if cut plane normal > 0
    else
    {
      Vector<float4> newSize(tileSize);
      Triplei newIndexSize(GetVolumeIndexSize());
      newSize     [iCutPlaneDir] = fCutPlaneNormDist - tileOrigin[iCutPlaneDir];
      newIndexSize[iCutPlaneDir] = ceil(newSize[iCutPlaneDir]*fWorldToVol[iCutPlaneDir]);
      Vector<float4> texCoordLL(GetTexCoordLowerLeft());
      Vector<float4> texCoordUR(GetTexCoordUpperRight());
      Vector<float4> texCoordSize(GetTexCoordSize());
      Vector<float4> newTexCoordUR(texCoordUR);
      newTexCoordUR[iCutPlaneDir] = texCoordLL[iCutPlaneDir] + ((fCutPlaneNormDist-tileOrigin[iCutPlaneDir])/tileSize[iCutPlaneDir]) * texCoordSize[iCutPlaneDir];
      // Update Tile with new dimensions:
      SetSize(newSize);
      SetVolumeIndexSize(newIndexSize);
      SetTexCoordUpperRight(newTexCoordUR);
    } // else cut plane normal <= 0
  } // if clip true

  return bVisible;
}


#undef FILE_REVISION


// $Log: vxTexMap2dTile.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/06/04 17:08:40  jmeade
// code review.
//
// Revision 1.7  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.6  2003/04/18 16:04:10  michael
// initial code review
//
// Revision 1.5  2003/01/22 21:47:10  ingmar
// dimension and units are now Triples
//
// Revision 1.4  2001/01/02 15:28:47  ingmar
// float8 -> float4 (no need to render with more than 23bits precision)
//
// Revision 1.3  2001/01/02 14:57:53  ingmar
// float4 -> float4 (no need to render with more than 23bits precision)
//
// Revision 1.2  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.6  2002/06/25 21:49:47  uday
// Clip() parameter update.
//
// Revision 1.5  2002/05/14 21:00:38  jaddonisio
// Removed duplicate call to GetVertexIndex(which is included in GetVertex).
//
// Revision 1.4  2002/05/08 21:24:27  manju
// Changed GetMaxComponentCoord to GetAbsMaxComponentCoord
//
// Revision 1.3  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.2  2002/05/01 13:49:00  michael
// Started code review related stuff ...
//
// Revision 1.1  2002/04/02 21:46:22  geconomos
// Moved from 75_rendererUtils and renamed to vxXXX.
//
// Revision 1.9  2002/03/27 17:38:13  jaddonisio
// Vertex index mapping inside GetVertex().
//
// Revision 1.8  2002/03/16 22:34:16  jaddonisio
// Copy constructor.
//
// Revision 1.7  2002/03/04 23:47:30  jaddonisio
// Specifiy arguments per coding standard.
//
// Revision 1.6  2002/03/04 20:33:22  michael
// adjusted all vxPlaneOrtho and PlaneOblique instantiations with the new
// templetized types ...
//
// Revision 1.5  2002/02/27 20:20:26  jaddonisio
// Clean up and put in const for Get functions.
//
// Revision 1.4  2002/02/25 23:12:14  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 1.3  2002/02/25 13:20:08  michael
// no more direction in plane, instead, GetNormal().GetAbsMaxComponentCoord()
//
// Revision 1.2  2002/02/19 17:19:16  jaddonisio
// i. New normal vector for a Tile.
// ii. New Clip method with vxPlaneOrtho.
// iii. New vertex index mapping for class.
//
// Revision 1.1  2002/02/06 14:52:01  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxTexMap2dTile.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxTexMap2dTile.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
