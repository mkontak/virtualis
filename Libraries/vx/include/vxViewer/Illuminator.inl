// $Id: Illuminator.inl,v 1.2 2004/03/05 16:37:30 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


/**
 * Calculate the phong lighting equation for two lights
 *
 * @param gradient/normal
 * @return combined illumination factor
 */
float4 Illuminator::ShadePhongReflectanceMap(const Vector3D<float4> & normal)
{
  return(m_fAmbient + LookUpInReflectionMapDiffuse(normal) + LookUpInReflectionMapSpecular(normal));
} // ShadePhongReflectanceMap()


/**
 * Calculate the diffuse part of the phong lighting equation for one light
 *
 * @param gradient/normal
 * @param light normal
 * @return combined illumination factor
 *
 */
float4 Illuminator::ShadePhongDiffuse(const Vector3D<float4> & normal, 
                                      const Vector3D<float4> & lightNormal) const
{
  float4 fThetaLight(fabs(normal.Dot(lightNormal)));
  const float4 fDiffuseLight(fThetaLight * m_fDiffuse);

  return m_fAmbient + fDiffuseLight;
} // ShadePhongDiffuse()


/**
 * Calculate the specular part of the phong lighting equation for one light
 *
 * @param gradient/normal
 * @param half vector
 * @return combined illumination factor
 */
float4 Illuminator::ShadePhongSpecular(const Vector3D<float4> & normal, 
                                       const Vector3D<float4> & halfVector) const
{
  // can only do 0.7, if (0.7 ^ m_fPhongExponent) * 256 < 1.
  // Here we assume m_fPhongExponent == 16, so it is fine.
  const float4 fOneOverLenghtSquare(1); // normal.GetSquaredLength();

  float4 fGammaLight1(normal.Dot(halfVector));
  fGammaLight1 *= fGammaLight1; // * fOneOverLenghtSquare;
  fGammaLight1 = (fGammaLight1<=0.7*0.7) ? 0 : float4(pow((float4)fGammaLight1, m_fPhongExponent));

  return fGammaLight1 * m_fSpecular;
} // ShadePhongSpecular()



// $Log: Illuminator.inl,v $
// Revision 1.2  2004/03/05 16:37:30  geconomos
// Updated to compile with 7.1 compiler.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2003/04/14 21:06:02  michael
// performed initial code review
//
// Revision 1.15  2002/10/23 23:13:40  ingmar
// made lighting brighter from more angled to allow for brighter endoluminal flight
//
// Revision 1.14  2002/09/25 23:12:35  ingmar
// changes from intel  tuning visit
//
// Revision 1.13  2002/07/31 18:33:50  ingmar
// added bidirectional lighting
//
// Revision 1.12  2002/07/05 21:44:45  ingmar
// more proper diffuse light computation
//
// Revision 1.11  2002/06/21 21:42:46  ingmar
// made ShadePhongDiffuse() independent of the member light source
// added const to the interface of ShadePhongDiffuse() and ShadePhongSpecular()
//
// Revision 1.10  2002/06/21 21:26:14  ingmar
// cleaned up ShadePhong Diffuse() / Specular()
//
// Revision 1.9  2002/06/21 20:55:06  ingmar
// added ShadePhongSpecular()
//
// Revision 1.8  2002/06/21 17:07:27  ingmar
// seperated ShadePhongTwoLights() from ShadePhongReflectanceMap()
//
// Revision 1.7  2002/06/20 18:39:53  soeren
// extended Illuminator to have n lights instead of two
//
// Revision 1.6  2002/06/20 16:41:30  soeren
// activated specular map
//
// Revision 1.5  2002/06/19 21:52:24  ingmar
// added reflectance map lookup interpolation and debugimage write out
//
// Revision 1.4  2002/06/18 23:29:58  soeren
// disabled lookup of diffuse value
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
