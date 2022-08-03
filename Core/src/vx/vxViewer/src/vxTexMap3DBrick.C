// $Id: vxTexMap3DBrick.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Michael Meissner (michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxTexMap3dBrick.h"
#include <utility>
#include "box.h"


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


// static member initialization
const std::pair<uint1,uint1> vxTexMap3dBrick::m_vSideSegIndices[12] = { std::pair<uint1,uint1>(0,1), std::pair<uint1,uint1>(0,2), std::pair<uint1,uint1>(0,4),
                                                                        std::pair<uint1,uint1>(3,1), std::pair<uint1,uint1>(3,2), std::pair<uint1,uint1>(3,7),
                                                                        std::pair<uint1,uint1>(5,1), std::pair<uint1,uint1>(5,4), std::pair<uint1,uint1>(5,7),
                                                                        std::pair<uint1,uint1>(6,2), std::pair<uint1,uint1>(6,4), std::pair<uint1,uint1>(6,7), };
/**
 * Constructor
 */ 
vxTexMap3dBrick::vxTexMap3dBrick()
//*******************************************************************
{
  SetOrigin(Vector<float4>(0.0, 0.0, 0.0));
  SetSize(Vector<float4>(0.0, 0.0, 0.0));
  SetVolumeIndexOrigin(Vector<uint2>(0.0, 0.0, 0.0));
  SetVolumeIndexSize(Triplei(0, 0, 0));
  SetTexCoordLowerLeft(Vector<float4>(0.0, 0.0, 0.0));
  SetTexCoordUpperRight(Vector<float4>(1.0, 1.0, 1.0));
  SetDensityTextureId(0);
  SetLabelTextureId(0);
  SetDistance(-1.0F);
} // constructor


/** 
 * assignment operator, needs to copy all data into this
 * @param brick brick
 * @return const reference to this
 */
vxTexMap3dBrick & vxTexMap3dBrick::operator=(const vxTexMap3dBrick & brick)
//*******************************************************************
{
  if (&brick != this)
  {
    SetOrigin(brick.GetOrigin());
    SetSize(brick.GetSize());
    SetVolumeIndexOrigin(brick.GetVolumeIndexOrigin());
    SetVolumeIndexSize(brick.GetVolumeIndexSize());
    SetTexCoordLowerLeft(brick.GetTexCoordLowerLeft());
    SetTexCoordUpperRight(brick.GetTexCoordUpperRight());
    SetDistance(brick.GetDistance());
    SetDensityTextureId(brick.GetDensityTextureId());
    SetLabelTextureId(brick.GetLabelTextureId());
  }

  return *this;
} // operator=()


/**
 * Get the side segment indices.
 * @param uSideIndex index to side
 * @return pair on of indices
 * Indices are as follows:
 *      6-----7
 *     /|    /|
 *    2-----3 |
 *    | |   | |
 *    | 4---|-5
 *    |/    |/
 *    0-----1
 *
 * @param uSideIndex   Side index
 * @return
 */
std::pair<uint1, uint1> vxTexMap3dBrick::GetSideSegIndices(uint1 uSideIndex) const
//*******************************************************************
{
  return m_vSideSegIndices[uSideIndex];
}


/**
 * Get the edge points of a side.
 * @param uSideIndex index to side
 * @param point1 point1
 * @param point2 point2
 * Indices are as follows:
 *      6-----7
 *     /|    /|
 *    2-----3 |
 *    | |   | |
 *    | 4---|-5
 *    |/    |/
 *    0-----1
 *
 * @param index of edge
 * @param point1
 * @param point2
 */
void vxTexMap3dBrick::GetEdgeVertices(uint1 uSideIndex, 
                                      Point<float4> & point1, 
                                      Point<float4> & point2) const
//*******************************************************************
{
  point1 = GetVertex(m_vSideSegIndices[uSideIndex].first);
  point2 = GetVertex(m_vSideSegIndices[uSideIndex].second);
} // GetEdgeVertices()


/**
 * Returns the actual coordinate of the vertex
 * @param uIndex index to corner vertex
 * @return point
 * Indices are as follows:
 *      6-----7
 *     /|    /|
 *    2-----3 |
 *    | |   | |
 *    | 4---|-5
 *    |/    |/
 *    0-----1
 *
 * @param uIndex
 * @return
 */
Point<float4> vxTexMap3dBrick::GetVertex(const uint1 & uIndex) const
//*******************************************************************
{
  return Point<float4>(GetOrigin().m_x + GetSize().m_x * ((uIndex & 1)        ? 1.0 : 0.0),
                       GetOrigin().m_y + GetSize().m_y * (((uIndex >> 1) & 1) ? 1.0 : 0.0),
                       GetOrigin().m_z + GetSize().m_z * ((uIndex & 4)        ? 1.0 : 0.0));
}


/**
 * Returns the actual texture coordinate of the vertex
 * @param uIndex index to corner vertex
 * @return vector
 * Indices are as follows:
 *      6-----7
 *     /|    /|
 *    2-----3 |
 *    | |   | |
 *    | 4---|-5
 *    |/    |/
 *    0-----1
 *
 * @param uIndex    Vertex index.
 * @return
 */ 
Vector<float4> vxTexMap3dBrick::GetVertexTexCoord(const uint1 & uIndex) const
//*******************************************************************
{
  return Vector<float4>(GetTexCoordLowerLeft().m_x + ((uIndex & 1)        ? GetTexCoordSize().m_x : 0.0),
                        GetTexCoordLowerLeft().m_y + (((uIndex >> 1) & 1) ? GetTexCoordSize().m_y : 0.0),
                        GetTexCoordLowerLeft().m_z + ((uIndex & 4)        ? GetTexCoordSize().m_z : 0.0));
}


/**
 * Get the texture edge points of a side.
 * @param uSideIndex index to edge
 * @param point1 point1
 * @param point2 point2
 *
 * Indices are as follows:
 *      6-----7
 *     /|    /|
 *    2-----3 |
 *    | |   | |
 *    | 4---|-5
 *    |/    |/
 *    0-----1
 *
 * @param uSideIndex   Edge index.
 * @param point1       First texture edge point.
 * @param point2       Second texture edge point.
 */
void vxTexMap3dBrick::GetTexEdgeVertices(uint1 uSideIndex, 
                                         Point<float4> & point1, 
                                         Point<float4> & point2) const
//*******************************************************************
{
  point1 = Point<float4>(GetVertexTexCoord(m_vSideSegIndices[uSideIndex].first));
  point2 = Point<float4>(GetVertexTexCoord(m_vSideSegIndices[uSideIndex].second));
}



/**
 * Returns the actual size in texture coordinate space
 *
 * @return the actual size in texture coordinate space
 */
Vector<float4> vxTexMap3dBrick::GetTexCoordSize() const
//*******************************************************************
{
  return Vector<float4>(vxTexMap2dTile::GetTexCoordSize().m_x, vxTexMap2dTile::GetTexCoordSize().m_y,
                        GetTexCoordUpperRight().m_z - GetTexCoordLowerLeft().m_z);
}


/**
 * Clip
 * @param box clipping box 
 * @return succesful?
 */ 
bool vxTexMap3dBrick::Clip(Box<float4> & box)
//*******************************************************************
{
  // for xy-dimension
  ////////////////////
  if (vxTexMap2dTile::Clip(box) == false)
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
  float4 fWeightZmin((fMinZ - GetOrigin().m_z) / (GetSize().m_z));
  float4 fWeightZmax((fMaxZ - GetOrigin().m_z) / (GetSize().m_z));
  float4 fTexCoordZmin(GetTexCoordLowerLeft().m_z + fWeightZmin * GetTexCoordSize().m_z);
  float4 fTexCoordZmax(GetTexCoordLowerLeft().m_z + fWeightZmax * GetTexCoordSize().m_z);

  SetOrigin(Vector<float4>(vxTexMap2dTile::GetOrigin().m_x, vxTexMap2dTile::GetOrigin().m_y, fMinZ));
  SetSize(Vector<float4>(vxTexMap2dTile::GetSize().m_x, vxTexMap2dTile::GetSize().m_y, fMaxZ-fMinZ));
  SetTexCoordLowerLeft(Vector<float4>(vxTexMap2dTile::GetTexCoordLowerLeft().m_x, vxTexMap2dTile::GetTexCoordLowerLeft().m_y, fTexCoordZmin));
  SetTexCoordUpperRight(Vector<float4>(vxTexMap2dTile::GetTexCoordUpperRight().m_x, vxTexMap2dTile::GetTexCoordUpperRight().m_y, fTexCoordZmax));

  return true;
} // Clip()


#undef FILE_REVISION


// Revision History:
// $Log: vxTexMap3DBrick.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/06/05 13:30:25  michael
// cosmetics
//
// Revision 1.8  2003/06/05 12:25:57  michael
// code review
//
// Revision 1.7  2003/06/05 12:24:22  michael
// code review
//
// Revision 1.6  2003/06/04 17:58:36  jmeade
// code review.
//
// Revision 1.5  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.4  2003/04/18 16:04:10  michael
// initial code review
//
// Revision 1.3  2003/01/22 21:47:28  ingmar
// dimension and units are now Triples
//
// Revision 1.2  2001/01/02 15:28:25  ingmar
// float8 -> float4 (no need to render with more than 23bits precision)
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.22  2002/05/29 19:40:49  jaddonisio
//     End Project Dependency
// Ported from TexMapBrick3D and changed for new vxBlockVolume.
//
// Revision 1.21  2002/04/02 21:50:55  geconomos
// Updated to use vxServerTexMap2d in lieu of ServerTexMap2d.
//
// Revision 1.20  2002/03/27 17:44:21  jaddonisio
// New function to get edge vertices.
//
// Revision 1.19  2002/02/25 23:12:14  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 1.18  2002/02/19 17:38:22  jaddonisio
// Return copies.
//
// Revision 1.17  2002/02/06 14:53:24  jaddonisio
// Changed TexMap3dBrick class so that it is derived from the new base class vxTexMap2dTile.
//
// Revision 1.16  2002/02/05 14:43:59  michael
// cleaned code such that Joe can now work on the 2d texture map implementation
//
// Revision 1.15  2002/02/04 18:20:10  michael
// re-structured 3D texmap classes to serve as template for new 2D texmap classes
//
// Revision 1.14  2002/02/04 16:37:13  michael
// removed texture data since this is handled in OpenGL (holds a real copy of it)
//
// Revision 1.13  2002/01/31 16:53:43  michael
// naming and coding convention stuff
//
// Revision 1.12  2002/01/29 20:09:59  michael
// comments, restructuring, and misc
//
// Revision 1.11  2001/12/21 00:10:15  michael
// 3d texture mapping done, works all fine. :-)
//
// Revision 1.10  2001/12/20 16:30:57  michael
// new 3d texture mapping works now fine with tags inside
//
// Revision 1.9  2001/12/20 07:25:50  michael
// 3d texture mapping with dependent texture classification does now work
//
// Revision 1.8  2001/12/17 23:57:33  michael
// 3D texture mapping now works on the NVIDIA but no tags yet :-(
//
// Revision 1.7  2001/12/13 23:32:27  michael
// bricking now works nicely. Bricks are always chosen such that they do not
// waste texture memory when being larger at the volume border. :-)
//
// Revision 1.6  2001/12/12 23:55:17  michael
// 3d texture mapping is now compiling through
//
// Revision 1.5  2001/12/11 22:14:31  michael
// new 3dTexMapRenderer compiles but still enough to do ...
//
// Revision 1.4  2001/12/11 16:52:50  michael
// added comments ...
//
// Revision 1.3  2001/12/11 16:29:59  michael
// added the sorting methods and the rendering of the plane.
//
// Revision 1.2  2001/12/10 17:22:47  michael
// added texture id and actual texture generation
//
// Revision 1.1  2001/12/07 14:12:46  michael
// Added brick class for 3D texture mapping as well as a brick server that
// generates bricks out of a given volume.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxTexMap3DBrick.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxTexMap3DBrick.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $