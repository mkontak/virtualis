// $Id: vxRendererTexMap2DMPRNew.C,v 1.4 2006/01/18 20:39:26 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)

/*
  Renderer that does not require a texture server but
  generates the textures on the fly. In addition, uses
  simple slice drawing without requiring texture shaders
  and/or register combiners
*/


// includes
#include "stdafx.h"
#include "vxRendererTexMap2DMPRNew.h"
#include "vxMathUtils.h"
#include "vxPlaneOrtho.h"
#include "vxEnvironment.h"
#include "vxTexMap2dTile.h"
#include "vxUtils.h"
#include "vxRenderingMode.h"


// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Constructor
 * @param reference to an environment object
 */ 
vxRendererTexMap2DMPRNew::vxRendererTexMap2DMPRNew(vxEnvironment & environment)
:vxRendererTexMap(environment),
m_piTFTextureDataAlphaOne(NULL),
m_pTextureIntensityBuffer(NULL),
m_pTextureLabelBuffer(NULL)
//*******************************************************************
{
} // constructor


/**
 * Routine to initialize renderer
 */
void vxRendererTexMap2DMPRNew::Initialize()
//*******************************************************************
{
  vxRendererTexMap::Initialize();

  // create texture Id
  GLuint Ids[3];
  glGenTextures(3, Ids);  
  SetIntensityTextureID(Ids[0]);
  SetLabelTextureID(Ids[1]);
  SetTFtextureIDalphaOne(Ids[2]);
} // Initialize()


/**
 * Routine to uninitialize renderer
 */
void vxRendererTexMap2DMPRNew::Uninitialize()
//*******************************************************************
{
  vxRendererTexMap::Uninitialize();

  // delete the texture ID
  GLuint Id0(GetIntensityTextureID());
  glDeleteTextures(1, &Id0);
  GLuint Id1(GetLabelTextureID());
  glDeleteTextures(1, &Id1);
  GLuint Id2(GetTFtextureIDalphaOne());
  glDeleteTextures(1, &Id2);
} // Uninitialize()


/**
 * Set the transfer function, triggers dependent texture generation.
 */
void vxRendererTexMap2DMPRNew::UpdateTransferFunctionTexture()
//*******************************************************************
{
  uint2 iSizeX(0);
  uint2 iSizeY(0);
  uint1 iAlpha(0), iRed(0), iGreen(0), iBlue(0);
  float4 * pRGBA(GetEnvironment().GetColormap()->GetTablePointerRGBA(255));
  const bool bShowColor(GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_COLORMAP_IN_MPR));

  if (GetRenderMode() == DENSITY_TAG_POST_CLASSIFY)
  {
    iSizeX = 256;
    iSizeY = 2;

    if (m_piTFTextureDataAlphaOne == NULL)
    {
      m_piTFTextureDataAlphaOne = new GLubyte[4*iSizeY*iSizeX];
    }

    float4 fAlpha(0.0);
    for (int i(0); i<256; i++)
    {
      if (bShowColor == false)
      {
        iRed = iGreen = iBlue = pRGBA[4*i + 3];
      }
      else
      {
        iRed   = pRGBA[4*i + 0];
        iGreen = pRGBA[4*i + 1];
        iBlue  = pRGBA[4*i + 2];
      }
      m_piTFTextureDataAlphaOne[4*i + 0] = iRed; 
      m_piTFTextureDataAlphaOne[4*i + 1] = iGreen; 
      m_piTFTextureDataAlphaOne[4*i + 2] = iBlue; 
      m_piTFTextureDataAlphaOne[4*i + 3] = 255; 
      m_piTFTextureDataAlphaOne[1024 + 4*i + 0] = iRed; 
      m_piTFTextureDataAlphaOne[1024 + 4*i + 1] = iGreen; 
      m_piTFTextureDataAlphaOne[1024 + 4*i + 2] = iBlue; 
      m_piTFTextureDataAlphaOne[1024 + 4*i + 3] = 255; 
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glBindTexture(GL_TEXTURE_2D, GetTFtextureIDalphaOne());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iSizeX, iSizeY,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_piTFTextureDataAlphaOne);
  }

  return;
} // UpdateTransferFunctionTexture()


/**
 * Create the texture for slice from volume
 */
void vxRendererTexMap2DMPRNew::CopyIntensityTextureToBuffer()
//*******************************************************************
{
  vxEnvironment & environment(GetEnvironment());
  const int4 iSliceNumber(environment.GetPlaneOrtho()->GetDistance());
  Vector3D<int4> volIndexDim(environment.GetIntensityVolume()->GetHeader().GetVolDim());
  Triple<float4> units(environment.GetIntensityVolume()->GetHeader().GetVoxelUnits());
  
  // get plane location, relative to the other viewers (offset)
  vxPlaneOrtho<float4> * pPlaneOrtho(environment.GetPlaneOrtho());
  const uint1 uViewAxis(pPlaneOrtho->GetNormal().GetMaxAbsComponentCoord()); // 0, 1, 2.
  const int4 iPlaneDistance(vxUtils::CalculateOffsetDistance(0.5 + pPlaneOrtho->GetAbsDistance()/units[uViewAxis],
                                                             volIndexDim[uViewAxis],
                                                             environment.GetOffset(),
                                                             environment.GetMaximumOffset()));

  // setup all the other parameters
  Vector3D<int4> iStartVoxel(0,0,0);
  iStartVoxel[uViewAxis] = iPlaneDistance;

  vxBlockVolumeIterator<uint2> volIter(*(environment.GetIntensityVolume()));
  volIter.SetPos(iStartVoxel);
  Triplei iTileSize(m_Tile.GetVolumeIndexSize());

  int4 iIndex(0);
  int4 iShift(iTileSize[(uViewAxis+1)%3]);
  int4 iWidth(volIndexDim[(uViewAxis+1)%3]);
  int4 iHeight(volIndexDim[(uViewAxis+2)%3]);

  switch(uViewAxis)
  {
  case 0: // Sagittal
    {
     for (int4 iIndexY(0); iIndexY<iHeight; iIndexY++)
     {
       for (int4 iIndexX(0); iIndexX<iWidth; iIndexX++, volIter.NextXZY())
       {
         m_pTextureIntensityBuffer[iIndex++] = (volIter.GetVoxel() >> 4);
       }
       iIndex = (iIndexY+1)*iShift;
     }
     break;
    }
  case 1: // Coronal
    {
     for (int4 iIndexY(0); iIndexY<iHeight; iIndexY++)
     {
       for (int4 iIndexX(0); iIndexX<iWidth; iIndexX++, volIter.NextYXZ())
       {
         m_pTextureIntensityBuffer[iIndex++] = (volIter.GetVoxel() >> 4);
       }
       iIndex = (iIndexY+1)*iShift;
     }
     break;
    }
  case 2: // Axial
    {
     for (int4 iIndexY(0); iIndexY<iHeight; iIndexY++)
     {
       for (int4 iIndexX(0); iIndexX<iWidth; iIndexX++, volIter.NextZYX())
       {
         m_pTextureIntensityBuffer[iIndex++] = (volIter.GetVoxel() >> 4);
       }
       iIndex = (iIndexY+1)*iShift;
     }
      break;
    }
  default:
    break; // should not happen
  }
} // CopyIntensityTextureToBuffer()


/**
 * Create the texture for slice from volume
 */
void vxRendererTexMap2DMPRNew::CopyLabelTextureToBuffer()
//*******************************************************************
{
  vxEnvironment & environment(GetEnvironment());
  const int4 iSliceNumber(environment.GetPlaneOrtho()->GetDistance());
  Vector3D<int4> volIndexDim(environment.GetIntensityVolume()->GetHeader().GetVolDim());
  Triple<float4> units(environment.GetIntensityVolume()->GetHeader().GetVoxelUnits());

  // get plane location, relative to the other viewers (offset)
  vxPlaneOrtho<float4> * pPlaneOrtho(environment.GetPlaneOrtho());
  const uint1 uViewAxis(pPlaneOrtho->GetNormal().GetMaxAbsComponentCoord()); // 0, 1, 2.
  const int4 iPlaneDistance(vxUtils::CalculateOffsetDistance(0.5 + pPlaneOrtho->GetAbsDistance()/units[uViewAxis],
                                                             volIndexDim[uViewAxis],
                                                             environment.GetOffset(),
                                                             environment.GetMaximumOffset()));

  // setup all the other parameters
  Vector3D<int4> iStartVoxel(0,0,0);
  iStartVoxel[uViewAxis] = iPlaneDistance;

  vxBlockVolumeIterator<uint2> volIter(*(environment.GetLabelVolume()));
  volIter.SetPos(iStartVoxel);
  Vector3D<int4> iTileSize(m_Tile.GetVolumeIndexSize());

  int4 iIndex(0);
  int4 iShift(iTileSize[(uViewAxis+1)%3]);
  int4 iWidth(volIndexDim[(uViewAxis+1)%3]);
  int4 iHeight(volIndexDim[(uViewAxis+2)%3]);

  switch(uViewAxis)
  {
  case 0: // Sagittal
    {
     for (int4 iIndexY(0); iIndexY<iHeight; iIndexY++)
     {
       for (int4 iIndexX(0) ; iIndexX<iWidth ; iIndexX++, volIter.NextXZY())
       {
         m_pTextureLabelBuffer[iIndex++] = volIter.GetVoxel();
       }
       iIndex = (iIndexY+1)*iShift;
     }
     break;
    }
  case 1: // Coronal
    {
     for (int4 iIndexY(0); iIndexY<iHeight; iIndexY++)
     {
       for (int4 iIndexX(0) ; iIndexX<iWidth ; iIndexX++, volIter.NextYXZ())
       {
         m_pTextureLabelBuffer[iIndex++] = volIter.GetVoxel();
       }
       iIndex = (iIndexY+1)*iShift;
     }
     break;
    }
  case 2: // Axial
    {
     for (int4 iIndexY(0); iIndexY<iHeight; iIndexY++)
     {
       for (int4 iIndexX(0) ; iIndexX<iWidth ; iIndexX++, volIter.NextZYX())
       {
         m_pTextureLabelBuffer[iIndex++] = volIter.GetVoxel();
       }
       iIndex = (iIndexY+1)*iShift;
     }
     break;
    }
  default:
    break;// should not happen
  }
} // CopyLabelTextureToBuffer()


/**
 * Set up tile statistics
 * @return success value
 */
void vxRendererTexMap2DMPRNew::GenerateTileInfo()
//*******************************************************************
{
  vxEnvironment & environment(GetEnvironment());

  // initialize the plane/tile parameters
  Triple<float4> units(environment.GetIntensityVolume()->GetHeader().GetVoxelUnits());
  Triple<int4> indexDim(environment.GetIntensityVolume()->GetHeader().GetVolDim());
  Vector<float4> tileOrigin(0,0,0);
  Vector<float4> tileNormal(0,0,0);
  Vector<uint2> volIdxOrigin(0,0,0);
  float4 fTexCoord1(1.0F), fTexCoord2(1.0F), fTexCoord3(1.0F);
  uint2  iTileIndexSize1(1), iTileIndexSize2(1), iTileIndexSize3(1);
  float4 fTextureBorderMargin(0.005F);
  Triple<int4> tileIndexSize(0,0,0);
  Vector<float4> texCoordUpperRight(1,1,1);
  Vector<float4> texSize(0,0,0);

  // get plane location, relative to the other viewers (offset)
  vxPlaneOrtho<float4> * pPlaneOrtho(environment.GetPlaneOrtho());
  const uint1 uViewAxis = pPlaneOrtho->GetNormal().GetMaxAbsComponentCoord(); // 0, 1, 2.
  const int4 iPlaneDistance(vxUtils::CalculateOffsetDistance(0.5 + pPlaneOrtho->GetAbsDistance()/units[uViewAxis],
                                                              indexDim[uViewAxis],
                                                              GetEnvironment().GetOffset(),
                                                              GetEnvironment().GetMaximumOffset()));

  // tile origin
  tileOrigin[uViewAxis] = iPlaneDistance*units[uViewAxis];
  m_Tile.SetOrigin(tileOrigin);
  
  // tile normal:
  tileNormal[uViewAxis] = 1;
  m_Tile.SetNormal(Normal<float4>(tileNormal));
  
  // volume index origin
  volIdxOrigin[uViewAxis] = iPlaneDistance;
  m_Tile.SetVolumeIndexOrigin(volIdxOrigin);
  iTileIndexSize2 = GetNextPowerOfTwo(indexDim[(uViewAxis+1)%3]);
  iTileIndexSize3 = GetNextPowerOfTwo(indexDim[(uViewAxis+2)%3]);

  // fix tiles which are partially outside the volume
  if (iTileIndexSize2 != indexDim[(uViewAxis+1)%3])
  {
    // fix the texture coordinate in i2-direction because 
    // the Tile is larger than the bit texture remaining
    fTexCoord2 = (float4) (indexDim[(uViewAxis+1)%3]-1) / (float4) (iTileIndexSize2-1); 
    fTexCoord2 -= fTextureBorderMargin; 
  }
  
  if (iTileIndexSize3 != indexDim[(uViewAxis+2)%3])
  {
    // fix the texture coordinate in i2-direction because 
    // the Tile is larger than the bit texture remaining
    fTexCoord3 = (float4) (indexDim[(uViewAxis+2)%3]-1) / (float4) (iTileIndexSize3-1); 
    fTexCoord3 -= fTextureBorderMargin; 
  }
  
  // tile index size:
  tileIndexSize[(uViewAxis+1)%3] = iTileIndexSize2;
  tileIndexSize[(uViewAxis+2)%3] = iTileIndexSize3;
  m_Tile.SetVolumeIndexSize(tileIndexSize);
  
  // texture coord., lower left:
  m_Tile.SetTexCoordLowerLeft(Vector<float4>(0.0, 0.0, 0.0));
  
  // texture coord., upper right
  texCoordUpperRight[(uViewAxis+1)%3] = fTexCoord2;
  texCoordUpperRight[(uViewAxis+2)%3] = fTexCoord3;
  m_Tile.SetTexCoordUpperRight(texCoordUpperRight); 
  
  // tile size in units
  texSize[(uViewAxis+1)%3] = max(1, (indexDim[(uViewAxis+1)%3]-1)) * units[(uViewAxis+1)%3];
  texSize[(uViewAxis+2)%3] = max(1, (indexDim[(uViewAxis+2)%3]-1)) * units[(uViewAxis+2)%3];
  m_Tile.SetSize(texSize);

  m_Tile.SetDensityTextureId(GetIntensityTextureID());
  m_Tile.SetLabelTextureId(GetLabelTextureID());
} // GenerateTileInfo()

  
/**
 * Set up textures for current slice from intensity volume
 * @return success value
 */
void vxRendererTexMap2DMPRNew::GenerateIntensityTexture()
//*******************************************************************
{
  vxEnvironment & environment(GetEnvironment());
  const uint1 uPlaneDir(environment.GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());
  const int4 iPlaneDistance(environment.GetPlaneOrtho()->GetDistance());

  Vector3D<int4> volDim(environment.GetIntensityVolume()->GetHeader().GetVolDim());
  int4 iDimXOfTextures0(GetNextPowerOfTwo(volDim.Y()));
  int4 iDimYOfTextures0(GetNextPowerOfTwo(volDim.Z()));

  int4 iDimXOfTextures1(GetNextPowerOfTwo(volDim.Z()));
  int4 iDimYOfTextures1(GetNextPowerOfTwo(volDim.X()));
  
  int4 iDimXOfTextures2(GetNextPowerOfTwo(volDim.X()));
  int4 iDimYOfTextures2(GetNextPowerOfTwo(volDim.Y()));

  const int4 iSizeOfTextures0(m_Tile.GetVolumeIndexSize().Y() * m_Tile.GetVolumeIndexSize().Z());
  const int4 iSizeOfTextures1(m_Tile.GetVolumeIndexSize().Z() * m_Tile.GetVolumeIndexSize().X());
  const int4 iSizeOfTextures2(m_Tile.GetVolumeIndexSize().X() * m_Tile.GetVolumeIndexSize().Y());
 
  switch (uPlaneDir)
  {
  case 0:
    // Xzy order
    m_pTextureIntensityBuffer = new GLubyte[iSizeOfTextures0]; 
    memset(m_pTextureIntensityBuffer, environment.GetIntensityVolume()->GetHeader().GetBackground(), iSizeOfTextures0);
    CopyIntensityTextureToBuffer();
    glBindTexture(GL_TEXTURE_2D, GetIntensityTextureID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, iDimXOfTextures0, iDimYOfTextures0 , 0, GL_ALPHA, GL_UNSIGNED_BYTE, m_pTextureIntensityBuffer);
    delete [] m_pTextureIntensityBuffer;
    break;
  case 1:
    // Yxz order
    m_pTextureIntensityBuffer = new GLubyte[iSizeOfTextures1]; 
    memset(m_pTextureIntensityBuffer, environment.GetIntensityVolume()->GetHeader().GetBackground(), iSizeOfTextures1);
    CopyIntensityTextureToBuffer();
    glBindTexture(GL_TEXTURE_2D, GetIntensityTextureID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, iDimXOfTextures1, iDimYOfTextures1 , 0, GL_ALPHA, GL_UNSIGNED_BYTE, m_pTextureIntensityBuffer);
    delete [] m_pTextureIntensityBuffer;
    break;
  case 2:
    // Zyx order 
    m_pTextureIntensityBuffer = new GLubyte[iSizeOfTextures2]; 
    memset(m_pTextureIntensityBuffer, environment.GetIntensityVolume()->GetHeader().GetBackground(), iSizeOfTextures2);
    CopyIntensityTextureToBuffer();
    glBindTexture(GL_TEXTURE_2D, GetIntensityTextureID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, iDimXOfTextures2, iDimYOfTextures2 , 0, GL_ALPHA, GL_UNSIGNED_BYTE, m_pTextureIntensityBuffer);
    delete [] m_pTextureIntensityBuffer;
    break;
  }
} // GenerateIntensityTexture()



/**
 * Set up textures for current slice from intensity volume
 * @return success value
 */
void vxRendererTexMap2DMPRNew::GenerateLabelTexture()
//*******************************************************************
{
  vxEnvironment & environment(GetEnvironment());
  const uint1 uPlaneDir(environment.GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());
  const int4 iPlaneDistance(environment.GetPlaneOrtho()->GetDistance());

  Vector3D<int4> volDim(environment.GetIntensityVolume()->GetHeader().GetVolDim());
  int4 iDimXOfTextures0(GetNextPowerOfTwo(volDim.Y()));
  int4 iDimYOfTextures0(GetNextPowerOfTwo(volDim.Z()));

  int4 iDimXOfTextures1(GetNextPowerOfTwo(volDim.Z()));
  int4 iDimYOfTextures1(GetNextPowerOfTwo(volDim.X()));
  
  int4 iDimXOfTextures2(GetNextPowerOfTwo(volDim.X()));
  int4 iDimYOfTextures2(GetNextPowerOfTwo(volDim.Y()));

  const int4 iSizeOfTextures0(m_Tile.GetVolumeIndexSize().Y() * m_Tile.GetVolumeIndexSize().Z());
  const int4 iSizeOfTextures1(m_Tile.GetVolumeIndexSize().Z() * m_Tile.GetVolumeIndexSize().X());
  const int4 iSizeOfTextures2(m_Tile.GetVolumeIndexSize().X() * m_Tile.GetVolumeIndexSize().Y());
 
  switch (uPlaneDir)
  {
  case 0:
    // Xzy order
    m_pTextureLabelBuffer = new GLubyte[iSizeOfTextures0]; 
    memset(m_pTextureLabelBuffer, environment.GetLabelVolume()->GetHeader().GetBackground(),iSizeOfTextures0);
    CopyLabelTextureToBuffer();
    glBindTexture(GL_TEXTURE_2D, GetLabelTextureID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, iDimXOfTextures0, iDimYOfTextures0 , 0, GL_ALPHA, GL_UNSIGNED_BYTE, m_pTextureLabelBuffer);      
    delete [] m_pTextureLabelBuffer;
    break;
  case 1:
    // Yxz order
    m_pTextureLabelBuffer = new GLubyte[iSizeOfTextures1]; 
    memset(m_pTextureLabelBuffer, environment.GetLabelVolume()->GetHeader().GetBackground(),iSizeOfTextures1);
    CopyLabelTextureToBuffer();
    glBindTexture(GL_TEXTURE_2D, GetLabelTextureID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, iDimXOfTextures1, iDimYOfTextures1 , 0, GL_ALPHA, GL_UNSIGNED_BYTE, m_pTextureLabelBuffer);      
    delete [] m_pTextureLabelBuffer;
    break;
  case 2:
    // Zyx order 
    m_pTextureLabelBuffer = new GLubyte[iSizeOfTextures2]; 
    memset(m_pTextureLabelBuffer, environment.GetLabelVolume()->GetHeader().GetBackground(),iSizeOfTextures2);
    CopyLabelTextureToBuffer();
    glBindTexture(GL_TEXTURE_2D, GetLabelTextureID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, iDimXOfTextures2, iDimYOfTextures2 , 0, GL_ALPHA, GL_UNSIGNED_BYTE, m_pTextureLabelBuffer);      
    delete [] m_pTextureLabelBuffer;
    break;
  }
} // GenerateLabelTexture()

 
/**
 * Private function that uses openGL to draw a tile with texture
 * @param pointer to texture tile 2D object
 * @param index of axis
 */
void vxRendererTexMap2DMPRNew::GLDrawTexturedTile(const vxTexMap2dTile * pTile, 
                                                  const uint1 & uIndex)
//*******************************************************************
{
  // GL draw textured tile
  glBegin(GL_TRIANGLE_FAN);

  for (int i(0); i<4; i++)
  {
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, pTile->GetVertexTexCoord(i)[(uIndex+1)%3], pTile->GetVertexTexCoord(i)[(uIndex+2)%3]);
    if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_COMPONENT_IN_MPR))
    {
      glMultiTexCoord2fARB(GL_TEXTURE2_ARB, pTile->GetVertexTexCoord(i)[(uIndex+1)%3], pTile->GetVertexTexCoord(i)[(uIndex+2)%3]);
    }
    glVertex3f(pTile->GetVertex(i)[0], pTile->GetVertex(i)[1], pTile->GetVertex(i)[2]);
  }
  glEnd();
} // GLDrawTexturedTile()


/**
 * Private function that uses openGL to draw a tile without texture
 * @param pointer to a texture tile 2D object
 * @param index of axis
 */
void vxRendererTexMap2DMPRNew::GLDrawUntexturedTile(const vxTexMap2dTile * pTile, 
                                                    const uint1 & uIndex)
//*******************************************************************
{
  // draw untextured tile:
  glDisable(GL_TEXTURE_2D);
  glColor3f(1,1,1);
  glBegin(GL_TRIANGLE_FAN);
  for (int i(0); i<4; i++)
  {
    glVertex3f(pTile->GetVertex(i)[0], pTile->GetVertex(i)[1], pTile->GetVertex(i)[2]);
  }
  glEnd();

  glColor3f(1,1,0);
  glLineWidth(1);
  glBegin(GL_LINE_LOOP);
  for (int i(0); i<4; i++)
  {
    glVertex3f(pTile->GetVertex(i)[0], pTile->GetVertex(i)[1], pTile->GetVertex(i)[2]);
  }
  glEnd();
} // GLDrawUntexturedTile()


/**
 * Public function to render a single tile
 * @param pointer to texture tile 2D object
 * @param index of axis
 * @param texture ID 1
 * @param texture ID 2
 * @param flag to turn tecxture on/off
 */
void vxRendererTexMap2DMPRNew::RenderTile(const vxTexMap2dTile * pTile, 
                                          const uint1 & uIndex, 
                                          GLuint id0, 
                                          GLuint id1, 
                                          bool bTexOn)
//*******************************************************************
{
  if (bTexOn && GetTexCombEnabledFlag())
  {
    // Setup tile texture:
    SetupTileTexture(pTile->GetDensityTextureId(), id0, pTile->GetLabelTextureId(), id1);

    GLDrawTexturedTile(pTile, uIndex);

    if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE) == true)
    {
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(0.0, -1);

      glDepthFunc(GL_LESS);

      // Setup tile texture:
      SetupTileTextureIntensityRange(pTile);
      GLDrawTexturedTile(pTile, uIndex);

      glDisable(GL_POLYGON_OFFSET_FILL);
    }
  }
  else // Draw tile without texture...
  {
    GLDrawUntexturedTile(pTile, uIndex);
  }
} // RenderTile()



/**
 * Private method to setup the texture units and register combiners for the current tile
 * @param pointer to a texture map 2D tile
 */
void vxRendererTexMap2DMPRNew::SetupTileTextureIntensityRange(const vxTexMap2dTile * pTile)
//*******************************************************************
{
  if (GetRenderMode() == DENSITY_TAG_POST_CLASSIFY)
  {
    // texture shader setup
    /////////////////////////
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_2D, pTile->GetDensityTextureId());
    glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glBindTexture(GL_TEXTURE_2D, GetIntensityRangeTextureID());
    glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_AR_TEXTURE_2D_NV);
    glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_COMPONENT_IN_MPR))
    {
      glActiveTextureARB(GL_TEXTURE2_ARB);
      glBindTexture(GL_TEXTURE_2D, GetDummyTextureID());
      glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      glActiveTextureARB(GL_TEXTURE3_ARB);
      glBindTexture(GL_TEXTURE_2D, GetDummyTextureCombID());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_AR_TEXTURE_2D_NV);
      glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE2_ARB);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
  }
} // SetupTileTextureIntensityRange()


/**
 * Private method to setup the texture units and register combiners for the current tile
 * @param texture ID 0 (intensity texture)
 * @param texture ID 1 (intensity lookup texture)
 * @param texture ID 2 (label texture)
 * @param texture ID 3 (label lookup texture)
 */
void vxRendererTexMap2DMPRNew::SetupTileTexture(const GLuint & Id0, 
                                                const GLuint & Id1, 
                                                const GLuint & Id2, 
                                                const GLuint & Id3)
//*******************************************************************
{
  if (GetRenderMode() == DENSITY_TAG_POST_CLASSIFY)
  {
    // texture shader setup
    /////////////////////////
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_2D, Id0);
    glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glBindTexture(GL_TEXTURE_2D, Id1);
    glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_AR_TEXTURE_2D_NV);
    glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_COMPONENT_IN_MPR))
    {
      glActiveTextureARB(GL_TEXTURE2_ARB);
      glBindTexture(GL_TEXTURE_2D, Id2);
      glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      glActiveTextureARB(GL_TEXTURE3_ARB);
      glBindTexture(GL_TEXTURE_2D, Id3);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_AR_TEXTURE_2D_NV);
      glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE2_ARB);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
  }
} // SetupTileTexture()


/**
 * Set up textures for TF and components
 * @return success value
 */
int4 vxRendererTexMap2DMPRNew::PreRender()
//*******************************************************************
{
  vxRendererTexMap::PreRender();

  bool bRender(false); 
  vxEnvironment & environment(GetEnvironment());

  if (environment.GetIntensityVolume() != NULL)
  {
    if (environment.GetLabelVolume() != NULL)
    {
      bRender = true;
    }
  }
  
  // if plane is changed, update label and intensity textures
  if (environment.IsModified(vxEnvironment::PLANE_MPR))
  {
    GenerateTileInfo();
    GenerateIntensityTexture();
    GenerateLabelTexture();
  }
  else
  {
    // update intensity texture
    if (environment.IsModified(vxEnvironment::VOLUME_INTENSITY))
    {
      GenerateTileInfo();
      GenerateIntensityTexture();
    }
    // update label texture
    if (environment.IsModified(vxEnvironment::VOLUME_LABEL))
    {
      GenerateTileInfo();
      GenerateLabelTexture();
    }
  }

  if (GetEnvironment().IsModified(vxEnvironment::COLORMAP) || GetEnvironment().IsModified(vxEnvironment::RENDERING_MODE))
  {
    UpdateTransferFunctionTexture();
  }

  SetRenderingNecessary(GetRenderingNecessary() && bRender);
  
  return 1;
} // PreRender()


/**
 * Public method to render the slice/tile
 * @return success/fail flag
 */
void vxRendererTexMap2DMPRNew::Render()
//*******************************************************************
{
  int4 iFlag(0);

  if (GetRenderingNecessary() == true)
  {
    uint1 uViewAxis(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());

    // restore the OpenGL state
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    SetupGLTextureCombiners();
    glEnable(GL_DEPTH_TEST);

    RenderTile(&m_Tile, uViewAxis, GetTFtextureIDalphaOne(), GetComponentTextureID());
    glDisable(GL_DEPTH_TEST);
    DisableGLTextureCombiners();

    iFlag = 1;

    // restore the OpenGL state
    glPopAttrib();
  }
} // Render()


#undef FILE_REVISION


// $Log: vxRendererTexMap2DMPRNew.C,v $
// Revision 1.4  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.12.1  2005/07/19 11:50:33  frank
// missing include
//
// Revision 1.2  2004/04/06 16:27:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.26  2004/02/10 23:02:12  michael
// added color in MPR display to the texture mapping based renderer
//
// Revision 1.25  2004/01/05 19:03:03  michael
// added "mailto:"
//
// Revision 1.24  2003/09/04 17:29:45  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.23  2003/06/18 15:20:00  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.22  2003/06/09 12:30:24  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.21  2003/05/19 14:46:49  frank
// Removed unused return parameter
//
// Revision 1.20  2003/05/17 20:12:32  michael
// fixed bug introduced during code review
//
// Revision 1.19  2003/05/17 19:15:18  michael
// more code review
//
// Revision 1.18  2003/05/17 15:44:51  michael
// reformatting
//
// Revision 1.17  2003/05/16 15:34:08  dongqing
// code review
//
// Revision 1.16  2003/05/16 13:58:18  michael
// code review
//
// Revision 1.15  2003/05/16 13:22:15  frank
// formatting
//
// Revision 1.14  2003/01/22 22:04:05  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.13  2001/01/02 15:29:18  ingmar
// float8 -> float4 (no need to render with more than 23bits precision)
//
// Revision 1.12  2002/12/06 19:20:25  ingmar
// vxRenderingMode::SHOW_LABEL -> SHOW_COMPONENT_IN_MPR
//
// Revision 1.11  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.10  2002/11/12 00:15:19  michael
// accounted for rounding error when converting world plane distance to volume index coordinates
//
// Revision 1.9  2002/11/11 19:31:39  michael
// more fixing of texture map server-less rendering
//
// Revision 1.8  2002/11/10 23:05:38  michael
// This renderer now directly inherits from vxRendererTexMap and does not
// require any texture server
//
// Revision 1.7  2002/09/09 22:47:43  michael
// Added author and removed redundant code
//
// Revision 1.6  2002/07/30 15:07:51  uday
// Renamed functions.
//
// Revision 1.5  2002/07/29 17:03:16  uday
// Seperated Intensity and label texture generations.
//
// Revision 1.4  2002/07/26 23:17:08  michael
// commented out missing filter files
//
// Revision 1.3  2002/07/26 19:40:50  uday
// update GenerateTileInfo().
//
// Revision 1.2  2002/07/26 18:30:46  michael
// Fixed new texture map based renderers
//
// Revision 1.1  2002/07/26 17:09:21  uday
// Initial Version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererTexMap2DMPRNew.C,v 1.4 2006/01/18 20:39:26 geconomos Exp $
// $Id: vxRendererTexMap2DMPRNew.C,v 1.4 2006/01/18 20:39:26 geconomos Exp $
