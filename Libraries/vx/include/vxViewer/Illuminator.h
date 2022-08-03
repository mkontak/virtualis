// $Id: Illuminator.h,v 1.2 2004/10/20 21:50:23 michael Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

#ifndef Illuminator_h
#define Illuminator_h


// includes
#include "Vector3D.h"


// class definition
class VX_VIEWER_DLL Illuminator
{
public:

  /// default contructor
  Illuminator();

  /// destructor
  ~Illuminator() {};

  /// set light source
  void SetLightSource(const int4 iLightSource, const Vector3D<float4> & lightVector, 
                      const bool & bDiffusePortionActive, const bool & bSpecularPortionActive);

  /// set view direction
  void SetViewDirection(const Vector3D<float4> & fViewDirection);
  
  /// set phong constants
  void SetPhongConstants(const float4 fAmbient, const float4 fDiffuse, 
                         const float4 fSpecular, const float4 fPhongExponent);

  /// shading using reflectance map
  inline float4 ShadePhongReflectanceMap(const Vector3D<float4> & normal);

  /// shading using diffuse phong
  inline float4 ShadePhongDiffuse(const Vector3D<float4> & normal, 
                                  const Vector3D<float4> & normalLight) const;

  /// shading using specular phong
  inline float4 ShadePhongSpecular(const Vector3D<float4> & normal, 
                                   const Vector3D<float4> & halfVector) const;

  /// set number of light sources
  void SetNumberOfLightSources(const int4 iNumberOfLightSources);

  /// set reflectance map resolution
  void SetEdgeLengthOfMaps(const int4 iEdgeLength);

  /// create diffuse reflectance map
  void CreateReflectionMapDiffuse();

  /// create specular reflectance map
  void CreateReflectionMapSpecular();

  /// enable/disable specular light
  void EnableSpecularPortionOfLight(const int4 iWhichLight, const bool & bFlag);

  /// enable/disable diffuse light
  void EnableDiffusePortionOfLight(const int4 iWhichLight, const bool & bFlag);

  /// enable/disable a light source
  void EnableLight(const int4 iWhichLight, const bool & bFlag);
  
  /// get specular value from map
  float4 LookUpInReflectionMapSpecular(const Vector3D<float4> & fNormal);

  /// get diffuse value from map
  float4 LookUpInReflectionMapDiffuse(const Vector3D<float4> & fNormal);

  /// write reflection map to file (helper function)
  void WriteToDiskReflectionMapDiffuse();

private:

  /// get diffuse value
  float4 GetDiffuse(const Vector3D<float4> & fNormal);

  /// get specular value
  float4 GetSpecular(const Vector3D<float4> & fNormal);

  /// create normals
  void CreateNormals();  

  /// compute reflection vectors
  void ComputeReflectionVectors();  

// Data members
private:

  /// ambient lighting constant
  float4 m_fAmbient;

  /// diffuse lighting constant
  float4 m_fDiffuse;

  /// specular lighting constant
  float4 m_fSpecular;

  /// phong exponent lighting constant
  float4 m_fPhongExponent;

  /// map resolution constant for edge scale
  float4 m_fEdgeScale;

  /// map resolution constant for edge length
  int4 m_iEdgeLength;

  /// map resolution constant for step in V
  int4 m_iStepV;

  /// map resolution constant for step along map
  int4 m_iStepMap;

  /// map resolution constant for map size
  int4 m_iMapSize;

  /// number of lights
  int4 m_iNumberOfLightSources;

  /// view direction
  Vector3D<float4> m_viewDirection;

  /// reflection vectors
  std::vector<Vector3D<float4> > m_vReflection;

  /// light sources
  std::vector<Vector3D<float4> > m_vLightSources;

  /// diffuse active?
  std::vector<bool> m_vbIsLightSourceDiffuseActive;

  /// specular active?
  std::vector<bool> m_vbIsLightSourceSpecularActive;

  /// diffuse map
  std::vector<float4> m_vfMapDiffuse;

  /// specular map
  std::vector<float4> m_vfMapSpecular;

  /// normal vector
  std::vector<Vector3D<float4> > m_vNormal;
}; // class Illuminator


#include "Illuminator.inl"


#endif // Illuminator_h


// $Log: Illuminator.h,v $
// Revision 1.2  2004/10/20 21:50:23  michael
// added VX_VIEWER_DLL to export classes
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2003/04/23 18:52:03  frank
// Code review
//
// Revision 1.15  2003/04/15 21:39:38  michael
// performed more code review
//
// Revision 1.14  2003/04/14 21:06:03  michael
// performed initial code review
//
// Revision 1.13  2003/01/22 21:41:01  ingmar
// added missing includes
//
// Revision 1.12  2002/06/21 21:42:46  ingmar
// made ShadePhongDiffuse() independent of the member light source
// added const to the interface of ShadePhongDiffuse() and ShadePhongSpecular()
//
// Revision 1.11  2002/06/21 21:26:14  ingmar
// cleaned up ShadePhong Diffuse() / Specular()
//
// Revision 1.10  2002/06/21 20:55:06  ingmar
// added ShadePhongSpecular()
//
// Revision 1.9  2002/06/21 17:07:27  ingmar
// seperated ShadePhongTwoLights() from ShadePhongReflectanceMap()
//
// Revision 1.8  2002/06/20 20:05:14  soeren
// disbaled temporaly specular highlight
//
// Revision 1.7  2002/06/20 18:39:53  soeren
// extended Illuminator to have n lights instead of two
//
// Revision 1.6  2002/06/20 16:04:29  soeren
// added lookuo for specular
//
// Revision 1.5  2002/06/20 15:40:07  soeren
// added intial version of specular refelction map
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/Illuminator.h,v 1.2 2004/10/20 21:50:23 michael Exp $
// $Id: Illuminator.h,v 1.2 2004/10/20 21:50:23 michael Exp $