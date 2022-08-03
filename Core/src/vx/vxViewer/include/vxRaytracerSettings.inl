// $Id: vxRaytracerSettings.inl,v 1.3 2007/03/02 19:17:42 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owners: George Economos (mailto:george@viatronix.com)


/**
 * Helper function to pre-integrate the RGBA between two pixels
 * @param iIndexEnd          end index
 * @param fAlphaCorrExponent unknown
 * @param fMax               maxium alpha value
 * @param fColorScaleTo8bit  scale factor to get color to [0...255]
 * @param pfColormap         incoming colormap (RGBA)
 * @param pfResult           outgoing results (RGBAW)
 */
inline void FileScope_PreIntegrate1D(const int4 iIndexEnd, 
                                     const float4 fAlphaCorrExponent, 
                                     const float4 fMax,
                                     const float4 fColorScaleTo8bit,
                                     const float4 * pfColormap, 
                                     float4 * pfResult)
{
  float4 fARacc(0);
  float4 fAGacc(0);
  float4 fABacc(0);
  float4 fTacc(1);
  float4 fA(0);
  float4 fAccWeight1(0);
  float4 fAccWeight2(0);

  for (int4 k(0); ((fTacc >= 0.05) && (k<iIndexEnd)); k++)
  {
    float4 * pfColormapEntry(const_cast<float *>(pfColormap + k*4) );
    fA = *(pfColormapEntry + 3)/fMax;

    if (fA > 0.0)
    {
      const float4 fAlphaCorr(1 - pow(1-fA, fAlphaCorrExponent));
      const float4 fTA(fTacc * fAlphaCorr);
      fARacc += fTA * fColorScaleTo8bit * *(pfColormapEntry++);
      fAGacc += fTA * fColorScaleTo8bit * *(pfColormapEntry++);
      fABacc += fTA * fColorScaleTo8bit * *(pfColormapEntry);
      fTacc *= (1 - fAlphaCorr);

      // pre-integrate the gradient location weight for each interval
      float4 fWeight(float4(k)/float4(iIndexEnd));
      fAccWeight1 += (1.0f - fWeight) * fAlphaCorr; 
      fAccWeight2 +=         fWeight  * fAlphaCorr; 
    }
  }
  *(pfResult++) = fARacc; 
  *(pfResult++) = fAGacc; 
  *(pfResult++) = fABacc;     
  *(pfResult++) = 1 - fTacc;

  float4 fAccWeightSum(fAccWeight1 + fAccWeight2);

  if (fAccWeightSum > 0)
  {
    fAccWeight1 /= fAccWeightSum;
    fAccWeight2 /= fAccWeightSum;
  }
  *(pfResult) = 0.5F; //2 * fAccWeight1;
} // FileScope_PreIntegrate1D()


/**
 * Helper function to pre-integrate the RGBA between two pixels
 *
 * @param iIndexStart        the start index
 * @param iIndexEnd          the end index
 * @param iIndexInc          unknown
 * @param fAlphaCorrExponent unknown
 * @param p8BitTFunc         the 8-bit 1D transfer function
 * @param pfTFunc2D          the 8-bit 2D transfer function
 */
inline void FileScope_PreIntegrate2D8bit(const int4 iIndexStart, 
                                         const int4 iIndexEnd, 
                                         const int4 iIndexInc, 
                                         const float4 fAlphaCorrExponent, 
                                         const float4 * pColormap, 
                                         float4 * pfTarget)
{
  float4 fARacc(0);
  float4 fAGacc(0);
  float4 fABacc(0);
  float4 fTacc(1);
  float4 fA(0);
  float4 fAccWeight1(0);
  float4 fAccWeight2(0);

  for (int4 k(iIndexStart); ((fTacc >= 0.05) && ((iIndexInc > 0) ? (k<iIndexEnd) : (k>iIndexEnd))); k += iIndexInc)
  {
    // pre-integrate color and alpha for each interval
    float4 * pfTFunc1D(const_cast<float *>(pColormap + k*4) );
    fA = *(pfTFunc1D + 3);

    if (fA > 0.0)
    {
      const float4 fAlphaCorr(1 - pow(1-fA, fAlphaCorrExponent));
      const float4 fTA(fTacc * fAlphaCorr);
      fARacc += fTA * *(pfTFunc1D++);
      fAGacc += fTA * *(pfTFunc1D++);
      fABacc += fTA * *(pfTFunc1D);
      fTacc *= (1 - fAlphaCorr);

      // pre-integrate the gradient location weight for each interval
      float4 fWeight(float4(iIndexStart - k)/float4(iIndexStart - iIndexEnd));
      fAccWeight1 += (1.0f - fWeight) * fAlphaCorr; 
      fAccWeight2 +=         fWeight  * fAlphaCorr; 
    }
  }
  *(pfTarget++) = fARacc; 
  *(pfTarget++) = fAGacc; 
  *(pfTarget++) = fABacc;     
  *(pfTarget++) = 1 - fTacc;

  float4 fAccWeightSum(fAccWeight1 + fAccWeight2);

  if (fAccWeightSum > 0)
  {
    fAccWeight1 /= fAccWeightSum;
    fAccWeight2 /= fAccWeightSum;
  }
  *(pfTarget) = 2 * fAccWeight1;
 } // FileScope_PreIntegrate2D8bit()


// $Log: vxRaytracerSettings.inl,v $
// Revision 1.3  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.2  2006/08/03 14:15:22  geconomos
// cosmetics
//
// Revision 1.1  2006/06/20 20:05:48  geconomos
// refactored vxSoftwareRendererCpu3D
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRaytracerSettings.inl,v 1.3 2007/03/02 19:17:42 geconomos Exp $
// $Id: vxRaytracerSettings.inl,v 1.3 2007/03/02 19:17:42 geconomos Exp $
