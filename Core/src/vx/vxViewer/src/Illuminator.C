// $Id: Illuminator.C,v 1.2 2004/03/05 16:37:30 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// includes
#include "stdafx.h"
#include "Illuminator.h"
#include "Image.h"
#include "Point2D.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


// namespaces
using namespace std;


/**
 * Default constructor
 */
Illuminator::Illuminator() :
m_fAmbient(0.5f),
m_fDiffuse(0.3f),
m_fSpecular(0.2f),
m_fPhongExponent(50.0f),
m_fEdgeScale(0.0),
m_iEdgeLength(0),
m_iStepV(0),
m_iStepMap(0),
m_iMapSize(0),
m_viewDirection(-1.0f,-1.0f,-1.0f),
m_iNumberOfLightSources(0)
{
}


/**
 * Sets the number of light sources
 *
 * @param number of light sources
 */
void Illuminator::SetNumberOfLightSources(const int4 iNumberOfLightSources)
{
  m_iNumberOfLightSources = iNumberOfLightSources;
  m_vLightSources.resize(m_iNumberOfLightSources);
  m_vbIsLightSourceSpecularActive.resize(m_iNumberOfLightSources, false);
  m_vbIsLightSourceDiffuseActive.resize(m_iNumberOfLightSources, false);
} // SetNumberOfLightSources()


/**
 * Set an individual light source
 *
 * @param light source index
 * @param diffuse portion
 * @param specular portion
 */
void Illuminator::SetLightSource(const int4 iWhichLight,
                                 const Vector3D<float4> & fLight, 
                                 const bool & bDiffusePortionActive, 
                                 const bool & bSpecularPortionActive)
{
  if (iWhichLight < m_iNumberOfLightSources)
  {
    m_vLightSources[iWhichLight] = fLight;
    m_vbIsLightSourceSpecularActive[iWhichLight] = bSpecularPortionActive;
    m_vbIsLightSourceDiffuseActive[iWhichLight] = bDiffusePortionActive;
    CreateReflectionMapDiffuse();
  }
} // SetLightSource()


/**
 * Enable specular light
 *
 * @param index of light source
 * @param true/false
 */
void Illuminator::EnableSpecularPortionOfLight(const int4 iWhichLight, const bool & bFlag)
{
  if (iWhichLight < m_iNumberOfLightSources) 
  {
    m_vbIsLightSourceSpecularActive[iWhichLight] = bFlag;
  }
} // EnableSpecularPortionOfLight()


/**
 * Enable diffuse light
 *
 * @param index of light source
 * @param true/false
 */
void Illuminator::EnableDiffusePortionOfLight(const int4 iWhichLight, const bool & bFlag)
{
  if (iWhichLight < m_iNumberOfLightSources) 
  {
    m_vbIsLightSourceDiffuseActive[iWhichLight] = bFlag;
  }
} // EnableDiffusePortionOfLight()


/**
 * Enable light source
 *
 * @param index of light source
 * @param true/false
 */
void Illuminator::EnableLight(const int4 iWhichLight, const bool & bFlag)
{
  if (iWhichLight < m_iNumberOfLightSources) 
  {
    m_vbIsLightSourceDiffuseActive[iWhichLight] = m_vbIsLightSourceSpecularActive[iWhichLight] = bFlag;
  }
} // EnableLight()


/**
 * Set the phong illumination parameters
 *
 * @param ambient factor
 * @param diffuse factor
 * @param specular factor
 * @param phong exponent
 */
void Illuminator::SetPhongConstants(const float4 fAmbient, 
                                    const float4 fDiffuse, 
                                    const float4 fSpecular,
                                    const float4 fPhongExponent)
{
  m_fAmbient  = fAmbient;
  m_fDiffuse  = fDiffuse;
  m_fSpecular = fSpecular;
  m_fPhongExponent = fPhongExponent;

  return;
} // SetPhongConstants()


/**
 * Set resolution of maps
 *
 * @param edge lenght (discrete resolution)
 */
void Illuminator::SetEdgeLengthOfMaps(int4 iEdgeLength)
{
  if (iEdgeLength != m_iEdgeLength)
  {
    m_iEdgeLength = iEdgeLength;
    m_fEdgeScale  = m_iEdgeLength / 2.0;
    m_iStepV   = m_iEdgeLength + 1;
    m_iStepMap = m_iStepV * m_iStepV;
    m_iMapSize = 6 * (m_iEdgeLength + 1) * (m_iEdgeLength + 1);
    CreateNormals();
  }

  return;
} // SetEdgeLengthOfMaps()


/**
 *	Set the actual view direction
 *
 * @param view direction vector
 */
void Illuminator::SetViewDirection(const Vector3D<float4> & viewDirection)
{
  if (m_viewDirection != viewDirection)
  {
    m_viewDirection = viewDirection;
    CreateReflectionMapSpecular();
  }

  return;
} // SetViewDirection()


/**
 * Get specular lighting effect 
 *
 * @param gradient/normal
 * @return diffuse factor
 */
float4 Illuminator::LookUpInReflectionMapSpecular(const Vector3D<float4> & normal)
{
  // Find maximum component
  int4 iMaxComponent(normal.GetMaxAbsComponentCoord());

  // get corresponding reflectance cube face
  float4 * pMap;
  if (normal[iMaxComponent] < 0)
  {
    pMap = &m_vfMapSpecular[2*iMaxComponent*m_iStepMap];
  }
  else
  {
    pMap = &m_vfMapSpecular[(1 + 2*iMaxComponent)*m_iStepMap];
  }

  // get lower left corner of matching tile
  float4 fDiff(fabs(normal[iMaxComponent]));
  if (fDiff == 0) 
  {
    fDiff = 100.0f;
  }
  float4 fU((1+(normal[(iMaxComponent+1)%3] / fDiff))*m_fEdgeScale);
  float4 fV((1+(normal[(iMaxComponent+2)%3] / fDiff))*m_fEdgeScale);
  int4 iU(fU);
  int4 iV(fV);
  pMap += iU + iV*m_iStepV;

  // bilinearly interpolate to final lookup value
  // c--d
  // |  |
  // a--b
  const float4 fA(*pMap++);
  const float4 fB(*pMap);
  pMap += m_iStepV;
  const float4 fD(*pMap--);
  const float4 fC(*pMap);

  const float4 fWeightU(fU - iU);
  const float4 fWeightV(fV - iV);

  const float4 fAB(fA + (fB-fA)*fWeightU);
  const float4 fCD(fC + (fD-fC)*fWeightU);

  const float4 fABCD(fAB + (fCD-fAB)*fWeightV);

  return fABCD;
} // LookUpInReflectinMapSpecular()


/**
 * Get diffuse lighting effect 
 *
 * @param gradient/normal
 * @return diffuse factor
 */
float4 Illuminator::LookUpInReflectionMapDiffuse(const Vector3D<float4> & normal)
{
  // Find maximum component
  int4 iMaxComponent(normal.GetMaxComponentCoord());

  // get corresponding reflectance cube face
  float4 * pMap;
  if (normal[iMaxComponent] < 0)
  {
    pMap = &m_vfMapDiffuse[2*iMaxComponent*m_iStepMap];
  }
  else
  {
    pMap = &m_vfMapDiffuse[(1 + 2*iMaxComponent)*m_iStepMap];
  }

  // get lower left corner of matching tile
  float4 fDiff(fabs(normal[iMaxComponent]));
  if (fDiff == 0)
  {
    fDiff = 100.0f;
  }
  float4 fU((1+(normal[(iMaxComponent+1)%3] / fDiff))*m_fEdgeScale);
  float4 fV((1+(normal[(iMaxComponent+2)%3] / fDiff))*m_fEdgeScale);
  int4 iU(fU);
  int4 iV(fV);
  pMap += iU + iV*m_iStepV;

  // bilinearly interpolate to final lookup value
  // c--d
  // |  |
  // a--b
  const float4 fA(*pMap++);
  const float4 fB(*pMap); 
  pMap += m_iStepV;
  const float4 fD(*pMap--);
  const float4 fC(*pMap);

  const float4 fWeightU(fU - iU);
  const float4 fWeightV(fV - iV);

  const float4 fAB(fA + (fB-fA)*fWeightU);
  const float4 fCD(fC + (fD-fC)*fWeightU);

  const float4 fABCD(fAB + (fCD-fAB)*fWeightV);

  return fABCD;
} // LookUpInReflectionMapDiffuse()


/**
 * Create normals
 */
void Illuminator::CreateNormals()
{
  m_vNormal.resize(m_iMapSize);
  vector<Vector3D<float4> >::iterator iterNormal = m_vNormal.begin();
  int4 u, v;
  for (int4 k(0); k<3; ++k)  // first x as max component, then y, then z
  {
    for (v = 0; v<=m_iEdgeLength; ++v)
    {
      for (u = 0; u<=m_iEdgeLength; ++u)
      {
        (*iterNormal)[(k+0)%3] = -1;
        (*iterNormal)[(k+1)%3] = -1 + ((2.0*u)/m_iEdgeLength);
        (*iterNormal)[(k+2)%3] = -1 + ((2.0*v)/m_iEdgeLength);
        (*iterNormal).Normalize();
        ++iterNormal;
      }
    } // end for all v

    for (v = 0; v<=m_iEdgeLength; ++v)
    {
      for (u = 0; u<=m_iEdgeLength; ++u)
      {
        (*iterNormal)[(k+0)%3] = +1;
        (*iterNormal)[(k+1)%3] = -1 + ((2.0*u)/m_iEdgeLength);
        (*iterNormal)[(k+2)%3] = -1 + ((2.0*v)/m_iEdgeLength);
        (*iterNormal).Normalize();
        ++iterNormal;
      }
    } // end for all v
  } // end for all k (x-axis)
} // CreateNormals()


/**
 * Private method to get diffuse lighting
 *
 * @param gradient/normal
 * @return diffuse factor
 */
float4 Illuminator::GetDiffuse(const Vector3D<float4> & normal)
{
  float4 fDiffuse(0.0f);
  for (int4 i(m_iNumberOfLightSources-1); i >= 0; --i)
  {
    if (m_vbIsLightSourceDiffuseActive[i])
    {
      float4 fTheta = normal.Dot(m_vLightSources[i]);
      fTheta = fTheta * fTheta;
      fDiffuse += fTheta * m_fDiffuse;
    }
  }

  return(fDiffuse);
} // GetDiffuse


/**
 * Private method to get specular lighting
 *
 * @param gradient/normal
 * @return specular factor
 */
float4 Illuminator::GetSpecular(const Vector3D<float4> & normal)
{
  float4 fSpecular(0.0f); 
  for (int4 i(m_iNumberOfLightSources-1); i >= 0; --i)
  {
    if (m_vbIsLightSourceSpecularActive[i])
    {
      float4 fTmp(2 * normal.Dot(m_viewDirection));

      Vector3D<float4> fReflectionVec = (normal * fTmp) - m_viewDirection;
      fTmp = fReflectionVec.Dot(m_vLightSources[i]);
      if (fTmp > 0.5) 
      {
        fSpecular += pow((float4)fTmp, m_fPhongExponent) * m_fSpecular;
      }
    }
  }
  return fSpecular;
} // GetSpecular()



/**
 * Create diffuse reflectance map
 */
void Illuminator::CreateReflectionMapDiffuse()
{
  if (m_vfMapDiffuse.size() != m_iMapSize) 
  {
    m_vfMapDiffuse.resize(m_iMapSize);
  }
  for (int4 k(m_iMapSize-1); k>=0; --k)
  {
    m_vfMapDiffuse[k] = GetDiffuse(m_vNormal[k]);
  }

  return;
} // CreateReflectionMapDiffuse()


/**
 * Create specular reflectance map
 */
void Illuminator::CreateReflectionMapSpecular()
{
  if (m_vfMapSpecular.size() != m_iMapSize) 
  {
    m_vfMapSpecular.resize(m_iMapSize);
  }
  for (int4 k(m_iMapSize-1); k>=0; --k)
  {
    m_vfMapSpecular[k] = GetSpecular(m_vNormal[k]);
  }

  return;
} // CreateReflectionMapSpecular()


/**
 * Write the diffuse reflectance map to file
 */
void Illuminator::WriteToDiskReflectionMapDiffuse()
{
  int4 iTileEdgeLength(m_iStepV+2);
  Image<PixelRGB<uint1> > image(PixelRGB<uint1>(0,0,0));
  image.Resize(Triple<uint4>(iTileEdgeLength*3, iTileEdgeLength*2,0));

  vector<Vector3D<float4> >::iterator iterNormal= m_vNormal.begin();
  float4 * pMap;
  Vector3D<float4> * pVec;
  int4 u, v;
  int4 x, y;

  pMap = &m_vfMapDiffuse[0*m_iStepMap]; x=0*(iTileEdgeLength); y=0*(iTileEdgeLength); for (v = 0; v<=m_iEdgeLength; ++v)  for (u = 0; u<=m_iEdgeLength; ++u)  image.SetPixel(Point2D<uint2>(x+u, y+v), PixelRGB<uint1>(255 * pMap[u + v*m_iStepV], 255 * pMap[u + v*m_iStepV], 255 * pMap[u + v*m_iStepV])); 
  pMap = &m_vfMapDiffuse[1*m_iStepMap]; x=0*(iTileEdgeLength); y=1*(iTileEdgeLength); for (v = 0; v<=m_iEdgeLength; ++v)  for (u = 0; u<=m_iEdgeLength; ++u)  image.SetPixel(Point2D<uint2>(x+u, y+v), PixelRGB<uint1>(255 * pMap[u + v*m_iStepV], 255 * pMap[u + v*m_iStepV], 255 * pMap[u + v*m_iStepV])); 
  pMap = &m_vfMapDiffuse[2*m_iStepMap]; x=1*(iTileEdgeLength); y=0*(iTileEdgeLength); for (v = 0; v<=m_iEdgeLength; ++v)  for (u = 0; u<=m_iEdgeLength; ++u)  image.SetPixel(Point2D<uint2>(x+u, y+v), PixelRGB<uint1>(255 * pMap[u + v*m_iStepV], 255 * pMap[u + v*m_iStepV], 255 * pMap[u + v*m_iStepV])); 
  pMap = &m_vfMapDiffuse[3*m_iStepMap]; x=1*(iTileEdgeLength); y=1*(iTileEdgeLength); for (v = 0; v<=m_iEdgeLength; ++v)  for (u = 0; u<=m_iEdgeLength; ++u)  image.SetPixel(Point2D<uint2>(x+u, y+v), PixelRGB<uint1>(255 * pMap[u + v*m_iStepV], 255 * pMap[u + v*m_iStepV], 255 * pMap[u + v*m_iStepV])); 
  pMap = &m_vfMapDiffuse[4*m_iStepMap]; x=2*(iTileEdgeLength); y=0*(iTileEdgeLength); for (v = 0; v<=m_iEdgeLength; ++v)  for (u = 0; u<=m_iEdgeLength; ++u)  image.SetPixel(Point2D<uint2>(x+u, y+v), PixelRGB<uint1>(255 * pMap[u + v*m_iStepV], 255 * pMap[u + v*m_iStepV], 255 * pMap[u + v*m_iStepV])); 
  pMap = &m_vfMapDiffuse[5*m_iStepMap]; x=2*(iTileEdgeLength); y=1*(iTileEdgeLength); for (v = 0; v<=m_iEdgeLength; ++v)  for (u = 0; u<=m_iEdgeLength; ++u)  image.SetPixel(Point2D<uint2>(x+u, y+v), PixelRGB<uint1>(255 * pMap[u + v*m_iStepV], 255 * pMap[u + v*m_iStepV], 255 * pMap[u + v*m_iStepV])); 
  image.Write("D:/ds/refMap.ppm"); // hard coded 

  pVec = &m_vNormal[0*m_iStepMap]; x=0*(iTileEdgeLength); y=0*(iTileEdgeLength); for (v = 0; v<=m_iEdgeLength; ++v)  for (u = 0; u<=m_iEdgeLength; ++u)  image.SetPixel(Point2D<uint2>(x+u, y+v), PixelRGB<uint1>(255 * fabs(pVec[u + v*m_iStepV].X()), 255 * fabs(pVec[u + v*m_iStepV].Y()), 255 * fabs(pVec[u + v*m_iStepV].Z()))); 
  pVec = &m_vNormal[1*m_iStepMap]; x=0*(iTileEdgeLength); y=1*(iTileEdgeLength); for (v = 0; v<=m_iEdgeLength; ++v)  for (u = 0; u<=m_iEdgeLength; ++u)  image.SetPixel(Point2D<uint2>(x+u, y+v), PixelRGB<uint1>(255 * fabs(pVec[u + v*m_iStepV].X()), 255 * fabs(pVec[u + v*m_iStepV].Y()), 255 * fabs(pVec[u + v*m_iStepV].Z()))); 
  pVec = &m_vNormal[2*m_iStepMap]; x=1*(iTileEdgeLength); y=0*(iTileEdgeLength); for (v = 0; v<=m_iEdgeLength; ++v)  for (u = 0; u<=m_iEdgeLength; ++u)  image.SetPixel(Point2D<uint2>(x+u, y+v), PixelRGB<uint1>(255 * fabs(pVec[u + v*m_iStepV].X()), 255 * fabs(pVec[u + v*m_iStepV].Y()), 255 * fabs(pVec[u + v*m_iStepV].Z()))); 
  pVec = &m_vNormal[3*m_iStepMap]; x=1*(iTileEdgeLength); y=1*(iTileEdgeLength); for (v = 0; v<=m_iEdgeLength; ++v)  for (u = 0; u<=m_iEdgeLength; ++u)  image.SetPixel(Point2D<uint2>(x+u, y+v), PixelRGB<uint1>(255 * fabs(pVec[u + v*m_iStepV].X()), 255 * fabs(pVec[u + v*m_iStepV].Y()), 255 * fabs(pVec[u + v*m_iStepV].Z()))); 
  pVec = &m_vNormal[4*m_iStepMap]; x=2*(iTileEdgeLength); y=0*(iTileEdgeLength); for (v = 0; v<=m_iEdgeLength; ++v)  for (u = 0; u<=m_iEdgeLength; ++u)  image.SetPixel(Point2D<uint2>(x+u, y+v), PixelRGB<uint1>(255 * fabs(pVec[u + v*m_iStepV].X()), 255 * fabs(pVec[u + v*m_iStepV].Y()), 255 * fabs(pVec[u + v*m_iStepV].Z()))); 
  pVec = &m_vNormal[5*m_iStepMap]; x=2*(iTileEdgeLength); y=1*(iTileEdgeLength); for (v = 0; v<=m_iEdgeLength; ++v)  for (u = 0; u<=m_iEdgeLength; ++u)  image.SetPixel(Point2D<uint2>(x+u, y+v), PixelRGB<uint1>(255 * fabs(pVec[u + v*m_iStepV].X()), 255 * fabs(pVec[u + v*m_iStepV].Y()), 255 * fabs(pVec[u + v*m_iStepV].Z()))); 
  image.Write("D:/ds/refMapVectors.ppm"); // hard coded

  return;
} // WriteToDiskReflectionMapDiffuse()


#undef FILE_REVISION


// $Log: Illuminator.C,v $
// Revision 1.2  2004/03/05 16:37:30  geconomos
// Updated to compile with 7.1 compiler.
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.18  2003/04/23 18:52:03  frank
// Code review
//
// Revision 1.17  2003/04/15 21:39:38  michael
// performed more code review
//
// Revision 1.16  2003/04/14 21:06:03  michael
// performed initial code review
//
// Revision 1.15  2003/01/22 21:41:34  ingmar
// synchronized to new Point2D access to Image pixels
//
// Revision 1.14  2002/12/20 19:01:15  ingmar
// removed single scalar constructor from PixelRGBA
//
// Revision 1.13  2002/09/10 15:51:45  dmitry
// Nevermind, the last change was not needed.
//
// Revision 1.12  2002/09/10 15:17:17  dmitry
// Write on an image only works with the new compilers because of template specialization.
//
// Revision 1.11  2002/06/21 17:07:27  ingmar
// seperated ShadePhongTwoLights() from ShadePhongReflectanceMap()
//
// Revision 1.10  2002/06/20 20:28:02  ingmar
// fixed GetSpecular()
//
// Revision 1.9  2002/06/20 20:05:14  soeren
// disbaled temporaly specular highlight
//
// Revision 1.8  2002/06/20 18:39:53  soeren
// extended Illuminator to have n lights instead of two
//
// Revision 1.7  2002/06/20 16:04:29  soeren
// added lookuo for specular
//
// Revision 1.6  2002/06/20 15:40:07  soeren
// added intial version of specular refelction map
//
// Revision 1.5  2002/06/19 23:10:02  ingmar
// fixed the normal setup to be symmetric
//
// Revision 1.4  2002/06/19 21:52:24  ingmar
// added reflectance map lookup interpolation and debugimage write out
//
// Revision 1.3  2002/06/18 23:29:02  soeren
// added CreateReflectionMap and LookUp functions
//
// Revision 1.2  2002/06/18 21:29:35  soeren
// first tested working version
//
// Revision 1.1  2002/06/18 18:09:19  soeren
// uupps two ls
//
// Revision 1.1  2002/06/18 17:36:31  soeren
// initial version
//
