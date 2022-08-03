// $Id: LinearVolRayIterator.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/LinearVolRayIterator.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)
//
// Complete History at bottom of file.


#ifndef LinearVolRayIterator_h
#define LinearVolRayIterator_h

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
 * Macro for central difference gradients.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
#define CentralDifferenceVoxelGradientDoubledRaw(pVol, xStep, yStep, zStep, gradX, gradY, gradZ) \
  gradX = ( *( (pVol) + xStep ) - *( (pVol) - xStep ) );                      \
  gradY = ( *( (pVol) + yStep ) - *( (pVol) - yStep ) );                      \
  gradZ = ( *( (pVol) + zStep ) - *( (pVol) - zStep ) );


/**
 * Macro for central difference gradients.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
#define CentralDifferenceVoxelGradientDoubled12bitRaw(pVol, xStep, yStep, zStep, gradX, gradY, gradZ) \
  gradX = ( (0x0FFF & (*( (pVol) + xStep ))) - (0x0FFF & (*( (pVol) - xStep ))) );                    \
  gradY = ( (0x0FFF & (*( (pVol) + yStep ))) - (0x0FFF & (*( (pVol) - yStep ))) );                    \
  gradZ = ( (0x0FFF & (*( (pVol) + zStep ))) - (0x0FFF & (*( (pVol) - zStep ))) );


/**
 * Fast assembly code to round double to int (for a 3-item vector).
 * It seems that if iOutX etc. is a variable in a structure, such as  Triple.m_x
 * It has to use reference  uint2 & iTmpX = iOutX otherwise assembly code can not
 * get compiled
 */
#define FastRoundFloatToIntXYZ(iOutX, iOutY, iOutZ, fInX, fInY, fInZ) \
  double fTmpX = fInX;                                                   \
  double fTmpY = fInY;                                                   \
  double fTmpZ = fInZ;                                                   \
  __asm FLD   QWORD PTR [fTmpX]                                          \
  __asm FISTP DWORD PTR [iOutX]                                          \
  __asm FLD   QWORD PTR [fTmpY]                                          \
  __asm FISTP DWORD PTR [iOutY]                                          \
  __asm FLD   QWORD PTR [fTmpZ]                                          \
  __asm FISTP DWORD PTR [iOutZ]


/**
 * Fast assembly code to trunc double to int (for a 3-item vector).
 * It seems that if iOutX etc. is a variable in a structure, such as  Triple.m_x
 * It has to use reference  uint2 & iTmpX = iOutX otherwise assembly code can not
 * get compiled
 */
#define FastTruncFloatToIntXYZ(iOutX, iOutY, iOutZ, fInX, fInY, fInZ) \
  double fTmpX = fInX-0.5;                                               \
  double fTmpY = fInY-0.5;                                               \
  double fTmpZ = fInZ-0.5;                                               \
  __asm FLD   QWORD PTR [fTmpX]                                          \
  __asm FISTP DWORD PTR [iOutX]                                          \
  __asm FLD   QWORD PTR [fTmpY]                                          \
  __asm FISTP DWORD PTR [iOutY]                                          \
  __asm FLD   QWORD PTR [fTmpZ]                                          \
  __asm FISTP DWORD PTR [iOutZ]


/**
 * An iterator that incrementally traverses  voxels along a ray.
 * $Id: LinearVolRayIterator.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a>
 */
class RayIterator : public VolIterator 
{

// Member Functions
public:

  /// CONSTRUCTOR 

  RayIterator(const Point<float4> startPoint , const Triple<uint4> & dim,
       const T & background, const typename std::vector<T>::iterator baseAddr, 
       const Vector<float> & directionVector, const float4 fSampleDistance):
    VolIterator(Point<uint2>(static_cast<uint2>(startPoint.m_x), 
                             static_cast<uint2>(startPoint.m_y),
                             static_cast<uint2>(startPoint.m_z)), dim, background, baseAddr),
    m_floatPos(startPoint), 
    m_directionVector(directionVector), 
    m_fSampleDistance(fSampleDistance)
  {
    m_unitSamplePt = directionVector * m_fSampleDistance; 
    m_backgroundFloat = static_cast<float4>(background);
    SetSldDeltaVector();
  }
    
  /// VOLUME TRAVERSAL

  /// Centers the iterator at the location.
  inline void CenterAt(const Point<float4> & pos)
  {
    m_floatPos = pos;
    
    m_pos.m_x = static_cast<uint2>(m_floatPos.m_x);
    m_pos.m_y = static_cast<uint2>(m_floatPos.m_y);
    m_pos.m_z = static_cast<uint2>(m_floatPos.m_z);
 
    m_pVox = m_pBaseAddr + GetOffset(m_pos);    
  }

  /// Set sample distance.
  inline void SetSampleDistance(const float4 fSampleDistance)
  {
    m_fSampleDistance = fSampleDistance;
    m_unitSamplePt = m_directionVector * m_fSampleDistance; 
    // m_unitSamplePt *= fSampleDistance / m_fSampleDistance; 
    // m_fSampleDistance = fSampleDistance;
    SetSldDeltaVector();
  }

  /// Set direction vector.
  inline void SetDirectionVector(const Vector<float> & directionVector)
  {
    m_directionVector = directionVector;
    m_unitSamplePt = m_directionVector * m_fSampleDistance; 
    SetSldDeltaVector();
  }

  inline void SetSldDeltaVector()
  {
    m_sldDeltaVector = m_directionVector;
    const float4 radius(8.0F);
    m_sldDeltaVector *= radius/Max(Abs(m_directionVector.m_x), Abs(m_directionVector.m_y), Abs(m_directionVector.m_z));
  } // SetSldDeltaVector()
  
  
  /// is true when the traversal is finished.
  inline bool IsEnd() const
  {
    return ( !InsideVolume(m_pos) );
  }
    
  // SET & GET VOXELS

  /// Returns the value at the current location: Safe version.
  inline const T & GetFloorVoxel() const
  {
    if ( InsideVolume(m_pos) )
      return (*m_pVox);
    else
      return (m_background);
  }

  /// Returns the value at the current location: Safe version.
  inline const T & GetNearestVoxel() 
  {
    Point<uint2> m_nearestPos;
      
    m_nearestPos.m_x = static_cast<uint2>(m_floatPos.m_x + 0.5);
    m_nearestPos.m_y = static_cast<uint2>(m_floatPos.m_y + 0.5);
    m_nearestPos.m_z = static_cast<uint2>(m_floatPos.m_z + 0.5);
             
    if ( InsideVolume(m_nearestPos) )
      return *(m_pBaseAddr + GetOffset(m_nearestPos));
    else
      return (m_background);
  }

  /// Returns the value at the current location: Safe version.
  inline const float4 & GetResampledVoxel() 
  {
    if ( InsideVolume(m_pos) == false )
    {
      return (m_backgroundFloat);
    }
    return GetResampledVoxelRaw();
  }
  
  
  /// Returns the value at the current location: Un-Safe version.
  inline const float4 & GetResampledVoxelRaw(int2 mask) 
  {
    // Compute weights for tri-linear interpolation.
    float4 fXDiff = m_floatPos.m_x - m_pos.m_x;
    float4 fYDiff = m_floatPos.m_y - m_pos.m_y;
    float4 fZDiff = m_floatPos.m_z - m_pos.m_z;

    // Compute the resampled value by tri-linear interpolation.
    m_resampledVoxel= Trilin(fXDiff,fYDiff,fZDiff,
                            (mask & (*(m_pVox))), 
                            (mask & (*(m_pVox + 1))), 
                            (mask & (*(m_pVox + m_yStep))), 
                            (mask & (*(m_pVox + 1 + m_yStep))),
                            (mask & (*(m_pVox + m_zStep))), 
                            (mask & (*(m_pVox + 1 + m_zStep))),
                            (mask & (*(m_pVox + m_yStep + m_zStep))),
                            (mask & (*(m_pVox + 1 + m_yStep + m_zStep))));
    
    return (m_resampledVoxel);
  }
  
  
  /// Returns the resampled gradient value at the current location: Safe version.
  inline const Vector<float4> & GetResampledGradient() 
  {
    if(m_pos.m_x < m_dim.m_x - 2 && m_pos.m_y < m_dim.m_y - 2 && m_pos.m_z < m_dim.m_z - 2 &&
       m_pos.m_x > 1 && m_pos.m_y > 1 && m_pos.m_z > 1 ) {      
      return GetResampledGradientRaw();
    }
    return m_gradientVector;
  }


  /// Returns the resampled gradient value at the current location: Un-Safe version.
  inline const Vector<float4> & GetResampledGradientRaw() 
  {
    // weights for tri-linear interpolation
    float4 fXDiff = m_floatPos.m_x - m_pos.m_x;
    float4 fYDiff = m_floatPos.m_y - m_pos.m_y;
    float4 fZDiff = m_floatPos.m_z - m_pos.m_z;

    CentralDifferenceSampleGradientDoubledRaw(m_pVox, 1 , m_yStep, m_zStep,
      1 + m_yStep, m_yStep + m_zStep, 1 + m_zStep, 1 + m_yStep + m_zStep, 
      fXDiff, fYDiff, fZDiff, m_gradientVector);
    m_gradientVector.Normalize();  // TODO: more Normalize() out of GetGrad
    return m_gradientVector;
  }

  
  /// Returns the resampled gradient value at the current location: Un-Safe version.
  inline const Vector<float4> & GetResampledGradient12bitRaw() 
  {
    // weights for tri-linear interpolation
    float4 fXDiff = m_floatPos.m_x - m_pos.m_x;
    float4 fYDiff = m_floatPos.m_y - m_pos.m_y;
    float4 fZDiff = m_floatPos.m_z - m_pos.m_z;
    
    CentralDifferenceSampleGradientDoubled12bitRaw(m_pVox, 1 , m_yStep, m_zStep,
      1 + m_yStep, m_yStep + m_zStep, 1 + m_zStep, 1 + m_yStep + m_zStep, 
      fXDiff, fYDiff, fZDiff, m_gradientVector);
    m_gradientVector.Normalize();  // TODO: more Normalize() out of GetGrad
    return m_gradientVector;
  }

  
  /// Next sampling point along the ray.
  void operator++()
  {
    m_floatPos += m_unitSamplePt;    
    
    m_pos.m_x = static_cast<uint2>(m_floatPos.m_x);
    m_pos.m_y = static_cast<uint2>(m_floatPos.m_y);
    m_pos.m_z = static_cast<uint2>(m_floatPos.m_z);
   
    m_pVox = m_pBaseAddr + GetOffset(m_pos);    
  }

  
  /// space leap along the ray.
  void SpaceLeap()
  {
    m_floatPos += m_sldDeltaVector;    
    
    m_pos.m_x = static_cast<uint2>(m_floatPos.m_x);
    m_pos.m_y = static_cast<uint2>(m_floatPos.m_y);
    m_pos.m_z = static_cast<uint2>(m_floatPos.m_z);
    
    m_pVox = m_pBaseAddr + GetOffset(m_pos);    
  }
  

  /// Next nonzero sampling point along the ray.
  void NextNonZero()
  {
    do {
      m_floatPos += m_unitSamplePt;   
      
      m_pos.m_x = static_cast<uint2>(m_floatPos.m_x);
      m_pos.m_y = static_cast<uint2>(m_floatPos.m_y);
      m_pos.m_z = static_cast<uint2>(m_floatPos.m_z);
          
      m_pVox = m_pBaseAddr + GetOffset(m_pos);    
    } while(InsideVolume(m_pos) && (*m_pVox == m_background));
  }
    
  /// STATUS REPORTS

  /// Returns the current x,y,z position.
  inline const Point<float4> & GetPos() const
  {
    return m_floatPos;
  }

 
protected:

  /**
 * Calculate the gradient vector of a linear volume.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
  inline void CentralDifferenceSampleGradientDoubledRaw(typename std::vector<T>::iterator puVol, const uint4 &flc_uXStep,
                                                        const uint4 &flc_uYStep, const uint4 &flc_uZStep,
                                                        const uint4 &flc_uXYStep, const uint4 &flc_uYZStep,
                                                        const uint4 &flc_uXZStep, const uint4 &flc_uXYZStep,
                                                        const float4 &fXDiff, const float4 &fYDiff, const float4 &fZDiff,
                                                        Vector<float4> &m_gradientVector) 
  {
    //  TODO: local copy of all 40 voxels involved may make this faster
    //        for this the overall result of the computation below has to be worked out on paper
    float4 fGrad000x, fGrad100x, fGrad010x, fGrad110x, fGrad001x, fGrad101x, fGrad011x, fGrad111x;
    float4 fGrad000y, fGrad100y, fGrad010y, fGrad110y, fGrad001y, fGrad101y, fGrad011y, fGrad111y;
    float4 fGrad000z, fGrad100z, fGrad010z, fGrad110z, fGrad001z, fGrad101z, fGrad011z, fGrad111z;
    
    CentralDifferenceVoxelGradientDoubledRaw(puVol             , flc_uXStep, flc_uYStep, flc_uZStep, fGrad000x, fGrad000y, fGrad000z);
    CentralDifferenceVoxelGradientDoubledRaw(puVol+flc_uXStep  , flc_uXStep, flc_uYStep, flc_uZStep, fGrad100x, fGrad100y, fGrad100z);
    CentralDifferenceVoxelGradientDoubledRaw(puVol+flc_uYStep  , flc_uXStep, flc_uYStep, flc_uZStep, fGrad010x, fGrad010y, fGrad010z);
    CentralDifferenceVoxelGradientDoubledRaw(puVol+flc_uXYStep , flc_uXStep, flc_uYStep, flc_uZStep, fGrad110x, fGrad110y, fGrad110z);
    CentralDifferenceVoxelGradientDoubledRaw(puVol+flc_uZStep  , flc_uXStep, flc_uYStep, flc_uZStep, fGrad001x, fGrad001y, fGrad001z);
    CentralDifferenceVoxelGradientDoubledRaw(puVol+flc_uXZStep , flc_uXStep, flc_uYStep, flc_uZStep, fGrad101x, fGrad101y, fGrad101z);
    CentralDifferenceVoxelGradientDoubledRaw(puVol+flc_uYZStep , flc_uXStep, flc_uYStep, flc_uZStep, fGrad011x, fGrad011y, fGrad011z);
    CentralDifferenceVoxelGradientDoubledRaw(puVol+flc_uXYZStep, flc_uXStep, flc_uYStep, flc_uZStep, fGrad111x, fGrad111y, fGrad111z);
    
    m_gradientVector.m_x = float4(Trilin(fXDiff, fYDiff, fZDiff,
      fGrad000x, fGrad100x, fGrad010x, fGrad110x,
      fGrad001x, fGrad101x, fGrad011x, fGrad111x));
    
    m_gradientVector.m_y = float4(Trilin(fXDiff, fYDiff, fZDiff,
      fGrad000y, fGrad100y, fGrad010y, fGrad110y,
      fGrad001y, fGrad101y, fGrad011y, fGrad111y));
    
    m_gradientVector.m_z = float4(Trilin(fXDiff, fYDiff, fZDiff,
      fGrad000z, fGrad100z, fGrad010z, fGrad110z,
      fGrad001z, fGrad101z, fGrad011z, fGrad111z));
    
  } // CentralDifferenceSampleGradientDoubledRaw
  

  /**
 * Calculate the gradient vector of a linear volume.
 *
 * @param   MANY   With the exact same names both inside and outside the function.
 */
  inline void CentralDifferenceSampleGradientDoubled12bitRaw(typename std::vector<T>::iterator puVol, const uint4 &flc_uXStep,
                                                        const uint4 &flc_uYStep, const uint4 &flc_uZStep,
                                                        const uint4 &flc_uXYStep, const uint4 &flc_uYZStep,
                                                        const uint4 &flc_uXZStep, const uint4 &flc_uXYZStep,
                                                        const float4 &fXDiff, const float4 &fYDiff, const float4 &fZDiff,
                                                        Vector<float4> &m_gradientVector) 
  {
    //  TODO: local copy of all 40 voxels involved may make this faster
    //        for this the overall result of the computation below has to be worked out on paper
    float4 fGrad000x, fGrad100x, fGrad010x, fGrad110x, fGrad001x, fGrad101x, fGrad011x, fGrad111x;
    float4 fGrad000y, fGrad100y, fGrad010y, fGrad110y, fGrad001y, fGrad101y, fGrad011y, fGrad111y;
    float4 fGrad000z, fGrad100z, fGrad010z, fGrad110z, fGrad001z, fGrad101z, fGrad011z, fGrad111z;
    
    CentralDifferenceVoxelGradientDoubled12bitRaw(puVol             , flc_uXStep, flc_uYStep, flc_uZStep, fGrad000x, fGrad000y, fGrad000z);
    CentralDifferenceVoxelGradientDoubled12bitRaw(puVol+flc_uXStep  , flc_uXStep, flc_uYStep, flc_uZStep, fGrad100x, fGrad100y, fGrad100z);
    CentralDifferenceVoxelGradientDoubled12bitRaw(puVol+flc_uYStep  , flc_uXStep, flc_uYStep, flc_uZStep, fGrad010x, fGrad010y, fGrad010z);
    CentralDifferenceVoxelGradientDoubled12bitRaw(puVol+flc_uXYStep , flc_uXStep, flc_uYStep, flc_uZStep, fGrad110x, fGrad110y, fGrad110z);
    CentralDifferenceVoxelGradientDoubled12bitRaw(puVol+flc_uZStep  , flc_uXStep, flc_uYStep, flc_uZStep, fGrad001x, fGrad001y, fGrad001z);
    CentralDifferenceVoxelGradientDoubled12bitRaw(puVol+flc_uXZStep , flc_uXStep, flc_uYStep, flc_uZStep, fGrad101x, fGrad101y, fGrad101z);
    CentralDifferenceVoxelGradientDoubled12bitRaw(puVol+flc_uYZStep , flc_uXStep, flc_uYStep, flc_uZStep, fGrad011x, fGrad011y, fGrad011z);
    CentralDifferenceVoxelGradientDoubled12bitRaw(puVol+flc_uXYZStep, flc_uXStep, flc_uYStep, flc_uZStep, fGrad111x, fGrad111y, fGrad111z);
    
    m_gradientVector.m_x = float4(Trilin(fXDiff, fYDiff, fZDiff,
      fGrad000x, fGrad100x, fGrad010x, fGrad110x,
      fGrad001x, fGrad101x, fGrad011x, fGrad111x));
    
    m_gradientVector.m_y = float4(Trilin(fXDiff, fYDiff, fZDiff,
      fGrad000y, fGrad100y, fGrad010y, fGrad110y,
      fGrad001y, fGrad101y, fGrad011y, fGrad111y));
    
    m_gradientVector.m_z = float4(Trilin(fXDiff, fYDiff, fZDiff,
      fGrad000z, fGrad100z, fGrad010z, fGrad110z,
      fGrad001z, fGrad101z, fGrad011z, fGrad111z));
    
  } // CentralDifferenceSampleGradientDoubledRaw

  
  /// Returns true if the iterator is inside the volume. Stronger version than that of  base class.
  inline bool InsideVolume(const Point<uint2> & pos) const
  {
    return( pos.m_x <= m_dim.m_x-3 && pos.m_y <= m_dim.m_y - 3 && pos.m_z < m_dim.m_z - 3 &&
            pos.m_x >= 2 && pos.m_y >= 2 && pos.m_z >= 2 );
  }
   
// Data Members
protected:

  /// The exact position of sampling points. m_pos is the floor voxel.
  Point<float4> m_floatPos;

  /// The unit sample distance point.
  Point<float4> m_unitSamplePt;  /// vector<> m_deltaSamplePt

  /// The ray direction.
  Vector<float4> m_directionVector;  /// obsolete, use above

  /// space leap jump vector
  Vector<float4> m_sldDeltaVector; 

  /// The sampling distance along the ray.
  float4 m_fSampleDistance;

  /// The gradient vector.
  Vector<float4> m_gradientVector;

  /// The resampled voxel value.
  float4 m_resampledVoxel;

  /// The background voxel. 
  float4 m_backgroundFloat;

}; // class LinearVolRayIterator

#undef FastTruncFloatToIntXYZ

#endif // LinearVolRayIterator_h


// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/LinearVolRayIterator.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $
// $Id: LinearVolRayIterator.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $
