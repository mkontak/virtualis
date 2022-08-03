// $Id: BlockVolRayIterator.h,v 1.2 2005/04/27 17:36:12 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/BlockVolRayIterator.h,v 1.2 2005/04/27 17:36:12 geconomos Exp $
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


#ifndef BlockVolRayIterator_h
#define BlockVolRayIterator_h

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
#define BlockVolCentralDifferenceGradient(gradX, gradY, gradZ) \
  gradX = ( PeekNextX() - PeekPrevX() ) / 2;                      \
  gradY = ( PeekNextY() - PeekPrevY() ) / 2;                      \
  gradZ = ( PeekNextZ() - PeekPrevZ() ) / 2;

/**
 * Fast assembly code to round double to int (for a 3-item vector).
 * It seems that if iOutX etc. is a variable in a structure, such as  Triple.m_x
 * It has to use reference  uint2 & iTmpX = iOutX otherwise assembly code can not
 * get compiled
 */
#define FastRoundFloatToIntTriple(iOutX, iOutY, iOutZ, fInX, fInY, fInZ) \
  double fTmpX = fInX;                                                   \
  double fTmpY = fInY;                                                   \
  double fTmpZ = fInZ;                                                   \
  uint2 & iTmpX = iOutX;                                                 \
  uint2 & iTmpY = iOutY;                                                 \
  uint2 & iTmpZ = iOutZ;                                                 \
  __asm FLD   QWORD PTR [fTmpX]                                          \
  __asm FISTP DWORD PTR [iTmpX]                                          \
  __asm FLD   QWORD PTR [fTmpY]                                          \
  __asm FISTP DWORD PTR [iTmpY]                                          \
  __asm FLD   QWORD PTR [fTmpZ]                                          \
  __asm FISTP DWORD PTR [iTmpZ]


/**
 * Fast assembly code to trunc double to int (for a 3-item vector).
 * It seems that if iOutX etc. is a variable in a structure, such as  Triple.m_x
 * It has to use reference  uint2 & iTmpX = iOutX otherwise assembly code can not
 * get compiled
 */
#define FastTruncFloatToIntTriple(iOutX, iOutY, iOutZ, fInX, fInY, fInZ) \
  double fTmpX = fInX-0.5;                                               \
  double fTmpY = fInY-0.5;                                               \
  double fTmpZ = fInZ-0.5;                                               \
  uint2 & iTmpX = iOutX;                                                 \
  uint2 & iTmpY = iOutY;                                                 \
  uint2 & iTmpZ = iOutZ;                                                 \
  __asm FLD   QWORD PTR [fTmpX]                                          \
  __asm FISTP DWORD PTR [iTmpX]                                          \
  __asm FLD   QWORD PTR [fTmpY]                                          \
  __asm FISTP DWORD PTR [iTmpY]                                          \
  __asm FLD   QWORD PTR [fTmpZ]                                          \
  __asm FISTP DWORD PTR [iTmpZ]

/**
 * An iterator that incrementally traverses  voxels along a ray.
 * $Id: BlockVolRayIterator.h,v 1.2 2005/04/27 17:36:12 geconomos Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a>
 */
class RayIterator : public VolIterator 
{

// Member Functions
public:

  /// CONSTRUCTOR 

  RayIterator(const Point<float4> startPoint , const Triple<uint4> & dim,
              const T & background, BlockVector<T> & blocks, 
              const uint4 & blockBitsX, 
              const uint4 & blockBitsXY, 
              const Vector<float> & directionVector, const float4 fSampleDistance):
     VolIterator(Point<uint2>(static_cast<uint2>(startPoint.m_x), 
                             static_cast<uint2>(startPoint.m_y),
                             static_cast<uint2>(startPoint.m_z)), dim, background, blocks,
                             blockBitsX, blockBitsXY),
     m_floatPos(startPoint), m_directionVector(directionVector), 
     m_fSampleDistance(fSampleDistance)
  {
    m_unitSamplePt = Point3D<float4>( m_directionVector * m_fSampleDistance );  
    m_backgroundFloat = static_cast<float4>(background);
  }
    
  /// VOLUME TRAVERSAL

  /// Centers the iterator at the location.
  inline void CenterAt(const Point<float4> & pos)
  {
    m_floatPos = pos;
        
    m_pos.m_x = static_cast<uint2>(m_floatPos.m_x);
    m_pos.m_y = static_cast<uint2>(m_floatPos.m_y);
    m_pos.m_z = static_cast<uint2>(m_floatPos.m_z);
         
    m_blockOffset = GetBlockIndexV(m_pos);
    uint4 block = GetBlockV(m_pos);
    if ( block < m_rBlocks.size() )
      m_iterBlock = &m_rBlocks[block];
    else
      m_iterBlock = &m_rBlocks[m_rBlocks.size()-1]; //background block
  }

  /// Set direction vector.
  inline void SetDirectionVector(const Vector<float> & directionVector)
  {
    m_directionVector = directionVector;
    m_unitSamplePt = m_directionVector * m_fSampleDistance; 
  }

  /// Set sample distance.
  inline void SetSampleDistance(const float4 fSampleDistance)
  {
    m_fSampleDistance = fSampleDistance;
    m_unitSamplePt = m_directionVector * m_fSampleDistance; 
  }

  /// is true when the traversal is finished.
  inline bool IsEnd() const
  {
    if ( InsideVolume(m_pos) )
      return (false);
    else
      return (true);
  }
    
  // SET & GET VOXELS

  /// Returns the value at the current location: Safe version.
  inline const T & GetFloorVoxel() const
  {
    if ( InsideVolume(m_pos) )
      return( m_iterBlock->At(m_blockOffset) );
    else
      return (m_backgroundVoxel);
  }

  /// Returns the value at the current location: Safe version.
  inline const T & GetNearestVoxel() const
  {
    Point<uint2> m_nearestPos;

    m_nearestPos.m_x = static_cast<uint2>(m_floatPos.m_x + 0.5);
    m_nearestPos.m_y = static_cast<uint2>(m_floatPos.m_y + 0.5);
    m_nearestPos.m_z = static_cast<uint2>(m_floatPos.m_z + 0.5);
                              
    if(InsideVolume(m_nearestPos))
    {
      return(m_rBlocks[GetBlockV(m_nearestPos)].At(GetBlockIndexV(m_nearestPos)));
    } 
    else
    {
      return(m_backgroundVoxel);
    }
  }

  /// Returns the value at the current location: Safe version.
  inline const float4  GetResampledVoxel() 
  {

    if ( InsideVolume(m_pos) == false )
    {
      return (m_backgroundFloat);
    }

    // weights for tri-linear interpolation
    float4 fXDiff = m_floatPos.m_x - m_pos.m_x;
    float4 fYDiff = m_floatPos.m_y - m_pos.m_y;
    float4 fZDiff = m_floatPos.m_z - m_pos.m_z;

    double fValue = Trilin(fXDiff,fYDiff,fZDiff,
                            GetFloorVoxel(), 
                            PeekNextX(), 
                            PeekNextY(), 
                            PeekNextXY(),
                            PeekNextZ(), 
                            PeekNextXZ(),
                            PeekNextYZ(),
                            PeekNextXYZ());
        
    return (fValue);
  }
  
  
  /// Returns the resampled gradient value at the current location: Safe version.
  inline Vector<float4>  GetResampledGradient() 
  {
    // weights for tri-linear interpolation
    float4 fXDiff = m_floatPos.m_x - m_pos.m_x;
    float4 fYDiff = m_floatPos.m_y - m_pos.m_y;
    float4 fZDiff = m_floatPos.m_z - m_pos.m_z;

    if(m_pos.m_x < m_dim.m_x - 1 && m_pos.m_y < m_dim.m_y - 1 && m_pos.m_z < m_dim.m_z - 1 &&
       m_pos.m_x > 0 && m_pos.m_y > 0 && m_pos.m_z >0 ) {      
      CalcCentralDifferenceGradient(fXDiff, fYDiff, fZDiff, m_gradientVector);
    }
    
    m_gradientVector.Normalize();
    return m_gradientVector;
  }
  
  /// Next sampling point along the ray.
  void operator++()
  {
    m_floatPos += m_unitSamplePt;    
    
    m_pos.m_x = static_cast<uint2>(m_floatPos.m_x);
    m_pos.m_y = static_cast<uint2>(m_floatPos.m_y);
    m_pos.m_z = static_cast<uint2>(m_floatPos.m_z);
    
    m_blockOffset = GetBlockIndexV(m_pos);
 
    uint4 block = GetBlockV(m_pos);
    if ( block < m_rBlocks.size() )
      m_iterBlock = &m_rBlocks[block];
    else
      m_iterBlock = &m_rBlocks[m_rBlocks.size()-1]; //background block
  }
  
  /// Next nonzero sampling point along the ray.
  void NextNonZero()
  {
    do 
    {
      m_floatPos += m_unitSamplePt;    
      
      m_pos.m_x = static_cast<uint2>(m_floatPos.m_x);
      m_pos.m_y = static_cast<uint2>(m_floatPos.m_y);
      m_pos.m_z = static_cast<uint2>(m_floatPos.m_z);
      
      m_blockOffset = GetBlockIndexV(m_pos);
      uint4 block = GetBlockV(m_pos);
      if ( block < m_rBlocks.size() )
        m_iterBlock = &m_rBlocks[block];
      else
        m_iterBlock = &m_rBlocks[m_rBlocks.size()-1]; //background block
    } while ( InsideVolume() && (m_backgroundVoxel == m_iterBlock->At(m_blockOffset)) );
  }
    
  /// STATUS REPORTS

  /// Returns the current x,y,z position.
  inline const Point<float4> & GetPos() const
  {
    return m_floatPos;
  }

  /// Returns true if the iterator is inside the volume.
  inline bool InsideVolume() const
  {
    return(InsideVolume(m_pos));
  }

protected:

/**
 * Calculate the gradient vector of a block volume.
 *
 * @param  
 */
  inline void CalcCentralDifferenceGradient(  const float4 &fXDiff, const float4 &fYDiff, 
                                              const float4 &fZDiff, Vector<float4> &vGradVector)
  {
    float4 fGrad000x, fGrad100x, fGrad010x, fGrad110x, fGrad001x, fGrad101x, fGrad011x, fGrad111x;
    float4 fGrad000y, fGrad100y, fGrad010y, fGrad110y, fGrad001y, fGrad101y, fGrad011y, fGrad111y;
    float4 fGrad000z, fGrad100z, fGrad010z, fGrad110z, fGrad001z, fGrad101z, fGrad011z, fGrad111z;
    
    BlockVolCentralDifferenceGradient(fGrad000x, fGrad000y, fGrad000z);  IncX();
    BlockVolCentralDifferenceGradient(fGrad100x, fGrad100y, fGrad100z);  IncY();      
    BlockVolCentralDifferenceGradient(fGrad110x, fGrad110y, fGrad110z);  DecX();
    BlockVolCentralDifferenceGradient(fGrad010x, fGrad010y, fGrad010z);  DecY();  IncZ();   
    BlockVolCentralDifferenceGradient(fGrad001x, fGrad001y, fGrad001z);  IncX();
    BlockVolCentralDifferenceGradient(fGrad101x, fGrad101y, fGrad101z);  IncY();
    BlockVolCentralDifferenceGradient(fGrad111x, fGrad111y, fGrad111z);  DecX();    
    BlockVolCentralDifferenceGradient(fGrad011x, fGrad011y, fGrad011z);  DecY();  DecZ();
    
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
  

  /// Returns true if the iterator is inside the volume. Stronger version than that of  base class.
  inline bool InsideVolume(const Point<uint2> & pos) const
  {
    return( pos.m_x<= m_dim.m_x -3 && pos.m_y<=m_dim.m_y-3 && pos.m_z<=m_dim.m_z-3 &&
            pos.m_x>=2 && pos.m_y>=2 && pos.m_z>=2 );
  }
  
   
  /// INCREMENTAL PEEK AT SOME VOXEL
  
  /// Peeks at the next voxel in the xy direction w/out moving the iterator.
  inline const T & PeekNextXY() const
  {
    if ( ((m_pos.m_x + 1 & 7) != 0) &&  ((m_pos.m_y + 1 & 7) != 0) )
      return( m_iterBlock->At(m_blockOffset + 1 + m_offsetX) );
    else
      return(GetVoxelV(Point<uint2>(m_pos.m_x + 1, m_pos.m_y + 1, m_pos.m_z) ));
  }

  /// Peek at the next voxel in the xz direction w/out moving the iterator.
  inline const T & PeekNextXZ() const
  {
    if ( ((m_pos.m_x + 1 & 7) != 0) &&  ((m_pos.m_z + 1 & 7) != 0) )
      return( m_iterBlock->At(m_blockOffset + 1 + m_offsetXY) );
    else
      return(GetVoxelV(Point<uint2>(m_pos.m_x + 1, m_pos.m_y, m_pos.m_z + 1) ));
  }

  /// Peek at the next voxel in the yz direction w/out moving the iterator.
  inline const T & PeekNextYZ() const
  {
    if ( ((m_pos.m_y + 1 & 7) != 0) &&  ((m_pos.m_z + 1 & 7) != 0) ) 
      return( m_iterBlock->At(m_blockOffset + m_offsetX + m_offsetXY ) );
    else
      return(GetVoxelV(Point<uint2>(m_pos.m_x, m_pos.m_y + 1, m_pos.m_z + 1) ));
  }

  /// Peek at the next voxel in the xyz direction w/out moving the iterator.
  inline const T & PeekNextXYZ() const
  {
    if ( ((m_pos.m_x + 1 & 7) != 0) &&  ((m_pos.m_y + 1 & 7) != 0)  &&  ((m_pos.m_z + 1 & 7) != 0) ) 
      return( m_iterBlock->At(m_blockOffset + 1 + m_offsetX + m_offsetXY) );
    else
      return(GetVoxelV(Point<uint2>(m_pos.m_x + 1, m_pos.m_y + 1, m_pos.m_z + 1) ));
  }
  
// Data Members
protected:

  /// The exact position of sample points. m_pos is the floor voxel position (floor voxel).
  Point<float4> m_floatPos;

  /// The unit distance sample vector.
  Point<float4> m_unitSamplePt;

  /// The ray direction vector.
  Vector<float4> m_directionVector;

  /// The unit sampling distance along the ray.
  float4 m_fSampleDistance;

  /// The gradient vector.
  Vector<float4> m_gradientVector;

  /// The resampled voxel value.
  float4 m_resampledVoxel;

  /// The background voxel. 
  float4 m_backgroundFloat;


}; // class BlockVolRayIterator


#endif // BlockVolRayIterator_h


// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/BlockVolRayIterator.h,v 1.2 2005/04/27 17:36:12 geconomos Exp $
// $Id: BlockVolRayIterator.h,v 1.2 2005/04/27 17:36:12 geconomos Exp $
