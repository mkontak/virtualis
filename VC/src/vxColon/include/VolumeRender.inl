// $Id: VolumeRender.inl,v 1.10.2.3 2009/03/05 22:22:44 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.net)
//////////////////////////////////////////////////////////////////////

/**
 * Macro for trilinear iterpolation.
 *
 *    Trilinear interpolation: x,y,z are the three weights for
 *    the x, y, and z directions, and a,b,c,d,e,f,g, and h are
 *    the 8 vertices (values) to interpolate between.
 *    (x, y, and z are the distances from point a along the x,
 *    y, and z axis respectively. )
 *
 *
 *                         g----------------h 
 *            Y           /|               /|
 *                       / |              / |
 *                      /  |             /  |
 *            |        c----------------d   |
 *            |        |   |            |   |
 *            |        |   |            |   |
 *            |        |   |            |   |
 *            |      Z |   |            |   |
 *            |        |   e------------|---f
 *            |     /  |  /             |  /
 *            |    /   | /              | /
 *            |   /    |/               |/
 *            |  /     a----------------b
 *            | / 
 *            |/
 *            o------------------  X
 *
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
#define Trilin(x,y,z,a,b,c,d,e,f,g,h)      \
                                           \
       ((((a)*(1.0 - (z))               +  \
          (e)*(z))*(1.0 - (y))          +  \
         ((c)*(1.0 - (z))               +  \
          (g)*(z))*(y))*(1.0-(x))       +  \
        (((b)*(1.0 - (z))               +  \
          (f)*(z))*(1.0 - (y))          +  \
         ((d)*(1.0 - (z))               +  \
          (h)*(z))*(y))*(x))


/**
 * Macro for Bilinear interpolation.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
#define Bilin(x,y,a,b,c,d)                 \
                                           \
        (((a)*(1.0 - (x))               +  \
          (b)*(x))*(1.0-(y))            +  \
         ((c)*(1.0 - (x))               +  \
          (d)*(x))*(y))


/**
 * Macro for central difference gradients.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
#define CentralDifferenceGradient(pVol, xStep, yStep, zStep, gradX, gradY, gradZ) \
  gradX = ( *( (pVol) + xStep ) - *( (pVol) - xStep ) ) / 2;                      \
  gradY = ( *( (pVol) + yStep ) - *( (pVol) - yStep ) ) / 2;                      \
  gradZ = ( *( (pVol) + zStep ) - *( (pVol) - zStep ) ) / 2;


#if defined(_WIN64) || defined(WIN64)

	// This should be implemented in assembly codes with 64bits intrincs
	// 09-30-2010

	/**
	 * Fast code to round a double to an int.
	 */
	#define FastRoundDoubleToInt(iOutVal, fInVal) \
	  iOutVal = static_cast<int>(fInVal+0.5f);


	/**
	 * Fast assembly code to round double to int (for a 3-item vector).
	 */
	#define FastRoundFloatToIntXYZ(iOutX, iOutY, iOutZ, fInX, fInY, fInZ) \
	  iOutX = static_cast<int>(fInX+0.5f);                                \
	  iOutY = static_cast<int>(fInY+0.5f);                                \
	  iOutZ = static_cast<int>(fInZ+0.5f);


	/**
	 * Fast assembly code to trunc double to int (for a 3-item vector).
	 */
	#define FastTruncFloatToIntXYZ(iOutX, iOutY, iOutZ, fInX, fInY, fInZ) \
	  iOutX = static_cast<int>(fInX);                                     \
	  iOutY = static_cast<int>(fInY);                                     \
	  iOutZ = static_cast<int>(fInZ);

#else

/**
 * Fast assembly code to round a double to an int.
 *
 *   __asm FLD   QWORD PTR [fInVal ] ;;get double to be converted (must be double)
 *   __asm FISTP DWORD PTR [iOutVal] ;;store integer result
 */
#define FastRoundDoubleToInt(iOutVal, fInVal) \
  __asm FLD   QWORD PTR [fInVal ]             \
  __asm FISTP DWORD PTR [iOutVal]             \


/**
 * Fast assembly code to round double to int (for a 3-item vector).
 */
#define FastRoundFloatToIntXYZ(iOutX, iOutY, iOutZ, fInX, fInY, fInZ) \
  double fTmpX = fInX;                                                \
  double fTmpY = fInY;                                                \
  double fTmpZ = fInZ;                                                \
  __asm FLD   QWORD PTR [fTmpX]                                       \
  __asm FISTP DWORD PTR [iOutX]                                       \
  __asm FLD   QWORD PTR [fTmpY]                                       \
  __asm FISTP DWORD PTR [iOutY]                                       \
  __asm FLD   QWORD PTR [fTmpZ]                                       \
  __asm FISTP DWORD PTR [iOutZ]


/**
 * Fast assembly code to trunc double to int (for a 3-item vector).
 */
#define FastTruncFloatToIntXYZ(iOutX, iOutY, iOutZ, fInX, fInY, fInZ) \
  double fTmpX = fInX-0.5;                                            \
  double fTmpY = fInY-0.5;                                            \
  double fTmpZ = fInZ-0.5;                                            \
  __asm FLD   QWORD PTR [fTmpX]                                       \
  __asm FISTP DWORD PTR [iOutX]                                       \
  __asm FLD   QWORD PTR [fTmpY]                                       \
  __asm FISTP DWORD PTR [iOutY]                                       \
  __asm FLD   QWORD PTR [fTmpZ]                                       \
  __asm FISTP DWORD PTR [iOutZ]

#endif


/**
 * Grab the uncleansed value for a voxel using the diff volume or the regular volume
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline uint1 ReaderLib::VolumeRender::GetUncleansedValue( const Point<uint2> & position ) const
{
  if ( ! m_pDiffVolume ) return( GetVoxelRaw( position ) );
  uint1 uValue = m_pDiffVolumeIterator->Peek( position.m_x, position.m_y, position.m_z );
  if ( uValue == m_pDiffVolumeBackground ) uValue = GetVoxelRaw( position );
  return uValue;
} // GetUncleansedValue( position )


/**
 * Utility for washout math.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline void ReaderLib::VolumeRender::ClampFloatTo255( float4 & fValue )
{
  if (fValue > 255) fValue  = 255;
} // ClampFloatTo255()


/**
 * Utility for washout math.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline void ReaderLib::VolumeRender::ClampFloatTo255RGB(float4 & fRedValue, float4 & fGreenValue, float4 & fBlueValue)
{
  if (fRedValue   > 255) fRedValue   = 255;
  if (fGreenValue > 255) fGreenValue = 255;
  if (fBlueValue  > 255) fBlueValue  = 255;
} // ClampFloatTo255RGB


/**
 * Changes the ray direction for Mercator/Spherical/etc projections.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline void ReaderLib::VolumeRender::SetupAlternativeProjection(const uint4 &u, const uint4 &v,
                                                     const float4 &fDu, const float4 &fDv,
                                                     const Point<float4> &flc_vViewPoint,
                                                     const Vector<float4> &flc_vViewVector,
                                                     const Vector<float4> &flc_vUpVector,
                                                     const Vector<float4> &flc_vRightVector,
                                                     const Vector<float4> &flc_vDownVector,
                                                     const float4 &flc_fFovRadians,
                                                     Normal<float4> &vRayNormal,
                                                     Normal<float4> &vLight1Normal,
                                                     Normal<float4> &vLight2Normal,
                                                     Point<float4> &vPoint,
                                                     float4 &fLeapDistance)
{
  // pre-compute power function for franks projection
  double fPower = 1;// / rdrGlobal.m_pStudy->m_submarine.GetPotentialStrength();
  if (fPower < 0.01) fPower = 0.01;
  if (fPower > 100) fPower = 100;

  if (m_eProjectionType == VolumeRender::MERCATUR)
  {
    double fAngle = (float4(u)/float4(m_uRenderSize)*1.06 - 0.03) * (M_PI*2.0);
    Matrix<float4> m;
    m.Rotate(Normal<float4>(flc_vViewVector), fAngle);
    vRayNormal = Normal<float4>(m * flc_vUpVector);
    vLight1Normal = vRayNormal;
    vLight2Normal = vRayNormal;
    //vPoint = rdrGlobal.m_pStudy->m_submarine.GetPositionWorld() + flc_vViewVector * fDv * 40.0;
    vPoint = flc_vViewPoint + flc_vViewVector * fDv * 40.0;
    fLeapDistance = 0;
  }
  else if (m_eProjectionType == VolumeRender::SPHERICAL)
  {
    double fRadius = float4(v)/float4(m_uRenderSize) * M_PI;
    double fAngle = (float4(u)/float4(m_uRenderSize)*1.06 - 0.03) * (M_PI*2.0);
    Matrix<float4> m;
    m.Rotate(Normal<float4>(flc_vUpVector), fRadius);
    m.Rotate(Normal<float4>(flc_vViewVector), fAngle);
    vRayNormal = Normal<float4>(m * flc_vViewVector);
    vLight1Normal = vRayNormal;
    vLight2Normal = vRayNormal;
    vPoint = flc_vViewPoint;
  }
  else if (m_eProjectionType == VolumeRender::FRANKS)
  {
    // convert the du, dv image coordinates into radius and angle
    double radius = flc_fFovRadians/2 * pow(sqrt(static_cast<double>(fDu*fDu + fDv*fDv)),fPower);
    double angle = atan2(fDv,fDu);
    Matrix<float4> m;
    m.Rotate(Normal<float4>(flc_vUpVector), radius);
    m.Rotate(Normal<float4>(flc_vViewVector), angle);
    vRayNormal = Normal<float4>(m * flc_vViewVector);
    vLight1Normal = Normal<float4>( Vector<float4>(vRayNormal) + flc_vUpVector * m_fMoveLightVertDirection + flc_vRightVector * m_fMoveLightHorizDirection );
    vLight2Normal = Normal<float4>( flc_vDownVector + flc_vViewVector * m_fMoveLightVertDirection + flc_vRightVector * m_fMoveLightHorizDirection );
    vPoint = flc_vViewPoint;
  } // endif projection type

} // SetupAlternativeProjection
  

/**
 * Perform Volume Rendering speedup via leaping through balnk space.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline bool ReaderLib::VolumeRender::SpaceLeap(const Normal<float4> &vRayNormal,
                                    const uint4 &flc_uXStep, const uint4 &flc_uYStep, const uint4 &flc_uZStep,
                                    const float4 &flc_fXScl, const float4 &flc_fYScl, const float4 &flc_fZScl,
                                    const uint1 * const flc_puSLDvol, const float4 &flc_fDfbScale,
                                    const float4 &flc_fDfbSpaceLeapConversationFactor, const float4 &flc_fDfbSpaceLeapThreshold,
                                    const float4 &fFirstLeapDistance, Point<float4> &vPoint, float4 &fDepth,
																		const uint4 flc_uVolMaxX, const uint4 flc_uVolMaxY, const uint4 flc_uVolMaxZ)
{
  float4 fLeapDistance = fFirstLeapDistance;
  uint4 uVoxelX, uVoxelY, uVoxelZ;
	int iCount = 0;
  while (fLeapDistance > flc_fDfbSpaceLeapThreshold)
  {
    //m_spaceLeapCount++;
    //m_sumSpaceLeapDistance += leapDistance;
    //if(leapDistance > m_maxSpaceLeapDistance) m_maxSpaceLeapDistance = leapDistance;
    
    fDepth += fLeapDistance;
    vPoint += Vector<float4>(vRayNormal) * fLeapDistance;
    
    FastRoundFloatToIntXYZ(uVoxelX, uVoxelY, uVoxelZ,(vPoint.m_x*flc_fXScl),(vPoint.m_y*flc_fYScl),(vPoint.m_z*flc_fZScl));

		//Skip 2 slices at volume faces
    if (uVoxelX < 2 || uVoxelX > flc_uVolMaxX - 2 ||  
        uVoxelY < 2 || uVoxelY > flc_uVolMaxY - 2 ||  
        uVoxelZ < 2 || uVoxelZ > flc_uVolMaxZ - 2)
      return false;
		
    fLeapDistance = flc_puSLDvol[uVoxelX*flc_uXStep + uVoxelY*flc_uYStep + uVoxelZ*flc_uZStep]*flc_fDfbScale - flc_fDfbSpaceLeapConversationFactor;
    iCount ++;
  } // endwhile SpaceLeap
	return true;
} // SpaceLeap


/**
 * Count and paint the newly visible surface voxels.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline void ReaderLib::VolumeRender::VisibleSurfaceDetermination3D(const float4 &fDepth, uint1 * const &flc_puPaintVol, const uint1 &flc_uCurrentSeenValue,
                                                        const uint4 &flc_uXStep, const uint4 &flc_uYStep, const uint4 &flc_uZStep,
                                                        const uint4 &flc_uXYStep, const uint4 &flc_uYZStep, const uint4 &flc_uXZStep,
                                                        const uint4 &flc_uXYZStep, const uint4 &uIndexOffset, uint4 &uSurfaceCount)
{
  if(fDepth < m_fDepthVisibilityThresh)
  {
    uint1 * const puPaint = flc_puPaintVol + uIndexOffset;
    if( *(puPaint) & SUSPICIOUSROI ) ;
    else if( *(puPaint) & CADROI ) ;
    else
    {
      if(*(puPaint) & COUNTMASK) {*(puPaint) &= NOTCOUNTMASK; uSurfaceCount++;} *(puPaint) |= flc_uCurrentSeenValue;
    }
    if( *(puPaint + flc_uXStep) & SUSPICIOUSROI ) ;
    else if( *(puPaint + flc_uXStep) & CADROI ) ;
    else
    {
      if(*(puPaint + flc_uXStep) & COUNTMASK) {*(puPaint + flc_uXStep) &= NOTCOUNTMASK; uSurfaceCount++;} *(puPaint + flc_uXStep) |= flc_uCurrentSeenValue;
    }
    if( *(puPaint + flc_uYStep) & SUSPICIOUSROI ) ;
    else if( *(puPaint + flc_uYStep) & CADROI ) ;
    else
    {
      if(*(puPaint + flc_uYStep) & COUNTMASK) {*(puPaint + flc_uYStep) &= NOTCOUNTMASK; uSurfaceCount++;} *(puPaint + flc_uYStep) |= flc_uCurrentSeenValue;
    }
    if( *(puPaint + flc_uXYStep) & SUSPICIOUSROI ) ;
    else if( *(puPaint + flc_uXYStep) & CADROI ) ;
    else
    {
      if(*(puPaint + flc_uXYStep) & COUNTMASK) {*(puPaint + flc_uXYStep) &= NOTCOUNTMASK; uSurfaceCount++;} *(puPaint + flc_uXYStep) |= flc_uCurrentSeenValue;
    }
    if( *(puPaint + flc_uZStep) & SUSPICIOUSROI ) ;
    else if( *(puPaint + flc_uZStep) & CADROI ) ;
    else
    {
      if(*(puPaint + flc_uZStep) & COUNTMASK) {*(puPaint + flc_uZStep) &= NOTCOUNTMASK; uSurfaceCount++;} *(puPaint + flc_uZStep) |= flc_uCurrentSeenValue;
    }
    if( *(puPaint + flc_uXZStep) & SUSPICIOUSROI ) ;
    else if( *(puPaint + flc_uXZStep) & CADROI ) ;
    else
    {
      if(*(puPaint + flc_uXZStep) & COUNTMASK) {*(puPaint + flc_uXZStep) &= NOTCOUNTMASK; uSurfaceCount++;} *(puPaint + flc_uXZStep) |= flc_uCurrentSeenValue;
    }
    if( *(puPaint + flc_uYZStep) & SUSPICIOUSROI ) ;
    else if( *(puPaint + flc_uYZStep) & CADROI ) ;
    else 
    {
      if(*(puPaint + flc_uYZStep) & COUNTMASK) {*(puPaint + flc_uYZStep) &= NOTCOUNTMASK; uSurfaceCount++;} *(puPaint + flc_uYZStep) |= flc_uCurrentSeenValue;
    }
    if( *(puPaint + flc_uXYZStep) & SUSPICIOUSROI );
    else if( *(puPaint + flc_uXYZStep) & CADROI );
    else
    {
      if(*(puPaint + flc_uXYZStep) & COUNTMASK) {*(puPaint + flc_uXYZStep) &= NOTCOUNTMASK; uSurfaceCount++;} *(puPaint + flc_uXYZStep) |= flc_uCurrentSeenValue;
    }
  } // endif counting
} // VisibleSurfaceDetermination3D


/**
 * Change the color of the current sample if it intersects with the shadow.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline void ReaderLib::VolumeRender::ShadowSample(const float4 & fAbsDistance, const float4 & fHalfThickness, const float4 & fFalloff,
                                       float4 & fSampleRed, float4 & fSampleGreen, float4 & fSampleBlue,
                                       const float4 & fShadowRed, const float4 & fShadowGreen, const float4 & fShadowBlue)
{
  const float4 fOuterThickness = fHalfThickness + fFalloff;
  if (fAbsDistance > fOuterThickness)
  {
    // Make the common case fast
    return;
  }
  else if (fAbsDistance < fHalfThickness)
  {
    fSampleRed = fShadowRed;
    fSampleGreen = fShadowGreen;
    fSampleBlue = fShadowBlue;
  }
  else if (fAbsDistance < fOuterThickness)
  {
    // interpolate
    const float4 fShadowFactor = Interpolate(fHalfThickness, fAbsDistance, fOuterThickness, 1.0F, 0.0F);
    fSampleRed   = fSampleRed   * (1.0F - fShadowFactor) + fShadowRed   * fShadowFactor;
    fSampleGreen = fSampleGreen * (1.0F - fShadowFactor) + fShadowGreen * fShadowFactor;
    fSampleBlue  = fSampleBlue  * (1.0F - fShadowFactor) + fShadowBlue  * fShadowFactor;
  } // endif distance from plane
} // ShadowSample


/**
 * Change the color of the current sample if it intersects with the shadow.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline void ReaderLib::VolumeRender::ShadowHalfPlane(const float4 & fDistance, const float4 & fHalfThickness, const float4 & fFalloff,
                                          float4 & fSampleRed, float4 & fSampleGreen, float4 & fSampleBlue,
                                          const float4 & fShadowRed, const float4 & fShadowGreen, const float4 & fShadowBlue)
{
  const float4 fOuterThickness = fHalfThickness + fFalloff;
  if (fDistance > fOuterThickness)
  {
    // Make the common case fast
    return;
  }
  else if (fDistance > fHalfThickness)
  {
    // interpolate
    const float4 fShadowFactor = Interpolate(fHalfThickness, fDistance, fOuterThickness, 1.0F, 0.0F);
    fSampleRed   = fSampleRed   * (1.0F - fShadowFactor) + fShadowRed   * fShadowFactor;
    fSampleGreen = fSampleGreen * (1.0F - fShadowFactor) + fShadowGreen * fShadowFactor;
    fSampleBlue  = fSampleBlue  * (1.0F - fShadowFactor) + fShadowBlue  * fShadowFactor;
  }
  else
  {
    fSampleRed = fShadowRed;
    fSampleGreen = fShadowGreen;
    fSampleBlue = fShadowBlue;
  } // endif distance from plane
} // ShadowSample


/**
 * Perform shadowing of the endoscopic view.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline void ReaderLib::VolumeRender::AddShadows(const bool &flc_bObliqueShadow, const bool &flc_bGridShadow, const bool &flc_bAxialShadow,
                                     const bool &flc_bSagittalShadow, const bool &flc_bCoronalShadow, const bool &flc_bSegmentHalfPlaneShadow,
                                     const Point<float4> &vPoint,
                                     const Normal<float4> flc_vObliqueNormal,
                                     const float4 flc_fObliqueMinDistance,
                                     const float4 flc_fObliqueMaxDistance,
                                     const Point<float4> flc_vOrthogonalPoint,
                                     float4 &fSampleRed, float4 &fSampleGreen, float4 &fSampleBlue)
{

  // If we are displaying the Oblique shadow and the sample is within the Oblique slice
  if (flc_bObliqueShadow)
  {
    // calculate distance according to the plane equation
    float4 fPlaneDistance = vPoint.m_x * flc_vObliqueNormal.X() +
                            vPoint.m_y * flc_vObliqueNormal.Y() +
                            vPoint.m_z * flc_vObliqueNormal.Z();
    const float4 fObliqueCentralDistance = (flc_fObliqueMinDistance + flc_fObliqueMaxDistance) * 0.5F;
    ShadowSample(fabs(fObliqueCentralDistance - fPlaneDistance), m_fHalfShadowThickness, m_fHalfShadowThickness,
                 fSampleRed, fSampleGreen, fSampleBlue,
                 fSampleRed   / 2 + 64, // average of gray and sample color
                 fSampleGreen / 2 + 64,
                 fSampleBlue  / 2 + 64);
  } // endif obliqueShadow

  // If we are displaying the half plane shadow and the sample is within the half plane slice
  //if (flc_bSegmentHalfPlaneShadow)
  //{
  //  // calculate distance according to the plane equation
  //  float4 fPlaneDistance = vPoint.m_x * flc_vObliqueNormal.X() +
  //                          vPoint.m_y * flc_vObliqueNormal.Y() +
  //                          vPoint.m_z * flc_vObliqueNormal.Z();
  //  const float4 fObliqueCentralDistance = (flc_fObliqueMinDistance + flc_fObliqueMaxDistance) * 0.5F;
  //  ShadowHalfPlane((fObliqueCentralDistance - fPlaneDistance), m_fHalfShadowThickness, m_fHalfShadowThickness,
  //                  fSampleRed, fSampleGreen, fSampleBlue,
  //                  fSampleRed   / 2 + 64, // average of gray and sample color
  //                  fSampleGreen / 2 + 64,
  //                  fSampleBlue  / 2 + 64);
  //} // endif half plane shadow

  //if (flc_bGridShadow)
  //{
  //  int4 iUnitDigit = int(vPoint.m_x)%10;
  //  float4 fWeight = vPoint.m_x-float4(int(vPoint.m_x));
  //  if(iUnitDigit == 0)  ShadowSample(    fWeight, 0, 0.25F, fSampleRed, fSampleGreen, fSampleBlue, 0,0,0);
  //  if(iUnitDigit == 9)  ShadowSample(1.0-fWeight, 0, 0.25F, fSampleRed, fSampleGreen, fSampleBlue, 0,0,0);
  //  
  //  iUnitDigit = int(vPoint.m_y)%10;
  //  fWeight = vPoint.m_y-float4(int(vPoint.m_y));
  //  if(iUnitDigit == 0)  ShadowSample(    fWeight, 0, 0.25F, fSampleRed, fSampleGreen, fSampleBlue, 0,0,0);
  //  if(iUnitDigit == 9)  ShadowSample(1.0-fWeight, 0, 0.25F, fSampleRed, fSampleGreen, fSampleBlue, 0,0,0);
  //  
  //  iUnitDigit = int(vPoint.m_z)%10;
  //  fWeight = vPoint.m_z-float4(int(vPoint.m_z));
  //  if(iUnitDigit == 0)  ShadowSample(    fWeight, 0, 0.25F, fSampleRed, fSampleGreen, fSampleBlue, 0,0,0);
  //  if(iUnitDigit == 9)  ShadowSample(1.0-fWeight, 0, 0.25F, fSampleRed, fSampleGreen, fSampleBlue, 0,0,0);
  //} // endif gridShadow
  
  if (flc_bAxialShadow)
  {
    // highlight intersection with the z-axis in red
    ShadowSample(fabs(vPoint.m_z - flc_vOrthogonalPoint.m_z*GetUnits().m_z),
                 m_fHalfShadowThickness, m_fHalfShadowThickness,
                 fSampleRed, fSampleGreen, fSampleBlue,
                 128.0F, 0.0F, 0.0F);
  } // endif axialShadow

  if (flc_bSagittalShadow)
  {
    // highlight intersection with the x-axis in blue
    ShadowSample(fabs(vPoint.m_x - flc_vOrthogonalPoint.m_x*GetUnits().m_x),
                 m_fHalfShadowThickness, m_fHalfShadowThickness,
                 fSampleRed, fSampleGreen, fSampleBlue,
                 0.0F, 128.0F, 0.0F);
  } // endif saggittalShadow

  if (flc_bCoronalShadow)
  {
    // highlight intersection with the y-axis in green
    ShadowSample(fabs(vPoint.m_y - flc_vOrthogonalPoint.m_y*GetUnits().m_y),
                 m_fHalfShadowThickness, m_fHalfShadowThickness,
                 fSampleRed, fSampleGreen, fSampleBlue,
                 0.0F, 0.0F, 128.0F);
  } // endif coronalShadow
} // AddShadows


/**
 * Get the color of the current sample from the painted volume.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline void ReaderLib::VolumeRender::SamplePaintVol(const uint1 * const &flc_puPaintVol, const uint1 flc_uCurrentSeenVal,
                                         const uint4 &flc_uXStep, const uint4 &flc_uYStep,
                                         const uint4 &flc_uZStep, const uint4 &uIndexOffset,
                                         const float4 &fXDiff, const float4 &fYDiff, const float4 &fZDiff,
                                         const float4 &flc_fNotSeenRed, const float4 &flc_fNotSeenGreen, const float4 &flc_fNotSeenBlue,
                                         const float4 &flc_fSeenRed, const float4 &flc_fSeenGreen, const float4 &flc_fSeenBlue,
                                         float4 &fSampleRed, float4 &fSampleGreen, float4 &fSampleBlue)
{
  const uint1 * puPaint = flc_puPaintVol + uIndexOffset;
  if(fZDiff >= 0.5) puPaint += flc_uZStep;
  if(fYDiff >= 0.5) puPaint += flc_uYStep;
  if(fXDiff >= 0.5) puPaint += flc_uXStep;

  if( (*puPaint) & SUSPICIOUSROI )
  {
		if( rdrGlobal.m_bDisplaySRTroi )
    {
      fSampleRed   = 245.0F;
      fSampleGreen =  82.0F;
      fSampleBlue  = 197.0F;
    }
    else
    {
      fSampleRed   = flc_fSeenRed;   //TransferFunctionVC::m_vSeen.m_x;
      fSampleGreen = flc_fSeenGreen; //TransferFunctionVC::m_vSeen.m_y;
      fSampleBlue  = flc_fSeenBlue;  //TransferFunctionVC::m_vSeen.m_z;
    }
  }
  else if( (*puPaint) & CADROI )
  {
    if( rdrGlobal.m_bDisplayCadFindings )
    {
      fSampleRed   = 128.0F;
      fSampleGreen = 197.0F;
      fSampleBlue  = 255.0F;
    }
    else
    {
      fSampleRed   = flc_fSeenRed;   //TransferFunctionVC::m_vSeen.m_x;
      fSampleGreen = flc_fSeenGreen; //TransferFunctionVC::m_vSeen.m_y;
      fSampleBlue  = flc_fSeenBlue;  //TransferFunctionVC::m_vSeen.m_z;
    }
  }
  else if( ((*puPaint)&NOTCOUNTMASK) < flc_uCurrentSeenVal )
  // if( *puPaint == 128 )
  {
    fSampleRed   = flc_fNotSeenRed;   //TransferFunctionVC::m_vNotSeen.m_x;
    fSampleGreen = flc_fNotSeenGreen; //TransferFunctionVC::m_vNotSeen.m_y;
    fSampleBlue  = flc_fNotSeenBlue;  //TransferFunctionVC::m_vNotSeen.m_z;
  }
  else //if( *puPaint == 127 )
  {
    fSampleRed   = flc_fSeenRed;   //TransferFunctionVC::m_vSeen.m_x;
    fSampleGreen = flc_fSeenGreen; //TransferFunctionVC::m_vSeen.m_y;
    fSampleBlue  = flc_fSeenBlue;  //TransferFunctionVC::m_vSeen.m_z;
  } // endif not seen yet

} // SamplePaintVol


/**
 * Update the color of the current sample according to the test volume.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline void ReaderLib::VolumeRender::SampleTestVol(const uint1 * const flc_puTestVol, const uint1 flc_uTestThreshVal,
                                        const uint4 &flc_uXStep, const uint4 &flc_uYStep,
                                        const uint4 &flc_uZStep, const uint4 &uIndexOffset,
                                        const float4 &fXDiff, const float4 &fYDiff, const float4 &fZDiff,
                                        const float4 &flc_fRed, const float4 &flc_fGreen, const float4 &flc_fBlue,
                                        float4 &fSampleRed, float4 &fSampleGreen, float4 &fSampleBlue, float4 &fOpacity)
{
  const uint1 * puTest = flc_puTestVol + uIndexOffset;
  if(fZDiff >= 0.5) puTest += flc_uZStep;
  if(fYDiff >= 0.5) puTest += flc_uYStep;
  if(fXDiff >= 0.5) puTest += flc_uXStep;

  if( ((*puTest)&NOTCOUNTMASK) > flc_uTestThreshVal )
  {
    fSampleRed   = flc_fRed;
    fSampleGreen = flc_fGreen;
    fSampleBlue  = flc_fBlue;
    fOpacity = 1.0;
  }
} // SampleTestVol


/**
 * Calculate the gradient vector of a linear volume.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline void ReaderLib::VolumeRender::CalcCentralDifferenceGradient(const uint1 * const &puVol, const uint4 &flc_uXStep,
                                                        const uint4 &flc_uYStep, const uint4 &flc_uZStep,
                                                        const uint4 &flc_uXYStep, const uint4 &flc_uYZStep,
                                                        const uint4 &flc_uXZStep, const uint4 &flc_uXYZStep,
                                                        const float4 &fXDiff, const float4 &fYDiff, const float4 &fZDiff,
                                                        Vector<float4> &vGradVector)
{
  float4 fGrad000x, fGrad100x, fGrad010x, fGrad110x, fGrad001x, fGrad101x, fGrad011x, fGrad111x;
  float4 fGrad000y, fGrad100y, fGrad010y, fGrad110y, fGrad001y, fGrad101y, fGrad011y, fGrad111y;
  float4 fGrad000z, fGrad100z, fGrad010z, fGrad110z, fGrad001z, fGrad101z, fGrad011z, fGrad111z;

  CentralDifferenceGradient(puVol             , flc_uXStep, flc_uYStep, flc_uZStep, fGrad000x, fGrad000y, fGrad000z);
  CentralDifferenceGradient(puVol+flc_uXStep  , flc_uXStep, flc_uYStep, flc_uZStep, fGrad100x, fGrad100y, fGrad100z);
  CentralDifferenceGradient(puVol+flc_uYStep  , flc_uXStep, flc_uYStep, flc_uZStep, fGrad010x, fGrad010y, fGrad010z);
  CentralDifferenceGradient(puVol+flc_uXYStep , flc_uXStep, flc_uYStep, flc_uZStep, fGrad110x, fGrad110y, fGrad110z);
  CentralDifferenceGradient(puVol+flc_uZStep  , flc_uXStep, flc_uYStep, flc_uZStep, fGrad001x, fGrad001y, fGrad001z);
  CentralDifferenceGradient(puVol+flc_uXZStep , flc_uXStep, flc_uYStep, flc_uZStep, fGrad101x, fGrad101y, fGrad101z);
  CentralDifferenceGradient(puVol+flc_uYZStep , flc_uXStep, flc_uYStep, flc_uZStep, fGrad011x, fGrad011y, fGrad011z);
  CentralDifferenceGradient(puVol+flc_uXYZStep, flc_uXStep, flc_uYStep, flc_uZStep, fGrad111x, fGrad111y, fGrad111z);
  
  vGradVector.m_x = float4(Trilin(fXDiff, fYDiff, fZDiff,
                                  fGrad000x, fGrad100x, fGrad010x, fGrad110x,
                                  fGrad001x, fGrad101x, fGrad011x, fGrad111x));

  vGradVector.m_y = float4(Trilin(fXDiff, fYDiff, fZDiff,
                                  fGrad000y, fGrad100y, fGrad010y, fGrad110y,
                                  fGrad001y, fGrad101y, fGrad011y, fGrad111y));

  vGradVector.m_z = float4(Trilin(fXDiff, fYDiff, fZDiff,
                                  fGrad000z, fGrad100z, fGrad010z, fGrad110z,
                                  fGrad001z, fGrad101z, fGrad011z, fGrad111z));
} // CalcCentralDifferenceGradient

/**
 * Calculate the gradient vector of using both the block difference volume and the ecv volume
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline void ReaderLib::VolumeRender::CalcCentralDifferenceGradient(Point<uint2> &vVolCoord,
                                                        const float4 &fXDiff, const float4 &fYDiff, const float4 &fZDiff,
                                                        Vector<float4> &vGradVector)
{

  float4 fGrad000x, fGrad100x, fGrad010x, fGrad110x, fGrad001x, fGrad101x, fGrad011x, fGrad111x;
  float4 fGrad000y, fGrad100y, fGrad010y, fGrad110y, fGrad001y, fGrad101y, fGrad011y, fGrad111y;
  float4 fGrad000z, fGrad100z, fGrad010z, fGrad110z, fGrad001z, fGrad101z, fGrad011z, fGrad111z;
  
  fGrad000x = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(0,0,0)+Vector<uint2>( 1, 0, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(0,0,0)+Vector<uint2>(-1, 0, 0)))/2.0;
  fGrad000y = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(0,0,0)+Vector<uint2>( 0, 1, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(0,0,0)+Vector<uint2>( 0,-1, 0)))/2.0;
  fGrad000z = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(0,0,0)+Vector<uint2>( 0, 0, 1)) - GetUncleansedValue(vVolCoord+Vector<uint2>(0,0,0)+Vector<uint2>( 0, 0,-1)))/2.0;

  fGrad100x = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(1,0,0)+Vector<uint2>( 1, 0, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(1,0,0)+Vector<uint2>(-1, 0, 0)))/2.0;
  fGrad100y = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(1,0,0)+Vector<uint2>( 0, 1, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(1,0,0)+Vector<uint2>( 0,-1, 0)))/2.0;
  fGrad100z = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(1,0,0)+Vector<uint2>( 0, 0, 1)) - GetUncleansedValue(vVolCoord+Vector<uint2>(1,0,0)+Vector<uint2>( 0, 0,-1)))/2.0;

  fGrad010x = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(0,1,0)+Vector<uint2>( 1, 0, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(0,1,0)+Vector<uint2>(-1, 0, 0)))/2.0;
  fGrad010y = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(0,1,0)+Vector<uint2>( 0, 1, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(0,1,0)+Vector<uint2>( 0,-1, 0)))/2.0;
  fGrad010z = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(0,1,0)+Vector<uint2>( 0, 0, 1)) - GetUncleansedValue(vVolCoord+Vector<uint2>(0,1,0)+Vector<uint2>( 0, 0,-1)))/2.0;

  fGrad110x = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(1,1,0)+Vector<uint2>( 1, 0, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(1,1,0)+Vector<uint2>(-1, 0, 0)))/2.0;
  fGrad110y = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(1,1,0)+Vector<uint2>( 0, 1, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(1,1,0)+Vector<uint2>( 0,-1, 0)))/2.0;
  fGrad110z = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(1,1,0)+Vector<uint2>( 0, 0, 1)) - GetUncleansedValue(vVolCoord+Vector<uint2>(1,1,0)+Vector<uint2>( 0, 0,-1)))/2.0;

  fGrad001x = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(0,0,1)+Vector<uint2>( 1, 0, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(0,0,1)+Vector<uint2>(-1, 0, 0)))/2.0;
  fGrad001y = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(0,0,1)+Vector<uint2>( 0, 1, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(0,0,1)+Vector<uint2>( 0,-1, 0)))/2.0;
  fGrad001z = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(0,0,1)+Vector<uint2>( 0, 0, 1)) - GetUncleansedValue(vVolCoord+Vector<uint2>(0,0,1)+Vector<uint2>( 0, 0,-1)))/2.0;

  fGrad101x = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(1,0,1)+Vector<uint2>( 1, 0, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(1,0,1)+Vector<uint2>(-1, 0, 0)))/2.0;
  fGrad101y = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(1,0,1)+Vector<uint2>( 0, 1, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(1,0,1)+Vector<uint2>( 0,-1, 0)))/2.0;
  fGrad101z = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(1,0,1)+Vector<uint2>( 0, 0, 1)) - GetUncleansedValue(vVolCoord+Vector<uint2>(1,0,1)+Vector<uint2>( 0, 0,-1)))/2.0;

  fGrad011x = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(0,1,1)+Vector<uint2>( 1, 0, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(0,1,1)+Vector<uint2>(-1, 0, 0)))/2.0;
  fGrad011y = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(0,1,1)+Vector<uint2>( 0, 1, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(0,1,1)+Vector<uint2>( 0,-1, 0)))/2.0;
  fGrad011z = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(0,1,1)+Vector<uint2>( 0, 0, 1)) - GetUncleansedValue(vVolCoord+Vector<uint2>(0,1,1)+Vector<uint2>( 0, 0,-1)))/2.0;

  fGrad111x = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(1,1,1)+Vector<uint2>( 1, 0, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(1,1,1)+Vector<uint2>(-1, 0, 0)))/2.0;
  fGrad111y = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(1,1,1)+Vector<uint2>( 0, 1, 0)) - GetUncleansedValue(vVolCoord+Vector<uint2>(1,1,1)+Vector<uint2>( 0,-1, 0)))/2.0;
  fGrad111z = float4(GetUncleansedValue(vVolCoord+Vector<uint2>(1,1,1)+Vector<uint2>( 0, 0, 1)) - GetUncleansedValue(vVolCoord+Vector<uint2>(1,1,1)+Vector<uint2>( 0, 0,-1)))/2.0;
  
  vGradVector.m_x = float4(Trilin(fXDiff, fYDiff, fZDiff,
                                  fGrad000x, fGrad100x, fGrad010x, fGrad110x,
                                  fGrad001x, fGrad101x, fGrad011x, fGrad111x));

  vGradVector.m_y = float4(Trilin(fXDiff, fYDiff, fZDiff,
                                  fGrad000y, fGrad100y, fGrad010y, fGrad110y,
                                  fGrad001y, fGrad101y, fGrad011y, fGrad111y));

  vGradVector.m_z = float4(Trilin(fXDiff, fYDiff, fZDiff,
                                  fGrad000z, fGrad100z, fGrad010z, fGrad110z,
                                  fGrad001z, fGrad101z, fGrad011z, fGrad111z));

} // CalcCentralDifferenceGradient


/**
 * Calculate the phong lighting equation for two lights.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
inline void ReaderLib::VolumeRender::ShadeTwoLightPhong(const Normal<float4> &vLight1Normal, const Normal<float4> &vLight2Normal,
                                             Vector<float4> &vGradVector, const Vector<float4> &vHalfwayVector,
                                             float4& fSampleRed, float4& fSampleGreen, float4 &fSampleBlue)
{
  // carefully normalize the gradient (no shading if no gradient)
  if ( vGradVector.m_x == 0.0F && vGradVector.m_y == 0.0F && vGradVector.m_z == 0 )
    return;
  vGradVector.Normalize();

  float4 fLdotN1 = vGradVector.Dot(vLight1Normal);
  if (fLdotN1<0) fLdotN1=0;
  
  float4 fLdotN2 = vGradVector.Dot(vLight2Normal);
  if (fLdotN2<0) fLdotN2=0;
  
  float4 fNdotH = vGradVector.Dot(vHalfwayVector);
  fNdotH = (fNdotH<=0.5) ? 0 : float4(pow((double)fNdotH, (double)m_fSpecularPower));
  float4 fDiffuse1 = float4(fLdotN1 * m_fKD);
  float4 fDiffuse2 = float4(fLdotN2 * m_fKD);
  float4 fSpecular = float4((fNdotH * m_fKS) * 255); // light color
  
  // shade the sample
  fSampleRed   = fSampleRed   * m_fKA + fSampleRed   * fDiffuse1 * 1.0 + fSampleRed   * fDiffuse2 * 1.0 + fSpecular;
  fSampleGreen = fSampleGreen * m_fKA + fSampleGreen * fDiffuse1 * 0.7 + fSampleGreen * fDiffuse2 * 1.0 + fSpecular;
  fSampleBlue  = fSampleBlue  * m_fKA + fSampleBlue  * fDiffuse1 * 0.9 + fSampleBlue  * fDiffuse2 * 1.0 + fSpecular;
} // ShadeTwoLightPhong


/**
 * All the work for each ray (assumes the ray is inside the volume).
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
#define OPACITY_THRESH 0.5
#define ADJUST_OPACITY_THRESH 0.05
inline void ReaderLib::VolumeRender::CastSingleRay(const uint4 &u, const uint4 &v,
                                        const bool &flc_bObliqueShadow, const bool &flc_bGridShadow, const bool &flc_bAxialShadow,
                                        const bool &flc_bSagittalShadow, const bool &flc_bCoronalShadow, const bool &flc_bSegmentHalfPlaneShadow,
                                        const bool &flc_bShadow, const bool &flc_bBiopsyMode, const bool &flc_bTestVol,
                                        const bool &flc_bRaycastPaintVol,
                                        const bool &flc_bSpaceLeaping,
                                        const bool &flc_bUncleansed,
                                        const Normal<float4> flc_vObliqueNormal,
                                        const float4 flc_fObliqueMinDistance,
                                        const float4 flc_fObliqueMaxDistance,
                                        const Point<float4> flc_vOrthogonalPoint,
                                        Normal<float4> &vRayNormal,
                                        Normal<float4> &vLight1Normal,
                                        Normal<float4> &vLight2Normal,
                                        Point<float4> &vPoint,
                                        float4 &fLeapDistance,
                                        const uint4 &flc_uXStep, const uint4 &flc_uYStep, const uint4 &flc_uZStep,
                                        const uint4 &flc_uXYStep, const uint4 &flc_uYZStep, const uint4 &flc_uXZStep,
                                        const uint4 &flc_uXYZStep,
                                        const uint4 &flc_uXStep3, const uint4 &flc_uYStep3, const uint4 &flc_uZStep3,
                                        const uint4 &flc_uXYStep3, const uint4 &flc_uYZStep3, const uint4 &flc_uXZStep3,
                                        const uint4 &flc_uXYZStep3,
                                        const float4 &flc_fXScl, const float4 &flc_fYScl, const float4 &flc_fZScl,
                                        const uint4 &flc_uVolMaxX, const uint4 &flc_uVolMaxY, const uint4 &flc_uVolMaxZ,
                                        const uint1 * const flc_puECVvol,
                                        const uint1 * const flc_puSLDvol,
                                        const uint1 * const flc_puTestVol,
                                        uint1 * const &flc_puPaintVol, const uint1 &flc_uCurrentSeenVal,
                                        const float4 &flc_fNotSeenRed, const float4 &flc_fNotSeenGreen, const float4 &flc_fNotSeenBlue,
                                        const float4 &flc_fSeenRed, const float4 &flc_fSeenGreen, const float4 &flc_fSeenBlue,
                                        const float4 * &flc_pfOpacitySurfaceTransferFunction,
                                        const float4 * &flc_pfRedBiopsyTransferFunction, const float4 * &flc_pfGreenBiopsyTransferFunction,
                                        const float4 * &flc_pfBlueBiopsyTransferFunction, const float4 * &flc_pfOpacityBiopsyTransferFunction,
                                        float4 &fRedValue, float4 &fGreenValue, float4 &fBlueValue, float4 &fAlphaValue, float4 &fDepthValue,
                                        const float4 &flc_fDfbScale,
                                        const float4 &flc_fDfbSpaceLeapConversationFactor, const float4 &flc_fDfbSpaceLeapThreshold,
                                        uint4 &uSurfaceCount,
                                        Vector<float4> &vGradVector, Vector<float4> &vHalfwayVector, Vector<float4> &vIntervalVector,
																				const bool bReverseNormal)
{ 
      float4 fAccumulatedOpacity = 0.0, fAccumulatedBuffer = 0.0;
      float4 fOpacity,fLastOpacity = 0;
      uint4 uVoxelX, uVoxelY, uVoxelZ;
      
      // pre-compute the halfway vector
      vHalfwayVector = Vector<float4>(vRayNormal) + Vector<float4>(vLight2Normal);
      vHalfwayVector.Normalize();

      // compute the inter-sample interval vector
      vIntervalVector = Vector<float4>(vRayNormal) * m_fInterSampleDistance;

      // initialize the length of the ray
      float4 fDepth = 0;

      if (flc_bSpaceLeaping)
      {
				if(!SpaceLeap(vRayNormal, flc_uXStep, flc_uYStep, flc_uZStep, flc_fXScl, flc_fYScl, flc_fZScl,
                      flc_puSLDvol, flc_fDfbScale, flc_fDfbSpaceLeapConversationFactor, flc_fDfbSpaceLeapThreshold,
                      fLeapDistance, vPoint, fDepth, flc_uVolMaxX, flc_uVolMaxY, flc_uVolMaxZ))
					// Ray hits nothing
					return;
      } // endif flc_bSpaceLeaping
      
      // jitter the starting position along the ray
      float4 fJitterDistance = m_jitter.GetValue(u,v);
      vPoint -= vIntervalVector * fJitterDistance;
      fDepth -= fJitterDistance;
      
      while (fAccumulatedOpacity < m_fOpacityThreshold)
      {
        //m_sampleCount++;

        FastTruncFloatToIntXYZ(uVoxelX, uVoxelY, uVoxelZ,(vPoint.m_x*flc_fXScl),(vPoint.m_y*flc_fYScl),(vPoint.m_z*flc_fZScl));
        float4 fXDiff = vPoint.m_x*flc_fXScl - uVoxelX;
        float4 fYDiff = vPoint.m_y*flc_fYScl - uVoxelY;
        float4 fZDiff = vPoint.m_z*flc_fZScl - uVoxelZ;
        
        if (uVoxelX < 2 || uVoxelX > flc_uVolMaxX - 2 ||  
            uVoxelY < 2 || uVoxelY > flc_uVolMaxY - 2 ||  
            uVoxelZ < 2 || uVoxelZ > flc_uVolMaxZ - 2)
        {
          // outside the volume -> stop the ray casting
          fAccumulatedOpacity = m_fOpacityThreshold;
          break;
        }

        const uint4 uIndexOffset = uVoxelX * flc_uXStep + uVoxelY * flc_uYStep + uVoxelZ * flc_uZStep;
        const uint4 uIndexOffset3 = uVoxelX * flc_uXStep3 + uVoxelY * flc_uXStep3 + uVoxelZ * flc_uZStep3;

        int4 iValue;
        double fValue;
        const uint1 * const puVol = flc_puECVvol + uIndexOffset;

        
        if(flc_bUncleansed)
        {
          // Render from the Block-Uncleansed-Difference Volume
          fValue = Trilin(fXDiff,fYDiff,fZDiff,
                          GetUncleansedValue(Point<uint2>(uVoxelX  ,uVoxelY  ,uVoxelZ  )),
                          GetUncleansedValue(Point<uint2>(uVoxelX+1,uVoxelY  ,uVoxelZ  )),
                          GetUncleansedValue(Point<uint2>(uVoxelX  ,uVoxelY+1,uVoxelZ  )),
                          GetUncleansedValue(Point<uint2>(uVoxelX+1,uVoxelY+1,uVoxelZ  )),
                          GetUncleansedValue(Point<uint2>(uVoxelX  ,uVoxelY  ,uVoxelZ+1)),
                          GetUncleansedValue(Point<uint2>(uVoxelX+1,uVoxelY  ,uVoxelZ+1)),
                          GetUncleansedValue(Point<uint2>(uVoxelX  ,uVoxelY+1,uVoxelZ+1)),
                          GetUncleansedValue(Point<uint2>(uVoxelX+1,uVoxelY+1,uVoxelZ+1)));
        }
        else
        {
          fValue = Trilin(fXDiff,fYDiff,fZDiff,
                          *(puVol), 
                          *(puVol + flc_uXStep), 
                          *(puVol + flc_uYStep), 
                          *(puVol + flc_uXYStep),
                          *(puVol + flc_uZStep), 
                          *(puVol + flc_uXZStep),
                          *(puVol + flc_uYZStep),
                          *(puVol + flc_uXYZStep));
        } // endif diff volume rendering

        FastRoundDoubleToInt(iValue, fValue);
        fOpacity = flc_pfOpacitySurfaceTransferFunction[iValue];
        
        if(fAccumulatedOpacity < 0.5F && flc_puPaintVol )
        {
          VisibleSurfaceDetermination3D(fDepth, flc_puPaintVol, flc_uCurrentSeenVal-1,
                                        flc_uXStep, flc_uYStep, flc_uZStep, flc_uXYStep, flc_uYZStep, flc_uXZStep,
                                        flc_uXYZStep, uIndexOffset, uSurfaceCount);
        } // endif still marking

        if (fOpacity > 0.0F)
        {
          float4 fSampleRed,fSampleGreen,fSampleBlue;

          if(flc_bRaycastPaintVol && flc_puPaintVol)
          {
            SamplePaintVol(flc_puPaintVol, flc_uCurrentSeenVal, flc_uXStep, flc_uYStep, flc_uZStep, uIndexOffset, 
                           fXDiff, fYDiff, fZDiff,
                           flc_fNotSeenRed, flc_fNotSeenGreen, flc_fNotSeenBlue,
                           flc_fSeenRed, flc_fSeenGreen, flc_fSeenBlue,
                           fSampleRed, fSampleGreen, fSampleBlue);
          }
          else
          {
            const uint1 * puPaint = flc_puPaintVol + uIndexOffset;
            if(fZDiff >= 0.5) puPaint += flc_uZStep;
            if(fYDiff >= 0.5) puPaint += flc_uYStep;
            if(fXDiff >= 0.5) puPaint += flc_uXStep;

            if( (*puPaint) & SUSPICIOUSROI && rdrGlobal.m_bDisplaySRTroi )
            {
              fSampleRed   = 245.0F;
              fSampleGreen =  82.0F;
              fSampleBlue  = 197.0F;
            }
            else if( (*puPaint) & CADROI && rdrGlobal.m_bDisplayCadFindings )
            {
              fSampleRed   = 128.0F;
              fSampleGreen = 197.0F;
              fSampleBlue  = 255.0F;
            }
            else
            {
              fSampleRed   = flc_fNotSeenRed;   //TransferFunctionVC::m_vNotSeen.m_x;
              fSampleGreen = flc_fNotSeenGreen; //TransferFunctionVC::m_vNotSeen.m_y;
              fSampleBlue  = flc_fNotSeenBlue;  //TransferFunctionVC::m_vNotSeen.m_z;
            }
          } // endif PaintVol or not

          if (flc_bShadow)
          {
            AddShadows(flc_bObliqueShadow, flc_bGridShadow, flc_bAxialShadow, flc_bSagittalShadow, flc_bCoronalShadow, flc_bSegmentHalfPlaneShadow,
                       vPoint, flc_vObliqueNormal, flc_fObliqueMinDistance, flc_fObliqueMaxDistance, flc_vOrthogonalPoint,
                       fSampleRed, fSampleGreen, fSampleBlue);
          } // flc_shadow

          if (flc_bBiopsyMode == true && 
              v > m_magicWindowStartPt.m_y && v < (m_magicWindowStartPt.m_y+m_uMagicWindowSize) && 
              u > m_magicWindowStartPt.m_x && u < (m_magicWindowStartPt.m_x+m_uMagicWindowSize) )
          {

            fSampleRed   = flc_pfRedBiopsyTransferFunction[iValue];
            fSampleGreen = flc_pfGreenBiopsyTransferFunction[iValue];
            fSampleBlue  = flc_pfBlueBiopsyTransferFunction[iValue];
            fOpacity     = flc_pfOpacityBiopsyTransferFunction[iValue];
          }
          /*else if (flc_bTestVol == true && 
                   v > m_magicWindowStartPt.m_y && v < (m_magicWindowStartPt.m_y+m_uMagicWindowSize) && 
                   u > m_magicWindowStartPt.m_x && u < (m_magicWindowStartPt.m_x+m_uMagicWindowSize) )
          {
            fSampleRed   = flc_fNotSeenRed;
            fSampleGreen = flc_fNotSeenGreen;
            fSampleBlue  = flc_fNotSeenBlue;
            fOpacity     = flc_pfOpacityBiopsyTransferFunction[iValue];
            SampleTestVol(flc_puTestVol, 1, flc_uXStep, flc_uYStep, flc_uZStep, uIndexOffset, fXDiff, fYDiff, fZDiff,
                          255.0F, 255.0F, 255.0F, fSampleRed, fSampleGreen, fSampleBlue, fOpacity);

            CalcCentralDifferenceGradient(puVol, flc_uXStep, flc_uYStep, flc_uZStep, flc_uXYStep, flc_uYZStep, flc_uXZStep, flc_uXYZStep, fXDiff, fYDiff, fZDiff, vGradVector);
						if(bReverseNormal) vGradVector = - vGradVector;
						ShadeTwoLightPhong(vLight1Normal, vLight2Normal, vGradVector, vHalfwayVector, fSampleRed, fSampleGreen, fSampleBlue);
            ClampFloatTo255RGB(fSampleRed, fSampleGreen, fSampleBlue);
          } */
          else
          { 
            
            if(flc_bUncleansed)
            {
              // Render from the Block-Uncleansed-Difference Volume
              CalcCentralDifferenceGradient(Point<uint2>(uVoxelX,uVoxelY,uVoxelZ), fXDiff, fYDiff, fZDiff, vGradVector);
            }
            else
            {
              CalcCentralDifferenceGradient(puVol, flc_uXStep, flc_uYStep, flc_uZStep, flc_uXYStep, flc_uYZStep, flc_uXZStep, flc_uXYZStep, fXDiff, fYDiff, fZDiff, vGradVector);
            } // endif diff volume rendering
						if(bReverseNormal) vGradVector = - vGradVector;
						ShadeTwoLightPhong(vLight1Normal, vLight2Normal, vGradVector, vHalfwayVector, fSampleRed, fSampleGreen, fSampleBlue);
            ClampFloatTo255RGB(fSampleRed, fSampleGreen, fSampleBlue);

          } // end biopsy mode
          
          fAccumulatedBuffer = (1.0f - fAccumulatedOpacity) * fOpacity;
          fRedValue   += fAccumulatedBuffer * fSampleRed;
          fGreenValue += fAccumulatedBuffer * fSampleGreen;
          fBlueValue  += fAccumulatedBuffer * fSampleBlue;
          fAccumulatedOpacity += fAccumulatedBuffer;

          vPoint += vIntervalVector;
          fDepth += m_fInterSampleDistance;

        }

        else
        { // in empty space, try space leaping
        
          fLeapDistance = flc_puSLDvol[uIndexOffset]*flc_fDfbScale - flc_fDfbSpaceLeapConversationFactor;
          if (fLeapDistance <= flc_fDfbSpaceLeapThreshold || !flc_bSpaceLeaping)
          {
            // if no leap, be sure to still STEP along ray
            vPoint += vIntervalVector;
            fDepth += m_fInterSampleDistance;
          }
          else
          { // space leaping after first sample
						if(!SpaceLeap(vRayNormal, flc_uXStep, flc_uYStep, flc_uZStep, flc_fXScl, flc_fYScl, flc_fZScl,
                          flc_puSLDvol, flc_fDfbScale, flc_fDfbSpaceLeapConversationFactor, flc_fDfbSpaceLeapThreshold,
                          fLeapDistance, vPoint, fDepth, flc_uVolMaxX, flc_uVolMaxY, flc_uVolMaxZ))
							// Leap outside the volume, hit nothing -> stop the ray casting
							fAccumulatedOpacity = m_fOpacityThreshold;
          } // endif leapDistance > 1
          
        } // endif opacity or space leap

        // if we just passed the first colon wall sample, store this depth
        if (fAccumulatedOpacity > ADJUST_OPACITY_THRESH && fLastOpacity <= ADJUST_OPACITY_THRESH) fDepthValue = fDepth;
        fLastOpacity = fAccumulatedOpacity;

      } // endwhile not early ray termination
      

      // saturate color
      ClampFloatTo255RGB(fRedValue, fGreenValue, fBlueValue);
			fAlphaValue = fAccumulatedOpacity*255;
			ClampFloatTo255(fAlphaValue);
} // CastSingleRay


/** Notes in the following was made by Dongqing in 2007/05/03
 *  There is a "bug" causing the lumen coverage works incorrectly after the UD view was refreshed. 
 *  We spent 2 days and could not figure out why. By designed, the UD segment view should not 
 *  record lumen coverage. So, we make a special CastSingleForUDview, where we don't count the 
 *  lumen coverage. That seems walk around the problem.
 */
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
inline void ReaderLib::VolumeRender::CastSingleRayForUDview(const uint4 &u, const uint4 &v,
                                        const bool &flc_bObliqueShadow, const bool &flc_bGridShadow, const bool &flc_bAxialShadow,
                                        const bool &flc_bSagittalShadow, const bool &flc_bCoronalShadow, const bool &flc_bSegmentHalfPlaneShadow,
                                        const bool &flc_bShadow, const bool &flc_bBiopsyMode, const bool &flc_bTestVol,
                                        const bool &flc_bRaycastPaintVol,
                                        const bool &flc_bSpaceLeaping,
                                        const bool &flc_bUncleansed,
                                        const Normal<float4> flc_vObliqueNormal,
                                        const float4 flc_fObliqueMinDistance,
                                        const float4 flc_fObliqueMaxDistance,
                                        const Point<float4> flc_vOrthogonalPoint,
                                        Normal<float4> &vRayNormal,
                                        Normal<float4> &vLight1Normal,
                                        Normal<float4> &vLight2Normal,
                                        Point<float4> &vPoint,
                                        float4 &fLeapDistance,
                                        const uint4 &flc_uXStep, const uint4 &flc_uYStep, const uint4 &flc_uZStep,
                                        const uint4 &flc_uXYStep, const uint4 &flc_uYZStep, const uint4 &flc_uXZStep,
                                        const uint4 &flc_uXYZStep,
                                        const uint4 &flc_uXStep3, const uint4 &flc_uYStep3, const uint4 &flc_uZStep3,
                                        const uint4 &flc_uXYStep3, const uint4 &flc_uYZStep3, const uint4 &flc_uXZStep3,
                                        const uint4 &flc_uXYZStep3,
                                        const float4 &flc_fXScl, const float4 &flc_fYScl, const float4 &flc_fZScl,
                                        const uint4 &flc_uVolMaxX, const uint4 &flc_uVolMaxY, const uint4 &flc_uVolMaxZ,
                                        const uint1 * const flc_puECVvol,
                                        const uint1 * const flc_puSLDvol,
                                        const uint1 * const flc_puTestVol,
                                        uint1 * const &flc_puPaintVol, const uint1 &flc_uCurrentSeenVal,
                                        const float4 &flc_fNotSeenRed, const float4 &flc_fNotSeenGreen, const float4 &flc_fNotSeenBlue,
                                        const float4 &flc_fSeenRed, const float4 &flc_fSeenGreen, const float4 &flc_fSeenBlue,
                                        const float4 * &flc_pfOpacitySurfaceTransferFunction,
                                        const float4 * &flc_pfRedBiopsyTransferFunction, const float4 * &flc_pfGreenBiopsyTransferFunction,
                                        const float4 * &flc_pfBlueBiopsyTransferFunction, const float4 * &flc_pfOpacityBiopsyTransferFunction,
                                        float4 &fRedValue, float4 &fGreenValue, float4 &fBlueValue, float4 &fAlphaValue, float4 &fDepthValue,
                                        const float4 &flc_fDfbScale,
                                        const float4 &flc_fDfbSpaceLeapConversationFactor, const float4 &flc_fDfbSpaceLeapThreshold,
                                        uint4 &uSurfaceCount,
                                        Vector<float4> &vGradVector, Vector<float4> &vHalfwayVector, Vector<float4> &vIntervalVector,
																				const bool bReverseNormal)
{ 
      float4 fAccumulatedOpacity = 0.0, fAccumulatedBuffer = 0.0;
      float4 fOpacity,fLastOpacity = 0;
      uint4 uVoxelX, uVoxelY, uVoxelZ;
      
      // pre-compute the halfway vector
      vHalfwayVector = Vector<float4>(vRayNormal) + Vector<float4>(vLight2Normal);
      vHalfwayVector.Normalize();

      // compute the inter-sample interval vector
      vIntervalVector = Vector<float4>(vRayNormal) * m_fInterSampleDistance;

      // initialize the length of the ray
      float4 fDepth = 0;

      if (flc_bSpaceLeaping)
      {
				if(!SpaceLeap(vRayNormal, flc_uXStep, flc_uYStep, flc_uZStep, flc_fXScl, flc_fYScl, flc_fZScl,
                      flc_puSLDvol, flc_fDfbScale, flc_fDfbSpaceLeapConversationFactor, flc_fDfbSpaceLeapThreshold,
                      fLeapDistance, vPoint, fDepth, flc_uVolMaxX, flc_uVolMaxY, flc_uVolMaxZ))
					// Ray hits nothing
					return;
      } // endif flc_bSpaceLeaping
      
      // jitter the starting position along the ray
      float4 fJitterDistance = m_jitter.GetValue(u,v);
      vPoint -= vIntervalVector * fJitterDistance;
      fDepth -= fJitterDistance;
      
      while (fAccumulatedOpacity < m_fOpacityThreshold)
      {
        //m_sampleCount++;

        FastTruncFloatToIntXYZ(uVoxelX, uVoxelY, uVoxelZ,(vPoint.m_x*flc_fXScl),(vPoint.m_y*flc_fYScl),(vPoint.m_z*flc_fZScl));
        float4 fXDiff = vPoint.m_x*flc_fXScl - uVoxelX;
        float4 fYDiff = vPoint.m_y*flc_fYScl - uVoxelY;
        float4 fZDiff = vPoint.m_z*flc_fZScl - uVoxelZ;
        
        if (uVoxelX < 2 || uVoxelX > flc_uVolMaxX - 2 ||  
            uVoxelY < 2 || uVoxelY > flc_uVolMaxY - 2 ||  
            uVoxelZ < 2 || uVoxelZ > flc_uVolMaxZ - 2)
        {
          // outside the volume -> stop the ray casting
          fAccumulatedOpacity = m_fOpacityThreshold;
          break;
        }

        const uint4 uIndexOffset = uVoxelX * flc_uXStep + uVoxelY * flc_uYStep + uVoxelZ * flc_uZStep;
        const uint4 uIndexOffset3 = uVoxelX * flc_uXStep3 + uVoxelY * flc_uXStep3 + uVoxelZ * flc_uZStep3;

        int4 iValue;
        double fValue;
        const uint1 * const puVol = flc_puECVvol + uIndexOffset;

        
        if(flc_bUncleansed)
        {
          // Render from the Block-Uncleansed-Difference Volume
          fValue = Trilin(fXDiff,fYDiff,fZDiff,
                          GetUncleansedValue(Point<uint2>(uVoxelX  ,uVoxelY  ,uVoxelZ  )),
                          GetUncleansedValue(Point<uint2>(uVoxelX+1,uVoxelY  ,uVoxelZ  )),
                          GetUncleansedValue(Point<uint2>(uVoxelX  ,uVoxelY+1,uVoxelZ  )),
                          GetUncleansedValue(Point<uint2>(uVoxelX+1,uVoxelY+1,uVoxelZ  )),
                          GetUncleansedValue(Point<uint2>(uVoxelX  ,uVoxelY  ,uVoxelZ+1)),
                          GetUncleansedValue(Point<uint2>(uVoxelX+1,uVoxelY  ,uVoxelZ+1)),
                          GetUncleansedValue(Point<uint2>(uVoxelX  ,uVoxelY+1,uVoxelZ+1)),
                          GetUncleansedValue(Point<uint2>(uVoxelX+1,uVoxelY+1,uVoxelZ+1)));
        }
        else
        {
          fValue = Trilin(fXDiff,fYDiff,fZDiff,
                          *(puVol), 
                          *(puVol + flc_uXStep), 
                          *(puVol + flc_uYStep), 
                          *(puVol + flc_uXYStep),
                          *(puVol + flc_uZStep), 
                          *(puVol + flc_uXZStep),
                          *(puVol + flc_uYZStep),
                          *(puVol + flc_uXYZStep));
        } // endif diff volume rendering

        FastRoundDoubleToInt(iValue, fValue);
        fOpacity = flc_pfOpacitySurfaceTransferFunction[iValue];
        
        //if(fAccumulatedOpacity < 0.5F && flc_puPaintVol )
        //{
        //  VisibleSurfaceDetermination3D(fDepth, flc_puPaintVol, flc_uCurrentSeenVal-1,
        //                                flc_uXStep, flc_uYStep, flc_uZStep, flc_uXYStep, flc_uYZStep, flc_uXZStep,
        //                                flc_uXYZStep, uIndexOffset, uSurfaceCount);
        //} // endif still marking

        if (fOpacity > 0.0F)
        {
          float4 fSampleRed,fSampleGreen,fSampleBlue;

          if(flc_bRaycastPaintVol && flc_puPaintVol)
          {
            SamplePaintVol(flc_puPaintVol, flc_uCurrentSeenVal, flc_uXStep, flc_uYStep, flc_uZStep, uIndexOffset, 
                           fXDiff, fYDiff, fZDiff,
                           flc_fNotSeenRed, flc_fNotSeenGreen, flc_fNotSeenBlue,
                           flc_fSeenRed, flc_fSeenGreen, flc_fSeenBlue,
                           fSampleRed, fSampleGreen, fSampleBlue);
          }
          else
          {
            const uint1 * puPaint = flc_puPaintVol + uIndexOffset;
            if(fZDiff >= 0.5) puPaint += flc_uZStep;
            if(fYDiff >= 0.5) puPaint += flc_uYStep;
            if(fXDiff >= 0.5) puPaint += flc_uXStep;

            if( (*puPaint) & SUSPICIOUSROI && rdrGlobal.m_bDisplaySRTroi )
            {
              fSampleRed   = 245.0F;
              fSampleGreen =  82.0F;
              fSampleBlue  = 197.0F;
            }
            else if( (*puPaint) & CADROI && rdrGlobal.m_bDisplayCadFindings )
            {
              fSampleRed   = 128.0F;
              fSampleGreen = 197.0F;
              fSampleBlue  = 255.0F;
            }
            else
            {
              fSampleRed   = flc_fNotSeenRed;   //TransferFunctionVC::m_vNotSeen.m_x;
              fSampleGreen = flc_fNotSeenGreen; //TransferFunctionVC::m_vNotSeen.m_y;
              fSampleBlue  = flc_fNotSeenBlue;  //TransferFunctionVC::m_vNotSeen.m_z;
            }
          } // endif PaintVol or not

          if (flc_bShadow)
          {
            AddShadows(flc_bObliqueShadow, flc_bGridShadow, flc_bAxialShadow, flc_bSagittalShadow, flc_bCoronalShadow, flc_bSegmentHalfPlaneShadow,
                       vPoint, flc_vObliqueNormal, flc_fObliqueMinDistance, flc_fObliqueMaxDistance, flc_vOrthogonalPoint,
                       fSampleRed, fSampleGreen, fSampleBlue);
          } // flc_shadow

          if (flc_bBiopsyMode == true && 
              v > m_magicWindowStartPt.m_y && v < (m_magicWindowStartPt.m_y+m_uMagicWindowSize) && 
              u > m_magicWindowStartPt.m_x && u < (m_magicWindowStartPt.m_x+m_uMagicWindowSize) )
          {

            fSampleRed   = flc_pfRedBiopsyTransferFunction[iValue];
            fSampleGreen = flc_pfGreenBiopsyTransferFunction[iValue];
            fSampleBlue  = flc_pfBlueBiopsyTransferFunction[iValue];
            fOpacity     = flc_pfOpacityBiopsyTransferFunction[iValue];
          }
          /*else if (flc_bTestVol == true && 
                   v > m_magicWindowStartPt.m_y && v < (m_magicWindowStartPt.m_y+m_uMagicWindowSize) && 
                   u > m_magicWindowStartPt.m_x && u < (m_magicWindowStartPt.m_x+m_uMagicWindowSize) )
          {
            fSampleRed   = flc_fNotSeenRed;
            fSampleGreen = flc_fNotSeenGreen;
            fSampleBlue  = flc_fNotSeenBlue;
            fOpacity     = flc_pfOpacityBiopsyTransferFunction[iValue];
            SampleTestVol(flc_puTestVol, 1, flc_uXStep, flc_uYStep, flc_uZStep, uIndexOffset, fXDiff, fYDiff, fZDiff,
                          255.0F, 255.0F, 255.0F, fSampleRed, fSampleGreen, fSampleBlue, fOpacity);

            CalcCentralDifferenceGradient(puVol, flc_uXStep, flc_uYStep, flc_uZStep, flc_uXYStep, flc_uYZStep, flc_uXZStep, flc_uXYZStep, fXDiff, fYDiff, fZDiff, vGradVector);
						if(bReverseNormal) vGradVector = - vGradVector;
						ShadeTwoLightPhong(vLight1Normal, vLight2Normal, vGradVector, vHalfwayVector, fSampleRed, fSampleGreen, fSampleBlue);
            ClampFloatTo255RGB(fSampleRed, fSampleGreen, fSampleBlue);
          } */
          else
          { 
            
            if(flc_bUncleansed)
            {
              // Render from the Block-Uncleansed-Difference Volume
              CalcCentralDifferenceGradient(Point<uint2>(uVoxelX,uVoxelY,uVoxelZ), fXDiff, fYDiff, fZDiff, vGradVector);
            }
            else
            {
              CalcCentralDifferenceGradient(puVol, flc_uXStep, flc_uYStep, flc_uZStep, flc_uXYStep, flc_uYZStep, flc_uXZStep, flc_uXYZStep, fXDiff, fYDiff, fZDiff, vGradVector);
            } // endif diff volume rendering
						if(bReverseNormal) vGradVector = - vGradVector;
						ShadeTwoLightPhong(vLight1Normal, vLight2Normal, vGradVector, vHalfwayVector, fSampleRed, fSampleGreen, fSampleBlue);
            ClampFloatTo255RGB(fSampleRed, fSampleGreen, fSampleBlue);

          } // end biopsy mode
          
          fAccumulatedBuffer = (1.0f - fAccumulatedOpacity) * fOpacity;
          fRedValue   += fAccumulatedBuffer * fSampleRed;
          fGreenValue += fAccumulatedBuffer * fSampleGreen;
          fBlueValue  += fAccumulatedBuffer * fSampleBlue;
          fAccumulatedOpacity += fAccumulatedBuffer;

          vPoint += vIntervalVector;
          fDepth += m_fInterSampleDistance;

        }

        else
        { // in empty space, try space leaping
        
          fLeapDistance = flc_puSLDvol[uIndexOffset]*flc_fDfbScale - flc_fDfbSpaceLeapConversationFactor;
          if (fLeapDistance <= flc_fDfbSpaceLeapThreshold || !flc_bSpaceLeaping)
          {
            // if no leap, be sure to still STEP along ray
            vPoint += vIntervalVector;
            fDepth += m_fInterSampleDistance;
          }
          else
          { // space leaping after first sample
						if(!SpaceLeap(vRayNormal, flc_uXStep, flc_uYStep, flc_uZStep, flc_fXScl, flc_fYScl, flc_fZScl,
                          flc_puSLDvol, flc_fDfbScale, flc_fDfbSpaceLeapConversationFactor, flc_fDfbSpaceLeapThreshold,
                          fLeapDistance, vPoint, fDepth, flc_uVolMaxX, flc_uVolMaxY, flc_uVolMaxZ))
							// Leap outside the volume, hit nothing -> stop the ray casting
							fAccumulatedOpacity = m_fOpacityThreshold;
          } // endif leapDistance > 1
          
        } // endif opacity or space leap

        // if we just passed the first colon wall sample, store this depth
        if (fAccumulatedOpacity > ADJUST_OPACITY_THRESH && fLastOpacity <= ADJUST_OPACITY_THRESH) fDepthValue = fDepth;
        fLastOpacity = fAccumulatedOpacity;

      } // endwhile not early ray termination
      

      // saturate color
      ClampFloatTo255RGB(fRedValue, fGreenValue, fBlueValue);
			fAlphaValue = fAccumulatedOpacity*255;
			ClampFloatTo255(fAlphaValue);
} // CastSingleRayForUDview

///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~







/**
 * Get the current setting of the visible image.
 */
inline uint4 ReaderLib::VolumeRender::GetDisplaySize() const
{
  return m_uDisplaySize;
} // GetDisplaySize


/**
 * Set the size of the final image on the screen.
 *
 * @param   uDisplaySize   The size in pixels.
 */
inline void ReaderLib::VolumeRender::SetDisplaySize(const uint4 uDisplaySize)
{
  m_uDisplaySize = uDisplaySize;
}


/**
 * Get the number of actual rays cast in each dimension.
 */
inline uint4 ReaderLib::VolumeRender::GetRenderSize() const
{
  return m_uRenderSize;
} // GetRenderSize


/**
 * Get the (power of 2) size of the render buffer.
 */
inline uint4 ReaderLib::VolumeRender::GetRenderBufferSize() const
{
  return m_uRenderBufferSize;
} // GetRenderBufferSize


/**
 * Get the Depth Buffer for for the image.
 */
inline float4 * ReaderLib::VolumeRender::GetDepthBuffer() const
{
  return m_pfDepthBuffer;
} // GetDepthBuffer


// $Log: VolumeRender.inl,v $
// Revision 1.10.2.3  2009/03/05 22:22:44  dongqing
// Remove SRT and CAD ROI display switch check in VisibleSurfaceDetermination3D.
// That fixes the wired behavior of the switch.
//
// Revision 1.10.2.2  2008/03/04 15:43:53  dongqing
// fix bug for checking display option on CAD finding
//
// Revision 1.10.2.1  2007/05/03 15:23:59  dongqing
// add function CastSingleRayForUDview() to fix lumen coverage problem
//
// Revision 1.10  2007/01/26 22:33:05  dongqing
// add CAD ROI display
//
// Revision 1.9  2006/12/21 16:22:22  dongqing
// Add the switch for display SRT ROI on/off
//
// Revision 1.8  2006/10/04 20:53:06  frank
// disabled the 3D segments shadow
//
// Revision 1.7  2006/09/26 19:37:04  frank
// replaced hardcoded numbers with constants
//
// Revision 1.6  2006/09/22 16:34:44  dongqing
// show the Suspicious ROI color coding in non-paint mode also
//
// Revision 1.5  2006/09/20 18:51:59  dongqing
// 1st code adjustment for the Suspicious region tool
//
// Revision 1.4  2006/09/13 13:06:23  dongqing
// add temporary polyp segmentation display code in paint volume codes
//
// Revision 1.3  2005/10/28 13:32:04  geconomos
// added ability to render into a fixed or adpative image buffer
//
// Revision 1.2  2005/09/29 13:33:13  vxconfig
// made the difference volume a new vx block volume
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.3  2005/09/01 19:28:19  geconomos
// Issue# 4392: Paint volume was not saving properly. Error turned out to be in the linear volume class.
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.12.2.6.2.1.6.1  2005/06/28 18:03:17  geconomos
// Issue #4202: Fixed 3D rendering hang-up when rendering MR case from outside the colon.
//
// Revision 3.12.2.6.2.1  2005/03/15 21:11:45  frank
// Issue #4021: removed polyp segmentation coloring during flythrough
//
// Revision 3.12.2.6  2004/11/20 15:28:33  dongqing
// back using the paint volume
//
// Revision 3.12.2.5  2004/11/08 14:25:44  dongqing
// add temporary solution for display segmentation results
//
// Revision 3.12.2.4  2004/10/18 02:25:08  jmeade
// Different marking color for polyp segmentation.
//
// Revision 3.12.2.3  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.12.2.2  2003/02/21 21:40:02  jmeade
// One-line FlushAccumulationBuffer() method made inline.
//
// Revision 3.12.2.1  2003/02/21 19:14:56  jmeade
// One-line FlushAccumulationBuffer() method made inline.
//
// Revision 3.12  2003/01/22 22:29:56  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.11  2002/11/24 17:43:44  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
//
// Revision 3.10  2002/08/28 19:29:09  kevin
// Lots of updates to segmental viewing
//
// Revision 3.9  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.8  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.7.2.1  2002/02/06 23:52:51  kevin
// Added color schemes to handle paintnig information for color blind people (NOTE: we should
// expand this to all our color information in the next release. e.g. centerline color, bookmark color etc)
//
// Revision 3.7  2002/01/10 18:04:09  kevin
// Made diff volumes lazy-load since they will not be used in the common case
//
// Revision 3.6  2001/12/20 22:43:59  binli
// 3D measurement line: basic function (to be improved)
//
// Revision 3.5  2001/11/14 01:34:16  kevin
// Cleaned up alot of Supine/Proneregistration stuff, fixed some
// of the multi-rendering and added feature match pair rendering
//
// Revision 3.4  2001/11/11 19:55:31  kevin
// Fixes for rdrGlobal.m_pStudy when renderin 2 endoscopic views in registration mode
//
// Revision 3.3  2001/11/06 15:25:02  kevin
// Added a get uncleansed sample method which utilizes the diff volume
//
// Revision 3.2  2001/01/13 09:47:40  kevin
// Fixed the bug in uncleased rendering where both supine and prone datasets
// pointed the curent study for the block diff volume
//
// Revision 3.1  2001/11/01 19:52:59  kevin
// Added 3D Uncleansed DIff-Block_volume rendering
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:24   ingmar
// Initial revision.
// Revision 1.41  2001/07/30 16:30:36  liwei
// TransferFunction renamed to TransferFunctionVC.
//
// Revision 1.40  2001/06/26 16:19:56  ana
// Results of code review for Fields and Jitter classes
//
// Revision 1.39  2001/05/29 17:25:40  jmeade
// Fixed changes from (and continued) code review, particularly with translucent window
//
// Revision 1.38  2001/05/25 20:29:00  kevin
// Various coding conventions related to ReaderGlobal and VolumeRender
//
// Revision 1.37  2001/05/07 00:51:27  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.36  2001/04/30 02:20:03  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.35  2001/04/29 22:42:41  kevin
// Variable naming for coding standards in VolumeRender Class
//
// Revision 1.34  2001/04/11 20:50:26  kevin
// Coding Standard updates of variable names, types and
// comments in the VolumeRender class.
//
