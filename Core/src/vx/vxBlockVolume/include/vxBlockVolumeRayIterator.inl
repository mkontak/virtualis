// $Id: vxBlockVolumeRayIterator.inl,v 1.5.10.4 2010/08/03 21:01:28 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// defines
#define FILE_REVISION "$Revision: 1.5.10.4 $"


/**
 * Template function for trilinear iterpolation.
 *
 *    Trilinear interpolation: x,y,z are the three weights for
 *    the x, y, and z directions, and a,b,c,d,e,f,g, and h are
 *    the 8 vertices (values) to interpolate between.
 *    (x, y, and z are the distances from point a along the x,
 *    y, and z axis respectively.)
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
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::Trilinear(int4 x, int4 y, int4 z, 
                                                    const int4 a, const int4 b, const int4 c, const int4 d, 
                                                    const int4 e, const int4 f, const int4 g, const int4 h, int4 & iFinal) const
{
  // interpolate in x               
  int4 ab(((x*(b-a))>>16) + a);
  int4 cd(((x*(d-c))>>16) + c);
  int4 ef(((x*(f-e))>>16) + e);
  int4 gh(((x*(h-g))>>16) + g);

  // interpolate in y
  int4 abcd(((y*(cd-ab))>>16) + ab);
  int4 efgh(((y*(gh-ef))>>16) + ef);

  // interpolate in z and return result
  iFinal = (((z*(efgh-abcd))>>16) + abcd);
} // Trilinear


// defines
#undef Trilin
#define Trilin(x,y,z,a,b,c,d,e,f,g,h,ab,cd,ef,gh,abcd,efgh,final)      \
                                 \
  ab = x*(b-a) + a;              \
  cd = x*(d-c) + c;              \
  abcd = y*(cd-ab) + ab;         \
                                 \
  ef = x*(f-e) + e;              \
  gh = x*(h-g) + g;              \
  efgh = y*(gh-ef) + ef;         \
                                 \
  final = (z*(efgh-abcd) + abcd);


/**
 * Places the iterator at the given position
 * @param new iterator position
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::SetPos(const Vector4d<float4> & pos)
{
  m_pos = pos;
  UpdateInternalIntegerPosition();
} // SetPos()


/**
 * Places the iterator at the given position
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::UpdateInternalIntegerPosition()
{

#ifdef _WIN64
  
  double fTmpX(m_pos.X() );
  double fTmpY(m_pos.Y() );
  double fTmpZ(m_pos.Z() );


  int iTmpX( static_cast< int >( fTmpX ) );
  int iTmpY( static_cast< int >( fTmpY ) );
  int iTmpZ( static_cast< int >( fTmpZ ));

#else

  double fTmpX(m_pos.X() - 0.49999999999 );
  double fTmpY(m_pos.Y() - 0.49999999999 );
  double fTmpZ(m_pos.Z() - 0.49999999999 );


  int iTmpX;
  int iTmpY;
  int iTmpZ;

  __asm 
  {
    FLD   QWORD PTR [fTmpX]                                    
    FISTP DWORD PTR [iTmpX]                                         
    FLD   QWORD PTR [fTmpY]                                    
    FISTP DWORD PTR [iTmpY]                                          
    FLD   QWORD PTR [fTmpZ]                                          
    FISTP DWORD PTR [iTmpZ] 
  }

#endif

  // compute the difference from where we are to where we want to go
  const int4 iTmpDeltaX(iTmpX - m_iPosX);
  const int4 iTmpDeltaY(iTmpY - m_iPosY);
  const int4 iTmpDeltaZ(iTmpZ - m_iPosZ);

  /// compute the the new position within the block [0,15]
  const int4 iBlkPosX((m_iPosX & 0xF) + iTmpDeltaX);
  const int4 iBlkPosY((m_iPosY & 0xF) + iTmpDeltaY);
  const int4 iBlkPosZ((m_iPosZ & 0xF) + iTmpDeltaZ);

  // update to new position
  m_iPosX = iTmpX;
  m_iPosY = iTmpY;
  m_iPosZ = iTmpZ;

  // if NOT (any pos has any bit set outside the first 4 bits) ==> inside block
  if (!((iBlkPosX | iBlkPosY | iBlkPosZ) & 0x1FFFFFFF0))
  { 
    m_pVox += iTmpDeltaX;
    m_pVox += iTmpDeltaY*16;
    m_pVox += iTmpDeltaZ*256;
  }
  else
  { // ad some other block
    UpdateInternalIntegerPositionElse();
  }
} // UpdateInternalIntegerPosition


/**
 * Set sample distance 
 * @param new distance between samples
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::SetRaySegment(const RaySegment & raySegment)
{
  m_pos = raySegment.GetStartPoint();
  m_fRaySegmentEndT = raySegment.GetEndPoint().T();
  UpdateInternalIntegerPosition();
} // SetRaySegment()


/**
 * Set sample distance
 * @param new distance between samples
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::SetSampleIncrementVector(const Vector4Df & sampleIncVec)
{
  m_sampleIncVector = sampleIncVec;
  SetSldDeltaVector();
} // SetSampleIncrementVector()


/**
 * Set spaceleap vector according to current direction vector
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::SetSldDeltaVector()
{
  const float4 radius(1.0F);

  m_sldDeltaVector = m_sampleIncVector;
  m_sldDeltaVector *= radius/Max(Abs(m_sampleIncVector.X()), Abs(m_sampleIncVector.Y()), Abs(m_sampleIncVector.Z()));
  m_sldDeltaVectorTwo  = m_sldDeltaVector + m_sldDeltaVector;
  m_sldDeltaVectorMinusHalf = m_sldDeltaVector * (-0.5);
} // SetSldDeltaVector()


/**
 * Returns offset that corresponds to the given position
 * @return offset
 */
template <class VoxelType>
const int4 vxBlockVolumeRayIterator<VoxelType>::GetOffsetL2Vol() const
{                                 
  return ((m_iPosX & eL2VolPosMaskX) >> 6 )+      /// 11111 +
         ((m_iPosY & eL2VolPosMaskY) >> 1 ) +     /// 11111 00000 +
         ((m_iPosZ & eL2VolPosMaskZ) << 4 );      /// 11111 00000 00000 
} // GetOffsetL2Vol()																																																																																																																																				


/**
 * Get distance to end of block
 * @return distance at which the ray intersects with the current block
 */
template <class VoxelType>
float4 vxBlockVolumeRayIterator<VoxelType>::GetDistanceToEndOfBlock() const
{
  const int4 iBlockWidth(16);

  // find candidate planes
  float4 fPlaneX(m_iPosX - (m_iPosX % iBlockWidth));
  float4 fPlaneY(m_iPosY - (m_iPosY % iBlockWidth));
  float4 fPlaneZ(m_iPosZ - (m_iPosZ % iBlockWidth));

  if (m_sldDeltaVector.X() >= 0) fPlaneX += iBlockWidth;
  if (m_sldDeltaVector.Y() >= 0) fPlaneY += iBlockWidth;
  if (m_sldDeltaVector.Z() >= 0) fPlaneZ += iBlockWidth;

  // find distances from candidate plane
  const float4 fdX(fabs(m_pos.X() - fPlaneX));
  const float4 fdY(fabs(m_pos.Y() - fPlaneY));
  const float4 fdZ(fabs(m_pos.Z() - fPlaneZ));

  float4 fSldX(fabs(m_sldDeltaVector.X()));
  float4 fSldY(fabs(m_sldDeltaVector.Y()));
  float4 fSldZ(fabs(m_sldDeltaVector.Z()));

  if (fSldX == 0.0) fSldX = 0.000001f;
  if (fSldY == 0.0) fSldY = 0.000001f;
  if (fSldZ == 0.0) fSldZ = 0.000001f;

  // find # of streps to reach candidate plane
  const float4 fStepsX(fdX / fSldX);
  const float4 fStepsY(fdY / fSldY);
  const float4 fStepsZ(fdZ / fSldZ);

  // find minimum step number
  if (fStepsY < fStepsX)
    if (fStepsZ < fStepsY)
      return fStepsZ;
    else
      return fStepsY;
  else
    if (fStepsZ < fStepsX)
      return fStepsZ;
    else
      return fStepsX;
} // GetDistanceToEndOfBlock()																																																																																																																																				


/**
 * Get distance to end of small block
 * @return distance at which the ray intersects with the current block
 */
template <class VoxelType>
float4 vxBlockVolumeRayIterator<VoxelType>::GetDistanceToEndOfSmallBlock() const
{
  const int4 iBlockWidth(1);

  // find candidate planes
  float4 fPlaneX(m_iPosX);
  float4 fPlaneY(m_iPosY);
  float4 fPlaneZ(m_iPosZ);

  if (m_sldDeltaVector.X() >= 0) fPlaneX += iBlockWidth;
  if (m_sldDeltaVector.Y() >= 0) fPlaneY += iBlockWidth;
  if (m_sldDeltaVector.Z() >= 0) fPlaneZ += iBlockWidth;

  // find distances from candidate plane
  const float4 fdX(fabs(m_pos.X() - fPlaneX));
  const float4 fdY(fabs(m_pos.Y() - fPlaneY));
  const float4 fdZ(fabs(m_pos.Z() - fPlaneZ));

  float4 fSldX(fabs(m_sldDeltaVector.X()));
  float4 fSldY(fabs(m_sldDeltaVector.Y()));
  float4 fSldZ(fabs(m_sldDeltaVector.Z()));

  if (fSldX == 0.0) fSldX = 0.000001f;
  if (fSldY == 0.0) fSldY = 0.000001f;
  if (fSldZ == 0.0) fSldZ = 0.000001f;

  // find # of streps to reach candidate plane
  const float4 fStepsX(fdX / fSldX);
  const float4 fStepsY(fdY / fSldY);
  const float4 fStepsZ(fdZ / fSldZ);

  //// find minimum step number
  if (fStepsY < fStepsX)
    if (fStepsZ < fStepsY)
      return(fStepsZ);
    else
      return(fStepsY);
  else
    if (fStepsZ < fStepsX)
      return(fStepsZ);
    else
      return(fStepsX);
} // GetDistanceToEndOfSmallBlock()																																																																																																																																				


/**
 * Returns offset that corresponds to the given position
 * @return offset
 */
template <class VoxelType>
const int4 vxBlockVolumeRayIterator<VoxelType>::GetOffsetL1Blk() const
{      
  return ((m_iPosX & eL1BlkPosMask) >> eL0VoxPosBits  )+ 
         ((m_iPosY & eL1BlkPosMask) >> 2 )+  // 2 =(eL0VoxPosBits - eL1BlkPosBits)
         ((m_iPosZ & eL1BlkPosMask) );
} // GetOffsetL1Blk()


/**
 * Returns the offset that corresponds to the given position.
 * @return the offset.
 */
template <class VoxelType>
const int4 vxBlockVolumeRayIterator<VoxelType>::GetOffsetL0Vox() const
{
  return ((m_iPosX & eL0VoxPosMask)) +                      													
         ((m_iPosY & eL0VoxPosMask) <<    eL0VoxPosBits )+  
         ((m_iPosZ & eL0VoxPosMask) << 8 ); // 8 = (2*eL0VoxPosBits)
} // GetOffsetL0Vox()


/**
 * Get voxel (un-safe versions)
 * @return voxel
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeRayIterator<VoxelType>::GetVoxel() const
{
  return *m_pVox;
} // GetVoxel()


/**
 * Returns the voxel value at the current position.
 * May be garbage or segfault if the position is not valid.
 * @param offset
 * @return const reference to the voxel
 */
template <class VoxelType>
const VoxelType & vxBlockVolumeRayIterator<VoxelType>::GetVoxel(const uint8 uOffset) const
{
  return m_rVol.m_BlockL2Vol.At(((uOffset & eLinearOffsetMaskForL2VolPosZ) >> 18) |
                                ((uOffset & eLinearOffsetMaskForL2VolPosY) >> 12) |
                                ((uOffset & eLinearOffsetMaskForL2VolPosX) >>  6)).
                             At(((uOffset & eLinearOffsetMaskForL1BlkPosZ) >> 22) |
                                ((uOffset & eLinearOffsetMaskForL1BlkPosY) >> 13) |
                                ((uOffset & eLinearOffsetMaskForL1BlkPosX) >>  4)).
                             At(((uOffset & eLinearOffsetMaskForL0VoxPosZ) >> 14) |
                                ((uOffset & eLinearOffsetMaskForL0VoxPosY) >>  7) |
                                ((uOffset & eLinearOffsetMaskForL0VoxPosX)));
} // GetVoxel()


/**
 * Returns the value at the current position (un-safe, fast case)
 * @return sample
 */
template <class VoxelType>
const int4 vxBlockVolumeRayIterator<VoxelType>::GetSampleInt()
{
//#ifdef _WIN64
  float4 fSample(GetSample());
  return static_cast< int4 >( fSample );
//#else
//  
//  float4 fSample(GetSample());
//  int4 iSample;
//  __asm {
//    FLD   DWORD PTR [fSample]
//    FISTP DWORD PTR [iSample] 
//  }
//  return iSample;
//#endif
} // GetSampleInt()


/**
 * Returns value at current position (un-safe, fast case)
 * @return sample
 */
template <class VoxelType>
const float4 & vxBlockVolumeRayIterator<VoxelType>::GetSample()
{
  // Compute weights for tri-linear interpolation.
  const int4 iBlkPosX((m_iPosX & 0xF));
  const int4 iBlkPosY((m_iPosY & 0xF));
  const int4 iBlkPosZ((m_iPosZ & 0xF));

  if ((iBlkPosX < 15) && (iBlkPosY < 15) && (iBlkPosZ < 15))//// Test if all needed voxel are within one block, if yes faster access is possible.
  {
    const float4 fXDiff(m_pos.X() - m_iPosX);
    const float4 fYDiff(m_pos.Y() - m_iPosY);
    const float4 fZDiff(m_pos.Z() - m_iPosZ);
                                                                                                     
    const int4 vox000(*(m_pVox));           // a
    const int4 vox100(*(m_pVox +   1));     // b
    const int4 vox010(*(m_pVox +  16));     // c
    const int4 vox110(*(m_pVox +  17));     // d    17 = 1 + 16
    const int4 vox001(*(m_pVox + 256));     // e
    const int4 vox101(*(m_pVox + 257));     // f   257 = 1      + 256
    const int4 vox011(*(m_pVox + 272));     // g   272 =     16 + 256
    const int4 vox111(*(m_pVox + 273));     // h   273 = 1 + 16 + 256

    float4 ab,cd,ef,gh,abcd,efgh;
    Trilin(
          fXDiff, fYDiff, fZDiff, 
          vox000, vox100, 
          vox010, vox110, 
          vox001, vox101, 
          vox011, vox111, ab,cd,ef,gh,abcd,efgh, m_fResampledVoxel);

    return m_fResampledVoxel;
  }
  else
  {
    //LogDbg( "SampleElse", "", "" );
    return GetSampleElse();   // Compute the resampled value by tri-linear interpolation.
  }
} // GetSample()


/**
 * Returns value at current position (un-safe, fast case)
 * @return sample
 */
template <class VoxelType>
const float4 & vxBlockVolumeRayIterator<VoxelType>::GetSampleSIMD()
{
  
#ifdef __ICL
  // Compute weights for tri-linear interpolation.

  const int4 iBlkPosX((m_iPosX & 0xF));
  const int4 iBlkPosY((m_iPosY & 0xF));
  const int4 iBlkPosZ((m_iPosZ & 0xF));

  if ((iBlkPosX < 15) && (iBlkPosY < 15) && (iBlkPosZ < 15))//// Test if all needed voxel are within one block, if yes faster access is possible.
  {
    const float4 const fXDiff(m_pos.X() - m_iPosX);
    __declspec(align(16)) float x[4] = { fXDiff, fXDiff,  fXDiff,  fXDiff };

    const float4 const fYDiff(m_pos.Y() - m_iPosY);
    __declspec(align(16)) float y[4] = { fYDiff, fYDiff, -fYDiff, -fYDiff };

                                                                                                     
  __declspec(align(16)) const float aecg[4] = { *(m_pVox   ),*(m_pVox          + 256),*(m_pVox     + 16),*(m_pVox     + 16 + 256) };                             
  __declspec(align(16)) const float bfdh[4] = { *(m_pVox + 1),*(m_pVox + 1      + 256),*(m_pVox + 1 + 16),*(m_pVox + 1 + 16 + 256) };
  

  __declspec(align(16)) float res[4]; 

  //  xmm1          xmm2          xmm3          xmm4          xmm5                                   xmm6          xmm7
  
  _asm movaps xmm1, x    ;            //  x  x  x  x
  _asm movaps xmm2, y    ;            //                y  y -y -y 
  _asm movaps xmm4, aecg ;            //                                            a  e  c  g
  _asm movaps xmm5, bfdh ;            //                                                          b  f  d  h

  // interpolate in x               
  //int4 ab = x*(b-a) + a;
  //int4 ef = x*(f-e) + e;
  //int4 cd = x*(d-c) + c;
  //int4 gh = x*(h-g) + g;
  
  _asm subps xmm5, xmm4 ;             // x  x  x  x     y  y -y -y    z  z  z  z    a  e  c  g    b-a      f-e      d-c      h-g
 
  _asm mulps xmm5, xmm1 ;             // x  x  x  x     y  y -y -y    z  z  z  z    a  e  c  g    x(b-a)   x(f-e)   x(d-c)   x(h-g)
  
  _asm addps xmm5, xmm4 ;             // x  x  x  x     y  y -y -y    z  z  z  z    a  e  c  g    x(b-a)+a x(f-e)+e x(d-c)+c x(h-g)+g
  // x  x  x  x    -y -y  y  y    z  z  z  z    a  e  c  g    ab       ef       cd       gh
  

  // interpolate in y
  //int4 abcd = y*(cd-ab) + ab;       
  //int4 efgh = y*(gh-ef) + ef;       
  
  // shuffle  aecg -> cgae
  _asm movaps xmm6, xmm5;             // x  x  x  x     y  y -y -y    z  z  z  z    a  e  c  g    ab       ef       cd       gh          ab       ef       cd       gh
  _asm shufps xmm5, xmm6, 0x4e;       // x  x  x  x     y  y -y -y    z  z  z  z    a  e  c  g    cd       gh       ab       ef          ab       ef       cd       gh      
  
  _asm subps xmm5, xmm6 ;             // x  x  x  x     y  y -y -y    z  z  z  z    a  e  c  g    cd-ab    gh-ef    ab-cd    ef-gh       ab       ef       cd       gh
  
  _asm mulps xmm5, xmm2 ;             // x  x  x  x     y  y -y -y    z  z  z  z    a  e  c  g    y(cd-ab) y(gh-ef) y(cd-ab) y(gh-ef)    ab       ef       cd       gh
  
  _asm addps xmm5, xmm6 ;             // x  x  x  x     y  y -y -y    z  z  z  z    a  e  c  g    y(cd-ab) y(gh-ef) y(cd-ab) y(gh-ef)    ab       ef       cd       gh
                                      // x  x  x  x     y  y -y -y    z  1  1  z    a  e  c  g    abcd     efgh     abcd     efgh        ab       ef       cd       gh
  
  // interpolate in z and return result
  //iFinal = z*(efgh-abcd) + abcd;
  
  _asm movaps res, xmm5 ;             //                                                          abcd     efgh     abcd     efgh
  //                                  //                                                           0         1       2        3       

  const float4 fZDiff(m_pos.Z() - m_iPosZ);
  m_fResampledVoxel = fZDiff*(res[1] - res[0]) + res[0];

  _asm nop;
  _asm nop;
    
  return(m_fResampledVoxel);
  }
  else
#endif // __ICL
  {
    return GetSampleElse(); // Compute the resampled value by tri-linear interpolation.
  }
} // GetSampleSIMD()


/**
 * Returns the value at the current position: Interpolates the label based on trilinear 
 * distance-weighted majority.
 * @return sample
 */
template <class VoxelType>
int4 vxBlockVolumeRayIterator<VoxelType>::GetSampleBestLabel()
{
  // Compute weights for tri-linear interpolation.
  const int4 iBlkPosX((m_iPosX & 0xF));
  const int4 iBlkPosY((m_iPosY & 0xF));
  const int4 iBlkPosZ((m_iPosZ & 0xF));

  if ((iBlkPosX < 15) && (iBlkPosY < 15) && (iBlkPosZ < 15))//// Test if all needed voxel are within one block, if yes faster access is possible.
  {
    const int4 vox[8] = { *(m_pVox),           // a vox000
                          *(m_pVox +   1),     // b vox100
                          *(m_pVox +  16),     // c vox010  
                          *(m_pVox +  17),     // d vox110   17 = 1 + 16
                          *(m_pVox + 256),     // e vox001
                          *(m_pVox + 257),     // f vox101  257 = 1      + 256
                          *(m_pVox + 272),     // g vox011  272 =   + 16 + 256
                          *(m_pVox + 273) };   // h vox111  273 = 1 + 16 + 256

    if (vox[0] == 0 && vox[1] == 0 && vox[2] == 0 && vox[3] == 0 &&
        vox[0] == 0 && vox[1] == 0 && vox[2] == 0 && vox[3] == 0)
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
      }
    }
    return vox[iMaxIndex];
  }
  else
  {
    return GetSampleBestLabelElse();
  }

  // never gets here
  return 0;
} // GetSampleBestLabel()


/**
 * Returns the value at the current position: Interpolates the label based 
 * on trilinear distance-weighted majority.
 * @return sample
 */
template <class VoxelType>
VoxelType vxBlockVolumeRayIterator<VoxelType>::GetSampleNearest()
{
  const int4 iBlkPosX( m_iPosX & 0xF );
  const int4 iBlkPosY( m_iPosY & 0xF );
  const int4 iBlkPosZ( m_iPosZ & 0xF );

  if ((iBlkPosX < 15) && (iBlkPosY < 15) && (iBlkPosZ < 15))//// Test if all needed voxel are within one block, if yes faster access is possible.
  {
    // compute weights
    const float4 fWeightX1( m_pos.X() - m_iPosX );
    const float4 fWeightY1( m_pos.Y() - m_iPosY );
    const float4 fWeightZ1( m_pos.Z() - m_iPosZ );

    // compute offset within 8-neighborhood
    int4 iOffset(0);
    iOffset += 1   * (fWeightX1 >= 0.5F);
    iOffset += 16  * (fWeightY1 >= 0.5F);
    iOffset += 256 * (fWeightZ1 >= 0.5F);

    return m_pVox[iOffset];
  }

  return GetSampleNearestElse();

} // GetSampleNearest()


/**
 * Advance to next sample position along the ray
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::operator++()
{
  SetPos(m_pos + m_sampleIncVector);
} // operator++()


/**
 * Spaceleap ones along the ray
 * @param distance
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::SpaceLeap(float4 fSpaceLeapDist)
{
  Vector4d<float4> incVec(m_sldDeltaVector * fSpaceLeapDist);
  m_pos += incVec;
  UpdateInternalIntegerPosition();
} // SpaceLeap()


/**
 * Spaceleap ones along the ray
 * @param distance
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::SpaceLeapInt(int4 iSpaceLeapDist)
{
  Vector4d<float4> incPos(m_sldDeltaVector * iSpaceLeapDist);
  m_pos += incPos;
  UpdateInternalIntegerPosition();
} // SpaceLeapInt()


/**
 * space leap in world space distance (millimeters) along the ray
 *
 * @param fSpaceLeapDist the distance to leap in millimeters (world space)
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::SpaceLeapMM(float4 fSpaceLeapDist)
{
  const Triple<float4> & units( m_rVol.GetHeader().GetVoxelUnits() );
  Vector4d<float4> worldIncVector( m_sldDeltaVector.m_x * units.m_x, m_sldDeltaVector.m_y * units.m_y, m_sldDeltaVector.m_z * units.m_z, m_sldDeltaVector.m_t );
  const float4 fScaleForOneMM( 1 / worldIncVector.GetLengthXYZ() );
  Vector4d<float4> incVec(m_sldDeltaVector * fSpaceLeapDist * fScaleForOneMM);
  m_pos += incVec;
  UpdateInternalIntegerPosition();
} // SpaceLeapMM()


/**
 * Spaceleap ones along the ray.
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::SpaceLeapMinusHalf()
{
  m_pos += m_sldDeltaVectorMinusHalf;
  UpdateInternalIntegerPosition();
} // SpaceLeapMinusHalf()


/**
 * Spaceleap ones along the ray.
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::SpaceLeapOne()
{
  m_pos += m_sldDeltaVector;
  UpdateInternalIntegerPosition();
} // SpaceLeapOne()


/**
 * Spaceleap ones along the ray.
 */
template <class VoxelType>
void vxBlockVolumeRayIterator<VoxelType>::SpaceLeapTwo()
{
  m_pos += m_sldDeltaVectorTwo;
  UpdateInternalIntegerPosition();
} // SpaceLeapTwo()


/**
 * Get floating point position
 * @return sample position
 */
template <class VoxelType>
const Vector4d<float4> & vxBlockVolumeRayIterator<VoxelType>::GetPosFloat() const
{
  return m_pos;
} // GetPosFloat()


/**
 * Returns current x,y,z sample position
 * @return sample position
 */
template <class VoxelType>
const Vector3D<int4>  vxBlockVolumeRayIterator<VoxelType>::GetPosInt() const
{
  return Vector3D<int4>(m_iPosX, m_iPosY, m_iPosZ);
} // GetPosInt()


/**
 * Peeks at the next voxel in the xyz direction w/out moving the iterator.
 * In case that the peek looks into the neighbor block.
 */
template <class VoxelType> 
const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx1ny1nz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0) && ((m_iPosY & eL0VoxPosMax) > 0) && ((m_iPosZ & eL0VoxPosMax) > 0))
  {
    //return *(m_pVox - 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ);
    return *(m_pVox - 273);
  }
  else
  {
    return PeekRelative(-1,-1,-1);
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px1ny1nz() const
{
  if (((m_iPosY & eL0VoxPosMax) > 0              ) && ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 272));
  }
  else
  {
    return(PeekRelative(0,-1,-1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px1ny1nz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) && ((m_iPosY & eL0VoxPosMax) > 0              ) && ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 271));
  }
  else
  {
    return(PeekRelative(1,-1,-1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px1ny1nz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) && ((m_iPosY & eL0VoxPosMax) > 0              ) && ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 270));
  }
  else
  {
    return(PeekRelative(2,-1,-1));
  }
}

template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx0py1nz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) && ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 257));
  }
  else
  {
    return(PeekRelative(-1, 0,-1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px0py1nz() const
{
  if (((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX                        - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 256));
  }
  else
  {
    return(PeekRelative(0, 0,-1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px0py1nz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) &&                                                  ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX                        - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 255));
  }
  else
  {
    return(PeekRelative(1, 0,-1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px0py1nz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) &&                                                  ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX                        - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox -254));
  }
  else
  {
    return(PeekRelative(2, 0,-1));
  }
}

template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx1py1nz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )) && ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 241));
  }
  else
  {
    return(PeekRelative(-1, 1,-1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px1py1nz() const
{
  if (((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )) && ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 240));
  }
  else
  {
    return(PeekRelative(0, 1,-1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px1py1nz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )) && ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 239));
  }
  else
  {
    return(PeekRelative(1, 1,-1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px1py1nz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )) && ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 238));
  }
  else
  {
    return(PeekRelative(2, 1,-1));
  }
}

template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx2py1nz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)) && ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 225));
  }
  else
  {
    return(PeekRelative(-1, 2,-1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px2py1nz() const
{
  if (((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)) && ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 224));
  }
  else
  {
    return(PeekRelative(0, 2,-1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px2py1nz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)) && ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 223));
  }
  else
  {
    return(PeekRelative(1, 2,-1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px2py1nz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)) && ((m_iPosZ & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox - 222));
  }
  else
  {
    return(PeekRelative(2, 2,-1));
  }
}


template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx1ny0pz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) && ((m_iPosY & eL0VoxPosMax) > 0              ))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY                      ));
    return(*(m_pVox - 17));
  }
  else
  {
    return(PeekRelative(-1,-1, 0));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px1ny0pz() const
{
  if (((m_iPosY & eL0VoxPosMax) > 0              ))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY                      ));
    return(*(m_pVox - 16));
  }
  else
  {
    return(PeekRelative(0,-1, 0));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px1ny0pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) && ((m_iPosY & eL0VoxPosMax) > 0              ))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY                      ));
    return(*(m_pVox - 15));
  }
  else
  {
    return(PeekRelative(1,-1, 0));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px1ny0pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) && ((m_iPosY & eL0VoxPosMax) > 0              ))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY                      ));
    return(*(m_pVox - 14));
  }
  else
  {
    return(PeekRelative(2,-1, 0));
  }
}

template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx0py0pz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX                                             ));
    return(*(m_pVox - 1));
  }
  else
  {
    return(PeekRelative(-1, 0, 0));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px0py0pz() const
{
  return(GetVoxel());
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px0py0pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX                                             ));
    return(*(m_pVox + 1));
  }
  else
  {
    return(PeekRelative(1, 0, 0));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px0py0pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX                                             ));
    return(*(m_pVox + 2));
  }
  else
  {
    return(PeekRelative(2, 0, 0));
  }
}

template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx1py0pz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY                      ));
    return(*(m_pVox + 15));
  }
  else
  {
    return(PeekRelative(-1, 1, 0));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px1py0pz() const
{
  if (((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY                      ));
    return(*(m_pVox + 16));
  }
  else
  {
    return(PeekRelative(0, 1, 0));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px1py0pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY                      ));
    return(*(m_pVox + 17));
  }
  else
  {
    return(PeekRelative(1, 1, 0));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px1py0pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY                      ));
    return(*(m_pVox + 18));
  }
  else
  {
    return(PeekRelative(2, 1, 0));
  }
}

template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx2py0pz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY                      ));
    return(*(m_pVox + 31));
  }
  else
  {
    return(PeekRelative(-1, 2, 0));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px2py0pz() const
{
  if (((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY                      ));
    return(*(m_pVox + 32));
  }
  else
  {
    return(PeekRelative(0, 2, 0));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px2py0pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY                      ));
    return(*(m_pVox + 33));
  }
  else
  {
    return(PeekRelative(1, 2, 0));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px2py0pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY                      ));
    return(*(m_pVox + 34));
  }
  else
  {
    return(PeekRelative(2, 2, 0));
  }
}


template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx1ny1pz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) && ((m_iPosY & eL0VoxPosMax) > 0              ) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 239));
  }
  else
  {
    return(PeekRelative(-1,-1, 1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px1ny1pz() const
{
  if (((m_iPosY & eL0VoxPosMax) > 0              ) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 240));
  }
  else
  {
    return(PeekRelative(0,-1, 1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px1ny1pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) && ((m_iPosY & eL0VoxPosMax) > 0              ) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 241));
  }
  else
  {
    return(PeekRelative(1,-1, 1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px1ny1pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) && ((m_iPosY & eL0VoxPosMax) > 0              ) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 242));
  }
  else
  {
    return(PeekRelative(2,-1, 1));
  }
}

template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx0py1pz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) &&                                                  ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX                        + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 255));
  }
  else
  {
    return(PeekRelative(-1, 0, 1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px0py1pz() const
{
  if (((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX                        + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 256));
  }
  else
  {
    return(PeekRelative(0, 0, 1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px0py1pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) &&                                                  ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX                        + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 257));
  }
  else
  {
    return(PeekRelative(1, 0, 1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px0py1pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) &&                                                  ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX                        + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 258));
  }
  else
  {
    return(PeekRelative(2, 0, 1));
  }
}

template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx1py1pz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 271));
  }
  else
  {
    return(PeekRelative(-1, 1, 1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px1py1pz() const
{
  if (((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 272));
  }
  else
  {
    return(PeekRelative(0, 1, 1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px1py1pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 273));
  }
  else
  {
    return(PeekRelative(1, 1, 1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px1py1pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 274));
  }
  else
  {
    return(PeekRelative(2, 1, 1));
  }
}

template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx2py1pz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 287));
  }
  else
  {
    return(PeekRelative(-1, 2, 1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px2py1pz() const
{
  if (((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 288));
  }
  else
  {
    return(PeekRelative(0, 2, 1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px2py1pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 289));
  }
  else
  {
    return(PeekRelative(1, 2, 1));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px2py1pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax ))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 290));
  }
  else
  {
    return(PeekRelative(2, 2, 1));
  }
}


template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx1ny2pz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) && ((m_iPosY & eL0VoxPosMax) > 0              ) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 495));
  }
  else
  {
    return(PeekRelative(-1,-1, 2));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px1ny2pz() const
{
  if (((m_iPosY & eL0VoxPosMax) > 0              ) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 496));
  }
  else
  {
    return(PeekRelative(0,-1, 2));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px1ny2pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) && ((m_iPosY & eL0VoxPosMax) > 0              ) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 497));
  }
  else
  {
    return(PeekRelative(1,-1, 2));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px1ny2pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) && ((m_iPosY & eL0VoxPosMax) > 0              ) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 498));
  }
  else
  {
    return(PeekRelative(2,-1, 2));
  }
}

template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx0py2pz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) &&                                                  ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX                        + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 511));
  }
  else
  {
    return(PeekRelative(-1, 0, 2));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px0py2pz() const
{
  if (((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX                        + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 512));
  }
  else
  {
    return(PeekRelative(0, 0, 2));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px0py2pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) &&                                                  ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX                        + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 513));
  }
  else
  {
    return(PeekRelative(1, 0, 2));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px0py2pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) &&                                                  ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX                        + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 514));
  }
  else
  {
    return(PeekRelative(2, 0, 2));
  }
}

template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx1py2pz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 527));
  }
  else
  {
    return(PeekRelative(-1, 1, 2));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px1py2pz() const
{
  if (((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 528));
  }
  else
  {
    return(PeekRelative(0, 1, 2));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px1py2pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 529));
  }
  else
  {
    return(PeekRelative(1, 1, 2));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px1py2pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax )) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 530));
  }
  else
  {
    return(PeekRelative(2, 1, 2));
  }
}

template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1nx2py2pz() const
{
  if (((m_iPosX & eL0VoxPosMax) > 0            ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox - 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 543));
  }
  else
  {
    return(PeekRelative(-1, 2, 2));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek0px2py2pz() const
{
  if (((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 0*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 544));
  }
  else
  {
    return(PeekRelative(0, 2, 2));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek1px2py2pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax ) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 545));
  }
  else
  {
    return(PeekRelative(1, 2, 2));
  }
}
template <class VoxelType> const VoxelType & vxBlockVolumeRayIterator<VoxelType>::Peek2px2py2pz() const
{
  if (((m_iPosX & eL0VoxPosMax) < eL0VoxPosMax-1) && ((m_iPosY & eL0VoxPosMax) < (eL0VoxPosMax-1)) && ((m_iPosZ & eL0VoxPosMax) < eL0VoxPosMax-1))
  {
    //return(*(m_pVox + 2*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));
    return(*(m_pVox + 546));
  }
  else
  {
    return(PeekRelative(2, 2, 2));
  }
}

/**
 * Returns sample central difference gradient at the current position (un-safe, fast case)
 * @return sample gradient
 */
template <class VoxelType>
const Vector3D<float4> & vxBlockVolumeRayIterator<VoxelType>::GetSampleGradientCentralDifference()
{
  // weights for tri-linear interpolation
  const int4 iBlkPosX((m_iPosX & 0xF));
  const int4 iBlkPosY((m_iPosY & 0xF));
  const int4 iBlkPosZ((m_iPosZ & 0xF));

  if ((iBlkPosX > 1) && (iBlkPosX < 14) &&
      (iBlkPosY > 1) && (iBlkPosY < 14) &&
      (iBlkPosZ > 1) && (iBlkPosZ < 14))//// Test if all needed voxel are within one block, if yes faster access is possible.
  {
    float4 fXDiff(m_pos.X() - m_iPosX);
    float4 fYDiff(m_pos.Y() - m_iPosY);
    float4 fZDiff(m_pos.Z() - m_iPosZ);

    const int4 i0px0py0pz(*(m_pVox));
    const int4 i1px0py0pz(*(m_pVox + 1));
    const int4 i1nx0py0pz(*(m_pVox - 1));    
    const int4 i0px1py0pz(*(m_pVox + 16));

    const int4 i0px1ny0pz(*(m_pVox - 16));    
    const int4 i0px0py1pz(*(m_pVox + 256));
    const int4 i0px0py1nz(*(m_pVox - 256));    
    const int4 i2px0py0pz(*(m_pVox + 2));

    const int4 i1px1py0pz(*(m_pVox +  17));    // 17 = 1 + 16
    const int4 i1px1ny0pz(*(m_pVox -  15));    // -15 = 1 - 16
    const int4 i1px0py1pz(*(m_pVox + 257));    // 257 = 1 + 256    
    const int4 i1px0py1nz(*(m_pVox - 255));    // -255 = 1 - 256

    const int4 i1nx1py0pz(*(m_pVox +  15));    // 15 = - 1 + 16
    const int4 i0px2py0pz(*(m_pVox +  32));    // 32 = 2*16
    const int4 i0px1py1pz(*(m_pVox + 272));    // 272 = 16 + 256   
    const int4 i0px1py1nz(*(m_pVox - 240));    // -240 = 16 - 256

    const int4 i2px1py0pz(*(m_pVox +  18));    // 18 = 2*1 + 16
    const int4 i1px2py0pz(*(m_pVox +  33));    // 33 = 1 + 2*16
    const int4 i1px1py1pz(*(m_pVox + 273));    // 273 = 1 + 16 + 256    
    const int4 i1px1py1nz(*(m_pVox - 239));    // -239 = 1 + 16 - 256

    const int4 i1nx0py1pz(*(m_pVox + 255));    // 255 = - 1 + 256
    const int4 i0px1ny1pz(*(m_pVox + 240));    // 240 = - 16 + 256 
    const int4 i0px0py2pz(*(m_pVox + 512));    // 512 = 2 * 256    
    const int4 i2px0py1pz(*(m_pVox + 258));    // 258 = 2*1 + 256

    const int4 i1px1ny1pz(*(m_pVox + 241));    // 241 = 1 - 16 + 256
    const int4 i1px0py2pz(*(m_pVox + 513));    // 513 = 1 + 2*256   
    const int4 i1nx1py1pz(*(m_pVox + 271));    // 271 = - 1 + 16 + 256
    const int4 i0px2py1pz(*(m_pVox + 288));    // 288 = 2 * 16 + 256  

    const int4 i0px1py2pz(*(m_pVox + 528));    // 528 = 16 + 2*256     
    const int4 i2px1py1pz(*(m_pVox + 274));    // 274 = 2*1 + 16 + 256
    const int4 i1px2py1pz(*(m_pVox + 289));    // 289 = 1 + 2* 16 + 256
    const int4 i1px1py2pz(*(m_pVox + 529));    // 529 = 1 + 16 + 2* 256     

    float4 ab,cd,ef,gh,abcd,efgh,final;

    /// these are the 8 gradients in the 8 corners
    Vector3D <int4> fGrad000(i1px0py0pz - i1nx0py0pz, i0px1py0pz - i0px1ny0pz, i0px0py1pz - i0px0py1nz);
    Vector3D <int4> fGrad100(i2px0py0pz - i0px0py0pz, i1px1py0pz - i1px1ny0pz, i1px0py1pz - i1px0py1nz);
    Vector3D <int4> fGrad010(i1px1py0pz - i1nx1py0pz, i0px2py0pz - i0px0py0pz, i0px1py1pz - i0px1py1nz);
    Vector3D <int4> fGrad110(i2px1py0pz - i0px1py0pz, i1px2py0pz - i1px0py0pz, i1px1py1pz - i1px1py1nz);
    Vector3D <int4> fGrad001(i1px0py1pz - i1nx0py1pz, i0px1py1pz - i0px1ny1pz, i0px0py2pz - i0px0py0pz);
    Vector3D <int4> fGrad101(i2px0py1pz - i0px0py1pz, i1px1py1pz - i1px1ny1pz, i1px0py2pz - i1px0py0pz);
    Vector3D <int4> fGrad011(i1px1py1pz - i1nx1py1pz, i0px2py1pz - i0px0py1pz, i0px1py2pz - i0px1py0pz);
    Vector3D <int4> fGrad111(i2px1py1pz - i0px1py1pz, i1px2py1pz - i1px0py1pz, i1px1py2pz - i1px1py0pz);

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
  }
  else
  {
    return GetSampleGradientCentralDifferenceElse();
  }
} // GetSampleGradientCentralDifference()


/**
 * Returns sample intermediate difference gradient at the current position (un-safe, fast case)
 * @return sample gradient
 */
template <class VoxelType>
const Vector3D<float4> & vxBlockVolumeRayIterator<VoxelType>::GetSampleGradientImmediateDifference()
{
  // weights for tri-linear interpolation
  const int4 iBlkPosX((m_iPosX & 0xF));
  const int4 iBlkPosY((m_iPosY & 0xF));
  const int4 iBlkPosZ((m_iPosZ & 0xF));

  if ((iBlkPosX < 15) &&
      (iBlkPosY < 15) &&
      (iBlkPosZ < 15))//// Test if all needed voxel are within one block, if yes faster access is possible.
  {
    const float4 fA(*(m_pVox ));
    const float4 fB(*(m_pVox +   1 ));
    const float4 fC(*(m_pVox +  16 ));
    const float4 fD(*(m_pVox +  17));    // 17 = 1 + 16
    const float4 fE(*(m_pVox + 256));
    const float4 fF(*(m_pVox + 257));    // 257 = 1 + 256    
    const float4 fG(*(m_pVox + 272));    // 272 = 16     + 256        
    const float4 fH(*(m_pVox + 273));    // 273 = 1 + 16 + 256   

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
  }
  else
  {
    return GetSampleGradientImmediateDifferenceElse();
  }
} // GetSampleGradientImmediateDifference()


/**
 * Helper function Sobel B slow
 * @param value a
 * @param value b
 * @param value c
 * @param value d
 * @param value e
 * @param value f
 * @return result
 */
inline int4 sobelb_slow(int4 a, int4 b, int4 c, int4 d, int4 e, int4 f) 
{
  int x(b - a + f - e);
  x += x;
  int y (d - c);
  y += y + y;

  return x + y;
} // sobelb_slow()


/**
 * Helper function Sobel C slow
 * @param value a
 * @param value b
 * @param value c
 * @param value d
 * @param value e
 * @param value f
 * @return result
 */
inline int4 sobelc_slow(int4 a, int4 b, int4 c, int4 d, int4 e, int4 f)
{
  int x (d - c);
  x+= x;
  int y (b - a + f - e + x);
  y += y + y;

  return y;
} // sobelc_slow()


/**
 * Returns sample Sobel gradient at the current position (un-safe, fast case)
 * @return sample gradient
 */
template <class VoxelType>
const Vector3D<float4> & vxBlockVolumeRayIterator<VoxelType>::GetSampleGradientSobel()
{
  // weights for tri-linear interpolation

  const int4 iBlkPosX((m_iPosX & 0xF));
  const int4 iBlkPosY((m_iPosY & 0xF));
  const int4 iBlkPosZ((m_iPosZ & 0xF));

  if ((iBlkPosX > 1) && (iBlkPosX < 14) &&
      (iBlkPosY > 1) && (iBlkPosY < 14) &&
      (iBlkPosZ > 1) && (iBlkPosZ < 14))//// Test if all needed voxel are within one block, if yes faster access is possible.
  {

    float4 fXDiff(m_pos.X() - m_iPosX);
    float4 fYDiff(m_pos.Y() - m_iPosY);
    float4 fZDiff(m_pos.Z() - m_iPosZ);

    const int4 i1nx1ny1nz(*(m_pVox - 273));    // -1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    
    const int4 i0px1ny1nz(*(m_pVox - 272));    // 0*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    
    const int4 i1px1ny1nz(*(m_pVox - 271));    // 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    
    const int4 i2px1ny1nz(*(m_pVox - 270));    // 2*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    

    const int4 i1nx0py1nz(*(m_pVox - 257));    // - 1*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    
    const int4 i0px0py1nz(*(m_pVox - 256));    //  0*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    
    const int4 i1px0py1nz(*(m_pVox - 255));    // 1*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    
    const int4 i2px0py1nz(*(m_pVox - 254));    // 2*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    

    const int4 i1nx1py1nz(*(m_pVox - 241));    //- 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    
    const int4 i0px1py1nz(*(m_pVox - 240));    // 0*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    
    const int4 i1px1py1nz(*(m_pVox - 239));    // 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    
    const int4 i2px1py1nz(*(m_pVox - 238));    // 2*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    

    const int4 i1nx2py1nz(*(m_pVox - 225));    // - 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    
    const int4 i0px2py1nz(*(m_pVox - 224));    // 0*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    
    const int4 i1px2py1nz(*(m_pVox - 223));    // 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    
    const int4 i2px2py1nz(*(m_pVox - 222));    // 2*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY - 1*eL0VoxOffsetDeltaZ));    


    const int4 i1nx1ny0pz(*(m_pVox -  17));    //- 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    
    const int4 i0px1ny0pz(*(m_pVox -  16));    // 0*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    
    const int4 i1px1ny0pz(*(m_pVox -  15));    // 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    
    const int4 i2px1ny0pz(*(m_pVox -  14));    // 2*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    

    const int4 i1nx0py0pz(*(m_pVox -   1));    // - 1*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    
    const int4 i0px0py0pz(*(m_pVox      ));    // 0*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    
    const int4 i1px0py0pz(*(m_pVox +   1));    // 1*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    
    const int4 i2px0py0pz(*(m_pVox +   2));    // 2*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    

    const int4 i1nx1py0pz(*(m_pVox +  15));    //- 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    
    const int4 i0px1py0pz(*(m_pVox +  16));    // 0*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    
    const int4 i1px1py0pz(*(m_pVox +  17));    // 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    
    const int4 i2px1py0pz(*(m_pVox +  18));    // 2*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    

    const int4 i1nx2py0pz(*(m_pVox +  31));    //- 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    
    const int4 i0px2py0pz(*(m_pVox +  32));    // 0*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    
    const int4 i1px2py0pz(*(m_pVox +  33));    // 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    
    const int4 i2px2py0pz(*(m_pVox +  34));    // 2*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 0*eL0VoxOffsetDeltaZ));    


    const int4 i1nx1ny1pz(*(m_pVox + 239));    // - 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    
    const int4 i0px1ny1pz(*(m_pVox + 240));    // 0*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    
    const int4 i1px1ny1pz(*(m_pVox + 241));    // 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    
    const int4 i2px1ny1pz(*(m_pVox + 242));    // 2*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    

    const int4 i1nx0py1pz(*(m_pVox + 255));    // - 1*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    
    const int4 i0px0py1pz(*(m_pVox + 256));    // 0*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    
    const int4 i1px0py1pz(*(m_pVox + 257));    // 1*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    
    const int4 i2px0py1pz(*(m_pVox + 258));    // 2*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    

    const int4 i1nx1py1pz(*(m_pVox + 271));    // - 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    
    const int4 i0px1py1pz(*(m_pVox + 272));    // 0*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    
    const int4 i1px1py1pz(*(m_pVox + 273));    // 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    
    const int4 i2px1py1pz(*(m_pVox + 274));    // 2*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    

    const int4 i1nx2py1pz(*(m_pVox + 287));    // - 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    
    const int4 i0px2py1pz(*(m_pVox + 288));    // 0*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    
    const int4 i1px2py1pz(*(m_pVox + 289));    // 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    
    const int4 i2px2py1pz(*(m_pVox + 290));    // 2*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 1*eL0VoxOffsetDeltaZ));    


    const int4 i1nx1ny2pz(*(m_pVox + 495));    // - 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    
    const int4 i0px1ny2pz(*(m_pVox + 496));    // 0*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    
    const int4 i1px1ny2pz(*(m_pVox + 497));    // 1*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    
    const int4 i2px1ny2pz(*(m_pVox + 498));    // 2*eL0VoxOffsetDeltaX - 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    

    const int4 i1nx0py2pz(*(m_pVox + 511));    // - 1*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    
    const int4 i0px0py2pz(*(m_pVox + 512));    // 0*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    
    const int4 i1px0py2pz(*(m_pVox + 513));    // 1*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    
    const int4 i2px0py2pz(*(m_pVox + 514));    // 2*eL0VoxOffsetDeltaX + 0*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    

    const int4 i1nx1py2pz(*(m_pVox + 527));    // - 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    
    const int4 i0px1py2pz(*(m_pVox + 528));    // 0*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    
    const int4 i1px1py2pz(*(m_pVox + 529));    // 1*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    
    const int4 i2px1py2pz(*(m_pVox + 530));    // 2*eL0VoxOffsetDeltaX + 1*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    

    const int4 i1nx2py2pz(*(m_pVox + 543));    // - 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    
    const int4 i0px2py2pz(*(m_pVox + 544));    // 0*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    
    const int4 i1px2py2pz(*(m_pVox + 545));    // 1*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    
    const int4 i2px2py2pz(*(m_pVox + 546));    // 2*eL0VoxOffsetDeltaX + 2*eL0VoxOffsetDeltaY + 2*eL0VoxOffsetDeltaZ));    

    float4 ab, cd, ef, gh, abcd, efgh, final;

    // these are the 8 gradients in the 8 corners
#define sobelb(a,b,c,d,e,f) ((b - a) * 2) + (d - c) * 3 +  (f - e) * 2
#define sobelc(a,b,c,d,e,f) ((b - a) * 3) + (d - c) * 6 +  (f - e) * 3    

#define sobelb_new(a,b,c,d,e,f) ((b - a + f - e) * 2 + (d - c) * 3)
#define sobelc_new(a,b,c,d,e,f) ((b - a + f - e + (d - c) * 2) * 3)

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
  }
  else
  {
    return GetSampleGradientSobelElse();
  }
} // GetSampleGradientSobel()


/**
 * Returns true if the iterator is inside the volume.
 */
template <class VoxelType>
bool vxBlockVolumeRayIterator<VoxelType>::IsInsideVolume() const
{
  const Triple<int4> & dim( m_rVol.GetHeader().GetVolDim() );
  return
  (
    m_pos.m_x >= 0 && m_pos.m_x <= (float4)(dim.m_x-1) &&
    m_pos.m_y >= 0 && m_pos.m_y <= (float4)(dim.m_y-1) &&
    m_pos.m_z >= 0 && m_pos.m_z <= (float4)(dim.m_z-1) );
} // IsInsideVolume()


/**
 * Returns the offset that corresponds to the current voxel position
 * @return offset
 */
template <class VoxelType>
const uint8 vxBlockVolumeRayIterator<VoxelType>::GetOffset() const
{
  return (uint8(eTotalPosMaskX & m_iPosX) + 
          (uint8(eTotalPosMaskY & m_iPosY) << eTotalPosBitsX)+ 
          (uint8(eTotalPosMaskZ & m_iPosZ) << 22) );     // 22 = (eTotalPosBitsX + eTotalPosBitsY)
} // GetOffset	()	


/**
 * Returns the offset that corresponds to the current voxel position
 * @param position in X
 * @param position in Y
 * @param position in Z
 * @return offset
 */
template <class VoxelType>
const uint8 vxBlockVolumeRayIterator<VoxelType>::GetOffset(const int4 iPosX,                                                      
                                                          const int4 iPosY, 
                                                          const int4 iPosZ) const
{
  return (uint8(eTotalPosMaskX & iPosX) + 
          (uint8(eTotalPosMaskY & iPosY) << eTotalPosBitsX)+ 
          (uint8(eTotalPosMaskZ & iPosZ) << 22) );   // 22 = (eTotalPosBitsX + eTotalPosBitsY)
} // GetOffset()


#undef Trilin
#undef FILE_REVISION


// $Log: vxBlockVolumeRayIterator.inl,v $
// Revision 1.5.10.4  2010/08/03 21:01:28  dongqing
// another typo on constants
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.5.10.3  2010/08/03 15:29:37  dongqing
// fix a typo in constants
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.5.10.2  2010/07/19 17:50:16  dongqing
// fix typo in constants
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.5.10.1  2010/07/16 20:31:28  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.5  2006/03/29 19:56:19  geconomos
// corrected float initialization  in UpdateInternalIntegerPosition() for 32 and 64 bit builds (very strange)
//
// Revision 1.4  2006/02/09 19:42:34  geconomos
// corrected UpdateInternalIntegerPosition() for 32-bit builds
//
// Revision 1.3  2006/02/09 14:03:41  geconomos
// 64-bit conditionals
//
// Revision 1.2  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.59  2004/02/03 12:34:01  michael
// fixed bug in IsInsideVolume() function since it did not use the real index-1 size
// but always check for one more voxel; -1 for size test was missing!!!
//
// Revision 1.58  2003/09/08 16:32:47  frank
// Added functions to better control space leaping
//
// Revision 1.57  2003/06/25 18:35:40  frank
// Added IsInsideVolume
//
// Revision 1.56  2003/05/03 16:18:20  michael
// completed code review
//
// Revision 1.55  2003/05/02 14:36:56  dongqing
// code walk through
//
// Revision 1.54  2003/03/18 13:24:28  michael
// fixed bug in GetNearestSample (instead of '+' use '*'), related to issue #2909
//
// Revision 1.53  2003/01/22 21:34:59  ingmar
// this iterator is now only using a RaySegment and a Vector4Df increment to determine how to traverse the ray
//
// Revision 1.52  2002/11/26 18:08:21  ingmar
// now using Vector4d<float4> to keep the current position as well as offset
//
// Revision 1.51  2002/11/20 23:30:54  frank
// Added some new ray sampling options for label volumes
//
// Revision 1.50  2002/10/23 16:02:28  ingmar
// code beautify
//
// Revision 1.49  2002/09/25 23:21:57  ingmar
// changes from intel  tuning visit
//
// Revision 1.48  2002/09/25 23:12:35  ingmar
// changes from intel  tuning visit
//
// Revision 1.47  2002/08/02 21:09:53  ingmar
// coding standard compliant variable naming
//
// Revision 1.46  2002/07/12 23:24:36  ingmar
// GetSampleInt(): round instead of truncate and float4 instead of float8
//
// Revision 1.45  2002/07/12 23:18:15  ingmar
// avoided calling ftoa() for samples -> 3.8fps
//
// Revision 1.44  2002/07/09 20:58:23  ingmar
// added 4^3 intensity histogram and adjusted rendering to
// first spaceleap in 16^3 SLD and than to skipp over 4^3 blocks not "seen" by the transfer function
//
// Revision 1.43  2002/07/09 17:12:04  ingmar
// changed Sld block width to 16
// added GetDistanceToEndOfSmallBlock()  for blocksize 4^3
//
// Revision 1.42  2002/06/18 15:46:52  ingmar
// removed normalization for gradient computation to allow anisotropic correction in the client code
//
// Revision 1.41  2002/06/14 22:50:15  soeren
// uuups
//
// Revision 1.40  2002/06/14 22:38:31  ingmar
// improved the weights of the sobel filter
//
// Revision 1.39  2002/06/14 22:20:50  ingmar
// finished Sobel Gradient Filter implementation
// added several Peek functions needed for 64 neighborhood of sobel filter
//
// Revision 1.38  2002/06/14 21:12:05  soeren
// fixed "internal compiler error" in GetDistanceToBlock
//
// Revision 1.37  2002/06/14 14:42:41  soeren
// disabled ingmars function
//
// Revision 1.36  2002/06/14 14:42:10  soeren
// disabled ingmars function
//
// Revision 1.35  2002/06/13 21:02:45  ingmar
// added GetDistanceToEndOfBlock()
//
// Revision 1.34  2002/06/07 23:34:44  soeren
// beutified central difference gradient
//
// Revision 1.33  2002/06/07 21:41:54  soeren
// worked on sobel
//
// Revision 1.32  2002/06/06 20:49:17  soeren
// added inital version of Sobel gradient
//
// Revision 1.31  2002/06/06 20:45:03  soeren
// Just beautyfied
//
// Revision 1.30  2002/06/06 20:35:04  soeren
// implemented GetSampleGradientImmediateDifference
//
// Revision 1.29  2002/06/06 15:29:18  soeren
// implemented GetSampleGradientImmediateDifference
//
// Revision 1.28  2002/06/05 14:53:48  soeren
// and another 10 percent faster, changing VoxelType to hardcoded int4
//
// Revision 1.27  2002/06/05 14:35:03  soeren
// renamed peeks
//
// Revision 1.26  2002/06/04 15:51:40  soeren
// improved GetSampleGradientCentralDifference
//
// Revision 1.25  2002/06/04 15:01:56  soeren
// renamed GetSampleGradient to GetSampleCentraldifferencegradient
//
// Revision 1.24  2002/06/04 13:53:36  soeren
// beautified
//
// Revision 1.23  2002/06/04 13:32:57  soeren
// optimized GetSample
//
// Revision 1.22  2002/06/04 13:08:36  soeren
// removed old gradient computation
//
// Revision 1.21  2002/06/03 22:56:23  uday
// Updated Peek functions.
//
// Revision 1.20  2002/06/03 14:04:48  soeren
// removed obsolute Next and prev else
//
// Revision 1.19  2002/06/02 18:59:54  soeren
// fixed new gradientTrilinear function
//
// Revision 1.18  2002/06/02 18:20:26  soeren
// removed obsolute PeekNext and PeekPrev
//
// Revision 1.17  2002/06/02 17:50:58  soeren
// added more accurate GetGradientTrilinear function and the 30 Peek functions for that function
//
// Revision 1.16  2002/06/02 17:07:19  soeren
// initial version of GetGradientTrilinear()
//
// Revision 1.15  2002/06/02 15:30:52  soeren
// Moved Constructor to C file
//
// Revision 1.14  2002/06/02 15:21:55  soeren
// Speed up, through moving else of UpdateInternalIntegerPosition to C file:-)
//
// Revision 1.13  2002/06/02 15:15:57  soeren
// Worked on CleanUp.
//
// Revision 1.12  2002/06/02 14:38:21  soeren
// removed non raw functions and renamed raw functions to non raw,
// since there is nothing unsave now
//
// Revision 1.11  2002/06/02 14:03:02  soeren
// fixed some peek bugs
//
// Revision 1.10  2002/05/24 20:08:24  ingmar
// towards coding standard compliance
//
// Revision 1.9  2002/05/23 23:14:11  ingmar
// improved trilin call parameters
//
// Revision 1.8  2002/05/07 22:40:30  ingmar
// changed to coding standard compliant enums
//
// Revision 1.7  2002/04/09 23:46:40  ingmar
// removed obsulete member direction vector
//
// Revision 1.6  2002/04/08 23:58:21  ingmar
// back to box metric
//
// Revision 1.5  2002/04/08 21:48:19  ingmar
// new spaceleap computation
//
// Revision 1.4  2002/04/04 22:09:23  ingmar
// fixed walking into negative positions
//
// Revision 1.3  2002/04/04 21:24:07  ingmar
// fixed GetOffsetL2Vol()
//
// Revision 1.2  2002/04/04 19:08:56  ingmar
// fixed some bugs in Peek and code beautifiy
//
// Revision 1.1  2002/04/04 14:42:52  ingmar
// initial version
//
// Revision 1.6  2002/04/02 20:05:07  ingmar
// now using bounding box of contributing voxels to test for end of ray
//
// Revision 1.5  2002/03/20 16:14:35  ingmar
// Instead of fast access pointer to the beginning of the data of a block, we are using now the direct
// access function of the block.
//
// Revision 1.4  2002/03/19 20:11:50  ingmar
// faster SetPos() through checking if we are inside the current block
//
// Revision 1.3  2002/03/19 17:01:02  ingmar
// included all voliter functionality to allow compiler to make better optimaizations
//
// Revision 1.2  2002/03/19 02:01:14  ingmar
// updated Rayiter
//
// Revision 1.1  2002/03/13 20:18:05  ingmar
// intial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeRayIterator.inl,v 1.5.10.4 2010/08/03 21:01:28 dongqing Exp $
// $Id: vxBlockVolumeRayIterator.inl,v 1.5.10.4 2010/08/03 21:01:28 dongqing Exp $
