// $Id: vxBlockVolumeRayIterator.C,v 1.2 2006/04/19 14:07:46 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// includes
#include "StdAfx.h"
#include "vxBlockVolumeRayIterator.h"


// partial template instatiations
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRayIterator<uint2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRayIterator< int >;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRayIterator< int2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolumeRayIterator< bool >;


// define declarations
#undef Trilin
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Trilinear Interpolation (inlined by precompiler)
 */
#define Trilin(x,y,z,a,b,c,d,e,f,g,h,ab,cd,ef,gh,abcd,efgh,final)      \
                                 \
  ab = x*(b-a) + a;              \
  cd = x*(d-c) + c;              \
  ef = x*(f-e) + e;              \
  gh = x*(h-g) + g;              \
                                 \
  abcd = y*(cd-ab) + ab;         \
  efgh = y*(gh-ef) + ef;         \
  final = (z*(efgh-abcd) + abcd);


/**
 * Constructor
 * @param type of the volume to attach the iterator to
 * @param type of the voxels in the volume
 * @param referance to the volume to be rendered
 */
template <class VoxelType>
vxBlockVolumeRayIterator<VoxelType>::vxBlockVolumeRayIterator(vxBlockVolume<VoxelType> & vol)
:
m_rVol(vol),
m_sampleIncVector(Vector4Df(1,0,0,0.1F)),
m_fRaySegmentEndT(1)
{
  // "previous" position needs to be in another block!!! otherwise everything breaks!!!
  m_iPosX = m_iPosY = m_iPosZ = 16;
  SetPos(Vector4Df(0,0,0,0));
  SetSldDeltaVector();
} // constructor


/**
 * Constructor
 * @param type of the volume to attach the iterator to.
 * @param type of the voxels in the volume.
 * @param starting position of the ray.
 * @param traversal direction of the ray.
 * @param distance between samples along the ray
 * @param referance to the volume to be rendered.
 */
template <class VoxelType>
vxBlockVolumeRayIterator<VoxelType>::vxBlockVolumeRayIterator(vxBlockVolume<VoxelType> & vol, 
                                                              const RaySegment & raySegment, 
                                                              const Vector4Df  & sampleIncrementVec)
:
m_rVol(vol),
m_sampleIncVector(sampleIncrementVec),
m_fRaySegmentEndT(raySegment.GetEndPoint().T())
{
  // "previous" position needs to be in another block!!! otherwise everything breaks!!!
  m_iPosX = raySegment.GetStartPoint().X() + 16;
  m_iPosY = raySegment.GetStartPoint().Y() + 16;
  m_iPosZ = raySegment.GetStartPoint().Z() + 16;
  SetPos(raySegment.GetStartPoint());
  SetSldDeltaVector();
} // constructor


/**
 * Peeks at the next voxel in the x direction w/out moving the iterator.
 * In case that the peek looks into the neighbor block.
 * @param relative offset in X
 * @param relative offset in Y
 * @param relative offset in Z
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeRayIterator<VoxelType>::PeekRelative(int4 idX, int4 idY, int4 idZ) const
{
  return GetVoxel(GetOffset(m_iPosX+idX, m_iPosY+idY, m_iPosZ+idZ));
} // PeekRelative()


/**
 * Update the internal integer position (slow case)
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::UpdateInternalIntegerPositionElse()
{
  m_pL1  = m_rVol.m_BlockL2Vol.GetDataPtr() + GetOffsetL2Vol();
  m_pL0  = m_pL1->GetDataPtr() + GetOffsetL1Blk();
  m_pVox = m_pL0->GetDataPtr() + GetOffsetL0Vox();
} // UpdateInternalIntegerPositionElse()


/**
 * Get sample (slow case)
 */
template <class VoxelType>
const float4 & vxBlockVolumeRayIterator<VoxelType>::GetSampleElse()
{
  const float4 fXDiff(m_pos.X() - m_iPosX);
  const float4 fYDiff(m_pos.Y() - m_iPosY);
  const float4 fZDiff(m_pos.Z() - m_iPosZ);
  const int4 vox000(Peek0px0py0pz());
  const int4 vox100(Peek1px0py0pz());
  const int4 vox010(Peek0px1py0pz());
  const int4 vox110(Peek1px1py0pz());
  const int4 vox001(Peek0px0py1pz());
  const int4 vox101(Peek1px0py1pz());
  const int4 vox011(Peek0px1py1pz());
  const int4 vox111(Peek1px1py1pz());
  float4 ab,cd,ef,gh,abcd,efgh;
  Trilin(
        fXDiff, fYDiff, fZDiff, 
        vox000, vox100, 
        vox010, vox110, 
        vox001, vox101, 
        vox011, vox111, ab,cd,ef,gh,abcd,efgh, m_fResampledVoxel);

  return(m_fResampledVoxel);
} // GetSampleElse()


/**
 * Returns the value at the current position: Slow version. 
 * Interpolates the label based on trilinear distance-weighted majority.
 */
template <class VoxelType>
int4 vxBlockVolumeRayIterator<VoxelType>::GetSampleBestLabelElse()
{
  const int4 vox[8] = { (Peek0px0py0pz()),
                        (Peek1px0py0pz()),
                        (Peek0px1py0pz()),
                        (Peek1px1py0pz()),
                        (Peek0px0py1pz()),
                        (Peek1px0py1pz()),
                        (Peek0px1py1pz()),
                        (Peek1px1py1pz()) };
  
  if (vox[0] == 0 && vox[1] == 0 && vox[2] == 0 && vox[3] == 0 &&
      vox[4] == 0 && vox[5] == 0 && vox[6] == 0 && vox[7] == 0)
    return 0;

  const float4 fWeightX1(m_pos.X() - m_iPosX);
  const float4 fWeightY1(m_pos.Y() - m_iPosY);
  const float4 fWeightZ1(m_pos.Z() - m_iPosZ);
  const float4 fWeightX0(1.0F - fWeightX1);
  const float4 fWeightY0(1.0F - fWeightY1);
  const float4 fWeightZ0(1.0F - fWeightZ1);

  float4 fOpacity[8] = { (fWeightX0 * fWeightY0 * fWeightZ0 * (vox[0] != 0)),
                         (fWeightX1 * fWeightY0 * fWeightZ0 * (vox[1] != 0)),
                         (fWeightX0 * fWeightY1 * fWeightZ0 * (vox[2] != 0)),
                         (fWeightX1 * fWeightY1 * fWeightZ0 * (vox[3] != 0)),
                         (fWeightX0 * fWeightY0 * fWeightZ1 * (vox[4] != 0)),
                         (fWeightX1 * fWeightY0 * fWeightZ1 * (vox[5] != 0)),
                         (fWeightX0 * fWeightY1 * fWeightZ1 * (vox[6] != 0)),
                         (fWeightX1 * fWeightY1 * fWeightZ1 * (vox[7] != 0)) };

  int4 iMaxIndex(0);

  // search for maximum
  for (int4 i(0); i<8; ++i)
  {
    if (vox[i] != 0)
    {
      // collect all the other matching labels opacities into here
      for (int4 j(i+1); j<8; ++j)
      {
        if (vox[j] == vox[i])
        {
          fOpacity[i] += fOpacity[j];
          fOpacity[j] = 0.0F;
        }
      }

      // check for a new maximum
      if (fOpacity[i] > fOpacity[iMaxIndex])
      {
        iMaxIndex = i;
      }
    } // ith voxel non zero?
  } // for all eight neighbouring voxels

  return vox[iMaxIndex];
} // GetSampleBestLabelElse()


/**
 * Returns the sample at the current position (un-safe, slow case)
 * Chooses the nearest voxel based on trilinear distance.
 */
template <class VoxelType>
VoxelType vxBlockVolumeRayIterator<VoxelType>::GetSampleNearestElse()
{
  // compute weights
  const float4 fWeightX1(m_pos.X() - m_iPosX);
  const float4 fWeightY1(m_pos.Y() - m_iPosY);
  const float4 fWeightZ1(m_pos.Z() - m_iPosZ);

  // choose nearest voxel
  if (fWeightX1 < 0.5F) // x0
    if (fWeightY1 < 0.5F) // y0
      if (fWeightZ1 < 0.5F) // z0
        return Peek0px0py0pz();
      else // z1
        return Peek0px0py1pz();
    else // y1
      if (fWeightZ1 < 0.5F) // z0
        return Peek0px1py0pz();
      else // z1
        return Peek0px1py1pz();
  else // x1
    if (fWeightY1 < 0.5F) // y0
      if (fWeightZ1 < 0.5F) // z0
        return Peek1px0py0pz();
      else // z1
        return Peek1px0py1pz();
    else // y1
      if (fWeightZ1 < 0.5F) // z0
        return Peek1px1py0pz();
      else // z1
        return Peek1px1py1pz();
} // GetSampleNearestElse()


/**
 * Returns sample central difference gradient at the current position (un-safe, slow case)
 * @return sample gradient
 */
template <class VoxelType>
const Vector3D<float4> & vxBlockVolumeRayIterator<VoxelType>::GetSampleGradientCentralDifferenceElse()
{
  float4 fXDiff(m_pos.X() - m_iPosX);
  float4 fYDiff(m_pos.Y() - m_iPosY);
  float4 fZDiff(m_pos.Z() - m_iPosZ);

  const float4 i0px0py0pz(Peek0px0py0pz());    
  const float4 i1px0py0pz(Peek1px0py0pz());
  const float4 i1nx0py0pz(Peek1nx0py0pz());
  const float4 i0px1py0pz(Peek0px1py0pz());

  const float4 i0px1ny0pz(Peek0px1ny0pz());
  const float4 i0px0py1pz(Peek0px0py1pz());
  const float4 i0px0py1nz(Peek0px0py1nz());    
  const float4 i2px0py0pz(Peek2px0py0pz());

  const float4 i1px1py0pz(Peek1px1py0pz());
  const float4 i1px1ny0pz(Peek1px1ny0pz());
  const float4 i1px0py1pz(Peek1px0py1pz());    
  const float4 i1px0py1nz(Peek1px0py1nz());

  const float4 i1nx1py0pz(Peek1nx1py0pz());
  const float4 i0px2py0pz(Peek0px2py0pz());
  const float4 i0px1py1pz(Peek0px1py1pz());    
  const float4 i0px1py1nz(Peek0px1py1nz());

  const float4 i2px1py0pz(Peek2px1py0pz());
  const float4 i1px2py0pz(Peek1px2py0pz());
  const float4 i1px1py1pz(Peek1px1py1pz());    
  const float4 i1px1py1nz(Peek1px1py1nz());

  const float4 i1nx0py1pz(Peek1nx0py1pz());
  const float4 i0px1ny1pz(Peek0px1ny1pz());
  const float4 i0px0py2pz(Peek0px0py2pz());        
  const float4 i2px0py1pz(Peek2px0py1pz());

  const float4 i1px1ny1pz(Peek1px1ny1pz());
  const float4 i1px0py2pz(Peek1px0py2pz());    
  const float4 i1nx1py1pz(Peek1nx1py1pz());
  const float4 i0px2py1pz(Peek0px2py1pz());

  const float4 i0px1py2pz(Peek0px1py2pz());    
  const float4 i2px1py1pz(Peek2px1py1pz());
  const float4 i1px2py1pz(Peek1px2py1pz());
  const float4 i1px1py2pz(Peek1px1py2pz());   

  float4 ab,cd,ef,gh,abcd,efgh,final;

  /// these are the 8 gradients in the 8 corners
  Vector3D <float4> fGrad000(i1px0py0pz - i1nx0py0pz, i0px1py0pz - i0px1ny0pz, i0px0py1pz - i0px0py1nz);
  Vector3D <float4> fGrad100(i2px0py0pz - i0px0py0pz, i1px1py0pz - i1px1ny0pz, i1px0py1pz - i1px0py1nz);
  Vector3D <float4> fGrad010(i1px1py0pz - i1nx1py0pz, i0px2py0pz - i0px0py0pz, i0px1py1pz - i0px1py1nz);
  Vector3D <float4> fGrad110(i2px1py0pz - i0px1py0pz, i1px2py0pz - i1px0py0pz, i1px1py1pz - i1px1py1nz);
  Vector3D <float4> fGrad001(i1px0py1pz - i1nx0py1pz, i0px1py1pz - i0px1ny1pz, i0px0py2pz - i0px0py0pz);
  Vector3D <float4> fGrad101(i2px0py1pz - i0px0py1pz, i1px1py1pz - i1px1ny1pz, i1px0py2pz - i1px0py0pz);
  Vector3D <float4> fGrad011(i1px1py1pz - i1nx1py1pz, i0px2py1pz - i0px0py1pz, i0px1py2pz - i0px1py0pz);
  Vector3D <float4> fGrad111(i2px1py1pz - i0px1py1pz, i1px2py1pz - i1px0py1pz, i1px1py2pz - i1px1py0pz);


  Trilin(fXDiff, fYDiff, fZDiff,
         fGrad000.X(), 
         fGrad100.X(), 
         fGrad010.X(), 
         fGrad110.X(),
         fGrad001.X(), 
         fGrad101.X(), 
         fGrad011.X(), 
         fGrad111.X(), 
         ab,cd,ef,gh,abcd,efgh, final);
  m_gradientVector.X() = final;

  Trilin(fXDiff, fYDiff, fZDiff,
         fGrad000.Y(), 
         fGrad100.Y(), 
         fGrad010.Y(), 
         fGrad110.Y(),
         fGrad001.Y(), 
         fGrad101.Y(), 
         fGrad011.Y(), 
         fGrad111.Y(), 
         ab,cd,ef,gh,abcd,efgh, final);
  m_gradientVector.Y() = final;

  Trilin(fXDiff, fYDiff, fZDiff,
         fGrad000.Z(), 
         fGrad100.Z(), 
         fGrad010.Z(), 
         fGrad110.Z(),
         fGrad001.Z(), 
         fGrad101.Z(), 
         fGrad011.Z(), 
         fGrad111.Z(), 
         ab,cd,ef,gh,abcd,efgh, final);
  m_gradientVector.Z() = final;

  return m_gradientVector;
} // GetSampleGradientCentralDifferenceElse()


/**
 * Returns sample intermediate difference gradient at the current position (un-safe, slow case)
 * @return sample gradient
 */
template <class VoxelType>
const Vector3D<float4> & vxBlockVolumeRayIterator<VoxelType>::GetSampleGradientImmediateDifferenceElse()
{
  const float4 fA(Peek0px0py0pz());    
  const float4 fB(Peek1px0py0pz());
  const float4 fC(Peek0px1py0pz());
  const float4 fD(Peek1px1py0pz());
  const float4 fE(Peek0px0py1pz());
  const float4 fF(Peek1px0py1pz());    
  const float4 fG(Peek0px1py1pz());    
  const float4 fH(Peek1px1py1pz());    

  const float4 fXDiff(m_pos.X() - m_iPosX);
  const float4 fYDiff(m_pos.Y() - m_iPosY);
  const float4 fZDiff(m_pos.Z() - m_iPosZ);

  /// FRONT - BACK
  const float4 fAB(fXDiff * (fB - fA) + fA);
  const float4 fCD(fXDiff * (fD - fC) + fC);
  const float4 fEF(fXDiff * (fF - fE) + fE);
  const float4 fGH(fXDiff * (fH - fG) + fG);
  const float4 fABCD(fYDiff * (fCD - fAB) + fAB);
  const float4 fEFGH(fYDiff * (fGH - fEF) + fEF);  
  m_gradientVector.Z() = fEFGH - fABCD;

  /// BOTTOM - TOP
  const float4 fABEF(fZDiff * (fEF - fAB) + fAB);
  const float4 fCDGH(fZDiff * (fGH - fCD) + fCD);
  m_gradientVector.Y() = fCDGH - fABEF;

  /// RIGHT - LEFT
  const float4 fAC(fYDiff * (fC - fA) + fA);
  const float4 fEG(fYDiff * (fG - fE) + fE);
  const float4 fACEG(fZDiff * (fEG - fAC) + fAC);
  const float4 fBD(fYDiff * (fD - fB) + fB);
  const float4 fFH(fYDiff * (fH - fF) + fF);
  const float4 fBDFH(fZDiff * (fFH - fBD) + fBD);
  m_gradientVector.X() = fBDFH - fACEG; 
  
  return m_gradientVector;
} // GetSampleGradientImmediateDifferenceElse()


/**
 * Returns the sample Sobel gradient at the current position (un-safe, slow case)
 * @return sample gradient
 */
template <class VoxelType>
const Vector3D<float4> & vxBlockVolumeRayIterator<VoxelType>::GetSampleGradientSobelElse()
{
  // weights for tri-linear interpolation
  const int4 iBlkPosX((m_iPosX & 0xF));
  const int4 iBlkPosY((m_iPosY & 0xF));
  const int4 iBlkPosZ((m_iPosZ & 0xF));

  float4 fXDiff(m_pos.X() - m_iPosX);
  float4 fYDiff(m_pos.Y() - m_iPosY);
  float4 fZDiff(m_pos.Z() - m_iPosZ);

  const float4 i1nx1ny1nz(Peek1nx1ny1nz());
  const float4 i0px1ny1nz(Peek0px1ny1nz());
  const float4 i1px1ny1nz(Peek1px1ny1nz());
  const float4 i2px1ny1nz(Peek2px1ny1nz());
                                            
  const float4 i1nx0py1nz(Peek1nx0py1nz());
  const float4 i0px0py1nz(Peek0px0py1nz());
  const float4 i1px0py1nz(Peek1px0py1nz());
  const float4 i2px0py1nz(Peek2px0py1nz());
                                            
  const float4 i1nx1py1nz(Peek1nx1py1nz());
  const float4 i0px1py1nz(Peek0px1py1nz());
  const float4 i1px1py1nz(Peek1px1py1nz());
  const float4 i2px1py1nz(Peek2px1py1nz());
                                            
  const float4 i1nx2py1nz(Peek1nx2py1nz());
  const float4 i0px2py1nz(Peek0px2py1nz());
  const float4 i1px2py1nz(Peek1px2py1nz());
  const float4 i2px2py1nz(Peek2px2py1nz());

  const float4 i1nx1ny0pz(Peek1nx1ny0pz());
  const float4 i0px1ny0pz(Peek0px1ny0pz());
  const float4 i1px1ny0pz(Peek1px1ny0pz());
  const float4 i2px1ny0pz(Peek2px1ny0pz());
                                            
  const float4 i1nx0py0pz(Peek1nx0py0pz());
  const float4 i0px0py0pz(Peek0px0py0pz());
  const float4 i1px0py0pz(Peek1px0py0pz());
  const float4 i2px0py0pz(Peek2px0py0pz());
                                            
  const float4 i1nx1py0pz(Peek1nx1py0pz());
  const float4 i0px1py0pz(Peek0px1py0pz());
  const float4 i1px1py0pz(Peek1px1py0pz());
  const float4 i2px1py0pz(Peek2px1py0pz());
                                            
  const float4 i1nx2py0pz(Peek1nx2py0pz());
  const float4 i0px2py0pz(Peek0px2py0pz());
  const float4 i1px2py0pz(Peek1px2py0pz());
  const float4 i2px2py0pz(Peek2px2py0pz());

  const float4 i1nx1ny1pz(Peek1nx1ny1pz());
  const float4 i0px1ny1pz(Peek0px1ny1pz());
  const float4 i1px1ny1pz(Peek1px1ny1pz());
  const float4 i2px1ny1pz(Peek2px1ny1pz());
                                            
  const float4 i1nx0py1pz(Peek1nx0py1pz());
  const float4 i0px0py1pz(Peek0px0py1pz());
  const float4 i1px0py1pz(Peek1px0py1pz());
  const float4 i2px0py1pz(Peek2px0py1pz());
                                                  
  const float4 i1nx1py1pz(Peek1nx1py1pz());      
  const float4 i0px1py1pz(Peek0px1py1pz());
  const float4 i1px1py1pz(Peek1px1py1pz());
  const float4 i2px1py1pz(Peek2px1py1pz());
                                            
  const float4 i1nx2py1pz(Peek1nx2py1pz());
  const float4 i0px2py1pz(Peek0px2py1pz());
  const float4 i1px2py1pz(Peek1px2py1pz());
  const float4 i2px2py1pz(Peek2px2py1pz());  

  const float4 i1nx1ny2pz(Peek1nx1ny2pz());
  const float4 i0px1ny2pz(Peek0px1ny2pz());
  const float4 i1px1ny2pz(Peek1px1ny2pz());
  const float4 i2px1ny2pz(Peek2px1ny2pz());
                                            
  const float4 i1nx0py2pz(Peek1nx0py2pz());
  const float4 i0px0py2pz(Peek0px0py2pz());
  const float4 i1px0py2pz(Peek1px0py2pz());
  const float4 i2px0py2pz(Peek2px0py2pz());
                                            
  const float4 i1nx1py2pz(Peek1nx1py2pz());
  const float4 i0px1py2pz(Peek0px1py2pz());
  const float4 i1px1py2pz(Peek1px1py2pz());
  const float4 i2px1py2pz(Peek2px1py2pz());
                                            
  const float4 i1nx2py2pz(Peek1nx2py2pz());
  const float4 i0px2py2pz(Peek0px2py2pz());
  const float4 i1px2py2pz(Peek1px2py2pz());
  const float4 i2px2py2pz(Peek2px2py2pz());

  float4 ab,cd,ef,gh,abcd,efgh,final;
  Vector3D <float4> fGrad000(sobelb(i1nx1ny1nz, i1px1ny1nz,  
                                     i1nx0py1nz, i1px0py1nz, 
                                     i1nx1py1nz, i1px1py1nz) +
                             sobelc(i1nx1ny0pz, i1px1ny0pz, 
                                     i1nx0py0pz, i1px0py0pz, 
                                     i1nx1py0pz, i1px1py0pz) +
                             sobelb(i1nx1ny1pz, i1px1ny1pz, 
                                     i1nx0py1pz, i1px0py1pz, 
                                     i1nx1py1pz, i1px1py1pz),

                             sobelb(i1nx1ny1nz, i1nx1py1nz, 
                                     i0px1ny1nz, i0px1py1nz, 
                                     i1px1ny1nz, i1px1py1nz) +
                             sobelc(i1nx1ny0pz, i1nx1py0pz, 
                                     i0px1ny0pz, i0px1py0pz, 
                                     i1px1ny0pz, i1px1py0pz) +
                             sobelb(i1nx1ny1pz, i1nx1py1pz, 
                                     i0px1ny1pz, i0px1py1pz,
                                     i1px1ny1pz, i1px1py1pz),

                             sobelb(i1nx1ny1nz, i1nx1ny1pz, 
                                     i0px1ny1nz, i0px1ny1pz, 
                                     i1px1ny1nz, i1px1ny1pz) +
                             sobelc(i1nx0py1nz, i1nx0py1pz, 
                                     i0px0py1nz, i0px0py1pz, 
                                     i1px0py1nz, i1px0py1pz) +
                             sobelb(i1nx1py1nz, i1nx1py1pz, 
                                     i0px1py1nz, i0px1py1pz, 
                                     i1px1py1nz, i1px1py1pz));

  Vector3D <float4> fGrad100(sobelb(i0px1ny1nz, i2px1ny1nz, 
                                     i0px0py1nz, i2px0py1nz, 
                                     i0px1py1nz, i2px1py1nz) +
                             sobelc(i0px1ny0pz, i2px1ny0pz, 
                                     i0px0py0pz, i2px0py0pz, 
                                     i0px1py0pz, i2px1py0pz) +
                             sobelb(i0px1ny1pz, i2px1ny1pz, 
                                     i0px0py1pz, i2px0py1pz, 
                                     i0px1py1pz, i2px1py1pz),

                             sobelb(i0px1ny1nz, i0px1py1nz, 
                                     i1px1ny1nz, i1px1py1nz, 
                                     i2px1ny1nz, i2px1py1nz) +
                             sobelc(i0px1ny0pz, i0px1py0pz, 
                                     i1px1ny0pz, i1px1py0pz, 
                                     i2px1ny0pz, i2px1py0pz) +
                             sobelb(i0px1ny1pz, i0px1py1pz, 
                                     i1px1ny1pz, i1px1py1pz, 
                                     i2px1ny1pz, i2px1py1pz),

                             sobelb(i0px1ny1nz, i0px1ny1pz, 
                                     i1px1ny1nz, i1px1ny1pz, 
                                     i2px1ny1nz, i2px1ny1pz) +
                             sobelc(i0px0py1nz, i0px0py1pz, 
                                     i1px0py1nz, i1px0py1pz, 
                                     i2px0py1nz, i2px0py1pz) +
                             sobelb(i0px1py1nz, i0px1py1pz, 
                                     i1px1py1nz, i1px1py1pz, 
                                     i2px1py1nz, i2px1py1pz));

  Vector3D <float4> fGrad010(sobelb(i1nx0py1nz, i1px0py1nz, 
                                     i1nx1py1nz, i1px1py1nz, 
                                     i1nx2py1nz, i1px2py1nz) +
                             sobelc(i1nx0py0pz, i1px0py0pz, 
                                     i1nx1py0pz, i1px1py0pz, 
                                     i1nx2py0pz, i1px2py0pz) +
                             sobelb(i1nx0py1pz, i1px0py1pz, 
                                     i1nx1py1pz, i1px1py1pz, 
                                     i1nx2py1pz, i1px2py1pz),

                             sobelb(i1nx0py1nz, i1nx2py1nz, 
                                     i0px0py1nz, i0px2py1nz, 
                                     i1px0py1nz, i1px2py1nz) +
                             sobelc(i1nx0py0pz, i1nx2py0pz, 
                                     i0px0py0pz, i0px2py0pz, 
                                     i1px0py0pz, i1px2py0pz) +
                             sobelb(i1nx0py1pz, i1nx2py1pz, 
                                     i0px0py1pz, i0px2py1pz, 
                                     i1px0py1pz, i1px2py1pz),

                             sobelb(i1nx0py1nz, i1nx0py1pz, 
                                     i0px0py1nz, i0px0py1pz, 
                                     i1px0py1nz, i1px0py1pz) +
                             sobelc(i1nx1py1nz, i1nx1py1pz, 
                                     i0px1py1nz, i0px1py1pz, 
                                     i1px1py1nz, i1px1py1pz) +
                             sobelb(i1nx2py1nz, i1nx2py1pz, 
                                     i0px2py1nz, i0px2py1pz, 
                                     i1px2py1nz, i1px2py1pz));

  Vector3D <float4> fGrad110(sobelb(i0px0py1nz, i2px0py1nz, 
                                     i0px1py1nz, i2px1py1nz, 
                                     i0px2py1nz, i2px2py1nz) +
                             sobelc(i0px0py0pz, i2px0py0pz, 
                                     i0px1py0pz, i2px1py0pz, 
                                     i0px2py0pz, i2px2py0pz) +
                             sobelb(i0px0py1pz, i2px0py1pz, 
                                     i0px1py1pz, i2px1py1pz, 
                                     i0px2py1pz, i2px2py1pz),

                             sobelb(i0px0py1nz, i0px2py1nz, 
                                     i1px0py1nz, i1px2py1nz, 
                                     i2px0py1nz, i2px2py1nz) +
                             sobelc(i0px0py0pz, i0px2py0pz, 
                                     i1px0py0pz, i1px2py0pz, 
                                     i2px0py0pz, i2px2py0pz) +
                             sobelb(i0px0py1pz, i0px2py1pz, 
                                     i1px0py1pz, i1px2py1pz, 
                                     i2px0py1pz, i2px2py1pz),

                             sobelb(i0px0py1nz, i0px0py1pz, 
                                     i1px0py1nz, i1px0py1pz, 
                                     i2px0py1nz, i2px0py1pz) +
                             sobelc(i0px1py1nz, i0px1py1pz, 
                                     i1px1py1nz, i1px1py1pz, 
                                     i2px1py1nz, i2px1py1pz) +
                             sobelb(i0px2py1nz, i0px2py1pz, 
                                     i1px2py1nz, i1px2py1pz, 
                                     i2px2py1nz, i2px2py1pz));

  Vector3D <float4> fGrad001(sobelb(i1nx1ny0pz, i1px1ny0pz, 
                                     i1nx0py0pz, i1px0py0pz, 
                                     i1nx1py0pz, i1px1py0pz) +
                             sobelc(i1nx1ny1pz, i1px1ny1pz, 
                                     i1nx0py1pz, i1px0py1pz, 
                                     i1nx1py1pz, i1px1py1pz) +
                             sobelb(i1nx1ny2pz, i1px1ny2pz, 
                                     i1nx0py2pz, i1px0py2pz, 
                                     i1nx1py2pz, i1px1py2pz),

                             sobelb(i1nx1ny0pz, i1nx1py0pz, 
                                     i0px1ny0pz, i0px1py0pz, 
                                     i1px1ny0pz, i1px1py0pz) +
                             sobelc(i1nx1ny1pz, i1nx1py1pz, 
                                     i0px1ny1pz, i0px1py1pz, 
                                     i1px1ny1pz, i1px1py1pz) +
                             sobelb(i1nx1ny2pz, i1nx1py2pz, 
                                     i0px1ny2pz, i0px1py2pz, 
                                     i1px1ny2pz, i1px1py2pz),

                             sobelb(i1nx1ny0pz, i1nx1ny2pz, 
                                     i0px1ny0pz, i0px1ny2pz, 
                                     i1px1ny0pz, i1px1ny2pz) +
                             sobelc(i1nx0py0pz, i1nx0py2pz, 
                                     i0px0py0pz, i0px0py2pz, 
                                     i1px0py0pz, i1px0py2pz) +
                             sobelb(i1nx1py0pz, i1nx1py2pz, 
                                     i0px1py0pz, i0px1py2pz, 
                                     i1px1py0pz, i1px1py2pz));

  Vector3D <float4> fGrad101(sobelb(i0px1ny0pz, i2px1ny0pz, 
                                     i0px0py0pz, i2px0py0pz, 
                                     i0px1py0pz, i2px1py0pz) +
                             sobelc(i0px1ny1pz, i2px1ny1pz, 
                                     i0px0py1pz, i2px0py1pz, 
                                     i0px1py1pz, i2px1py1pz) +
                             sobelb(i0px1ny2pz, i2px1ny2pz, 
                                     i0px0py2pz, i2px0py2pz, 
                                     i0px1py2pz, i2px1py2pz),

                             sobelb(i0px1ny0pz, i0px1py0pz, 
                                     i1px1ny0pz, i1px1py0pz, 
                                     i2px1ny0pz, i2px1py0pz) +
                             sobelc(i0px1ny1pz, i0px1py1pz, 
                                     i1px1ny1pz, i1px1py1pz, 
                                     i2px1ny1pz, i2px1py1pz) +
                             sobelb(i0px1ny2pz, i0px1py2pz, 
                                     i1px1ny2pz, i1px1py2pz, 
                                     i2px1ny2pz, i2px1py2pz),

                             sobelb(i0px1ny0pz, i0px1ny2pz, 
                                     i1px1ny0pz, i1px1ny2pz, 
                                     i2px1ny0pz, i2px1ny2pz) +
                             sobelc(i0px0py0pz, i0px0py2pz, 
                                     i1px0py0pz, i1px0py2pz, 
                                     i2px0py0pz, i2px0py2pz) +
                             sobelb(i0px1py0pz, i0px1py2pz, 
                                     i1px1py0pz, i1px1py2pz, 
                                     i2px1py0pz, i2px1py2pz));

  Vector3D <float4> fGrad011(sobelb(i1nx0py0pz, i1px0py0pz, 
                                     i1nx1py0pz, i1px1py0pz, 
                                     i1nx2py0pz, i1px2py0pz) +
                             sobelc(i1nx0py1pz, i1px0py1pz, 
                                     i1nx1py1pz, i1px1py1pz, 
                                     i1nx2py1pz, i1px2py1pz) +
                             sobelb(i1nx0py2pz, i1px0py2pz, 
                                     i1nx1py2pz, i1px1py2pz, 
                                     i1nx2py2pz, i1px2py2pz),

                             sobelb(i1nx0py0pz, i1nx2py0pz, 
                                     i0px0py0pz, i0px2py0pz, 
                                     i1px0py0pz, i1px2py0pz) +
                             sobelc(i1nx0py1pz, i1nx2py1pz, 
                                     i0px0py1pz, i0px2py1pz, 
                                     i1px0py1pz, i1px2py1pz) +
                             sobelb(i1nx0py2pz, i1nx2py2pz, 
                                     i0px0py2pz, i0px2py2pz, 
                                     i1px0py2pz, i1px2py2pz),

                             sobelb(i1nx0py0pz, i1nx0py2pz, 
                                     i0px0py0pz, i0px0py2pz, 
                                     i1px0py0pz, i1px0py2pz) +
                             sobelc(i1nx1py0pz, i1nx1py2pz, 
                                     i0px1py0pz, i0px1py2pz, 
                                     i1px1py0pz, i1px1py2pz) +
                             sobelb(i1nx2py0pz, i1nx2py2pz, 
                                     i0px2py0pz, i0px2py2pz, 
                                     i1px2py0pz, i1px2py2pz));

  Vector3D <float4> fGrad111(sobelb(i0px0py0pz, i2px0py0pz, 
                                     i0px1py0pz, i2px1py0pz, 
                                     i0px2py0pz, i2px2py0pz) +
                             sobelc(i0px0py1pz, i2px0py1pz, 
                                     i0px1py1pz, i2px1py1pz, 
                                     i0px2py1pz, i2px2py1pz) +
                             sobelb(i0px0py2pz, i2px0py2pz, 
                                     i0px1py2pz, i2px1py2pz, 
                                     i0px2py2pz, i2px2py2pz),

                             sobelb(i0px0py0pz, i0px2py0pz, 
                                     i1px0py0pz, i1px2py0pz, 
                                     i2px0py0pz, i2px2py0pz) +
                             sobelc(i0px0py1pz, i0px2py1pz, 
                                     i1px0py1pz, i1px2py1pz, 
                                     i2px0py1pz, i2px2py1pz) +
                             sobelb(i0px0py2pz, i0px2py2pz, 
                                     i1px0py2pz, i1px2py2pz, 
                                     i2px0py2pz, i2px2py2pz),

                             sobelb(i0px0py0pz, i0px0py2pz, 
                                     i1px0py0pz, i1px0py2pz, 
                                     i2px0py0pz, i2px0py2pz) +
                             sobelc(i0px1py0pz, i0px1py2pz, 
                                     i1px1py0pz, i1px1py2pz, 
                                     i2px1py0pz, i2px1py2pz) +
                             sobelb(i0px2py0pz, i0px2py2pz, 
                                     i1px2py0pz, i1px2py2pz, 
                                     i2px2py0pz, i2px2py2pz));

  Trilin(fXDiff, fYDiff, fZDiff,
         fGrad000.X(), 
         fGrad100.X(), 
         fGrad010.X(), 
         fGrad110.X(),
         fGrad001.X(), 
         fGrad101.X(), 
         fGrad011.X(), 
         fGrad111.X(), 
         ab,cd,ef,gh,abcd,efgh, final);
  m_gradientVector.X() = final;

  Trilin(fXDiff, fYDiff, fZDiff,
         fGrad000.Y(), 
         fGrad100.Y(), 
         fGrad010.Y(), 
         fGrad110.Y(),
         fGrad001.Y(), 
         fGrad101.Y(), 
         fGrad011.Y(), 
         fGrad111.Y(), 
         ab,cd,ef,gh,abcd,efgh, final);
  m_gradientVector.Y() = final;

  Trilin(fXDiff, fYDiff, fZDiff,
         fGrad000.Z(), 
         fGrad100.Z(), 
         fGrad010.Z(), 
         fGrad110.Z(),
         fGrad001.Z(), 
         fGrad101.Z(), 
         fGrad011.Z(), 
         fGrad111.Z(), 
         ab,cd,ef,gh,abcd,efgh, final);
  m_gradientVector.Z() = final;
  
  return m_gradientVector;
} // GetSampleGradientSobelElse()


#undef FILE_REVISION


// $Log: vxBlockVolumeRayIterator.C,v $
// Revision 1.2  2006/04/19 14:07:46  geconomos
// initialized m_iPosY and m_iPosZ in constructors
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.35  2004/01/14 18:48:49  wenli
// vxBlockVolumeRayIterator< bool > is added
//
// Revision 1.34  2003/11/21 16:17:57  michael
// fixed issue of uninitialized data!!!
//
// Revision 1.33  2003/05/03 16:18:20  michael
// completed code review
//
// Revision 1.32  2003/05/02 14:36:56  dongqing
// code walk through
//
// Revision 1.31  2003/01/22 21:34:59  ingmar
// this iterator is now only using a RaySegment and a Vector4Df increment to determine how to traverse the ray
//
// Revision 1.30  2002/11/26 18:08:21  ingmar
// now using Vector4d<float4> to keep the current position as well as offset
//
// Revision 1.29  2002/11/20 23:30:54  frank
// Added some new ray sampling options for label volumes
//
// Revision 1.28  2002/08/28 15:51:43  ingmar
// removed not needed instantiations (oops)
//
// Revision 1.27  2002/08/28 15:08:22  ingmar
// removed not needed instantiations
//
// Revision 1.26  2002/08/21 22:05:33  ingmar
// now differentiating between visibleVoxelCropBox and enclosing multipleOf16BlockCropBox
//
// Revision 1.25  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.24  2002/06/18 15:46:52  ingmar
// removed normalization for gradient computation to allow anisotropic correction in the client code
//
// Revision 1.23  2002/06/14 22:38:31  ingmar
// improved the weights of the sobel filter
//
// Revision 1.22  2002/06/14 22:20:50  ingmar
// finished Sobel Gradient Filter implementation
// added several Peek functions needed for 64 neighborhood of sobel filter
//
// Revision 1.21  2002/06/13 22:10:05  ingmar
// now using rayIter->GetDistanceToEndOfBlock()
// also ray sampling patters code fixes
//
// Revision 1.20  2002/06/10 20:59:19  soeren
// changed constructor
//
// Revision 1.19  2002/06/07 23:34:44  soeren
// beutified central difference gradient
//
// Revision 1.18  2002/06/06 20:49:16  soeren
// added inital version of Sobel gradient
//
// Revision 1.17  2002/06/06 20:45:03  soeren
// Just beautyfied
//
// Revision 1.16  2002/06/06 20:35:04  soeren
// implemented GetSampleGradientImmediateDifference
//
// Revision 1.15  2002/06/06 15:29:18  soeren
// implemented GetSampleGradientImmediateDifference
//
// Revision 1.14  2002/06/05 14:53:48  soeren
// and another 10 percent faster, changing VoxelType to hardcoded int4
//
// Revision 1.13  2002/06/05 14:35:03  soeren
// renamed peeks
//
// Revision 1.12  2002/06/04 15:51:40  soeren
// improved GetSampleGradientCentralDifference
//
// Revision 1.11  2002/06/04 13:32:57  soeren
// optimized GetSample
//
// Revision 1.10  2002/06/04 13:08:36  soeren
// removed old gradient computation
//
// Revision 1.9  2002/06/03 14:04:48  soeren
// removed obsolute Next and prev else
//
// Revision 1.8  2002/06/02 15:30:52  soeren
// Moved Constructor to C file
//
// Revision 1.7  2002/06/02 15:21:55  soeren
// Speed up, through moving else of UpdateInternalIntegerPosition to C file:-)
//
// Revision 1.6  2002/06/02 14:38:21  soeren
// removed non raw functions and renamed raw functions to non raw,
// since there is nothing unsave now
//
// Revision 1.5  2002/06/02 14:03:02  soeren
// fixed some peek bugs
//
// Revision 1.4  2002/05/07 22:40:11  ingmar
// changed to coding standard compliant enums
//
// Revision 1.3  2002/04/04 22:09:23  ingmar
// fixed walking into negative positions
//
// Revision 1.2  2002/04/04 19:08:56  ingmar
// fixed some bugs in Peek and code beautifiy
//
// Revision 1.1  2002/04/04 14:42:52  ingmar
// initial version
//
// Revision 1.3  2002/04/02 20:05:41  ingmar
// removed dead, commented code
//
// Revision 1.2  2002/03/20 16:14:34  ingmar
// Instead of fast access pointer to the beginning of the data of a block, we are using now the direct
// access function of the block.
//
// Revision 1.1  2002/03/19 17:01:25  ingmar
// included all voliter functionality to allow compiler to make better optimaizations
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeRayIterator.C,v 1.2 2006/04/19 14:07:46 geconomos Exp $
// $Id: vxBlockVolumeRayIterator.C,v 1.2 2006/04/19 14:07:46 geconomos Exp $
